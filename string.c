#include "string.h"

int streq(const char *a, const char *b) {
  for (; *a && *b; a++, b++)
    if (*a != *b)
      return 0;

  return 1;
}
