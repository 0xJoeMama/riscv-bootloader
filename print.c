#include "print.h"
#include "types.h"

#define OK_RES ((print_result){.type = PRINT_OK})
#define RES(val) ((print_result){.type = val})

print_result print(sprinter *printer, const char *s) {
  printer->print(s);
  return OK_RES;
}

print_result printn(sprinter *printer, const char *s, usize n) {
  if (printer->printn) {
    printer->printn(s, n);
    return OK_RES;
  }

  for (usize i = 0; i < n; i++)
    printer->print_c(s[i]);

  return OK_RES;
}

print_result vprintf(sprinter *printer, const char *s,
                     __builtin_va_list *args) {
  const char *last_checkpoint = s;

  for (; *s; s++) {
    if (*s != '%')
      continue;

    printn(printer, last_checkpoint, s - last_checkpoint);

    s++;
    char c = *s;
    if (!c)
      return RES(PRINT_INVALID_FORMAT);

    switch (c) {
    case 'u':
      print_u32(__builtin_va_arg(*args, u32), printer);
      break;

    case 'U':
      print_u64(__builtin_va_arg(*args, u64), printer);
      break;

    case 's':
      print(printer, __builtin_va_arg(*args, const char *));
      break;

    case 'p':
      hprint_u64((u64) __builtin_va_arg(*args, void *), printer);
      break;

    case 'x':
      hprint_u32(__builtin_va_arg(*args, u32), printer);
      break;

    case 'X':
      hprint_u64(__builtin_va_arg(*args, u64), printer);
      break;

    default:
      return RES(PRINT_INVALID_FORMAT);
    };

    last_checkpoint = s + 1;
  }

  printn(printer, last_checkpoint, s - last_checkpoint);

  return OK_RES;
}

print_result printf(sprinter *printer, const char *s, ...) {
  __builtin_va_list va;
  __builtin_va_start(va, s);

  print_result res = vprintf(printer, s, &va);
  __builtin_va_end(va);

  return res;
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
  return print(printer, s);
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
  return print(printer, s);
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

  return print(printer, s);
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

  return print(printer, s);
}

int print_prepare(sprinter *printer) {
  return printer->prepare ? printer->prepare(printer->arg) : 0;
}

int print_flush(sprinter *printer) {
  return printer->flush ? printer->flush(printer->arg) : 0;
}
