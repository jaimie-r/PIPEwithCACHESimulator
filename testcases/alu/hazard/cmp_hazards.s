    .arch armv8-a
    .text
    .align  2
start:
    // set up
    // running sum
    movz    x0, #1
    // counter
    movz   x1, #0
    movz   x4, 10
    // current term
    movz    x2, #1
    mvn     x5, x5
// start loop
.loop:
    stur    x0, [x5]
    add    x2, x2, #1
    adds    x0, x0, x2
    add     x1, x1, #1
    cmp     x1, x4
    b.ne .loop
    ret
    .size   start, .-start
    .ident  "GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section    .note.GNU-stack,"",@progbits
