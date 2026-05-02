#ifndef CPU_H
#define CPU_H

#include "isa.h"

enum OUT_STATUS {
    RUNNING = 0,     // Simulador em execução
    OUT_MEM_END = 1, // Saída por fim de memória
    OUT_ECALL = 2,   // Saída por instrução ECALL
    OUT_ERROR = 3,   // Saída por erro de memoria ou instrução desconhecida
};

struct CpuState {
    UWord Reg[NUM_REGISTERS]; // Registradores
    UByte Mem[MEM_SIZE];      // Memória principal
    OUT_STATUS Out;           // Tipo de saída (ECALL ou fim de memória)
    UWord pc;                 // Program Counter
    UWord ri;                 // Registrador de instrução
    UWord sp;                 // Stack Pointer
    UWord gp;                 // General Purpose Register
    struct {
        Opcode ins_code;            // Código da instrução
        InstructionType ins_format; // Formato da instrução
        UWord rs1;                  // Registrador 1
        UWord rs2;                  // Registrador 2
        UWord rd;                   // Registrador de destino
        UWord shamt;                // Quantidade de shifts
        Word imm_i;                 // Imediato tipo I
        Word imm_s;                 // Imediato tipo S
        Word imm_j;                 // Imediato tipo J
        Word imm_u;                 // Imediato tipo U
        Word imm_b;                 // Imediato tipo B
    } ic_t;
};

void loadmemory(CpuState &state, const char* code_path = "teste/code.bin", const char* data_path = "teste/data.bin");
void initialize_cpu_state(CpuState &state);
void run();
extern CpuState cpu_state;

#endif // CPU_H
