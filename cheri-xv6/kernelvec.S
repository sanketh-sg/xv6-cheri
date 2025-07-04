.globl trap_handler
.globl kernelvec
.type kernelvec, @function


.align 16
kernelvec:
        # Make room to save registers (capabilities need 2x space)
        cincoffset csp, csp, -512  # 256 bytes for GPRs + 256 for capabilities

        # Save integer registers (now using capability stores)
        csc cra,  0(csp)
        csc csp,  16(csp)  # Note: 16-byte increments for capabilities
        csc cgp,  32(csp)
        csc ctp,  48(csp)   # CHERI tp may contain capability metadata
        csc ct0,  64(csp)
        csc ct1,  80(csp)
        csc ct2,  96(csp)
        csc ca0,  144(csp)  # a0-a7 now capability registers
        csc ca1,  160(csp)
        csc ca2,  176(csp)
        csc ca3,  192(csp)
        csc ca4,  208(csp)
        csc ca5,  224(csp)
        csc ca6,  240(csp)
        csc ca7,  256(csp)
        csc ct3,  432(csp)
        csc ct4,  448(csp)
        csc ct5,  464(csp)
        csc ct6,  480(csp)

        # Save PCC and special registers
        #cspecialr ct0, pcc
        #csc ct0, 496(csp)   # Store PCC at end of frame


        # Call C trap handler with proper capability setup
        cllc ct6, trap_handler
        cjalr ct6

        # Restore integer registers
        clc cra,  0(csp)
        clc csp,  16(csp)
        clc cgp,  32(csp)
        # ctp (hartid) deliberately not restored
        clc ct0,  64(csp)
        clc ct1,  80(csp)
        clc ct2,  96(csp)
        clc ca0,  144(csp)
        clc ca1,  160(csp)
        clc ca2,  176(csp)
        clc ca3,  192(csp)
        clc ca4,  208(csp)
        clc ca5,  224(csp)
        clc ca6,  240(csp)
        clc ca7,  256(csp)
        clc ct3,  432(csp)
        clc ct4,  448(csp)
        clc ct5,  464(csp)
        clc ct6,  480(csp)

        # Restore PCC 
        #clc ct0, 496(csp)
        #cspecialw pcc, ct0

        # Restore stack
        cincoffset csp, csp, 512

        # Return with capability-aware instruction
        sret  

.size kernelvec, . - kernelvec