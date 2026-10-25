#include "fdt.h"
#include "assert.h"
#include "print.h"
#include "std.h"
#include "string.h"
#include "symbols.h"

void print_reserved_region(struct fdt_reserve_entry *e, sprinter *printer) {
  hprint_u64(be_u64(e->address), printer);
  print("-", printer);
  hprint_u64(be_u64(e->address) + be_u64(e->size), printer);
  print("\n", printer);
}

void print_mem_regions(struct fdt_reserve_entry *begin, sprinter *printer) {
  print("Memory regions:\n", printer);
  while (be_u64(begin->address) || be_u64(begin->size))
    print_reserved_region(begin++, printer);

  print("==============================\n", printer);
}

struct fdt_reserve_entry *get_reserved_mem(struct fdt_header *h) {
  u32 mem_offset = be_u32(h->off_mem_rsvmap);
  return (struct fdt_reserve_entry *)((char *)h + mem_offset);
}

static u32 chomp_be_u32(fdt_ptr *ptr) {
  u32 res = be_u32(*(be_u32_t *)ptr->ptr);
  ptr->ptr += sizeof(be_u32_t);
  return res;
}

static struct fdt_prop_data chomp_prop_data(fdt_ptr *ptr) {
  struct fdt_prop_data *fdt_data = ptr->gen_ptr;
  ptr->ptr += sizeof(*fdt_data);
  return *fdt_data;
}

const char *fdt_prop_name(const struct fdt_header *hdr,
                          const struct fdt_prop_data *prop) {
  u32 string_offset = be_u32(hdr->off_dt_strings);
  char *strings = (char *)hdr + string_offset;
  return strings + be_u32(prop->nameoff);
}

void *fdt_chomp_value(struct fdt_prop_data *data, fdt_ptr *ptr) {
  u32 len = be_u32(data->len);
  u32 padded_len = (len & (-4)) + 4 * (len % 4 ? 1 : 0);
  be_u32_t *value = (be_u32_t *)ptr->ptr;
  ptr->ptr += padded_len;

  return value;
}

fdt_ptr mk_fdt_ptr(struct fdt_header *hdr) {
  return (fdt_ptr){.ptr = (char *)hdr};
}

void fdt_goto_struct(struct fdt_header *hdr, fdt_ptr *ptr) {
  ptr->ptr += be_u32(hdr->off_dt_struct);
}

static int is_see_in_region(void *ptr, usize len) {
  char *start;
  return see_start >= start && see_start < start + len;
}

static void add_memory_meta(struct memory **mem_head, void *ptr, usize len) {
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
                              sprinter *printer) {
  u64 start = (((u64)be_u32(region[0])) << 32) | be_u32(region[1]);
  usize len = (((u64)be_u32(region[2])) << 32) | be_u32(region[3]);
  void *ptr = (void *)start;

  if (!is_see_in_region(ptr, len)) {
    add_memory_meta(mem_head, ptr, len);
  } else {
    // add whole region from ptr up to see_start in case it's strictly inside
    // the region
    if (see_start > (char *)ptr)
      add_memory_meta(mem_head, ptr, see_start - (char *)ptr);

    // check if we have even 1 more byte after the SEE
    if (see_end + 1 < (char *)ptr + len)
      add_memory_meta(mem_head, see_end, (char *)ptr + len - see_end);
  }
}

static void read_memory_device(struct fdt_header *hdr, struct memory **mem_head,
                               fdt_ptr *ptr, sprinter *printer) {
  u32 type;
  while ((type = chomp_be_u32(ptr)) != FDT_END_NODE) {
    switch (type) {
    case FDT_PROP: {
      struct fdt_prop_data data = chomp_prop_data(ptr);
      const char *name = fdt_prop_name(hdr, &data);

      void *value = fdt_chomp_value(&data, ptr);

      if (streq(name, "device_type")) {
        assert(streq(value, "memory"));
      } else if (streq(name, "reg")) {
        u32 len = be_u32(data.len);

        // TODO: for now assume that the #address-cells and #size-cells
        // properties have the value of 2 every pair is a memory region
        u32 region_cnt = len / (4 * sizeof(be_u32_t));

        // every region contains 2 u32 that is the initial address and 2 u32
        // that is the length of the region in total 4 u32s per region

        be_u32_t *reg = value;
        for (u32 i = 0; i < region_cnt; i++)
          add_memory_region(mem_head, reg + i * 4, printer);
      } else if (streq(name, "initial-mapped-area")) {
      } else {
        panic("Unknown memory node property");
      }

    } break;

    case FDT_NOP:
      break;

    default:
      print("Invalid memory node property type ", printer);
      print_u32(type, printer);
      print("\n", printer);
      panic("Bad property type");
    }
  }
}

struct parsed_fdt print_device_tree(struct fdt_header *hdr, sprinter *printer) {
  u32 off_dt_struct = be_u32(hdr->off_dt_struct);

  fdt_ptr ptr = mk_fdt_ptr(hdr);
  fdt_goto_struct(hdr, &ptr);

  char *strings = (char *)hdr + be_u32(hdr->off_dt_strings);

  struct parsed_fdt res = {
      .first_mem = NULL,
  };

  u32 type;
  while ((type = chomp_be_u32(&ptr)) != FDT_END) {
    switch (type) {
    case FDT_BEGIN_NODE: {
      char *name = ptr.ptr;

      // advance word by word until we find a word that contains at least one
      // null-terminator
      // set the ptr to be the beginning of the word right after this one
      for (char (*word)[4] = ptr.gen_ptr;; word++) {
        for (int i = 0; i < sizeof(*word); i++) {
          if (!(*word)[i]) {
            ptr.gen_ptr = word + 1;
            goto done;
          }
        }
      }

    done:
      if (strstarts(name, "memory@")) {
        read_memory_device(hdr, &res.first_mem, &ptr, printer);
      } else {
        // ptr here contains the name of the device
        print("Device Name: ", printer);
        print(name, printer);
        print("\n", printer);
      }
    } break;
    case FDT_NOP:
    case FDT_END_NODE:
      continue;
    case FDT_PROP: {
      struct fdt_prop_data fdt_data = chomp_prop_data(&ptr);
      const char *property_name = fdt_prop_name(hdr, &fdt_data);

      print("--- ", printer);
      print(property_name, printer);
      print(": ", printer);

      be_u32_t *value = fdt_chomp_value(&fdt_data, &ptr);
      for (; (char *)value != ptr.ptr; value++) {
        hprint_u32(be_u32(*value), printer);
        print(" ", printer);
      }

      print("\n", printer);
    } break;
    default:
      print("Type ", printer);
      print_u32(type, printer);
      print(" is unknown\n", printer);
      panic("Invalid FDT property type\n");
    }
  }

out:
  print("<<<<-------->>>>\n", printer);

  return res;
}
