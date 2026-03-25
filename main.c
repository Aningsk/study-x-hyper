#include "arch/arch.h"
#include "drivers/pl011.h"
#include "utils/xlog.h"
#include "utils/xmalloc.h"
#include "utils/kalloc.h"
#include "arch/mm.h"
#include "arch/vcpu.h"
#include "vm/vm.h"
#include "vm/guest.h"

__attribute__((aligned(SZ_4K))) char sp_stack[SZ_4K * NCPU] = { 0 };

extern guest_t guest_vm_image;

int hyp_secondary_init(void)
{
	LOG_NOTICE("Core-%d is up and running\n", coreid());

	stage2_mmu_init();
	hyper_setup();

	start_vcpu();

	while (1) {
		asm volatile("wfe");
	}

	return 0;
}

int hyp_primary_init(void)
{
	pl011_init();
	LOG_INFO("Hello, world! Now running in EL%d\n", get_el());
	LOG_NOTICE("Core-%d is up and running\n", coreid());

	/* Fast small memory alloc algorithm */
	xmalloc_init();
	/* Page memory alloc algorithm */
	kalloc_init();

	stage2_mmu_init();
	hyper_setup();

	pcpu_init();
	vcpu_init();
	LOG_INFO("pCPU/vCPU arrays initialized\n");

	vm_config_t guest_vm_cfg = {
		.guest_image = &guest_vm_image,
		.guest_dtb = NULL,
		.guest_initrd = NULL,
		.entry_addr = 0x80200000,
		.ram_size = 0x80000,
		.ncpu = 2,
	};

	create_guest_vm(&guest_vm_cfg);
	start_vcpu();

	while (1) {
		asm volatile("wfe");
	}

	return 0;
}
