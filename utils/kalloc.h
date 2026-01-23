#ifndef _KALLOC_H
#define _KALLOC_H

void kalloc_init(void);
void *alloc_one_page(void);
void free_one_page(void *p);

#endif /* _KALLOC_H */
