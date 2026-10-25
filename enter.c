#include "assert.h"
#include "atomic.h"
#include "fdt.h"
#include "fw_dynamic.h"
#include "print.h"
#include "std.h"
#include "types.h"

#define SERIAL_ADDR ((char *)0x10000000LU)

static void print_serial(const char *c) {
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

// the boot flag that blocks all CPUs except the boot CPU from initializing
// themselves this is initially set to 1 boot CPU switches it atomically to 0
// once initialization is done, it switches to 2 and all CPUs from now on are
// expected to initialize themselves with the secondary method
atomic_u64 boot = {1};

void *cpu_stacks;

static char stacks[4096 * 4];

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

  // TODO: here we need to allocate the CPU stacks on the physical memory that
  // we parsed from the fdt, for now support at MOST 4 CPUs
  cpu_stacks = stacks;
  atomic_fence(); // keep a store fence here to avoid reordering

  while (!atomic_u64_cmpxchg_aq_rl(&boot, 2, 0))
    ;

  spin();
}

__attribute__((noreturn)) extern void enter_secondary(u64 mhartid) {
  print_serial("Hart ");
  print_u64(mhartid, print_serial);
  print_serial(" online!");
  print_serial("\n");

  spin();
}
