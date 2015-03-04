/*! \file usart.h
 *  \author	Daniel Pekarek
 *  \date	27.04.2013
 *  \version	1.0
 *  \brief	USART library
 *
 *  This library handles asynchronous USART reads and writes.
 */
#ifndef	__USART_H__
#define	__USART_H__

#define	WRITEBUFFERSIZE		6	/*!< Transmit buffer size: 2^x bytes */
#define	READBUFFERSIZE		6	/*!< Receive buffer size: 2^x bytes */

#define	XOFF			0x13
#define	XON			0x11

#define	BAUD_MACRO

//#define USART_available()	(UCSR0A & (1 << RXC0))

/*! \brief Returns the amount of data in the read buffer */
//#define USART_available()	((1 << READBUFFERSIZE) - USART_free() - 1)
unsigned char USART_available();

/*! \brief Calculates the baud setting.
 *
 *  This function converts the given baud rate to the microcontroller's baud
 *  rate configuration value. This value must be passed to USART_init().
 *
 *  \param [in] baud	The baud rate to be used
 *  \return		The baud rate configuration value
 */
#ifdef BAUD_MACRO
#define	USART_getBaud(baud)	(F_CPU / 8 / baud - 1)
#else
unsigned short USART_getBaud(unsigned long baud);
#endif

#define	USART_5N1		0x00
#define	USART_6N1		0x02
#define	USART_7N1		0x04
#define	USART_8N1		0x06
#define	USART_5N2		0x08
#define	USART_6N2		0x0A
#define	USART_7N2		0x0C
#define	USART_8N2		0x0E
#define	USART_5E1		0x20
#define	USART_6E1		0x22
#define	USART_7E1		0x24
#define	USART_8E1		0x26
#define	USART_5E2		0x28
#define	USART_6E2		0x2A
#define	USART_7E2		0x2C
#define	USART_8E2		0x2E
#define	USART_5O1		0x30
#define	USART_6O1		0x32
#define	USART_7O1		0x34
#define	USART_8O1		0x36
#define	USART_5O2		0x38
#define	USART_6O2		0x3A
#define	USART_7O2		0x3C
#define	USART_8O2		0x3E

/*! \brief Initializes the USART library
 *
 *  \param [in] baud	The baud rate to be used, see datasheet for more details.
 */
void USART_init(unsigned short baud, unsigned char config);

/*! \brief Returns the number of free bytes of the receive buffer
 *  \return		Number of free bytes
 */
unsigned char USART_free(void);

/*! \brief Receives a single byte synchronously.
 *
 *  This function receives a single byte from the USART. It blocks
 *  until one byte was received.
 *
 *  \return		The received byte
 */
unsigned char USART_receive(void);

/*! \brief Indicates if the receive buffer is not empty.
 *  \return		True if at least one byte can be read
 */
unsigned char USART_canRead(void);

/*! \brief Reads data from the receive buffer.
 *
 *  This function transfers data from the receive buffer to a given
 *  buffer for further use. If you read too many bytes you will destroy
 *  the ringbuffer.
 *
 *  \param [out] buf	Destination buffer
 *  \param [in] length	Number of bytes to transfer
 *  \return		The number of bytes transfered.
 */
unsigned char USART_read(unsigned char* buf, unsigned char length);

/*! \brief Reads a single byte from the receive buffer.
 *  \return		The byte removed from the receive buffer.
 */
unsigned char USART_readByte(unsigned char* b);

/*! \brief Sends a block of data.
 *
 *  \param [in] data	Pointer to the data to send
 *  \param [in] length	Amount of data to send
 *  \return		0 on error, 1 on success
 */
unsigned char USART_send(const unsigned char* data, unsigned char length);

#endif /* __USART_H__ */
