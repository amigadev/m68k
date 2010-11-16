#ifndef __ERROR_H__
#define __ERROR_H__

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
 *	$Id: error.h,v 1.5 2003/06/22 22:47:42 jesper Exp $
 *	$Log: error.h,v $
 *	Revision 1.5  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.4  2003/06/20 10:07:05  jesper
 *	Alignment testing of code added
 *	
 *	Revision 1.3  2003/06/20 08:10:28  jesper
 *	new code-generation complete
 *	
 *	Revision 1.2  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

enum
{
	ERROR_SUCCESS	= 0,
	EGLOBAL_MISSING_FEATURE,
	EGLOBAL_FAILED_MEMORY_ALLOCATION,

	// parsing errors

	EPARSE_BASE = 1000,
	EPARSE_UNMATCHED_PARANTHESES,
	EPARSE_VALUE_OUT_OF_BOUNDS,
	EPARSE_ADDRESS_OUT_OF_BOUNDS,
	EPARSE_INVALID_OPERAND,
	EPARSE_INVALID_SIZE_OPERAND,
	EPARSE_INVALID_SYMBOL,
	EPARSE_INVALID_VALUE,
	EPARSE_INVALID_REGISTER,
	EPARSE_INVALID_SIZE_MODIFIER,
	EPARSE_INVALID_SCALE_MODIFIER,
	EPARSE_INVALID_ARGUMENT,
	EPARSE_TOO_MANY_REGISTERS,
	EPARSE_TOO_MANY_DISPLACEMENTS,
	EPARSE_INVALID_BITFIELD,
	EPARSE_INVALID_MNEMONIC,
	EPARSE_DUPLICATE_SYMBOL,
	EPARSE_UNTERMINATED_STRING,
	EPARSE_MISPLACED_ESCAPING,
	EPARSE_INVALID_STRING,
	EPARSE_INVALID_SECTION_TYPE,
	EPARSE_INVALID_SYMBOL_NAME,
	EPARSE_INVALID_SECTION_NAME,

	// code generation errors
	
	ECODE_BASE = 2000,
	ECODE_INVALID_ADDRESSING_MODE,
	ECODE_INVALID_OPERAND,
	ECODE_INVALID_NUMBER_OF_ARGUMENTS,
	ECODE_INVALID_INSTRUCTION_SIZE,
	ECODE_RESTORE_WITHOUT_STORE,
	ECODE_INVALID_ADDRESS_SIZE,
	ECODE_RELATIVE_ADDRESS_TOO_LARGE,
	ECODE_UNDECLARED_SYMBOL,
	ECODE_MISALIGNED_INSTRUCTION,
	ECODE_MISALIGNED_ADDRESS,
	ECODE_UNRESOLVED_SECTION,

	// architecture errors

	EARCH_BASE = 3000,
	EARCH_UNSUPPORTED_INSTRUCTION,
	EARCH_MISPLACED_BITFIELD,
	EARCH_UNSUPPORTED_BITFIELD,
	EARCH_UNSUPPORTED_SECTION_TYPE,

	// arguments errors

	EINIT_BASE = 4000,
	EINIT_INVALID_ARGUMENT,
};

extern void dofail( unsigned int errno, const char* file, unsigned int line );
const char* errname( unsigned int errno );

#define fail(errno) dofail(errno,__FILE__,__LINE__)

#endif
