#include "options.h"
#include <iostream>
Options options_global; // Variável global para armazenar as opções


Options parse_arguments(int argc, char *argv[]) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            help_message();
            exit(0);
        } else if (arg == "--debug") {
            string debug_level = argv[++i];
            if (debug_level == "info" || debug_level == "i") {
                options.DEBUG_LEVEL = INFO;
            } else if (debug_level == "warn" || debug_level == "w") {
                options.DEBUG_LEVEL = WARN;
            } else if (debug_level == "error" || debug_level == "e") {
                options.DEBUG_LEVEL = ERROR;
            }
        } else if ((arg == "--asm-file" || arg == "-f") && i + 1 < argc) {
            options.asm_file_path = argv[++i];
        } else if ((arg == "--memory-config" || arg == "-m") && i + 1 < argc) {
            string config = argv[++i];
            if (config == "default") {
                options.memory_config_type = DEFAULT;
            } else if (config == "compactDataAtZero") {
                options.memory_config_type = COMPACT_DATA_AT_ZERO;
            } else if (config == "compactTextAtZero") {
                options.memory_config_type = COMPACT_TEXT_AT_ZERO;
            }
        }
    }
    return options;
}

void help_message() {
    cout << "Usage: ./trabalho [options]\n\n";
    cout << "Options:\n";
    cout << "  -h, --help                          Mostra esta mensagem de ajuda\n";
    cout << "  --debug LEVEL                       Define nível de debug\n";
    cout << "                                      NONE (padrão) | INFO | WARN | ERROR\n";
    cout << "  -a, --asm-file <path>               Caminho para arquivo assembly\n";
    cout << "  -mc, --memory-config <type>         Configuração de memória\n";
    cout << "                                      default (padrão)\n";
    cout << "                                      compactDataAtZero\n";
    cout << "                                      compactTextAtZero\n\n";
    cout << "Exemplos:\n";
    cout << "  ./trabalho                          Executa com .data/.text padrão\n";
    cout << "  ./trabalho --debug INFO             Com resumo de stats\n";
    cout << "  ./trabalho --debug WARN -a prog.asm\n";
}