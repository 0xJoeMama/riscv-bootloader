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

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

struct fdt_prop_data {
  be_u32_t len;
  be_u32_t nameoff;
};

typedef struct fdt_reserve_entry {
  be_u64_t address;
  be_u64_t size;
} fdt_reserve_entry;

fdt_reserve_entry *get_reserved_mem(fdt_header *);
void print_mem_regions(fdt_reserve_entry *begin, sprinter *);

struct parsed_fdt {
  u32 poweroff_address;
};

struct parsed_fdt print_device_tree(fdt_header *, sprinter *);

#endif // FDT_H
