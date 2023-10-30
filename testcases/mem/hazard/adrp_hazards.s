    .arch armv8-a
    .text
    .align  2
start:
    eor     x5, x5, x5
    mvn     x5, x5
    // use adrp
    adrp x0, .LANCHOR0
    add x0, x0, :lo12:.LANCHOR0
    // load in value to x1 and use immediately after
    ldur    x1, [x0]
    stur    x1, [x5] 
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
