#include <stdint.h>
#include "defs.h"
volatile static int started = 0;
int main(void) {
    // TODO: Initialize the console and printf
    printstring("Starting CHERI-XV6...\n");
    trapinit();
    asm volatile("ecall");
    printstring("Hello, CHERI-XV6!\n");
    return 0;
}