#include "lc3vm.h"
#include "lc3vm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{    
    // Memory Initialization
    uint16_t memory[MEMORY_MAX] = {0};
    
    //uint16_t buffer[7] = {0xF026, 0x1220, 0xF026, 0x1240, 0x1060, 0xF027, 0xF025};
    //FILE *fileptr = fopen("test.bin", "wb");
    //fwrite(buffer, sizeof(uint16_t), 7, fileptr);
    //fclose(fileptr);

    FILE *fileptr = fopen(argv[1], "rb");
    if (fileptr == NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        abort();
    }
    
    // Find size of binary file in bit.
    // To find the numbeer of 16 byte instruction
    // we have to divide by 2 because 1 byte = 8 bit
    fseek(fileptr, 0, SEEK_END);
    size_t numInstr = ftell(fileptr) / 2;
    rewind(fileptr);
    
    // Allocate program memory
    uint16_t *program = (uint16_t *)malloc(numInstr * sizeof(uint16_t));
    if (program == NULL) {
        fprintf(stderr, "Cannot allocate %ld bit\n", numInstr * sizeof(uint16_t));
        free(program);
        fclose(fileptr);
        abort();
    }
    
    // Read file
    size_t readElem = fread(program, sizeof(uint16_t), numInstr, fileptr);
    if (readElem != numInstr) {
        fprintf(stderr, "File not read properly\n");
        fclose(fileptr);
        free(program);
        abort();
    }
    
    for (size_t i = 0; i < numInstr; ++i) {
        printf("%zu: 0x%04X\n", i + 1, program[i]);
        memory[PC_START + i] = program[i];
    }
    
    // Close file
    fclose(fileptr);
    free(program);

    programRun(memory);

    return 0;
}
