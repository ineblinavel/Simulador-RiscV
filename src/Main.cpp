#include "isa.h"
#include "decode.h"
#include "execute.h"
#include "cpu.h"
#include <iostream>
#include <fstream>
using namespace std;


int main(){
    initialize_cpu_state(cpu_state);
    run();
    return 0;
}
