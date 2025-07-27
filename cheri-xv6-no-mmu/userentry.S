.section .text
.globl _user_entry
.type _user_entry, @function



_user_entry:
            # Set up stack pointer - matches the disassembly
            lui     t0, 512              # Load 512 into upper bits  
            addiw   t0, t0, -16          # Add -16, result is 0x1ffff0
            cspecialr ca0, ddc           # Get default data capability into ca0
            csetaddr ca0, ca0, t0        # Set address to stack location (0x1ffff0)
            lui     t1, 1                # Load 0x1000 for bounds
            csetbounds ca0, ca0, t1      # Set bounds to 4KB
            cmove   csp, ca0             # Move to stack pointer

            # Call main function directly using PCC
            cspecialr ca0, pcc           # Get program counter capability
            li     t0, 132               # Offset to main function (0x84)
            csetaddr ca0, ca0, t0        # Set address to main
            cjalr   ca0                  # Call main
spin:
    j spin

.size _user_entry, . - _user_entry
