	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Simple tests for the subs instruction with def-use hazards.
    movz x0, #1
    movz x1, #0x8000, lsl 48
    // -1
    subs x2, x2, x0
    subs x3, x2, x1
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
