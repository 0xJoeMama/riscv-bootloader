CROSS=riscv64-elf
CC=$(CROSS)-gcc
CFLAGS=-nostdlib -nodefaultlibs -nostdinc -nostartfiles
AS=$(CROSS)-as
LD=$(CROSS)-ld
OBJCOPY=$(CROSS)-objcopy

LDFLAGS=-static  -Tlinker.ld

all: kernel

kernel.elf: enter.o bootloader.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.s
	$(AS) -o $@ $^

kernel: kernel.elf
	$(OBJCOPY) -O binary $^ $@

run: kernel
	qemu-riscv64 ./kernel

clean:
	rm -rf *.o kernel kernel.elf
