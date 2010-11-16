#ifndef __MANGLE_H__
#define __MANGLE_H__

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
 *	$Id: mangle.h,v 1.8 2003/08/23 11:44:54 jesper Exp $
 *	$Log: mangle.h,v $
 *	Revision 1.8  2003/08/23 11:44:54  jesper
 *	idiot fixes :)
 *	
 *	Revision 1.7  2003/07/23 08:24:11  jesper
 *	*** empty log message ***
 *	
 *	Revision 1.6  2003/06/22 12:02:32  jesper
 *	more msvc fixes
 *	
 *	Revision 1.5  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.4  2003/06/19 14:55:30  jesper
 *	code generation update
 *	
 *	Revision 1.3  2003/06/19 06:49:45  jesper
 *	added new interface for instruction-pushing, currently incomplete
 *	
 *	Revision 1.2  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

#include "operand.h"

enum
{
	OS_DEFAULT,
	OS_BYTE,
	OS_WORD,
	OS_LONG
};

struct symbol_t;
struct mnemonic_t;
typedef struct mangle_t
{
	struct symbol_t*	declare;
	struct mnemonic_t*	match;
	char			mnemonic[32];
	uint8_t			opsize;
	int8_t			num_args;
	operand_t*		args[256];
} mangle_t;

extern int mangle( const char* in, mangle_t* out );
extern void cleanmangle( mangle_t* in );

#define OS_INVALID(x) (mangle->opsize == (x))
#define OS_VALID(x) (mangle->opsize != (x))
#define OS_SIZE_REV2() ((mangle->opsize == OS_BYTE) ? 1 : (mangle->opsize == OS_LONG) ? 2 : 3)
#define OS_SIZE_NRM1() ((mangle->opsize == OS_BYTE) ? 0 : (mangle->opsize == OS_LONG) ? 2 : 1)
#define OS_SIZE_SEL(t1,t2,t3) ((mangle->opsize == OS_BYTE) ? t1 : (mangle->opsize == OS_WORD) ? t2 : t3)
#define OS_REMAP(x) if(mangle->opsize == OS_DEFAULT) (mangle->opsize = (x))


#define AM_INVALID(i,x) (mangle->args[(i)]->mode&(x)) // REWRTIE - returns true if a specified a-mode is found
#define AM_VALID(i,x) (mangle->args[(i)]->mode&(~(x))) // REWRITE - returns true if a not specified a-mode is found
#define AM_IMMEDIATE_BOUNDS(i,l,h) ((mangle->args[(i)]->value.uint<(l)) || (mangle->args[(i)]->value.uint>(h)))
#define AM_IMMEDIATE_BOUNDS_SIGN(i,l,h) ((((int)mangle->args[(i)]->value.uint)<(l)) || (((int)mangle->args[(i)]->value.uint)>(h)))
#define AM_IMMEDIATE_SYMBOL(i) (mangle->args[(i)]->value.symbol)
#define AM_ARGS(x) (mangle->num_args != (x))

#define IS_DEF(x) cs_instr_t is; unsigned short instr = (x); memset(&is,0,sizeof(is));
#define IS_MERGE(x,i,l) (instr |= ((x)&((1<<(l))-1))<<(i))
#define IS_STORE() is.opsize = 2; is.ops = instr
#define IS_SET(x) instr = (x)
#define IS_EWORD(x) is.ea[is.easize] = *(x); is.easize++
#define IS_PUSH() cs_push_instr(&is)

#endif
