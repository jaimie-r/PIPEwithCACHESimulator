    .arch armv8-a
	.text
	.align	2
start:
    EOR x0, x0, x0
    ADD x0, x0, #1
    EOR x1, x1, x1
    ADD x1, x1, #32
    MOVZ x4, #0
    MOVZ x3, #1
    SUBS x3, x4, x3

    // start loop
.LSL_LOOP:
    SUBS x2, x1, x4
    B.EQ .LOOP_PREP
    LSL x0, x0, #1
    ADDS x1, x1, x3
    B .LSL_LOOP

.LOOP_PREP:
    EOR x1, x1, x1
    ADD x1, x1, #10

.LSR_LOOP:
    SUBS x2, x1, x4
    B.EQ .EXIT
    LSR x0, x0, #1
    ADDS x1, x1, x3
    B .LSR_LOOP

.EXIT:
    // Print x0
    // correct: 4194304
    EOR 	x5, x5, x5
	MVN 	x5, x5
	STUR	x0, [x5]
	RET

	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits