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
    uint64_t mcause = r_mcause();
    
    // Handle syscalls (ecall from user mode - mcause = 8)
    if (mcause == 8) {
        // Get syscall parameters - extract ca0 FIRST before it gets overwritten
        uintptr_t nr;
        uintptr_t param;
        uintptr_t retval = 0;
        
        asm volatile ("cmove %0, ca0" : "=C" (param));  // Extract parameter first
        asm volatile ("cmove %0, ca7" : "=C" (nr));     // Then extract syscall number

        switch(nr){
            case PRINTSTRING:
            {
                // Convert user virtual address to physical address
                // User virtual 0x0-0x1FFFFF maps to physical 0x80200000-0x803FFFFF  
                uint64_t user_virtual_addr = (uint64_t)param;
                uint64_t physical_addr = 0x80200000 + user_virtual_addr;
                
                // Create a capability to the physical address
                char * __capability str = (char * __capability)cheri_address_set(cheri_ddc_get(), physical_addr);
                printstring(str);
                break;
            }
                
            case PUTCHAR:
                putachar((char)param);
                break;
                
            case GETCHAR:
                retval = (uint64_t)getachar();
                break;
                
            default:
                putachar('?');  // Unknown syscall
                break;
        }
        
        // Increment PC to next instruction
        uintptr_t pc = r_mepc();
        pc = cheri_address_set(pc, cheri_address_get(pc) + 4);
        w_mepc(pc);
        
        // Set return value
        asm volatile ("cmove ca0, %0" : : "C" (retval));
        
    } else {
        // Not a syscall
        printstring("ERR");
        printhex(mcause);
    }
}


// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.