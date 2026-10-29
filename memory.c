#include "memory.h"
#include "assert.h"
#include "endian.h"
#include "fdt.h"
#include "std.h"
#include "string.h"
#include "symbols.h"

struct reserved_region {
  char *start;
  char *end;
};

static struct reserved_region
get_reserved_region(void *ptr, usize len, struct fdt_reserve_entry *res) {
  char *start = ptr;

  // consider the SEE itself to be a reserved region
  if ((see_start >= start && see_start < start + len) ||
      (see_end > start && see_end <= start + len)) {
    return (struct reserved_region){
        .start = see_start,
        .end = see_end,
    };
  }

  for (; res->address.__underlying && res->size.__underlying; res++) {
    char *res_start = (char *)be_u64(res->address);
    char *res_end = (char *)(be_u64(res->address) + be_u64(res->size));

    if ((res_start >= start && res_start < start + len) ||
        (res_end > start && res_end <= start + len))
      return (struct reserved_region){
          .start = res_start,
          .end = res_end,
      };
  }

  return (struct reserved_region){0};
}

static void place_meta(struct memory **mem_head, void *ptr, usize len) {
  if (len < sizeof(struct memory))
    panic("Cannot place memory metadata. Region size too small...");

  // place a memory struct right the beginning of this region
  struct memory *m = ptr;
  m->start = m + 1;          // start AFTER the part we allocated
  m->len = len - sizeof(*m); // only count the remaining length
  m->next = *mem_head;

  *mem_head = m;
}

static void add_memory_region(struct memory **mem_head, be_u32_t *region,
                              struct fdt_reserve_entry *res) {
  u64 start = (((u64)be_u32(region[0])) << 32) | be_u32(region[1]);
  usize len = (((u64)be_u32(region[2])) << 32) | be_u32(region[3]);
  void *ptr = (void *)start;

  struct reserved_region reserved = get_reserved_region(ptr, len, res);
  // if no reserved region was found, it means we are safe to just claim the
  // whole region
  if (reserved.start == reserved.end)
    return place_meta(mem_head, ptr, len);

  // add whole region from ptr up to see_start in case it's strictly inside
  // the region
  if (reserved.start > (char *)ptr)
    place_meta(mem_head, ptr, reserved.start - (char *)ptr);

  // check if we have even 1 more byte after the SEE
  if (reserved.end + 1 < (char *)ptr + len)
    place_meta(mem_head, reserved.end, (char *)ptr + len - reserved.end);
}

struct memory_parser {
  struct memory *prev;
  struct fdt_reserve_entry *reserved_regions;
};

static fdt_visit_flags memory_begin_node(const char *name, void *arg) {
  if (streq(name, FDT_ROOT))
    return FDT_RECURSE;
  else if (strstarts(name, "memory@"))
    return FDT_READ_PROPS;
  else
    return 0;
}

static void memory_prop(const char *name, void *value, usize value_len,
                        void *arg) {
  struct memory_parser *parser = arg;
  struct memory **mem_head = &parser->prev;

  if (streq(name, "device_type")) {
    assert(streq(value, "memory"));
  } else if (streq(name, "reg")) {
    // TODO: for now assume that the #address-cells and #size-cells
    // properties have the value of 2 every pair is a memory region
    u32 region_cnt = value_len / (4 * sizeof(be_u32_t));

    // every region contains 2 u32 that is the initial address and 2 u32
    // that is the length of the region in total 4 u32s per region

    be_u32_t *reg = value;
    for (u32 i = 0; i < region_cnt; i++)
      add_memory_region(mem_head, reg + i * 4, parser->reserved_regions);
  } else if (streq(name, "initial-mapped-area")) {
    panic("Not implemented yet");
  } else {
    panic("Unknown memory node property");
  }
}

static fdt_visit_flags memory_end_node(void *arg) {
  struct memory_parser *parser = arg;
  return 0;
}

static struct fdt_visitor memory_visitor = {
    .begin_node = memory_begin_node,
    .prop = memory_prop,
    .end_node = memory_end_node,
};

struct memory *get_system_memory(struct fdt_header *hdr) {
  struct memory_parser parser = {
      .prev = NULL,
      .reserved_regions = fdt_get_reserved(hdr),
  };

  fdt_visit(hdr, &memory_visitor, &parser);

  assert_neq(parser.prev, NULL);
  return parser.prev;
}

static void *get_first_aligned_address(struct memory *mem, usize alignment) {
  usize align_mask = alignment - 1;
  int already_aligned = ((usize)mem->start & align_mask) ? 1 : 0;

  void *old_start = mem->start;
  void *new_start;

  if (!already_aligned) {
    new_start = (void *)((usize)old_start & ~align_mask) + alignment;
    if (new_start >= old_start + mem->len)
      return NULL;
  } else {
    new_start = old_start;
  }

  mem->start = new_start;
  mem->len -= new_start - old_start;

  return new_start;
}

void *mem_alloc_perma_object(struct memory *mem, usize len, usize alignment) {
  for (struct memory *m = mem; m; m = m->next) {
    void *start;
    if ((start = get_first_aligned_address(m, alignment))) {
      void *res = start;
      m->start += len;
      m->len -= len;

      return res;
    }
  }

  return NULL;
}
