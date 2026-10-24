#include "fdt.h"
#include "print.h"
#include "std.h"

void print_reserved_region(fdt_reserve_entry *e, sprinter print) {
  hprint_u64(be_u64(e->address), print);
  print("-");
  hprint_u64(be_u64(e->address) + be_u64(e->size), print);
  print("\n");
}

void print_mem_regions(fdt_reserve_entry *begin, sprinter print) {
  print("Memory regions:\n");
  while (be_u64(begin->address) || be_u64(begin->size))
    print_reserved_region(begin++, print);

  print("==============================");
}

fdt_reserve_entry *get_reserved_mem(fdt_header *h) {
  u32 mem_offset = be_u32(h->off_mem_rsvmap);
  return (fdt_reserve_entry *)((char *)h + mem_offset);
}
