#include "lc3vm.h"
#include <stdio.h>

int main()
{
	// Register Initialization
	uint16_t reg[REG_SIZE] = {0};
    reg[RPC] = PC_START;
    
	// Memory Initialization
	uint16_t memory[MEMORY_MAX] = {0};

	return 0;
}
