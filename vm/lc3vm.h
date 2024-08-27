#ifndef H_LC3VM
#define H_LC3VM

#include <stdint.h>
#include <stdbool.h>

// MAIN MEMORY
// Main memory, max 65535 instructions (128 kB)
#define MEMORY_MAX (1 << 16)

// REGISTERS
// 8 general porpouse (R0 also for stdin/stdout)
// RPC program counter (memory address of the next instruction)
// RCND conditional register. Give information about previous operation
// RCNT counter register
enum regist{ R0 = 0, R1, R2, R3, R4, R5, R6, R7, RPC, RCND, RCNT };
#define PC_START 0x3000 // default PC start position
#define REG_SIZE RCNT

// CONDITION FLAGS
// RCND is used to track information about the most recently executed
// calculation. It is used for implement if condition.
// FP: positive (1)
// FZ: zero (2)
// FN: negative (4)
enum flags {FP = 1 << 0, FZ = 1 << 1, FN = 1 << 2 };
void update_flag(uint16_t *reg, enum regist r);


// OPERATIONS
// operation are in the first 4 bits of a memory address
// in total we have 2^4 = 16 operation
void OP_BR(uint16_t *reg, uint16_t instruction);                    // 0x0 0000 Conditional Branch 
void OP_ADD(uint16_t *reg, uint16_t instruction);                   // 0x1 0001 Addition
void OP_LD(uint16_t *reg, uint16_t *memory, uint16_t instruction);  // 0x2 0010 Load RPC + offset
void OP_ST(uint16_t *reg, uint16_t *memory, uint16_t instruction);  // 0x3 0011 Store
void OP_JSR(uint16_t *reg, uint16_t instruction);                   // 0x4 0100 Jump to subrutine
void OP_AND(uint16_t *reg, uint16_t instruction);                   // 0x5 0101 Bitwise and
void OP_LDR(uint16_t *reg, uint16_t *memory, uint16_t instruction); // 0x6 0110 Load base + offset
void OP_STR(uint16_t *reg, uint16_t *memory, uint16_t instruction); // 0x7 0111 Store base + offset
void OP_RTI();                                                      // 0x8 1000 Return from intrrupt (not implemented)
void OP_NOT(uint16_t *reg, uint16_t instruction);                   // 0x9 1001 Bitwise not
void OP_LDI(uint16_t *reg, uint16_t *memory, uint16_t instruction); // 0xA 1010 Load Indirect
void OP_STI(uint16_t *reg, uint16_t *memory, uint16_t instruction); // 0xB 1011 Store Indirect
void OP_JMP(uint16_t *reg, uint16_t instruction);                   // 0xC 1100 Jump/return to subrutine
void OP_RES();                                                      // 0xD 1101 Unused (not implemented)
void OP_LEA(uint16_t *reg, uint16_t instruction);                   // 0xE 1110 Load effective address
void OP_TRAP(uint16_t *reg, uint16_t *memory, 						// 0xF 1111 System trap/call
			 uint16_t instruction, bool *running);					

enum op {op_br = 0, op_add, op_ld, op_st, op_jsr, op_and, op_ldr, op_str, op_rti, op_not,
		 op_ldi, op_sti, op_jmp, op_res, op_lea, op_trap};


// TRAP FUNCTIONS
void T_getc(uint16_t *reg);						// 0x20 Read a char from keyboard
void T_out(uint16_t *reg);						// 0x21 Write a char to console
void T_puts(uint16_t *reg, uint16_t *memory);	// 0x22 Write a string of chars to the console
void T_in(uint16_t *reg);						// 0x23 Read a char from keyboard and print to console
void T_putsp(uint16_t *reg, uint16_t *memory);	// 0x24 Output a byte string
void T_halt(bool *running);						// 0x25 Halt the execution
void T_inu16(uint16_t *reg);					// 0x26 Read a uint16_t from terminal
void T_outu16(uint16_t *reg);					// 0x27 Write uint16_t

enum traps {TRAP_GETC = 0x20, TRAP_OUT = 0x21, TRAP_PUTS = 0x22, 
			TRAP_IN = 0x23, TRAP_PUTSP = 0x24, TRAP_HALT = 0x25,
			TRAP_INU16 = 0x26, TRAP_OUTU16 = 0x27 };


// MAPPED REGISTERS
// Memory used for interact with special hardware. LC-3 has keyboard status
// register (if a key is pressed) and keyboard data register (which key)
enum {MR_KBSR = 0xFE00, MR_KBDR = 0xFE02};


// UTILS
uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t mem_read(uint16_t *memory, uint16_t address);
void mem_write(uint16_t *memory, uint16_t address, uint16_t val);

void programRun(uint16_t* memory);
void loadProgram(char* fileName, uint16_t* memory);

#endif
