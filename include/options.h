#ifndef  OPTIONS_H
#define  OPTIONS_H
#include <string>
using namespace std;

enum MemoryConfigType {
    DEFAULT,
    COMPACT_DATA_AT_ZERO,
    COMPACT_TEXT_AT_ZERO
};

enum DebugLevel {
    NONE,
    INFO,
    WARN,
    ERROR,
};
struct Options {
    DebugLevel DEBUG_LEVEL = NONE;   // Ativa mensagens de depuração
    bool TRACE = false;                // Ativa rastreamento detalhado de cada instrução
    string asm_file_path = "";          // Caminho para o arquivo de código assembly
    MemoryConfigType memory_config_type = COMPACT_TEXT_AT_ZERO; // Tipo de configuração de memória (possiveis:  "default", "compactDataAtZero", "compactTextAtZero")
};



Options parse_arguments(int argc, char* argv[]);
void help_message();


extern Options options_global;
#endif // OPTIONS_H