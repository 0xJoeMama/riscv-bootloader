CROSS=riscv64-elf
CC=$(CROSS)-gcc
ARCH=rv64gc
CFLAGS=-nostdlib -nodefaultlibs -nostdinc -nostartfiles -g3 -march=$(ARCH) -mabi=lp64 -mcmodel=medany -MMD
ASFLAGS=-march=$(ARCH) -mabi=lp64
AS=$(CROSS)-as
LD=$(CROSS)-ld

LDFLAGS=-static  -Tlinker.ld

all: sbi.elf

sbi.elf: atomic.o serial.o enter.o c_init.o print.o fw_dynamic.o endian.o string.o fdt.o 
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	rm -rf *.o sbi.elf *.d

-include *.d
