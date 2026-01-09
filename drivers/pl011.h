#ifndef _PL011_H
#define _PL011_H

void pl011_putc(char c);
void pl011_puts(char *s);
int pl011_getc(void);
void pl011_irq_handler(void);
void pl011_init(void);

#endif /* _PL011_H */
