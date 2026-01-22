#ifndef _XMALLOC_H
#define _XMALLOC_H

#include "spinlock.h"
#include "types.h"
#include "arch.h"
#include <stdint.h>
#include <stddef.h>

#define CONFIG_BLK_SIZE 8192

#define BLK_SLICE_BIT 10
#define BLK_SLICE_SIZE (1 << BLK_SLICE_BIT)
#define BLK_SLICE_NUM (CONFIG_BLK_SIZE / BLK_SLICE_SIZE)
#define BLK_SLICE_TYPE_MAX BLK_SLICE_BIT
#define BLK_SLICE_SIZE_LOG2 BLK_SLICE_BIT

#define ALIGN_BIT 3
#define ALIGN_SIZE (1 << ALIGN_BIT)
#define ALIGN_MASK (ALIGN_SIZE - 1)
#define ALIGN_UP(a) (((a) + ALIGN_MASK) & ~ALIGN_MASK)

static inline uint8_t clz32(uint32_t x)
{
	uint8_t n;
	n = __builtin_clz(x);
	return n;
}

#define BLK_SIZE2TYPE(size) (32 - clz32((uint32_t)(size)-1))
#define BLK_TYPE2SIZE(type) (1 << type)

typedef struct {
	size_t blk_size;
	uint32_t freelist_cnt;
	uint32_t nofree_cnt;
	uintptr_t slice_cnt;
	uintptr_t slice_addr;
	size_t slice_offset;
	uintptr_t free_head;
} blk_list_t;

typedef struct {
	spinlock_t blk_lock;
	const char *pool_name;
	uintptr_t pool_start;
	uintptr_t pool_end;
	uint32_t slice_cnt;
	char slice_type[BLK_SLICE_NUM];
	blk_list_t blk_list[BLK_SLICE_TYPE_MAX];
} blk_pool_t;

int xmalloc_init(void);
void *xmalloc(uint32_t size);
int xfree(void *ptr);

#endif
