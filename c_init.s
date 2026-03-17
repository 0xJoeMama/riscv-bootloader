.globl _start

.extern enter
.extern print_message

.section .text
_start:
  # just setup a stack and jump to C
  # no more assembly needed
  la sp, stack
  call enter

.section .data
stack: .space 4096
