	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Test an instruction below text segment
    movz x0, #0x1234
    // Should get STAT_INS in decode
    // because we branch to invalid instruction
    ret x0
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
