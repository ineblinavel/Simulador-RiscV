.data
w0:   .word 0x80FF7F01
w1:   .word 0x00000000
ok:   .asciz "MEM OK\n"
fail: .asciz "MEM FAIL\n"

.text
main:
    la   t0, w0

    lb   t1, 0(t0)
    addi t2, zero, 1
    bne  t1, t2, fail_label

    lb   t1, 2(t0)
    addi t2, zero, -1
    bne  t1, t2, fail_label

    lbu  t1, 3(t0)
    addi t2, zero, 128
    bne  t1, t2, fail_label

    addi t3, t0, 4
    lw   t4, -4(t3)
    li   t5, 0x80FF7F01
    bne  t4, t5, fail_label

    li   t6, 0x12345678
    sw   t6, 4(t0)
    lw   t1, 4(t0)
    bne  t1, t6, fail_label

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
