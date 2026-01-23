#include "arch/arch.h"
#include "drivers/pl011.h"
#include "utils/printf.h"
#include "utils/xmalloc.h"
#include "utils/kalloc.h"

__attribute__((aligned(SZ_4K))) char sp_stack[SZ_4K * NCPU] = { 0 };

int main(void)
{
	pl011_init();
	printf("Hello, world!\n");

	/* Fast small memory alloc algorithm */
	xmalloc_init();
	/* Page memory alloc algorithm */
	kalloc_init();

	return 0;
}
