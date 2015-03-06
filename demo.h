#ifndef __DEMO_H__
#define __DEMO_H__

#include "types.h"

void DEMO_init();
void DEMO_drawPixel(const u16 x, const u16 y, const u16 color);
u16 DEMO_width();
u16 DEMO_height();

#endif
