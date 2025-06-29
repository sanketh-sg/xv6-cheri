#include <stdint.h>
#include "riscv.h"
#include "hardware.h"
#include  "defs.h"
#include <cheri_init_globals.h>
#include <cheriintrin.h>
#include <cheri/cheri.h>
#include <cheri/cheric.h>

extern char __bss[], __bss_end[], stack0[];


void foo(){
  panic("KERNEL PANIC!!!!");
}

volatile struct uart* __capability uart0;
volatile struct plic* __capability plic;


void start(){
     // Initialize the CHERI global state
    cheri_init_globals(); 


    uint64_t x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK; // Clear the MPP bits
    x |= MSTATUS_MPP_S; // Set MPP to Supervisor mode
    w_mstatus(x); // Write back the modified status register

    //TODO
    //w_mtvec((uintptr_t)cheri_address_set(cheri_pcc_get(), (ptraddr_t)kernelvec));
    w_mtvec((uintptr_t)cheri_address_set(cheri_pcc_get(), (ptraddr_t)foo));
    
    // w_mepc((uintptr_t)main); // Set the machine exception program counter to main
    w_mepc((uintptr_t)cheri_address_set(cheri_pcc_get(), (ptraddr_t)main));
    w_satp(0); //disable paging

    w_medeleg(0xffff); // Enable all exceptions
    w_mideleg(0xffff); // Enable all interrupts
    w_sie(r_sie() | SIE_SEIE | SIE_SSIE | SIE_STIE); // Enable Supervisor External, Software, and Timer interrupts

    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    init_uart0(); // Initialize UART0
    plic_cap_init(); // Initialize the PLIC (Platform-Level Interrupt Controller) capability

    timerinit(); // Initialize the timer

    
    
    // using the PLIC (Platform-Level Interrupt Controller) and machine-mode interrupts.
    interruptinit(); // Initialize interrupts
    
    // enable uart rx irqs
    extern volatile struct uart* __capability uart0;
    uart0->IER=0x1;

    int id = r_mhartid(); // Get the hart ID
    w_tp(id); // Set the thread pointer to the hart ID
    
    asm volatile("mret"); // Return from machine mode to user mode
}

void init_uart0(void) {
  uart0 = (volatile struct uart* __capability) UART0_CAP;
}

void plic_cap_init(void){
  plic = (volatile struct plic* __capability) PLIC_CAP;
}


void timerinit(void) {
  // we only have one CPU...
  int id = 0; //r_mhartid(); // Get the hart ID

  int interval = 1000000; // 1 second interval

  void* __capability clint_cap = cheri_bounds_set(cheri_address_set(cheri_ddc_get(), CLINT_BASE), CLINT_SIZE);


  // Set offset to target MTIME and MTIMECMP
  volatile uint64_t* __capability mtime = (volatile uint64_t* __capability) cheri_offset_set(clint_cap, CLINT_MTIME - CLINT_BASE);
  volatile uint64_t* __capability mtimecmp = (volatile uint64_t* __capability) cheri_offset_set(clint_cap, CLINT_MTIMECMP(id) - CLINT_BASE);

  // Read mtime, set mtimecmp
  *mtimecmp = *mtime + interval;

  //CLINT triggers a timer interrupt automatically when the value of the mtime register 
  //reaches or exceeds the value in the mtimecmp register.

  
  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}


// ask each hart to generate timer interrupts.
// void
// timerinit()
// {
//   // enable supervisor-mode timer interrupts.
//   w_mie(r_mie() | MIE_STIE);
  
//   uint64_t mcfg = r_menvcfg();
//   mcfg |= (1L << 28);
//   w_menvcfg(mcfg);
//   // enable the sstc extension (i.e. stimecmp).
//   w_menvcfg(r_menvcfg() | (1L << 63)); 
  
//   // allow supervisor to use stimecmp and time.
//   w_mcounteren(r_mcounteren() | 2);
  
//   // ask for the very first timer interrupt.
//   w_stimecmp(r_time() + 1000000);
// }

void interruptinit(void){
  // set desired IRQ priorities non-zero (otherwise disabled).
  // *(uint32*)(PLIC + UART0_IRQ*4) = 1;

  *(uint32_t* __capability)cheri_offset_set(PLIC_CAP, UART0_IRQ * 4) = 1;
     
  // set uart's enable bit for this hart's M-mode.
  // *(uint32*)PLIC_MENABLE = (1 << UART0_IRQ);


  *(uint32_t* __capability)PLIC_MENABLE = (1 << UART0_IRQ);
  
  // set this hart's M-mode priority threshold to 0.
  //   *(uint32*)PLIC_MPRIORITY = 0; 


  *(uint32_t* __capability)PLIC_MPRIORITY = 0; 

  // enable machine-mode external interrupts.
  w_mie(r_mie() | MIE_MEIE);

}