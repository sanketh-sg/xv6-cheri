.section .text
.global _entry

_entry:
    # --- Clear EXEC permission on DDC (W^X policy) ---
    cspecialr ct1, ddc
    li t2, 0x7ffffffd
    candperm  ct1, ct1, t2
    cspecialw ddc, ct1

    # --- Get stack pointer capability ---
1:
    #auipcc    ca0, %pcrel_hi(next)
    #cincoffset ca0, ca0, %pcrel_lo(1b)
    cllc   ca0,    next

    cspecialr ct3, pcc
    csetaddr  ca0, ct3, a0

    li	      t5, 0x7FFFFFFF
    candperm  ca0, ca0, t5

    li        t3, 1
    csetflags ca0, ca0, t3
    jr.cap    ca0

next:
    # Load stack0 into ca0
    #auipcc      ca0, %pcrel_hi(stack0)
    #cincoffset  ca0, ca0, %pcrel_lo(next)
    cllc   ca0,    stack0

    # Set bounds to entire 128 KB region
    li          t0, 128*1024
    csetbounds  ca0, ca0, t0

    # Calculate offset into per-hart stack
    csrr        t1, mhartid
    addi        t1, t1, 1
    li          t2, 4096
    mul         t3, t1, t2

    # Restrict bounds to just this hart's slice (optional but good practice)
    cincoffset  ca0, ca0, t3      # move to base of this hart's stack slice
    csetbounds  ca0, ca0, t2      # restrict bounds to 4 KiB
    cincoffset  ca0, ca0, t2      # move to top of the slice (stack grows down)

    # Set csp = ca0
    cmove       csp, ca0

3:
    #auipcc    ca0, %pcrel_hi(start)
    #cincoffset ca0, ca0, %pcrel_lo(3b)
    cllc    ca0,    start
    cjalr     cra, ca0              # call start() in kernel.c

spin:
    j spin
