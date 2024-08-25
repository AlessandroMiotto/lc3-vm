#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "lc3vm.h"

// Update RCND in base of r-th sign. Used for condition check
void update_flag(uint16_t *reg, enum regist r)  // as convention, the sign of our value is in the most significant bit
{
    if (reg[r] == 0) reg[RCND] = FZ;            // if regrister r is all zero           0b0000000000000000
    else if (reg[r] >> 15 ) reg[RCND] = FN;     // if regrister r has 15th element one  0b1***************
    else reg[RCND] = FP;                        // if regrister r has 15th element zero 0b0***************
}


// Memory Read Access: return value stored into memory address 
uint16_t mem_read(uint16_t *memory, uint16_t address)
{
    // If memory is mapped keyboard memory
    if (address == MR_KBSR) {
        // to implement
        printf("HIT MAPPED ADDRESS. Not implemented yet!\n");
    }
    
    return memory[address];
}

// Memory Write: write val in memory address
void mem_write(uint16_t *memory, uint16_t address, uint16_t val)
{
    memory[address] = val;
}



// ===================================================================================
// ============================ MATHEMATICAL INSTRUCTIONS ============================
// ===================================================================================

// ==================================== ADD ===========================================
// - add1: add the value of two registers
//          0001|DR1|SR1|0|00|SR2
//          0001|010|000|0|00|001 -> ADD(0001) R2(010) R0(000) R1(001) 
//                                -> add the content of R0 to R1 and store in R2
// - add2: add the constant stored in IMM5 to a register.
//          0001|DR1|SR1|1|IMM05
//          0001|001|000|1|0101 -> ADD(0001) R1(010) R0(000) 5(0101)
//                              -> add 5 to content R0 and store in R1 the result
void OP_ADD(uint16_t *reg, uint16_t instruction)
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

    update_flag(reg, DR1);
}

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

// ==================================== AND ===========================================
// AND: 2 implementation immediate or registers like ADD.
// Compute bitwise and operation between two registers or immediate value
// register mode:   0101|DR1|SR1|000|SR2
// immediate mode:  0101|DR1|SR1|1|IMM05
void OP_AND(uint16_t *reg, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;        // destination register
    uint16_t SR1 = (instruction >> 6) & 0x7;        // first source register
    uint16_t IMM_FLAG = (instruction >> 5) & 0x1;   // if 0 -> add1, 1 -> add2

    if (IMM_FLAG) {
        uint16_t IMM5 = sign_extend(instruction & 0x1F, 5);
        reg[DR1] = reg[SR1] & IMM5;
    }
    else {
        uint16_t SR2 = instruction & 0x7;
        reg[DR1] = reg[SR1] & reg[SR2];
    }

    update_flag(reg, DR1);
}

// ==================================== NOT ===========================================
// NOT: compute bitwise not of source register SR1 and store the result in destination register DR1
// 1001|DR1|SR1|******
void OP_NOT(uint16_t *reg, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;        // destination register
    uint16_t SR1 = (instruction >> 6) & 0x7;        // first source register

    reg[DR1] = ~reg[SR1];
    update_flag(reg, DR1);
}


// ===================================================================================
// ============================= LOAD DATA INSTRUCTIONS ==============================
// ===================================================================================

// ==================================== LDI ===========================================
// LDI: Load Indirect, is used to load a value from a location in memory into a register
//
//                              1010|DR1|PCOFFSET09
// Where: 
//  - 1010 is the operetion code for OP_LDI
//  - DR1 is the destination register that store the loaded value
//  - PCoffset9 is 9 bit address in main memory from PC_START
void OP_LDI(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instruction & 0x1ff, 9);
    reg[DR1] = mem_read(memory, reg[RPC] + PCoffset9);
    update_flag(reg, DR1);
}


// ===================================================================================
// ============================ STORE DATA INSTRUCTIONS ==============================
// ===================================================================================



// ===================================================================================
// ============================ CONTROL FLOW INSTRUCTIONS ============================
// ===================================================================================

// ==================================== JMP ===========================================
// RPC is incremented each instruction executed but we can jump to a
// specific location in memory specified in SR1
// 1100|000|SR1|000000
void OP_JMP(uint16_t *reg, uint16_t instruction)
{
    uint16_t SR1 = (instruction >> 6) & 0x7;
    reg[RPC] = reg[SR1];
}

// ==================================== JSR ===========================================
// JSR is a control flow that help to implement subroutines. It has two formmat:
// - JSR:  0100|1|***OFFSET11   -> 
// - JSRR: 0100|0|00|SR1|000000
// save previous position to register R7
void OP_JSR(uint16_t *reg, uint16_t instruction)
{
    uint16_t JMP_FLAG = (instruction >> 11) & 1;
    reg[R7] = reg[RPC]; // save in R7 where ew branch
    if (JMP_FLAG){
        uint16_t PC_OFFSET = sign_extend(instruction & 0x7FF, 11);
        reg[RPC] += PC_OFFSET;
    }
    else {
        uint16_t SR1 = (instruction >> 6) & 0x7;
        reg[RPC] += SR1;
    }
}

// ==================================== BR ============================================
// BR: conditional jump in base of last operation computed
// - NZP = 001 : jump if positive
// - NZP = 010 : jump if zero
// - NZP = 100 : jump if negative
// 0000|NZP|OFFSET009 
void OP_BR(uint16_t *reg, uint16_t instruction)
{
    uint16_t PC_OFFSET = sign_extend(instruction & 0x1FF, 9);
    uint16_t COND_FLAG = (instruction >> 9) & 0x7;
    if (COND_FLAG & reg[RCND])
        reg[RPC] += PC_OFFSET;
}


// ===================================================================================
// ========================== NOT IMPLEMENTED INSTRUCTIONS ===========================
// ===================================================================================

// ==================================== RTI ===========================================
// Not implemented instruction
void OP_RTI()
{
    printf("RTI instruction is not implemented!\n");
    abort();
}


// ==================================== RES ===========================================
// Not implemented instruction
void OP_RES()
{
    printf("RES instruction is not implemented!\n");
    abort();
}