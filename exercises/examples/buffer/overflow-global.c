#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char global_buffer_a[16];
char global_buffer_b[16];

void write_to_buf(char* buf){
    buf[16] = 'X'; // Intentionally writing beyond the buffer size
}

int main(void){
    // Copy input to global buffer
    strcpy(global_buffer_a, "This is string");
    strcpy(global_buffer_b, "Another test");
    
    printf("Buffer A: %s\n", global_buffer_a);
    printf("Buffer B: %s\n", global_buffer_b);

    // Write to the second global buffer
    write_to_buf(global_buffer_b);

    printf("After Buffer A: %s\n", global_buffer_a);
    printf("After Buffer B: %s\n", global_buffer_b);

    return 0;
}