    .arch armv8-a
    .text
    .align  2
start:
    // Give x0 a value
    movz    x0, #0x1234
    // Print x0
    eor     x5, x5, x5
    mvn     x5, x5
    stur    x0, [x5]
    // Give x0 another value, x0 should be zero-ed out
    movz    x0, #0x1234, LSL #16
    // Print new x0
    eor     x5, x5, x5
    mvn     x5, x5
    stur    x0, [x5]
    ret
    .size   start, .-start
    .ident  "GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section    .note.GNU-stack,"",@progbits
