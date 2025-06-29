.globl exception
.globl kernelvec
.align 4
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
        cspecialr ct0, pcc
        csc ct0, 496(csp)   # Store PCC at end of frame

        # Ensure system register access permission
        cspecialr ct0, pcc
        lui t1, %hi(CHERI_PERM_SYSTEM_REGS)
        candperm ct0, ct0, t1
        cjr ct0

        # Call C trap handler with proper capability setup
        cllc ct0, exception
        cjalr ct0

        # Restore PCC first
        clc ct0, 496(csp)
        cspecialw pcc, ct0

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

        # Restore stack
        cincoffset csp, csp, 512

        # Return with capability-aware instruction
        sret  # CHERI variant of sret that handles capability restoration