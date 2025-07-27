.globl trap_handler
.globl kernelvec
.type kernelvec, @function

.align 16
kernelvec:
    # Allocate trap frame: 32 GPRs + 32 CHERI registers (conservatively)
    # CHERI registers are saved every 16 bytes, total 512 bytes
    cincoffset csp, csp, -512

    # Save capability registers (16-byte aligned)
    csc cra,   0(csp)
    csc csp,  16(csp)
    csc cgp,  32(csp)
    csc ctp,  48(csp)
    csc ct0,  64(csp)
    csc ct1,  80(csp)
    csc ct2,  96(csp)
    csc cs0, 112(csp)
    csc cs1, 128(csp)

    csc ca0, 144(csp)
    csc ca1, 160(csp)
    csc ca2, 176(csp)
    csc ca3, 192(csp)
    csc ca4, 208(csp)
    csc ca5, 224(csp)
    csc ca6, 240(csp)
    csc ca7, 256(csp)

    csc cs2, 272(csp)
    csc cs3, 288(csp)
    csc cs4, 304(csp)
    csc cs5, 320(csp)
    csc cs6, 336(csp)
    csc cs7, 352(csp)
    csc cs8, 368(csp)
    csc cs9, 384(csp)
    csc cs10,400(csp)
    csc cs11,416(csp)

    csc ct3, 432(csp)
    csc ct4, 448(csp)
    csc ct5, 464(csp)
    csc ct6, 480(csp)


    # Set up call to trap_handler in C
    cllc ct6, trap_handler
    cjalr ct6  # call trap_handler()

    # Restore capability registers
    clc cra,   0(csp)
    clc csp,  16(csp)
    clc cgp,  32(csp)
    # clc ctp skipped (optional: hartid etc.)
    clc ct0,  64(csp)
    clc ct1,  80(csp)
    clc ct2,  96(csp)
    clc cs0, 112(csp)
    clc cs1, 128(csp)

    clc ca0, 144(csp)
    clc ca1, 160(csp)
    clc ca2, 176(csp)
    clc ca3, 192(csp)
    clc ca4, 208(csp)
    clc ca5, 224(csp)
    clc ca6, 240(csp)
    clc ca7, 256(csp)

    clc cs2, 272(csp)
    clc cs3, 288(csp)
    clc cs4, 304(csp)
    clc cs5, 320(csp)
    clc cs6, 336(csp)
    clc cs7, 352(csp)
    clc cs8, 368(csp)
    clc cs9, 384(csp)
    clc cs10,400(csp)
    clc cs11,416(csp)

    clc ct3, 432(csp)
    clc ct4, 448(csp)
    clc ct5, 464(csp)
    clc ct6, 480(csp)


    # Restore stack
    cincoffset csp, csp, 512

    # Return to interrupted code
    mret

.size kernelvec, . - kernelvec
