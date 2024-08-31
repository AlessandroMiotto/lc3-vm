import sys, re


def main():
    # Open file and clean comments and empty lines
    lines = []
    file = open(sys.path[0] + "/../code.asm", "r")
    for line in file:
        l = re.sub(r";.+\n", "", line).strip()
        l = l.replace(';', '')
        if l != "":
            lines.append(l)
    file.close()
    
    program = [0] * len(lines)
    for i in range(len(lines)):
        tokens = lines[i].split(" ")
        if tokens[0] == "HALT":
            program[i] = 0xF025
        else:
            program[i] += instructions[tokens[0]] << 12
        program[i] = instrAttribute(program[i], tokens)

        print(f"0x{hex(0x300 + i)[2:].upper()}: 0b{bin(program[i])[2:].zfill(16)} 0x{hex(program[i])[2:].zfill(4).upper()} {lines[i]}")
    
    binfile = open("assembler/program.bin", "wb")
    for inst in program:
        binfile.write(inst.to_bytes(2, byteorder='little'))
    binfile.close()


def instrAttribute(binary, token):
    # TRAP binary: 1111|0000|TRAPVEC8
    if token[0] == "TRAP":
        binary += traps[token[1]]

    # add/and binarys
    if token[0] == "ADD" or token[0] == "AND":
        binary += (registers[token[1]] << 9) + (registers[token[2]] << 6)
        if token[3] in registers:
            # opcode|DR1|SR1|0|00|SR2
            binary += registers[token[3]]
        else:
            # opcode|DR1|SR1|1|IMM05
            binary += (0b1 << 5) + int(token[3], 16)

    # NOT binary: 1001|DR1|SR1|******
    if token[0] == "NOT":
        binary += (registers[token[1]] << 9) + (registers[token[2]] << 6) + 0b111111

    # LD/LDI/LEA binarys: opcode|DR1|PCOFFSET9
    if token[0] == "LD" or token[0] == "LDI" or token[0] == "LEA":
        binary += (registers[token[1]] << 9) + int(token[2], 16)

    # LDR binary: 0110|DR1|SR1|OFFST6
    if token[0] == "LDR":
        binary += (
            (registers[token[1]] << 9) + (registers[token[2]] << 6) + int(token[3], 16)
        )

    # ST/STI binary: opcode|SR1|PCOFFSET9
    if token[0] == "ST" or token[0] == "STI":
        binary += (registers[token[1]] << 9) + int(token[2], 16)

    # STR binary: 0011|SR1|SR2|OFFST6
    if token[0] == "STR":
        binary += (
            (registers[token[1]] << 9) + (registers[token[2]] << 6) + int(token[3], 16)
        )

    # JMP binary: 1100|000|SR1|000000
    if token[0] == "JMP":
        binary += registers[token[1]] << 6

    # JSR binarys
    if token[0] == "JSR":
        if token[1] in registers:
            # 0100|0|00|SR1|000000
            binary += registers[token[1]] << 6
        else:
            # 0100|1|***OFFSET11
            binary += (0b1 << 11) + int(token[1], 16)

    # BR binarys: 0000|NZP|OFFSET009
    if token[0] == "BR":
        if token[1] == "P":
            binary += (0b001 << 9) + int(token[2], 16)
        elif token[1] == "Z":
            binary += (0b010 << 9) + int(token[2], 16)
        else:
            binary += (0b100 << 9) + int(token[2], 16)

    return binary


registers = {
    "R0": 0x0,
    "R1": 0x1,
    "R2": 0x2,
    "R3": 0x3,
    "R4": 0x4,
    "R5": 0x5,
    "R6": 0x6,
    "R7": 0x7,
}

instructions = {
    "BR": 0x0,
    "ADD": 0x1,
    "LD": 0x2,
    "ST": 0x3,
    "JSR": 0x4,
    "AND": 0x5,
    "LDR": 0x6,
    "STR": 0x7,
    "RTI": 0x8,
    "NOT": 0x9,
    "LDI": 0xA,
    "STI": 0xB,
    "JMP": 0xC,
    "RES": 0xD,
    "LEA": 0xE,
    "TRAP": 0xF,
}

traps = {
    "GETC": 0x20,
    "OUT": 0x21,
    "PUTS": 0x22,
    "IN": 0x23,
    "PUTSP": 0x24,
    "HALT": 0x25,
    "IN_U16": 0x26,
    "OUT_U16": 0x27,
}

if __name__ == "__main__":
    main()
