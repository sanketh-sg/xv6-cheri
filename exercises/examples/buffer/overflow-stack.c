// Purpose: Demonstrate stack buffer overflow
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#pragma weak write_buf
void
write_buf(char *buf, size_t ix)
{
    buf[ix] = 'b';
}

int
main(void)
{
    char upper[0x10]; /* upper is the first array of size 0-15*/
    char lower[0x10];

    printf("upper = %p, lower = %p, diff = %zx\n",
        upper, lower, (size_t)(upper - lower));

    /* Assert that these get placed how we expect */
    assert((ptraddr_t)upper == (ptraddr_t)&lower[sizeof(lower)]); //to check if upper and lower are adjacent

    upper[0] = 'a';
    printf("upper[0] = %c\n", upper[0]);

    write_buf(lower, sizeof(lower)); // writes b to lower[16]

    printf("upper[0] = %c\n", upper[0]);

    return 0;
}
