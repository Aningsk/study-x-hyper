#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include "types.h"
#include "arch.h"

#define INVALID_COREID  (-1)

typedef struct spinlock {
	int coreid;
	unsigned int lock;
	char *name;
} spinlock_t;

static inline int spin_check(spinlock_t *spinlock)
{
	if (spinlock->lock && spinlock->coreid == coreid()) {
		/* spinlock is currently held by the calling CPU */
		return 1;
	} else {
		/* spinlock is unlocked or
		 * spinlock is not currently held by the calling CPU
		 */
		return 0;
	}
}

static inline void __arch_spinlock_init(spinlock_t *spinlock, char *name)
{
	spinlock->coreid = -1;
	spinlock->lock = 0;
	spinlock->name = name;
}

#define arch_spinlock_init(lock) __arch_spinlock_init(lock, #lock)

void arch_spin_lock(spinlock_t *spinlock);
void arch_spin_unlock(spinlock_t *spinlock);

#endif
