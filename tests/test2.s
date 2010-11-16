	; ---
	move.l	(20,a0,d0),d0
	move.w	20(a0,d0),d0

	; --- 020+
	move.l	(2000,a0,d0),d0
	move.w	2000(a0,d0),d0


	; ---
	move.l	(2000,a0),d0
	move.l	2000(a0),d0

	; --- 020+
	move.l	2000(pc,d0),d0
	move.l	(2000,pc,d0),d0

	; ---
	move.l	(20,pc,d0),d0
	move.l	20(pc,d0),d0

	; ---
	move.l	(1000,pc),d0
	move.l	1000(pc),d0


	; ---
	move.l	-(a0),d0

	; ---
	move.l	(a0)+,d0

	; ---
	move.l	(a0),d0

	; ---
	move.l	a0,d0

	; ---
	move.l	d0,d0

	; ---
	move.l	#$1234,d0

	; ---
	move.l	$1234.w,d0
	move.l	$1234.l,d0
