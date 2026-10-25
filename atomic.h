#ifndef ATOMIC_H
#define ATOMIC_H

#include "types.h"

#define atomic_int(type)                                                       \
  typedef struct {                                                             \
    volatile type __underlying;                                                \
  } atomic_##type;                                                             \
  type atomic_##type##_cmpxchg_aq_rl(atomic_##type *x, type set_to,            \
                                     type expected);                           \
  type atomic_##type##_add_aq_rl(atomic_##type *x, type addent)

atomic_int(u32);
atomic_int(u64);

#define atomic_read(x) (x.__underlying)

#define atomic_fence() asm __volatile__("fence")

#endif // ATOMIC_H
