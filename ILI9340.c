#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <limits.h>

#include "ILI9340.h"

#define	NOP __asm__ __volatile__ ("nop");
#define	WAIT NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP // 14 NOPs 

#define	SET_BIT(port, mask)	port |= (mask)
#define	CLEAR_BIT(port, mask)	port &= ~(mask)

#define	ESET_BIT(pin, mask)	SET_BIT(pin, mask)
#define	ECLEAR_BIT(pin, mask)	CLEAR_BIT(pin, mask)
#define	SET_PIN(pin)		ESET_BIT(pin, pin##MASK)
#define	CLEAR_PIN(pin)		ECLEAR_BIT(pin, pin##MASK)

#define	delay			_delay_ms

static u16 __width = ILI9340_TFTWIDTH;
static u16 __height = ILI9340_TFTHEIGHT;

static const u8 init_sequence[] PROGMEM = {
	0x13, // 19 commands
	0xCB, 0x05, 0x39, 0x2C, 0x00, 0x34, 0x02,
	0xCF, 0x03, 0x00, 0xC1, 0x30,
	0xE8, 0x03, 0x85, 0x00, 0x78,
	0xEA, 0x02, 0x00, 0x00,
	0xED, 0x04, 0x64, 0x03, 0x12, 0x81,
	0xF7, 0x01, 0x20,
	0xC0, 0x01, 0x23, // Power control: VRH[5:0]
	0xC1, 0x01, 0x10, // Power control: SAP[2:0];BT[3:0]
	0xC5, 0x02, 0x3E, 0x28, // VCM control
	0xC7, 0x01, 0x86, //VCM control 2
	0x36, 0x01, 0xC8, // Memory Access Control
	0x3A, 0x01, 0x55,
	0xB1, 0x02, 0x00, 0x18,
	0xB6, 0x03, 0x08, 0x82, 0x27, // Display Function Control
	0xF2, 0x01, 0x00, // Gamma Function Disable
	0x26, 0x01, 0x01, // Gamma curve select
	0xE0, 0x0F, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
	0xE1, 0x0F, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	0x11, 0x00 // Exit sleep
};

inline void spiwrite_with_abandon(const u8 c)
{
	SPDR = c;
}

inline void spiwrite(const u8 c)
{
	SPDR = c;
	WAIT;
}

void ILI9340_writecommand(const u8 c)
{
	SET_PIN(DC);
	CLEAR_PIN(SCLK);
	CLEAR_PIN(CS);
	spiwrite(c);
	SET_PIN(CS);
}

void ILI9340_writedata(const u8 c)
{
	CLEAR_PIN(DC);
	CLEAR_PIN(SCLK);
	CLEAR_PIN(CS);
	spiwrite(c);
	SET_PIN(CS);
}

#define	DELAY	0x80
#define	NO_SLEEP
void ILI9340_commandList(const u8* addr)
{
	u8 cmds;
	u8 args;
#ifndef NO_SLEEP
	u16 ms;
#endif
	cmds = pgm_read_byte(addr++);
	while(cmds--) {
		ILI9340_writecommand(pgm_read_byte(addr++));
		args = pgm_read_byte(addr++);
#ifndef NO_SLEEP
		ms = args & DELAY;
		args &= ~DELAY;
#endif
		while(args--) {
			ILI9340_writedata(pgm_read_byte(addr++));
		}
#ifndef NO_SLEEP
		if(ms) {
			ms = pgm_read_byte(addr++);
			if(ms == 255)
				ms = 500;
			delay(ms);
		}
#endif
	}
}

void ILI9340_init()
{
	// TODO: initialize spi
	SET_PIN(RST);
	delay(5);
	CLEAR_PIN(RST);
	delay(20);
	SET_PIN(RST);
	delay(150);

	ILI9340_commandList(init_sequence);
	delay(120);

	// Display on
	ILI9340_writecommand(0x29);
	ILI9340_writecommand(0x2C);
}

void ILI9340_setAddrWindow(const u16 x0, const u16 y0, const u16 x1,
		const u16 y1)
{
	ILI9340_writecommand(ILI9340_CASET); // Column addr set
	ILI9340_writedata(x0 >> 8);
	ILI9340_writedata(x0 & 0xFF); // XSTART
	ILI9340_writedata(x1 >> 8);
	ILI9340_writedata(x1 & 0xFF); // XEND
	ILI9340_writecommand(ILI9340_PASET); // Row addr set
	ILI9340_writedata(y0 >> 8);
	ILI9340_writedata(y0 & 0xFF); // YSTART
	ILI9340_writedata(y1 >> 8);
	ILI9340_writedata(y1 & 0xFF); // YEND
	ILI9340_writecommand(ILI9340_RAMWR); // write to RAM
}

void ILI9340_pushColor(const u16 color)
{
	SET_PIN(DC);
	CLEAR_PIN(CS);
	spiwrite(color >> 8);
	spiwrite_with_abandon(color & 0xFF);
	SET_PIN(CS);
}

void ILI9340_drawPixel(const u16 x, const u16 y, const u16 color)
{
	ILI9340_setAddrWindow(x, y, x, y);
	SET_PIN(DC);
	CLEAR_PIN(CS);
	spiwrite(color >> 8);
	spiwrite_with_abandon(color & 0xFF);
	SET_PIN(CS);
}

void ILI9340_drawVLine(const u16 x, const u16 y, const u16 h,
		const u16 color)
{
	u16 _h = h;
	if((x >= __width) || (y >= __height))
		return;

	if((y + h - 1) >= __height)
		_h = __height - y;

	ILI9340_setAddrWindow(x, y, x, y + _h - 1);

	u8 hi = color >> 8;
	u8 lo = color & 0xFF;

	SET_PIN(DC);
	CLEAR_PIN(CS);
	while(_h--) {
		spiwrite(hi);
		spiwrite(lo);
	}
	SET_PIN(CS);
}

void ILI9340_drawHLine(const u16 x, const u16 y, const u16 w,
		const u16 color)
{
	u16 _w = w;
	if((x >= __width) || (y >= __height))
		return;
	if((x + w - 1) >= __width)
		_w = __width - x;

	ILI9340_setAddrWindow(x, y, x + _w - 1, y);

	u8 hi = color >> 8;
	u8 lo = color & 0xFF;

	SET_PIN(DC);
	CLEAR_PIN(CS);
	while(_w--) {
		spiwrite(hi);
		spiwrite(lo);
	}
	SET_PIN(CS);
}

void ILI9340_fillScreen(const u16 color)
{
	ILI9340_fillRect(0, 0, __width, __height, color);
}

void ILI9340_fillRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 color)
{
	u16 _w = w;
	u16 _h = h;
	if((x >= __width) || (y >= __height))
		return;
	if((x + w - 1) >= __width)
		_w = __width - x;
	if((y + h - 1) >= __height)
		_h = __height - y;

	ILI9340_setAddrWindow(x, y, x + _w - 1, y + _h - 1);

	u8 hi = color >> 8;
	u8 lo = color & 0xFF;

	SET_PIN(DC);
	CLEAR_PIN(CS);

	for(; _h > 0; _h--) {
		for(; _w >= 0; _w--) {
			spiwrite(hi);
			spiwrite(lo);
		}
	}

	SET_PIN(CS);
}

u16 ILI9340_color565(const u8 r, const u8 g, const u8 b)
{
	return ((u16)(r & 0xF8) << 8) | ((u16)(g & 0xFC) << 3) | (b >> 3);
}

void ILI9340_setRotation(const u8 m)
{
	ILI9340_writecommand(ILI9340_MADCTL);
	u8 rotation = m % 4;
	switch(rotation) {
		case 0:
			ILI9340_writedata(ILI9340_MADCTL_MX
					| ILI9340_MADCTL_BGR);
			__width = ILI9340_TFTWIDTH;
			__height = ILI9340_TFTHEIGHT;
			break;
		case 1:
			ILI9340_writedata(ILI9340_MADCTL_MV
					| ILI9340_MADCTL_BGR);
			__width = ILI9340_TFTHEIGHT;
			__height = ILI9340_TFTWIDTH;
			break;
		case 2:
			ILI9340_writedata(ILI9340_MADCTL_MY
					| ILI9340_MADCTL_BGR);
			__width = ILI9340_TFTWIDTH;
			__height = ILI9340_TFTHEIGHT;
			break;
		case 3:
			ILI9340_writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_MY
					| ILI9340_MADCTL_MX
					| ILI9340_MADCTL_BGR);
			__width = ILI9340_TFTHEIGHT;
			__height = ILI9340_TFTWIDTH;
			break;
	}
}

void ILI9340_invertDisplay(const u8 i)
{
	ILI9340_writecommand(i ? ILI9340_INVON : ILI9340_INVOFF);
}
