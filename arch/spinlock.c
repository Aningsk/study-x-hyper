#include "spinlock.h"
#include "printf.h"

void arch_spin_lock(spinlock_t *spinlock)
{
	if (spin_check(spinlock)) {
		abort("spinlock - %s is already held by core: %d",
		      spinlock->name, spinlock->coreid);
	}

	unsigned int tmp;
	asm volatile("1: ldaxr %w[tmp], %0\n"
		     "cbnz %w[tmp], 1b\n"
		     "stlxr %w[tmp], %w[one], %0\n"
		     "cbnz %w[tmp], 1b\n"
		     : "=Q"(spinlock->lock), [tmp] "=&r"(tmp)
		     : [one] "r"(1U)
		     : "memory");

	spinlock->coreid = coreid();
}

void arch_spin_unlock(spinlock_t *spinlock)
{
	if (!spin_check(spinlock)) {
		abort("spinlock - %s is unlocked by invalid coreid",
		      spinlock->name);
	}

	asm volatile("stlr wzr, %0" : "=Q"(spinlock->lock)::"memory");
	spinlock->coreid = INVALID_COREID;
}
