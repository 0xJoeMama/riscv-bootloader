# implement the functions of atomic ints in assembly
# it's the only way

.section .text
# this has the semantics of cmpxchg STRONG as it cannot fail spuriously. 
# It may be beneficial to make a version that *can* fail spuriously.
# u64 atomic_u64_cmpxchg_aq_rl(atomic_u64 *x, type set_to, type expected);
# a0: x
# a1: set_to
# a2: expected
# -> a0: 0 if the 
.global atomic_u64_cmpxchg_aq_rl
atomic_u64_cmpxchg_aq_rl:
_load_value_1:
  lr.d.aq t0, 0(a0) # load double word into t0 and insert new entry into the reservation table
  bne t0, a2, _fail_1

  sc.d.rl t0, a1, 0(a0)
  # reservation was invalidated
  bnez t0, _load_value_1

  li a0, 1
  ret

_fail_1:
  li a0, 0
  ret

  
# this has the semantics of cmpxchg STRONG as it cannot fail spuriously. 
# It may be beneficial to make a version that *can* fail spuriously.
# u32 atomic_u32_cmpxchg_aq_rl(atomic_u32 *x, type set_to, type expected);
# a0 -> x
# a1 -> set_to
# a2 -> expected
.global atomic_u32_cmpxchg_aq_rl
atomic_u32_cmpxchg_aq_rl:
_load_value_2:
  lr.w.aq t0, (a0) # load double word into t0 and insert new entry into the reservation table
  bne t0, a2, _fail_2

  sc.w.rl t0, a1, (a0)
  # reservation was invalidated
  bnez t0, _load_value_2

  li a0, 1
  ret

_fail_2:
  li a0, 0
  ret

  
# u64 atomic_u64_add_aq_rl(atomic_u64 *x, u64 addent);
# a0: x
# a1: addent
# -> a0: value found at x
.global atomic_u64_add_aq_rl
atomic_u64_add_aq_rl:
  amoadd.d.aqrl a0, a1, (a0)
  ret

# u32 atomic_u32_add_aq_rl(atomic_u32 *x, u32 addent);
# a0: x
# a1: addent
# -> a0: value found at x
.global atomic_u32_add_aq_rl
atomic_u32_add_aq_rl:
  amoadd.w.aqrl a0, a1, (a0)
  ret

# u32 atomic_u32_swap_aq(atomic_u32 *x, u32 value);
# a0: x
# a1: value
# -> a0: value found at x during the swap
.global atomic_u32_swap_aq
atomic_u32_swap_aq:
  amoswap.w.aq a0, a1, (a0)
  ret

# u32 atomic_u32_swap_rl(atomic_u32 *x, u32 value);
# a0: x
# a1: value
# -> a0: value found at x during the swap
.global atomic_u32_swap_rl
atomic_u32_swap_rl:
  amoswap.w.rl a0, a1, (a0)
  ret
