#include "pmp.h"
#include "assert.h"
#include "serial.h"
#include "types.h"

void setup_pmp() {
  protect_see();

  u64 pmpcfg;
  asm volatile("csrr %0, pmpcfg0" : "=r"(pmpcfg));
  pmp_t *cfgs = (pmp_t *)&pmpcfg;

  pmp_t cfg = cfgs[1];

  print(&emergency_serial, "Setting up PMP protection domains:\n");
  assert_eq(cfg.__zero, 0);
  printf(&emergency_serial, "Domain 1: Locked (%u), A (%u), RWX (%u%u%u)\n",
         cfg.locked, cfg.a, cfg.r, cfg.w, cfg.x);

  cfg = cfgs[2];
  printf(&emergency_serial, "Domain 2: Locked (%u), A (%u), RWX (%u%u%u)\n",
         cfg.locked, cfg.a, cfg.r, cfg.w, cfg.x);

  cfg = cfgs[3];
  printf(&emergency_serial, "Domain 3: Locked (%u), A (%u), RWX (%u%u%u)\n",
         cfg.locked, cfg.a, cfg.r, cfg.w, cfg.x);
}
