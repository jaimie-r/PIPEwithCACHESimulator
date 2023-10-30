	.arch armv8-a
	.text
	.align	2
start:

    // WRITE AFTER READ HAZARD

    // put values into the registers
    movz x0, #12
    movz x1, #94
    movz x2, #18

    adds x3, x0, x2
    adds x4, x1, x3 // hazard -- x3 read from before written to

    // Print x4
    // correct: 124
    eor 	x5, x5, x5
	mvn 	x5, x5
	stur	x4, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
