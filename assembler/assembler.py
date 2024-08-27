def main():
    file = open("code.asm", "r")
    text = file.readlines()
    file.close()
    program = [0] * len(text)
    for i in range (len(text)):
        text[i], _, _ = text[i].partition(";")
        text[i].replace('\n', '')
        parts = text[i].split()
        program[i] = instructions[parts[0]]

        if parts[0] == "TRAP":
           program[i] += traps[parts[1]]
        
        if parts[0] == "ADD" or parts[0] == "AND":
           program[i] += registers[parts[1]] + registers[parts[2]]
           if parts[3] in registers:
               program[i] += "000" + registers[parts[3]]
           else:
               program[i] += '1' + format(int(parts[3], 16), '05b')

        if parts[0] == "LD" or parts[0] == "LDI" or parts[0] == "LEA":
            program[i] += registers[parts[1]] + format(int(parts[2], 16), '05b')

        if parts[0] == "LDR":
            program[i] += registers[parts[1]] + registers[parts[2]] + format(int(parts[3], 16), '05b')
        
        if parts[0] == "ST" or parts[0] == "STI":
            program[i] += registers[parts[1]] + format(int(parts[2], 16), '09b')
        
        if parts[0] == "STR":
            program[i] += registers[parts[1]] + registers[parts[2]] + format(int(parts[3], 16), '06b')
        
        if parts[0] == "JMP":
            program[i] += "000" + registers[parts[1]] + "000000"
        
        if parts[0] == "JSR":
            if parts[1] in registers:
                program[i] += "000" + registers[parts[1]] + "000000"
            else:
                program[i] += "1" + format(int(parts[2], 16), '011b')
        
        if parts[0] == "BR":
            if parts[1] == "P":
                program[i] += "001" + format(int(parts[2], 16), '09b')
            elif parts[1] == "Z":
                program[i] += "010" + format(int(parts[2], 16), '09b')
            else:
                program[i] += "100" + format(int(parts[2], 16), '09b')

        print(program[i], hex(int(program[i], 2)), text[i])
        program[i] = format(int(program[i], 2), '04x')

    print(program)
    binfile = open("program.bin", "wb")
    for inst in program:
        binfile.write(int(format(int(inst, 16), '016b'), 2).to_bytes(2, byteorder='little'))
    binfile.close()
    


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

if __name__ == "__main__":
    main()