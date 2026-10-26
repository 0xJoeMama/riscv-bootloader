#ifndef SPINLOCK_H
#define SPINLOCK_H
#include "atomic.h"

typedef struct {
  atomic_u32 __locked;
} Spinlock;

void spinlock_lock(Spinlock *lock);
void spinlock_unlock(Spinlock * lock);
int spinlock_held(Spinlock *lock);

#ifdef ASSERT_H
#define assert_held(lock) assert(spinlock_held(lock))
#endif
#endif // SPINLOCK_H
