#include "fdt.h"
#include "assert.h"
#include "std.h"

struct fdt_reserve_entry *fdt_get_reserved(struct fdt_header *h) {
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
  return (fdt_ptr){
      .ptr = (char *)hdr,
  };
}

void fdt_goto_struct(struct fdt_header *hdr, fdt_ptr *ptr) {
  ptr->ptr += be_u32(hdr->off_dt_struct);
}

void fdt_visit(struct fdt_header *hdr, struct fdt_visitor *visitor, void *arg) {
  u32 off_dt_struct = be_u32(hdr->off_dt_struct);

  fdt_ptr ptr = mk_fdt_ptr(hdr);
  fdt_goto_struct(hdr, &ptr);

  char *strings = (char *)hdr + be_u32(hdr->off_dt_strings);
  u32 type;

  int visit_props = 0;
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

    done: {
      fdt_visit_flags flags = visitor->begin_node(name, arg);
      visit_props = flags & FDT_READ_PROPS;
    }
    } break;
    case FDT_NOP:
      continue;
    case FDT_END_NODE:
      if (visitor->end_node && visitor->end_node(arg) == FDT_STOP)
        return;
      continue;
    case FDT_PROP: {
      struct fdt_prop_data fdt_data = chomp_prop_data(&ptr);
      const char *property_name = fdt_prop_name(hdr, &fdt_data);
      void *value = fdt_chomp_value(&fdt_data, &ptr);

      if (visit_props && visitor->prop)
        visitor->prop(property_name, value, be_u32(fdt_data.len), arg);
    } break;
    default:
      panic("Invalid FDT property type %u\n", type);
    }
  }
}
