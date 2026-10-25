#include "string.h"

int streq(const char *a, const char *b) {
  for (; *a || *b; a++, b++)
    if (*a != *b)
      return 0;

  return 1;
}

int strstarts(const char *a, const char *prefix) {
  for (; *prefix; a++, prefix++)
    if (*a != *prefix)
      return 0;

  return 1;
}
