#include "logging.h"
#include "iostream"
#include "options.h"
#include "cpu.h"
#include <iomanip>
#include <sstream>

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
    // ERROR sempre aparece (sem condição)
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