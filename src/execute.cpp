#include "global.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;
void zera_r0(){
    Reg[0] = 0;
}

const UWord DATA_MEMORY_BASE = 0x00002000;

bool check_bounds(UWord address, UWord bytes = 1) {
    UWord end = address + bytes - 1;
    if (end < address || address < DATA_MEMORY_BASE || end > DATA_MEMORY_LIMIT || end >= MEM_SIZE) {
        cerr << "Erro: Acesso fora dos limites de memória! Endereço: 0x" << hex << address << dec << endl;
        Out = 3;
        return false;
    }
    return true;
}

void move_pc(Word offset){
    pc += offset - 4; // Subtrai 4 para compensar o incremento padrão do PC após a execução da instrução
}

void ADD(UWord rd, UWord rs1, UWord rs2) {  //0
    Reg[rd] = Reg[rs1] + Reg[rs2];  
}
void ADDI(UWord rd, UWord rs1, Word imm) { //1
    Reg[rd] = (UWord)((Word)Reg[rs1] + imm);
}

void AND(UWord rd, UWord rs1, UWord rs2){ //2
    Reg[rd] = Reg[rs1] & Reg[rs2];
}

void ANDI(UWord rd, UWord rs1, Word imm){  //3
    Reg[rd] = Reg[rs1] & imm;
}

void AUIPC(UWord rd, Word imm){
    Reg[rd] = imm + pc;
}
void BEQ(UWord rs1, UWord rs2, Word label){ //5
    if((Word)Reg[rs1] == (Word)Reg[rs2]){
        move_pc(label);
    }
}

void BNE(UWord rs1, UWord rs2, Word offset){ //6
    if (Reg[rs1] != Reg[rs2]){
        move_pc(offset);
    }
}

void BGE(UWord rs1, UWord rs2, Word label){ //7
    if((Word)Reg[rs1] >= (Word)Reg[rs2]){
        move_pc(label);
    }
} 
void BGEU(UWord rs1, UWord rs2, Word label){ //8
    UWord temp = (UWord)Reg[rs2];
    if(Reg[rs1] >= temp){
        move_pc(label);
    }    
} 

void BLT(UWord rs1, UWord rs2, Word label){ //9
    if((Word)Reg[rs1] < (Word)Reg[rs2]){
        move_pc(label);
    }
}

void BLTU(UWord rs1, UWord rs2, Word label){ //10
    UWord temp = (UWord)Reg[rs2];
    if((UWord)Reg[rs1] < temp){
        move_pc(label);
    }
}

void JAL(UWord rd, Word label){ //11
    Reg[rd] = pc + 4;
    move_pc(label);
}

void JALR(UWord rd, UWord rs1, Word label){ //12 
    UWord target = (Reg[rs1] + label) & ~1;
    Reg[rd] = pc + 4;
    pc = target - 4;
}

void LB(UWord rd, UWord rs1, Word offset){ //13
    if (!check_bounds(Reg[rs1] + offset)) {
        return;
    }
    Byte temp = static_cast<Byte>(Mem[Reg[rs1] + offset]);
    Reg[rd] = (Word)temp;
}

void OR(UWord rd, UWord rs1, UWord rs2){ //14
    Reg[rd] = Reg[rs1] | Reg[rs2];
}

void LBU(UWord rd, UWord rs1, Word offset) { //15
    if (!check_bounds(Reg[rs1] + offset)) {
        return;
    }
    UByte temp = Mem[Reg[rs1] + (offset)];  
    Reg[rd] = (Word)temp;                
}


void LW(UWord rd, UWord rs1, Word offset){ //16
    if (!check_bounds(Reg[rs1] + offset, 4)) {
        return;
    }
    Word addr = Reg[rs1] + offset;
    UByte byte0 = Mem[addr];
    UByte byte1 = Mem[addr + 1];
    UByte byte2 = Mem[addr + 2];
    UByte byte3 = Mem[addr + 3];
    Reg[rd] = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}
void LUI(UWord rd, Word imm){ //17
    Reg[rd] = imm;
}
void SLT(UWord rd, UWord rs1, UWord rs2){ // 18
    if((Word)Reg[rs1] < (Word)Reg[rs2]){
        Reg[rd] = 0x00000001;
    }else{
        Reg[rd] = 0x00000000;
    }
}
void SLTU(UWord rd, UWord rs1, UWord rs2){ //19
    UWord temp = (UWord)Reg[rs2];
    if(Reg[rs1] < temp){
        Reg[rd] = 0x00000001;
    }else{
        Reg[rd] = 0x00000000;
    }
}
void ORI(UWord rd, UWord rs1, Word imm){ // 20
    Reg[rd] = Reg[rs1] | imm;
}

void SB(UWord rs1, UWord rs2, Word offset){ //21 
    if (!check_bounds(Reg[rs1] + offset)) {
        return;
    }
    Byte temp = Reg[rs2];
    Mem[Reg[rs1] + offset] = temp & 0xFF;
}

void SLLI(UWord rd, UWord rs1, Word imm){  //22
    Reg[rd] = (Word)Reg[rs1] << imm;
}

void SRLI(UWord rd, UWord rs1, Word imm){  //23
    Reg[rd] = Reg[rs1] >> imm;
}
void SRAI(UWord rd, UWord rs1, Word imm){  // 24
    Reg[rd] = ((signed)Reg[rs1]) >> imm;
}

void SUB(UWord rd, UWord rs1, UWord rs2){ // 25
    Reg[rd] = Reg[rs1] - Reg[rs2];
}

void SW(UWord rs2, UWord rs1, Word offset){ //26
    Word value = Reg[rs2];
    if (!check_bounds(Reg[rs1] + offset, 4)) {
        return;
    }
    Mem[Reg[rs1] + offset] = value & 0xFF;              
    Mem[Reg[rs1] + offset + 1] = (value >> 8) & 0xFF;   
    Mem[Reg[rs1] + offset + 2] = (value >> 16) & 0xFF;  
    Mem[Reg[rs1] + offset + 3] = (value >> 24) & 0xFF; 
}
void XOR(UWord rd, UWord rs1, UWord rs2){ //27
    Reg[rd] = Reg[rs1] ^ Reg[rs2];
}


void print() {
    Word addr = Reg[10];  // Achar o endereço armazenado em Reg[10]
    int i = 0;
    while (Mem[addr + i] != 0) {  // Le cada byte da memoria e imprime o caractere correspondente
        char caracter = static_cast<char>(Mem[addr + i]);  // Converte o byte para um caractere
        cout << caracter;  // Imprime o caractere
        i++;
    }
    return;
}
void ecall() {
    if (Reg[17] == 10) {
        Out = 2;
    } else if (Reg[17] == 1) {
        cout << (Word)Reg[10]; // Imprime o valor do registrador a0
    }else if (Reg[17] == 4){
        print(); // Imprime a string começando no endereço armazenado em a0
    }else{
        throw runtime_error("Chamada de sistema não implementada");
    }
    return;
}

void execute() {
    UWord instrucao = ic.ins_code;
    zera_r0();
    try {
        switch (instrucao) {
            case OP_ADD: 
                //cout << "ADD" << endl;
                ADD(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_ADDI: 
                //cout << "ADDI" << endl;
                ADDI(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_AND: 
                //cout << "AND" << endl;
                AND(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_ANDI: 
                //cout << "ANDI" << endl;
                ANDI(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_AUIPC: 
                //cout << "AUIPC" << endl;
                AUIPC(ic.rd, ic.imm_u);
                break;
            case OP_BEQ: 
                // cout << "BEQ" << endl;
                BEQ(ic.rs1, ic.rs2, ic.imm_b);
                break;
            case OP_BNE: 
                //cout << "BNE" << endl;
                BNE(ic.rs1, ic.rs2, ic.imm_b);
                break;
            case OP_BGE: 
                //cout << "BGE" << endl;
                BGE(ic.rs1, ic.rs2, ic.imm_b);
                break;
            case OP_BGEU: 
                //cout << "BGEU" << endl;
                BGEU(ic.rs1, ic.rs2, ic.imm_b);
                break;
            case OP_BLT: 
                //cout << "BLT" << endl;
                BLT(ic.rs1, ic.rs2, ic.imm_b);
                break;
            case OP_BLTU: 
                //cout << "BLTU" << endl;
                BLTU(ic.rs1, ic.rs2, ic.imm_b);
                break;
            case OP_JAL: 
                //cout << "JAL" << endl;
                JAL(ic.rd, ic.imm_j);
                break;
            case OP_JALR: 
                //cout << "JALR" << endl;
                JALR(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_LB: 
                // cout << "LB" << endl;
                LB(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_OR: 
                //cout << "OR" << endl;
                OR(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_LBU: 
                //cout << "LBU" << endl;
                LBU(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_LW: 
                LW(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_LUI: 
                //cout << "LUI" << endl;
                LUI(ic.rd, ic.imm_u);
                break;
            case OP_SLT: 
                //cout << "SLT" << endl;
                SLT(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_SLTU: 
                //cout << "SLTU" << endl;
                SLTU(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_ORI: 
                //cout << "ORI" << endl;
                ORI(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_SB: 
                //cout << "SB" << endl;
                SB(ic.rs1, ic.rs2, ic.imm_s);
                break;
            case OP_SLLI: 
                //cout << "SLLI" << endl;
                SLLI(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_SRLI: 
                //cout << "SRLI" << endl;
                SRLI(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_SRAI: 
                //cout << "SRAI" << endl;
                SRAI(ic.rd, ic.rs1, ic.imm_i);
                break;
            case OP_SUB: 
                //cout << "SUB" << endl;
                SUB(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_SW: 
                //cout << "SW" << endl;
                SW(ic.rs2, ic.rs1, ic.imm_s);
                break;
            case OP_XOR: 
                //cout << "XOR" << endl;
                XOR(ic.rd, ic.rs1, ic.rs2);
                break;
            case OP_ECALL:
                //cout << "ECALL" << endl;
                ecall();
                break;
            default:
                throw runtime_error("Instrução desconhecida");
                break;
        }
    } catch (const exception& e) {
        cerr << "Erro ao executar a instrucao: " << instrucao << ' ' << e.what() << endl;
        Out = 3;
    }
    // Zera o reg[0] após cada instrução
    zera_r0();
}
