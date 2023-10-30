    .arch armv8-a
    .text
    .align  2
start:
    // Use subs
    movz    x1, #1
    movz    x2, #1
    subs    x0, x1, x2
    // Use add (does not change flags)
    add     x0, x0, #1
    movz    x3, #0x1234
    movz    x4, #0x5678
    // check zero flag is still set
    b.eq .L1
    mvn     x5, x5
    stur    x3, [x5]
    ret
.L1:
    mvn     x5, x5
    stur    x4, [x5]
    ret
    .size   start, .-start
    .ident  "GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section    .note.GNU-stack,"",@progbits
