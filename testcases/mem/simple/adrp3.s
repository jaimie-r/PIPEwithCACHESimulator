    .arch armv8-a
    .text
    .align	2
    .p2align 3,,7

rand_func:
    nop
    nop
    nop
    nop
    nop
    ret
    .size	rand_func, .-rand_func
    .section	.text.startup,"ax",@progbits
    
start:
    // Populate register 
    movz    x0, #0 
    adrp    x0, .TESTOBJ
    nop
    nop
    nop 
    mvn     x5, x5
    nop
    nop
    nop
    stur    x0, [x5]
    nop
    nop
    nop
    adrp    x1, .TESTOBJ    
    ret
    .size	start, .-start
    .data
    .align	4
    .set	.TESTOBJ, . + 0
    .type	allocMem, %object
    .size	allocMem, 8192

// Zeroed out array containing 2 * PAGESIZE bytes
allocMem:
    .zero 8192
    .ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section	.note.GNU-stack,"",@progbits
