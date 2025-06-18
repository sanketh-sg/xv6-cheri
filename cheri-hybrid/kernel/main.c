#include <stdint.h>
#include <stddef.h>
#include <cheriintrin.h>
#include <cheri/cheric.h>
#include <cheri_init_globals.h>

int main(void);

__attribute__((aligned(16))) char stack0[4096];

struct uart {
   union {
     volatile uint8_t THR; // W = transmit hold register
     volatile uint8_t RBR; // R = receive buffer register
     volatile uint8_t DLL; // R/W = divisor latch low
   };
   union {
     volatile uint8_t IER; // R/W = interrupt enable register
     volatile uint8_t DLH; // R/W = divisor latch high
   };
   union {
     volatile uint8_t IIR; // R = interrupt identif. reg.
     volatile uint8_t FCR; // W = FIFO control reg.
   };
   volatile uint8_t LCR; // R/W = line control register
   volatile uint8_t MCR; // R/W = modem control register
   volatile uint8_t LSR; // R   = line status register
};

volatile struct uart* __capability uart0;

static volatile struct uart* __capability get_uart0(void) {
    void *__capability base = cheri_ddc_get();
    base = __builtin_cheri_cap_from_pointer(base, (void*)0x10000000);
    base = cheri_bounds_set(base, sizeof(struct uart));
    return (volatile struct uart* __capability)base;
}

void out_of_bound(volatile struct uart* __capability uart0) {
    // Make a valid capability to the UART memory
    volatile char *__capability base = (volatile char *__capability)uart0;

    // Bounds are sizeof(struct uart), so this should be valid:
    size_t uart_size = sizeof(struct uart);

    // Now offset just *past* the bounds (this is out of bounds)
    volatile char *__capability oob_ptr = cheri_offset_set(base, uart_size);  // 1 byte past valid

    // Dereference should cause CHERI bounds violation trap
    *oob_ptr = 'A';
}


void putachar(volatile struct uart* __capability uart0, char c) {

    while ((uart0->LSR & (1 << 5)) == 0); // Wait for THRE bit
    uart0->THR = c;
    
}

void printstring(const char *s) {
    // volatile struct uart* __capability uart0 = get_uart0();

    volatile ptraddr_t cheri_addr = cheri_address_get(uart0);
    volatile ptraddr_t cheri_base = cheri_base_get(uart0);
    volatile size_t len = cheri_length_get(uart0);
    volatile size_t offset = cheri_offset_get(uart0);
    volatile size_t perms = cheri_perms_get(uart0);
    volatile _Bool tag = cheri_tag_get(uart0);

    for (size_t i = 0; s[i] != '\0'; i++) {
        putachar(uart0, s[i]);
    }

    out_of_bound(uart0);
    
}

void start(void) {
    // This function is called by the runtime to initialize the kernel
    // and set up the initial environment.
    cheri_init_globals();
    main();
}

int main(void) {
    uart0 = get_uart0();
    printstring("Hallo RISC-V!\n");
    return 0;
}
