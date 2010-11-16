#ifndef __ADDRMODE_H__
#define __ADDRMODE_H__

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
 *	$Id: operand.h,v 1.5 2003/08/23 11:44:55 jesper Exp $
 *	$Log: operand.h,v $
 *	Revision 1.5  2003/08/23 11:44:55  jesper
 *	idiot fixes :)
 *	
 *	Revision 1.4  2003/07/23 08:24:11  jesper
 *	*** empty log message ***
 *	
 *	Revision 1.3  2003/06/22 11:57:27  jesper
 *	fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *	
 *	Revision 1.2  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

/* MC680x0 Registers */

enum
{
	REG_ERR = -1,

	REG_D0 = 0,
	REG_D1,
	REG_D2,
	REG_D3,
	REG_D4,
	REG_D5,
	REG_D6,
	REG_D7,

	REG_A0 = 8,
	REG_A1,
	REG_A2,
	REG_A3,
	REG_A4,
	REG_A5,
	REG_A6,
	REG_A7,

	REG_FP0 = 16,
	REG_FP1,
	REG_FP2,
	REG_FP3,
	REG_FP4,
	REG_FP5,
	REG_FP6,
	REG_FP7,

	REG_SR = 24,
	REG_CCR,
	REG_PC,
	REG_ZPC,

};

enum
{
	RSCALE_1	= 0,
	RSCALE_2	= 1,
	RSCALE_4	= 2,
	RSCALE_8	= 3,

	RSIZE_DEFAULT	= -1,
	RSIZE_W		= 0,
	RSIZE_L		= 1,
};

#define REG_ISDATA(x) (((x)->idx >= REG_D0)&&((x)->idx <= REG_D7))
#define REG_ISADDR(x) (((x)->idx >= REG_A0)&&((x)->idx <= REG_A7))
#define REG_ISPC(x) (((x)->idx == REG_PC)||((x)->idx == REG_ZPC))
#define REG_ISPLAIN(x) (((x)->scale == RSCALE_1)&& (((x)->size == RSIZE_L)||((x)->size == RSIZE_DEFAULT)))
#define REG_REMAP(x,y) ((x)->size = ((x)->size == RSIZE_DEFAULT) ? (y) : (x)->size)


typedef struct asmreg_t
{
	int16_t	idx;			// index of register
	uint8_t		scale;		// scaling-factor, if any
	int8_t		size;		// size of register
} asmreg_t;

enum
{
	VSIZE_Z = 1,		// null
	VSIZE_W = 2,		// word
	VSIZE_L = 3		// longword
};

struct symbol_t;
typedef struct value_t
{
	uint32_t uint;
	int16_t size;
	struct symbol_t* symbol;	// if NULL, value is a valid integer
} value_t;

extern char* getstring( const char* start, const char* end );
extern int getreg( const char* start, const char* end, asmreg_t* reg );
extern int getval( const char* start, const char* end, value_t* val, int allow_symbol );

enum
{
	BF_REGISTER = 32
};

/* MC680x0 Addressing Modes */

enum
{
	AM_ERROR					= 0x00000000,

	AM_REGISTER_DIRECT_DATA				= 0x00000001,	// Dn				OK
	AM_REGISTER_DIRECT_ADDRESS			= 0x00000002,	// An				OK
	AM_REGISTER_INDIRECT_ADDRESS			= 0x00000004,	// (An)				OK
	AM_REGISTER_INDIRECT_ADDRESS_POSTINC		= 0x00000008,	// (An)+			OK
	AM_REGISTER_INDIRECT_ADDRESS_PREDEC		= 0x00000010,	// -(An)			OK
	AM_REGISTER_INDIRECT_ADDRESS_DISPLACE		= 0x00000020,	// (d16,An)			OK
	AM_ADDRESS_INDIRECT_INDEX_8BIT_DISPLACE		= 0x00000040,	// (d8,An,Xn)			OK
	AM_ADDRESS_INDIRECT_INDEX_BASE_DISPLACE		= 0x00000080,	// (bd,An,Xn)			OK
	AM_MEMORY_INDIRECT_POSTINDEX			= 0x00000100,	// ([bd,An],Xn,od)
	AM_MEMORY_INDIRECT_PREINDEX			= 0x00000200,	// ([bd,An,Xn],od)
	AM_PC_INDIRECT_DISPLACE				= 0x00000400,	// (d16,PC)			OK
	AM_PC_INDIRECT_INDEX_8BIT_DISPLACE		= 0x00000800,	// (d8,PC,Xn)			OK
	AM_PC_INDIRECT_INDEX_BASE_DISPLACE		= 0x00001000,	// (bd,PC,Xn)			OK
	AM_PC_MEMORY_INDIRECT_POSTINDEX			= 0x00002000,	// ([bd,PC],Xn,od)
	AM_PC_MEMORY_INDIRECT_PREINDEX			= 0x00004000,	// ([bd,PC,Xn],od)
	AM_ABSOLUTE_SHORT				= 0x00008000,	// (xxx).W			OK
	AM_ABSOLUTE_LONG				= 0x00010000,	// (xxx).L			OK
	AM_IMMEDIATE					= 0x00020000,	// #<xxx>			OK
	// custom modes, added for simplification
	AM_CCR						= 0x00040000,	// ccr				OK
	AM_SR						= 0x00080000,	// sr				OK
	AM_STRING					= 0x00100000,	// "abc"			OK
};


typedef struct operand_t
{
	struct operand_t* next;

	/* addressing-mode */

	uint32_t mode;

	/* address, index */

	asmreg_t areg;
	asmreg_t reg;

	/* immediate, displace, absolute */

	value_t value;

	/* bitfield */

	value_t offset;	/* -1 = unspecified, 0-31 = bit-offset, 32-40 = data-register */
	value_t width;	/* -1 = unspecified, 0-31 = bit-offset, 32-40 = data-register */

	/* string */

	char* string;
} operand_t;

enum
{
	EA_LABS,
	EA_LREL,
	EA_WABS,
	EA_WREL,
};

struct symbol_t;
typedef struct eaddr_t
{
	uint8_t reg;
	uint8_t mode;

	uint16_t num_ew;
	uint16_t ew[6];
	struct symbol_t* syms[6];	// symbol tag for this word
	uint16_t type[6];	// mode to use for symbol
} eaddr_t;

struct mangle_t;
extern int get_ea( struct mangle_t* mangle, operand_t* arg, eaddr_t* ea );

#define CALCEA(i,x) (get_ea(mangle,mangle->args[(i)],(x)))

#endif
