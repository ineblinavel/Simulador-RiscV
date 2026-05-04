# Teste de escrita fora da faixa de dados
# Tenta fazer store (SW) em endereço inválido
# Esperado: falha em ambos RARS e simulador

.data
buffer: .word 0x00000000

.text
main:
    li t0, 0x00005000    # Endereço bem além do limite de dados
    li t1, 0xDEADBEEF
    sw t1, 0(t0)         # Store em endereço inválido
    
    li a7, 10
    ecall
