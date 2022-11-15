MAINDIR = $(shell pwd)
TARGET = i686-elf
BUILDTOOLS = $(MAINDIR)/build-tools/bin
PREFIX = $(BUILDTOOLS)/$(TARGET)
CC = $(PREFIX)-gcc
AS = $(PREFIX)-as

CSTD = -std=gnu99
CFLAGS = -ffreestanding -O2 -Wall -Wextra
LFLAGS = -ffreestanding -O2 -nostdlib -lgcc

SRCDIR = $(MAINDIR)/src
BUILDDIR = $(MAINDIR)/build
VPATH = $(SRCDIR)

run : $(BUILDDIR)/myos.img
	qemu-system-i386 -cdrom $<

all : $(BUILDDIR)/myos.img

$(BUILDDIR)/myos.img : grub.cfg $(BUILDDIR)/myos.bin
	mkdir -p $(BUILDDIR)/isodir/boot/grub
	cp $(BUILDDIR)/myos.bin $(BUILDDIR)/isodir/boot/myos.bin
	cp $< $(BUILDDIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(BUILDDIR)/isodir

verify-multiboot : $(BUILDDIR)/myos.bin
	grub-file --is-x86-multiboot $<

$(BUILDDIR)/myos.bin : linker.ld $(BUILDDIR)/boot.o $(BUILDDIR)/kernel.o
	$(CC) $(LFLAGS) -T $< -o $@ $(filter-out $<,$^)

$(BUILDDIR)/boot.o : boot.s
	$(AS) $< -o $@

$(BUILDDIR)/kernel.o : kernel.c
	$(CC) $(CSTD) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(BUILDDIR)/*
