# xv6-cheri

This project is a modified version of the xv6 operating system, adapted for the CHERI (Capability Hardware Enhanced RISC Instructions) architecture.


## Compiling

This project uses cross compiling technique, we use [tools](https://github.com/CTSRD-CHERI/cheri-exercises/tree/master/tools) `ccc` which help in compiling it to cheri.


Under the hood it uses below command for purecap, 
`/home/cheri/cheri/output/sdk/bin/clang -target riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d -mno-relax --sysroot=/home/cheri/cheri/output/sdk/sysroot-riscv64-purecap -g -O2 -fuse-ld=lld -Wall -Wcheri -c <src_filename> -o <obj_filename>`

Also refer to run.sh in cheri-xv6 folder which upon running takes care of compiling and linking the OS.

## Prerequisites

- CHERI toolchain (compiler, linker, QEMU, etc.)
- Make
- Git

## Getting Started

1. **Clone the repository:**
    ```sh
    git clone <repository-url>
    cd xv6-cheri
    ```

2. **Set up the CHERI toolchain:**
    Ensure the CHERI toolchain binaries are in your `PATH`.


## Additional Information

- For more details, refer to the official [xv6](https://pdos.csail.mit.edu/6.828/2021/xv6.html) and [CHERI](https://www.cl.cam.ac.uk/research/security/ctsrd/cheri/) documentation.
- If you encounter issues, check the `Makefile` and ensure all dependencies are installed.
