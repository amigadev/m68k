/*

Copyright (c) 2003 Jesper Svennevid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software",	0), to deal in
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
 *	$Id: mnemonics_array.c,v 1.4 2003/06/22 11:57:27 jesper Exp $
 *	$Log: mnemonics_array.c,v $
 *	Revision 1.4  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.3  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

#include <string.h>

#include "types.h"
#include "mnemonics.h"

#ifndef NULL
#define NULL (0)
#endif

mnemonic_t mnemonics[] =
{
	// format:
	// mnemonic,	cpus, ud, func,		num_args
	// num_args is -1 if function does it's own checking (if it has variable number of args)

	MN("abcd",	C_ALL, 0, asm_null,	2,	0),	
	MN("add",	C_ALL, 0, asm_add,	2,	0),
	MN("adda",	C_ALL, 0, asm_adda,	2,	0),
	MN("addi",	C_ALL, 0, asm_addi,	2,	0),
	MN("addq",	C_ALL, 0, asm_addq,	2,	0),
	MN("addx",	C_ALL, 0, asm_null,	2,	0),
	MN("and",	C_ALL, 0, asm_and,	2,	0),
	MN("andi",	C_ALL, 0, asm_andi,	2,	0),
	
	MN("asl",	C_ALL, 0, asm_null,	-1,	0),
	MN("asr",	C_ALL, 0, asm_null,	-1,	0),

	MN("bcc",	C_ALL, CC_CC, asm_bcc,	1,	0),
	MN("bcs",	C_ALL, CC_CS, asm_bcc,	1,	0),
	MN("beq",	C_ALL, CC_EQ, asm_bcc,	1,	0),
	MN("bge",	C_ALL, CC_GE, asm_bcc,	1,	0),
	MN("bgt",	C_ALL, CC_GT, asm_bcc,	1,	0),
	MN("bhi",	C_ALL, CC_HI, asm_bcc,	1,	0),
	MN("ble",	C_ALL, CC_LE, asm_bcc,	1,	0),
	MN("bls",	C_ALL, CC_LS, asm_bcc,	1,	0),
	MN("blt",	C_ALL, CC_LT, asm_bcc,	1,	0),
	MN("bmi",	C_ALL, CC_MI, asm_bcc,	1,	0),
	MN("bne",	C_ALL, CC_NE, asm_bcc,	1,	0),
	MN("bpl",	C_ALL, CC_PL, asm_bcc,	1,	0),
	MN("bvc",	C_ALL, CC_VC, asm_bcc,	1,	0),
	MN("bvs",	C_ALL, CC_VS, asm_bcc,	1,	0),

	MN("bchg",	C_ALL, 0, asm_null,	2,	0),
	MN("bclr",	C_ALL, 0, asm_null,	2,	0),
	MN("bfchg",	C_020P, 0, asm_null,	1,	MF_BFIELD),
	MN("bfclr",	C_020P, 0, asm_null,	1,	MF_BFIELD),
	MN("bfexts",	C_020P, 0, asm_null,	2,	MF_BFIELD),
	MN("bfextu",	C_020P, 0, asm_null,	2,	MF_BFIELD),
	MN("bfffo",	C_020P, 0, asm_bfffo,	2,	MF_BFIELD),
	MN("bfins",	C_020P, 0, asm_null,	2,	MF_BFIELD),
	MN("bfset",	C_020P, 0, asm_null,	1,	MF_BFIELD),
	MN("bftst",	C_020P, 0, asm_null,	1,	MF_BFIELD),

/*
	"bkpt",		0, asm_null,
*/

	MN("bra",	C_ALL, 0, asm_null,	1,	0),
	MN("bset",	C_ALL, 0, asm_null,	2,	0),
	MN("bsr",	C_ALL, 0, asm_null,	1,	0),
	MN("btst",	C_ALL, 0, asm_null,	2,	0),
	MN("callm",	C_020, 0, asm_callm,	2,	0),
/*
	"cas",		C_020P, 0, asm_null,
	"cas2",		C_020P, 0, asm_null,
	"chk",		C_ALL, 0, asm_null,
	"chk2",		
*/

	MN("clr",	C_ALL, 0, asm_clr,	1,	0),
	MN("cmp",	C_ALL, 0, asm_null,	2,	0),
	MN("cmpa",	C_ALL, 0, asm_null,	2,	0),
	MN("cmpi",	C_ALL, 0, asm_null,	2,	0),
	MN("cmpm",	C_ALL, 0, asm_null,	2,	0),
	//MN("cmp2",	C_ALL, 0, asm_null,	2,	0),
/*
	"cpDBcc"
	cpGEN
	cpScc
	cpTRAPcc
*/

	MN("dbcc",	C_ALL, CC_CC, asm_dbcc,	2,	0),
	MN("dbcs",	C_ALL, CC_CS, asm_dbcc,	2,	0),
	MN("dbeq",	C_ALL, CC_EQ, asm_dbcc,	2,	0),
	MN("dbf",	C_ALL, CC_F, asm_dbcc,	2,	0),
	MN("dbge",	C_ALL, CC_GE, asm_dbcc,	2,	0),
	MN("dbgt",	C_ALL, CC_GT, asm_dbcc,	2,	0),
	MN("dbhi",	C_ALL, CC_HI, asm_dbcc,	2,	0),
	MN("dble",	C_ALL, CC_LE, asm_dbcc,	2,	0),
	MN("dbls",	C_ALL, CC_LS, asm_dbcc,	2,	0),
	MN("dblt",	C_ALL, CC_LT, asm_dbcc,	2,	0),
	MN("dbmi",	C_ALL, CC_MI, asm_dbcc,	2,	0),
	MN("dbne",	C_ALL, CC_NE, asm_dbcc,	2,	0),
	MN("dbpl",	C_ALL, CC_PL, asm_dbcc,	2,	0),
	MN("dbt",	C_ALL, CC_T, asm_dbcc,	2,	0),
	MN("dbvc",	C_ALL, CC_VC, asm_dbcc,	2,	0),
	MN("dbvs",	C_ALL, CC_VS, asm_dbcc,	2,	0),

	MN("divs",	C_ALL, 0, asm_null,	2,	0),
	MN("divsl",	C_ALL, 0, asm_null,	2,	0),
	MN("divu",	C_ALL, 0, asm_null,	2,	0),
	MN("divul",	C_ALL, 0, asm_null,	2,	0),

	MN("eor",	C_ALL, 0, asm_null,	2,	0),
	MN("eori",	C_ALL, 0, asm_null,	2,	0),

	MN("exg",	C_ALL, 0, asm_null,	2,	0),
	MN("ext",	C_ALL, 0, asm_null,	1,	0),
	MN("extb",	C_020P, 0, asm_null,	1,	0),

	MN("illegal",	C_ALL, 0, asm_illegal,	0,	0),

	MN("jmp",	C_ALL, 0, asm_jmp,	1,	0),
	MN("jsr",	C_ALL, 0, asm_jsr,	1,	0),

	MN("lea",	C_ALL, 0, asm_lea,	2,	0),
	MN("link",	C_ALL, 0, asm_null,	2,	0),

	MN("lsl",	C_ALL, 0, asm_null,	-1,	0),
	MN("lsr",	C_ALL, 0, asm_null,	-1,	0),

	MN("move",	C_ALL, 0, asm_move,	2,	0),
	MN("movea",	C_ALL, 0, asm_null,	2,	0),
	MN("move16",	C_040|C_060, 0, asm_null,	2,	0),
	MN("movem",	C_ALL, 0, asm_null,	2,	0),
	MN("movep",	C_ALL, 0, asm_null,	2,	0),
	MN("moveq",	C_ALL, 0, asm_moveq,	2,	0),

	MN("muls",	C_ALL, 0, asm_null,	2,	0),
	MN("mulu",	C_ALL, 0, asm_null,	2,	0),

	MN("nbcd",	C_ALL, 0, asm_null,	1,	0),

	MN("neg",	C_ALL, 0, asm_null,	1,	0),
	MN("negx",	C_ALL, 0, asm_null,	1,	0),
	MN("nop",	C_ALL, 0, asm_nop,	0,	0),
	MN("not",	C_ALL, 0, asm_not,	1,	0),

	MN("or",	C_ALL, 0, asm_null,	2,	0),
	MN("ori",	C_ALL, 0, asm_null,	2,	0),

	MN("pack",	C_020P, 0, asm_null,	3,	0),

	MN("pea",	C_ALL, 0, asm_pea,	1,	0),

	MN("rol",	C_ALL, 0, asm_null,	-1,	0),
	MN("ror",	C_ALL, 0, asm_null,	-1,	0),
	MN("roxl",	C_ALL, 0, asm_null,	-1,	0),
	MN("roxr",	C_ALL, 0, asm_null,	-1,	0),

	MN("rtd",	C_010|C_020|C_030|C_040|C_060, 0, asm_rtd,	1,	0),
	MN("rtm",	C_020, 0, asm_rtm,	1,	0),
	MN("rtr",	C_ALL, 0, asm_rtr,	0,	0),
	MN("rts",	C_ALL, 0, asm_rts,	0,	0),
	MN("sbcd",	C_ALL, 0, asm_null,	2,	0),

	MN("scc",	C_ALL, CC_CC, asm_scc,	1,	0),
	MN("scs",	C_ALL, CC_CS, asm_scc,	1,	0),
	MN("seq",	C_ALL, CC_EQ, asm_scc,	1,	0),
	MN("sf",	C_ALL, CC_F, asm_scc,	1,	0),
	MN("sge",	C_ALL, CC_GE, asm_scc,	1,	0),
	MN("sgt",	C_ALL, CC_GT, asm_scc,	1,	0),
	MN("shi",	C_ALL, CC_HI, asm_scc,	1,	0),
	MN("sle",	C_ALL, CC_LE, asm_scc,	1,	0),
	MN("sls",	C_ALL, CC_LS, asm_scc,	1,	0),
	MN("slt",	C_ALL, CC_LT, asm_scc,	1,	0),
	MN("smi",	C_ALL, CC_MI, asm_scc,	1,	0),
	MN("sne",	C_ALL, CC_NE, asm_scc,	1,	0),
	MN("spl",	C_ALL, CC_PL, asm_scc,	1,	0),
	MN("st",	C_ALL, CC_T, asm_scc,	1,	0),
	MN("svc",	C_ALL, CC_VC, asm_scc,	1,	0),
	MN("svs",	C_ALL, CC_VS, asm_scc,	1,	0),

	MN("sub",	C_ALL, 0, asm_null,	2,	0),
	MN("suba",	C_ALL, 0, asm_null,	2,	0),
	MN("subi",	C_ALL, 0, asm_null,	2,	0),
	MN("subq",	C_ALL, 0, asm_null,	2,	0),
	MN("subx",	C_ALL, 0, asm_null,	2,	0),
	MN("swap",	C_ALL, 0, asm_swap,	1,	0),
	MN("tas",	C_ALL, 0, asm_null,	1,	0),

	MN("trap",	C_ALL, 0, asm_null,	1,	0),
//	"TRAPcc"
	MN("trapv",	C_ALL, 0, asm_null,	0,	0),

	MN("tst",	C_ALL, 0, asm_tst,	1,	0),
	MN("unlk",	C_ALL, 0, asm_unlk,	1,	0),
	MN("unpk",	C_020P, 0, asm_null,	3,	0),

	MN("dc",	C_ALL, 0, asm_dc,	-1,	MF_CUSTOM),
	MN("equ",	C_ALL, 0, asm_equ,	1,	MF_NODECLARE),

	MN("store",	C_ALL, 0, asm_store,	0,	0),
	MN("restore",	C_ALL, 0, asm_restore,	0,	0),

	MN("section",	C_ALL, 0, asm_section,	-1,	MF_CUSTOM),

	MN("",		0,0,0,0,	0),
};

mnemonic_t* fetchmnemonic( const char* name )
{
	int i = 0;

	while(mnemonics[i].name[0])
	{
		if(!strcasecmp(mnemonics[i].name,name) && mnemonics[i].assemble)
			return &mnemonics[i];
		i++;
	}
	return NULL;                                                                                                                                                                                	
}
