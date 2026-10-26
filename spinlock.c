#include "spinlock.h"
#include "atomic.h"

enum {
  SPINLOCK_UNLOCKED = 0,
  SPINLOCK_LOCKED = 1,
};

void spinlock_lock(Spinlock *lock) {
  while (atomic_u32_swap_aq(&lock->__locked, SPINLOCK_LOCKED) !=
         SPINLOCK_UNLOCKED)
    ;
}

void spinlock_unlock(Spinlock *lock) {
  atomic_u32_swap_rl(&lock->__locked, SPINLOCK_UNLOCKED);
}

int spinlock_held(Spinlock *lock) { return atomic_read(lock->__locked); }

