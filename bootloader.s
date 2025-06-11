.globl _start

.extern enter

.equ STACK_SIZE, 4096
.equ CPU_NUM, 1

.section .text
_start:
  csrr t0, mhartid
  bne t0, zero, spin

  slli t0, t0, 10

  la sp, stacks + STACK_SIZE

  add sp, sp, t0

  jal zero, enter

spin:
  wfi
  jal zero, spin

stacks: .space STACK_SIZE * CPU_NUM


.end

