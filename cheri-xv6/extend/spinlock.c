#include "riscv.h"
#include "spinlock.h"

void initlock(struct spinlock * __capability lk, char * __capability name) {
    lk->locked = 0;
    lk->name = name;
    // lk->cpu = 0;
    lk->cpu = (__capability struct cpu *)0;
}
