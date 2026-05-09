#include "isa.h"
#include "cpu.h"
#include "logging.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;
void zera_r0(){
    cpu_state.Reg[0] = 0;
}

void write_register(UWord rd, UWord value) {
    if (rd == 0) {
        log_warning("Tentativa de escrever em x0 (sempre zero). Valor será ignorado.");
        return;
    }
    cpu_state.Reg[rd] = value;
}

const UWord DATA_MEMORY_BASE = 0x00002000;

bool check_bounds(UWord address, UWord bytes = 1) {
    UWord end = address + bytes - 1;
    if (end < address || address < DATA_MEMORY_BASE || end > DATA_MEMORY_LIMIT || end >= MEM_SIZE) {
        string msg = "Acesso fora dos limites de memória! PC: 0x" + 
                     to_string(cpu_state.pc - 4) + " Endereço: 0x" + 
                     to_string(address);
        log_error(msg);
        cpu_state.Out = OUT_ERROR;
        return false;
    }
    
    // Warning: acesso muito perto do limite
    if (end >= DATA_MEMORY_LIMIT - 16) {
        string msg = "Acesso próximo ao limite de memória de dados! Endereço: 0x" + 
                     to_string(address);
        log_warning(msg);
    }
    
    return true;
}

void move_pc(Word offset){
    cpu_state.pc += offset - 4; // Subtrai 4 para compensar o incremento padrão do PC após a execução da instrução
}

void count_instruction_type(Opcode opcode) {
    switch(opcode) {
        // ALU instructions
        case OP_ADD: case OP_ADDI: case OP_AND: case OP_ANDI: 
        case OP_OR: case OP_ORI: case OP_SLT: case OP_SLTU:
        case OP_SLLI: case OP_SRLI: case OP_SRAI: case OP_SUB:
        case OP_LUI: case OP_AUIPC:
            cpu_state.alu_count++;
            break;
            
        // Branch instructions
        case OP_BEQ: case OP_BNE: case OP_BGE: case OP_BGEU:
        case OP_BLT: case OP_BLTU:
            cpu_state.branch_count++;
            break;
            
        // Jump instructions
        case OP_JAL: case OP_JALR:
            cpu_state.jump_count++;
            break;
            
        // Memory instructions
        case OP_LB: case OP_LBU: case OP_LW: case OP_SB: case OP_SW:
            cpu_state.memory_count++;
            break;
            
        // Other instructions
        case OP_ECALL:
            cpu_state.other_count++;
            break;
            
        default:
            cpu_state.other_count++;
            break;
    }
}

void ADD(UWord rd, UWord rs1, UWord rs2) {  //0
    write_register(rd, cpu_state.Reg[rs1] + cpu_state.Reg[rs2]);
}
void ADDI(UWord rd, UWord rs1, Word imm) { //1
    write_register(rd, (UWord)((Word)cpu_state.Reg[rs1] + imm));
}

void AND(UWord rd, UWord rs1, UWord rs2){ //2
    write_register(rd, cpu_state.Reg[rs1] & cpu_state.Reg[rs2]);
}

void ANDI(UWord rd, UWord rs1, Word imm){  //3
    write_register(rd, cpu_state.Reg[rs1] & imm);
}

void AUIPC(UWord rd, Word imm){
    write_register(rd, imm + cpu_state.pc);
}
void BEQ(UWord rs1, UWord rs2, Word label){ //5
    if((Word)cpu_state.Reg[rs1] == (Word)cpu_state.Reg[rs2]){
        move_pc(label);
    }
}

void BNE(UWord rs1, UWord rs2, Word offset){ //6
    if (cpu_state.Reg[rs1] != cpu_state.Reg[rs2]){
        move_pc(offset);
    }
}

void BGE(UWord rs1, UWord rs2, Word label){ //7
    if((Word)cpu_state.Reg[rs1] >= (Word)cpu_state.Reg[rs2]){
        move_pc(label);
    }
} 
void BGEU(UWord rs1, UWord rs2, Word label){ //8
    UWord temp = cpu_state.Reg[rs2];
    if(cpu_state.Reg[rs1] >= temp){
        move_pc(label);
    }    
} 

void BLT(UWord rs1, UWord rs2, Word label){ //9
    if((Word)cpu_state.Reg[rs1] < (Word)cpu_state.Reg[rs2]){
        move_pc(label);
    }
}

void BLTU(UWord rs1, UWord rs2, Word label){ //10
    UWord temp = cpu_state.Reg[rs2];
    if((UWord)cpu_state.Reg[rs1] < temp){
        move_pc(label);
    }
}

void JAL(UWord rd, Word label){ //11
    write_register(rd, cpu_state.pc + 4);
    move_pc(label);
}

void JALR(UWord rd, UWord rs1, Word label){ //12 
    UWord target = (cpu_state.Reg[rs1] + label) & ~1;
    write_register(rd, cpu_state.pc + 4);
    cpu_state.pc = target - 4;
}

void LB(UWord rd, UWord rs1, Word offset) {
    UWord address = cpu_state.Reg[rs1] + offset;

    if (!check_bounds(address)) {
        return;
    }

    Byte temp = static_cast<Byte>(cpu_state.Mem[address]);
    write_register(rd, static_cast<Word>(temp));
}

void OR(UWord rd, UWord rs1, UWord rs2){ //14
    write_register(rd, cpu_state.Reg[rs1] | cpu_state.Reg[rs2]);
}

void LBU(UWord rd, UWord rs1, Word offset) { //15
    if (!check_bounds(cpu_state.Reg[rs1] + offset)) {
        return;
    }
    UByte temp = cpu_state.Mem[cpu_state.Reg[rs1] + (offset)];  
    write_register(rd, temp);
}


void LW(UWord rd, UWord rs1, Word offset){ //16
    if (!check_bounds(cpu_state.Reg[rs1] + offset, 4)) {
        return;
    }
    Word addr = cpu_state.Reg[rs1] + offset;
    if (addr % 4 != 0) {
        
        string msg = "Endereço de memória não alinhado para LW! PC: 0x" + 
                     to_string(cpu_state.pc - 4) + " Endereço: 0x" + 
                     to_string(addr);
        log_error(msg);
        cpu_state.Out = OUT_ERROR;
        return;
    }
    UByte byte0 = cpu_state.Mem[addr];
    UByte byte1 = cpu_state.Mem[addr + 1];
    UByte byte2 = cpu_state.Mem[addr + 2];
    UByte byte3 = cpu_state.Mem[addr + 3];
    UWord value = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
    write_register(rd, value);
}
void LUI(UWord rd, Word imm){ //17
    write_register(rd, imm);
}
void SLT(UWord rd, UWord rs1, UWord rs2){ // 18
    if((Word)cpu_state.Reg[rs1] < (Word)cpu_state.Reg[rs2]){
        write_register(rd, 0x00000001);
    }else{
        write_register(rd, 0x00000000);
    }
}
void SLTU(UWord rd, UWord rs1, UWord rs2){ //19
    UWord temp = cpu_state.Reg[rs2];
    if(cpu_state.Reg[rs1] < temp){
        write_register(rd, 0x00000001);
    }else{
        write_register(rd, 0x00000000);
    }
}
void ORI(UWord rd, UWord rs1, Word imm){ // 20
    write_register(rd, cpu_state.Reg[rs1] | imm);
}

void SB(UWord rs1, UWord rs2, Word offset){ //21 
    if (!check_bounds(cpu_state.Reg[rs1] + offset)) {
        return;
    }
    Byte temp = cpu_state.Reg[rs2];
    cpu_state.Mem[cpu_state.Reg[rs1] + offset] = temp & 0xFF;
}

void SLLI(UWord rd, UWord rs1, Word imm){  //22
    write_register(rd, (Word)cpu_state.Reg[rs1] << imm);
}

void SRLI(UWord rd, UWord rs1, Word imm){  //23
    write_register(rd, cpu_state.Reg[rs1] >> imm);
}
void SRAI(UWord rd, UWord rs1, Word imm){  // 24
    write_register(rd, ((signed)cpu_state.Reg[rs1]) >> imm);
}

void SUB(UWord rd, UWord rs1, UWord rs2){ // 25
    write_register(rd, cpu_state.Reg[rs1] - cpu_state.Reg[rs2]);
}

void SW(UWord rs2, UWord rs1, Word offset){ //26
    UWord address = cpu_state.Reg[rs1] + offset;
    Word value = cpu_state.Reg[rs2];
    if (!check_bounds(address, 4)) {
        return;
    }

    if (address % 4 != 0) {
        string msg = "Endereço de memória não alinhado para SW! PC: 0x" + 
                     to_string(cpu_state.pc - 4) + " Endereço: 0x" + 
                     to_string(address);
        log_error(msg);
        cpu_state.Out = OUT_ERROR;
        return;
    }
    cpu_state.Mem[address] = value & 0xFF;              
    cpu_state.Mem[address + 1] = (value >> 8) & 0xFF;   
    cpu_state.Mem[address + 2] = (value >> 16) & 0xFF;  
    cpu_state.Mem[address + 3] = (value >> 24) & 0xFF; 
    
}
void XOR(UWord rd, UWord rs1, UWord rs2){ //27
    cpu_state.Reg[rd] = cpu_state.Reg[rs1] ^ cpu_state.Reg[rs2];
}


void print() {
    Word addr = cpu_state.Reg[10];  // Achar o endereço armazenado em Reg[10]
    int i = 0;
    while (cpu_state.Mem[addr + i] != 0) {  // Le cada byte da memoria e imprime o caractere correspondente
        char caracter = static_cast<char>(cpu_state.Mem[addr + i]);  // Converte o byte para um caractere
        cout << caracter;  // Imprime o caractere
        i++;
    }
    return;
}
void ecall() {
    if (cpu_state.Reg[17] == 10) {
        cpu_state.Out = OUT_ECALL; // Sinaliza que o programa terminou por ECALL
    } else if (cpu_state.Reg[17] == 1) {
        cout << (Word)cpu_state.Reg[10]; // Imprime o valor do registrador a0
    }else if (cpu_state.Reg[17] == 4){
        print(); // Imprime a string começando no endereço armazenado em a0
    }else{
        throw runtime_error("Chamada de sistema não implementada");
    }
    return;
}

void execute() {
    Opcode instrucao = cpu_state.ic_t.ins_code;
    zera_r0();
    count_instruction_type(instrucao);
    log_trace(cpu_state);
    try {
        switch (instrucao) {
            case OP_ADD: 
                //cout << "ADD" << endl;
                ADD(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_ADDI: 
                //cout << "ADDI" << endl;
                ADDI(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_AND: 
                //cout << "AND" << endl;
                AND(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_ANDI: 
                //cout << "ANDI" << endl;
                ANDI(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_AUIPC: 
                //cout << "AUIPC" << endl;
                AUIPC(cpu_state.ic_t.rd, cpu_state.ic_t.imm_u);
                break;
            case OP_BEQ: 
                // cout << "BEQ" << endl;
                BEQ(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_b);
                break;
            case OP_BNE: 
                //cout << "BNE" << endl;
                BNE(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_b);
                break;
            case OP_BGE: 
                //cout << "BGE" << endl;
                BGE(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_b);
                break;
            case OP_BGEU: 
                //cout << "BGEU" << endl;
                BGEU(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_b);
                break;
            case OP_BLT: 
                //cout << "BLT" << endl;
                BLT(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_b);
                break;
            case OP_BLTU: 
                //cout << "BLTU" << endl;
                BLTU(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_b);
                break;
            case OP_JAL: 
                //cout << "JAL" << endl;
                JAL(cpu_state.ic_t.rd, cpu_state.ic_t.imm_j);
                break;
            case OP_JALR: 
                //cout << "JALR" << endl;
                JALR(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_LB: 
                // cout << "LB" << endl;
                LB(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_OR: 
                //cout << "OR" << endl;
                OR(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_LBU: 
                //cout << "LBU" << endl;
                LBU(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_LW: 
                LW(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_LUI: 
                //cout << "LUI" << endl;
                LUI(cpu_state.ic_t.rd, cpu_state.ic_t.imm_u);
                break;
            case OP_SLT: 
                //cout << "SLT" << endl;
                SLT(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_SLTU: 
                //cout << "SLTU" << endl;
                SLTU(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_ORI: 
                //cout << "ORI" << endl;
                ORI(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_SB: 
                //cout << "SB" << endl;
                SB(cpu_state.ic_t.rs1, cpu_state.ic_t.rs2, cpu_state.ic_t.imm_s);
                break;
            case OP_SLLI: 
                //cout << "SLLI" << endl;
                SLLI(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_SRLI: 
                //cout << "SRLI" << endl;
                SRLI(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_SRAI: 
                //cout << "SRAI" << endl;
                SRAI(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_i);
                break;
            case OP_SUB: 
                //cout << "SUB" << endl;
                SUB(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_SW: 
                //cout << "SW" << endl;
                SW(cpu_state.ic_t.rs2, cpu_state.ic_t.rs1, cpu_state.ic_t.imm_s);
                break;
            case OP_XOR: 
                //cout << "XOR" << endl;
                XOR(cpu_state.ic_t.rd, cpu_state.ic_t.rs1, cpu_state.ic_t.rs2);
                break;
            case OP_ECALL:
                //cout << "ECALL" << endl;
                ecall();
                break;
            case UNKNOWN_OPCODE:
                throw runtime_error("Instrução desconhecida");
                break;
            default:
                throw runtime_error("Instrução desconhecida");
                break;
        }
    } catch (const exception& e) {
        string msg = "Erro ao executar instrução. Detalhes: " + string(e.what());
        log_error(msg);
        cpu_state.Out = OUT_ERROR;
    }
    // Zera o reg[0] após cada instrução
    zera_r0();
}
