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

void printhex(uint64_t x){
    int i;
    char s;

    for(i=60; i>=0; i-=4){
        int digit = (x >> i) & 0xf;
        if (digit < 10)
            s = '0' + digit;
        else
            s = 'a' + digit - 10;
        putachar(s);
    }
}




 void panic(char *s){
    printstring("PANIC: ");
    printstring(s);
    panicked = 1;
    while(1); 
}

void uartintr(void){
    while(1){
        int c = getachar();
        if (c == -1){
            break; // No more characters to read
        }

        putachar(c); // Echo the character back
    }
}