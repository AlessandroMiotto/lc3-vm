# LC-3 Virtual Machine

LC-3 (Little Computer 3) is a simple 16-bit machine which include 4-bit opcodes and a total memory of 128kB. This virtual machine is written in pure C and can execute LC-3 instructions from a custom assembly code.

This project is based on these tutorials: [Write your Own Virtual Machine](https://www.jmeiners.com/lc3-vm/), [Writing a simple 16 bit VM in less than 125 lines of C](https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c#str---store-base--offset)

---

The `vm` directory contains the virtual machine source files that include documentation on each instruction. In `assembler` there is the python script that translates the custom assembly code into a binary file (`program.bin`) and contains information about the semantics of the assembly. This assembly implementation is, for now, very simple and only provides a layer of abstraction over the raw binary code.

Usage example
--------------
LC-3 virtual machine use make as build system and the assembler is written using python3.

1. Clone the repository.
    ```
    git clone https://github.com/AlessandroMiotto/lc3-vm/
    ```
2. Write your assembly to run in the virtual machine in `code.asm`.
3. Compileusing `make`. The build process should return assembly code, processed in binary and hexadecimal, formatted in something like this:
    ```
    1111000000100110 0xf026 TRAP IN_U16
    0001001000100000 0x1220 ADD R1 R0 0x00
    1111000000100110 0xf026 TRAP IN_U16
    0001001001000000 0x1240 ADD R1 R1 R0
    0001000001100000 0x1060 ADD R0 R1 0x00
    1111000000100111 0xf027 TRAP OUT_U16
    1111000000100101 0xf025 HALT
    ```
4. Run your program with `make run`.

Instruction and traps
--------------
This virtual machine implements 14 of LC-3's 16 instruction sets. An instruction is 16 bits (2 bytes) long and the first 4 are reserved for the operation code (OpCode), which specifies which operation to perform.

| Instruction| OpCode (Hex) | OpCode (Bin) | Description
|:---:|:-:|:--:|----------------- 
| BR  |0x0|0000| Conditional branch based on last result 
| ADD |0x1|0001| Addiction between registers
| LD  |0x2|0010| Load data from memory to a register
| ST  |0x3|0011| Store a register to memory
| JSR |0x4|0100| Jump to subrutine
| AND |0x5|0101| Bitwise and between registers
| LDR |0x6|0110| Load register from a specific point
| STR |0x7|0111| Store value from specific point
| RTI |0x8|1000| (not implemented)
| NOT |0x9|1001| Bitwise not of a register
| LDI |0xA|1010| Load register with the value pointed by memory
| STI |0xB|1011| Store intermediate value 
| JMP |0xC|1100| Jump into specific location in memory 
| RES |0xD|1101| (not implemented)
| LEA |0xE|1110| Load memory location address
| TRAP|0xF|1111| System calls

The last operation (TRAP) includes a set of more complex instructions that allow to interact with the I/O. The implemented TRAP instructions are as follows:

| TRAP Instruction| OpCode (Hex) | Description
|:------:|:--:|----------------- 
| GETC   |0x20| Read a char from keyboard 
| OUT    |0x21| Write a char to console
| PUTS   |0x22| Write a string of chars to the console
| IN     |0x23| Read a char from keyboard and print to console
| PUTSP  |0x24| Output a byte string
| HALT   |0x25| Halt the execution
| IN_U16 |0x26| Read a uint16_t from terminal
| OUT_U16|0x27| Write uint16_t

Assembly example
--------------
The assembly example included in this repository (`code.asm`) calculates the sum of two 16-bit numbers entered by the user in the terminal. You can find more assembly samples (e.g. multiplication between two number) in the examples directory.
```assembly
TRAP IN_U16         ; write uint16_t in R0 from keyboard
ADD R1 R0 0x00      ; add content of R0 to R1
TRAP IN_U16         ; write uint16_t in R0 from keyboard
ADD R1 R1 R0        ; add content of R0 to R1
ADD R0 R1 0x00      ; add content of R1 to R0
TRAP OUT_U16        ; write to stdout
HALT                ; stop program
```