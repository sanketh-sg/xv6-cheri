#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __CHERI_PURE_CAPABILITY__
#include <cheriintrin.h>
#endif

int main() {
    char *buf = malloc(10);
    if (!buf) {
        perror("malloc");
        return 1;
    }

#ifdef __CHERI_PURE_CAPABILITY__
    // Only apply bounds reduction in CHERI mode
    buf = __builtin_cheri_bounds_set(buf, 10);
    printf("[CHERI] Capability bounds set to 10 bytes.\n");
#endif

    strcpy(buf, "Hello");
    buf[10] = 'X';  // Out-of-bounds write

    printf("After overflow: %c\n", buf[10]);
    return 0;
}


