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
#define	USART0_TX_vect		USART_TX_vect
#endif
#if !defined(USART0_RX_vect) && defined(USART_RX_vect)
#define	USART0_RX_vect		USART_RX_vect
#endif
#if !defined(USART0_UDRE_vect) && defined(USART_UDRE_vect)
#define	USART0_UDRE_vect	USART_UDRE_vect
#endif

#define	RX_BUFFER_SIZE		(1 << READBUFFERSIZE)
#define	TX_BUFFER_SIZE		(1 << WRITEBUFFERSIZE)

#define	RX_BUFFER_MASK		(RX_BUFFER_SIZE - 1)
#define	TX_BUFFER_MASK		(TX_BUFFER_SIZE - 1)

static unsigned char tx_read;	/*!< Read pointer in the transmit buffer */
static unsigned char tx_write;	/*!< Write pointer in the transmit buffer */
static unsigned char rx_read;	/*!< Read pointer in the receive buffer */
static unsigned char rx_write;	/*!< Write pointer in the receive buffer */

static unsigned char txbuffer[TX_BUFFER_SIZE]; /*!< Transmit buffer */
static unsigned char rxbuffer[RX_BUFFER_SIZE]; /*!< Receive buffer */

static unsigned char paused;

//static void do_xoff()
//{
//	return;
//	unsigned char xoff = XOFF;
//	paused = USART_send(&xoff, 1);
//}
//
//static void do_xon()
//{
//	return;
//	unsigned char xon = XON;
//	if(USART_free() < (RX_BUFFER_SIZE / 2))
//		return;
//	if(paused)
//		USART_send(&xon, 1);
//	paused = 0;
//}

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
	UCSR0B |= ((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0));
	UCSR0B &= ~((1 << UDRIE0) | (1 << TXCIE0) | (1 << UCSZ02));
	UCSR0A &= ~(1 << MPCM0); // SingleMaster
	UCSR0C = config;
	tx_read = tx_write = 0;
	rx_read = rx_write = 0;
	paused = 0;
}

unsigned char USART_available()
{
	return (RX_BUFFER_SIZE + rx_write - rx_read) & RX_BUFFER_MASK;
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
	return rx_write >= rx_read ? RX_BUFFER_MASK - rx_write + rx_read - 1 :
		rx_read - rx_write - 1;
}

unsigned char USART_canRead(void)
{
	return rx_read != rx_write;
}

unsigned char USART_read(unsigned char* buf, unsigned char length)
{
	unsigned char i;
	for(i = 0; i < length; i++) {
		if(rx_read == rx_write)
			break;
		*buf++ = rxbuffer[rx_read++];
		rx_read &= RX_BUFFER_MASK;
	}
	//do_xon();
	return i;
}

unsigned char USART_readByte(unsigned char* b)
{
	if(rx_read == rx_write)
		return 0;
	*b = rxbuffer[rx_read++];
	rx_read &= RX_BUFFER_MASK;
	//do_xon();
	return 1;
}

unsigned char USART_send(const unsigned char* data, unsigned char length)
{
	unsigned char i;
	unsigned char sreg;
	sreg = SREG;
	cli();
	unsigned char freesize = tx_write >= tx_read ? TX_BUFFER_SIZE -
			(int)tx_write + (int)tx_read - 1 : tx_read -
			tx_write - 1;
	if(length > freesize) { // not enough free space
		SREG = sreg;
		return 0;
	}
	for(i = 0; i < length; i++) { // data -> transmit buffer
		txbuffer[tx_write++] = *data++;
		tx_write &= TX_BUFFER_MASK;
	}
	UCSR0B |= (1 << UDRIE0);
	SREG = sreg;
	return 1;
}

/*! \brief Transmit interrupt */
ISR(USART0_UDRE_vect)
{
	// buffer empty
	if(tx_read == tx_write) {
		UCSR0B &= ~(1 << UDRIE0);
	} else {
		UDR0 = txbuffer[tx_read++];
		tx_read &= TX_BUFFER_MASK;
	}
}

/*! \brief Receive interrupt */
ISR(USART0_RX_vect)
{
	//unsigned char f = USART_free();
	//if(f == 0) // rx buffer full, drop incoming byte
	//	return;
	rxbuffer[rx_write++] = UDR0;
	rx_write &= TX_BUFFER_MASK;
	//if(f < 5)
	//	do_xoff();
}
