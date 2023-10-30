	.arch armv8-a
	.text
	.align	2

start:

	mov	x1, 21
	nop
	nop
	nop
	nop

	mov	x2, 1
	nop
	nop
	nop
	nop

	mov	x0, 0
	nop
	nop
	nop
	nop

	mov x4, 1
	nop
	nop
	nop
	nop

.check:

	subs	x1, x1, x4
	nop
	nop
	nop
	nop

	bne	.calc
	nop
	nop
	nop
	nop

	mvn	x1, xzr
	nop
	nop
	nop
	nop

	stur	x0, [x1]
	nop
	nop
	nop
	nop

	ret
	nop
	nop
	nop
	nop

.calc:

	adds	x3, x0, x2
	nop
	nop
	nop
	nop

	mov	x0, x2
	nop
	nop
	nop
	nop

	mov	x2, x3
	nop
	nop
	nop
	nop

	b	.check
	nop
	nop
	nop
	nop

	.size	start, .-start
