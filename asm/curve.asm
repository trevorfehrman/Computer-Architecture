; for histogram stretch

    LDI R1, 1 ; number of times to print an asterisk on a line
    LDI R2, 1 ; count
    LDI R3, PrintAsterisk
    LDI R4, Loop

    LDI R0, FirstLoop
    CALL R0

    HLT

Loop:
    LDI R0, 10 ; newline's ASCII value
    PRA R0
FirstLoop:
    LDI R0, 64 ; end of loop
    CMP R1, R0
    ADD R1, R1
    LDI R0, 42 ; * in ASCII
    JLE R3
    RET

PrintAsterisk:
    CMP R1, R2
    JLE R4
    PRA R0
    INC R2
    JMP R3