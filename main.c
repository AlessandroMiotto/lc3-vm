#include "lc3vm.h"
#include <stdio.h>

int main()
{
	// Register Initialization
	uint16_t reg[REG_SIZE] = {0};
    reg[RPC] = PC_START;
    
	// Memory Initialization
	uint16_t memory[MEMORY_MAX] = {0};


	/*reg[R0] = 2;
    reg[R1] = 3;
    reg[R2] = 0;

    printf("R0: %d\n", reg[R0]);
    printf("R1: %d\n", reg[R1]);
    printf("R2: %d\n\n", reg[R2]);
    
    OP_ADD(reg, 0b0001010000000001);

    printf("R0: %d\n", reg[R0]);
    printf("R1: %d\n", reg[R1]);
    printf("R2: %d\n\n", reg[R2]);

	OP_ADD(reg, 0b0001000000100010);

	printf("R0: %d\n", reg[R0]);
    printf("R1: %d\n", reg[R1]);
    printf("R2: %d\n\n", reg[R2]);*/

	return 0;
}
