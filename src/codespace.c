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
 *	$Id: codespace.c,v 1.12 2003/06/23 15:51:38 jesper Exp $
 *	$Log: codespace.c,v $
 *	Revision 1.12  2003/06/23 15:51:38  jesper
 *	*** empty log message ***
 *	
 *	Revision 1.11  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.10  2003/06/22 12:02:32  jesper
 *	more msvc fixes
 *	
 *	Revision 1.9  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.8  2003/06/21 21:01:45  jesper
 *	added reloc-tables to sections
 *	
 *	Revision 1.7  2003/06/20 10:07:05  jesper
 *	Alignment testing of code added
 *	
 *	Revision 1.6  2003/06/20 08:10:28  jesper
 *	new code-generation complete
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

/*

	TODO:
	
	- add support for exporting and importing symbols to be resolved at
	linker time. Remake the get_symbol_offset() into returning a proper
	structure signaling this fact.

	- Add address-resolving to the optimizer-pass, to make sure everything
	fits to their extension-words.

	- Add resolver-tables for sections.

*/

#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#include "types.h"
#include "codespace.h"
#include "error.h"
#include "ahf.h"
#include "endian.h"

static symbol_t* root;
static symbol_t* cs_get_symbol( const char* name );

static cs_section_t* curr;
static cs_section_t* first;
static cs_section_t* last;
static cs_section_t* store;

int cs_setup()
{
	root = NULL;

	first = NULL;
	last = NULL;
	curr = NULL;
	store = NULL;

	return 1;
}

void cs_free()
{
}

symbol_t* cs_create_symbol( const char* start, const char* end, uint32_t flags )
{
	char buf[256];	
	symbol_t* sym = NULL;

	if(!end) end = start + strlen(start);

	if(end < start) return NULL;
	if((end-start) > 255) return NULL;

	strncpy(buf,start,end-start);
	buf[end-start] = '\0';

	// TODO: verify that the symbol does not contain any illegal characters

	// make sure symbol has not yet been created

	sym = cs_get_symbol(buf);
	if(sym)
	{
		// can't be declared more than once

		// TODO: this test needs much improvement

		if((sym->flags & SF_DECLARE) && (flags & SF_DECLARE))
			return NULL;

		// merge flags

		sym->flags |= flags;
	}
	else
	{
		// create new symbol

		sym = calloc(sizeof(symbol_t),1);
		if(!sym) return NULL;

		strncpy(sym->name,buf,sizeof(sym->name));
		sym->name[sizeof(sym->name)-1] = 0;
		sym->flags = flags;

		// attach to list

		if(SYM_ISLOCAL(buf))
		{
			// leak, so shoot me! :)
			if(!root) return NULL;	

			sym->root = root;
			sym->next = root->locals;
			root->locals = sym;
		}
		else
		{
			sym->root = NULL;
			sym->next = root;
			root = sym;
		}
	}

	return sym;
}

void cs_set_symbol_expression( symbol_t* sym, const char* expression )
{
	if(sym->flags & SF_EXPRESSION)
	{
		if(sym->expression)
			free(sym->expression);
		sym->expression = strdup(expression);
	}
}

static symbol_t* cs_get_symbol( const char* name )
{
	symbol_t* sym;

	if(SYM_ISLOCAL(name))
	{
		if(!root) return NULL;
		sym = root->locals;
	}
	else
		sym = root;		

	while(sym)
	{
		if(!strcmp(name,sym->name))
			break;

		sym = sym->next;
	}

	return sym;
}

static cs_section_t* cs_get_section( const char* name )
{
	cs_section_t* sec = first;

	// validate name before scanning

	if(strlen(name) != strspn(name,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$_"))
		fail(EPARSE_INVALID_SECTION_NAME);

	while(sec)
	{
		if(!strcasecmp(sec->name,name))
			break;

		sec = sec->next;
	}

	return sec;
}

cs_section_t* cs_create_section( const char* name, uint32_t flags )
{
	cs_section_t* sec = cs_get_section(name);

	if(sec)
	{
		// doesn't flags match? fail

		if(sec->flags != flags)
			return NULL;

		return (curr = sec);
	}

	// section not found, create a new one

	sec = calloc(sizeof(cs_section_t),1);
	if(!sec) return NULL;

	strncpy(sec->name,name,sizeof(sec->name));
	sec->name[sizeof(sec->name)-1] = 0;
	sec->flags = flags;

	// attach to section-list

	if(first)
	{
		// we have a section in the list already, attach to end of list


		last->next = sec;
		last = sec;
	}
	else
	{
		// first section to be created, setup list

		first = last = sec;
	}

	return (curr = sec);
}


void cs_push_section()
{
	if(!curr) cs_create_section("code",ST_CODE);

	store = curr;
}

void cs_pop_section()
{
	if(!store) fail(ECODE_RESTORE_WITHOUT_STORE);

	curr = store;
	store = NULL; 
}

void cs_push_instr(cs_instr_t* instr)
{
	cs_instr_t* out = calloc(1,sizeof(cs_instr_t));
	unsigned int i,size = 0;

	if(!curr) cs_create_section("code",ST_CODE);

	// fetch opcode

	switch(instr->opsize)
	{
		case 2:
		{
			out->opsize = instr->opsize;
			out->ops = instr->ops;			
		}
		break;

		default: fail(ECODE_INVALID_INSTRUCTION_SIZE); break;
	}


	// copy EAs

	for(i=0;i<instr->easize;i++)
	{
		out->ea[i] = instr->ea[i];
		size += instr->ea[i].num_ew*2;
	}
	out->easize = instr->easize;

	size += instr->opsize; 

	// setup instruction and attach

	out->entry.size = size;
	out->entry.type = CSENTRY_INSTR;

	cs_push_entry(curr,&out->entry);
}

void cs_push_symbol( symbol_t* symbol )
{
	cs_sym_t* out = calloc(1,sizeof(cs_sym_t));

	if(!curr) cs_create_section("code",ST_CODE);

	symbol->sec = curr;

	out->symbol = symbol;
	out->entry.size = 0;
	out->entry.type = CSENTRY_SYMBOL;

	cs_push_entry(curr,&out->entry);
}

void cs_push_data( uint32_t size, uint32_t data, uint32_t repeat, symbol_t* symbol )
{
	cs_data_t* out = calloc(1,sizeof(cs_data_t));

	if(!curr) cs_create_section("code",ST_CODE);

	if(symbol && (size != 4))
		fail(ECODE_INVALID_ADDRESS_SIZE);

	out->dsize = size;
	out->repsize = repeat; 

	switch(size)
	{
		case 1: out->db = data; break;
		case 2: out->ds = data; break;
		case 4: out->dl = data; break;
		default: fail(ECODE_INVALID_INSTRUCTION_SIZE); break;
	}

	out->tag = symbol;

	out->entry.size = size * repeat;
	out->entry.type = CSENTRY_DATA;

	cs_push_entry(curr,&out->entry);
}

void cs_push_entry( cs_section_t* section, cs_entry_t* entry )
{
	entry->owner = section;

	if(section->first)
	{
		section->curr->next = entry;
		entry->prev = section->curr;
		entry->next = NULL;
		section->curr = entry;
	}
	else
	{
		section->curr = section->first = entry;
		entry->prev = NULL;
		entry->next = NULL;
	}
}

uint32_t cs_feed( cs_section_t* section, uint32_t size, uint32_t repeat, uint8_t* data )
{
	// only increase size with BSS sections

	if(section->flags & ST_BSS)
		return (section->size += (size*repeat));

	// allocate more bufferspace if needed

	if(!section->data)
		section->data = calloc(1,(section->block = 4096));

	// copy data

	while(repeat--)
	{
		if((section->size + size) > section->block)
		{
			section->block = ((section->block + size) + 4095) & 4096;
			section->data = realloc(section->data,section->block);
		}

		memcpy(section->data + section->size, data, size);

		section->size += size;
	}

	return section->size;
}

uint32_t cs_write_ea( uint32_t pc, cs_instr_t* instr, eaddr_t* ea )
{
	switch(ea->num_ew)
	{
		case 0: break;
		case 1:
		{
			if(ea->syms[0])
			{
				uint32_t ofs;

				if(ea->syms[0]->sec != instr->entry.owner)
					fail(ECODE_RELATIVE_ADDRESS_TOO_LARGE);

				ofs = get_symbol_offset(ea->syms[0]);

				if(((ofs-pc) > -32768) || ((ofs-pc) > 32767))
					fail(ECODE_RELATIVE_ADDRESS_TOO_LARGE);

				uint16_t val = be16(ofs-pc);
				cs_feed(curr,2,1,(uint8_t*)&val);
			}
			else
			{
				uint16_t val = be16(ea->ew[0]);
				cs_feed(curr,2,1,(uint8_t*)&val);
			}
		}
		break;
		case 2:
		{
			if(ea->syms[0])
			{
				uint32_t ofs = be32(get_symbol_offset(ea->syms[0]));
				cs_push_reloc(instr->entry.owner,ea->syms[0]->sec,pc);

				cs_feed(curr,4,1,(uint8_t*)&ofs);
			}
			else
			{
				uint32_t val = be32((ea->ew[0]<<16)|(ea->ew[1]));
				cs_feed(curr,4,1,(uint8_t*)&val);
			}
		}
		break;
		default: fail(ECODE_INVALID_INSTRUCTION_SIZE); // not supporting any more currently, sorry
	}

	pc += ea->num_ew*2;

	return pc;
}

uint32_t cs_write_instr( uint32_t pc, cs_instr_t* instr )
{
	unsigned int i;

	if(pc&1)
		fail(ECODE_MISALIGNED_INSTRUCTION);

	switch(instr->opsize)
	{
		case 2:
		{
			uint16_t val = be16(instr->ops);
			cs_feed(curr,2,1,(uint8_t*)&val);
		}
		break;
		default: fail(ECODE_INVALID_INSTRUCTION_SIZE);
	}

	pc += instr->opsize;

	for(i=0;i<instr->easize;i++)
		pc = cs_write_ea(pc,instr,&instr->ea[i]);


	return pc;
}

uint32_t cs_write_data( uint32_t pc, cs_data_t* data )
{
	switch(data->dsize)
	{
		case 1: cs_feed(curr,1,1,&(data->db)); break;
		case 2:
		{
			uint16_t val = be16(data->ds);
			cs_feed(curr,2,1,(uint8_t*)&val);
		}
		break;
		case 4:
		{
			if(data->tag)
			{
				uint32_t ofs = be32(get_symbol_offset(data->tag));

				cs_push_reloc(data->entry.owner,data->tag->sec,pc);
				cs_feed(curr,4,1,(uint8_t*)&ofs);
			}
			else
			{
				uint32_t val = be32(data->dl);
				cs_feed(curr,4,1,(uint8_t*)&val);
			}
		}
		break;
	}

	pc += data->dsize;

	return pc;
}

void cs_write( cs_format_t fmt, const char* filename )
{
	cs_section_t* sec = first;

	while(sec)
	{
		cs_entry_t* entry = sec->first;
		uint32_t pc = 0;

		curr = sec;

		while(entry)
		{
			switch(entry->type)
			{
				case CSENTRY_INSTR:
					pc = cs_write_instr(pc,(cs_instr_t*)entry);
				break;

				case CSENTRY_SYMBOL: break;
				case CSENTRY_DATA:
					pc = cs_write_data(pc,(cs_data_t*)entry);
				break;
			}

			entry = entry->next;
		}
		sec = sec->next;
	}

	switch(fmt)
	{
		case CSF_AMIGAOS:
		{
			ahf_handle* handle;
			if((handle = ahf_open(filename)))
			{
				cs_section_t* curr = first;

				while(curr)
				{
					ahf_register(handle,curr);
					curr = curr->next;
				}

				ahf_close(handle);
			}
		}
		break;
	}
}

uint32_t get_symbol_offset( symbol_t* symbol )
{
	uint32_t ofs = 0;
  cs_entry_t* entry = NULL;

	if(!symbol->sec)
		fail(ECODE_UNDECLARED_SYMBOL);

	entry = symbol->sec->first;
	while(entry)
	{
		if((entry->type == CSENTRY_SYMBOL) && (((cs_sym_t*)entry)->symbol == symbol))
			break;

		ofs += entry->size;
		entry = entry->next;
	}

	if(!entry)
		fail(ECODE_UNDECLARED_SYMBOL);

	return ofs;
}

void cs_push_reloc( cs_section_t* owner, cs_section_t* target, uint32_t offset )
{
	cs_reloc_t* out = calloc(1,sizeof(cs_reloc_t));

	if(offset&1)
		fail(ECODE_MISALIGNED_ADDRESS);

	out->owner = owner;
	out->target = target;
	out->offset = offset;
	out->next = NULL;

	if(owner->lreloc)
	{
		owner->lreloc->next = out;
		owner->lreloc = out;
	}
	else
	{
		owner->freloc = owner->lreloc = out;
	}
}
