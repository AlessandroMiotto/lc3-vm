#ifndef H_LC3VM
#define H_LC3VM

#include <stdint.h>

// INSTRUCTION
#define OPC(i) ((i)>>12)    // extract the first 4 bit (operation code)
typedef void (*op_ex_f)(uint16_t instruction);  // function pointer

// OPERATIONS
//void OP_BR(uint16_t instruction);   // 0x0 0000 Conditional Branch 
void OP_ADD(uint16_t instruction);  // 0x1 0001 Addition
//void OP_LD(uint16_t instruction);   // 0x2 0010 Load RPC + offset
//void OP_ST(uint16_t instruction);   // 0x3 0011 Store
//void OP_JSR(uint16_t instruction);  // 0x4 0100 Jump to subrutine
//void OP_AND(uint16_t instruction);  // 0x5 0101 Bitwise and
//void OP_LDR(uint16_t instruction);  // 0x6 0110 Load base + offset
//void OP_STR(uint16_t instruction);  // 0x7 0111 Store base + offset
//void OP_RTI(uint16_t instruction);  // 0x8 1000 Return from intrrupt
//void OP_NOT(uint16_t instruction);  // 0x9 1001 Bitwise not
//void OP_LDI(uint16_t instruction);  // 0xA 1010 Load Indirect
//void OP_STI(uint16_t instruction);  // 0xB 1011 Store Indirect
//void OP_JMP(uint16_t instruction);  // 0xC 1100 Jump/return to subrutine
//void OP_RES(uint16_t instruction);  // 0xD 1101 Unused 
//void OP_LEA(uint16_t instruction);  // 0xE 1110 Load effective address
//void OP_TRAP(uint16_t instruction); // 0xF 1111 System trap/call



// UTILS
uint16_t mem_read(uint16_t address);
void mem_write(uint16_t address, uint16_t val);
uint16_t sign_extend(uint16_t x, int bit_count);

#endif
