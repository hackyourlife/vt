#include "gx.h"
#include "vt.h"

static int mode = MODE_VT52;

static u8 cursorX = 0;
static u8 cursorY = 0;
static u16 fgcolor = 0xFFFF;
static u16 bgcolor = 0x0000;

static u8 tabstops[COLUMNS];

void VT_init()
{
	int i;
	for(i = 0; i < COLUMNS; i++)
		tabstops[i] = i % 8 == 7;
}

void drawchar(char c)
{
	u16 px = cursorX * FONT_WIDTH;
	u16 py = cursorY * FONT_HEIGHT;
	drawchar_mini(c, fgcolor, bgcolor, px, py);
	cursorX++;
	if(cursorX == COLUMNS) {
		carriage_return();
		linefeed();
	}
}

void rx_char(char c)
{
	switch(mode) {
		case MODE_VT52:
			if(vt52_process(c))
				drawchar(c);
			break;
	}
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
	u8 i;
	u16 px = cursorX * 4;
	u16 py = cursorY * 6;
	while(px < WIDTH) {
		for(i = 0; i < 6; i++)
			GX_drawPixel(px, py + i, bgcolor);
		px++;
	}
}

void erase_cursor_to_end()
{
	erase_cursor_to_eol();
	if(cursorY == (LINES - 1))
		return;
	u16 px = 0;
	u16 py = (cursorY + 1) * 6;
	GX_fillRect(px, py, WIDTH - px, HEIGHT - py, bgcolor);
}

void erase_line(const u8 n)
{
	u16 y = n * FONT_HEIGHT;
	GX_fillRect(0, y, WIDTH, FONT_HEIGHT, bgcolor);
}
