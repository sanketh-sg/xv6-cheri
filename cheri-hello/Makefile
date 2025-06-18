CC=clang
LD=ld.lld

CFLAGS=--target=riscv64-unknown-freebsd -mabi=purecap -march=rv64gcxcheri -nostdlib -Wall -Werror -Wcheri
LDFLAGS=-T kernel/linker.ld -static

OBJDIR=kernel
OBJS=$(OBJDIR)/entry.o $(OBJDIR)/start.o

all: kernel.elf

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJDIR)/entry.o: $(OBJDIR)/entry.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/start.o: $(OBJDIR)/start.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f kernel.elf $(OBJS)
