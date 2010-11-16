	move.l	20(zpc),d0
	move.l	20(pc),d0
	move.l	20(zpc,d0),d0
	move.l	2000(zpc,d0),d0
	move.l	(zpc),d0

	lea.l	600(pc),a0
	jmp	600(pc)
	jsr	600(pc)