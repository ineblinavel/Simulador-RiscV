.data
output_ok:   .asciz "Unaligned Access Test: OK (Trap expected or handled)\n"
output_fail: .asciz "Unaligned Access Test: FAIL (Unaligned access did not trap)\n"

# A region of memory to perform accesses on
.align 4
test_data_region:
    .word 0x00000000 # Address 0x0
    .word 0x11111111 # Address 0x4
    .word 0x22222222 # Address 0x8
    .word 0x33333333 # Address 0xC
    .word 0x44444444 # Address 0x10

.text
main:
    # --- Test SW (Store Word) Unaligned ---
    # Attempt to store 0xDEADBEEF at (test_data_region + 1)
    # This should typically cause a store address misaligned exception.
    la   t0, test_data_region
    addi t0, t0, 1          # t0 = test_data_region + 1 (unaligned address)
    li   t1, 0xDEADBEEF      # Value to attempt to store
    sw   t1, 0(t0)           # Store t1 at unaligned address

    # --- Test LW (Load Word) Unaligned ---
    # Attempt to load into t2 from (test_data_region + 2)
    # This should typically cause a load address misaligned exception.
    la   t0, test_data_region
    addi t0, t0, 2          # t0 = test_data_region + 2 (unaligned address)
    lw   t2, 0(t0)           # Load into t2 from unaligned address

    # If the program reaches here, it implies that the unaligned accesses
    # did *not* cause an exception or halt the execution.
    # For a standard RISC-V implementation, this is considered a failure
    # as unaligned word accesses should lead to traps.
    la   a0, output_fail     # Load address of "FAIL" message
    li   a7, 4               # ecall for print string
    ecall

    li   a7, 10              # ecall for exit
    ecall

# This label would only be reached if an exception handler
# somehow redirects execution here after handling the misaligned access.
# This is not common for simple assembly tests, but included for completeness.
handle_exception:
    la   a0, output_ok       # Load address of "OK" message
    li   a7, 4               # ecall for print string
    ecall
    li   a7, 10              # ecall for exit
    ecall
