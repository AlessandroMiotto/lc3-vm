#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

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

// ==================================== LD ============================================
// LD: load data from main memory to a register DR1
// PC + offset memory address has memory address of the actual address.
//
//                              1010|DR1|PCOFFSET9
// Where: 
//  - 1010 is the operetion code for OP_LDI
//  - DR1 is the destination register that store the loaded value
//  - PCoffset9 is 9 bit address in main memory from PC_START
void OP_LD(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instruction & 0x1FF, 9);
    reg[DR1] = mem_read(memory, reg[RPC] + PCoffset9);
    update_flag(reg, DR1);
}

// ==================================== LDI ===========================================
// LDI: Load Indirect, is used to load a value from a location in memory into a register
// PC + offset memory address has memory address of the actual address.
// 1010|DR1|PCOFFSET9
void OP_LDI(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instruction & 0x1ff, 9);
    reg[DR1] = mem_read(memory, mem_read(memory, reg[RPC] + PCoffset9));
    update_flag(reg, DR1);
}

// ==================================== LDR ===========================================
// LDR: Load Register: is like LD but we specify a base to start offset
// 0110|DR1|SR1|OFFST6
void OP_LDR(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;
    uint16_t SR1 = (instruction >> 6) & 0x7;
    uint16_t SRoffset6 = sign_extend(instruction & 0x3f, 6);
    reg[DR1] = mem_read(memory, reg[SR1] + SRoffset6);
    update_flag(reg, DR1);
}

// ==================================== LEA ===========================================
// LEA Load Effective Address: load memory address to refister DR1 but it does not 
// bring program data into register but only the memory location.
// 1110|DR1|PCOFFSET9
void OP_LEA(uint16_t *reg, uint16_t instruction)
{
    uint16_t DR1 = (instruction >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instruction & 0x1ff, 9);
    reg[DR1] = reg[RPC] + PCoffset9;
    update_flag(reg, DR1);
}

// ===================================================================================
// ============================ STORE DATA INSTRUCTIONS ==============================
// ===================================================================================

// ==================================== ST ============================================
// ST: store the value of a given register SR1 to a memory location OFFSET9.
// 0011|SR1|PCOFFSET9
void OP_ST(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t SR1 = (instruction >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instruction & 0x1FF, 9);
    mem_write(memory, reg[RPC] + PCoffset9, reg[SR1]);
}

// ==================================== STI ============================================
// STI: like ST but instead of writing to memory address directly, it use intermediate
// address from the main memory where the actual address is.
// 0011|SR1|PCOFFSET9
void OP_STI(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t SR1 = (instruction >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instruction & 0x1FF, 9);
    mem_write(memory, mem_read(memory, reg[RPC] + PCoffset9), reg[SR1]);
}

// ==================================== STR ============================================
// STR: like ST but instead of starting from RPC, we can specify another base in SR2.
// 0011|SR1|SR2|OFFST6
void OP_STR(uint16_t *reg, uint16_t *memory, uint16_t instruction)
{
    uint16_t SR1 = (instruction >> 9) & 0x7;
    uint16_t SR2 = (instruction >> 6) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x1FF, 6);
    mem_write(memory, reg[SR2] + offset, reg[SR1]);
}


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
// ============================ TRAP ROUTINE INSTRUCTIONS ============================
// ===================================================================================
// TRAP routines are used for performing common tasks and interacting with the I/O
// TRAP routines are identified by trap code -> 1111|0000|TRAPVEC8
void OP_TRAP(uint16_t *reg, uint16_t *memory, uint16_t instruction, bool *running)
{
    switch (instruction & 0xFF) {
        case TRAP_GETC:
            T_getc(reg);
            break;
        case TRAP_OUT:
            T_out(reg);
            break;
        case TRAP_PUTS:
            T_puts(reg, memory);
            break;
        case TRAP_IN:
            T_in(reg);
            break;
        case TRAP_PUTSP:
            T_putsp(reg, memory);
            break;
        case TRAP_HALT:
            T_halt(running);
            break;
        case TRAP_INU16:
            T_inu16(reg);
            break;
        case TRAP_OUTU16:
            T_outu16(reg);
            break;
    }
}

// TRAP_GETC: Read a char from the keyboard and store in R0
void T_getc(uint16_t *reg) { reg[R0] = getchar(); }

// TRAP_OUT: Print in the terminal the char stored in R0
void T_out(uint16_t *reg) { fprintf(stdout, "%c", (char)reg[R0]); }

// TRAP_PUTS
// Write a string of charaters to the console stored contiguously in the memory starting
// from the address specify in R0
void T_puts(uint16_t *reg, uint16_t *memory)
{
    uint16_t* c = memory + reg[R0];
    while (*c) {
        putc((char)*c, stdout);
        ++c;
    }
    fflush(stdout);
}

// TRAP_IN
// Like TRAP_GETC but print to the console
void T_in(uint16_t *reg)
{
    char c = getchar();
    putc(c, stdout);
    fflush(stdout);
    reg[R0] = (uint16_t)c;
    update_flag(reg, R0);
}

// TRAP_PUTSP
// Output one char per byte, to two byte per word 
void T_putsp(uint16_t *reg, uint16_t *memory)
{
    uint16_t* c = memory + reg[R0];
    while (*c) {
        char c1 = (*c) & 0xFF;
        putc(c1, stdout);
        char c2 = (*c) >> 8;
        if (c2) putc(c2, stdout);
        ++c;
    }
    fflush(stdout);
}

// TRAP_HALT
// Keep track of the running VM in a boolean
void T_halt(bool *running) { *running = false; }

// TRAP_INU16
// Take a uint16_t and store in R0
void T_inu16(uint16_t *reg) { fscanf(stdin, "%hu", &reg[R0]); }

// TRAP_INU16
// Write a uint16_t stored in R0 and print it
void T_outu16(uint16_t *reg) { fprintf(stdout, "%hu\n", reg[R0]); }


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



// ===================================================================================
// ================================== RUN PROGRAM ====================================
// ===================================================================================
void programRun(uint16_t* memory)
{
    // Register Initialization
    uint16_t reg[REG_SIZE] = {0};
    reg[RPC] = PC_START;

    // Start virtual machine
    bool running = true;

    while (running)
    {
        uint16_t instruction = mem_read(memory, reg[RPC]++);
        uint16_t op = instruction >> 12;

        switch (op)
        {
        case op_add:
            OP_ADD(reg, instruction);
            break;
        case op_and:
            OP_AND(reg, instruction);
            break;
        case op_not:
            OP_NOT(reg, instruction);
            break;
        case op_br:
            OP_BR(reg, instruction);
            break;
        case op_jmp:
            OP_JMP(reg, instruction);
            break;
        case op_jsr:
            OP_JSR(reg, instruction);
            break;
        case op_ld:
            OP_LD(reg, memory, instruction);
            break;
        case op_ldi:
            OP_LDI(reg, memory, instruction);
            break;
        case op_ldr:
            OP_LDR(reg, memory, instruction);
            break;
        case op_lea:
            OP_LEA(reg, instruction);
            break;
        case op_st:
            OP_ST(reg, memory, instruction);
            break;
        case op_sti:
            OP_STI(reg, memory, instruction);
            break;
        case op_str:
            OP_STR(reg, memory, instruction);
            break;
        case op_trap:
            OP_TRAP(reg, memory, instruction, &running);
            break;
        case op_res:
            OP_RES();
            break;
        case op_rti:
            OP_RTI();
            break;
        default:
            printf("Instruction not implemented\n");
            abort();   
        }
    }
}


// ===================================================================================
// ================================== RUN PROGRAM ====================================
// ===================================================================================
void loadProgram(char* fileName, uint16_t* memory)
{
    // Open file
    FILE *fileptr = fopen(fileName, "rb");
    if (fileptr == NULL) {
        fprintf(stderr, "Cannot open file %s\n", fileName);
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

    // Save program into memory array
    for (size_t i = 0; i < numInstr; ++i) {
        // printf("%zu: 0x%04X\n", i + 1, program[i]);
        memory[PC_START + i] = program[i];
    }

    // Close file
    fclose(fileptr);
    free(program);
}
