#ifndef __MNEMONICS_H__
#define __MNEMONICS_H__

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
 *	$Id: mnemonics.h,v 1.3 2003/06/22 11:57:27 jesper Exp $
 *	$Log: mnemonics.h,v $
 *	Revision 1.3  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.2  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   
enum
{
	C_000		= 0x0001,
	C_010		= 0x0002,
	C_020		= 0x0004,
	C_030		= 0x0008,
	C_040		= 0x0010,
	C_060		= 0x0020,
	C_881		= 0x0040,
	C_882		= 0x0080,
	C_851		= 0x0100,
	C_C32		= 0x0200,
};

enum
{
	MF_BFIELD	= 0x01,		// instruction accepts bitfields in first parameter
	MF_NODECLARE	= 0x02,		// instruction manages symbol-declaration internally
	MF_CUSTOM	= 0x04,		// instruction is not a true instruction, all arguments turn into strings
};

#define MN(mnemonic,cpu,userdata,asmfunc,num_args,flags) {(mnemonic),(cpu),(userdata),(asmfunc),(num_args),(flags),NULL}

#define C_ALL (C_000|C_010|C_020|C_030|C_040|C_060)
#define C_020P (C_020|C_030|C_040|C_060)

enum
{
	CC_T = 0,		/* True */
	CC_F,			/* False */
	CC_HI,			/* High */
	CC_LS,			/* Low or Same */
	CC_CC,			/* Carry Clear */
	CC_CS,			/* Carry Set */
	CC_NE,			/* Not Equal */
	CC_EQ,			/* Equal */
	CC_VC,			/* Overflow Clear */
	CC_VS,			/* Overflow Set */
	CC_PL,			/* Plus */
	CC_MI,			/* Minus */
	CC_GE,			/* Greater or Equal */
	CC_LT,			/* Less Than */
	CC_GT,			/* Greater Than */
	CC_LE			/* Less or Equal */	 
};

struct mangle_t;
#define AFUNC(x) int x(struct mangle_t* mangle, unsigned int userdata)
typedef AFUNC((*asmfunc));

typedef struct mnemonic_t
{
	char			name[32];	/* name of mnemonic */
	uint16_t 		cpu;		/* which CPUs the instruction is available on */
	uint32_t 		userdata;	/* userdata for instruction */
	asmfunc			assemble;	/* assembler function */
	int8_t			num_args;	/* number of arguments for opcode */
	uint8_t			flags;		/* flags */
	struct mnemonic_t*	next;		/* next instruction in hash */
} mnemonic_t;

extern mnemonic_t* fetchmnemonic( const char* name );

extern mnemonic_t mnemonics[];

extern AFUNC(asm_null);
extern AFUNC(asm_add);
extern AFUNC(asm_adda);
extern AFUNC(asm_addi);
extern AFUNC(asm_addq);

extern AFUNC(asm_and);
extern AFUNC(asm_andi);

extern AFUNC(asm_bcc);

extern AFUNC(asm_bfffo);

extern AFUNC(asm_callm);
extern AFUNC(asm_clr);

extern AFUNC(asm_dbcc);
extern AFUNC(asm_illegal);

extern AFUNC(asm_jmp);
extern AFUNC(asm_jsr);

extern AFUNC(asm_lea);

extern AFUNC(asm_move);
extern AFUNC(asm_movea);
extern AFUNC(asm_moveq);
extern AFUNC(asm_nop);
extern AFUNC(asm_not);
extern AFUNC(asm_pea);

extern AFUNC(asm_rtd);
extern AFUNC(asm_rtm);
extern AFUNC(asm_rtr);
extern AFUNC(asm_rts);

extern AFUNC(asm_scc);
extern AFUNC(asm_swap);

extern AFUNC(asm_tst);

extern AFUNC(asm_unlk);

extern AFUNC(asm_dc);
extern AFUNC(asm_equ);

extern AFUNC(asm_store);
extern AFUNC(asm_restore);

extern AFUNC(asm_section);

#endif
