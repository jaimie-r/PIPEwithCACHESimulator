	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Test an instruction outside chARM instruction set
    // We specify that the shift bit for add and sub will be 0,
    // so this add instruction is illegal in chARM.
    add x0, x0, 0xfff000
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
