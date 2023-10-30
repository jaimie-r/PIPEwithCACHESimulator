    .arch armv8-a
    .text
    .align  2
start:
    mov x5, 0 
    mov x4, 1
    mov x2, 13 // compute factorial(x2)
    mov x0, 1
.L2:
    mov x1, x2
    mov x3, x0
    mov x0, 0
.L4:
    ands x6,x1,x4
    cmp x6,x4
    bne .L3
    adds    x0, x0, x3
.L3:
    lsl x3, x3, 1
    lsr x1, x1, 1
    cmp x1, x5 
    bne .L4
    subs    x2, x2, x4
    bne .L2
    mvn x1, xzr
    stur    x0, [x1]
    ret
    .size   start, .-start