#include "assert.h"
#include "fdt.h"
#include "fw_dynamic.h"
#include "print.h"
#include "std.h"
#include "types.h"

#define SERIAL_ADDR ((char *)0x10000000LU)

void print_serial(const char *c) {
  while (*c) {
    *SERIAL_ADDR = *c++;
  }
}

never spin(void) {
  while (1)
    __asm__ volatile("wfi");
}

never panic(const char *s) {
  print_serial("PANIKK!!: ");
  print_serial(s);
  print_serial("\n");

  spin();
}

__attribute__((noreturn)) extern void
enter(u64 mhartid, fdt_header *device_tree, fw_dynamic_info *fwdi) {
  print_serial("Makis OS has been booted on hart ");
  print_u64(mhartid, print_serial);
  print_serial("\n");

  print_fwdi(fwdi, print_serial);

  print_serial("Device tree located at ");
  hprint_u64((u64)device_tree, print_serial);
  print_serial("\n");

  assert_eq(be_u32(device_tree->magic), 0xd00dfeed);
  assert_eq(be_u32(device_tree->version), 17);

  u32 tree_sz = be_u32(device_tree->totalsize);

  fdt_reserve_entry *rsv_mem = get_reserved_mem(device_tree);
  print_mem_regions(rsv_mem, print_serial);
  struct parsed_fdt parsed = print_device_tree(device_tree, print_serial);

  *(u16*)(u64)(0x0000000000100000ULL) = 0x5555;
  spin();
}
