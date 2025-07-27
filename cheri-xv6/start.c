#include <stdint.h>
#include "riscv.h"
#include "hardware.h"
#include "defs.h"
#include "proc.h"

#include <cheri_init_globals.h>
#include <cheriintrin.h>
#include <cheri/cheri.h>
#include <cheri/cheric.h>

extern char __bss[], __bss_end[], stack0[];
extern void kernelvec(void);
extern void _user_entry(void);

// Function declarations
void init_user_capabilities(void);

__attribute__((aligned (PGSIZE))) uint64_t page_table[NPROC][PTSIZE];
__attribute__((aligned (PGSIZE))) uint64_t user_l1_tables[NPROC][256];   // Separate L1 tables for user space
__attribute__((aligned (PGSIZE))) uint64_t kernel_l1_tables[NPROC][256]; // Separate L1 tables for kernel space
uint64_t kernel_page_table[512] __attribute__((aligned(4096)));

struct pcbentry pcb[NPROC]; // Process control blocks
uint64_t current_pid; // Current process ID

volatile struct uart* __capability uart0;
volatile struct plic* __capability plic;
void* __capability kernel_ddc; // Global kernel DDC for hardware access


void start(){
     // Initialize the CHERI global state
    cheri_init_globals(); 

    // Save the kernel DDC for hardware access
    kernel_ddc = cheri_ddc_get(); 


    uint64_t x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK; // Clear the MPP bits
    x |= MSTATUS_MPP_U; // Set MPP to User mode
    w_mstatus(x); // Write back the modified status register

    
    // enable machine-mode interrupts.
    w_mstatus(r_mstatus() | MSTATUS_MIE);

    // enable software interrupts (ecall) in M mode.
    w_mie(r_mie() | MIE_MSIE);

    w_mtvec((uintptr_t)cheri_address_set(cheri_pcc_get(), (ptraddr_t)kernelvec)); // Set the machine trap vector to handle_timer_interrupt
    init_uart0(); // Initialize UART0

    // w_satp(0); //disable paging

    enable_paging(); // Enable paging and set up the page tables

    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    plic_cap_init(); // Initialize the PLIC (Platform-Level Interrupt Controller) capability

    // DON'T enable timer interrupts - we'll only handle syscalls (ecall)
    // w_mie(r_mie() | MIE_MTIE);  // DISABLED
    // timerinit(); // DISABLED
    
    // using the PLIC (Platform-Level Interrupt Controller) and machine-mode interrupts.
    interruptinit(); // Initialize interrupts
    
    // enable uart rx irqs
    // w_mepc((uintptr_t)main); // Set the machine exception program counter to main
    // w_mepc((uintptr_t)cheri_address_set(cheri_pcc_get(), (ptraddr_t)main));
    // Set MEPC to user entry point (virtual address 0)
    // The user code is physically at 0x80200000 but mapped to virtual 0x0
    // Use the user's PC capability from the PCB which points to virtual address 0
    w_mepc((uintptr_t)pcb[0].pc);
    
    // Set mscratch to point to kernel stack capability for context switching
    // Create a kernel stack capability - use a separate area for kernel stack
    void* __capability kernel_stack_base = cheri_bounds_set(
        cheri_address_set(cheri_ddc_get(), 0x80100000 - 4096),  // Kernel stack area base
        4096  // 4KB kernel stack
    );
    // Set the stack pointer to the top of the stack (grows downward)
    void* __capability kernel_stack = cheri_address_set(kernel_stack_base, 0x80100000);
    w_mscratch((uintptr_t)kernel_stack);

    // Set up user DDC (Default Data Capability) for user space access
    // The user DDC should point to the physical memory where user data actually resides
    // User virtual addresses 0x0-0x201000 map to physical 0x80200000-0x80401000
    // CHERI capabilities need to be based on physical addresses
    void* __capability user_ddc = cheri_bounds_set(
        cheri_address_set(cheri_ddc_get(), 0x80200000),  // Physical base where user memory is
        0x201000  // 2MB + 4KB for stack
    );
    w_ddc(user_ddc);

    // Set up initial user stack pointer
    // The user entry code expects to set up its own stack, but we can help by
    // providing a proper initial stack capability
    // However, since mret doesn't restore csp, the user code will set up its own stack
    
    asm volatile("mret"); // Return from machine mode to user mode
}

void init_uart0(void) {
    uart0 = (volatile struct uart* __capability) UART0_CAP;
    uart0->IER = 0x01;  // Enable Received Data Available interrupt
    uart0->FCR = 0x01;  // Enable FIFO
}

void plic_cap_init(void){
  plic = (volatile struct plic* __capability) PLIC_CAP;
}

void timerinit(void) {
  // we only have one CPU...
  int id = 0; //r_mhartid(); // Get the hart ID

  int interval = 100000000; // Much longer interval - 100 million cycles

  void* __capability clint_cap = cheri_bounds_set(cheri_address_set(kernel_ddc, CLINT_BASE), CLINT_SIZE);


  // Set offset to target MTIME and MTIMECMP
  volatile uint64_t* __capability mtime = (volatile uint64_t* __capability) cheri_offset_set(clint_cap, CLINT_MTIME - CLINT_BASE);
  volatile uint64_t* __capability mtimecmp = (volatile uint64_t* __capability) cheri_offset_set(clint_cap, CLINT_MTIMECMP(id) - CLINT_BASE);

  // Read mtime, set mtimecmp
  *mtimecmp = *mtime + interval;

  //CLINT triggers a timer interrupt automatically when the value of the mtime register 
  //reaches or exceeds the value in the mtimecmp register.
}

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

// Timer functions removed - we don't use timer interrupts


void* __capability init_pagetable(int proc) {
    // Clear the L2 page table (root)
    for (int i = 0; i < 512; i++) {
        page_table[proc][i] = 0;
    }
    
    // Clear the L1 page tables
    for (int i = 0; i < 256; i++) {
        user_l1_tables[proc][i] = 0;
        kernel_l1_tables[proc][i] = 0;
    }

    // Calculate addresses for L1 tables (now properly aligned)
    uint64_t user_l1_addr = (uint64_t)&user_l1_tables[proc][0];
    uint64_t kernel_l1_addr = (uint64_t)&kernel_l1_tables[proc][0];
    
    // Level 2 Page Table (Root) - 512 entries
    // Entry 0: Maps virtual 0x00000000-0x3FFFFFFF to user L1 table
    page_table[proc][0] = PA2PTE(user_l1_addr) | PTE_V;
    
    // Entry 256: Maps virtual 0x80000000-0xBFFFFFFF to kernel L1 table (identity mapping)
    page_table[proc][256] = PA2PTE(kernel_l1_addr) | PTE_V;

    // Level 1 User Page Table
    // Entry 0: Map virtual 0x0-0x1FFFFF (2MB) to physical user memory
    user_l1_tables[proc][0] = PA2PTE(0x80200000ULL + proc * 0x200000ULL) | 
                              PTE_V | PTE_R | PTE_W | PTE_X | PTE_U;

    // Level 1 Kernel Page Table
    // Identity map kernel space: virtual 0x80000000 -> physical 0x80000000
    // Map the first 512MB of kernel space (256 pages * 2MB each)
    for (int i = 0; i < 256; i++) {  
        kernel_l1_tables[proc][i] = PA2PTE(0x80000000ULL + i * 0x200000ULL) |
                                    PTE_V | PTE_R | PTE_W | PTE_X;
    }

    return cheri_bounds_set(
        cheri_address_set(cheri_ddc_get(), (ptraddr_t)&page_table[proc][0]),
        PGSIZE
    );
}

//     Physical Memory Layout:
// 0x80000000:    Kernel Base
// 0x80200000:    Process 0 Memory (2MB)
// 0x80400000:    Process 1 Memory (2MB)
// 0x80600000:    Process 2 Memory (2MB)

// Virtual Memory Mapping (for each process):
// User Virtual Address 0 → Maps to → Physical Address (0x80200000 + proc*2MB)


void enable_paging(void) {
    for (int i = 0; i < NPROC; i++) {
        // Initialize page table
        pcb[i].page_table_base = init_pagetable(i);
        
        // Set up physical base capability for user memory
        pcb[i].phybase = cheri_bounds_set(
            cheri_address_set(cheri_ddc_get(), 0x80200000ULL + i * 0x200000ULL),
            0x200000
        );
        
        // Set up user virtual PC (virtual address 0)
        // Create an executable capability for user code at virtual address 0
        // Use the kernel's PCC as base to preserve all execute permissions
        pcb[i].pc = cheri_bounds_set(
            cheri_address_set(cheri_pcc_get(), 0x0),
            0x200000
        );
        
        // Set up user virtual stack 
        // The user code expects stack at 0x1ffff0 with 4KB bounds
        pcb[i].sp = cheri_bounds_set(
            cheri_address_set(cheri_ddc_get(), 0x1ffff0),
            0x1000
        );
        
        pcb[i].state = READY;
    }
    
    current_pid = 0;
    pcb[0].state = RUNNING;
    
    // Debug: Print page table info
    debug_pagetable(0);
    
    // Enable paging with process 0's page table
    w_satp(MAKE_SATP((uint64_t)&page_table[0][0]));
    sfence_vma();
}



void debug_pagetable(int proc) {
    printstring("Page Table Debug for Process ");
    printhex(proc);
    printstring(":\n");
    
    printstring("L2[0] = ");
    printhex(page_table[proc][0]);
    printstring(" -> L1 User Table at ");
    printhex((uint64_t)&user_l1_tables[proc][0]);
    printstring("\n");
    
    printstring("L2[256] = ");
    printhex(page_table[proc][256]);
    printstring(" -> L1 Kernel Table at ");
    printhex((uint64_t)&kernel_l1_tables[proc][0]);
    printstring("\n");
    
    printstring("L1 User[0] = ");
    printhex(user_l1_tables[proc][0]);
    printstring(" -> Physical ");
    printhex(0x80200000ULL + proc * 0x200000ULL);
    printstring("\n");
    
    printstring("L1 Kernel[0] = ");
    printhex(kernel_l1_tables[proc][0]);
    printstring(" -> Physical ");
    printhex(0x80000000ULL);
    printstring("\n");
    
    // Additional debug: Check PA2PTE calculations
    printstring("PA2PTE calculations:\n");
    printstring("User L1 addr: ");
    printhex((uint64_t)&user_l1_tables[proc][0]);
    printstring(" -> PA2PTE: ");
    printhex(PA2PTE((uint64_t)&user_l1_tables[proc][0]));
    printstring("\n");
    
    printstring("Kernel L1 addr: ");
    printhex((uint64_t)&kernel_l1_tables[proc][0]);
    printstring(" -> PA2PTE: ");
    printhex(PA2PTE((uint64_t)&kernel_l1_tables[proc][0]));
    printstring("\n");
}

// void enable_paging(void{
    
//     for (int i = 0; i < NPROC; i++) {
//         // Initialize page table and save its capability
//         pcb[i].page_table_base = init_pagetable(i);

//         // Set up PCC and SP with bounds

//         void* __capability user_code_cap = cheri_bounds_set(cheri_address_set(cheri_pcc_get(), 0x80000000ULL + (i + 1) * 0x200000ULL), 0x200000);
//         void* __capability user_stack_cap = cheri_bounds_set(cheri_address_set(cheri_ddc_get(), 0x80000000ULL + (i + 1) * 0x200000ULL + 0x1ff000), 0x1000);
        
//         pcb[i].pc = user_code_cap;
//         pcb[i].sp = user_stack_cap;
//          // Set up physical base as a capability to physical memory (if needed)
//         pcb[i].phybase = cheri_bounds_set(cheri_address_set(cheri_ddc_get(), 0x80200000ULL + 0x200000 * i), 0x200000);

//         pcb[i].state = NONE;
//         pcb[i].wakeuptime = 0;
//     }

//     current_pid = 0; // Initialize current process ID
//     // Initialize the process control block (PCB) for the first process
//     pcb[0].state = RUNNING; // Set the state of the first process to RUNNING
//     pcb[1].state = READY;
//     pcb[2].state = READY;
//     w_mscratch((uintptr_t)pcb[0].phybase); // Set the scratch register to the physical base address of the first process

//     w_satp(MAKE_SATP((uint64_t) &page_table[0][0])); //
//     sfence_vma(); // Flush TLB
// }