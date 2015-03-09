#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bmp.h"
#include "gx.h"
#include "vt.h"

u16* buffer;
const char* vt52 = "\r\nHello, World!\r\nBla blub.\eHVT52 Emulator\eY\x33\x47"
	"Center\eB\rabcdef\eD\eD\eD\eKXYZ\t1\t2\t3\r\n0123456789ABCDEF\r\na\tbcd\te";

void DEMO_init()
{
}

void DEMO_drawPixel(const u16 x, const u16 y, const u16 color)
{
	int idx = y * WIDTH + x;
	buffer[idx] = color;
}

u16 DEMO_width()
{
	return WIDTH;
}

u16 DEMO_height()
{
	return HEIGHT;
}

int main(int argc, char** argv)
{
	buffer = (u16*) malloc(WIDTH * HEIGHT * sizeof(u16));
	VT_init();
	//const char* c = vt52;
	//while(*c != 0)
	//	rx_char(*(c++));
	// test VT routines
	//mode = MODE_VT52;
	//VT_reset();
	FILE* f = fopen("vt220-bug.log", "rb");
	char c;
	while(!feof(f)) {
		c = fgetc(f);
		rx_char(c);
	}
	// test GX routines
	GX_drawLine(0, 0, 20, 10, 0xFFFF);
	GX_drawPixel(1, 230, 0xFFFF);
	GX_drawCircle(160, 120, 64, 0xFFFF);
	GX_fillCircle(160, 120, 16, 0xFFFF);
	GX_fillTriangle(160, 230, 190, 210, 175, 190, 0xFFFF);
	GX_fillRect(160, 10, 32, 16, 0xFFFF);
	GX_drawRoundRect(32, 196, 64, 32, 5, 0xFFFF);
	GX_fillRoundRect(224, 196, 64, 32, 5, 0xFFFF);
	fclose(f);
	bmp_header hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.type = 0x4D42;
	hdr.Size2 = 0x28;
	hdr.OffBits = sizeof(bmp_header);
	hdr.BiBitCount = 16;
	hdr.Planes = 1;
	hdr.Size1 = WIDTH * HEIGHT * 2 + sizeof(bmp_header);
	hdr.Width = WIDTH;
	hdr.Height = HEIGHT;
	FILE* out = fopen("out.bmp", "wb");
	fwrite(&hdr, sizeof(hdr), 1, out);
	int y;
	for(y = HEIGHT - 1; y >= 0; y--)
		fwrite(&buffer[y * WIDTH], WIDTH * 2, 1, out);
	fclose(out);
	return 0;
}

void tx_char(char c)
{
	printf("%c", c);
}

//void scroll_up()
//{
//	int cx;
//	int cy;
//	int idx;
//	int o;
//	u16 c;
//	for(cy = 1; cy < LINES; cy++) {
//		for(cx = 0; cx < WIDTH; cx++) {
//			for(o = 0; o < 6; o++) {
//				idx = (cy * FONT_HEIGHT + o) * WIDTH + cx;
//				c = buffer[idx];
//				idx = ((cy - 1) * FONT_HEIGHT + o) * WIDTH + cx;
//				buffer[idx] = c;
//			}
//		}
//	}
//}
//
//void scroll_down()
//{
//	int cx;
//	int cy;
//	int idx;
//	int o;
//	u16 c;
//	for(cy = 0; cy < LINES - 1; cy++) {
//		for(cx = 0; cx < WIDTH; cx++) {
//			for(o = 0; o < FONT_HEIGHT; o++) {
//				idx = (cy * FONT_HEIGHT + o) * WIDTH + cx;
//				c = buffer[idx];
//				idx = ((cy + 1) * FONT_HEIGHT + o) * WIDTH + cx;
//				buffer[idx] = c;
//			}
//		}
//	}
//}
