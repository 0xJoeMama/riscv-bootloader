#include "cpu.h"
#include "assert.h"
#include "fdt.h"
#include "memory.h"
#include "serial.h"
#include "std.h"
#include "string.h"

struct cpu_visit_arg {
  int is_visiting_cpu;
  struct cpu *cpu;
  struct memory *mem;
};

static fdt_visit_flags cpu_begin_node(const char *name, void *arg) {
  struct cpu_visit_arg *varg = arg;

  if (streq(name, "cpus") || streq(name, FDT_ROOT)) {
    return FDT_RECURSE;
  } else if (strstarts(name, "cpu@")) {
    varg->is_visiting_cpu = 1;
    struct cpu *cpu = mem_alloc_perma_object(varg->mem, sizeof(struct cpu), 4);
    if (!cpu)
      panic("Failed to allocate metadata for CPU devices\n");

    cpu->next = varg->cpu;
    varg->cpu = cpu;

    return FDT_READ_PROPS;
  } else {
    return 0;
  }
}
void cpu_prop(const char *property_name, void *value, usize value_len,
              void *arg) {
  struct cpu_visit_arg *varg = arg;

  if (!varg->is_visiting_cpu)
    assert(0); // TODO: this is just here to check if we properly handle this

  if (streq(property_name, "device_type")) {
    assert(streq(value, "cpu"));
  } else if (streq(property_name, "clock-frequency")) {
    varg->cpu->freq = be_u32(*(be_u32_t *)value);
  } else if (streq(property_name, "timebase-frequency")) {
    varg->cpu->timebase_freq = be_u64(*(be_u64_t *)value);
  } else if (streq(property_name, "reg")) {
    varg->cpu->mhartid = be_u32(*(be_u32_t *)value);
  } else if (streq(property_name, "mmu-type")) {
    varg->cpu->mmu_type = value;
    varg->cpu->mmu_type_len = value_len;
  }
}

fdt_visit_flags cpu_end_node(void *arg) {
  struct cpu_visit_arg *varg = arg;
  varg->is_visiting_cpu = 0;
  return 0;
}

struct fdt_visitor cpu_visitor = {
    .begin_node = cpu_begin_node,
    .prop = cpu_prop,
};

struct cpu *get_system_cpus(struct fdt_header *hdr, struct memory *mem) {
  struct cpu_visit_arg arg = {
      .is_visiting_cpu = 0,
      .cpu = NULL,
      .mem = mem,
  };

  fdt_visit(hdr, &cpu_visitor, &arg);

  return arg.cpu;
}

void *cpu_stacks;

usize setup_cpus(struct cpu *cpus, struct memory *mem) {
  usize cpu_cnt = 0;
  for (struct cpu *cpu = cpus; cpu; cpu = cpu->next, cpu_cnt++)
    ;

  cpu_stacks = mem_alloc_perma_object(mem, cpu_cnt * 4096, 4096);
  atomic_fence("w, rw"); // keep a store fence here to avoid reordering

  if (!cpu_stacks)
    panic("Failed to allocate stacks for %U cpus\n", cpu_cnt);

  printf(&serial, "Located %U harts\n", cpu_cnt);

  return cpu_cnt;
}
