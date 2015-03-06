#ifndef __VT_H__
#define __VT_H__

#include "types.h"

#define	WIDTH		320
#define	HEIGHT		240
#define	LINES		40
#define	COLUMNS		80

#define	MODE_VT52	0
#define	MODE_VT102	1
#define	MODE_VT220	2
#define	MODE_VT340	3
#define	MODE_TEK4014	4

#define	NUL		0x00
#define	SOH		0x01
#define	STX		0x02
#define	ETX		0x03
#define	EOT		0x04
#define	ENQ		0x05
#define	ACK		0x06
#define	BEL		0x07
#define	BS		0x08
#define	HT		0x09
#define	LF		0x0A
#define	VT		0x0B
#define	FF		0x0C
#define	CR		0x0D
#define	SO		0x0E
#define	SI		0x0F
#define	DLE		0x10
#define	DC1		0x11
#define	DC2		0x12
#define	DC3		0x13
#define	DC4		0x14
#define	NAK		0x15
#define	SYN		0x16
#define	ETB		0x17
#define	CAN		0x18
#define	EM		0x19
#define	SUB		0x1A
#define	ESC		0x1B
#define	FS		0x1C
#define	GS		0x1D
#define	RS		0x1E
#define	US		0x1F
#define	DEL		0x7F
#define	IND		0x84
#define	NEL		0x85
#define	HTS		0x88
#define	RI		0x8D
#define	SS2		0x8E
#define	SS3		0x8F
#define	DCS		0x90
#define	CSI		0x9B
#define	ST		0x9C

// INITIALIZATION
void	VT_init();
void	VT_reset();

// GLOBAL VT COMMANDS
void	carriage_return();
void	linefeed();
void	reverse_linefeed();

void	cursor_up();
void	cursor_down();
void	cursor_up_scroll();
void	cursor_down_scroll();
void	cursor_left();
void	cursor_right();
void	cursor_home();
void	cursor_tabstop();
void	cursor_goto(const int x, const int y);

void	cursor_down_n(const u8 n);
void	cursor_up_n(const u8 n);
void	cursor_left_n(const u8 n);
void	cursor_right_n(const u8 n);

void	scroll_up();
void	scroll_down();

void	set_tabstop();
void	clear_tabstop();
void	clear_all_tabstops();

void	erase_cursor_to_eol();
void	erase_cursor_to_end();
void	erase_characters(const u8 n);
void	erase_cursor_line();
void	erase_cursor_to_bol();
void	erase_cursor_to_begin();
void	erase_screen();

// SPECIFIC VT
void	VT52_reset();
void	VT220_reset();
u8	VT52_process(char c);
u8	VT220_process(unsigned char c);

// RS232 COMMUNICATIONS
void	rx_char(char c);
void	tx_char(char);
void	tx_dec(u16 dec);
void	tx_str(const char* s);

// FONTS
#define FONT_WIDTH	4
#define	FONT_HEIGHT	6
void	drawchar_mini(const char c, const u16 fg, const u16 bg, const u16 x,
		const u16 y);

// SETTINGS
extern u16 fgcolor;
extern u16 bgcolor;
extern u8 auto_wrap;
extern u8 cursorX;
extern u8 cursorY;
extern u8 mode;

// COLORS
#define	DEFAULT_FG	7
#define	DEFAULT_BG	0
extern const u16 colors[16];

u16	get_color(const u8 idx);

#endif
