typedef unsigned int u32;

extern void enter(void) { 
  (*(volatile u32 *) 0x100000) = 0x5555;
}
