PHDRS 
{
  text PT_LOAD FLAGS(5);
  data PT_LOAD FLAGS(6);
  rodata PT_LOAD FLAGS(4);
}

SECTIONS
{
  /* start placing at 0x80000000 */
  . = 0x80000000;

  see_start = .;
  .text :
  {
    *(.text.boot)
    *(.text)
  } : text

  . = ALIGN(4);
  see_rodata = .;

  .rodata :
  {
    /* this does not play that well with PMP domains */
    *(.rodata)
    *(.srodata)
  } : rodata

  /* required in order for compressed instructions to not be through outside the PMP domain we set up */
  . = ALIGN(4);
  see_data = .;

  .data :
  {
    *(.data)
    *(.sdata)
  } : data

  .bss :
  {
    *(.bss)
    *(.sbss)
  } : data

  . = ALIGN(4);
  see_data_end = .;

  . = ALIGN(4096);
  see_end = .;
}
