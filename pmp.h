#ifndef PMP_H
#define PMP_H

typedef enum {
  PMP_ALLOW_READ = 1,
  PMP_ALLOW_WRITE = 2,
  PMP_ALLOW_EX = 4,
} pmp_permission_flags;

typedef enum {
  PMP_OFF = 0,
  PMP_TOR = 1 << 3,   // top of range
  PMP_NA4 = 2 << 3,   // naturally aligned four byte region
  PMP_NAPOT = 3 << 3, // naturally aligned power-of-2 region, >= 8 bytes
} pmp_matching_flags;

// do not allow further modification of this physical memory region
#define PMP_LOCK (1 << 7)

typedef struct {
  char r : 1;
  char w : 1;
  char x : 1;
  char a : 2;
  char __zero : 2;
  char locked : 1;
} pmp_t;

// sets up protection on the memory region occupied by the SEE itself
// This allows us to be able to detect memory errors, even in M-mode which makes
// me happy
void setup_pmp();
void protect_see();

#endif // PMP_H
