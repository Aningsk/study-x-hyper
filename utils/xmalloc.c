#include "xmalloc.h"
#include "utils.h"
#include "printf.h"
#include "xlog.h"
#include <errno.h>

extern blk_pool_t blk_pool_start[];
blk_pool_t *sys_blk;

int blk_pool_init(blk_pool_t *pool, const char *name, void *pool_start,
		  size_t pool_size)
{
	uint32_t blk_type;
	uint8_t align_mask;
	blk_list_t *blk_list;

	if (pool == NULL || name == NULL || pool_start == NULL) {
		return -EINVAL;
	}

	memset(pool_start, 0, pool_size);

	align_mask = sizeof(uintptr_t) - 1u;

	if (((size_t)pool_start & align_mask) || (pool_size & align_mask)) {
		return -EINVAL;
	}

	if (pool_size % BLK_SLICE_SIZE) {
		return -EINVAL;
	}

	arch_spinlock_init(&pool->blk_lock);

	pool->pool_name = name;
	pool->pool_start = (uintptr_t)pool_start;
	pool->pool_end = (uintptr_t)(pool_start + pool_size);
	pool->slice_cnt = 0;

	memset(pool->slice_type, 0, sizeof(pool->slice_type));

	for (blk_type = 0; blk_type < BLK_SLICE_BIT; blk_type++) {
		blk_list = &pool->blk_list[blk_type];
		memset(blk_list, 0, sizeof(*blk_list));
		blk_list->blk_size = BLK_TYPE2SIZE(blk_type);
	}

	return 0;
}

int xmalloc_init(void)
{
	int ret = blk_pool_init(
		blk_pool_start, "xmalloc-pool",
		(void *)((size_t)blk_pool_start + ALIGN_UP(sizeof(blk_pool_t))),
		CONFIG_BLK_SIZE);
	if (!ret) {
		LOG_INFO("Xmalloc have been initialized: \n");
		LOG_INFO("Xmalloc: pool_start : 0x%x\n", blk_pool_start);
		LOG_INFO("Xmalloc: pool_size  : 0x%x\n", CONFIG_BLK_SIZE);
		sys_blk = blk_pool_start;
	}

	return ret;
}

void *blk_alloc(blk_pool_t *pool, uint32_t size)
{
	uint32_t blk_type;
	blk_list_t *blk_list = NULL;
	uintptr_t avail_blk = (uintptr_t)NULL;

	size = size < sizeof(uintptr_t) ? sizeof(uintptr_t) : size;
	/* Calculate the size belong to which blk_type */
	blk_type = BLK_SIZE2TYPE(size);

	/* Traverse blk_list */
	while (blk_type < BLK_SLICE_TYPE_MAX) {
		blk_list = &(pool->blk_list[blk_type]);

		/* There is free object in current blk_list */
		if ((avail_blk = blk_list->free_head) != (uintptr_t)NULL) {
			blk_list->free_head = *(uintptr_t *)avail_blk;
			blk_list->freelist_cnt--;
			break;
		}

		/* Determine if a new slice is needed.
		 * (the blk_type has not been assigned a slice,
		 * or the previously assigned slice has been used up)
		 */
		if (blk_list->slice_addr == 0 ||
		    blk_list->slice_offset == BLK_SLICE_SIZE) {
			/* The blk pool slice is used up, then use a larger blk */
			if (pool->slice_cnt == BLK_SLICE_NUM) {
				blk_type++;
				continue;
			}

			/* Get the new slice for this blk type */
			blk_list->slice_addr = pool->pool_start +
					       pool->slice_cnt * BLK_SLICE_SIZE;
			/* Record which blk type this slice belongs to */
			pool->slice_type[pool->slice_cnt] = blk_type;
			blk_list->slice_offset = 0;
			pool->slice_cnt++;
			blk_list->slice_cnt++;
		}

		/* cut blk from slice */
		avail_blk = blk_list->slice_addr + blk_list->slice_offset;
		blk_list->slice_offset += blk_list->blk_size;
		break;
	}

	if (blk_list) {
		if (avail_blk == (uintptr_t)0) {
			abort("xmalloc failed to get available blk");
		} else {
			blk_list->nofree_cnt++;
		}
	}

	return (void *)avail_blk;
}

int blk_free(blk_pool_t *pool, void *blk)
{
	uint32_t slice_idx;
	uint32_t blk_type;
	blk_list_t *blk_list;

	/* Find which slice the freed memory address belongs to */
	slice_idx = ((uintptr_t)blk - pool->pool_start) >> BLK_SLICE_SIZE_LOG2;
	if (slice_idx >= BLK_SLICE_NUM) {
		return -EPERM;
	}

	blk_type = pool->slice_type[slice_idx];
	if (blk_type >= BLK_SLICE_TYPE_MAX) {
		return -EPERM;
	}

	blk_list = &(pool->blk_list[blk_type]);
	*((uintptr_t *)blk) = blk_list->free_head;
	blk_list->free_head = (uintptr_t)blk;
	blk_list->nofree_cnt--;
	blk_list->freelist_cnt++;

	return 0;
}

int xmalloc_blk_free(blk_pool_t *pool, void *blk)
{
	int ret;
	if (pool == NULL || blk == NULL) {
		return -EINVAL;
	}

	arch_spin_lock(&pool->blk_lock);

	ret = blk_free(pool, blk);

	arch_spin_unlock(&pool->blk_lock);
	return ret;
}

void *xmalloc_blk_alloc(blk_pool_t *pool, uint32_t size)
{
	uintptr_t avail_blk;

	if (pool == NULL) {
		return NULL;
	}

	arch_spin_lock(&pool->blk_lock);

	avail_blk = (uintptr_t)blk_alloc(pool, size);

	arch_spin_unlock(&pool->blk_lock);

	return (void *)avail_blk;
}

void *xmalloc(uint32_t size)
{
	return xmalloc_blk_alloc(sys_blk, size);
}

int xfree(void *ptr)
{
	return xmalloc_blk_free(sys_blk, ptr);
}
