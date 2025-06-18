
#include <cheri/cheric.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    void *cap = malloc(64);
    void *__capability root_cap = cheri_getdefault();
    void *seal = cheri_maketype(root_cap,1); // Type 1, sealed
    void *dup_seal = cheri_maketype(root_cap,1); //duplicate seal
    void *sealed_cap = cheri_seal(cap, seal);

    printf("Sealed capability: %p\n", sealed_cap);

    // Unsealing
    void *unsealed = cheri_unseal(sealed_cap, seal);
    printf("Unsealed again: %p\n", unsealed);
    void *dup_unseal = cheri_unseal(sealed_cap,dup_seal);
    printf("Unsealed by dup: %p\n", dup_unseal);

    free(cap);
    return 0;
}

