	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Load from unaligned address
    adrp x0, .LANCHOR0
    add x0, x0, #3
    nop
    nop
    nop
    // Should have STAT_ADR in memory stage
    stur x1, [x0]
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
