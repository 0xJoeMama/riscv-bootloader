#include "types.h"

#ifndef STD_H
#define STD_H

#define never __attribute__((noreturn)) void

never spin(void);
never panic(const char *s);

#define NULL ((void*)0)
#endif // STD_H

