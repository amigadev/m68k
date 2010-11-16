	andi.w	#$2332,d0
	and.l	d0,(20,a0)
	and.w	#$2332,d0
	and	#$3f,ccr

	bfchg	d0{d2:15}
