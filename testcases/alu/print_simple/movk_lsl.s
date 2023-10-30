	.arch armv8-a
	.text
	.align	2
start:
    movk 	x0, #0x1234
    nop
    nop
    nop
    lsl     x0, x0, #16
    nop
    nop
    nop
    movk    x0, #0x5678
    nop
    nop
    nop
    nop
    nop
    nop
    lsl     x0, x0, #16
    nop
    nop
    nop
    movk 	x0, #0x9101
    nop
    nop
    nop
    eor     x5, x5, x5
    nop
    nop
    nop
    mvn     x5, x5
    nop
    nop
    nop
    stur    x0, [x5]
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ret
    .size	start, .-start
    .ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
    .section	.note.GNU-stack,"",@progbits
