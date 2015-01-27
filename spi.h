#ifndef __SPI_H__
#define __SPI_H__

#include "types.h"

#define	SPI_CLOCK_DIV4		0x00
#define	SPI_CLOCK_DIV16		0x01
#define	SPI_CLOCK_DIV64		0x02
#define	SPI_CLOCK_DIV128	0x03
#define	SPI_CLOCK_DIV2		0x04
#define	SPI_CLOCK_DIV8		0x05
#define	SPI_CLOCK_DIV32		0x06

#define	SPI_MODE0		0x00
#define	SPI_MODE1		0x04
#define	SPI_MODE2		0x08
#define	SPI_MODE3		0x0C

#define	SPI_MODE_MASK		0x0C // CPOL = bit 3, CPHA = bit 2 on SPCR
#define	SPI_CLOCK_MASK		0x03 // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define	SPI_2XCLOCK_MASK	0x01 // SPI2X = bit 0 on SPSR

#define	LSBFIRST		0x00
#define	MSBFIRST		0x01

void SPI_init();
void SPI_cleanup();
void SPI_setBitOrder(const u8 bitOrder);
void SPI_setDataMode(const u8 mode);
void SPI_setClockDivider(const u8 rate);
void SPI_transfer(const u8 data);

#endif
