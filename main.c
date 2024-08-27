#include "lc3vm.h"
#include "lc3vm.h"
#include <stdint.h>
#include <stdio.h>
//#include <stdlib.h>

int main(int argc, char *argv[])
{    
    // Memory Initialization
    uint16_t memory[MEMORY_MAX] = {0};
    
    // Program write
    uint16_t buffer[7] = {0xF026, 0x1220, 0xF026, 0x1240, 0x1060, 0xF027, 0xF025};
    FILE *fileptr = fopen("program.bin", "wb");
    fwrite(buffer, sizeof(uint16_t), 7, fileptr);
    fclose(fileptr);    

    loadProgram(argv[1], memory);

    programRun(memory);

    return 0;
}
