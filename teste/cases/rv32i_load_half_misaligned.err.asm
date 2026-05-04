# Teste de acesso desalinhado em carga (complemento do SW desalinhado)
# Tenta carregar de endereço que não respeita alinhamento de word
# Esperado: falha em ambos RARS e simulador

.data
.align 4
data_area:
    .word 0x11111111
    .word 0x22222222

.text
main:
    la t0, data_area
    addi t0, t0, 3       # t0 aponta para endereço desalinhado (não é múltiplo de 4)
    lw t1, 0(t0)         # Load em endereço desalinhado
    
    li a7, 10
    ecall
