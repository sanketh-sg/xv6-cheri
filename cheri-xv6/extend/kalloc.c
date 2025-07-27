#include "riscv.h"
#include "defs.h"
#include "params.h"


extern char end[]; // first address after kernel, defined by kernel.ld.


struct run {
  struct run * __capability next;
};

struct  {
  struct run * __capability freelist;
}kmem;

void kinit() {
    void * __capability kend = cheri_address_set(cheri_ddc_get(), (ptraddr_t) end);
    freerange(kend, cheri_address_set(cheri_ddc_get(), PHYSTOP));
}

void freerange(void * __capability pa_start, void * __capability pa_end) {
    char * __capability p = (char * __capability) PGROUNDUP((uintptr_t) pa_start);
    for (; p + PGSIZE <= (char * __capability) pa_end; p += PGSIZE) {
        kfree(p);
    }
}


void kfree(void * __capability pa) {
    struct run * __capability r;
    if ((uintptr_t) pa % PGSIZE != 0 ||
        (uintptr_t) pa < (uintptr_t) end ||
        (uintptr_t) pa >= PHYSTOP) {
        panic("kfree");
    }
    memset(pa, 1, PGSIZE); // fill with junk to catch dangling refs
    r = (struct run * __capability) pa;
    r->next = kmem.freelist;
    kmem.freelist = r;
}

void * __capability kalloc() {
    struct run * __capability r;
    if ((r = kmem.freelist) == 0) {
        return 0;
    }
    kmem.freelist = r->next;
    memset(r, 5, PGSIZE); // fill with junk to catch dangling refs
    return (void * __capability) r;
}