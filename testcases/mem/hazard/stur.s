	.arch armv8-a
	.text
	.align	2
start:

	// Populate registers
	sub  	x0, sp, #32
	sub     x1, sp, #32
	movz	x2, #0xABCD
	movz    x3, #0xDCBA

	// Try storing in different offsets
	add     x0, x0, #8
	stur    x2, [x0]
	add     x0, x0, #16
	stur    x3, [x0]
	ldur    x4, [x1]

	// Print register
	eor 	x5, x5, x5
	mvn 	x5, x5
	stur    x4, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
