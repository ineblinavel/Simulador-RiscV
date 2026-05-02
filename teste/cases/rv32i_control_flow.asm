.data
ok:   .asciz "CTRL OK\n"
fail: .asciz "CTRL FAIL\n"

.text
main:
    addi s0, zero, 0
    jal  ra, func1

    addi t0, zero, 3
    bne  s0, t0, fail_label
    beq  s0, t0, pass_label
    jal  zero, fail_label

func1:
    addi s0, s0, 1
    jal  t1, func2
    addi s0, s0, 1
    jalr zero, ra, 0

func2:
    addi s0, s0, 1
    jalr zero, t1, 0

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
