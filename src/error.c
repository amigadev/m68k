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
 *	$Id: error.c,v 1.5 2003/06/22 22:47:42 jesper Exp $
 *	$Log: error.c,v $
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
   

#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void dofail( unsigned int errno, const char* file, unsigned int line )
{
	fprintf(stdout,"ERROR %u : %s (%s, %u)\n",errno,errname(errno),file,line);
	exit(1);
}

const char* errname( unsigned int errno )
{
	switch(errno)
	{
		case ERROR_SUCCESS: return "No error";
		case EGLOBAL_MISSING_FEATURE: return "Missing feature";
		case EGLOBAL_FAILED_MEMORY_ALLOCATION: return "Failed memory allocation";

		case EPARSE_UNMATCHED_PARANTHESES: return "Unmatched parantheses";
		case EPARSE_VALUE_OUT_OF_BOUNDS: return "Value out of bounds";
		case EPARSE_ADDRESS_OUT_OF_BOUNDS: return "Address out of bounds";
		case EPARSE_INVALID_OPERAND: return "Invalid operand";
		case EPARSE_INVALID_SIZE_OPERAND: return "Invalid size on operand";
		case EPARSE_INVALID_SYMBOL: return "Invalid symbol";
		case EPARSE_INVALID_REGISTER: return "Invalid register";
		case EPARSE_INVALID_SIZE_MODIFIER: return "Invalid size";
		case EPARSE_INVALID_SCALE_MODIFIER: return "Invalid scale";
		case EPARSE_INVALID_ARGUMENT: return "Invalid argument";
		case EPARSE_TOO_MANY_REGISTERS: return "Too many registers";
		case EPARSE_TOO_MANY_DISPLACEMENTS: return "Too many displacements";
		case EPARSE_INVALID_BITFIELD: return "Invalid bitfield";
		case EPARSE_INVALID_MNEMONIC: return "Invalid mnemonic";
		case EPARSE_DUPLICATE_SYMBOL: return "Duplicate symbol";
		case EPARSE_UNTERMINATED_STRING: return "Unterminated string";
		case EPARSE_MISPLACED_ESCAPING: return "Misplaced escaping";
		case EPARSE_INVALID_STRING: return "Invalid string";
		case EPARSE_INVALID_SECTION_TYPE: return "Invalid section type";
		case EPARSE_INVALID_SYMBOL_NAME: return "Invalid symbol name";
 		case EPARSE_INVALID_SECTION_NAME: return "Invalid section name";

		case ECODE_INVALID_ADDRESSING_MODE: return "Invalid addressing mode";
		case ECODE_INVALID_OPERAND: return "Invalid operand";
		case ECODE_INVALID_NUMBER_OF_ARGUMENTS: return "Invalid number of arguments";
		case ECODE_INVALID_INSTRUCTION_SIZE: return "Invalid instruction size";
		case ECODE_RESTORE_WITHOUT_STORE: return "RESTORE without STORE";
		case ECODE_INVALID_ADDRESS_SIZE: return "Invalid address size";
		case ECODE_RELATIVE_ADDRESS_TOO_LARGE: return "Relative address too large";
		case ECODE_UNDECLARED_SYMBOL: return "Undeclared symbol";
		case ECODE_MISALIGNED_INSTRUCTION: return "Misaligned instruction";
		case ECODE_MISALIGNED_ADDRESS: return "Misaligned address";
		case ECODE_UNRESOLVED_SECTION: return "Unresolved section";

		case EARCH_UNSUPPORTED_INSTRUCTION: return "Instruction not supported on this CPU";
		case EARCH_MISPLACED_BITFIELD: return "Bitfields not supported for this function";
		case EARCH_UNSUPPORTED_BITFIELD: return "Bitfield in current operand not supported";                

		case EINIT_INVALID_ARGUMENT: return "Invalid argument";

		default: return "Invalid Error";
	}
}
