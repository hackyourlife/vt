#ifndef __BMP_H__

#include "types.h"

typedef struct __attribute__((__packed__)) {
	u16	type;		/* 00 */	/* Magic: "BM" */
	u32	Size1;		/* 02 */
	u16	hotspot_x;	/* 06 */
	u16	hotspot_y;	/* 08 */
	u32	OffBits;	/* 0A */	/* image data starts at this addr (normal: 1078) */
	u32	Size2;		/* 0E */
	u32	Width;		/* 12 */
	u32	Height;		/* 16 */
	u16	Planes;		/* 1A */
	u16	BiBitCount;	/* 1C */	/* 8 bit=256 */
	u32	Compression;	/* 1E */
	u32	SizeImage;	/* 22 */
	u32	XPelsPerMtr;	/* 26 */
	u32	YPelsPerMtr;	/* 2A */
	u32	ColorsUsed;	/* 2E */
	u32	CImportant;	/* 32 */
} bmp_header;

#endif
