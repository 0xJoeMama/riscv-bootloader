#include "print.h"
#include "types.h"

#define MAX_U64_LEN 32

void print_u64(u64 x, sprinter printer) {
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
  printer(s);
}

void print_u32(u32 x, sprinter printer) {
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
  printer(s);
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

void hprint_u64(u64 x, sprinter print) {
  u64 mask = 0xFFULL << 60;

  char s[64 / 4 + 1] = {0};

  for (int i = 0; i < 64 / 4; i++) {
    s[i] = hex_digit((x & mask) >> (60 - 4 * i));
    mask >>= 4;
  }

  print(s);
}

void hprint_u32(u32 x, sprinter print) {
  u32 mask = 0xF << 28;

  char s[32 / 4 + 1] = {0};

  int i = 0;
  while (mask) {
    s[i] = hex_digit((x & mask) >> (28 - 4 * i));
    mask >>= 4;
    i++;
  }

  print(s);
}
