#ifndef CPU_H
#define CPU_H

#include "types.h"

struct cpu {
  u32 mhartid;
  u64 freq;
  u64 timebase_freq;
  const char *mmu_type;
  usize mmu_type_len;
  struct cpu *next;
};

struct memory;
struct fdt_header;
struct cpu *get_system_cpus(struct fdt_header *hdr, struct memory *mem);

extern void *cpu_stacks;
usize setup_cpus(struct cpu *cpus, struct memory *mem);

#endif // CPU_H
