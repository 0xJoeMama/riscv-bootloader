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

#define FDT_MAX_DEPTH 32
typedef struct {
  union {
    char *ptr;
    void *gen_ptr;
  };
  usize depth;
} fdt_ptr;

struct fdt_reserve_entry *fdt_get_reserved(struct fdt_header *);

const char *fdt_property_name(const struct fdt_header *hdr,
                              const struct fdt_prop_data *prop);
void *fdt_prop_value(struct fdt_prop_data *data, fdt_ptr *ptr);

fdt_ptr mk_fdt_ptr(struct fdt_header *hdr);
void fdt_goto_struct(struct fdt_header *hdr, fdt_ptr *ptr);

typedef enum {
  FDT_READ_PROPS = 1, // attempt to visit the properties of the current node
  FDT_STOP = 2,       // stop visiting the tree and return
  FDT_RECURSE = 4,    // attempt to visit child nodes of this node
} fdt_visit_flags;

struct fdt_visitor {
  fdt_visit_flags (*begin_node)(const char *name, void *);
  void (*prop)(const char *property_name, void *value, usize value_len, void *);
  fdt_visit_flags (*end_node)(void *);
};

void fdt_visit(struct fdt_header *, struct fdt_visitor *visitor, void *arg);

#define FDT_ROOT ""

#endif // FDT_H
