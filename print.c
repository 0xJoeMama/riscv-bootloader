#include "print.h"
#include "assert.h"
#include "atomic.h"
#include "types.h"

print_result print(const char *s, sprinter *printer) {
  printer->print(s);
  return (print_result){.type = PRINT_OK};
}

print_result print_u64(u64 x, sprinter *printer) {
  char s[32] = {0};

  u64 i = 0;
  // create buffer in reverse
  do {
    s[i++] = '0' + x % 10;
    x /= 10;
  } while (x);

  // flip the buffer
  for (u64 j = 0; j < i / 2; j++) {
    s[j] ^= s[i - j - 1];
    s[i - j - 1] ^= s[j];
    s[j] ^= s[i - j - 1];
  }

  // finally print
  return print(s, printer);
}

print_result print_u32(u32 x, sprinter *printer) {
  char s[13] = {0};

  u64 i = 0;
  // create buffer in reverse
  do {
    s[i++] = '0' + x % 10;
    x /= 10;
  } while (x);

  // flip the buffer
  for (u32 j = 0; j < i / 2; j++) {
    s[j] ^= s[i - j - 1];
    s[i - j - 1] ^= s[j];
    s[j] ^= s[i - j - 1];
  }

  // finally print
  return print(s, printer);
}

static char hex_digit(u64 x) {
  if (x >= 0 && x <= 9)
    return '0' + x;

  switch (x) {
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
    return 'A' + (x - 10);

  default:
    return 'U';
  }
}

print_result hprint_u64(u64 x, sprinter *printer) {
  u64 mask = 0xFFULL << 60;

  char s[64 / 4 + 1] = {0};

  for (int i = 0; i < 64 / 4; i++) {
    s[i] = hex_digit((x & mask) >> (60 - 4 * i));
    mask >>= 4;
  }

  return print(s, printer);
}

print_result hprint_u32(u32 x, sprinter *printer) {
  u32 mask = 0xF << 28;

  char s[32 / 4 + 1] = {0};

  int i = 0;
  while (mask) {
    s[i] = hex_digit((x & mask) >> (28 - 4 * i));
    mask >>= 4;
    i++;
  }

  return print(s, printer);
}

int print_prepare(sprinter *printer) {
  int res;
  if ((res = printer->prepare(printer->arg)))
    return res;

  while (!atomic_u32_cmpxchg_aq_rl(&printer->prepared, 1, 0))
    ;

  atomic_fence();

  return 0;
}

int print_flush(sprinter *printer) {
  // TODO: instead of panicing on unprepared, please make sure you properly
  // handle this
  assert_eq(atomic_read(printer->prepared), 1);

  int res;
  printer->flush(printer->arg);

  atomic_fence();
  while (!atomic_u32_cmpxchg_aq_rl(&printer->prepared, 0, 1))
    ;

  return res;
}
