#!/usr/local/bin/bash
set -xue

# QEMU for CHERI-RISC-V64 (this comes from cheribuild's QEMU)
QEMU=qemu-system-riscv64cheri

# Compiler wrapper (ccc will expand to clang with CHERI flags)
CC="ccc riscv64-purecap -g"

# Source files
ENTRY=entry.S
USER_ENTRY=userentry.S
START=start.c
MAIN=main.c
LINKER=linker.ld
USER_LINKER=user.ld
KPRINT=kprintf.c
TRAP=trap.c
KERNELVEC=kernelvec.S
PLIC=plic.c

# Compile sources
$CC -c $ENTRY -o entry.o
$CC -c $USER_ENTRY -o userentry.o
$CC -c $KPRINT -o kprintf.o
$CC -c $KERNELVEC -o kernelvec.o
$CC -c $PLIC -o plic.o
$CC -c $TRAP -o trap.o
$CC -c $START -o start.o
$CC -c $MAIN -o main.o

# Link user entry point
riscv64-unknown-freebsd-cc \
  -nostdlib -nostartfiles -ffreestanding -static \
  -Wl,-T,$USER_LINKER \
  -Wl,-Map=user.map \
  userentry.o main.o \
  -o user.elf

# Extract binary content from ELF (skip headers)
riscv64-unknown-freebsd-objcopy -O binary user.elf user.bin

# Link the kernel ELF using the CHERI-aware toolchain
riscv64-unknown-freebsd-cc \
  -nostdlib -nostartfiles -ffreestanding -static \
  -Wl,-T,$LINKER \
  -Wl,-Map=kernel.map \
  entry.o userentry.o start.o kprintf.o kernelvec.o plic.o trap.o main.o\
  -o kernel.elf

# Run with QEMU (no BIOS, direct ELF booting)
# $QEMU -machine virt -bios ./opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
$QEMU -bios none -kernel kernel.elf -device loader,file=user.bin,addr=0x80200000 -nographic -M virt -serial mon:stdio -serial pty -S -s