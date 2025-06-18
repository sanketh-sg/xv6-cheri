#include <stdio.h>

struct buf {
    char buffer[10];
    int i;
} b;

#pragma weak fill_buf
void
fill_buf(char *buf, size_t len)
{
    for (size_t i = 0; i <= len; i++)
        buf[i] = 'b';
}

int
main(void)
{
    b.i = 'c';
    printf("b.i = %c\n", b.i);

    fill_buf(b.buffer, sizeof(b.buffer));

    printf("b.i = %c\n", b.i);

    return 0;
}