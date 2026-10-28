#include "assert.h"
#include "atomic.h"
#include "fdt.h"
#include "fw_dynamic.h"
#include "memory.h"
#include "pmp.h"
#include "print.h"
#include "serial.h"
#include "std.h"
#include "types.h"

never spin(void) {
  while (1)
    asm volatile("wfi");
}

never panic(const char *s, ...) {
  __builtin_va_list va;

  // this is the only case where emergency serial is used currently
  // REMINDER: this bypasses all concurrency safety for the sake of giving *some
  // sort* of response as to the state of the machine
  print(&emergency_serial, "##################################################"
                           "##################################\n");
  printf(&emergency_serial, "PANIC! A failing state has been reached. Aborting "
                            "execution before further mistakes...\n");
  __builtin_va_start(va, s);
  vprintf(&emergency_serial, s, &va);
  __builtin_va_end(va);

  print(&emergency_serial, "##################################################"
                           "##################################\n");
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

static void sanity_check_stack(void *stack) {
  // stack sanity check
  u64 sp;
  asm volatile("mv %0, sp " : "=r"(sp));
  assert(sp >= (u64)stack && sp < (u64)stack + 4096);
}

static void mem_init(struct memory *mem) {
  if (!mem)
    panic("Did not locate any usable memory regions");

  for (struct memory *curr = mem; curr; curr = curr->next)
    printf(&serial, "Memory region starting at %p (length = %U MB %U kB)\n",
           curr->start, curr->len >> 20, (curr->len & (1 << 20) - 1) >> 10);
}

extern never enter(u64 mhartid, struct fdt_header *device_tree,
                   fw_dynamic_info *fwdi) {
  // check that our sp values are within our alloted stack
  sanity_check_stack(stack);

  // protect the memory regions that we have been placed on
  // this is used to save us FROM OURSELVES primarily and not from others
  setup_pmp();

  // TODO: currently this assumes the location of a UART interface
  print_prepare(&serial);

  // let everyone know we are here
  printf(&serial, "MakOS has been booted on hart %U\n", mhartid);
  print_fwdi(fwdi, &serial);

  printf(&serial, "Device tree located at %p\n", device_tree);

  // assert a valid device tree
  assert_eq(be_u32(device_tree->magic), 0xd00dfeed);
  assert_eq(be_u32(device_tree->version), 17);

  // we HUNGER for the memories
  struct memory *mem = get_system_memory(device_tree);
  mem_init(mem);

  atomic_u32_add_aq_rl(&cpus_up, 1);

  print_flush(&serial);

  // TODO: here we need to allocate the CPU stacks on the physical memory that
  // we parsed from the fdt, for now support at MOST 4 CPUs
  cpu_stacks = stacks;
  atomic_fence("w, rw"); // keep a store fence here to avoid reordering

  atomic_u64_cmpxchg_aq_rl(&boot, 2, 0);

  while (!atomic_u32_cmpxchg_aq_rl(&cpus_up, 0, 4))
    ;

  *(u16 *)(0x0000000000100000ULL) = 0x5555;
  spin();
}

extern never enter_secondary(u64 mhartid) {
  // stack sanity check
  sanity_check_stack(&stacks[mhartid * 4096]);

  print_prepare(&serial);
  printf(&serial, "Hart %U is now online\n", mhartid);
  print_flush(&serial);

  atomic_u32_add_aq_rl(&cpus_up, 1);

  spin();
}
