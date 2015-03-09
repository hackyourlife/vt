#ifndef __GX_H__
#define __GX_H__

#include "types.h"
#include "config.h"
#include DISPLAYDRIVER

#define	PASTER(x,y)	x ## _ ## y
#define	EVALUATOR(x,y)	PASTER(x,y)
#define	GXCMD(name)	EVALUATOR(DISPLAY, name)

#define	GX_drawPixel	GXCMD(drawPixel)
#define	GX_initDisplay	GXCMD(init)

#ifdef HAS_DRAWCIRCLE
#define	GX_drawCircle	GXCMD(drawCircle)
#endif
#ifdef HAS_FILLCIRCLE
#define	GX_fillCircle	GXCMD(fillCircle)
#endif
#ifdef HAS_DRAWRECT
#define	GX_drawRect	GXCMD(drawRect)
#endif
#ifdef HAS_FILLRECT
#define	GX_fillRect	GXCMD(fillRect)
#endif
#ifdef HAS_DRAWROUNDRECT
#define	GX_drawRoundRect GXCMD(drawRoundRect)
#endif
#ifdef HAS_FILLROUNDRECT
#define	GX_fillRoundRect GXCMD(fillRoundRect)
#endif
#ifdef HAS_DRAWTRIANGLE
#define	GX_drawTriangle	GXCMD(drawTriangle)
#endif
#ifdef HAS_FILLTRIANGLE
#define	GX_fillTriangle	GXCMD(fillTriangle)
#endif
#ifdef HAS_DRAWVLINE
#define	GX_drawVLine	GXCMD(drawVLine)
#endif
#ifdef HAS_DRAWHLINE
#define	GX_drawHLine	GXCMD(drawHLine)
#endif
#ifdef HAS_FILLSCREEN
#define	GX_fillScreen	GXCMD(fillScreen)
#endif
#ifdef HAS_BITMAP
#define	GX_begin	GXCMD(begin)
#define	GX_end		GXCMD(end)
#define	GX_pixel	GXCMD(pixel)
#endif

#define	RGB565(r, g, b)		(((r & 0xF8) << 8) | ((g & 0xFC) << 3) \
					| (b >> 3))

void	GX_init();
void	GX_drawCircle(const u16 x0, const u16 y0, const u16 r, const u16 color);
void	GX_fillCircle(const u16 x0, const u16 y0, const u16 r, const u16 color);
void	GX_drawLine(const u16 x0, const u16 y0, const u16 x1, const u16 y1,
		const u16 color);
void	GX_drawRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 color);
void	GX_fillRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 color);
void	GX_drawRoundRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 r, const u16 color);
void	GX_fillRoundRect(const u16 x, const u16 y, const u16 w, const u16 h,
		const u16 r, const u16 color);
void	GX_drawTriangle(const u16 x0, const u16 y0, const u16 x1, const u16 y1,
		const u16 x2, const u16 y2, const u16 color);
void	GX_fillTriangle(const u16 x0, const u16 y0, const u16 x1, const u16 y1,
		const u16 x2, const u16 y2, const u16 color);
void	GX_drawHLine(const u16 x, const u16 y, const u16 w, const u16 color);
void	GX_drawVLine(const u16 x, const u16 y, const u16 h, const u16 color);
void	GX_fillScreen(const u16 color);
void	GX_begin(const u16 x, const u16 y, const u8 width, const u8 height);
void	GX_end();
void	GX_pixel(const u16 color);

#endif
