CC = i686-elf-gcc
CFLAGS = -g -ffreestanding -fno-builtin -nostdlib -O0 -Wall -Werror \
	-std=gnu99 -I./stdlib

PROGRAMS = control abba racy philo
BINS = $(foreach p,$(PROGRAMS),$(p)/$(p).bin)
LIB = build/start.asm.o build/syscalls.asm.o build/logos.o

all: $(BINS)

build:
	mkdir -p build

build/start.asm.o: stdlib/start.asm | build
	nasm -f elf -g $< -o $@

build/syscalls.asm.o: stdlib/syscalls.asm | build
	nasm -f elf -g $< -o $@

build/logos.o: stdlib/logos.c stdlib/logos.h | build
	$(CC) $(CFLAGS) -c $< -o $@

$(BINS): %.bin: %.c $(LIB) linker.ld stdlib/logos.h
	$(CC) $(CFLAGS) -T linker.ld -o $@ $(LIB) $<

clean:
	rm -rf build $(BINS)

.PHONY: all clean
