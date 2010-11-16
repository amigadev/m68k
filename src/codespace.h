#ifndef __CODESPACE_H__
#define __CODESPACE_H__

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
 *	$Id: codespace.h,v 1.9 2003/06/22 22:47:42 jesper Exp $
 *	$Log: codespace.h,v $
 *	Revision 1.9  2003/06/22 22:47:42  jesper
 *	first pass of AHF output complete
 *	
 *	Revision 1.8  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.7  2003/06/21 21:01:45  jesper
 *	added reloc-tables to sections
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
   

#include "operand.h"

enum
{
	SF_DECLARE	= 0x00000001,	// declare symbol (calling another time with declaration will fail)
	SF_REFERENCE	= 0x00000002,
	SF_EXPORT	= 0x00000004,
	SF_IMPORT	= 0x00000008,
	SF_EXPRESSION	= 0x00000010,	// this is not a symbol but a expression
	SF_CHANGEABLE	= 0x00000020,	// use with SF_VALUE to reflect that the value can be altered (sym SET val)
};

struct cs_section_t;
typedef struct symbol_t
{
	char name[256]; // this should be changed
	uint32_t flags;

	char* expression;		// declared expressions (EQU, SET, etc.)

	struct cs_section_t* sec;	// section owning this symbol

	struct symbol_t* root;
	struct symbol_t* next;
	struct symbol_t* locals;	// for local symbol-handling
} symbol_t;

typedef enum
{
	CSENTRY_INSTR,
	CSENTRY_DATA,
	CSENTRY_SYMBOL
} cs_type_t;

typedef struct cs_entry_t
{
	struct cs_entry_t* prev;
	struct cs_entry_t* next;

	struct cs_section_t* owner;

	uint32_t size; // size of entry in bytes

	cs_type_t type;
} cs_entry_t;

typedef struct cs_instr_t
{
	cs_entry_t entry;

	uint32_t opsize; // size of initial opcode in bytes (not including EA)
	union
	{
		unsigned short ops;
		unsigned long opl;
	};
	
	// we need a symbol tag for instructions like bra.s

	uint32_t easize; // number of EAs
	eaddr_t ea[2]; // 2 ea's are max as of now, increase if needed
} cs_instr_t;

typedef struct cs_data_t
{
	cs_entry_t entry;

	int dsize; // size of data-entry
	int repsize; // number of times to repeat
	
	union
	{
		unsigned char db;	// 1 bytes
		unsigned short ds;	// 2 bytes
		unsigned long dl;	// 4 bytes
	};

	symbol_t* tag; // valid only for long data

	// TODO: fill this in with a proper layout

} cs_data_t;

typedef struct cs_sym_t
{
	cs_entry_t entry;
	symbol_t* symbol;
} cs_sym_t;

enum
{
	ST_CODE = 0x01,
	ST_DATA = 0x02,
	ST_BSS	= 0x04,

	ST_CHIP	= 0x10,
	ST_FAST = 0x20,
};

typedef struct cs_section_t
{
	char name[256];
	uint32_t flags;

	cs_entry_t* first;
	cs_entry_t* curr;

	uint32_t size;	// used block-size
	uint32_t block;	// size of available block
	uint8_t* data;	// data-block, valid if not BSS

	struct cs_reloc_t* freloc;
	struct cs_reloc_t* lreloc;

	struct cs_section_t* next;
} cs_section_t;

typedef struct cs_reloc_t
{
	struct cs_reloc_t* next;

	cs_section_t* owner;
	cs_section_t* target;

	uint32_t offset;
} cs_reloc_t;

typedef enum
{
	CSF_AMIGAOS,	// amiga hunk format, linkable object file
} cs_format_t;

int cs_setup();
void cs_free();

symbol_t* cs_create_symbol( const char* start, const char* end, uint32_t flags );
void cs_set_symbol_expression( symbol_t* sym, const char* expression );

cs_section_t* cs_create_section( const char* name, uint32_t flags );

void cs_push_section();
void cs_pop_section();

void cs_push_symbol( symbol_t* symbol );
void cs_push_instr( cs_instr_t* instr );

void cs_push_data( uint32_t size, uint32_t data, uint32_t repeat, symbol_t* symbol );
//void cs_push_data_ptr( uint32_t size, void* data, symbol_t* symbol );  -- TODO: implement this one too

void cs_push_reloc( cs_section_t* owner, cs_section_t* target, uint32_t offset );

uint32_t get_symbol_offset( symbol_t* symbol );

void cs_push_entry( cs_section_t* section, cs_entry_t* entry );

void cs_write( cs_format_t fmt, const char* filename );

// a local symbol starts with '.' or ends with '$'
#define SYM_ISLOCAL(x) (('.' == (x)[0])||('$' == (x)[strlen((x))-1]))

#endif
