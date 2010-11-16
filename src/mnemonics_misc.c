/*

Copyright (c) 2003 Jesper Svennevid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software",       0), to deal in
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
 *      $Id: mnemonics_misc.c,v 1.3 2003/06/22 22:47:42 jesper Exp $
 *      $Log: mnemonics_misc.c,v $
 *      Revision 1.3  2003/06/22 22:47:42  jesper
 *      first pass of AHF output complete
 *
 *      Revision 1.2  2003/06/22 12:02:32  jesper
 *      more msvc fixes
 *
 *      Revision 1.1  2003/06/22 12:01:09  jesper
 *      compiler-specific commands added
 *
 *      $EndLog$ */

#include "types.h"
#include "mnemonics.h"
#include "mangle.h"
#include "operand.h"
#include "error.h"
#include "codespace.h"

#include <stdio.h>
#include <string.h>

AFUNC(asm_section)
{
	switch(mangle->num_args)
	{
		case 1:
		{
			if(AM_VALID(0,AM_STRING))
				fail(EPARSE_INVALID_OPERAND);

			cs_create_section(mangle->args[0]->string,ST_CODE);
		}
		break;
		case 2:
		{
			uint32_t flags = 0;
		
			if(AM_VALID(0,AM_STRING)||AM_VALID(1,AM_STRING))
				fail(EPARSE_INVALID_OPERAND);

			// TODO: make this one simpler and more extensible

			if(!strcasecmp(mangle->args[1]->string,"code"))
				flags = ST_CODE;
			else if(!strcasecmp(mangle->args[1]->string,"code_c"))
				flags = ST_CODE|ST_CHIP;
			else if(!strcasecmp(mangle->args[1]->string,"code_f"))
				flags = ST_CODE|ST_FAST;
			else if(!strcasecmp(mangle->args[1]->string,"data"))
				flags = ST_DATA;
			else if(!strcasecmp(mangle->args[1]->string,"data_c"))
				flags = ST_DATA|ST_CHIP;
			else if(!strcasecmp(mangle->args[1]->string,"data_f"))
				flags = ST_DATA|ST_FAST;
			else if(!strcasecmp(mangle->args[1]->string,"bss"))
				flags = ST_BSS;
			else if(!strcasecmp(mangle->args[1]->string,"bss_c"))
				flags = ST_BSS|ST_CHIP;
			else if(!strcasecmp(mangle->args[1]->string,"bss_f"))
				flags = ST_BSS|ST_FAST;
			else fail(EPARSE_INVALID_OPERAND);

			cs_create_section(mangle->args[0]->string,flags);
		}
		break;
		case 3:
		default: fail(ECODE_INVALID_NUMBER_OF_ARGUMENTS);
	}

	return 0;
}
