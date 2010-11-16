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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "arguments.h"

/*
 *	$Id: arguments.c,v 1.4 2003/06/23 15:51:38 jesper Exp $
 *	$Log: arguments.c,v $
 *	Revision 1.4  2003/06/23 15:51:38  jesper
 *	*** empty log message ***
 *	
 *	Revision 1.3  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

typedef struct argentry_t
{
	struct argentry_t* next;
	char* svalue;
	uint32_t value;
	float fvalue;
} argentry_t;

typedef struct argument_t
{
	char name[64];
	char abbrev[16];
	int type;	
	int count;
	argentry_t* entry;
} argument_t;

argument_t argentries[] = 
{
	{"file","f",AT_STRING,0,NULL},
	{"output","o",AT_STRING,0,NULL},
	{"optimize","O",AT_INT32,0,NULL},
	{"","",0L,0,NULL}
};

int arginit()
{
	return 1;
}

void argexit()
{
}

int argcount( unsigned int index )
{
	return argentries[index].count;
}

const char* argstring( unsigned int index, unsigned int value )
{
	argentry_t* t = argentries[index].entry;

	while(t && ((value--)>0)) t = t->next;

	return t ? t->svalue : NULL;
}

int arginsert( argument_t* arg, const char* value )
{
	argentry_t* out = NULL;

	switch(arg->type)
	{
		case AT_STRING:
		{
			if(NULL != (out = calloc(sizeof(argentry_t),1)))
			{
				if(!(out->svalue = calloc(strlen(value)+1,1)))
				{
					free(out);
					return 0;
				}
				strcpy(out->svalue,value);
			}
		}
		break;

		case AT_INT32:
		{
			char* e;
			int ival = strtol(value,&e,10);

			if((e-value) != strlen(value))
				return 0;

			if(!(out = calloc(sizeof(argentry_t),1)))
				return 0;			

			out->value = ival;
		}
		break;

		case AT_FLOAT:
		{
			return 0;
		}
		break;
		default: return 0;
	}

	if(!out) return 0;

	out->next = arg->entry;
	arg->entry = out;
	arg->count++;

	return 1;
}

int argparse( int argc, const char** argv )
{
	int i,j;

	for(i=1; i < argc; i++)
	{
		if(!strncmp("--",argv[i],2))
		{
			j = 0;
			while(argentries[j].name[0])
			{
				if(!strcmp(argentries[j].name,argv[i]+2))
				{
					if(++i < argc)
					{
						if(!arginsert(&argentries[j],argv[i]))
							return 0;
					}
					else return 0;

					break;
				}
				j++;
			}
		}
		else if(!strncmp("-",argv[i],1))
		{
			j = 0;
			while(argentries[j].name[0])
			{
				if(!strncmp(argentries[j].abbrev,argv[i]+1,strlen(argentries[j].abbrev)))
				{
					const char* arg = NULL;
					if(strlen(argv[i]) == (strlen(argentries[j].abbrev)+1))
					{
						if(++i < argc)
						{
							arg = argv[i];
						}
						else return 0;
					}
					else
					{
						arg = argv[i] + (1 + strlen(argentries[j].abbrev));
					}
							
					if(arg)
					{
						if(!arginsert(&argentries[j],arg))
							return 0;
					}

					break;
				}
				j++;
			}
		}
		else
		{
			if(!arginsert(&argentries[ARG_FILENAMES],argv[i]))
				return 0;
		}
	}

	return 1;
}
