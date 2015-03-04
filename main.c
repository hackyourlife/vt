#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "usart.h"
#include "gx.h"
#include "vt.h"

#define	SET(port, pin)		port |= pin
#define	CLEAR(port, pin)	port &= ~(pin)

#define	LED_OFF(led)		SET(PORTD, led)
#define	LED_ON(led)		CLEAR(PORTD, led)

#define	LED_RED			_BV(PORTD3)
#define	LED_GREEN		_BV(PORTD4)
#define	LED_BLUE		_BV(PORTD6)
#define	LED_PWM			_BV(PORTD5)

const char welcome_text[] PROGMEM = "VT52 Emulator v1.0";
const char reset[] PROGMEM = "\eH\eJ";

void init();

void tx_char(char c)
{
	USART_send((unsigned char*) &c, 1);
}

void scroll_up()
{
}
void scroll_down()
{
}

void init_pins()
{
	DDRD |= _BV(PIND3) | _BV(PIND4) | _BV(PIND5) | _BV(PIND6);
	DDRD &= ~(_BV(PIND7));
	DDRC &= ~(_BV(PINC0) | _BV(PINC1));
}

void sequence(const char* s)
{
	unsigned char c;
	while(1) {
		c = (unsigned char)pgm_read_byte(s++);
		if(!c)
			return;
		rx_char(c);
	}

}

int main()
{
	unsigned char c;
	init_pins();
	LED_ON(LED_RED | LED_GREEN | LED_BLUE);

	USART_init(USART_getBaud(9600), USART_8N1);
	sei();

	GX_init();
	ILI9340_fillScreen(0x0000);
	VT_init();
	ILI9340_setRotation(1);

	LED_OFF(LED_PWM);

	LED_OFF(LED_GREEN | LED_BLUE);

	sequence(welcome_text);
	_delay_ms(1000);
	sequence(reset);

	LED_OFF(LED_RED);
	LED_ON(LED_GREEN);
	u8 cnt = 0;
	LED_OFF(LED_RED | LED_GREEN | LED_BLUE);

	while(1) {
		if(USART_readByte(&c)) {
			LED_ON(LED_BLUE);
			cnt++;
			rx_char(c);
			LED_OFF(LED_BLUE);
		}
	}

	return 0;
}
