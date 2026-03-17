#include "fw_dynamic.h"
#include "print.h"
#include "types.h"

#define SERIAL_ADDR ((char *)0x10000000LU)

void print_serial(const char *c) {
  while (*c) {
    *SERIAL_ADDR = *c++;
  }
}

__attribute__((noreturn)) extern void enter(u64 mhartid, void *device_tree,
                                            fw_dynamic_info *fwdi) {
  print_serial("Makis OS has been booted on hart ");
  print_u64(mhartid, print_serial);
  print_serial("\n");

  print_serial("Device tree located at ");
  print_u64((u64)device_tree, print_serial);
  print_serial("\n");

  print_fwdi(fwdi, print_serial);

  while (1)
    __asm__ volatile("wfi");
}
