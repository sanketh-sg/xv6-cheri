Any changes to this requires it be compiled using CHERI llvm for purecap 

To run the kernel on QEMU
qemu-system-riscv64cheri -bios none -kernel kernel.elf -nographic -M virt -singlestep -d nochain,cpu -S -s
