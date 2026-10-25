#include "fw_dynamic.h"
#include "print.h"

static void print_info_line(const char *desc, u64 info, sprinter *printer) {
  print(desc, printer);
  hprint_u64(info, printer);
  print("\n", printer);
}

void print_fwdi(fw_dynamic_info *fwdi, sprinter *printer) {
  print_info_line("Magic: ", fwdi->magic, printer);
  print_info_line("Version: ", fwdi->version, printer);
  print_info_line("Next Addr: ", fwdi->next_addr, printer);
  print_info_line("Next Mode: ", fwdi->next_mode, printer);
  print_info_line("Options: ", fwdi->options, printer);
}
