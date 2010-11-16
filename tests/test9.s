	section	code,code

main
	clr.l	d0
	;move.l	#8-1,d7	; this currently fails due to lack of expression solver
.loop
	add.l	d7,d0
	;dbra	d7,.loop

	rts
