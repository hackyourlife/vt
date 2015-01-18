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

#define	WRITEBUFFERSIZE		64	/*!< Transmit buffer size in bytes */
#define READBUFFERSIZE		128	/*!< Receive buffer size in bytes */

#define	XOFF			0x13
#define	XON			0x11

#define	BAUD_MACRO

//#define USART_available()	(UCSR0A & (1 << RXC0))

/*! \brief Returns the amount of data in the read buffer */
#define USART_available()	(READBUFFERSIZE - USART_free() - 1)

/*! \brief Calculates the baud setting.
 *
 *  This function converts the given baud rate to the microcontroller's baud
 *  rate configuration value. This value must be passed to USART_init().
 *
 *  \param [in] baud	The baud rate to be used
 *  \return		The baud rate configuration value
 */
#ifdef BAUD_MACRO
#define	USART_getBaud(baud)	((F_CPU / 8 / baud - 1) / 2)
#else
unsigned short USART_getBaud(unsigned long baud);
#endif

/*! \brief Initializes the USART library
 *
 *  \param [in] baud	The baud rate to be used, see datasheet for more details.
 */
void USART_init(unsigned short baud);

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
unsigned char USART_readByte(void);

/*! \brief Sends a block of data.
 *
 *  \param [in] data	Pointer to the data to send
 *  \param [in] length	Amount of data to send
 *  \return		0 on error, 1 on success
 */
unsigned char USART_send(const unsigned char* data, unsigned char length);

#endif /* __USART_H__ */
