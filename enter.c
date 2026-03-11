typedef unsigned long long u64;

#define SERIAL_ADDR ((char *)0x10000000LU)

void print_serial(const char *c) {
  while (*c) {
    *SERIAL_ADDR = *c++;
  }
}

__attribute__((noreturn)) extern void enter(u64 mhartid) {
  char s[12] = {0};

  u64 j = mhartid;
  u64 i = 0;
  do {
    s[i++] = '0' + j % 10;
    j /= 10;
  } while (j);

  print_serial("Makis OS has been booted on hart ");
  print_serial(s);
  print_serial("\n");

  while (1)
    ;
}
