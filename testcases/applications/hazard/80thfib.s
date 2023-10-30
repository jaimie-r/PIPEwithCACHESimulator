	.arch armv8-a
	.text
	.align	2
start:
	mov	x1, 81
	mov	x2, 1
	mov	x0, 0
	mov x4, 1
.check:
	subs	x1, x1, x4
	bne	.calc
	mvn	x1, xzr
	stur	x0, [x1]
	ret
.calc:
	adds	x3, x0, x2
	mov	x0, x2
	mov	x2, x3
	b	.check
	.size	start, .-start
