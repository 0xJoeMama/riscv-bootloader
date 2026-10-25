#include "endian.h"
#include "print.h"
#include "types.h"

#ifndef FDT_H
#define FDT_H

struct fdt_header {
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
};

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

struct fdt_prop_data {
  be_u32_t len;
  be_u32_t nameoff;
};

struct fdt_reserve_entry {
  be_u64_t address;
  be_u64_t size;
};

typedef struct {
  union {
    char *ptr;
    void *gen_ptr;
  };
} fdt_ptr;

struct fdt_reserve_entry *get_reserved_mem(struct fdt_header *);
void print_mem_regions(struct fdt_reserve_entry *begin, sprinter *);

const char *fdt_property_name(const struct fdt_header *hdr,
                              const struct fdt_prop_data *prop);
void *fdt_prop_value(struct fdt_prop_data *data, fdt_ptr *ptr);

fdt_ptr mk_fdt_ptr(struct fdt_header *hdr);
void fdt_goto_struct(struct fdt_header *hdr, fdt_ptr *ptr);

// page placed at the beginning of memory regions to hold metadata about the
// region
struct memory {
  _Alignas(4096) void *start; // TODO: this uses C11 style currently
  usize len;
  struct memory *next;
};

struct parsed_fdt {
  struct memory *first_mem;
};

struct parsed_fdt print_device_tree(struct fdt_header *, sprinter *);

#endif // FDT_H
