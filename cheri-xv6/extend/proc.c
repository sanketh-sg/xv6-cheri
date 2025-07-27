#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "param.h"

struct cpu cpus[NCPU];

struct proc proc[NPROC];

int
cpuid()
{
  int id = r_tp();
  return id;
}

struct cpu * __capability
mycpu(void)
{
    int id = cpuid();
    struct cpu __capability *c = &cpus[id];
    return c;
}

