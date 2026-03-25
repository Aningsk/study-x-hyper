#include "arch.h"
#include "types.h"
#include "pl011.h"
#include "printf.h"

__attribute__((aligned(SZ_4K))) char sp_stack[SZ_4K * NCPU] = { 0 };

extern void _start();
extern u64 smc_call(u64 funid, u64 target_cpu, u64 entrypoint);

static void delay(void)
{
	for (volatile int i = 0; i < 100000000; i++)
		;
}

int vm_secondary_init(void)
{
	printf("This is core-1\n");

	while (1) {
		printf("Running on core-1\n");
		delay();
	}
	return 0;
}

int vm_primary_init(void)
{
	pl011_init();
	printf("\e[1;32mWelcome to GuestVM!\e[0m\n");
	printf("This is core-0\n");

	/* wakeup vcore 1 */
	smc_call((u64)0xc4000003, (u64)1, (u64)_start);

	while (1) {
		printf("Running on core-0\n");
		delay();
	}
	return 0;
}
