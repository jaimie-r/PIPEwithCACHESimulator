	.arch armv8-a
	.text
	.align	2
start:
    // Put values into the registers
    movz x0, #52
    movz x1, #193
    cmp x0, x1
    b.ge .helper

.goback:
    // Print x0
    eor 	x5, x5, x5
	mvn 	x5, x5
	stur	x0, [x5]
    // correct: 52
    // incorrect: 1
	ret
.helper:
    movz x0, 1
    b .goback
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
