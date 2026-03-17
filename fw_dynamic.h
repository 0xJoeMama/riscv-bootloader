#ifndef FW_DYNAMIC_H
#define FW_DYNAMIC_H

#include "print.h"
#include "types.h"

typedef struct {
  u64 magic;
  u64 version;
  u64 next_addr;
  u64 next_mode;
  u64 options;
} fw_dynamic_info;

void print_fwdi(fw_dynamic_info *fwdi, sprinter printer);

#endif // FW_DYNAMIC_H
