#ifndef _VM_H
#define _VM_H

#include "types.h"
#include "guest.h"

typedef struct vm_config {
    guest_t *guest_image;
    guest_t *guest_dtb;
    guest_t *guest_initrd;
    u64      entry_addr;
} vm_config_t;

#endif /* _VM_H */
