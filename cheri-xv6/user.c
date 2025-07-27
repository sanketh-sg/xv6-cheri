#include <stdint.h>
#include "syscalls.h"

__attribute__ ((aligned (16))) char userstack[4096];


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
    syscall(PUTCHAR, (uintptr_t)c);
}

char user_getachar(void) {
    return (char)syscall(GETCHAR, 0);
}

char yield(void) {
    syscall(YIELD, 0);
}


int main(void) {

    char c=0;
    // TODO: Initialize the console and printf
    user_printstring("Starting CHERI-XV6...\n"); 
    user_printstring("Process 2\n")
    while(1) {
      for (c='0'; c <= '9'; c++) {
        putachar(c);
        yield();
      }
    }
    // user_printstring("Hello, CHERI-XV6!\n");
    return 0;
}