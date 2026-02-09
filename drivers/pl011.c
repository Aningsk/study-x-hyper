#include "pl011.h"

void pl011_putc(char c)
{
	/* Transmit fifo is full */
	while (*REG(PL011FR) & PL011_FR_TXFF) {
		/* release the core */
		asm volatile("yield" ::: "memory");
	}
	*REG(PL011DR) = c;
}

void pl011_puts(char *s)
{
	char c;
	while ((c = *s++)) {
		pl011_putc(c);
	}
}

int pl011_getc(void)
{
	/* Recieve buffer is not empty */
	if (*REG(PL011FR) & PL011_FR_RXFE) {
		return -1;
	} else {
		return *REG(PL011DR);
	}
}

void pl011_irq_handler(void)
{
	int status = *REG(PL011MIS);
	if (status & (1 << 4)) {
		for (;;) {
			int c = pl011_getc();
			if (c < 0)
				break;
			pl011_putc(c);
		}
	}
	*REG(PL011ICR) = (1 << 4);
}

void pl011_init(void)
{
	/* Disable the Uart */
	*REG(PL011CR) = 0;
	/* Clear all interrupt mask */
	*REG(PL011IMSC) = 0;
	/* Enable Fifos and set 8 data bits transmitted or received in a frame */
	*REG(PL011IMSC) = PL011_LCRH_FEN | PL011_LCRH_WLEN_8BIT;
	/* Enable Receive/Transmit */
	*REG(PL011CR) = 0x301;
	/* Receive interrupt mask */
	*REG(PL011IMSC) = (1 << 4);
}
