#include <avr/io.h>
#include "usart.h"
#include "gx.h"
#include "vt.h"

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

int main()
{
	unsigned char c;
	USART_init(USART_getBaud(BAUD));
	GX_init();
	VT_init();

	while(1) {
		if(USART_canRead()) {
			c = USART_readByte();
			rx_char(c);
		}
	}

	return 0;
}
