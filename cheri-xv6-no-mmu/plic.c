#include "riscv.h"
#include "defs.h"
#include "hardware.h"


void plic_init(void){

    *(uint32_t* __capability)cheri_offset_set(PLIC_CAP, UART0_IRQ * 4) = 1;

    *(uint32_t* __capability) PLIC_SENABLE = (1 << UART0_IRQ);

    *(uint32_t* __capability) PLIC_SPRIORITY = 1;

}

int plic_claim(void){
    int irq = *(uint32_t* __capability) PLIC_SCLAIM;
    return irq;
}

void plic_complete(int irq){
    *(uint32_t* __capability) PLIC_SCLAIM = irq;
}

