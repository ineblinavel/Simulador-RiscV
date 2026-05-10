#include "cpu.h"
#include <stdexcept>

Opcode get_instr_code(UWord opcode, UWord funct3, UWord funct7) {
    if (opcode == 0b0110011) { // Tipo R
        if (funct3 == 0x0 && funct7 == 0x00) return OP_ADD; // ADD
        else if (funct3 == 0x7 && funct7 == 0x00) return OP_AND; // AND
        else if (funct3 == 0x6 && funct7 == 0x00) return OP_OR; // OR
        else if (funct3 == 0x2 && funct7 == 0x00) return OP_SLT; // SLT
        else if (funct3 == 0x3 && funct7 == 0x00) return OP_SLTU; // SLTU
        else if (funct3 == 0x0 && funct7 == 0x20) return OP_SUB; // SUB
        else if (funct3 == 0x4 && funct7 == 0x00) return OP_XOR; // XOR
    }
    else if (opcode == 0b0010011) { // Tipo I
        if (funct3 == 0x0) return OP_ADDI; // ADDI
        else if (funct3 == 0x7) return OP_ANDI; // ANDI
        else if (funct3 == 0x6) return OP_ORI; // ORI
        else if (funct3 == 0x1) return OP_SLLI; // SLLI
        else if (funct3 == 0x5 && funct7 == 0x00) return OP_SRLI; // SRLI
        else if (funct3 == 0x5 && funct7 == 0x20) return OP_SRAI; // SRAI
    }
    else if (opcode == 0b0110111) { // LUI
        return OP_LUI;
    }
    else if (opcode == 0b0010111) { // AUIPC
        return OP_AUIPC;
    }
    else if (opcode == 0b1100011) { // Tipo B
        if (funct3 == 0x0) return OP_BEQ; // BEQ
        else if (funct3 == 0x1) return OP_BNE; // BNE
        else if (funct3 == 0x5) return OP_BGE; // BGE
        else if (funct3 == 0x7) return OP_BGEU; // BGEU
        else if (funct3 == 0x4) return OP_BLT; // BLT
        else if (funct3 == 0x6) return OP_BLTU; // BLTU
    }
    else if (opcode == 0b1101111) { // JAL
        return OP_JAL;
    }
    else if (opcode == 0b1100111) { // JALR
        return OP_JALR;
    }
    else if (opcode == 0b0000011) { // Tipo I (Load)
        if (funct3 == 0x0) return OP_LB; // LB
        else if (funct3 == 0x4) return OP_LBU; // LBU
        else if (funct3 == 0x2) return OP_LW; // LW
    }
    else if (opcode == 0b0100011) { // Tipo S
        if (funct3 == 0x0) return OP_SB; // SB
        else if (funct3 == 0x2) return OP_SW; // SW
    }
    else if (opcode == 0b1110011 && funct3 == 0x0) { // ECALL
        return OP_ECALL;
    }
    return UNKNOWN_OPCODE; // Código de erro se a instrução não for encontrada
}

InstructionType get_i_format(UWord opcode) {
    switch (opcode) {
        case 0x33: // R-type instructions (e.g., ADD, SUB, etc.)
            return R_TYPE; // R_FORMAT
        case 0x13: // I-type instructions (e.g., ADDI, ANDI, etc.)
            return I_TYPE; // I_FORMAT
        case 0x03: // I-type load instructions (e.g., LB, LH, LW)
            return I_TYPE; // I_FORMAT
        case 0x23: // S-type instructions (e.g., SB, SH, SW)
            return S_TYPE; // S_FORMAT
        case 0x63: // B-type instructions (e.g., BEQ, BNE, etc.)
            return B_TYPE; // B_FORMAT
        case 0x37: // U-type instructions (e.g., LUI)
            return U_TYPE; // U_FORMAT
        case 0x17: // U-type instructions (e.g., AUIPC)
            return U_TYPE; // U_FORMAT
        case 0x6F: // J-type instructions (e.g., JAL)
            return J_TYPE; // J_FORMAT
        default:
            return UNKNOWN_TYPE; // UNKNOWN_FORMAT
    }
}
Word get_imm(UWord ri, InstructionType instr_type) {
    Word imm = 0;
    switch (instr_type) {
        case I_TYPE: {
            imm = (ri & 0xFFF00000) >> 20;
            if (imm & 0x00000800) {
                imm |= 0xFFFFF000;
            }
            break;
        }
        case S_TYPE: {
            Word imm7 = (ri & 0xFE000000) >> 25;
            Word imm5 = (ri & 0x00000F80) >> 7;
            imm = (imm7 << 5) | imm5;
            if (imm & 0x00001000) {
                imm |= 0xFFFFE000;
            }
            break;
        }
        case B_TYPE: {
            int bit_12 = (ri >> 31) & 1;
            int bit_11 = (ri >> 7) & 1;
            Word imm6 = (ri & 0x7E000000) >> 25;
            Word imm4 = (ri & 0x00000F00) >> 8;
            imm = (bit_12 << 12) | (bit_11 << 11) | (imm6 << 5) | (imm4 << 1);
            if (bit_12) {
                imm |= 0xFFFFF000;
            }
            break;
        }
        case U_TYPE: {
            imm = ri & 0xFFFFF000;
            break;
        }
        case J_TYPE: {
            int bit_20 = (ri >> 31) & 1;
            int bit_11 = (ri >> 20) & 1;
            Word imm9 = (ri & 0x7FE00000) >> 21;
            Word imm8 = (ri & 0x000FF000) >> 12;
            imm = (bit_20 << 20) | (imm8 << 12) | (bit_11 << 11) | (imm9 << 1);
            if (bit_20) {
                imm |= 0xFFF00000;
            }
            break;
        }
        case R_TYPE:
            imm = 0; // R-type instructions do not have an immediate value
            break;
        case UNKNOWN_TYPE:
            throw std::runtime_error("Formato de instrução desconhecido");
            break;

    }
    return imm;
}

void decode (){
    UWord ri = cpu_state.ri;
    UWord opcode, rs2, rs1, rd, shamt, funct3, funct7;
    opcode	= ri & 0x7F;

    rs2		= (ri >> 20) & 0x1F;
    rs1		= (ri >> 15) & 0x1F;
    rd		= (ri >> 7)  & 0x1F;
    shamt	= (ri >> 20) & 0x1F;
    funct3	= (ri >> 12) & 0x7;
    funct7  = (ri >> 25);				
    cpu_state.ic_t.ins_code = get_instr_code(opcode, funct3, funct7);
    cpu_state.ic_t.ins_format = get_i_format(opcode);
    cpu_state.ic_t.rs1 = rs1;
    cpu_state.ic_t.rs2 = rs2;
    cpu_state.ic_t.rd = rd;
    cpu_state.ic_t.shamt = shamt;
    cpu_state.ic_t.imm_i = cpu_state.ic_t.ins_format == I_TYPE ? get_imm(ri, cpu_state.ic_t.ins_format) : 0;
    cpu_state.ic_t.imm_s = cpu_state.ic_t.ins_format == S_TYPE ? get_imm(ri, cpu_state.ic_t.ins_format) : 0;
    cpu_state.ic_t.imm_j = cpu_state.ic_t.ins_format == J_TYPE ? get_imm(ri, cpu_state.ic_t.ins_format) : 0;
    cpu_state.ic_t.imm_u = cpu_state.ic_t.ins_format == U_TYPE ? get_imm(ri, cpu_state.ic_t.ins_format) : 0;
    cpu_state.ic_t.imm_b = cpu_state.ic_t.ins_format == B_TYPE ? get_imm(ri, cpu_state.ic_t.ins_format) : 0;
}
