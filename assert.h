#ifndef ASSERT_H

#define ASSERT_H

#include "std.h"

#define assert(x)                                                              \
  ((x)                                                                         \
       ?: panic("Failed assertion at function '%s' in %s:%U\n", __FUNCTION__,    \
                __FILE__, __LINE__))

#define assert_eq(a, b) assert((a) == (b))
#define assert_neq(a, b) assert((a) != (b))

#endif // ASSERT_H
