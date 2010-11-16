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
 *	$Id: mangle.c,v 1.6 2003/06/22 22:47:42 jesper Exp $
 *	$Log: mangle.c,v $
 *	Revision 1.6  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.5  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.4  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "types.h"
#include "mangle.h"
#include "mnemonics.h"
#include "codespace.h"
#include "error.h"

/*
	TODO:

	- write solver for values
	- add support for complex operands

	- unify value-mangling with symbol-handling (size-specifier and such)
*/ 

enum
{
	ST_INACTIVE,
	ST_SYMBOL,
	ST_MNEMONIC,
	ST_OPERANDS,
};

enum
{
	OP_FLOAT,	// fpu operands allowed
};

int mnemangle( const char* start, const char* end, mangle_t* out );
int opmangle( const char* start, const char* end, operand_t* arg );

int mangle( const char* in, mangle_t* out )
{
	int ofs;
	const char* estart[256];
	const char* eend[256];

	const char* start = in;

	int state = ST_INACTIVE;
	int stack = 0;
	int in_string = 0;

	memset(estart,0,sizeof(estart));
	memset(eend,0,sizeof(eend));
	memset(out,0,sizeof(mangle_t));

	ofs = 0;
	while(*in)
	{
		if(('\r' == *in)||('\n' == *in))
			break;

		// handle comments

		if(((in>start) && isspace(*(in-1)))||(in==start))
		{
			if((';' == *in)||('*' == *in))
				break;
		}

		switch(state)
		{
			case ST_INACTIVE:
			{
				if(isalpha(*in)||('.' == *in)||('_' == *in))
				{
					if(start == in)
					{
						estart[ofs] = in;
						state = ST_SYMBOL;
					}
					else
					{
						ofs++;
						estart[ofs] = in;
						state = ST_MNEMONIC;
					}
				}
			}
			break;
			case ST_SYMBOL:
			{
				if(!estart[ofs])
				{
					if(!isspace(*in))
					{
						estart[ofs] = in;
						state = ST_MNEMONIC;
					}
				}
				else
				{
					if(isspace(*in)||(':' == *in))
						eend[ofs++] = in;
				}
			}
			break;
			case ST_MNEMONIC:
			{
				if(estart[ofs])
				{			
					if(isspace(*in))
						eend[ofs++] = in;
				}
				else
				{
					if(!isspace(*in))
					{
						estart[ofs] = in;
						state = ST_OPERANDS;

						if('(' == *in)
							stack++;
						else if('"' == *in)
							in_string ^= 1;
						else if('\\' == *in)
							in++;
					}
				}
			}
			break;
			case ST_OPERANDS:
			{
				// just break right now, no overflows thanks
				if(ofs > 128) exit(0);

				if(estart[ofs])
				{
					if('(' == *in)
						stack++;
					else if(')' == *in)
						stack--;
					else if('"' == *in)
						in_string ^= 1;
					else if('\\' == *in)
						in++;
					else if(!stack && !in_string && (isspace(*in)||(',' == *in)))
						eend[ofs++] = in;
				}
				else
				{
					if(!isspace(*in))
					{
						estart[ofs] = in;

						if('(' == *in)
							stack++;
						else if('"' == *in)
							in_string ^= 1;
						else if('\\' == *in)
							in++;
					}
				}
			}
			break;
		}

		// make sure we don't step past the end-mark (escaping could cause this)

		if(*in)
			in++;
	}

	if(in_string) fail(EPARSE_UNTERMINATED_STRING);
	if(stack) fail(EPARSE_UNMATCHED_PARANTHESES);

	// terminate any final field

	if(estart[ofs] && !eend[ofs] && (estart[ofs] != in)) eend[ofs++] = in;

	if(estart[0] && eend[0])
	{
		symbol_t* sym;
		if(!(sym = cs_create_symbol(estart[0],eend[0],SF_DECLARE)))
			fail(EPARSE_INVALID_SYMBOL);

		out->declare = sym;
	}
	if(estart[1] && eend[1])
	{
		mnemangle(estart[1],eend[1],out);					// mnemonic

		// locate matching mnemonic for validation

		out->match = fetchmnemonic(out->mnemonic);

		if(!out->match) fail(EPARSE_INVALID_MNEMONIC);

		if(ofs > 1)
		{
			int i = 0;

			while(i < ofs)
			{
				if(estart[i+2] && eend[i+2])
				{
					operand_t* op = calloc(sizeof(operand_t),1);
					if(!op) fail(EGLOBAL_FAILED_MEMORY_ALLOCATION);

					if(out->match->flags & MF_CUSTOM)
					{
						if(!getval(estart[i+2],eend[i+2],&op->value,0))
						{
							op->string = getstring(estart[i+2],eend[i+2]);
							if(!op->string) fail(EGLOBAL_FAILED_MEMORY_ALLOCATION);

							op->mode = AM_STRING;
						}
						else op->mode = AM_IMMEDIATE;

						// no bitfields, baby! (as we never mangle, it doesn't get reset)
						op->offset.uint = -1;
					}
					else
					{
						opmangle(estart[i+2],eend[i+2],op);

						// only allow bitfield on first operand

						if((i > 2) && (((int)op->offset.uint)>=0))
							fail(EARCH_UNSUPPORTED_BITFIELD);
					}

					// attach operand to queue

					out->args[out->num_args] = op;
					out->num_args++;
				}
				else if(estart[i+2])
					fail(EPARSE_INVALID_OPERAND);

				i++;
			}
		}
	}

	return 1;
}

void cleanmangle( mangle_t* in )
{
	int i = 0;

	for(i=0;i<in->num_args;i++)
		free(in->args[i]);

	memset(in,0,sizeof(mangle_t));
}

int mnemangle( const char* start, const char* end, mangle_t* out )
{
	const char* t = start;
	while((t <= end) && (*t != '.')) t++;

	if((*t == '.') && (end-t == 2))
	{
		switch(tolower(*(t+1)))
		{
			case 'b': out->opsize = OS_BYTE; break;
			case 'w': out->opsize = OS_WORD; break;
			case 'l': out->opsize = OS_LONG; break;
			default: fail(ECODE_INVALID_INSTRUCTION_SIZE); break;
		}
	}

	end = t > end ? end : t;
	strncpy(out->mnemonic,start,end-start);
	out->mnemonic[end-start] = '\0';

	return 1;
}

int opmangle( const char* start, const char* end, operand_t* arg )
{
	const char* t = start;

	char buf[256];
	strncpy(buf,start,end-start);
	buf[end-start] = 0;

	arg->areg.idx = REG_ERR;
	arg->areg.scale = RSCALE_1;
	arg->areg.size = RSIZE_DEFAULT;
	arg->reg.idx = REG_ERR;
	arg->reg.scale = RSCALE_1;
	arg->reg.size = RSIZE_DEFAULT;
	arg->value.uint = 0;
	arg->value.size = VSIZE_Z;
	arg->value.symbol = NULL;
	arg->offset.uint = -1;
	arg->offset.size = VSIZE_L;
	arg->offset.symbol = NULL;
	arg->width.uint = -1;
	arg->width.size = VSIZE_L;
	arg->offset.symbol = NULL;

	if(!(end-start)) fail(EPARSE_INVALID_ARGUMENT);

 	{
		int ofs = 0;
		const char* args[5];
		const char* arge[5]; 
		int argstack[5];
		int stack = 0;
		int i = 0;
		asmreg_t reg;

		memset(args,0,sizeof(args));
		memset(arge,0,sizeof(arge));
		memset(argstack,0,sizeof(argstack));
		args[ofs] = start;

		// [a][(b[,c[,d]])]

		while((t<end) && ('{' != *t))
		{
			if('(' == *t)
			{
				if(!stack && args[ofs] != t)
				{
					if(!(!ofs && ((t-start) == 1) && (*start == '-')))
					{
						argstack[ofs] = stack;
						arge[ofs++] = t;
						args[ofs] = t+1;
					}
				}
				stack++;
			}
			else if(')' == *t)
			{
				if(ofs)
				{
					argstack[ofs] = stack;
					arge[ofs++] = t;
					args[ofs] = t+1;
				}
				stack--;
			}
			else if(',' == *t)
			{
				if(stack>1) fail(EPARSE_UNMATCHED_PARANTHESES);

				if(!ofs && ((*args[ofs])=='('))
					args[ofs]++;

				argstack[ofs] = stack;
				arge[ofs++] = t;
				args[ofs] = t+1;
			}
			t++;
		}

		// terminate final argument

		if(!arge[ofs] && args[ofs] && (args[ofs] != t))
		{
			argstack[ofs] = stack;
			arge[ofs++] = t;
		}

		// process bitfield

		if('{' == *t)
		{
			enum
			{
				BF_OFFSET,
				BF_WIDTH,
				BF_END
			};
			const char* ts;
			int bfst = BF_OFFSET;
			asmreg_t breg;
		
			ts = ++t;
			while(t < end)
			{
				switch(bfst)
				{
					case BF_OFFSET:
					{
						if(!isalnum(*t))
						{
							if(':' == *t)
							{
								if(getreg(ts,t,&breg)&&REG_ISDATA(&breg))
								{
									arg->offset.uint = (breg.idx-REG_D0)+BF_REGISTER;
								}
								else if(!getval(ts,t,&arg->offset,1))
									fail(EPARSE_INVALID_BITFIELD);
								bfst = BF_WIDTH;
								ts = t+1;
							}
							else fail(EPARSE_INVALID_BITFIELD);
						}
					}
					break;
					case BF_WIDTH:
					{
						if(!isalnum(*t))
						{
							if('}' == *t)
							{
								if(getreg(ts,t,&breg)&&REG_ISDATA(&breg))
								{
									arg->width.uint = (breg.idx-REG_D0)+BF_REGISTER;
								}
								else if(!getval(ts,t,&arg->width,1))
									fail(EPARSE_INVALID_BITFIELD);
								bfst = BF_END;
							}
							else fail(EPARSE_INVALID_BITFIELD);
						}
					}
					break;
					case BF_END: fail(EPARSE_INVALID_BITFIELD);
				}
				t++;
			}

			if(bfst != BF_END) fail(EPARSE_INVALID_BITFIELD);
		}
/*
		for(i=0;i<ofs;i++)
		{
			char buf[256];

			strncpy(buf,args[i],arge[i]-args[i]);
			buf[arge[i]-args[i]] = '\0';
			printf("arg %ld: %s\n",i,buf);
		}
		printf("arguments: %ld %ld\n",ofs,stack);
*/
		//if(stack && t != (end-1)) fail(EPARSE_UNMATCHED_PARANTHESES);
                if(stack) fail(EPARSE_UNMATCHED_PARANTHESES);

		switch(ofs)
		{
			case 0: fail(ECODE_INVALID_OPERAND); break;
			case 1:
			{
				// possible modes:
				// AM_REGISTER_DIRECT_DATA			OK
				// AM_REGISTER_DIRECT_ADDRESS			OK
				// AM_REGISTER_INDIRECT_ADDRESS			OK
				// AM_REGISTER_INDIRECT_ADDRESS_POSTINC		OK
				// AM_REGISTER_INDIRECT_ADDRESS_PREDEC		OK
				// AM_ABSOLUTE_SHORT				OK
				// AM_ABSOLUTE_LONG				OK
				// AM_IMMEDIATE					OK
				// AM_CCR					OK
				// AM_SR					OK

				if(getreg(args[0],arge[0],&reg))
				{
					if(REG_ISDATA(&reg)&&REG_ISPLAIN(&reg))
					{
						arg->reg = reg;
						REG_REMAP(&arg->reg,RSIZE_L);
						arg->mode = AM_REGISTER_DIRECT_DATA;
					}
					else if(REG_ISADDR(&reg)&&REG_ISPLAIN(&reg))
					{
						arg->areg = reg;
						REG_REMAP(&arg->areg,RSIZE_L);
						arg->mode = AM_REGISTER_DIRECT_ADDRESS;
					}
					else if(reg.idx == REG_CCR)
					{
						arg->mode = AM_CCR;
					}
					else if(reg.idx == REG_SR)
					{
						arg->mode = AM_SR;
					}
				}
				else if(args[0][0] == '#')
				{
					if(getval(args[0]+1,arge[0],&arg->value,1))
						arg->mode = AM_IMMEDIATE;
				}
				else if((*start == '(') && getreg(args[0]+1,arge[0]-2,&reg) && !strncasecmp(")+",arge[0]-2,2))
				{
					if(REG_ISADDR(&reg)&&REG_ISPLAIN(&reg))
					{
						arg->areg = reg;
						arg->mode = AM_REGISTER_INDIRECT_ADDRESS_POSTINC;
					}
				}
				else if(!strncasecmp("-(",args[0],2) && getreg(args[0]+2,arge[0]-1,&reg) && (arge[0][-1] == ')'))
				{
					if(REG_ISADDR(&reg)&&REG_ISPLAIN(&reg))
					{
						arg->areg = reg;
						arg->mode = AM_REGISTER_INDIRECT_ADDRESS_PREDEC;
					}
				}
				else if((args[0][0] == '(') && getreg(args[0]+1,arge[0]-1,&reg) && (arge[0][-1] == ')'))
				{
					if(REG_ISPLAIN(&reg)&&REG_ISADDR(&reg))
					{
						arg->areg = reg;
						arg->mode = AM_REGISTER_INDIRECT_ADDRESS;
					}
					else if(REG_ISADDR(&reg)||REG_ISDATA(&reg)||REG_ISPC(&reg))
					{
						if(REG_ISPC(&reg))
							arg->areg = reg;
						else
							arg->reg = reg;
						arg->mode = REG_ISPC(&reg) ? AM_PC_INDIRECT_INDEX_BASE_DISPLACE : AM_ADDRESS_INDIRECT_INDEX_BASE_DISPLACE;
					}
				}
				else
				{
					if(getval(args[0],arge[0],&arg->value,1))
						arg->mode = (arg->value.size == VSIZE_W) ? AM_ABSOLUTE_SHORT : AM_ABSOLUTE_LONG;
				}
			}
			break;
			case 2:
			case 3:
			{
				int dcount = 0;
				value_t disp[1];
				int rcount = 0;
				asmreg_t regs[2];

				memset(disp,0,sizeof(disp));
				memset(regs,0,sizeof(regs));

				disp[0].size = VSIZE_Z;
				regs[0].idx = REG_ERR;
				regs[0].scale = RSCALE_1;
				regs[0].size = RSIZE_DEFAULT;
				regs[1].idx = REG_ERR;
				regs[1].scale = RSCALE_1;
				regs[1].size = RSIZE_DEFAULT;

				for(i=0;i<ofs;i++)
				{
					if(getreg(args[i],arge[i],&regs[rcount]))
					{
						if(!argstack[i]) fail(ECODE_INVALID_ADDRESSING_MODE);

						// valid register, make sure it's one we want

						if(REG_ISDATA(&regs[rcount])||REG_ISADDR(&regs[rcount])||REG_ISPC(&regs[rcount]))
						{
							if(rcount++>1) fail(EPARSE_TOO_MANY_REGISTERS);
						}
						else
						{
							fail(EPARSE_INVALID_REGISTER);
						}
					}
					else
					{
						if(getval(args[i],arge[i],&disp[dcount],1))
						{
							if(dcount++>0) fail(EPARSE_TOO_MANY_DISPLACEMENTS);
						}
					}

				}

				{
					int index = -1, address = -1;

					for(i=0;i<rcount;i++)
					{
						if((REG_ISADDR(&regs[i]) && (regs[i].scale == RSCALE_1) && ((regs[i].size == RSIZE_L)||regs[i].size == RSIZE_DEFAULT))||REG_ISPC(&regs[i]))
						{
							if(address<0)
								address = i;
							else if((address>=0)&&(index<0))
								index = i;
							else fail(EPARSE_TOO_MANY_REGISTERS);
						}
						else
						{
							if(index < 0)
								index = i;
							else fail(EPARSE_TOO_MANY_REGISTERS);
						}
					}

					if((address>=0) && (index<0) && dcount && (((int)disp[0].uint) >= -32768) && (((int)disp[0].uint) <= 32767))
					{
						arg->areg = regs[address];
						arg->value = disp[0];
						arg->mode = REG_ISPC(&regs[address]) ? AM_PC_INDIRECT_DISPLACE : AM_REGISTER_INDIRECT_ADDRESS_DISPLACE;
					}
					else
					{
						if(address>=0)
							arg->areg = regs[address];

						if(index>=0)
							arg->reg = regs[index];

						if(dcount)
						{
							if((((int)disp[0].uint) >= -128) && (((int)disp[0].uint) <= 127))
								arg->mode = ((address>=0)&&REG_ISPC(&regs[address])) ? AM_PC_INDIRECT_INDEX_8BIT_DISPLACE : AM_ADDRESS_INDIRECT_INDEX_8BIT_DISPLACE;
							else
								arg->mode = ((address>=0)&&REG_ISPC(&regs[address])) ? AM_PC_INDIRECT_INDEX_BASE_DISPLACE : AM_ADDRESS_INDIRECT_INDEX_BASE_DISPLACE;
						}
						else
							arg->mode = ((address>=0)&&REG_ISPC(&regs[address])) ? AM_PC_INDIRECT_INDEX_8BIT_DISPLACE : AM_ADDRESS_INDIRECT_INDEX_8BIT_DISPLACE;
						arg->value = disp[0];
					}
				}
			}
			break;
		}
	}

	REG_REMAP(&arg->areg,RSIZE_L);
	REG_REMAP(&arg->reg,RSIZE_W);

	return 1;
}
