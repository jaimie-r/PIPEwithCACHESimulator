	.arch armv8-a
	.text
	.align	2
start:

	// Populate registers
	// x0 has an invalid address because it's less than .data segment
	movz 	x0, #0x1000
	movz	x1, #0xABCD
	nop
	nop
	nop
	stur	x1, [x0]
	nop
	nop
	nop

	// Try loading with various offsets
	ldur	x2, [x0]
	nop
	nop
	nop
	add		x0, x0, #1
	nop
	nop
	nop
	ldur	x3, [x0]
	nop
	nop
	nop

	// Print registers
	eor 	x5, x5, x5
	nop
	nop
	nop
	mvn 	x5, x5
	nop
	nop
	nop
	stur	x2, [x5]
	nop
	nop
	nop
	stur	x3, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
