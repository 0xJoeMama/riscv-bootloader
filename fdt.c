#include "fdt.h"
#include "print.h"
#include "std.h"
#include "string.h"

void print_reserved_region(fdt_reserve_entry *e, sprinter *printer) {
  hprint_u64(be_u64(e->address), printer);
  print("-", printer);
  hprint_u64(be_u64(e->address) + be_u64(e->size), printer);
  print("\n", printer);
}

void print_mem_regions(fdt_reserve_entry *begin, sprinter *printer) {
  print("Memory regions:\n", printer);
  while (be_u64(begin->address) || be_u64(begin->size))
    print_reserved_region(begin++, printer);

  print("==============================\n", printer);
}

fdt_reserve_entry *get_reserved_mem(fdt_header *h) {
  u32 mem_offset = be_u32(h->off_mem_rsvmap);
  return (fdt_reserve_entry *)((char *)h + mem_offset);
}

static u32 chomp_be_u32(void **ptr) {
  u32 res = be_u32(*(be_u32_t *)*ptr);
  *ptr = (char *)(*ptr) + sizeof(be_u32_t);
  return res;
}

struct parsed_fdt print_device_tree(fdt_header *hdr, sprinter *printer) {
  u32 off_dt_struct = be_u32(hdr->off_dt_struct);

  void *ptr = ((char *)hdr + off_dt_struct);

  char *strings = (char *)hdr + be_u32(hdr->off_dt_strings);

  struct parsed_fdt res = {0};

  int grabbing_poweroff = 0;

  u32 type;
  while ((type = chomp_be_u32(&ptr)) != FDT_END) {
    switch (type) {
    case FDT_BEGIN_NODE: {
      // ptr here contains the name of the device
      print("Device Name: ", printer);
      print(ptr, printer);
      print("\n", printer);

      if (streq(ptr, "poweroff")) {
        grabbing_poweroff = 1;
      }

      // advance word by word until we find a word that contains at least one
      // null-terminator
      // set the ptr to be the beginning of the word right after this one
      for (char (*word)[4] = ptr;; word++) {
        for (int i = 0; i < sizeof(*word); i++) {
          if (!(*word)[i]) {
            ptr = word + 1;
            goto done;
          }
        }
      }

    done:
      break;
    }
    case FDT_END_NODE:
    case FDT_NOP:
      grabbing_poweroff = 0;
      continue;
    case FDT_PROP: {
      struct fdt_prop_data fdt_data = *(struct fdt_prop_data *)ptr;
      ptr = (char *)ptr + sizeof(fdt_data);

      char *property_name = strings + be_u32(fdt_data.nameoff);

      print("--- ", printer);
      print(property_name, printer);
      print(": ", printer);
      u32 len = be_u32(fdt_data.len);

      u32 padded_len = (len & (-4)) + 4 * (len % 4 ? 1 : 0);

      be_u32_t *value = ptr;

      if (streq(property_name, "value"))
        res.poweroff_address = be_u32(*value);

      for (int i = 0; i < padded_len / 4; i++, value++) {
        hprint_u32(be_u32(*value), printer);
        print(" ", printer);
      }

      print("\n", printer);

      ptr = (char *)ptr + padded_len;
    } break;
    default:
      panic("Invalid FDT property type\n");
    }
  }

out:
  print("<<<<-------->>>>\n", printer);

  return res;
}
