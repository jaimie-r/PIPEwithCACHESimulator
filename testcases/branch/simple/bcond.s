	.arch armv8-a
	.text
	.align	2
start:

    movz x0, #1
    movz x1, #2
    nop
    nop
    nop
    subs x3, x0, x1
    nop
    nop
    nop
    nop
    b.ne .notequal

.goback:

    nop
    nop
    nop

    // Print x0
    eor 	x5, x5, x5
    nop
    nop
    nop
	mvn 	x5, x5
    nop
    nop
    nop
    //correct value is 26
    //incorrect value is 1
	stur	x0, [x5]
	ret
	
.notequal:
    nop
    nop
    nop
    add x0, x0, #13
    nop
    nop
    nop
    add x0, x0, #12
    nop
    nop
    nop
    b .goback

    .size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
