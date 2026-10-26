.globl _start

.extern enter # initialization function
.extern enter_secondary # secondary CPU boot function
.extern atomic_u64_cmpxchg_aq_rl # well, the name says it all
.extern boot # the atomic boot flag to block all CPUs except the boot CPU
.extern cpu_stacks # stacks for alternate CPUs. Every CPU has a 4k entry into this stack. 
                   # we calculate using cpu_stacks + 4096 * mhartid

.section .text.boot
_start:
  # save ALL registers that will be used for the following function call
  mv s0, a0
  mv s1, a1
  mv s2, a2

  la a0, boot
  li a1, 0
  li a2, 1
  call atomic_u64_cmpxchg_aq_rl

  bnez a0, jump_to_c

  li t1, 2
  la a0, boot
_spin:
  ld t0, 0(a0)
  bne t0, t1, _spin

secondary_setup_stack:
  # annull the device tree information on secondary CPUs
  li s0, 0
  li s1, 0
  li s2, 0

  # load cpu_stacks
  la t0, cpu_stacks
  ld t0, (t0)

  # use hartid as index into stacks 
  csrr a0, mhartid

  # top of stack
  addi t1, a0, 1
  # in pages
  slli t1, t1, 12
  # offset from the beginning of cpu_stacks
  add sp, t0, t1

  call enter_secondary

jump_to_c:
  # restore the saved registers
  mv a0, s0
  mv a1, s1
  mv a2, s2

  # just setup a stack and jump to C
  # no more assembly needed
  la sp, stack
  addi t0, zero, 1
  slli t0, t0, 12
  add sp, sp, t0
  call enter

.section .data
.global stack
stack: .space 4096
