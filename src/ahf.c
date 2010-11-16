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

#include "types.h"
#include "codespace.h"
#include "endian.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

/*
 *      $Id: ahf.c,v 1.4 2003/08/23 11:50:46 jesper Exp $
 *      $Log: ahf.c,v $
 *      Revision 1.4  2003/08/23 11:50:46  jesper
 *      fixed C compatibility
 *
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

// OK, this one writes executables instead of objects right now, but it's just for testing

typedef enum
{
	HUNK_UNIT	= 0x3e7,
	HUNK_NAME	= 0x3e8,
	HUNK_CODE	= 0x3e9,
	HUNK_DATA	= 0x3ea,
	HUNK_BSS	= 0x3eb,
	HUNK_RELOC32	= 0x3ec,
	HUNK_RELOC16	= 0x3ed,
	HUNK_RELOC8	= 0x3ee,
	HUNK_EXT	= 0x3ef,
	HUNK_SYMBOL	= 0x3f0,
	HUNK_DEBUG	= 0x3f1,
	HUNK_END	= 0x3f2,
	HUNK_HEADER	= 0x3f3,

	HUNK_OVERLAY	= 0x3f5,
	HUNK_BREAK	= 0x3f6,
	HUNK_DREL32	= 0x3f7,
	HUNK_DREL16	= 0x3f8,
	HUNK_DREL8	= 0x3f9,
	HUNK_LIB	= 0x3fa,
	HUNK_INDEX	= 0x3fb,
	HUNK_REL32SHORT	= 0x3fc,
	HUNK_RELREL32	= 0x3fd,
	HUNK_ABSREL16	= 0x3fe,

	HUNK_ADVISORY	= 0x20000000,
	HUNK_CHIP	= 0x40000000,
	HUNK_FAST	= 0x80000000
} ahf_hunk_id;

typedef struct ahf_handle
{
	FILE* fp;

	uint32_t num_sections;
	cs_section_t** sections;	
} ahf_handle;

// open new AHF (Amiga Hunk Format) file
ahf_handle* ahf_open( const char* filename )
{
	FILE* fp = fopen(filename,"wb");

	if(fp)
	{
		ahf_handle* handle = calloc(1,sizeof(ahf_handle));
		handle->fp = fp;
		return handle;
	}
	return NULL;
}

// close handle (this flushes object to disk)
void ahf_close( ahf_handle* handle )
{
	uint32_t i;
	uint32_t t;


	// write header

	t = be32(HUNK_HEADER);
	fwrite(&t,1,4,handle->fp);	// header
	t = 0;
	fwrite(&t,1,4,handle->fp);	// names not exported

	t = be32(handle->num_sections);
	fwrite(&t,1,4,handle->fp);	// highest hunk number + 1
	t = 0;
	fwrite(&t,1,4,handle->fp);
	t = be32(handle->num_sections-1);
	fwrite(&t,1,4,handle->fp);

	for(i=0;i<handle->num_sections;i++)
	{
		t = be32((handle->sections[i]->size+3)>>2);
		fwrite(&t,1,4,handle->fp);
	}

	// write sections

	for( i = 0; i < handle->num_sections; i++)
	{
		ahf_hunk_id id = 0;

		if(handle->sections[i]->flags & ST_CODE)
			id = HUNK_CODE;
		else if(handle->sections[i]->flags & ST_DATA)
			id = HUNK_DATA;
		else if(handle->sections[i]->flags & ST_BSS)
			id = HUNK_BSS;
		else fail(EARCH_UNSUPPORTED_SECTION_TYPE);

		if(handle->sections[i]->flags & ST_CHIP)
			id |= HUNK_CHIP;
		if(handle->sections[i]->flags & ST_FAST)
			id |= HUNK_FAST;

		t = be32(id);
		fwrite(&t,1,4,handle->fp);
		t = be32((handle->sections[i]->size+3)>>2);
		fwrite(&t,1,4,handle->fp);
		
		if(!(handle->sections[i]->flags & ST_BSS))
		{
			int32_t j;

			fwrite(handle->sections[i]->data,1,handle->sections[i]->size,handle->fp);

			// align
			j = ((handle->sections[i]->size+3)&~3)-handle->sections[i]->size;
			while(j-->0)
				fputc(j&1?0x4e:0x71,handle->fp);

			if(handle->sections[i]->freloc)
			{
				// sort reloc-table according to section (bubble-sort, weeeehaaa!)

				int sorted = 0;
				cs_reloc_t* crel = NULL;

				t = be32(HUNK_RELOC32);
				fwrite(&t,1,4,handle->fp);

				while(!sorted)
				{
					cs_reloc_t* curr = handle->sections[i]->freloc;
					cs_reloc_t* last = NULL;
					sorted = 1;

					while(curr->next)
					{
						cs_reloc_t* next = curr->next;

						if(curr->target > next->target)
						{
							if(last)
								last->next = next;
							else
								handle->sections[i]->freloc = next;

							curr->next = next->next;
							next->next = curr;
							curr = next;
							sorted = 0;
						}

						last = curr;
						curr = curr->next;
					}
				}

				// write output

				crel = handle->sections[i]->freloc;

				while(crel)
				{
					int id = -1,count = 0,i;
					cs_reloc_t* curr = crel;

					// count entries in span

					while(curr && (curr->target == crel->target))
					{
						count++;
						curr = curr->next;
					}

					// find id

					for(i=0;i<handle->num_sections;i++)
					{
						if(handle->sections[i] == crel->target)
						{
							id = i;
							break;	
						}
					}

					// should not happen, but just in case...	
					if(-1 == id)
						fail(ECODE_UNRESOLVED_SECTION);

					t = be32(count);
					fwrite(&t,1,4,handle->fp);
					t = be32(id);
					fwrite(&t,1,4,handle->fp);
					for(i=0;i<count;i++)
					{
						t = be32(crel->offset);
						fwrite(&t,1,4,handle->fp);
						crel = crel->next;
					}
				}
				t = 0;
				fwrite(&t,1,4,handle->fp);

			}
		}

		t = be32(HUNK_END);
		fwrite(&t,1,4,handle->fp);
	}

	fclose(handle->fp);
}

// register section for output
void ahf_register( ahf_handle* handle, cs_section_t* section )
{
	handle->sections = (cs_section_t**)realloc(handle->sections,(handle->num_sections+1)*sizeof(cs_section_t*));
	handle->sections[handle->num_sections++] = section;
}
