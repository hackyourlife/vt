/*! \file usart.c
 *  \author	Daniel Pekarek
 *  \date	27.04.2013
 *  \version	1.0
 *  \brief	Implementation of USART library.
 */

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

#if !defined(USART0_TX_vect) && defined(USART_TX_vect)
#define	USART0_TX_vect	USART_TX_vect
#endif
#if !defined(USART0_RX_vect) && defined(USART_RX_vect)
#define	USART0_RX_vect	USART_RX_vect
#endif

static unsigned char* tx_read;		/*!< Read pointer in the transmit buffer */
static unsigned char* tx_write;		/*!< Write pointer in the transmit buffer */
static unsigned char* rx_read;		/*!< Read pointer in the receive buffer */
static unsigned char* rx_write;		/*!< Write pointer in the receive buffer */

static unsigned char writebuffer[WRITEBUFFERSIZE]; /*!< Transmit buffer */
static unsigned char readbuffer[READBUFFERSIZE]; /*!< Receive buffer */

static unsigned char running;		/*!< True when there is data to be transmitted */

static unsigned char paused;

static void do_xoff()
{
	return;
	unsigned char xoff = XOFF;
	paused = USART_send(&xoff, 1);
}

static void do_xon()
{
	return;
	unsigned char xon = XON;
	if(USART_free() < (READBUFFERSIZE / 2))
		return;
	if(paused)
		USART_send(&xon, 1);
	paused = 0;
}

#ifndef BAUD_MACRO
unsigned short USART_getBaud(unsigned long baud)
{
	return F_CPU / 8 / baud - 1;
}
#endif

void USART_init(unsigned short baud, unsigned char config)
{
	UBRR0H = (unsigned char) (baud >> 8);
	UBRR0L = (unsigned char) baud;
	UCSR0A |= (1 << U2X0);
	UCSR0B |= ((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0));
	UCSR0B &= ~(1 << UDRIE0);
	//UCSR0C |= ((1 << UCSZ00) | (1 << UCSZ01));
	//UCSR0C &= ~((1 << UCSZ02) | (1 << USBS0));
	UCSR0C = config;
	tx_read = tx_write = writebuffer;
	rx_read = rx_write = readbuffer;
	running = 0;
	paused = 0;
}

unsigned char USART_receive(void)
{
	/* Wait for data to be received */
	while(!(UCSR0A & (1 << RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}

unsigned char USART_free(void)
{
	return rx_write >= rx_read ? READBUFFERSIZE - (int)rx_write +
			(int)rx_read - 1 : rx_read - rx_write - 1;
}

unsigned char USART_canRead(void)
{
	return rx_read != rx_write;
}

unsigned char USART_read(unsigned char* buf, unsigned char length)
{
	unsigned char i;
	if(rx_read == rx_write)
		return 0;
	for(i = 0; i < length; i++) {
		*buf++ = *rx_read++;
		if(rx_read >= &readbuffer[READBUFFERSIZE])
			rx_read = readbuffer;
	}
	do_xon();
	return i;
}

unsigned char USART_readByte(unsigned char* b)
{
	if(rx_read == rx_write)
		return 0;
	*b = *rx_read++;
	if(rx_read >= &readbuffer[READBUFFERSIZE])
		rx_read = readbuffer;
	do_xon();
	return 1;
}

unsigned char USART_send(const unsigned char* data, unsigned char length)
{
	unsigned char i;
	unsigned char sreg;
	sreg = SREG;
	cli();
	unsigned char freesize = tx_write >= tx_read ? WRITEBUFFERSIZE -
			(int)tx_write + (int)tx_read - 1 : tx_read -
			tx_write - 1;
	if(length > freesize) { // not enough free space
		SREG = sreg;
		return 0;
	}
	if(!running) { // initial byte transmission, starts interrupt
		UDR0 = *data;
		data++;
		length--;
		running = 1;
	}
	for(i = 0; i < length; i++) { // data -> transmit buffer
		*tx_write++ = *data++;
		if(tx_write >= &writebuffer[WRITEBUFFERSIZE])
			tx_write = writebuffer;
	}
	SREG = sreg;
	return 1;
}

/*! \brief Transmit interrupt */
ISR(USART0_TX_vect)
{
	// buffer empty
	if(tx_read == tx_write) {
		running = 0;
		return;
	}
	UDR0 = *tx_read++;
	if(tx_read >= &writebuffer[WRITEBUFFERSIZE])
		tx_read = writebuffer;
}

/*! \brief Receive interrupt */
ISR(USART0_RX_vect)
{
	unsigned char f = USART_free();
	if(f == 0) // rx buffer full, drop incoming byte
		return;
	*rx_write++ = UDR0;
	if(f < 5)
		do_xoff();
	if(rx_write >= &readbuffer[READBUFFERSIZE])
		rx_write = readbuffer;
}
