#include "global.h"
#include "decode.h"
#include "execute.h"
#include "cpu.h"
#include <iostream>
#include <fstream>
using namespace std;


void loadmem(){
    pc = 0x00000000; // inicializa o pc na .text
    ifstream file("teste/code.bin", ios::binary);
    if (!file.is_open()) {
        cerr << "Não foi possível abrir o arquivo!" << endl;
        return;
    }
    UWord instruction;
    while (file.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) {
        if (pc + 3 < MEM_SIZE) {
            Mem[pc] = instruction & 0xFF;              // Byte 0
            Mem[pc + 1] = (instruction >> 8) & 0xFF;   // Byte 1
            Mem[pc + 2] = (instruction >> 16) & 0xFF;  // Byte 2
            Mem[pc + 3] = (instruction >> 24) & 0xFF;  // Byte 3
        } else {
            cerr << "Erro: Acesso fora dos limites de memória!" << endl;
            cerr << "pc: " << pc << endl;
            cerr << "Erro na leitura do arquivo! code.bin" << endl;
            break;
        }
        pc += 4;
    }
    file.close();
    pc = 0x00002000; // inicializa o pc na .data
    ifstream file2("teste/data.bin", ios::binary);
    if (!file2.is_open()) {
    cerr << "Erro na leitura do arquivo! data.bin" << endl;
        return;
    }
    UWord data;
    while (file2.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        if (pc + 3 < MEM_SIZE) {
            Mem[pc] = data & 0xFF;              // Byte 0
            Mem[pc + 1] = (data >> 8) & 0xFF;   // Byte 1
            Mem[pc + 2] = (data >> 16) & 0xFF;  // Byte 2
            Mem[pc + 3] = (data >> 24) & 0xFF;  // Byte 3
        } else {
            cerr << "Erro: Acesso fora dos limites de memória!" << endl;
            cerr << "pc: " << pc << endl;
            cerr << "Erro na leitura do arquivo! data.bin" << endl;
            break;
        }
        pc += 4;
    }
    file2.close();
    pc = 0; // zera o pc
    Reg[0] = 0; // zera o reg[0]
    Reg[2] = sp;
    Reg[3] = gp;
    return;
}

void fetch() {
    if (pc > INSTRUCTION_MEMORY_LIMIT) {
        Out = 1; // Programa atingiu o fim da memória de instruções
        return;
    }
    UByte byte0 = Mem[pc];
    UByte byte1 = Mem[pc + 1];
    UByte byte2 = Mem[pc + 2];
    UByte byte3 = Mem[pc + 3];
    ri = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}
void step(){
    fetch();
    if (Out != 0) {
        return; // Se o programa já terminou, não execute mais
    }
    decode();
    execute();
    if (Out != 0) {
        return;
    }
    pc += 4;
    return;
}
void run(){
    while(Out == 0){
    step();
    }
    if(Out == 1) {
        cout << endl;
        cout << "-- program is finished running (dropped off bottom) --" << endl; // programa chegou ao final da memoria de instruçoes
    } else if(Out == 2) {
        cout << endl;
        cout << "-- program is finished running (0) --" << endl;  // programa recebeu um ecall de saida
    } else if (Out == 3) {
        cout << "Erro: Execução interrompida por falha em instrução/memória" << endl;
    }else{
        cout << "Erro: Encerramento inesperado" << endl;
    }

}

int main(){
    loadmem();
    run();
    return 0;
}
