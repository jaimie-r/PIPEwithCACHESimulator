	.arch armv8-a
	.text
	.align	2
start:

	// Populate registers
    // x0 and x1 have invalid addresses
    movz 	x0, #0x1000
    movz    x1, #0x1000
    movz	x2, #0xABCD
    movz    x3, #0xDCBA
    nop
    nop
    nop

    // Try storing in different offsets
    add     x0, x0, #1
    nop
    nop
    nop
    stur    x2, [x0]
    nop
    nop
    nop
    add     x0, x0, #2
    nop
    nop
    nop
    stur    x3, [x0]
    nop
    nop
    nop
    ldur    x4, [x1]
    nop
    nop
    nop

    // Print register
    eor 	x5, x5, x5
    nop
    nop
    nop
    mvn 	x5, x5
    nop
    nop
    nop
    stur    x4, [x5]
    ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
