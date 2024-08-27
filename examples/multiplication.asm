TRAP IN_U16     ; (00) add first number in R1 (counter)
ADD R1 R0 0x0   ;
TRAP IN_U16     ; (02) add second number in R0
ADD R2 R0 0x0   ; (03) replicare R0 in R2
NOT R1 R1       ; (04) decrese by 1 the counter R1
ADD R1 R1 0x1   ; 
NOT R1 R1       ;
ADD R0 R0 R2    ; (07) add R2 at R0
NOT R1 R1       ; (08) decrese by 1 the counter R1
ADD R1 R1 0x1   ;
NOT R1 R1       ;
BR P 0x7        ; (11) repeat from (07) until R1 is not positive
TRAP OUT_U16    ; (12) print the result of multiplication
HALT            ; (13) stop the program