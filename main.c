#include "drivers/pl011.h"

#define SZ_4K 0x1000
#define NCPU 1

__attribute__((aligned(SZ_4K))) char sp_stack[SZ_4K * NCPU] = { 0 };

int main(void)
{
	pl011_init();
	pl011_puts("Hello, world!\n");

	return 0;
}
