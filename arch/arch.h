#ifndef _ARCH_H
#define _ARCH_H

#include "layout.h"
#include "types.h"

#define NCPU 1
#define SZ_4K 0x1000
#define PAGE_SIZE SZ_4K

/* Read a sys_reg to val */
#define __read_sysreg(val, reg) asm volatile("mrs %0, " #reg : "=r"(val))
#define read_sysreg(val, reg) __read_sysreg(val, reg)

#define __write_sysreg(reg, val) asm volatile("msr " #reg ", %0" : : "r"(val))
#define write_sysreg(reg, val)                                                 \
	do {                                                                   \
		u64 x = (u64)(val);                                            \
		__write_sysreg(reg, x);                                        \
	} while (0)

#define isb() asm volatile("isb");
#define dsb(sy) asm volatile("dsb " #sy);

#define irq_enable asm volatile("msr daifclr, #2" ::: "memory")
#define irq_disable asm volatile("msr daifset, #2" ::: "memory")

/* SPSR_EL2 */
#define SPSR_M(n) (n & 0xf)
#define SPSR_DAIF (0xf << 6)

static inline int coreid()
{
	int val;
	read_sysreg(val, mpidr_el1);
	return val & 0xf;
}

static inline void flush_tlb()
{
	dsb(ishst);
	asm volatile("tlbi vmalls12e1");
	dsb(ish);
	isb();
}

static inline u8 get_el(void)
{
	u64 el;
	read_sysreg(el, CurrentEL);
	return (el >> 2) & 0x3;
}

#endif
