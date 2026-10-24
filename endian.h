#ifndef ENDIAN_H
#define ENDIAN_H

#include "types.h"

#define endian_int(type)                                                       \
  typedef struct be_##type be_##type##_t;                                      \
  typedef struct le_##type le_##type##_t;                                      \
  struct be_##type {                                                           \
    type __underlying;                                                         \
  };                                                                           \
  struct le_##type {                                                           \
    type __underlying;                                                         \
  };                                                                           \
  type be_##type(struct be_##type);                                            \
  type le_##type(struct le_##type);                                            \
  struct be_##type type##_be(type);                                            \
  struct le_##type type##_le(type)

endian_int(u16);
endian_int(u32);
endian_int(u64);

#endif // ENDIAN_H
