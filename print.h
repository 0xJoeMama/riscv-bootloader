#ifndef PRINT_H
#define PRINT_H

#include "atomic.h"
#include "types.h"

typedef struct {
  void (*print)(const char *);
  void (*printn)(const char *, usize n);
  void (*print_c)(char);
  int (*prepare)(void *arg);
  int (*flush)(void *arg);
  void *arg;
} sprinter;

typedef struct {
  enum {
    PRINT_OK = 0,
    PRINT_INVALID_FORMAT,
  } type;
} print_result;

int print_prepare(sprinter *);
int print_flush(sprinter *);

print_result print(sprinter *printer, const char *s);
print_result printn(sprinter *printer, const char *s, usize n);
print_result print_u64(u64 x, sprinter *printer);
print_result print_u32(u32 x, sprinter *printer);
print_result hprint_u64(u64 x, sprinter *printer);
print_result hprint_u32(u32 x, sprinter *printer);

print_result vprintf(sprinter *printer, const char *s, __builtin_va_list *args);
print_result printf(sprinter *printer, const char *s, ...);

#endif // PRINT_H
