#include "isa.h"
#include "cpu.h"
#include "decode.h"
#include "execute.h"
#include "options.h"
#include "logging.h"
#include <fstream>
#include <iostream>
using namespace std;

CpuState cpu_state{};

void initialize_cpu_state(CpuState &state) {
    log_info("Initializing CPU state.");

    for (int i = 0; i < NUM_REGISTERS; i++) {
        state.Reg[i] = 0; // Inicializa os registradores com 0
    }
    for (int i = 0; i < MEM_SIZE; i++) {
        state.Mem[i] = 0; // Inicializa a memória com 0
    }
    state.Out = RUNNING; // Define o estado de saída como RUNNING
    state.pc = 0x00000000; // Inicializa o Program Counter
    state.ri = 0x00000000; // Inicializa o Registrador de Instrução
    state.sp = 0x00003ffc; // Inicializa o Stack Pointer
    state.gp = 0x00001800; // Inicializa o General Purpose Register
    state.ic_t = {}; // Inicializa o contexto da instrução
    loadmemory(state);
}


void loadmemory(CpuState &state, const char* code_path, const char* data_path) {
    state.pc = 0x00000000; // inicializa o pc na .text
    
    ifstream file(code_path, ios::binary);
    if (!file.is_open()) {
        log_error("Não foi possível abrir o arquivo!");
        return;
    }
    UWord instruction;
    while (file.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) {
        if (state.pc + 3 < MEM_SIZE) {
            state.Mem[state.pc] = instruction & 0xFF;              // Byte 0
            state.Mem[state.pc + 1] = (instruction >> 8) & 0xFF;   // Byte 1
            state.Mem[state.pc + 2] = (instruction >> 16) & 0xFF;  // Byte 2
            state.Mem[state.pc + 3] = (instruction >> 24) & 0xFF;  // Byte 3
        } else {
            log_error("Erro: Acesso fora dos limites de memória!");
            log_error("pc: " + to_string(state.pc));
            log_error("Erro na leitura do arquivo! code.bin");
            break;
        }
        state.pc += 4;
    }
    file.close();

    state.pc = 0x00002000; // inicializa o pc na .data
    ifstream file2(data_path, ios::binary);
    if (!file2.is_open()) {
        log_error("Erro na leitura do arquivo! data.bin");
        return;
    }
    UWord data;
    while (file2.read(reinterpret_cast<char *>(&data), sizeof(data))) {
        if (state.pc + 3 < MEM_SIZE) {
            state.Mem[state.pc] = data & 0xFF;             // Byte 0
            state.Mem[state.pc + 1] = (data >> 8) & 0xFF;  // Byte 1
            state.Mem[state.pc + 2] = (data >> 16) & 0xFF; // Byte 2
            state.Mem[state.pc + 3] = (data >> 24) & 0xFF; // Byte 3
        } else {
            log_error("Erro: Acesso fora dos limites de memória!");
            log_error("pc: " + to_string(state.pc));
            log_error("Erro na leitura do arquivo! data.bin");
            break;
        }
        state.pc += 4;
    }
    file2.close();
    state.pc = 0;     // zera o pc
    state.Reg[0] = 0; // zera o reg[0]
    state.Reg[2] = state.sp;
    state.Reg[3] = state.gp;
    return;
}

void fetch() {
    if (cpu_state.pc > INSTRUCTION_MEMORY_LIMIT) {
        cpu_state.Out = OUT_MEM_END; // Programa atingiu o fim da memória de instruções
        return;
    }
    UByte byte0 = cpu_state.Mem[cpu_state.pc];
    UByte byte1 = cpu_state.Mem[cpu_state.pc + 1];
    UByte byte2 = cpu_state.Mem[cpu_state.pc + 2];
    UByte byte3 = cpu_state.Mem[cpu_state.pc + 3];
    cpu_state.ri = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}
void step() {
    fetch();
    if (cpu_state.Out != 0) {
        return; // Se o programa já terminou, não execute mais
    }
    decode();
    execute();
    if (cpu_state.Out != 0) {
        return;
    }
    cpu_state.pc += 4;
    return;
}
void run() {
    while (cpu_state.Out == 0) {
        step();
    }
    if (cpu_state.Out == OUT_MEM_END) {
        log_info("Programa atingiu o fim da memória de instruções.");
        cout << endl;
        cout << "-- program is finished running (dropped off bottom) --" << endl; // programa chegou ao final da memoria de instruçoes
        exit(0);
    } else if (cpu_state.Out == OUT_ECALL) {
        log_info("Programa recebeu um ecall de saida.");
        cout << endl;
        cout << "-- program is finished running (0) --" << endl; // programa recebeu um ecall de saida
        exit(0);
    } else if (cpu_state.Out == OUT_ERROR) {
        log_error("Erro: Encerramento por erro de execução");
        exit(1); // programa terminou por erro de execução
    } else {
        log_error("Erro: Encerramento inesperado");
        exit(1);
    }
}
