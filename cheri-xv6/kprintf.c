#include "riscv.h"
#include "hardware.h"
#include "defs.h"

extern volatile struct uart* __capability uart0;
volatile int panicked = 0;

 void putachar(char c){
    while ((uart0->LSR & (1 << 5)) == 0);
    uart0->THR = c;
}

 void printstring(const char *s) {
    while(*s){
        putachar(*s++);
    }
}

 char getachar(void){
    char c;
    while ((uart0->LSR & (1 << 0)) == 0);
    c = uart0->RBR;
    return c;
}

 void panic(char *s){
    printstring("PANIC: ");
    printstring(s);
    panicked = 1;
    while(1); 
}
