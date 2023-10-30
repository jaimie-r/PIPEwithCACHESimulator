	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Simple tests for the cmp instruction.
    movz x0, #1
    movz x1, #0x8000, lsl 48
    sub x2, x2, #1
    nop
    nop
    nop
    cmp x2, x1
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
