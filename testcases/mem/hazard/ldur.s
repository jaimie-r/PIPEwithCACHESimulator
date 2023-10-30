	.arch armv8-a
	.text
	.align	2
start:

	// Populate registers
	sub 	x0, sp, #16
	movz	x1, #0xABCD
	stur	x1, [x0]

	// Try loading with various offsets
	ldur	x2, [x0]
	add		x0, x0, #8
	ldur	x3, [x0]

	// Print registers
	eor 	x5, x5, x5
	mvn 	x5, x5
	stur	x2, [x5]
	stur	x3, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
