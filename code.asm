TRAP IN_U16         ; write uint16_t in R0 from keyboard
ADD R1 R0 0x00      ; add content of R0 to R1
TRAP IN_U16         ; write uint16_t in R0 from keyboard
ADD R1 R1 R0        ; add content of R0 to R1
ADD R0 R1 0x00      ; add content of R1 to R0
TRAP OUT_U16        ; write to stdout
HALT                ; stop program