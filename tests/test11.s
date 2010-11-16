
main
	move.l	#0,d0
	lea.l	.label(pc),a0
	lea.l	.label,a1
	move.l	(a0),d0
	rts
.label	dc.l	10
