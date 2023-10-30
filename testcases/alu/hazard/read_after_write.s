	.arch armv8-a
	.text
	.align	2
start:

    // READ AFTER WRITE HAZARD
        // The ADDS instruction needs the values of X0 and X1 in the Execute phase
        // At that time, 
            // the first MOVZ is in the writeback phase
            // the second MOVZ is in the memory phase
        // This creates a hazard, as the second MOVZ will not have written to x1
        // before the ADDS instruction attempts to read from x1. 


    // Give x0 a value
	movz x0, #74

    // Give x1 a value
    movz x1, #18

    // Add the two registers together
    adds x2, x0, x1

    // Print x2
    // correct: 92
    eor 	x5, x5, x5
	mvn 	x5, x5
	stur	x2, [x5]
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
