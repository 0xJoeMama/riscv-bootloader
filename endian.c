#include "endian.h"

static u64 flip_bytes_64(u64 x) {
  u64 res = 0;
  for (int i = 0; i < 8; i++)
    res |= (x & (0xff << (i * 8))) >> (8 - i - 1) * 8;

  return res;
}

static u32 flip_bytes_32(u32 x) {
  return ((x & 0xFF) << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) |
         (x >> 24);
}

// RV64I is little-endian so conversions to little-endian are no-op

// big-endian -> u32
u32 be_u32(struct be_u32 be) { return flip_bytes_32(be.__underlying); }
// little-endian -> u32 (no-op)
u32 le_u32(struct le_u32 be) { return be.__underlying; }

// big-endian -> u64
u64 be_u64(struct be_u64 be) { return flip_bytes_64(be.__underlying); }
// little-endian -> u64 (no-op)
u64 le_u64(struct le_u64 be) { return be.__underlying; }
