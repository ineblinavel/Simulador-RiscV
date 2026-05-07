#include "logging.h"
#include "iostream"
#include "options.h"

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