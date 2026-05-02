#ifndef CPU_H
#define CPU_H
#include "global.h"

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
};

void loadmemory(CpuState &state, const char* code_path = "teste/code.bin", const char* data_path = "teste/data.bin");
void initialize_cpu_state(CpuState &state);
void run();
extern CpuState cpu_state;

#endif // CPU_H
