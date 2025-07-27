#include "riscv.h"
#include "defs.h"
#include "hardware.h"
#include "syscalls.h"
#include <stdint.h>
#include <cheriintrin.h>
#include <cheri/cheri.h>
#include <cheri/cheric.h>

extern void kernelvec(void);


void trap_handler() {
    uintptr_t nr;
    uintptr_t param;
    uintptr_t retval = 0;
    

    asm volatile ("cmove %0, ca7" : "=C" (nr));
    asm volatile ("cmove %0, ca0" : "=C" (param));

    switch(nr){
        case PRINTSTRING:
            printstring((char*)param);
            break;
        case PUTCHAR:
            putachar((char)param);
            break;
        case GETCHAR:
            retval = (uint64_t)getachar();
            break;
        default:
            printstring("Unknown syscall number in trap_handler()\n");
            break;
    }
    uintptr_t pc = r_mepc();  // capability type (void * used here as generic capability)
    pc = cheri_address_set(pc, cheri_address_get(pc) + 4); // safely increment address
    w_mepc(pc); // write back the incremented PC

    asm volatile ("cmove ca0, %0" : : "C" (retval)); // set return value in ca0
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