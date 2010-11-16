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
 *	$Id: main.c,v 1.12 2003/06/22 22:47:42 jesper Exp $
 *	$Log: main.c,v $
 *	Revision 1.12  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.11  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.10  2003/06/20 10:17:09  jesper
 *	corrected some output
 *	
 *	Revision 1.9  2003/06/20 08:10:28  jesper
 *	new code-generation complete
 *	
 *	Revision 1.8  2003/06/19 14:55:30  jesper
 *	code generation update
 *	
 *	Revision 1.7  2003/06/19 06:49:45  jesper
 *	added new interface for instruction-pushing, currently incomplete
 *	
 *	Revision 1.6  2003/06/18 20:16:35  jesper
 *	testing logging
 *	
 *
 *	$EndLog$ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "arguments.h"
#include "mnemonics.h"
#include "mangle.h"
#include "error.h"
#include "codespace.h"

int build( const char* filename );

int main(int argc, const char* argv[])
{
	if(argc>1)
	{
		int i;

		if(!arginit()) return 0;

		if(!argparse(argc,argv)) fail(EINIT_INVALID_ARGUMENT);

		// build codespace-tree

		printf("Pass 1...\n");

		cs_setup();

		i = 0;
		while(i < argcount(ARG_FILENAMES))
		{
			build(argstring(ARG_FILENAMES,i));
			i++;
		}

		printf("Pass 2...\n");

		cs_write(CSF_AMIGAOS,"test.obj");

		cs_free();
	}
	else
	{
		fprintf(stderr,"m68k - MC680x0 Assembler\n");
		fprintf(stderr,"Usage:\n");
		fprintf(stderr,"\tm68k <options> file1 file2 ...\n\n");
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t--optimize <n>,-O <n>\t\t\tLevel of optimization\n");
		fprintf(stderr,"\t--output <f>,-o <f>\t\t\tSelect output-file\n");
		fprintf(stderr,"\t--reloc, -r\t\t\t\tFail if code is not relocatable\n");
		fprintf(stderr,"\t--target <fmt>, -t <fmt>\t\tSpecify output format (amigaos, elf?)\n");
	}

	return 0;
}

int build( const char* filename )
{
	FILE* fp = fopen(filename,"r");
	static char buffer[512];
	mangle_t instr;

	if(fp)
	{
		while(!feof(fp))
		{
			if(fgets(buffer,sizeof(buffer),fp))
			{
//				printf(buffer);

				if(mangle(buffer,&instr))
				{
					if(instr.match)
					{
						if((instr.num_args>0)&&(((int)instr.args[0]->offset.uint)>=0)&&!(instr.match->flags&MF_BFIELD))
							fail(EARCH_MISPLACED_BITFIELD);

						// if mnemonic does not have special symbol-handling, store in codespace

						if(instr.declare && !(instr.match->flags & MF_NODECLARE))
							cs_push_symbol(instr.declare);

						if((instr.match->num_args != -1) && (instr.num_args != instr.match->num_args))
							fail(ECODE_INVALID_NUMBER_OF_ARGUMENTS);

						instr.match->assemble(&instr,instr.match->userdata);
					}
					else
					{
						// line without mnemonic, we still store symbols
						
						if(instr.declare)
							cs_push_symbol(instr.declare);
					}
				}
				cleanmangle(&instr);

/*				
				if(mangle(buffer,&instr) && strlen(instr.mnemonic))
				{
					i = 0;
					while(mnemonics[i].name[0])
					{
						if(!strcasecmp(mnemonics[i].name,instr.mnemonic) && mnemonics[i].assemble)
						{
							if((instr.num_args>0)&&(((int)instr.args[0]->offset.uint)>=0)&&!(mnemonics[i].flags&MF_BFIELD))
								fail(EARCH_MISPLACED_BITFIELD);

							// if mnemonic does not have special symbol-handling, store in codespace

							if(!(mnemonics[i].flags & MF_NODECLARE) && instr.declare)
								cs_store_symbol(instr.declare);

							if((mnemonics[i].num_args != -1) && (instr.num_args != mnemonics[i].num_args))
								fail(ECODE_INVALID_NUMBER_OF_ARGUMENTS);

							mnemonics[i].assemble(&instr,mnemonics->userdata);
							break;
						}
						i++;
					}
				}
				cleanmangle(&instr);
*/
			}
			else break;
		}
	}

	return 0;
}
