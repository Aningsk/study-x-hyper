#include "drivers/pl011.h"
#include "utils/printf.h"

#define SZ_4K 0x1000
#define NCPU 1

__attribute__((aligned(SZ_4K))) char sp_stack[SZ_4K * NCPU] = { 0 };

int main(void)
{
	unsigned long long mpidr = 0;

	pl011_init();

	printf("Hello, world!\n");

	asm volatile("mrs %0, mpidr_el1" : "=r" (mpidr));
	printf("> mpidr_el1 = 0x%x\n", mpidr);

	return 0;
}
