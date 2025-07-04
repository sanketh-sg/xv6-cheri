#include <stdint.h>



#ifndef __ASSEMBLER__

// which hart (core) is this?
static inline uint64_t 
r_mhartid()
{
  uint64_t  x;
//   register uintptr_t x asm("a0");  // explicitly use x10
  asm volatile("csrr %0, mhartid" : "=r" (x) );
  return x;
}

// Machine Status Register, mstatus

#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)    // machine-mode interrupt enable.

static inline uint64_t 
r_mstatus()
{
  uint64_t  x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void 
w_mstatus(uint64_t  x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void 
w_mepc(uintptr_t  x)
{
    // asm volatile("csrw mepc, %0" : : "r" (x));
  asm volatile("cspecialw mepcc, %0" : : "C" (x));
}

static inline void 
w_mtvec(uintptr_t  x)
{
    // asm volatile("csrw mepc, %0" : : "r" (x));
  asm volatile("cspecialw mtcc, %0" : : "C" (x));
}

// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

static inline uint64_t 
r_sstatus()
{
  uint64_t  x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void 
w_sstatus(uint64_t  x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

// // Supervisor Interrupt Pending
// static inline uintptr_t 
// r_sip()
// {
//   uintptr_t  x;
//   asm volatile("csrr %0, sip" : "=r" (x) );
//   return x;
// }

// static inline void 
// w_sip(uintptr_t  x)
// {
//   asm volatile("csrw sip, %0" : : "r" (x));
// }

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software

// Machine Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7) // timer
#define MIE_MSIE (1L << 3) // software


static inline uint64_t 
r_sie()
{
  uint64_t  x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void 
w_sie(uint64_t  x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

// Machine-mode Interrupt Enable
#define MIE_STIE (1L << 5)  // supervisor timer
static inline uint64_t 
r_mie()
{
  uint64_t  x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void 
w_mie(uint64_t  x)
{
  asm volatile("csrw mie, %0" : : "r" (x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void 
w_sepc(uintptr_t  x)
{
  asm volatile("cspecialw sepcc, %0" : : "C" (x));
}

static inline uintptr_t 
r_sepc()
{
  uintptr_t  x;
  asm volatile("cspecialr %0, sepcc" : "=C" (x) );
  return x;
}

// Machine Exception Delegation
static inline uint64_t 
r_medeleg()
{
  uint64_t  x;
  asm volatile("csrr %0, medeleg" : "=r" (x) );
  return x;
}

static inline void 
w_medeleg(uint64_t  x)
{
  asm volatile("csrw medeleg, %0" : : "r" (x));
}

// Machine Interrupt Delegation
static inline uint64_t 
r_mideleg()
{
  uint64_t  x;
  asm volatile("csrr %0, mideleg" : "=r" (x) );
  return x;
}

static inline void 
w_mideleg(uint64_t  x)
{
  asm volatile("csrw mideleg, %0" : : "r" (x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
static inline void 
w_stvec(uintptr_t  x)
{
  asm volatile("cspecialw stcc, %0" : : "C" (x));
}

static inline uintptr_t 
r_stvec()
{
  uintptr_t  x;
  asm volatile("cspecialr %0, stcc" : "=C"(x));
  return x;
}

// Supervisor Timer Comparison Register
static inline uintptr_t 
r_stimecmp()
{
  uint64_t  x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm volatile("csrr %0, 0x14d" : "=r" (x) );
  return x;
}

static inline void 
w_stimecmp(uint64_t  x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm volatile("csrw 0x14d, %0" : : "r" (x));
}

// Machine Environment Configuration Register
static inline uint64_t 
r_menvcfg()
{
  uint64_t  x;
  asm volatile("csrr %0, menvcfg" : "=r" (x) );
//   asm volatile("csrr %0, 0x30a" : "=r" (x) );
  return x;
}

static inline void 
w_menvcfg(uint64_t  x)
{
  asm volatile("csrw menvcfg, %0" : : "r" (x));
//   asm volatile("csrw 0x30a, %0" : : "r" (x));
//   asm volatile("cspecialw menvcfg, %0" : : "C" (x));
}

// Physical Memory Protection
static inline void
w_pmpcfg0(uint64_t  x)
{
  asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void
w_pmpaddr0(uint64_t  x)
{
  asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

// #define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uintptr_t)(pagetable)) >> 12))


// supervisor address translation and protection;
// holds the address of the page table.
static inline void 
w_satp(uint64_t  x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline uint64_t 
r_satp()
{
  uint64_t  x;
  asm volatile("csrr %0, satp" : "=r" (x) );
  return x;
}

// Supervisor Trap Cause
static inline uint64_t 
r_scause()
{
  uint64_t  x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

// Supervisor Trap Value
static inline uint64_t 
r_stval()
{
  uint64_t  x;
  asm volatile("csrr %0, stval" : "=r" (x) );
  return x;
}

// Machine-mode Counter-Enable
static inline void 
w_mcounteren(uint64_t  x)
{
  asm volatile("csrw mcounteren, %0" : : "r" (x));
}

static inline uint64_t 
r_mcounteren()
{
  uint64_t  x;
  asm volatile("csrr %0, mcounteren" : "=r" (x) );
  return x;
}

// machine-mode cycle counter
static inline uint64_t 
r_time()
{
  uint64_t  x;
  asm volatile("csrr %0, time" : "=r" (x) );
  return x;
}

// enable device interrupts
static inline void
intr_on()
{
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// disable device interrupts
static inline void
intr_off()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

// are device interrupts enabled?
static inline int
intr_get()
{
  uint64_t  x = r_sstatus();
  return (x & SSTATUS_SIE) != 0;
}

// static inline void *__capability r_sp() {
//     void *__capability sp;
//     asm volatile("cmove %0, sp" : "=C"(sp));
//     return sp;
// }

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
static inline uint64_t 
r_tp()
{
  uint64_t  x;
  asm volatile("mv %0, tp" : "=r" (x) );
  return x;
}

static inline void 
w_tp(uint64_t  x)
{
  asm volatile("mv tp, %0" : : "r" (x));
}

// static inline uintptr_t 
// r_ra()
// {
//   uintptr_t  x;
//   asm volatile("mv %0, ra" : "=r" (x) );
//   return x;
// }

// // flush the TLB.
// static inline void sfence_vma() {
//     asm volatile("cflush");  // CHERI-specific capability flush
//     asm volatile("sfence.vma zero, zero");
// }

// typedef uint64_t pte_t;
// typedef pte_t *__capability pagetable_t; // 512 PTEs

#endif // __ASSEMBLER__

// #define PGSIZE 4096 // bytes per page
// #define PGSHIFT 12  // bits of offset within a page

// #define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
// #define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

// #define PTE_V (1L << 0) // valid
// #define PTE_R (1L << 1)
// #define PTE_W (1L << 2)
// #define PTE_X (1L << 3)
// #define PTE_U (1L << 4) // user can access

// // shift a physical address to the right place for a PTE.
// #define PA2PTE(pa) ((((uintptr_t)pa) >> 12) << 10)

// #define PTE2PA(pte) (((pte) >> 10) << 12)

// #define PTE_FLAGS(pte) ((pte) & 0x3FF)

// // extract the three 9-bit page table indices from a virtual address.
// #define PXMASK          0x1FF // 9 bits
// #define PXSHIFT(level)  (PGSHIFT+(9*(level)))
// #define PX(level, va) ((((uintptr_t) (va)) >> PXSHIFT(level)) & PXMASK)

// // one beyond the highest possible virtual address.
// // MAXVA is actually one bit less than the max allowed by
// // Sv39, to avoid having to sign-extend virtual addresses
// // that have the high bit set.
// #define MAXVA (1L << (9 + 9 + 9 + 12 - 1))