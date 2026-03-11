.globl _start

.extern enter
.extern print_message

.section .text
_start:
  la sp, stack
  call enter

.section .data
stack: .space 4096
