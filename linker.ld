SECTIONS
{
  /* start placing at 0x80000000 */
  . = 0x80000000;
  . = ALIGN(4096);

  see_start = .;

  .text : ALIGN(4096)
  {
    *(.text.boot)
    *(.text)
  }

  .data : ALIGN(4096)
  {
    *(.data)
    *(.sdata)
    *(.rodata)
  }

  .bss : ALIGN(4096)
  {
    *(.bss)
    *(.sbss)
  }
  
  . = ALIGN(4096);
  see_end = .;
}
