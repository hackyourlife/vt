#include <ctype.h>
#include "vt.h"
#include "vt220.h"

#define	STATE_TEXT	0
#define	STATE_ESC	1
#define	STATE_CSI	2
#define	STATE_DECPRIV	3
#define	STATE_HASH	4
#define	STATE_DCS	5
#define	STATE_DCA1	6
#define	STATE_DCA2	7

#define	MAX_PARAMETERS	8
#define	MAX(X, Y)	((X) > (Y) ? (X) : (Y))

#define _BV(x)		(1 << (x))

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#else
#define pgm_read_byte(addr)	(*(const u8*)(addr))
#define PROGMEM
#endif

#define	DEFAULT_COLOR	0x70

static const char csi_7bit[] PROGMEM = "\e[";
static const char csi_8bit[] PROGMEM = "\x9b";

static const char primary_da_vt220[] PROGMEM = "?62;1;2;6;7;8;9c";
static const char primary_da_vt102[] PROGMEM = "?6c";
static const char* primary_da = primary_da_vt220;

static const char dsr_response[] PROGMEM = "0n";
static const char dsr_no_printer[] PROGMEM = "?13n";
static const char dsr_keyboard_language[] PROGMEM = "?27;0n"; // Unknown

static u8 state = STATE_TEXT;
static u16 parameters[MAX_PARAMETERS];
static u8 parameter_id = 0;
static u8 gx_color = DEFAULT_COLOR;
static u8 gx_flags = 0x00;

static u8 gx_color_stored = DEFAULT_COLOR;
static u8 gx_flags_stored = 0x00;
static u8 cursorX_stored = 0;
static u8 cursorY_stored = 0;
static u8 auto_wrap_stored = 1;
static u8 flags = _BV(MODE_7BIT);

// VT52 compatibility
static u8 px;
static u8 py;

static u8 VT220_process52(char c);
static void store_cursor();
static void restore_cursor();
static void reverse_video(u8 enable);
static void select_graphic_rendition(u16 code);
static void set_gx_mode();
static void tx_csi();
static void reset_sgr();

void VT220_reset()
{
	reset_sgr();
	gx_color_stored = DEFAULT_COLOR;
	gx_flags_stored = 0x00;
	cursorX_stored = 0;
	cursorY_stored = 0;
	auto_wrap_stored = 1;
	flags = _BV(MODE_7BIT);
	auto_wrap = 1;
	set_gx_mode();
	cursor_home();
	erase_screen();
}

static void VT220_clear_parameters()
{
	u8 i;
	for(i = 0; i < MAX_PARAMETERS; i++)
		parameters[i] = 0;
	parameter_id = 0;
	state = STATE_CSI;
}

u8 VT220_process(unsigned char c)
{
	u8 i;
	if(flags & _BV(MODE_VT52COMPAT))
		return VT220_process52(c);
	switch(state) {
		case STATE_TEXT:
			switch(c) {
				case NUL:
					break;
				case BEL:
					break;
				case BS:
					cursor_left();
					break;
				case HT:
					cursor_tabstop();
					break;
				case LF:
				case VT:
				case FF:
					linefeed();
					break;
				case CR:
					carriage_return();
					break;
				case ESC:
					VT220_clear_parameters();
					state = STATE_ESC;
					break;
				case DEL:
					break;
				case IND:
					linefeed();
					break;
				case NEL:
					linefeed();
					carriage_return();
					break;
				case HTS:
					set_tabstop();
					break;
				case RI:
					reverse_linefeed();
					break;
				case DCS:
					break;
				case CSI:
					VT220_clear_parameters();
					state = STATE_CSI;
					break;
				default:
					return 1;
			}
			return 0;
		case STATE_ESC:
			switch(c) {
				case CAN: // Cancel
				case SUB: // Substitute (FIXME: show glyph)
					state = STATE_TEXT;
					return 0;
				case '[': // CSI
					state = STATE_CSI;
					break;
				case 'N': // SS2
				case 'O': // SS3
				case 'D': // IND
					linefeed();
					state = STATE_TEXT;
					break;
				case 'M': // RI
					reverse_linefeed();
					state = STATE_TEXT;
					break;
				case DECSC:
					store_cursor();
					state = STATE_TEXT;
					break;
				case DECRC:
					restore_cursor();
					state = STATE_TEXT;
					break;
				case 'E':
					linefeed();
					carriage_return();
					state = STATE_TEXT;
					break;
				case 'H':
					set_tabstop();
					state = STATE_TEXT;
					break;
				case 'P':
					state = STATE_DCS;
					return 0;

				case '#':
					state = STATE_HASH;
					break;
				case 'Z':
					tx_csi();
					tx_str(primary_da);
					state = STATE_TEXT;
					break;
				case 'c': // Hard Terminal Reset (RIS)
					VT220_reset();
					state = STATE_TEXT;
					break;
				case ESC:
					state = STATE_ESC;
					break;
				default: // unknown sequence
					break;
			}
			return 0;
		case STATE_CSI:
			if(isdigit(c)) {
				parameters[parameter_id] *= 10;
				parameters[parameter_id] += c - '0';
				return 0;
			}
			switch(c) {
				case CAN: // Cancel
				case SUB: // Substitute (FIXME: show glyph)
					state = STATE_TEXT;
					return 0;
				case '?':
					state = STATE_DECPRIV;
					return 0;
				case ';':
					parameter_id++;
					if(parameter_id >= MAX_PARAMETERS)
						parameter_id = 0;
					return 0;
				case CUU:
					cursor_up_n(MAX(parameters[0], 1));
					break;
				case CUD:
					cursor_down_n(MAX(parameters[0], 1));
					break;
				case CUF:
					cursor_right_n(MAX(parameters[0], 1));
					break;
				case CUB:
					cursor_left_n(MAX(parameters[0], 1));
					break;
				// TODO: DECOM
				case CUP:
				case HVP:
					cursor_goto(MAX(parameters[1], 1),
							MAX(parameters[0], 1));
					break;
				case 'g':
					if(parameters[0] == 0)
						clear_tabstop();
					else if(parameters[0] == 3)
						clear_all_tabstops();
					break;
				case 'm':
					parameter_id++;
					if(parameter_id == MAX_PARAMETERS)
						parameter_id--;
					for(i = 0; i < parameter_id; i++)
						select_graphic_rendition(
								parameters[i]);
					break;
				case 'L': // Insert Line (IL)
				case 'M': // Delete Line (DL)
				case '@': // Insert Characters (ICH)
				case 'P': // Delete Character (DCH)
					break;
				case 'X': // Erase Character (ECH)
					erase_characters(MAX(parameters[0], 1));
					break;
				case 'K': // Erase in Line (EL)
					if(parameters[0] == 0)
						erase_cursor_to_eol();
					else if(parameters[0] == 1)
						erase_cursor_to_bol();
					else if(parameters[0] == 2)
						erase_cursor_line();
					break;
				case 'J': // Erase in Display (ED)
					if(parameters[0] == 0)
						erase_cursor_to_end();
					else if(parameters[0] == 1)
						erase_cursor_to_begin();
					else if(parameters[0] == 2)
						erase_screen();
				case 'r': // Set Top and Bottom Margins (DECSTBM)
					break;
				case 'c': // Primary DA
					tx_csi();
					tx_str(primary_da);
					break;
				case 'n': // DSR
					switch(parameters[0]) {
						case 5: // Operating Status
							tx_csi();
							tx_str(dsr_response);
							break;
						case 6: // CPR
							tx_csi();
							tx_dec(cursorY + 1);
							tx_char(';');
							tx_dec(cursorX + 1);
							tx_char('R');
							break;
					}
					break;
				case ESC:
					state = STATE_ESC;
					return 0;
				default: // unknown sequence
					break;
			}
			state = STATE_TEXT;
			return 0;
		case STATE_DECPRIV:
			if(isdigit(c)) {
				parameters[0] *= 10;
				parameters[0] += c - '0';
				return 0;
			}
			switch(c) {
				case CAN: // Cancel
				case SUB: // Substitute (FIXME: show glyph)
					state = STATE_TEXT;
					return 0;
				case 'h': // SM
					switch(parameters[0]) {
						case DECSCNM:
							reverse_video(1);
							break;
						case DECAWM:
							auto_wrap = 1;
							break;
					}
					break;
				case 'l': // RM
					switch(parameters[0]) {
						case DECANM:
							flags |= _BV(MODE_VT52COMPAT);
							break;
						case DECSCNM:
							reverse_video(0);
							break;
						case DECAWM:
							auto_wrap = 0;
							break;
					}
					break;
				case 'K': // Selective Erase In Line (DECSEL)
				case 'J': // Selective Erase In Display (DECSED)
				case 'n': // DSR
					switch(parameters[0]) {
						case 15:
							tx_csi();
							tx_str(dsr_no_printer);
							break;
						case 26:
							tx_csi();
							tx_str(dsr_keyboard_language);
							break;
					}
					break;
				case ESC:
					state = STATE_ESC;
					return 0;
			}
			state = STATE_TEXT;
			return 0;
		case STATE_HASH:
			state = STATE_TEXT;
			break;
		case STATE_DCS: // TODO: implement correctly
			if(isdigit(c)) {
				parameters[parameter_id] *= 10;
				parameters[parameter_id] += c - '0';
				return 0;
			} else if(c == ';') {
				parameter_id++;
				if(parameter_id >= MAX_PARAMETERS)
					parameter_id = 0;
				return 0;
			}
			switch(c) {
				case CAN: // Cancel
				case SUB: // Substitute (FIXME: show glyph)
				case ESC:
				case ST:
					state = STATE_TEXT;
					break;
			}
			return 0;
	}
	return 0;
}

static u8 VT220_process52(char c)
{
	switch(state) {
		case STATE_TEXT:
			switch(c) {
				case BEL:
					return 0;
					break;
				case HT:
					cursor_tabstop();
					return 0;
				case LF:
					linefeed();
					return 0;
				case CR:
					carriage_return();
					return 0;
				case ESC:
					state = STATE_ESC;
					return 0;
			}
			return 1;
			break;
		case STATE_ESC:
			switch(c) {
				case CAN: // Cancel
				case SUB: // Substitute (FIXME: show glyph)
					state = STATE_TEXT;
					return 0;
				case 'A':
					cursor_up();
					break;
				case 'B':
					cursor_down();
					break;
				case 'C':
					cursor_right();
					break;
				case 'D':
					cursor_left();
					break;
				case 'H':
					cursor_home();
					break;
				case 'I':
					reverse_linefeed();
					break;
				case 'K':
					erase_cursor_to_eol();
					break;
				case 'J':
					erase_cursor_to_end();
					break;
				case 'Y':
					state = STATE_DCA1;
					px = 0;
					py = 0;
					return 0;
				case 'Z':
					tx_char('K');
					break;
				case '<':
					flags &= ~_BV(MODE_VT52COMPAT);
					break;
				default:
					state = STATE_TEXT;
					return 1;
			}
			state = STATE_TEXT;
			return 0;
		case STATE_DCA1:
			if(c < 0x20) {
				state = STATE_TEXT;
				return 0;
			}
			state = STATE_DCA2;
			py = c - 0x1F;
			return 0;
		case STATE_DCA2:
			state = STATE_TEXT;
			if(c < 0x20) {
				return 0;
			}
			px = c - 0x1F;
			cursor_goto(px, py);
			return 0;
	}
	return 1;
}

static void reverse_video(u8 enable)
{
	if(enable)
		gx_flags |= _BV(DISPLAY_REVERSE);
	else
		gx_flags &= ~_BV(DISPLAY_REVERSE);
	set_gx_mode();
}

static void store_cursor()
{
	gx_color_stored = gx_color;
	gx_flags_stored = gx_flags;
	cursorX_stored = cursorX;
	cursorY_stored = cursorY;
	auto_wrap_stored = auto_wrap;
}

static void restore_cursor()
{
	gx_color = gx_color_stored;
	gx_flags = gx_flags_stored;
	cursorX = cursorX_stored;
	cursorY = cursorY_stored;
	auto_wrap = auto_wrap_stored;
}

static void select_graphic_rendition(u16 code)
{
	switch(code) {
		case 0:
			gx_flags = 0x00;
			gx_color = DEFAULT_COLOR;
			break;
		case 1:
			gx_flags |= _BV(DISPLAY_BOLD);
			break;
		case 4:
			gx_flags |= _BV(DISPLAY_UNDERSCORED);
			break;
		case 5:
			gx_flags |= _BV(DISPLAY_BLINKING);
			break;
		case 7:
			gx_flags |= _BV(DISPLAY_REVERSE);
			break;
		case 22:
			gx_flags &= ~_BV(DISPLAY_BOLD);
			break;
		case 24:
			gx_flags &= ~_BV(DISPLAY_UNDERSCORED);
			break;
		case 25:
			gx_flags &= ~_BV(DISPLAY_BLINKING);
			break;
		case 27:
			gx_flags &= ~_BV(DISPLAY_REVERSE);
			break;
	}
	set_gx_mode();
}

static void set_gx_mode()
{
	u8 bold = (gx_flags & _BV(DISPLAY_BOLD)) ? 8 : 0;
	u8 reverse = gx_flags & _BV(DISPLAY_REVERSE);
	u8 fg = ((gx_color >> 4) & 0x07) + bold;
	u8 bg = (gx_color & 0x07) + bold;
	fgcolor = get_color(reverse ? bg : fg);
	bgcolor = get_color(reverse ? fg : bg);
}

static void tx_csi()
{
	if(flags & _BV(MODE_7BIT)) {
		tx_char(0x1B);
		tx_char('[');
	} else
		tx_char(CSI);
}

static void reset_sgr()
{
	gx_color = DEFAULT_COLOR;
	gx_flags = 0x00;
}
