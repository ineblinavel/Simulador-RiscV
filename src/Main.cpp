#include "isa.h"
#include "decode.h"
#include "execute.h"
#include "cpu.h"
#include "options.h"
#include "logging.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
    options_global = parse_arguments(argc, argv);
    log_info("Program started.");
    initialize_cpu_state(cpu_state);
    run();
    log_info("Program finished.");
    return 0;
}
