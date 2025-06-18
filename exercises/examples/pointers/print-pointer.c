// print_pointer.c
#include <stdio.h>

int main() {
    int x = 42;
    int *p = &x;

    printf("Value: %d\n", *p);
    printf("Pointer: %p\n", (void *)p);
    return 0;
}

