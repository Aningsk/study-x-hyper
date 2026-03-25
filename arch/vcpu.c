#include "types.h"
#include "arch.h"
#include "vcpu.h"
#include "spinlock.h"
#include "printf.h"
#include "xlog.h"

pcpu_t pcpus[NCPU];
vcpu_t vcpus[NCPU];
static spinlock_t vcpus_lock;

/* Acquire the pcpu according to current core id */
pcpu_t *cur_pcpu()
{
	int id = coreid();
	return &pcpus[id];
}

/* Initialize the physical cpu array
 * core 0 ---> pcpus[0]
 * core 1 ---> pcpus[1]
 * ...
 */
void pcpu_init()
{
	for (int i = 0; i < NCPU; i++) {
		pcpus[i].cpuid = i;
		pcpus[i].vcpu = NULL;
	}
	return;
}

void vcpu_init()
{
	arch_spinlock_init(&vcpus_lock);
	for (int i = 0; i < NCPU; i++) {
		vcpus[i].state = VCPU_UNUSED;
	}
	return;
}

static vcpu_t *vcpu_alloc()
{
	arch_spin_lock(&vcpus_lock);
	for (vcpu_t *vcpu = vcpus; vcpu < &vcpu[NCPU]; vcpu++) {
		if (vcpu->state == VCPU_UNUSED) {
			vcpu->state = VCPU_ALLOCED;
			arch_spin_unlock(&vcpus_lock);
			return vcpu;
		}
	}
	arch_spin_unlock(&vcpus_lock);
	return NULL;
}

vcpu_t *create_vcpu(vm_t *vm, int vcpuid, u64 entry)
{
	vcpu_t *vcpu = vcpu_alloc();
	if (vcpu == NULL) {
		abort("Unable to alloc a vcpu");
	}

	vcpu->core_name = "Cortex-A72";
	vcpu->vm = vm;
	vcpu->cpuid = vcpuid;
	vcpu->regs.spsr = SPSR_M(5) | SPSR_DAIF; /* used to set the spsr_el2 */
	vcpu->regs.elr = entry; /* used to set the elr_el2 */

	vcpu->sys_regs.mpidr_el1 = vcpuid; /* used to fake the mpidr_el1 */
	vcpu->sys_regs.midr_el1 =
		0x410FD081; /* used to fake the core to cortex-a72 */

	return vcpu;
}

static void restore_sysreg(vcpu_t *vcpu)
{
	write_sysreg(spsr_el1, vcpu->sys_regs.spsr_el1);
	write_sysreg(elr_el1, vcpu->sys_regs.elr_el1);
	write_sysreg(vmpidr_el2, vcpu->sys_regs.mpidr_el1);
	write_sysreg(vpidr_el2, vcpu->sys_regs.midr_el1);
	write_sysreg(sp_el0, vcpu->sys_regs.sp_el0);
	write_sysreg(sp_el1, vcpu->sys_regs.sp_el1);
	write_sysreg(ttbr0_el1, vcpu->sys_regs.ttbr0_el1);
	write_sysreg(ttbr1_el1, vcpu->sys_regs.ttbr1_el1);
	write_sysreg(tcr_el1, vcpu->sys_regs.tcr_el1);
	write_sysreg(vbar_el1, vcpu->sys_regs.vbar_el1);
	write_sysreg(sctlr_el1, vcpu->sys_regs.sctlr_el1);
	write_sysreg(cntv_ctl_el0, vcpu->sys_regs.cntv_ctl_el0);
	write_sysreg(cntv_tval_el0, vcpu->sys_regs.cntv_tval_el0);
	write_sysreg(cntfrq_el0, vcpu->sys_regs.cntfrq_el0);
}

extern void switch_out(void);

static void switch_vcpu(vcpu_t *vcpu)
{
	cur_pcpu()->vcpu = vcpu;
	/* tpidr_el2 saves current vcpu addr, used to get vcpu in context */
	write_sysreg(tpidr_el2, vcpu);

	vcpu->state = VCPU_RUNNING;
	/* set stage-2 MMU Page table base addr */
	write_sysreg(vttbr_el2, vcpu->vm->vttbr);
	flush_tlb();
	/* set EL1/EL0 system register state */
	restore_sysreg(vcpu);
	isb();
	/* switch to EL1 */
	switch_out();
}

void start_vcpu()
{
	int id = coreid();
	vcpu_t *vcpu = &vcpus[id];
	if (vcpu->state != VCPU_READY) {
		abort("vCPU is not ready");
	}

	LOG_INFO("pCPU-%d: starting vCPU-%d\n", id, vcpu->cpuid);
	switch_vcpu(vcpu);
}
