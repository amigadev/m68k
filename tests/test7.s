bebe	equ	7

main
	move.l	#$45,.testval
	move.l	.testval(pc),d0
.main2
	move.l	.testval,d0
	move.l	#.testval,d0
	rts

.testval
	dc.l	0,0,0,0,0,0,0,0,0,.main2,0

	section	data,data
.teststring
	dc.b	"test string\n",0
	dc.b	0
	dc.l	main
.test2
	dc.w	1,2,3,4,5

	section	bss,bss

dump	dc.l	0