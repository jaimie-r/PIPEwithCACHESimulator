	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Mimics the bl_ret test,
    // but adds an incorrect offset.
    // This instruction's address is in the text segment,
    // but it will be unaligned.
    adrp x0, start
    add x0, x0, #15
    // Should get STAT_INS in decode
    // because we branch to unaligned instruction
    ret x0
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
