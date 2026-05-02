.data
ok:   .asciz "X0/SHIFT OK\n"
fail: .asciz "X0/SHIFT FAIL\n"

.text
main:
    addi t0, zero, 123
    addi zero, t0, 7
    bne  zero, x0, fail_label

    addi t1, zero, -1
    srli t2, t1, 31
    addi t3, zero, 1
    bne  t2, t3, fail_label

    srai t4, t1, 31
    addi t5, zero, -1
    bne  t4, t5, fail_label

pass_label:
    la   a0, ok
    addi a7, zero, 4
    ecall
    addi a7, zero, 10
    ecall

fail_label:
    la   a0, fail
    addi a7, zero, 4
    ecall
    addi a7, zero, 10
    ecall
