.extern XYZ
    .entry MAIN

KINITIALVALUE: sub *r4   ,    *r3

LOOP:   jmp L1

    macr M1
sub *r1, *r4
    bne END
    endmacr

    ;comment
    
prn #-5
bne LOOP

M1

L1:     inc K
bne LOOP

END:    stop

STR:    .string "abcdef"


K:      .data 4    ,    -55,4,4,4,6

    macr M2
mov reg1, val
add reg2, reg1
    endmacr

M2
ABC: mov XYZ, *r3
reg1: .data 6,5,-555,66
reg2: .data 6,5,-555,66
val: .string "asfas   %%dfjk"