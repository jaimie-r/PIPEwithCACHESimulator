	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Simple test for adrp.
    adrp x0, .LANCHOR0
	nop
	nop
	nop
	add	x0, x0, :lo12:.LANCHOR0
	nop
	nop
	nop
    ldur x1, [x0]       // 100
    ldur x2, [x0, #8]   // 200
    ldur x3, [x0, #16]  // 300
    ldur x4, [x0, #24]  // 400
	ret
	.size	start, .-start
    .data
	.align	4
	.set	.LANCHOR0,. + 0
	.type	x, %object
	.size	x, 64
x:
	.xword	100
    .xword  200
    .xword  300
    .xword  400
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
