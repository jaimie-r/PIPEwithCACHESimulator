    .arch armv8-a
    .text
    .align  2
start:
    // set up
    movz    x0, #10
    movz   x1, #1 
    mvn     x5, x5
// start loop
.loop:
    stur    x0, [x5]
    subs    x0, x0, x1
    b.ne .loop
    ret
    .size   start, .-start
    .ident  "GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section    .note.GNU-stack,"",@progbits
