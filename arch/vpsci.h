#ifndef _VPSCI_H
#define _VPSCI_H

#include "types.h"
#include "vcpu.h"

/* https://developer.aliyun.com/article/1205031 */
#define PSCI_VERSION 0x84000000
#define PSCI_MIGRATE_INFO_TYPE 0x84000006
#define PSCI_SYSTEM_OFF 0x84000008
#define PSCI_SYSTEM_RESET 0x84000009
#define PSCI_SYSTEM_CPUON 0xc4000003
#define PSCI_FEATURE 0x8400000a

u64 vpsci_trap_smc(vcpu_t *vcpu, u64 funid, u64 target_cpu, u64 entry_addr);
u64 smc_call(u64 funid, u64 target_cpu, u64 entry_addr);

#endif /* _VPSCI_H */
