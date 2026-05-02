#include "global.h"
UWord Reg[NUM_REGISTERS] = {0};  // Registradores inicializados com 0
UByte Mem[MEM_SIZE] = {0};         // Memória inicializada com 0
int Out = 0;                      // Inicialização da variável de saída
UWord pc = 0x00000000;  
UWord ri = 0x00000000;  
UWord sp = 0x00003ffc; 
UWord gp = 0x00001800;
instruction_context_st ic;        // Estrutura de contexto da instrução


