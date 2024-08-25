#include "lc3vm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main()
{    
	// Memory Initialization
	uint16_t memory[MEMORY_MAX] = {0};

    uint16_t program[] = {0xF026, 0x1220, 0xF026, 0x1240, 0x1060, 0xF027, 0xF025};

    for (int i = 0; i < 7; i++)
        memory[PC_START + i] = program[i];
    
    programRun(memory);

	return 0;
}
