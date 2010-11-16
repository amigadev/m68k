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
 *	$Id: mnemonics_data.c,v 1.6 2003/06/22 11:57:27 jesper Exp $
 *	$Log: mnemonics_data.c,v $
 *	Revision 1.6  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.5  2003/06/19 14:55:30  jesper
 *	code generation update
 *	
 *	Revision 1.4  2003/06/19 06:49:45  jesper
 *	added new interface for instruction-pushing, currently incomplete
 *	
 *	Revision 1.3  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   
#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "error.h"
#include "mangle.h"
#include "mnemonics.h"
#include "codespace.h"

// TODO: add support for storing strings

AFUNC(asm_dc)
{
	unsigned short i;

	for(i = 0; i < mangle->num_args;i++)
	{
		if(!(AM_INVALID(i,AM_IMMEDIATE)||AM_INVALID(i,AM_STRING))) fail(ECODE_INVALID_ADDRESSING_MODE);

		switch(mangle->opsize)
		{
			case OS_BYTE:
			{
				switch(mangle->args[i]->mode)
				{
					case AM_STRING:
					{
						const char* s = mangle->args[i]->string;
						int in_string = 0;

						while(*s)
						{
							if(in_string&1)
							{
								if('\\' == *s)
								{
									s++;
									if(!*s)
										fail(EPARSE_MISPLACED_ESCAPING);

									switch(*s)
									{
										case '0': cs_push_data(1,0,1,NULL); break;
										case 't': cs_push_data(1,9,1,NULL); break;
										case 'r': cs_push_data(1,13,1,NULL); break;
										case 'n': cs_push_data(1,10,1,NULL); break;
										case '"': cs_push_data(1,'"',1,NULL); break;
										default: cs_push_data(1,(*s)&0xff,1,NULL);
									}

								}
								else
								{
									if('"' == *s)
										in_string = 2;
									else
										cs_push_data(1,(*s)&0xff,1,NULL);
								}
							}
							else
							{
								if(!isspace(*s))
								{
									if(!in_string)
									{
										if('"' == *s)
											in_string = 1;
										else
											fail(EPARSE_INVALID_STRING);
									}
									else
										fail(EPARSE_INVALID_STRING);
								}
							}

							s++;
						}
					}
					break;
					
					case AM_IMMEDIATE:
					{
						if(mangle->args[i]->value.symbol) fail(ECODE_INVALID_ADDRESSING_MODE);
						if(mangle->args[i]->value.uint > 255) fail(EPARSE_VALUE_OUT_OF_BOUNDS);

						cs_push_data(1,mangle->args[i]->value.uint&0xff,1,NULL);
					}
					break;
				}
			}
			break;

			case OS_WORD:
			{
				if(AM_INVALID(i,AM_STRING)) fail(ECODE_INVALID_ADDRESSING_MODE);

				// TODO: should we allow absolut addresses < 64k?

				if(mangle->args[i]->value.symbol) fail(ECODE_INVALID_ADDRESSING_MODE);
				if(mangle->args[i]->value.uint > 65535) fail(EPARSE_VALUE_OUT_OF_BOUNDS);

				cs_push_data(2,mangle->args[i]->value.uint&0xffff,1,NULL);
			}
			break;

			case OS_LONG:
			{
				if(AM_INVALID(i,AM_STRING))
					cs_push_data(4,0,1,cs_create_symbol(mangle->args[i]->string,NULL,SF_REFERENCE));
				else
					cs_push_data(4,mangle->args[i]->value.uint,1,mangle->args[i]->value.symbol);
			}
			break;
		}
	}

	return 0;
}

AFUNC(asm_ds)
{
	return 1;
}

AFUNC(asm_equ)
{
	return 1;
}

AFUNC(asm_blk)
{
	return 1;
}

AFUNC(asm_store)
{
	if(OS_VALID(OS_DEFAULT)) fail(ECODE_INVALID_INSTRUCTION_SIZE);

	cs_push_section();

	return 1;
}

AFUNC(asm_restore)
{
	if(OS_VALID(OS_DEFAULT)) fail(ECODE_INVALID_INSTRUCTION_SIZE);

	cs_pop_section();

	return 1;
}
