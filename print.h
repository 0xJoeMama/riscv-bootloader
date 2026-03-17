#ifndef PRINT_H
#define PRINT_H

#include "types.h"

typedef void (*sprinter)(const char *);
void print_u64(u64 x, sprinter printer);

#endif // PRINT_H
