#ifndef ASSERT_H

#define ASSERT_H

#include "std.h"
#define get_line2(s) #s
#define get_line(s) get_line2(s)
#define assert(x)                                                              \
  (x                                                                           \
       ?: panic("FAILED ASSERTION AT " __FILE__                                \
                ":" get_line(__LINE__) " has failed spectacularly"))

#define assert_eq(a, b) assert((a) == (b))

#endif // ASSERT_H
