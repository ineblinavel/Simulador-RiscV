#include "cpu.h"
#include "decode.h"
#include "execute.h"
#include "options.h"
#include "logging.h"
#include "isa.h"
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
    state.Out = RUNNING;   // Define o estado de saída como RUNNING
    state.pc = 0x00000000; // Inicializa o Program Counter
    state.ri = 0x00000000; // Inicializa o Registrador de Instrução
    state.sp = 0x00003ffc; // Inicializa o Stack Pointer
    state.gp = 0x00001800; // Inicializa o General Purpose Register
    state.ic_t = {};       // Inicializa o contexto da instrução
    state.cycles = 0;      // Inicializa o contador de ciclos
    state.instret = 0;     // Inicializa o contador de instruções executadass
    state.alu_count = 0;   // Inicializa contador ALU
    state.branch_count = 0; // Inicializa contador de branches
    state.jump_count = 0;   // Inicializa contador de jumps
    state.memory_count = 0; // Inicializa contador de memória
    state.other_count = 0;  // Inicializa contador de outras instruções
    loadmemory(state);
}

void loadmemory(CpuState &state, const char *code_path, const char *data_path) {
    state.pc = 0x00000000; // inicializa o pc na .text

    ifstream file(code_path, ios::binary);
    if (!file.is_open()) {
        log_error("Não foi possível abrir o arquivo: " + string(code_path));
        state.Out = OUT_ERROR;
        return;
    }
    UWord instruction;
    while (file.read(reinterpret_cast<char *>(&instruction),
                     sizeof(instruction))) {
        // Valida se instrução não excede limites de .text
        if (state.pc + 3 > INSTRUCTION_MEMORY_LIMIT) {
            log_error("Erro: Instrução excede limite de .text!");
            log_error("PC: 0x" + to_string(state.pc));
            state.Out = OUT_ERROR;
            break;
        }
        state.Mem[state.pc] = instruction & 0xFF;             // Byte 0
        state.Mem[state.pc + 1] = (instruction >> 8) & 0xFF;  // Byte 1
        state.Mem[state.pc + 2] = (instruction >> 16) & 0xFF; // Byte 2
        state.Mem[state.pc + 3] = (instruction >> 24) & 0xFF; // Byte 3
        state.pc += 4;
    }
    file.close();

    state.pc = 0x00002000; // inicializa o pc na .data
    ifstream file2(data_path, ios::binary);
    if (!file2.is_open()) {
        log_error("Não foi possível abrir o arquivo: " + string(data_path));
        state.Out = OUT_ERROR;
        return;
    }
    UWord data;
    while (file2.read(reinterpret_cast<char *>(&data), sizeof(data))) {
        // Valida se dado não excede limites de .data
        if (state.pc + 3 > DATA_MEMORY_LIMIT) {
            log_error("Erro: Dados excedem limite de .data!");
            log_error("PC: 0x" + to_string(state.pc));
            state.Out = OUT_ERROR;
            break;
        }
        state.Mem[state.pc] = data & 0xFF;             // Byte 0
        state.Mem[state.pc + 1] = (data >> 8) & 0xFF;  // Byte 1
        state.Mem[state.pc + 2] = (data >> 16) & 0xFF; // Byte 2
        state.Mem[state.pc + 3] = (data >> 24) & 0xFF; // Byte 3
        state.pc += 4;
    }
    file2.close();
    
    // Reset pc e registradores apenas se carregamento foi bem-sucedido
    if (state.Out != OUT_ERROR) {
        state.pc = 0;     // zera o pc
        state.Reg[0] = 0; // zera o reg[0]
        state.Reg[2] = state.sp;
        state.Reg[3] = state.gp;
    }
    return;
}

void fetch() {
    if (cpu_state.pc > INSTRUCTION_MEMORY_LIMIT) {
        cpu_state.Out =
            OUT_MEM_END; // Programa atingiu o fim da memória de instruções
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
    cpu_state.cycles++;
    if(cpu_state.Out == RUNNING){
        cpu_state.instret++;
    }
    return;
}
void run() {
    while(cpu_state.Out == RUNNING){
        step();
    }
    
    if (cpu_state.Out == OUT_MEM_END) {
        log_info("Programa atingiu o fim da memória de instruções.");
        print_performance_summary();
        cout << endl;
        cout << "-- program is finished running (dropped off bottom) --"
             << endl;
        exit(0);
    } else if (cpu_state.Out == OUT_ECALL) {
        log_info("Programa recebeu um ecall de saida.");
        print_performance_summary();
        cout << endl;
        cout << "-- program is finished running (0) --"
             << endl;
        exit(0);
    } else if (cpu_state.Out == OUT_ERROR) {
        print_performance_summary();
        log_error("Erro: Encerramento por erro de execução");
        exit(1);
    } else {
        print_performance_summary();
        log_error("Erro: Encerramento inesperado");
        exit(1);
    }
}
