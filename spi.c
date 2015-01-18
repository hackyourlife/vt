#include <avr/io.h>

#include "spi.h"

void SPI_init()
{
	PORTB &= ~_BV(PORTB2);
	DDRB |= _BV(DDB2);
	SPCR |= _BV(MSTR);
	SPCR |= _BV(SPE);
	DDRB |= _BV(DDB5) | _BV(DDB3);
}

void SPI_cleanup()
{
	SPCR &= ~_BV(SPE);
}

void SPI_setBitOrder(const u8 bitOrder)
{
	if(bitOrder == LSBFIRST)
		SPCR |= _BV(DORD);
	else
		SPCR &= ~(_BV(DORD));
}

void SPI_setDataMode(const u8 mode)
{
	SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

void SPI_setClockDivider(const u8 rate)
{
	SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
	SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}

