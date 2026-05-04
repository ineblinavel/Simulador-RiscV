# Teste de acesso fora da faixa de dados
# Tenta acessar memória muito além do limite de dados (0x3FFF)
# Esperado: falha em ambos RARS e simulador

.data
valid_data: .word 0x12345678

.text
main:
    # Tenta carregar de endereço bem fora dos limites
    # Limite de dados é 0x3FFF, tentamos 0x10000
    li t0, 0x10000
    lw t1, 0(t0)    # Load do endereço inválido
    
    li a7, 10
    ecall
