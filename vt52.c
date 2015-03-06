#include "gx.h"
#include "vt.h"

#define	STATE_TEXT	0
#define	STATE_ESC	1
#define	STATE_DCA1	2
#define	STATE_DCA2	3

static int state = STATE_TEXT;
static int px;
static int py;

void VT52_reset()
{
	fgcolor = RGB565(0x00, 0xF6, 0xC0);
	bgcolor = 0x0000;
	cursor_home();
	erase_screen();
}

u8 VT52_process(char c)
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
					break;
				case 'Z':
					tx_char('K');
					break;
				default:
					state = STATE_TEXT;
					return 1;
			}
			state = STATE_TEXT;
			return 0;
			break;
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
