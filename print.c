#include "print.h"
#include "types.h"

void print_u64(u64 x, sprinter printer) {
  char s[12] = {0};

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
