#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

// page placed at the beginning of memory regions to hold metadata about the
// region
struct memory {
  _Alignas(4096) void *start; // TODO: this uses C11 style currently
  usize len;
  struct memory *next;
};

struct fdt_header;
struct memory *get_system_memory(struct fdt_header *);

#endif // MEMORY_H
