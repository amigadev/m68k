*
* this is a small testsource I use to verify the functionality of mnemonics
*

	; add testing

	addq.w	#3,d4
	addq.l	#8,23.w
	add.l	#3,d0
	add.l	d0,(a5)
	add.w	#45,d0
	add.l	#23,a4

	callm	#67,$2345
	clr.l	-(a3)

	illegal

	; move testing

	move.l	#245,d0
	move.l	#34,d0

	; you can explicitly force a operand into long mode using the size-specifier
	move.l	$1234,$1234		; ok, will generate two extension-words
	move.l	$1234.l,$1234.l		; ok, will generate four extension-words (containing 2 longwords)
	;move.l	$abcd.w,d0		; not ok, $abcd.w is a negative value and would not generate the wanted address


	move.l	-(a7),d0
	move.l	4.w,d4
	move.b	#34,(a5)
	move.l	(a4)+,(a2)+

	nop
	not.l	d0

	pea	(sp)

	rtd	#123
	rtm	a4
	rtr
	rts	; test

	swap	d5

	move.l	4(a0),4(a0,d0.w*4)
	move.l	4(a0),(a0,d0.w*4)

	tst	(a4)+

	unlk	a2
