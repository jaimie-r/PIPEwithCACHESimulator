	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Simple tests for the adds instruction with def-use hazards.
    movz x0, #0xffff
    movz x1, #0xffff, lsl 16
    movz x4, #1
    // 0xffffffff
    adds x5, x0, x1
    movz x2, #0xffff, lsl 32
    movz x3, #0xffff, lsl 48
    // 0xffffffff00000000
    adds x6, x2, x3
    // should be negative
    adds x7, x5, x6
    // zero
    adds x8, x4, x7
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
