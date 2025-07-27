#include <cheri/cheric.h>

typedef struct {
    void * __capability cra;
    void * __capability csp;
    void * __capability cgp;
    void * __capability ctp;
    void * __capability ct0;
    void * __capability ct1;
    void * __capability ct2;
    void * __capability ca0;
    void * __capability ca1;
    void * __capability ca2;
    void * __capability ca3;
    void * __capability ca4;
    void * __capability ca5;
    void * __capability ca6;
    void * __capability ca7;
    void * __capability cs0;
    void * __capability cs1;
    void * __capability cs2;
    void * __capability cs3;
    void * __capability cs4;
    void * __capability cs5;
    void * __capability cs6;
    void * __capability cs7;
    void * __capability cs8;
    void * __capability cs9;
    void * __capability cs10;
    void * __capability cs11;
    void * __capability ct3;
    void * __capability ct4;
    void * __capability ct5;
    void * __capability ct6;
} cheri_regs;