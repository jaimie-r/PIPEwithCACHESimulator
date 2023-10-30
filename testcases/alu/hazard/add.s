	.arch armv8-a
	.text
	.align	2
start:

    // Put values into the registers
    movz x0, #12
    movz x1, #13
    movz x2, #90

    add x0, x0, #90
    add x4, x0, #13

    // Print x4, should be 115
    eor 	x5, x5, x5
	mvn 	x5, x5
	stur	x4, [x5]

	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
