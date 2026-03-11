MEMORY 
{
  ROM (rx) : ORIGIN = 0x80000000, LENGTH = 256K
  RAM (rw) : ORIGIN = 0x80000000 + 256K, LENGTH = 256K
}

SECTIONS
{
  .text : ALIGN(4096)
  {
    bootloader.o(.text)
    *(.text)
  } > ROM

  .data : ALIGN(4096)
  {
    *(.data)
  } > RAM

  .bss : ALIGN(4096)
  {
    *(.bss)
  } > RAM
}
