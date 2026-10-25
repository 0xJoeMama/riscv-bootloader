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
    asm __volatile__("wfi");
}

never panic(const char *s) {
  // TODO: this always assumes the serial printer is PREPARED
  serial.print("PANIKK!!: ");
  serial.print(s);
  serial.print("\n");

  spin();
}

// the boot flag that blocks all CPUs except the boot CPU from initializing
// themselves this is initially set to 1 boot CPU switches it atomically to 0
// once initialization is done, it switches to 2 and all CPUs from now on are
// expected to initialize themselves with the secondary method
atomic_u64 boot = {1};
atomic_u32 cpus_up = {0};

void *cpu_stacks;

_Alignas(4096) static char stacks[4096 * 4];
extern char stack[4096];

static void mem_init(struct memory *mem) {
  if (!mem)
    panic("Did not locate any usable memory regions");

  for (struct memory *curr = mem; curr; curr = curr->next) {
    print("Memory region starting at ", &serial);
    hprint_u64((u64)curr->start - 4096, &serial);
    print(" (length = ", &serial);
    print_u64(curr->len / (1 << 20), &serial);
    print("MB)\n", &serial);
  }
}

extern never enter(u64 mhartid, struct fdt_header *device_tree,
                   fw_dynamic_info *fwdi) {
  serial_init();

  print_prepare(&serial);

  // stack sanity check
  u64 sp;
  asm volatile("mv %0, sp " : "=r"(sp));
  assert(sp >= (u64)stack && sp < (u64)stack + 4096);

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

  struct fdt_reserve_entry *rsv_mem = get_reserved_mem(device_tree);
  print_mem_regions(rsv_mem, &serial);
  struct parsed_fdt parsed = print_device_tree(device_tree, &serial);

  mem_init(parsed.first_mem);

  atomic_u32_add_aq_rl(&cpus_up, 1);

  print_flush(&serial);

  // TODO: here we need to allocate the CPU stacks on the physical memory that
  // we parsed from the fdt, for now support at MOST 4 CPUs
  cpu_stacks = stacks;
  atomic_fence(); // keep a store fence here to avoid reordering

  while (!atomic_u64_cmpxchg_aq_rl(&boot, 2, 0))
    ;

  while (!atomic_u32_cmpxchg_aq_rl(&cpus_up, 0, 4))
    ;

  *(u16 *)(0x0000000000100000ULL) = 0x5555;
  spin();
}

extern never enter_secondary(u64 mhartid) {
  print_prepare(&serial);

  // stack sanity check
  u64 sp;
  void *stack = &stacks[mhartid * 4096];
  asm volatile("mv %0, sp " : "=r"(sp));
  assert(sp >= (u64)stack && sp < (u64)stack + 4096);

  print("Hart ", &serial);
  print_u64(mhartid, &serial);
  print(" online!", &serial);
  print("\n", &serial);

  print_flush(&serial);

  atomic_u32_add_aq_rl(&cpus_up, 1);
  atomic_fence();

  spin();
}
