#ifndef ISA_H
#define ISA_H

#include <cstdint>


using UByte = std::uint8_t;
using Byte = std::int8_t;
using Word = std::int32_t;
using UWord = std::uint32_t;

constexpr UWord MEM_SIZE = 16384;       // Tamanho da memória
constexpr UByte NUM_REGISTERS = 32;     // Número de registradores

constexpr UWord INSTRUCTION_MEMORY_LIMIT = 0x00000ffc;                             // Limite da memória de instruções
constexpr UWord DATA_MEMORY_LIMIT = 0x00003fff; // Limite da memória de dados

enum InstructionType : Byte {
    NOP = 0,
    UNKNOWN_TYPE = -1,
    R_TYPE = 1, 
    I_TYPE = 2,
    S_TYPE = 3,
    B_TYPE = 4,
    U_TYPE = 5,
    J_TYPE = 6
};
enum Opcode : Byte {
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



#endif // ISA_H
