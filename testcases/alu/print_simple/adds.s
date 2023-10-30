	.arch armv8-a
	.text
	.align	2
start:

    // Put values into the registers
    movz x0, #34
    movz x1, #120
    nop
    nop
    nop
    nop

    adds x2, x0, x1
    nop
    nop
    nop

    // Print x2, should be 154
    eor 	x5, x5, x5
    nop
    nop
    nop
	mvn 	x5, x5
    nop
    nop
    nop
	stur	x2, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
