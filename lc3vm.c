#include "lc3vm.h"
#include <stdint.h>

// MAIN MEMORY
// Main memory, max 65535 instructions (128 kB)
// instructions are encoded in 16 bits:
//	- 4 bits for operation code
//	- 12 bits for 1,2,3... parameters
uint16_t PC_START = 0x3000;
uint16_t memory[UINT16_MAX + 1] = {0};

// Read and Write from the memory
uint16_t mem_read(uint16_t address) { return memory[address]; }
void mem_write(uint16_t address, uint16_t val) { memory[address] = val; }

// REGISTERS
// 8 general porpouse (R0 also for stdin/stdout)
// RPC program counter (memory address of the next instruction)
// RCND consitional register. Give information about previous operation
// RCNT counter register
enum regist { R0 = 0, R1, R2, R3, R4, R5, R6, R7, RPC, RCND, RCNT };
uint16_t reg[RCNT] = {0};

// CONDITION FLAGS
// RCND is used to track information about the most recently executed
// calculation. It is used for implement if condition.
// FP: positive (1)
// FZ: zero (2)
// FN: negative (4)
enum flags {FP = 1 << 0, FZ = 1 << 1, FN = 1 << 2 };
void update_flag(enum regist r)  // as convention, the sign of our value is in the most significant bit
{
    if (reg[r] == 0) reg[RCND] = FZ;            // if regrister r is all zero           0b0000000000000000
    else if (reg[r] >> 15 ) reg[RCND] = FN;     // if regrister r has 15th element one  0b1***************
    else reg[RCND] = FP;                        // if regrister r has 15th element zero 0b0***************
}

// OPERATIONS
// operation are in the first 4 bits of a memory address
// in total we have 16 operation
//op_ex_f op_ex[16] = {OP_BR,  OP_ADD, OP_LD, OP_ST, OP_JSR, OP_AND, OP_LDR, OP_STR,
//                    OP_RTI, OP_NOT, OP_LDI, OP_STI, OP_JMP, OP_RES, OP_LEA, OP_TRAP};


// ================ INSTRUCTIONS =================
// DR: destination register, 3 bits 
// SR: source register, 3 bits
// IMM: immediate value, 5 bits signed integer (two's complment)

// ADD: we define two add function. The 5th bit 

// - add1: add the value of two registers
//          0001|DR1|SR1|0|00|SR2
//          0001|010|000|0|00|001 -> ADD(0001) R2(010) R0(000) R1(001) 
//                                -> add the content of R0 to R1 and store in R2


// - add2: add the constant stored in IMM5 to a register.
//          0001|DR1|SR1|1|IMM5
//          0001|001|000|1|0101 -> ADD(0001) R1(010) R0(000) 5(0101)
//                              -> add 5 to content R0 and store in R1 the result


// IMM5 has only 5 bit (2^5=32 -> -16 to 15), usefull for increment/decrement. We have to estend IMM5 so it can
// be added to a 16 bit number. If IMM5 is positive (most significant digit is zero) we have to simply add zeros.
// For negative we have to extend with ones to preserve the original value
uint16_t sign_extend(uint16_t x, int bit_count)
{
    // if bit_count bit ox x is 1 (x is negative)
    if ((x >> (bit_count - 1)) & 1)
        x |= (0xFFFF << bit_count);
    
    return x;
}

// ADD FUNCTION
void OP_ADD(uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;        // destination register
    uint16_t SR1 = (instruction >> 6) & 0x7;        // first source register
    uint16_t IMM_FLAG = (instruction >> 5) & 0x1;   // if 0 -> add1, 1 -> add2

    if (IMM_FLAG) {
        uint16_t IMM5 = sign_extend(instruction & 0x1F, 5);
        reg[DR1] = reg[SR1] + IMM5;
    }
    else {
        uint16_t SR2 = instruction & 0x7;
        reg[DR1] = reg[SR1] + reg[SR2];
    }

    update_flag(DR1);
}


#include <stdio.h>

int main()
{
    reg[R0] = -2;
    reg[R1] = 3;
    reg[R2] = 0;

    printf("R0: %d\n", reg[R0]);
    printf("R1: %d\n", reg[R1]);
    printf("R2: %d\n\n", reg[R2]);
    
    OP_ADD(0b0001010000000001);

    printf("R0: %d\n", reg[R0]);
    printf("R1: %d\n", reg[R1]);
    printf("R2: %d\n\n", reg[R2]);

    return 0;
}