	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Simple tests for the add instruction.
    add x0, x0, #1
    add x1, x1, #0xfff
	nop
	nop
	nop
    add x4, x0, #1
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
