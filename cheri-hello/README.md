Any changes to this requires it be compiled using CHERI llvm for purecap.
Refer this [link](https://ctsrd-cheri.github.io/cheri-exercises/exercises/index.html) to know more about compiling for cheri.
To run the kernel on QEMU.
qemu-system-riscv64cheri -bios none -kernel kernel.elf -nographic -M virt -singlestep -d nochain,cpu -S -s
