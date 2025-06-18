
.section .text
.global _entry

.extern __start_bss
.extern __stop_bss

# These are typically defined in <cheri/cheric.h> (for C), but for assembly you need to hardcode them.
#.set CHERI_PERM_EXECUTE,     0x00000002
#.set CHERI_PERM_ALL,         0x7fffffff

_entry:
    # --- Clear EXEC permission on DDC (W^X policy) ---
    cspecialr ct1, ddc
    li t2, 0x7ffffffd               # CHERI_PERM_ALL & ~CHERI_PERM_EXECUTE
    candperm  ct1, ct1, t2
    cspecialw ddc, ct1

    # --- Get address of stack0 as a capability ---
1:
    la a0, next

    cspecialr ct3, pcc
    csetaddr  ca0, ct3, a0

    li	      t3, 0x7FFFFFD7 # CHERI_PERM_ALL & ~(CHERI_PERM_STORE | CHERI_PERM_STORE_CAP)
    candperm  ca0, ca0, t3

    li        t3, 1
    csetflags ca0, ca0, t3

    jr.cap    ca0

next:
    # --- Set up stack pointer (as a capability) ---
    # csp = bounded capability to stack0 + offset per hart
    la      t5, stack0
    li      t2, 4096
    csrr    t3, mhartid
    addi    t3, t3, 1
    mul     t4, t2, t3
    
    cspecialr ct1, ddc
    csetaddr  csp, ct1, t5
    csetbounds csp, csp, t4
    cincoffset csp, csp, t4

    # --- Call start using traditional call ---
    call start

spin:
    j spin
    
