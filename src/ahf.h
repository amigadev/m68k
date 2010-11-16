#ifndef __AHF_H__
#define __AHF_H__

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
 *      $Id: ahf.h,v 1.3 2003/06/23 15:51:38 jesper Exp $
 *      $Log: ahf.h,v $
 *      Revision 1.3  2003/06/23 15:51:38  jesper
 *      *** empty log message ***
 *
 *      Revision 1.2  2003/06/22 22:47:41  jesper
 *      first pass of AHF output complete
 *
 *      Revision 1.1  2003/06/22 11:57:27  jesper
 *      fixed msvc-problems (hopefully), updated relocs, started on 'section'
 *
 *      $EndLog$ */

#ifndef AHF_INTERNAL
typedef void ahf_handle;
#endif

ahf_handle* ahf_open( const char* filename );
void ahf_close( ahf_handle* handle );
void ahf_register( ahf_handle* handle, cs_section_t* section );

#endif
