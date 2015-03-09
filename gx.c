#include <stdlib.h>
#include "gx.h"

static u16 tmpu16;
#define	swap(a, b)	{ tmpu16 = a; a = b; b = tmpu16; }

static void GX_drawCircleHelper(const u16 x0, const u16 y0, const u16 r,
		const u8 corner, const u16 color);
static void GX_fillCircleHelper(const u16 x0, const u16 y0, const u16 r,
		const u8 corner, const s16 delta, const u16 color);

void GX_init()
{
	GX_initDisplay();
}

#ifndef HAS_DRAWCIRCLE
void GX_drawCircle(const u16 x0, const u16 y0, const u16 r, const u16 color)
{
	s16 f = 1 - r;
	s16 ddF_x = 1;
	s16 ddF_y = -2 * r;
	s16 x = 0;
	s16 y = r;

	GX_drawPixel(x0, y0 + r, color);
	GX_drawPixel(x0, y0 - r, color);
	GX_drawPixel(x0 + r, y0, color);
	GX_drawPixel(x0 - r, y0, color);

	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		GX_drawPixel(x0 + x, y0 + y, color);
		GX_drawPixel(x0 - x, y0 + y, color);
		GX_drawPixel(x0 + x, y0 - y, color);
		GX_drawPixel(x0 - x, y0 - y, color);
		GX_drawPixel(x0 + y, y0 + x, color);
		GX_drawPixel(x0 - y, y0 + x, color);
		GX_drawPixel(x0 + y, y0 - x, color);
		GX_drawPixel(x0 - y, y0 - x, color);
	}
}
#endif

static void GX_drawCircleHelper(const u16 x0, const u16 y0, const u16 r,
		const u8 corner, const u16 color)
{
	s16 f = 1 - r;
	s16 ddF_x = 1;
	s16 ddF_y = -2 * r;
	s16 x = 0;
	s16 y = r;
	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if(corner & 0x04) {
			GX_drawPixel(x0 + x, y0 + y, color);
			GX_drawPixel(x0 + y, y0 + x, color);
		}
		if(corner & 0x02) {
			GX_drawPixel(x0 + x, y0 - y, color);
			GX_drawPixel(x0 + y, y0 - x, color);
		}
		if(corner & 0x08) {
			GX_drawPixel(x0 - y, y0 + x, color);
			GX_drawPixel(x0 - x, y0 + y, color);
		}
		if(corner & 0x01) {
			GX_drawPixel(x0 - y, y0 - x, color);
			GX_drawPixel(x0 - x, y0 - y, color);
		}
	}
}

#ifndef HAS_FILLCIRCLE
void GX_fillCircle(const u16 x0, const u16 y0, const u16 r, const u16 color)
{
	GX_drawVLine(x0, y0 - r, 2 * r + 1, color);
	GX_fillCircleHelper(x0, y0, r, 3, 0, color);
}
#endif

static void GX_fillCircleHelper(const u16 x0, const u16 y0, const u16 r,
		const u8 corner, const s16 delta, const u16 color)
{
	s16 f = 1 - r;
	s16 ddF_x = 1;
	s16 ddF_y = -2 * r;
	s16 x = 0;
	s16 y = r;

	while(x < y) {
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if(corner & 0x01) {
			GX_drawVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			GX_drawVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if(corner & 0x02) {
			GX_drawVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			GX_drawVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

#ifndef HAS_DRAWLINE
void GX_drawLine(const u16 x0, const u16 y0, const u16 x1, const u16 y1,
		const u16 color)
{
	u16 _x0 = x0;
	u16 _y0 = y0;
	u16 _x1 = x1;
	u16 _y1 = y1;
	u8 steep = abs(y1 - y0) > abs(x1 - x0);
	if(steep) {
		swap(_x0, _y0);
		swap(_x1, _y1);
	}
	if(_x0 > _x1) {
		swap(_x0, _x1);
		swap(_y0, _y1);
	}

	s16 dx = _x1 - _x0;
	s16 dy = abs(_y1 - _y0);

	s16 err = dx / 2;
	s16 ystep;

	ystep = _y0 < _y1 ? 1 : -1;

	for(; _x0 <= _x1; _x0++) {
		if(steep)
			GX_drawPixel(_y0, _x0, color);
		else
			GX_drawPixel(_x0, _y0, color);
		err -= dy;
		if(err < 0) {
			_y0 += ystep;
			err += dx;
		}
	}
}
#endif

#ifndef HAS_DRAWRECT
void GX_drawRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 color)
{
	GX_drawHLine(x, y, w, color);
	GX_drawHLine(x, y + h - 1, w, color);
	GX_drawVLine(x, y, h, color);
	GX_drawVLine(x + w - 1, y, h, color);
}
#endif

#ifndef HAS_FILLRECT
void GX_fillRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 color)
{
	u16 i;
	for(i = x; i < x + w; i++)
		GX_drawVLine(i, y, h, color);
}
#endif

#ifndef HAS_DRAWROUNDRECT
void GX_drawRoundRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 r, const u16 color)
{
	GX_drawHLine(x + r, y, w - 2 * r, color); // top
	GX_drawHLine(x + r, y + h - 1, w - 2 * r, color); // bottom
	GX_drawVLine(x, y + r, h - 2 * r, color); // left
	GX_drawVLine(x + w - 1, y + r, h - 2 * r, color); // right
	GX_drawCircleHelper(x + r, y + r, r, 1, color);
	GX_drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	GX_drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	GX_drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}
#endif

#ifndef HAS_FILLROUNDRECT
void GX_fillRoundRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 r, const u16 color)
{
	GX_fillRect(x + r, y, w - 2 * r, h, color);
	GX_fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	GX_fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}
#endif

#ifndef HAS_DRAWTRIANGLE
void GX_drawTriangle(const u16 x0, const u16 y0, const u16 x1, const u16 y1,
		const u16 x2, const u16 y2, const u16 color)
{
	GX_drawLine(x0, y0, x1, y1, color);
	GX_drawLine(x1, y1, x2, y2, color);
	GX_drawLine(x2, y2, x0, y0, color);
}
#endif

#ifndef HAS_FILLTRIANGLE
void GX_fillTriangle(const u16 x0, const u16 y0, const u16 x1, const u16 y1,
		const u16 x2, const u16 y2, const u16 color)
{
	u16 _x0 = x0;
	u16 _y0 = y0;
	u16 _x1 = x1;
	u16 _y1 = y1;
	u16 _x2 = x2;
	u16 _y2 = y2;
	u16 a;
	u16 b;
	u16 y;
	u16 last;

	if(y0 > y1) {
		swap(_y0, _y1);
		swap(_x0, _x1);
	}

	if(_y1 > _y2) {
		swap(_y1, _y2);
		swap(_x1, _x2);
	}

	if(_y0 > _y1) {
		swap(_y0, _y1);
		swap(_x0, _x1);
	}

	if(_y0 == _y2) { // Handle awkward all-on-same-line-case as its own thing
		a = b = _x0;
		if(_x1 < a)
			a = _x1;
		else if(_x1 > b)
			b = _x1;
		if(_x2 < a)
			a = _x2;
		else if(_x2 > b)
			b = _x2;
		GX_drawHLine(a, _y0, b - a + 1, color);
		return;
	}

	s16 dx01 = _x1 - _x0;
	s16 dy01 = _y1 - _y0;
	s16 dx02 = _x2 - _x0;
	s16 dy02 = _y2 - _y0;
	s16 dx12 = _x2 - _x1;
	s16 dy12 = _y2 - _y1;
	s16 sa = 0;
	s16 sb = 0;

	// For upper part of triangle, find scanline crossings for segments 0-1
	// and 0-2. If y1 = y2 (flat-bottomed triangle), the scanline y1 is
	// included here (and second loop will be skipped, avoiding /0 error
	// there), otherwise scanline y1 is skipped here and handled in the
	// second loop, which also avoids a /0 error here if y0 = y1
	// (flat-topped triangle).
	if(_y1 == _y2)
		last = _y1;
	else
		last = _y1 - 1;

	for(y = _y0; y <= last; y++) {
		a = _x0 + sa / dy01;
		b = _x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if(a > b)
			swap(a, b);
		GX_drawHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments 0-2
	// and 1-2. This loop is skipped if y1 = y2.
	sa = dx12 * (y - _y1);
	sb = dx02 * (y - _y0);
	for(; y <= _y2; y++) {
		a = _x1 + sa / dy12;
		b = _x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b)
			swap(a, b);
		GX_drawHLine(a, y, b - a + 1, color);
	}
}
#endif

#ifndef HAS_DRAWHLINE
void GX_drawHLine(const u16 x, const u16 y, const u16 w, const u16 color)
{
	GX_drawLine(x, y, x + w - 1, y, color);
}
#endif

#ifndef HAS_DRAWVLINE
void GX_drawVLine(const u16 x, const u16 y, const u16 h, const u16 color)
{
	GX_drawLine(x, y, x, y + h - 1, color);
}
#endif

#ifndef HAS_FILLSCREEN
void GX_fillScreen(const u16 color)
{
	GX_fillRect(0, 0, GXCMD(width)(), GXCMD(height)(), color);
}
#endif

#ifndef HAS_BITMAP
static u16 x0;
static u16 y0;
static u16 w;
static u16 px;
static u16 py;
void GX_begin(const u16 x, const u16 y, const u8 width, const u8 height)
{
	x0 = x;
	y0 = y;
	px = x;
	py = y;
	w = width;
}

void GX_end()
{
}

void GX_pixel(const u16 color)
{
	GX_drawPixel(px, py, color);
	px++;
	if((px - x0) >= w) {
		px = x0;
		py++;
	}
}
#endif
