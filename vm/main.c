#include "lc3vm.h"
#include "lc3vm.h"
#include <stdint.h>
#include <stdio.h>
//#include <stdlib.h>

int main()
{    
    // Memory Initialization
    uint16_t memory[MEMORY_MAX] = {0};
    
    // Program load
    loadProgram("assembler/program.bin", memory);

    // Program run
    programRun(memory);

    return 0;
}
