#include "arch/arch.h"
#include "drivers/pl011.h"
#include "utils/printf.h"
#include "utils/xmalloc.h"
#include "utils/kalloc.h"
#include "arch/mm.h"

#include "test/stage2_mmu.h"

__attribute__((aligned(SZ_4K))) char sp_stack[SZ_4K * NCPU] = { 0 };

int main(void)
{
	pl011_init();
	printf("Hello, world! Now running in EL%d\n", get_el());

	/* Fast small memory alloc algorithm */
	xmalloc_init();
	/* Page memory alloc algorithm */
	kalloc_init();

	stage2_mmu_init();
	hyper_setup();

	test_create_vm_mapping();

	return 0;
}
