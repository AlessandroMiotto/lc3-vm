import sys

# Python script that translate LC3 assembly code 
# to binary file. Input assembly file path and output
# binary code
def main():
    # Load Assembly File
    file = open(sys.path[0] + "/../code.asm", "r")
    text = file.readlines()
    file.close()

    # Program array that contain binary
    program = [0] * len(text)   

    for i in range (len(text)):
        text[i], _, _ = text[i].partition(";")  # Remove comments
        text[i].replace('\n', '')               # Remove end of line
        parts = text[i].split()                 # split each line

        # take instructions attribute
        program[i] = instrAttribute(instructions[parts[0]], parts)

        # print output and transform into hex
        print(program[i], hex(int(program[i], 2)), text[i])
        program[i] = format(int(program[i], 2), '04x')

    # open file and print in binary
    binfile = open("assembler/program.bin", "wb")
    for inst in program:
        binfile.write(int(format(int(inst, 16), '016b'), 2).to_bytes(2, byteorder='little'))
    binfile.close()

# ---------- DICTIONARIES FOR REGISTER -----------
# ------------- AND INSTRUCTIONS -----------------

registers = {
    'R0' : '000',
    'R1' : '001',
    'R2' : '010',
    'R3' : '011',
    'R4' : '100',
    'R5' : '101',
    'R6' : '110',
    'R7' : '111',
}

instructions = {
    'ADD'   : '0001',
    'AND'   : '0101',
    'NOT'   : '1001',
    'TRAP'  : '11110000',
    'HALT'  : '1111000000100101',
    'LD'    : '0010',
    'LDI'   : '1010',
    'LDR'   : '0110',
    'LEA'   : '1110',
    'ST'    : '0011',
    'STI'   : '1011',
    'STR'   : '0111',
    'JMP'   : '1100',
    'JSR'   : '0100',
    'BR'    : '0000',
}

traps = {
    'GETC'      : '00100000',
    'OUT'       : '00100001',
    'PUTS'      : '00100010',
    'IN'        : '00100011',
    'PUTSP'     : '00100100',  
    'IN_U16'    : '00100110',
    'OUT_U16'   : '00100111',
}


def instrAttribute(instruction, parts):
    # TRAP instruction: 1111|0000|TRAPVEC8  
    if parts[0] == "TRAP":
        instruction += traps[parts[1]]
    
    # ADD/AND instructions  
    if parts[0] == "ADD" or parts[0] == "AND":
        instruction += registers[parts[1]] + registers[parts[2]]
        if parts[3] in registers:
            # opcode|DR1|SR1|0|00|SR2
            instruction += "000" + registers[parts[3]]
        else:
            # opcode|DR1|SR1|1|IMM05
            instruction += '1' + format(int(parts[3], 16), '05b')

    # NOT instruction: 1001|DR1|SR1|******
    if parts[0] == "NOT":
        instruction += registers[parts[1]] + registers[parts[2]] + "111111"

    # LD/LDI/LEA instructions: opcode|DR1|PCOFFSET9
    if parts[0] == "LD" or parts[0] == "LDI" or parts[0] == "LEA":
        instruction += registers[parts[1]] + format(int(parts[2], 16), '05b')

    # LDR instruction: 0110|DR1|SR1|OFFST6
    if parts[0] == "LDR":
        instruction += registers[parts[1]] + registers[parts[2]] + format(int(parts[3], 16), '05b')
    
    # ST/STI instruction: opcode|SR1|PCOFFSET9
    if parts[0] == "ST" or parts[0] == "STI":
        instruction += registers[parts[1]] + format(int(parts[2], 16), '09b')
    
    # STR instruction: 0011|SR1|SR2|OFFST6
    if parts[0] == "STR":
        instruction += registers[parts[1]] + registers[parts[2]] + format(int(parts[3], 16), '06b')
    
    # JMP instruction: 1100|000|SR1|000000
    if parts[0] == "JMP":
        instruction += "000" + registers[parts[1]] + "000000"
    
    # JSR instructions
    if parts[0] == "JSR":
        if parts[1] in registers:
            # 0100|0|00|SR1|000000
            instruction += "000" + registers[parts[1]] + "000000"
        else:
            # 0100|1|***OFFSET11
            instruction += "1" + format(int(parts[1], 16), '011b')
    
    # BR instructions: 0000|NZP|OFFSET009
    if parts[0] == "BR":
        if parts[1] == "P":
            instruction += "001" + format(int(parts[2], 16), '09b')
        elif parts[1] == "Z":
            instruction += "010" + format(int(parts[2], 16), '09b')
        else:
            instruction += "100" + format(int(parts[2], 16), '09b')

    return instruction

if __name__ == "__main__":
    main()