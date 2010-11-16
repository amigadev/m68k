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
 *	$Id: operand.c,v 1.8 2003/06/22 22:47:42 jesper Exp $
 *	$Log: operand.c,v $
 *	Revision 1.8  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.7  2003/06/22 18:58:05  jesper
 *	fixed bug with negative values
 *	
 *	Revision 1.6  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.5  2003/06/20 08:09:20  jesper
 *	incorrect EA generation for absolute short
 *	
 *	Revision 1.4  2003/06/19 14:55:30  jesper
 *	code generation update
 *	
 *	Revision 1.3  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "types.h"
#include "mangle.h"
#include "operand.h"
#include "codespace.h"
#include "error.h"

#ifndef NULL
#define NULL (0)
#endif

char* getstring( const char* start, const char* end )
{
	char* str = calloc((end-start)+1,1);
	if(!str) return NULL;

	strncpy(str,start,end-start);
	str[end-start] = 0;

	return str;
}

enum
{
	ST_REG,
	ST_SIZE,
	ST_SCALE,
	ST_END

};

int getreg( const char* start, const char* end, asmreg_t* reg )
{
	const char* t = start;
	int state = ST_REG;
	reg->idx = REG_ERR;
	reg->scale = RSCALE_1;
	reg->size = RSIZE_DEFAULT;

	if((end - start) <= 0)
		return 0;

	if(end-start <= 0)
		return REG_ERR;

	// extract scaling

	while(t <= end)
	{
		switch(state)
		{
			case ST_REG:
			{
				if((t == end)||!isalnum(*t))
				{
					// did we process anything?

					if(t == start)
						return 0;


					if(('d' == tolower(*start))||('a' == tolower(*start)))
					{
						char* e;
						int v;
						// address or data-register

						v = strtol(start+1,&e,10);

						if(t == start)
							return 0;

						reg->idx = v + (('d' == tolower(*start)) ? REG_D0 : REG_A0);

						if('.' == *t)
							state = ST_SIZE;
						else if('*' == *t)
							state = ST_SCALE;
						else
							state = ST_END;
					}
					else if(!strncasecmp("pc",start,t-start))
					{
						reg->idx = REG_PC;
						state = ST_END;
					}
					else if(!strncasecmp("zpc",start,t-start))
					{
						reg->idx = REG_ZPC;
						state = ST_END;
					}
					else if(!strncasecmp("sp",start,t-start))
					{
						reg->idx = REG_A7;
						state = ST_END;

						if('.' == *t)
							state = ST_SIZE;
						else if('*' == *t)
							state = ST_SCALE;
						else
							state = ST_END;
					}
					else if(!strncasecmp("ccr",start,t-start))
					{
						reg->idx = REG_CCR;
						state = ST_END;
					}
					else if(!strncasecmp("sr",start,t-start))
					{
						reg->idx = REG_SR;
						state = ST_END;
					}
				}
			}
			break;

			case ST_SIZE:
			{
				if(isalpha(*t))
				{
					if('w' == tolower(*t))
						reg->size = RSIZE_W;
					else if('l' == tolower(*t))
						reg->size = RSIZE_L;
					else return 0;
				}
				else
				{
					if('*' == *t)
						state = ST_SCALE;
					else
						state = ST_END;
				}
			}
			break;
			case ST_SCALE:
			{
				if(isdigit(*t))
				{
					if('1' == *t)
						reg->scale = RSCALE_1;
					else if('2' == *t)
						reg->scale = RSCALE_2;
					else if('4' == *t)
						reg->scale = RSCALE_4;
					else if('8' == *t)
						reg->scale = RSCALE_8;

					state = ST_END;
				}
				else
					return 0;
			}
			break;

			// if we process anything at all in this state, fail			
			case ST_END:  if(t!=end) return 0;
		}

		t ++;
	}

	if(reg->idx != REG_ERR)
		return 1;

	return 0;
}

// TODO: add support for:
//	calculations
//	strings
//	characters
//	...
int getval( const char* start, const char* end, value_t* val, int allow_symbol )
{
	const char* t = start;
	char* e = NULL;
	int hex = 0,neg = 1;
 
	val->uint = 0;
	val->size = VSIZE_Z;
	val->symbol = NULL;

	if(allow_symbol && (isalpha(*start)||('.' == *start)||('_' == *start)))
	{
		symbol_t* sym = cs_create_symbol(start,end,SF_REFERENCE);

		// enforce to long right now...
		val->size = VSIZE_L;
		val->symbol = sym;

		return 1;
	}

	if('$' == *start)
		hex++,t++,start++;

	if('-' == *start)
		neg-=neg*2,t++,start++;

	while(t < end)
	{
		if(!(((*t>='0') && (*t<='9')) || (hex && (tolower(*t)>='a') && (tolower(*t)<='f'))))
		{
			if(t == (end-2) && ('.' == *t))
			{
				if('w' == tolower(*(end-1)))
					val->size = VSIZE_W;
				else if('l' == tolower(*(end-1)))
					val->size = VSIZE_L;
				else return 0; // invalid size-specifier

				break;
			}
			else return 0; // invalid character in value
		}

		t++;
	}

	val->uint = (uint32_t)(strtol(start,&e,hex?16:10)*neg);

	if((t == e) && (val->size == VSIZE_Z))
	{
		if(val->uint<65536)
			val->size = VSIZE_W;
		else
			val->size = VSIZE_L;
	}

	if((val->size == VSIZE_W)&&(((int)val->uint)<0))
		val->uint &= 0xffff;

	if((val->size == VSIZE_W)&&(val->uint>65535))
		fail(EPARSE_VALUE_OUT_OF_BOUNDS);

	return (t == e);
}

int get_ea( mangle_t* mangle, operand_t* arg, eaddr_t* ea )
{
	memset(ea,0,sizeof(eaddr_t));

	switch(arg->mode)
	{
		case AM_ERROR: fail(ECODE_INVALID_ADDRESSING_MODE);

		case AM_REGISTER_DIRECT_DATA:
		{
			ea->mode = 0;
			ea->reg = arg->reg.idx-REG_D0;
		}
		break;

		case AM_REGISTER_DIRECT_ADDRESS:
		{
			ea->mode = 1;
			ea->reg = arg->areg.idx-REG_A0;
		}
		break;

		case AM_REGISTER_INDIRECT_ADDRESS:
		{
			ea->mode = 2;
			ea->reg = arg->areg.idx-REG_A0;
		}
		break;
		
		case AM_REGISTER_INDIRECT_ADDRESS_POSTINC:
		{
			ea->mode = 3;
			ea->reg = arg->areg.idx-REG_A0;
		}
		break;
		
		case AM_REGISTER_INDIRECT_ADDRESS_PREDEC:
		{
			ea->mode = 4;
			ea->reg = arg->areg.idx-REG_A0;
		}
		break;

		case AM_REGISTER_INDIRECT_ADDRESS_DISPLACE:
		{
			ea->mode = 5;
			ea->reg = arg->areg.idx-REG_A0;
			ea->num_ew = 1;
			ea->ew[0] = arg->value.uint;
		}
		break;

		case AM_ADDRESS_INDIRECT_INDEX_8BIT_DISPLACE:
		{
			ea->mode = 6;
			ea->reg = arg->areg.idx-REG_A0;
			ea->num_ew = 1;
			ea->ew[0] = (arg->reg.idx << 12)|(arg->reg.size << 11)|(arg->reg.scale << 9)|(((char)arg->value.uint)&0xff);
		}
		break;

		case AM_ADDRESS_INDIRECT_INDEX_BASE_DISPLACE:
		{
			ea->mode = 6;
			ea->num_ew = 1;
			ea->ew[0] = 1<<8; // sets a full extension-word

			if(arg->areg.idx != REG_ERR)
				ea->reg = arg->areg.idx-REG_A0;
			else
			{
				ea->reg = 0;
				ea->ew[0] |= 1<<6;	// suppress base
			}

			// index register
			if(arg->reg.idx != REG_ERR)
				ea->ew[0] |= arg->reg.idx<<12; // register index & type
			else
				ea->ew[0] |= (1<<6); // suppress index register
			ea->ew[0] |= (arg->reg.size&1) << 11; // index size
			ea->ew[0] |= (arg->reg.scale&3)<<9; // scale factor

			// displacement
			if(arg->value.size == VSIZE_W)
			{
				ea->num_ew += 1;
				ea->ew[1] = (short)arg->value.uint;
			}
			else if(arg->value.size == VSIZE_L)
			{
				ea->num_ew += 2;
				ea->ew[1] = (arg->value.uint>>16)&0xffff;
				ea->ew[2] = arg->value.uint&0xffff;
			}
			ea->ew[0] |= (arg->value.size&3)<<4;
		}
		break;
		
		case AM_ABSOLUTE_SHORT:
		{
			if(arg->value.size == VSIZE_W)
			{
				// enforced short mode, fail if invalid address

				if(arg->value.uint > 65535) fail(EPARSE_ADDRESS_OUT_OF_BOUNDS);

				// EA: absolute short
				ea->mode = 7;
				ea->reg = 0;
				ea->num_ew = 1;
				ea->ew[0] = arg->value.uint;
			}
			else
			{
				// flexible mode, change to long if invalid address

				if(arg->value.uint > 65535)
				{

					// EA: absolute long
					ea->mode = 7;
					ea->reg = 1;
					ea->num_ew = 2;
					ea->syms[0] = arg->value.symbol;
					ea->type[0] = EA_LABS;
					ea->ew[0] = (arg->value.uint>>16)&0xffff;
					ea->ew[1] = arg->value.uint&0xffff;
				}
				else
				{
					// EA: absolute short
					ea->mode = 7;
					ea->reg = 0;
					ea->num_ew = 1;
					ea->ew[0] = arg->value.uint;

					if(arg->value.symbol) fail(ECODE_INVALID_ADDRESS_SIZE);
				}
			}

		}
		break;

		case AM_ABSOLUTE_LONG:
		{
			if(arg->value.size == VSIZE_L)
			{
				// enforced long mode, don't optimize to short address

				ea->mode = 7;
				ea->reg = 1;
				ea->num_ew = 2;
				ea->ew[0] = (arg->value.uint>>16)&0xffff;
				ea->ew[1] = arg->value.uint&0xffff;
				ea->syms[0] = arg->value.symbol;
				ea->type[0] = EA_LABS;
			}
			else
			{
				if(arg->value.uint == ((short)(arg->value.uint&0xffff)))
				{
					// EA: absolute short
					ea->mode = 7;
					ea->reg = 0;
					ea->num_ew = 1;
					ea->ew[0] = arg->value.uint&0xffff;
				}
				else if(arg->value.size == VSIZE_W)
				{
					fail(EPARSE_INVALID_SIZE_OPERAND);
				}
				else
				{
					ea->mode = 7;
					ea->reg = 1;
					ea->num_ew = 2;
					ea->ew[0] = (arg->value.uint>>16)&0xffff;
					ea->ew[1] = arg->value.uint&0xffff;
					ea->syms[0] = arg->value.symbol;
					ea->type[0] = EA_LABS;
				}
			}
			
		}
		break;

		case AM_IMMEDIATE:
		{
			ea->mode = 7;
			ea->reg = 4;
			switch(mangle->opsize)
			{
				case OS_BYTE:
				{
					if( (((int)arg->value.uint)<-128) || (((int)arg->value.uint)>127) )
						fail(EPARSE_VALUE_OUT_OF_BOUNDS);

					ea->num_ew = 1;
					ea->ew[0] = arg->value.uint&0xff;
				}
				break;
				case OS_WORD:
				{
					if( (((int)arg->value.uint)<-32768) || (((int)arg->value.uint)>32767) )
						fail(EPARSE_VALUE_OUT_OF_BOUNDS);

					ea->num_ew = 1;
					ea->ew[0] = arg->value.uint&0xffff;
				}
				break;
				case OS_LONG:
				{
					ea->num_ew = 2;
					ea->ew[0] = (arg->value.uint>>16)&0xffff;
					ea->ew[1] = arg->value.uint&0xffff;
					ea->syms[0] = arg->value.symbol;
					ea->type[0] = EA_LABS;
				}
				break;
				default: printf("poop\n"); return 0;
			}
		}
		break;

		case AM_PC_INDIRECT_DISPLACE:
		{
			// rewrite if zero-pc specified
			if(REG_ZPC == arg->areg.idx)
			{
				arg->mode = AM_PC_INDIRECT_INDEX_BASE_DISPLACE;
				return get_ea(mangle,arg,ea);
			}

			ea->mode = 7;
			ea->reg = 2;
			ea->num_ew = 1;
			ea->ew[0] = arg->value.uint;
			ea->syms[0] = arg->value.symbol;
			ea->type[0] = EA_WREL;
		}
		break;

		case AM_PC_INDIRECT_INDEX_8BIT_DISPLACE:
		{
			// rewrite if zero-pc specified
			if(REG_ZPC == arg->areg.idx)
			{
				arg->mode = AM_PC_INDIRECT_INDEX_BASE_DISPLACE;
				return get_ea(mangle,arg,ea);
			}

			ea->mode = 7;
			ea->reg = 3;
			ea->num_ew = 1;
			ea->ew[0] = (arg->reg.idx<<12)|((arg->reg.size&1)<<11)|((arg->reg.scale&3)<<9)|(((char)arg->value.uint)&0xff);
		}
		break;

		case AM_PC_INDIRECT_INDEX_BASE_DISPLACE:
		{
			ea->mode = 7;
			ea->reg = 3;
			ea->num_ew = 1;
			ea->ew[0] = 1<<8;

			if(arg->areg.idx == REG_ZPC)
				ea->ew[0] |= 1<<7; // suppress PC

			// index register
			if(arg->reg.idx != REG_ERR)
				ea->ew[0] |= arg->reg.idx<<12; // register index & type
			else
				ea->ew[0] |= (1<<6); // suppress index register
			ea->ew[0] |= (arg->reg.size&1) << 11; // index size
			ea->ew[0] |= (arg->reg.scale&3)<<9; // scale factor

			// displacement
			if(arg->value.size == VSIZE_W)
			{
				ea->num_ew += 1;
				ea->ew[1] = (short)arg->value.uint;
			}
			else if(arg->value.size == VSIZE_L)
			{
				ea->num_ew += 2;
				ea->ew[1] = (arg->value.uint>>16)&0xffff;
				ea->ew[2] = arg->value.uint&0xffff;
			}
			ea->ew[0] |= (arg->value.size&3)<<4;
		}
		break;

		case AM_CCR:
		case AM_SR:
			fail(ECODE_INVALID_ADDRESSING_MODE);
		break;

		default: fail(EGLOBAL_MISSING_FEATURE);
	}
	return 1;
}
