#include <stdint.h>
#include "syscalls.h"
#include <cheri_init_globals.h>
__attribute__ ((aligned (16))) char userstack[4096];

int main(void);

uintptr_t syscall(uintptr_t nr, uintptr_t param) {
    uintptr_t retval = 0;
    asm volatile ("cmove ca7, %0" : : "C" (nr)); // set syscall number in ca7
    asm volatile ("cmove ca0, %0" : : "C" (param)); // set syscall parameter in ca0

    asm volatile ("ecall"); // make the syscall

    asm volatile ("cmove %0, ca0" : "=C" (retval)); // get the return value from ca0
    return retval;

}

void user_printstring(const char *s) {
    syscall(PRINTSTRING, (uintptr_t)s);
}

void user_putachar(char c) {
    syscall(PUTCHAR, c);
}

char user_getachar(void) {
    return (char)syscall(GETCHAR, 0);
}

// char yield(void) {
//     syscall(YIELD, 0);
// }


int main(void) {

    // cheri_init_globals(); 
    volatile char c='A';

    user_printstring("Starting CHERI-XV6...\n"); 
    while (1) {
      user_putachar(c);
      c++;
      if( c > 'Z' ) c = 'A';
    //   yield();
    }
    // user_printstring("Hello, CHERI-XV6!\n");
    return 0;
}
