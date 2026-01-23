#ifndef _LAYOUT_H
#define _LAYOUT_H

/* Hypervisor load on this physical memory address */
#define HIMAGE_VADDR    0x40200000

#define PHYBASE         0x40000000
#define PHYSIZE         (512 * 1024 * 1024)
#define PHYEND          (PHYBASE + PHYSIZE)

#endif /* _LAYOUT_H */
