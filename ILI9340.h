#ifndef __ILI9340_H__
#define __ILI9340_H__

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "types.h"

#define	ILI9340_TFTWIDTH	240
#define	ILI9340_TFTHEIGHT	320

#define	ILI9340_NOP		0x00
#define	ILI9340_SWRESET		0x01
#define	ILI9340_RDDID		0x04
#define	ILI9340_RDDST		0x09

#define	ILI9340_SLPIN		0x10
#define	ILI9340_SLPOUT		0x11
#define	ILI9340_PTLON		0x12
#define	ILI9340_NORON		0x13

#define	ILI9340_RDMODE		0x0A
#define	ILI9340_RDMADCTL	0x0B
#define	ILI9340_RDPIXFMT	0x0C
#define	ILI9340_RDIMGFMT	0x0A
#define	ILI9340_RDSELFDIAG	0x0F

#define	ILI9340_INVOFF		0x20
#define	ILI9340_INVON		0x21
#define	ILI9340_GAMMASET	0x26
#define	ILI9340_DISPOFF		0x28
#define	ILI9340_DISPON		0x29

#define	ILI9340_VSCRDEF		0x33
#define	ILI9340_VSCRSADD	0x37

#define	ILI9340_CASET		0x2A
#define	ILI9340_PASET		0x2B
#define	ILI9340_RAMWR		0x2C
#define	ILI9340_RAMRD		0x2E

#define	ILI9340_PTLAR		0x30
#define	ILI9340_MADCTL		0x36


#define	ILI9340_MADCTL_MY	0x80
#define	ILI9340_MADCTL_MX	0x40
#define	ILI9340_MADCTL_MV	0x20
#define	ILI9340_MADCTL_ML	0x10
#define	ILI9340_MADCTL_RGB	0x00
#define	ILI9340_MADCTL_BGR	0x08
#define	ILI9340_MADCTL_MH	0x04

#define	ILI9340_PIXFMT		0x3A

#define	ILI9340_FRMCTR1		0xB1
#define	ILI9340_FRMCTR2		0xB2
#define	ILI9340_FRMCTR3		0xB3
#define	ILI9340_INVCTR		0xB4
#define	ILI9340_DFUNCTR		0xB6

#define	ILI9340_PWCTR1		0xC0
#define	ILI9340_PWCTR2		0xC1
#define	ILI9340_PWCTR3		0xC2
#define	ILI9340_PWCTR4		0xC3
#define	ILI9340_PWCTR5		0xC4
#define	ILI9340_VMCTR1		0xC5
#define	ILI9340_VMCTR2		0xC7

#define	ILI9340_RDID1		0xDA
#define	ILI9340_RDID2		0xDB
#define	ILI9340_RDID3		0xDC
#define	ILI9340_RDID4		0xDD

#define	ILI9340_GMCTRP1		0xE0
#define	ILI9340_GMCTRN1		0xE1

// Color definitions
#define	ILI9340_BLACK		0x0000
#define	ILI9340_BLUE		0x001F
#define	ILI9340_RED		0xF800
#define	ILI9340_GREEN		0x07E0
#define	ILI9340_CYAN		0x07FF
#define	ILI9340_MAGENTA		0xF81F
#define	ILI9340_YELLOW		0xFFE0
#define	ILI9340_WHITE		0xFFFF

// I/O pins
#define	CS			PORTB
#define	DC			PORTB
#define	MOSI			PORTB
#define	MISO			PORTB
#define	SCLK			PORTB
#define	RST			PORTB

#define	CSMASK			_BV(PORTB2)
#define	DCMASK			_BV(PORTB1)
#define	MOSIMASK		_BV(PORTB3)
#define	MISOMASK		_BV(PORTB4)
#define	SCLKMASK		_BV(PORTB5)
#define	RSTMASK			_BV(PORTB0)

#define	DDRCS			DDRB
#define	DDRDC			DDRB
#define	DDRRST			DDRB

// Commands
void	ILI9340_writecommand(const u8 c);
void	ILI9340_writedata(const u8 c);
void	ILI9340_commandList(const u8* addr);
void	ILI9340_init();
void	ILI9340_setAddrWindow(const u16 x0, const u16 y0, const u16 x1,
		const u16 y1);
void	ILI9340_pushColor(const u16 color);
void	ILI9340_drawPixel(const u16 x, const u16 y, const u16 color);
void	ILI9340_drawVLine(const u16 x, const u16 y, const u16 h,
		const u16 color);
void	ILI9340_drawHLine(const u16 x, const u16 y, const u16 w,
		const u16 color);
void	ILI9340_fillScreen(const u16 color);
void	ILI9340_fillRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 color);
u16	ILI9340_color565(const u8 r, const u8 g, const u8 b);
void	ILI9340_setRotation(const u8 m);
void	ILI9340_invertDisplay(const u8 i);
void	ILI9340_begin(const u16 x, const u16 y, const u8 width, const u8 height);
void	ILI9340_end();
void	ILI9340_pixel(const u16 color);

void	ILI9340_scrollingDefinition(const u16 tfa, const u16 bfa);
void	ILI9340_scrollPosition(const u16 vsp);
void	ILI9340_scroll_up(const u16 lines, const u16 bgcolor);

u16	ILI9340_width();
u16	ILI9340_height();

// Exports
#define	HAS_DRAWHLINE
#define	HAS_DRAWVLINE
#define	HAS_FILLRECT
#define	HAS_FILLSCREEN
#define	HAS_BITMAP

#endif
