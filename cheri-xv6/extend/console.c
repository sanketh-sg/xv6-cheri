#include <stdarg.h>

#include "spinlock.h"
#include "riscv.h"

#define BACKSPACE 0x100
#define C(x) ((x)-'@')

void consputc(int c){
    if(c == BACKSPACE){
        uartputc_sync('/b');
        uartputc_sync(' ');
        uartputc_sync('/b');
    } else {
        uartputc_sync(c);
    }
}

