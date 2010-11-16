#ifndef __ARGPARSE_H__
#define __ARGPARSE_H__

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
 *	$Id: arguments.h,v 1.3 2003/06/23 15:51:38 jesper Exp $
 *	$Log: arguments.h,v $
 *	Revision 1.3  2003/06/23 15:51:38  jesper
 *	*** empty log message ***
 *	
 *	Revision 1.2  2003/06/18 20:23:19  jesper
 *	added logging to all files
 *	
 *	$EndLog$ */
   

enum
{
	ARG_FILENAMES,		/* array of strings */
	ARG_OPTIMIZE,		/* integer */
	ARG_OUTPUT,		/* string */
};

enum
{
	AT_STRING,
	AT_INT32,
	AT_FLOAT
};

extern int arginit();
extern void argexit();

extern int argparse( int argc, const char** argv );

extern int argcount( unsigned int index );
extern const char* argstring( unsigned int index, unsigned int value );
extern uint32_t argval( unsigned int index, unsigned int value );
extern float argfloat( unsigned int index, unsigned int value );

#endif
