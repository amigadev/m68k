/*

Copyright (c) 2003 Jesper Svennevid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*
 *	$Id: mnemonics.c,v 1.9 2003/08/23 11:44:55 jesper Exp $
 *	$Log: mnemonics.c,v $
 *	Revision 1.9  2003/08/23 11:44:55  jesper
 *	idiot fixes :)
 *	
 *	Revision 1.8  2003/07/23 08:24:11  jesper
 *	*** empty log message ***
 *	
 *	Revision 1.7  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.6  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.5  2003/06/19 13:32:50  jesper
 *	Fixed move -> moveq optimization bug (lacked symbol-check)
 *	
 *	Revision 1.4  2003/06/19 06:49:45  jesper
 *	added new interface for instruction-pushing, currently incomplete
 *	
 *	Revision 1.3  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "codespace.h"
#include "mnemonics.h"
#include "mangle.h"
#include "error.h"

int mnemonics_setup()
{
	// insert all instructions into a hash-table for swift searches

	return 0;
}

AFUNC(asm_null)
{
	return 0;
}

AFUNC(asm_add)
{
	IS_DEF(0xd000);	// 1101 rrro oomm mrrr

	OS_REMAP(OS_WORD);

	// INSTRUCTION REWRITE: add -> adda
	if(AM_INVALID(1,AM_REGISTER_DIRECT_ADDRESS))
		return asm_adda(mangle,0);

	return 0;
}

AFUNC(asm_adda)
{
	return 0;
}

AFUNC(asm_addi)
{
	return 0;
}

AFUNC(asm_addq)
{
	return 0;
}

AFUNC(asm_and)
{
	return 0;
}

AFUNC(asm_andi)
{
	return 0;
}

AFUNC(asm_bfffo)
{
	return 0;
}

AFUNC(asm_callm)
{
	return 0;
}

AFUNC(asm_clr)
{
	return 0;
}

AFUNC(asm_illegal)
{
	return 0;
}

AFUNC(asm_jmp)
{
	return 0;
}

AFUNC(asm_jsr)
{
	return 0;
}

AFUNC(asm_lea)
{
	return 0;
}

AFUNC(asm_move)
{
	return 0;
}

AFUNC(asm_movea)
{
	return 0;
}

AFUNC(asm_moveq)
{
	return 0;
}

AFUNC(asm_nop)
{
	return 0;
}

AFUNC(asm_not)
{
	return 0;
}

AFUNC(asm_pea)
{
	return 0;
}

AFUNC(asm_bcc)
{
	return 0;
}

AFUNC(asm_dbcc)
{
	return 0;
}

AFUNC(asm_rtd)
{
	return 0;
}

AFUNC(asm_rtm)
{
	return 0;
}

AFUNC(asm_rtr)
{
	return 0;
}

AFUNC(asm_rts)
{
	return 0;
}

AFUNC(asm_scc)
{
	return 0;
}

AFUNC(asm_swap)
{
	return 0;
}

AFUNC(asm_tst)
{
	return 0;
}

AFUNC(asm_unlk)
{
	return 0;
}
