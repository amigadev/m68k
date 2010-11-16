#ifndef __ENDIAN_H__
#define __ENDIAN_H__

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

#ifdef WIN32
#define __BYTE_ORDER 1234
#define __LITTLE_ENDIAN 1234
#else
#include <endian.h>
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be16(x) ((((x)>>8)&0x00ff)|(((x)<<8)&0xff00))
#define be32(x) ((((x)>>24)&0xff)|(((x)>>8)&0xff00)|(((x)<<8)&0xff0000)|(((x)<<24)&0xff000000))
#else
#define be16(x) (x)
#define be32(x) (x)
#endif

#define hi16(x) (((x)>>8)&0xff)
#define lo16(x) ((x)&0xff)
#define hi32(x) (((x)>>16)&0xffff)
#define lo32(x) ((x)&0xffff)

#endif
