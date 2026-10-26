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

  /* required in order for compressed instructions to not be through outside the PMP domain we set up */
  . = ALIGN(4);
  see_data = .;

  .data : ALIGN(4096)
  {
    *(.data)
    *(.sdata)
    /* this does not play that well with PMP domains */
    *(.rodata)
  }

  .bss : ALIGN(4096)
  {
    *(.bss)
    *(.sbss)
  }

  . = ALIGN(4);
  see_data_end = .;

  . = ALIGN(4096);
  see_end = .;
}
