#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>


#define UByte uint8_t
#define Byte int8_t
#define Word int32_t
#define UWord uint32_t

#define MEM_SIZE 16384       // Tamanho da memória
#define NUM_REGISTERS 32     // Número de registradores

const UWord INSTRUCTION_MEMORY_LIMIT = 0x00000ffc;                             // Limite da memória de instruções
const UWord DATA_MEMORY_LIMIT = 0x00003fff; // Limite da memória de dados

extern UWord Reg[NUM_REGISTERS];  // Registradores
extern UByte Mem[MEM_SIZE];        // Memória principal
extern int Out;                   // Tipo de saída (ECALL ou fim de memória)
extern UWord pc;                  // Program Counter
extern UWord ri;                  // Registrador de instrução
extern UWord sp;                  // Stack Pointer
extern UWord gp;                  // General Purpose Register


enum InstructionType {
    UNKNOWN_TYPE = -1,
    R_TYPE = 1, 
    I_TYPE = 2,
    S_TYPE = 3,
    B_TYPE = 4,
    U_TYPE = 5,
    J_TYPE = 6
};
enum Opcode {
    UNKNOWN_OPCODE = -1,
    OP_ADD = 0,
    OP_ADDI = 1,
    OP_AND = 2,
    OP_ANDI = 3,
    OP_AUIPC = 4,
    OP_BEQ = 5,
    OP_BNE = 6,
    OP_BGE = 7,
    OP_BGEU = 8,
    OP_BLT = 9,
    OP_BLTU = 10,
    OP_JAL = 11,
    OP_JALR = 12,
    OP_LB = 13,
    OP_OR = 14,
    OP_LBU = 15,
    OP_LW = 16,
    OP_LUI = 17,
    OP_SLT = 18,
    OP_SLTU = 19,
    OP_ORI = 20,
    OP_SB = 21,
    OP_SLLI = 22,
    OP_SRLI = 23,
    OP_SRAI = 24,
    OP_SUB = 25,
    OP_SW = 26,
    OP_XOR = 27,
    OP_ECALL = 28
};
typedef struct {
    Opcode ins_code;     // Código da instrução
    InstructionType ins_format;   // Formato da instrução
    UWord rs1;          // Registrador 1
    UWord rs2;          // Registrador 2
    UWord rd;           // Registrador de destino
    UWord shamt;        // Quantidade de shifts
    Word imm_i;         // Imediato tipo I
    Word imm_s;         // Imediato tipo S
    Word imm_j;         // Imediato tipo J
    Word imm_u;         // Imediato tipo U
    Word imm_b;         // Imediato tipo B
} instruction_context_st;
extern instruction_context_st ic; // Contexto da instrução


#endif
