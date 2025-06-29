#!/usr/local/bin/bash
set -xue

# QEMU for CHERI-RISC-V64 (this comes from cheribuild's QEMU)
QEMU=qemu-system-riscv64cheri

# Compiler wrapper (ccc will expand to clang with CHERI flags)
CC="ccc riscv64-purecap"

# Source files
ENTRY=entry.S
KERNEL=kernel.c
MAIN=main.c
LINKER=linker.ld
KPRINT=kprintf.c

# Compile sources
$CC -c $ENTRY -o entry.o
$CC -c $KPRINT -o kprintf.o
$CC -c $KERNEL -o kernel.o
$CC -c $MAIN -o main.o

# Link the kernel ELF using the CHERI-aware toolchain
riscv64-unknown-freebsd-cc \
  -nostdlib -nostartfiles -ffreestanding -static \
  -Wl,-T,$LINKER \
  -Wl,-Map=kernel.map \
  entry.o kernel.o kprintf.o main.o\
  -o kernel.elf

# Run with QEMU (no BIOS, direct ELF booting)
# $QEMU -machine virt -bios ./opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
$QEMU -bios none -kernel kernel.elf -nographic -M virt  -d nochain,cpu -S -s