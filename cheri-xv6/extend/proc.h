#include <cheri/cheric.h> // or <cheriintrin.h> depending on your environment
#include "riscv.h"
#include "spinlock.h"
#include <stdint.h>
#include "param.h"


enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct context {
  void * __capability ra;
  void * __capability sp;

  // callee-saved capabilities
  void * __capability s0;
  void * __capability s1;
  void * __capability s2;
  void * __capability s3;
  void * __capability s4;
  void * __capability s5;
  void * __capability s6;
  void * __capability s7;
  void * __capability s8;
  void * __capability s9;
  void * __capability s10;
  void * __capability s11;
};

#include <cheri/cheric.h> // or <cheriintrin.h>

struct trapframe {
  /*   0 */ uint64_t kernel_satp;   // kernel page table (not a capability)
  /*   8 */ uint64_t kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64_t kernel_trap;   // usertrap()
  /*  24 */ void * __capability epc;           // saved user program counter
  /*  32 */ uint64_t kernel_hartid; // saved kernel tp

  // CHERI-aware saved user registers
  /*  40 */ void * __capability ra;
  /*  48 */ void * __capability sp;
  /*  56 */ void * __capability gp;
  /*  64 */ void * __capability tp;
  /*  72 */ void * __capability t0;
  /*  80 */ void * __capability t1;
  /*  88 */ void * __capability t2;
  /*  96 */ void * __capability s0;
  /* 104 */ void * __capability s1;
  /* 112 */ void * __capability a0;
  /* 120 */ void * __capability a1;
  /* 128 */ void * __capability a2;
  /* 136 */ void * __capability a3;
  /* 144 */ void * __capability a4;
  /* 152 */ void * __capability a5;
  /* 160 */ void * __capability a6;
  /* 168 */ void * __capability a7;
  /* 176 */ void * __capability s2;
  /* 184 */ void * __capability s3;
  /* 192 */ void * __capability s4;
  /* 200 */ void * __capability s5;
  /* 208 */ void * __capability s6;
  /* 216 */ void * __capability s7;
  /* 224 */ void * __capability s8;
  /* 232 */ void * __capability s9;
  /* 240 */ void * __capability s10;
  /* 248 */ void * __capability s11;
  /* 256 */ void * __capability t3;
  /* 264 */ void * __capability t4;
  /* 272 */ void * __capability t5;
  /* 280 */ void * __capability t6;
};


struct proc {
    struct spinlock lock; // protects this proc's state

    enum procstate state; // process state
    void * __capability chan; // if SLEEPING, then sleeping on chan
    int killed; // if non-zero, have been killed
    int pid; // process ID
    int xstate; // exit status to be returned to parent


    struct proc * __capability parent; // parent process

    uint64_t kstack; // kernel stack
    uint64_t sz; // size of process memory (bytes)
    pagetable_t pagetable; // user page table
    struct trapframe * __capability trapframe; // trap frame for current syscall
    struct context context; // swtch() here to run process
    // file descriptors todo
    char name[16]; // process name (debugging)
};

struct cpu {
    struct proc * __capability proc; // the process running on this CPU, if any
    struct context context; // swtch() here to enter scheduler
    int noff; // number of nested pushcli calls
    int intena; // were interrupts enabled before pushcli?
};

extern struct cpu cpus[NCPU];