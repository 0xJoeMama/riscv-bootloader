SECTIONS
{
  . = 0x0;
  .text :
  {
    *(.text)
  }
  
  .data :
  {
    *(.data)
  }

  . = 512K;
  .magic : {
    SHORT(0xAA55);
  }
}
