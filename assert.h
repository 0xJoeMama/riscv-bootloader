#ifndef ASSERT_H

#define ASSERT_H

#include "std.h"

#define get_line2(s) #s
#define get_line(s) get_line2(s)
#define assert_eq(a, b) ((a) == (b) ?: panic("ASSERTION AT " __FILE__ ":" get_line(__LINE__) " has failed spectacularly"))

#endif // ASSERT_H
