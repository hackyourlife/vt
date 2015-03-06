#include "gx.h"
#include "vt.h"

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#else
#define pgm_read_byte(addr)	(*(const u8*)(addr))
#define pgm_read_word(addr)	(*(const u16*)(addr))
#define PROGMEM
#endif

#define	MAX(X, Y)	((X) > (Y) ? (X) : (Y))

u8 mode = MODE_VT220;

u8 cursorX = 0;
u8 cursorY = 0;
u16 fgcolor = 0xFFFF;
u16 bgcolor = 0x0000;
u8 auto_wrap = 1;

static u8 tabstops[COLUMNS];

const u16 colors[16] PROGMEM = {
	RGB565(0x00, 0x00, 0x00),	// BLACK
	RGB565(0xCD, 0x00, 0x00),	// RED
	RGB565(0x00, 0xCD, 0x00),	// GREEN
	RGB565(0xCD, 0xCD, 0x00),	// YELLOW
	RGB565(0x00, 0x00, 0xEE),	// BLUE
	RGB565(0xCD, 0x00, 0xCD),	// MAGENTA
	RGB565(0x00, 0xCD, 0xCD),	// CYAN
	//RGB565(0xE5, 0xE5, 0xE5),	// GRAY
	RGB565(0xA0, 0xA0, 0xA0),	// GRAY
	RGB565(0x1F, 0x1F, 0x1F),	// DARK GRAY
	RGB565(0xFF, 0x00, 0x00),	// BRIGHT RED
	RGB565(0x00, 0xFF, 0x00),	// BRIGHT GREEN
	RGB565(0xFF, 0xFF, 0x00),	// BRIGHT YELLOW
	RGB565(0x5C, 0x5C, 0xFF),	// BRIGHT BLUE
	RGB565(0xFF, 0x00, 0xFF),	// BRIGHT MAGENTA
	RGB565(0x00, 0xFF, 0xFF),	// BRIGHT CYAN
	RGB565(0xFF, 0xFF, 0xFF)	// WHITE
};

void VT_init()
{
	VT_reset();
}

void VT_reset()
{
	u8 i;
	for(i = 0; i < COLUMNS; i++)
		tabstops[i] = i % 8 == 7;
	switch(mode) {
		case MODE_VT52:
			VT52_reset();
			break;
		case MODE_VT220:
			VT220_reset();
			break;
	}
}

void drawchar(char c)
{
	if(cursorX == COLUMNS) {
		if(auto_wrap) {
			carriage_return();
			linefeed();
		} else
			cursorX--;
	}
	u16 px = cursorX * FONT_WIDTH;
	u16 py = cursorY * FONT_HEIGHT;
	drawchar_mini(c, fgcolor, bgcolor, px, py);
	cursorX++;
	if(cursorX > COLUMNS)
		cursorX = COLUMNS;
}

void rx_char(char c)
{
	switch(mode) {
		case MODE_VT52:
			if(VT52_process(c))
				drawchar(c);
			break;
		case MODE_VT220:
			if(VT220_process(c))
				drawchar(c);
			break;
	}
}

u16 get_color(const u8 idx)
{
	return pgm_read_word(&colors[idx & 0x0F]);
}

void set_tabstop()
{
	tabstops[cursorX] = 1;
}

void clear_tabstop()
{
	tabstops[cursorX] = 0;
}

void clear_all_tabstops()
{
	u8 i;
	for(i = 0; i < COLUMNS; i++)
		tabstops[i] = 0;
}

void cursor_tabstop()
{
	while(++cursorX < COLUMNS)
		if(tabstops[cursorX])
			break;
	if(cursorX == COLUMNS)
		cursorX--;
}

void cursor_down()
{
	cursorY++;
	if(cursorY == LINES)
		cursorY--;
}

void cursor_up()
{
	if(cursorY > 0)
		cursorY--;
}

void cursor_left()
{
	if(cursorX > 0)
		cursorX--;
}

void cursor_right()
{
	cursorX++;
	if(cursorX == COLUMNS)
		cursorX--;
}

void cursor_down_n(const u8 n)
{
	cursorY += n;
	if(cursorY >= LINES)
		cursorY = LINES - 1;
}

void cursor_up_n(const u8 n)
{
	u8 _n = n;
	while(_n-- && (cursorY > 0))
		cursorY--;
}

void cursor_left_n(const u8 n)
{
	u8 _n = n;
	while(_n-- && (cursorX > 0))
		cursorX--;
}

void cursor_right_n(const u8 n)
{
	cursorX += n;
	if(cursorX >= COLUMNS)
		cursorX = COLUMNS - 1;
}

void cursor_goto(const int x, const int y)
{
	cursorX = x - 1;
	cursorY = y - 1;
	if(cursorX > COLUMNS)
		cursorX = COLUMNS - 1;
	if(cursorY > LINES)
		cursorY = LINES - 1;
	if(cursorX < 0)
		cursorX = 0;
	if(cursorY < 0)
		cursorY = 0;
}

void cursor_home()
{
	cursorX = 0;
	cursorY = 0;
}

void carriage_return()
{
	cursorX = 0;
}

void linefeed()
{
	cursorY++;
	if(cursorY == LINES) {
		cursorY--;
		scroll_up();
	}
}

void reverse_linefeed()
{
	if(cursorY > 0)
		cursorY--;
	else
		scroll_down();
}

void erase_cursor_to_eol()
{
	u16 px = cursorX * FONT_WIDTH;
	u16 py = cursorY * FONT_HEIGHT;
	GX_fillRect(px, py, WIDTH - px, FONT_HEIGHT, bgcolor);
}

void erase_cursor_to_end()
{
	erase_cursor_to_eol();
	if(cursorY == (LINES - 1))
		return;
	u16 py = (cursorY + 1) * FONT_HEIGHT;
	GX_fillRect(0, py, WIDTH, HEIGHT - py, bgcolor);
}

void erase_line(const u8 n)
{
	u16 y = n * FONT_HEIGHT;
	GX_fillRect(0, y, WIDTH, FONT_HEIGHT, bgcolor);
}

void erase_cursor_line()
{
	erase_line(cursorX);
}

void erase_cursor_to_bol()
{
	u16 px = cursorX * FONT_WIDTH;
	u16 py = cursorY * FONT_HEIGHT;
	GX_fillRect(0, py, px, FONT_HEIGHT, bgcolor);
}

void erase_cursor_to_begin()
{
	erase_cursor_to_bol();
	if(cursorY == 0)
		return;
	u16 py = (cursorY - 1) * FONT_HEIGHT;
	GX_fillRect(0, 0, WIDTH, py, bgcolor);
}

void erase_screen()
{
	GX_fillScreen(bgcolor);
}

void erase_characters(const u8 n)
{
	u16 x = cursorX * FONT_WIDTH;
	u16 y = cursorY * FONT_HEIGHT;
	u16 w = n * FONT_WIDTH;
	if(x + w > WIDTH)
		w = WIDTH - x;
	GX_fillRect(x, y, w, FONT_HEIGHT, bgcolor);
}

void scroll_up()
{
	//ILI9340_scroll_up(FONT_HEIGHT, bgcolor);
}

void scroll_down()
{
}

void tx_dec(u16 dec)
{
	u8 i = 4;
	u8 chars[5] = {0, 0, 0, 0, 0};
	chars[0] = dec / 10000;
	dec %= 10000;
	chars[1] = dec / 1000;
	dec %= 1000;
	chars[2] = dec / 100;
	dec %= 100;
	chars[3] = dec / 10;
	dec %= 10;
	chars[4] = dec;
	if(chars[0] != 0)
		i = 0;
	else if(chars[1] != 0)
		i = 1;
	else if(chars[2] != 0)
		i = 2;
	else if(chars[3] != 0)
		i = 3;
	while(i < 5)
		tx_char(chars[i++] + '0');
}

void tx_str(const char* s)
{
	unsigned char c;
	while(1) {
		c = (unsigned char)pgm_read_byte(s++);
		if(!c)
			return;
		tx_char(c);
	}
}
