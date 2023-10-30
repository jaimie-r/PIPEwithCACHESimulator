	.arch armv8-a
	.text
	.align	2
start:
    // Put values into the registers
    add x0, x0, #12
    nop
    nop
    nop

    add x0, x0, #90
    nop
    nop
    nop

    add x4, x0, #13
    nop
    nop
    nop

    // Print x4, should be 115
    eor 	x5, x5, x5
    nop
    nop
    nop

	mvn 	x5, x5
    nop
    nop
    nop

	stur	x4, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
