#ifndef _MM_H
#define _MM_H

#include "types.h"

/*
 *  48 bit Virtual Address
 *
 *     48    39 38    30 29    21 20    12 11       0
 *    +--------+--------+--------+--------+----------+
 *    | level0 | level1 | level2 | level3 | page off |
 *    +--------+--------+--------+--------+----------+
 *
 */
#define PINDEX(level, ipa)    (((ipa) >> (39 - (level * 9))) & 0x1FFUL)

/* bit[1]: The type of this page table entry
 * 0 - block descriptor
 * 1 - table descriptor
 *
 * bit[0]: The page table is valid
 * 0 - The page table entry is invalid
 * 1 - The page table entry is valid
 */
#define PTE_VALID   0x1 /* Table/Page Description Valid */
#define PTE_TABLE   0x2 /* Level 0,1,2 Table Description */
#define PTE_V       0x3 /* Page Description and valid */
#define PTE_AF      (1 << 10) /* Access Flag */

/* Get the next-level table address [47:12] */
#define PTE_PA(pte) ((u64)(pte) & 0xFFFFFFFFF000UL)

/* AArch64 Memory Model Feature Register 0 */
#define PA_RANGE(n)   ((n) & 0xFUL)

/* VTCR_EL2: Virtuaization Translation Control Register */
#define VTCR_T0SZ(n)  ((n) & 0x3FUL) /* IPA region size is 2(64-T0SZ) bytes */
#define VTCR_SL0(n)   (((n) & 0x3UL) << 6)  /* Starting level of the stage 2 translation lookup */
#define VTCR_SH0(n)   (((n) & 0x3UL) << 12) /* Shareability attribute */
#define VTCR_TG0(n)   (((n) & 0x3UL) << 14) /* Granule size */
#define VTCR_PS(n)    (((n) & 0x7UL) << 16) /* Physical address Size for the second stage of translation */
#define VTCR_NSW      (1UL << 29)
#define VTCR_NSA      (1UL << 30)

/* Memory Attribute */
#define DEVICE_nGnRnE_INDEX 0x0
#define NORMAL_NC_INDEX     0x1

#define DEVICE_nGnRnE       0x0    /* Device-nGnRnE memory */
#define NORMAL_NC           0x44   /* Normal memory, Inner/Outer Non-cacheable */

/* Stage 2 attribute */
#define S2PTE_S2AP(ap)      (((ap) & 3) << 6)
#define S2PTE_RO            S2PTE_S2AP(1)
#define S2PTE_WO            S2PTE_S2AP(2)
#define S2PTE_RW            S2PTE_S2AP(3)
#define S2PTE_ATTR(attr)    (((attr) & 7) << 2)
#define S2PTE_NORMAL        S2PTE_ATTR(NORMAL_NC_INDEX)
#define S2PTE_DEVICE        S2PTE_ATTR(DEVICE_nGnRnE_INDEX)

/* Hypervisor Configuration Register */
#define HCR_VM              (1UL << 0)   /* EL1&0 stage 2 address translation enabled. */
#define HCR_SWIO            (1UL << 1)   /* Data cache invalidate by set/way instructions in EL1 */
#define HCR_FMO             (1UL << 3)   /* Physical FIQ Routing */
#define HCR_IMO             (1UL << 4)   /* Physical IRQ Routing */
#define HCR_RW              (1UL << 31)  /* The Execution state for EL1 is AArch64 */
#define HCR_TSC             (1UL << 19)  /* Trap SMC instructions to EL2 */

void stage2_mmu_init(void);
void hyper_setup(void);
void create_guest_mapping(u64 *pgt, u64 va, u64 pa, u64 size, u64 mattr);

#endif /* _VMM_H */
