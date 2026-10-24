#include "endian.h"
#include "print.h"
#include "types.h"

#ifndef FDT_H
#define FDT_H

typedef struct fdt_header {
  be_u32_t magic;
  be_u32_t totalsize;
  be_u32_t off_dt_struct;
  be_u32_t off_dt_strings;
  be_u32_t off_mem_rsvmap;
  be_u32_t version;
  be_u32_t last_comp_version;
  be_u32_t boot_cpuid_phys;
  be_u32_t size_dt_strings;
  be_u32_t size_dt_struct;
} fdt_header;

typedef struct fdt_reserve_entry {
  be_u64_t address;
  be_u64_t size;
} fdt_reserve_entry;

fdt_reserve_entry *get_reserved_mem(fdt_header *);
void print_mem_regions(fdt_reserve_entry *begin, sprinter);
#endif // FDT_H
