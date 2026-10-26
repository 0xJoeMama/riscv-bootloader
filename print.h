#ifndef PRINT_H
#define PRINT_H

#include "atomic.h"
#include "types.h"

typedef struct {
  void (*print)(const char *);
  void (*print_c)(char);
  int (*prepare)(void *arg);
  int (*flush)(void *arg);
  void *arg;
  atomic_u32 prepared;
} sprinter;

typedef struct {
  enum {
    PRINT_OK = 0,
  } type;
} print_result;

int print_prepare(sprinter *);
int print_flush(sprinter *);

print_result print(const char *s, sprinter *printer);
print_result print_u64(u64 x, sprinter *printer);
print_result print_u32(u32 x, sprinter *printer);
print_result hprint_u64(u64 x, sprinter *printer);
print_result hprint_u32(u32 x, sprinter *printer);

#endif // PRINT_H
