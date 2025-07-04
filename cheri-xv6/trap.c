#include "riscv.h"
#include "defs.h"
#include "hardware.h"
#include <stdint.h>
#include <cheriintrin.h>
#include <cheri/cheri.h>
#include <cheri/cheric.h>

extern void kernelvec(void);

void trapinit(void){

    w_stvec((uintptr_t)cheri_address_set(cheri_pcc_get(), (ptraddr_t)kernelvec));

}

void trap_handler(){
    int which_dev = 0;

    uintptr_t sepc = r_sepc();
    uint64_t sstatus = r_sstatus();
    uint64_t scause = r_scause();

    if((sstatus & SSTATUS_SPP) == 0)
        panic("trap_handler: not in supervisor mode\n");

    if(intr_get() != 0)
        panic("trap_handler: interrupts enabled\n");
    
    if((which_dev == devintr()) == 0){
        printstring("Interrupt or trap from unkown source\n");
        panic("trap_handler: unknown source\n");
    }

    if(scause == 0x9)
    w_sepc(sepc + 4); // return to next instruction after ecall
    
    //TODO: Handle the trap for process and clock interrupts
    else{
        w_sepc(sepc);
    }
    
    w_sstatus(sstatus);
}


// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.

int devintr(){
    uint64_t scause = r_scause();

    if(scause == 0x8000000000000009L){
        int irq = plic_claim();

        if (irq == UART0_IRQ){
            uartintr();
        } else if (irq){
            printstring("Unexpected interrupt from device devintr()\n");
        }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
        if(irq)
            plic_complete(irq);
      
      return 1;
    } else 
        return 0; 
}