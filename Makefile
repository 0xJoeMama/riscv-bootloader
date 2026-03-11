CROSS=riscv64-elf
CC=$(CROSS)-gcc
CFLAGS=-nostdlib -nodefaultlibs -nostdinc -nostartfiles -g3 -march=rv64i -mabi=lp64 -mcmodel=medany
ASFLAGS=-march=rv64i -mabi=lp64
AS=$(CROSS)-as
LD=$(CROSS)-ld

LDFLAGS=-static  -Tlinker.ld

all: kernel.elf

kernel.elf: enter.o bootloader.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	rm -rf *.o kernel kernel.elf
