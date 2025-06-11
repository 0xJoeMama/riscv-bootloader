CC=riscv64-linux-gnu-gcc
CFLAGS=-nostdlib -nodefaultlibs -nostdinc -nostartfiles
AS=riscv64-linux-gnu-as
LD=riscv64-linux-gnu-ld

LDFLAGS=-static

all: kernel

kernel: enter.o bootloader.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.s
	$(AS) -o $@ $^

run: kernel
	qemu-riscv64 ./kernel

clean:
	rm -rf *.o kernel
