    .arch armv8-a
    .text
    .align  2
start:
    // use adrp
    adrp x0, .LANCHOR0
    nop
    nop
    nop
    add x0, x0, :lo12:.LANCHOR0
    nop
    nop
    nop
    // load in value to x1
    ldur    x1, [x0]
    nop
    nop
    nop
    eor     x5, x5, x5
    nop
    nop
    nop
    mvn     x5, x5
    nop
    nop
    nop
    // print 0? idk what whoever wrote this was thinking -Zach
    stur    x6, [x5]
    ret
    .size   start, .-start
    .data
    .align  4
    .set    .LANCHOR0,. + 0
    .type   myvar, %object
    .size   myvar, 16
myvar:
    .xword  25
    .ident  "GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section    .note.GNU-stack,"",@progbits
