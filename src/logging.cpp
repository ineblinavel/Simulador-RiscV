#include "logging.h"
#include "iostream"
#include "options.h"
#include "cpu.h"
#include <iomanip>
#include <sstream>
#include <unistd.h>

void log_info(const string &message) {
    if(options_global.DEBUG_LEVEL >= INFO) {
        cout << "[INFO] " << message << endl;
    }
}

void log_warning(const string &message) {
    if(options_global.DEBUG_LEVEL >= WARN) {
        cerr << "\033[33m[WARNING] " << message << "\033[0m" << endl;
    }
}

void log_error(const string &message) {
    cerr << "\033[31m[ERROR] " << message << "\033[0m" << endl;
}

void print_performance_summary() {
    if(options_global.DEBUG_LEVEL >= INFO) {
        stringstream ss;
        double ipc = (cpu_state.cycles > 0) ? (double)cpu_state.instret / cpu_state.cycles : 0.0;
        
        ss << "\n=== Performance Summary ===\n"
           << "Instructions Retired (instret): " << cpu_state.instret << "\n"
           << "Cycles: " << cpu_state.cycles << "\n"
           << "IPC (Instructions per Cycle): " << fixed << setprecision(2) << ipc << "\n\n"
           << "Instruction Type Distribution:\n"
           << "  ALU: " << cpu_state.alu_count << "\n"
           << "  Branch: " << cpu_state.branch_count << "\n"
           << "  Jump: " << cpu_state.jump_count << "\n"
           << "  Memory: " << cpu_state.memory_count << "\n"
           << "  Other: " << cpu_state.other_count;
        
        log_info(ss.str());
    }
}
string opcode_to_string(Opcode opcode) {
    switch (opcode) {
        case OP_ADD: return "ADD";
        case OP_ADDI: return "ADDI";
        case OP_AND: return "AND";
        case OP_ANDI: return "ANDI";
        case OP_AUIPC: return "AUIPC";
        case OP_BEQ: return "BEQ";
        case OP_BNE: return "BNE";
        case OP_BGE: return "BGE";
        case OP_BGEU: return "BGEU";
        case OP_BLT: return "BLT";
        case OP_BLTU: return "BLTU";
        case OP_JAL: return "JAL";
        case OP_JALR: return "JALR";
        case OP_LB: return "LB";
        case OP_OR: return "OR";
        case OP_LBU: return "LBU";
        case OP_LW: return "LW";
        case OP_LUI: return "LUI";
        case OP_SLT: return "SLT";
        case OP_SLTU: return "SLTU";
        case OP_ORI: return "ORI";
        case OP_SB: return "SB";
        case OP_SLLI: return "SLLI";
        case OP_SRLI: return "SRLI";
        case OP_SRAI: return "SRAI";
        case OP_SUB: return "SUB";
        case OP_SW: return "SW";
        case OP_XOR: return "XOR";
        case OP_ECALL: return "ECALL";
        default: return "UNKNOWN_OPCODE";
    }
}

string format_instruction_assembly(const CpuState& state) {
    stringstream ss;
    Opcode op = state.ic_t.ins_code;
    
    // R-type: rd, rs1, rs2
    if (op == OP_ADD || op == OP_AND || op == OP_OR || op == OP_SLT || 
        op == OP_SLTU || op == OP_SUB) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", x" 
           << state.ic_t.rs1 << ", x" << state.ic_t.rs2;
    }
    // I-type: rd, rs1, imm
    else if (op == OP_ADDI || op == OP_ANDI || op == OP_ORI || op == OP_SLLI || 
             op == OP_SRLI || op == OP_SRAI) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", x" 
           << state.ic_t.rs1 << ", " << state.ic_t.imm_i;
    }
    // Load: rd, imm(rs1)
    else if (op == OP_LB || op == OP_LBU || op == OP_LW) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", " 
           << state.ic_t.imm_i << "(x" << state.ic_t.rs1 << ")";
    }
    // Store: rs2, imm(rs1)
    else if (op == OP_SB || op == OP_SW) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rs2 << ", " 
           << state.ic_t.imm_s << "(x" << state.ic_t.rs1 << ")";
    }
    // Branch: rs1, rs2, imm
    else if (op == OP_BEQ || op == OP_BNE || op == OP_BGE || op == OP_BGEU || 
             op == OP_BLT || op == OP_BLTU) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rs1 << ", x" 
           << state.ic_t.rs2 << ", " << state.ic_t.imm_b;
    }
    // JAL: rd, imm
    else if (op == OP_JAL) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", " 
           << state.ic_t.imm_j;
    }
    // JALR: rd, imm(rs1)
    else if (op == OP_JALR) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", " 
           << state.ic_t.imm_i << "(x" << state.ic_t.rs1 << ")";
    }
    // LUI: rd, imm
    else if (op == OP_LUI) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", 0x" 
           << hex << (state.ic_t.imm_u & 0xFFFFF) << dec;
    }
    // AUIPC: rd, imm
    else if (op == OP_AUIPC) {
        ss << opcode_to_string(op) << " x" << state.ic_t.rd << ", 0x" 
           << hex << (state.ic_t.imm_u & 0xFFFFF) << dec;
    }
    // ECALL
    else if (op == OP_ECALL) {
        ss << "ECALL";
    }
    else {
        ss << opcode_to_string(op);
    }
    
    return ss.str();
}

string get_register_values(const CpuState& state) {
    stringstream ss;
    const Opcode op = state.ic_t.ins_code;
    
    // R-type: rs1, rs2, rd
    if (op == OP_ADD || op == OP_AND || op == OP_OR || op == OP_SLT || 
        op == OP_SLTU || op == OP_SUB) {
        ss << "  [rs1(x" << state.ic_t.rs1 << ")=" << state.Reg[state.ic_t.rs1] 
           << " | rs2(x" << state.ic_t.rs2 << ")=" << state.Reg[state.ic_t.rs2] << "]";
    }
    // I-type: rs1, immediate
    else if (op == OP_ADDI || op == OP_ANDI || op == OP_ORI || op == OP_SLLI || 
             op == OP_SRLI || op == OP_SRAI) {
        ss << "  [rs1(x" << state.ic_t.rs1 << ")=" << state.Reg[state.ic_t.rs1] 
           << " | imm=" << state.ic_t.imm_i << "]";
    }
    // Load
    else if (op == OP_LB || op == OP_LBU || op == OP_LW) {
        ss << "  [rs1(x" << state.ic_t.rs1 << ")=" << state.Reg[state.ic_t.rs1] 
           << " | offset=" << state.ic_t.imm_i << "]";
    }
    // Store
    else if (op == OP_SB || op == OP_SW) {
        ss << "  [rs1(x" << state.ic_t.rs1 << ")=" << state.Reg[state.ic_t.rs1] 
           << " | rs2(x" << state.ic_t.rs2 << ")=" << state.Reg[state.ic_t.rs2] << "]";
    }
    // Branch
    else if (op == OP_BEQ || op == OP_BNE || op == OP_BGE || op == OP_BGEU || 
             op == OP_BLT || op == OP_BLTU) {
        ss << "  [rs1(x" << state.ic_t.rs1 << ")=" << state.Reg[state.ic_t.rs1] 
           << " | rs2(x" << state.ic_t.rs2 << ")=" << state.Reg[state.ic_t.rs2] 
           << " | target=" << state.ic_t.imm_b << "]";
    }
    // JAL/JALR
    else if (op == OP_JAL || op == OP_JALR) {
        if (op == OP_JAL) {
            ss << "  [target=" << state.ic_t.imm_j << "]";
        } else {
            ss << "  [rs1(x" << state.ic_t.rs1 << ")=" << state.Reg[state.ic_t.rs1] 
               << " | offset=" << state.ic_t.imm_i << "]";
        }
    }
    // LUI/AUIPC
    else if (op == OP_LUI || op == OP_AUIPC) {
        ss << "  [imm=0x" << hex << (state.ic_t.imm_u & 0xFFFFF) << dec << "]";
    }
    
    return ss.str();
}

void log_trace(const CpuState& state) {
    if (options_global.TRACE) {
        static int trace_count = 0;
        trace_count++;
        
        stringstream ss;
        
        // Detecta se stderr é terminal ou arquivo
        bool is_terminal = isatty(fileno(stderr));
        
        if (is_terminal) {
            // Terminal: usa cores e boxes
            const char* BORDER = (trace_count % 2 == 0) ? "\033[36m" : "\033[35m";
            const char* PC_COLOR = "\033[32m";
            const char* INSTR_COLOR = "\033[33m";
            const char* ASSEMBLY_COLOR = (trace_count % 2 == 0) ? "\033[97m" : "\033[96m";
            const char* VALUES_COLOR = "\033[90m";
            const char* RESET = "\033[0m";
            
            ss << BORDER << "┌──────────────────────────────────────────────────────────────┐" << RESET << "\n"
               << BORDER << "│ " << RESET
               << "PC: " << PC_COLOR << "0x" << setfill('0') << setw(8) << hex << state.pc << dec 
               << RESET << "  |  " 
               << "Instr: " << INSTR_COLOR << "0x" << setfill('0') << setw(8) << hex << state.ri << dec 
               << RESET
               << BORDER << "  │" << RESET << "\n"
               << BORDER << "├──────────────────────────────────────────────────────────────┤" << RESET << "\n"
               << BORDER << "│ " << RESET
               << ASSEMBLY_COLOR << format_instruction_assembly(state)
               << RESET
               << BORDER << std::string(max(0, 60 - (int)format_instruction_assembly(state).length()), ' ')
               << "│" << RESET << "\n"
               << BORDER << "│" << RESET
               << VALUES_COLOR << get_register_values(state)
               << RESET
               << BORDER << " │" << RESET << "\n"
               << BORDER << "└──────────────────────────────────────────────────────────────┘" << RESET;
        } else {
            // Arquivo: sem cores, separador simples
            ss << "─────────────────────────────────────────────────────────────\n"
               << "PC: 0x" << setfill('0') << setw(8) << hex << state.pc << dec 
               << "  |  Instr: 0x" << setfill('0') << setw(8) << hex << state.ri << dec << "\n"
               << format_instruction_assembly(state) << "\n"
               << get_register_values(state) << "\n"
               << "─────────────────────────────────────────────────────────────";
        }
        
        cerr << ss.str() << endl;
    }
}