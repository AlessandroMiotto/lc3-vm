TRAP IN_U16     ; add first number in R1 (counter)
ADD R1 R0 0x00  ;
TRAP IN_U16     ; add second number in R0
ADD R2 R0 0x00  ; replicare R0 in R2
ADD R1 R1 0x1F  ; decrese by 1 the counter R1
ADD R0 R0 R2    ; add R2 at R0
ADD R1 R1 0x1F  ; decrese by 1 the counter R1
BR P 0x1FD      ; repeat from the previous 2 instruction until R1 is not positive
TRAP OUT_U16    ; print the result of multiplication
HALT            ; stop the program