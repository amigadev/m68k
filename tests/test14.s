	section	code,code

main
	lea.l	.value(pc),a0
	move.l	#7,(a0)
	rts

.value	dc.l	0
