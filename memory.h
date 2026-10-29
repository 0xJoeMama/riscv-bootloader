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

/* very bad function. ONLY USE ON VITAL PARTS OF CODE
 *
 * Guaranteed to make a continuous allocation in physical memory.
 * Memory allocated using this may not be freed or returned to the system in any
 * way. Memory allocated using this is NOT aligned in any way.
 *
 * Returns: a pointer to a region of size len or NULL if no such region exists
 */
void *mem_alloc_perma_object(struct memory *mem, usize len, usize alignment);

#endif // MEMORY_H
