typedef unsigned int u32;

#define SERIAL_ADDR ((char *)0x10000000LU)

void print_message(const char *c) {
  while (*c) {
    *SERIAL_ADDR = *c++;
  }
}

__attribute__((noreturn)) extern void enter(void) {
  print_message("Makis OS has been booted\n");

  while (1)
    ;
}
