.globl _start

.extern enter

.section .text
_start:
  csrr t0, mhartid
  bne t0, zero, spin

  slli t0, t0, 10

  # la sp, stacks + STACK_SIZE

  add sp, sp, t0

  jal zero, enter
  .word 0

spin:
  wfi
  jal zero, spin

.end


.section .bootsig
.byte 0x55, 0xaa
