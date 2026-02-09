#ifndef _GUEST_H
#define _GUEST_H

#include "types.h"

typedef struct guest {
    char *guest_name;
    u64   start_addr;
    u64   image_size;
} guest_t;

#endif /* _GUEST_H */
