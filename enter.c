#include "assert.h"
#include "atomic.h"
#include "fdt.h"
#include "fw_dynamic.h"
#include "print.h"
#include "serial.h"
#include "std.h"
#include "types.h"

never spin(void) {
  while (1)
    __asm__ volatile("wfi");
}

never panic(const char *s) {
  // TODO: this always assumes the serial printer is PREPARED
  print("PANIKK!!: ", &serial);
  print(s, &serial);
  print("\n", &serial);

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
  serial_init();

  print_prepare(&serial);

  print("Makis OS has been booted on hart ", &serial);
  print_u64(mhartid, &serial);
  print("\n", &serial);

  print_fwdi(fwdi, &serial);

  print("Device tree located at ", &serial);
  hprint_u64((u64)device_tree, &serial);
  print("\n", &serial);

  assert_eq(be_u32(device_tree->magic), 0xd00dfeed);
  assert_eq(be_u32(device_tree->version), 17);

  u32 tree_sz = be_u32(device_tree->totalsize);

  fdt_reserve_entry *rsv_mem = get_reserved_mem(device_tree);
  print_mem_regions(rsv_mem, &serial);
  struct parsed_fdt parsed = print_device_tree(device_tree, &serial);

  print_flush(&serial);
  // TODO: here we need to allocate the CPU stacks on the physical memory that
  // we parsed from the fdt, for now support at MOST 4 CPUs
  cpu_stacks = stacks;
  atomic_fence(); // keep a store fence here to avoid reordering

  while (!atomic_u64_cmpxchg_aq_rl(&boot, 2, 0))
    ;

  spin();
}

__attribute__((noreturn)) extern void enter_secondary(u64 mhartid) {
  print_prepare(&serial);

  print("Hart ", &serial);
  print_u64(mhartid, &serial);
  print(" online!", &serial);
  print("\n", &serial);

  print_flush(&serial);

  spin();
}
