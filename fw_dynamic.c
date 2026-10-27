#include "fw_dynamic.h"
#include "print.h"

void print_fwdi(fw_dynamic_info *fwdi, sprinter *printer) {
  printf(printer, "Magic: %X\n", fwdi->magic);
  printf(printer, "Version: %u\n", fwdi->version);
  printf(printer, "Next Addr: %p\n", fwdi->next_addr);
  printf(printer, "Next Mode: %u\n", fwdi->next_mode);
  printf(printer, "Options: %X\n", fwdi->options);
}
