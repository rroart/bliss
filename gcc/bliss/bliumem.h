/* 
 
   BLISS Compiler utility message handling header (bliumsg.h)

   ------------------------------------------------------------------

   Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  
   
   ------------------------------------------------------------------.  */

/* Allocate size bytes.  */

#define BLI_ALLOC(size)  bliumem_alloc(size);

void *bliumem_alloc (size_t size);

/* Dynamically growable array.  */

#define GTY(a)

struct dynarray_struct GTY(())
{
  native_char eyec[4];
  uint32 entry_size;
  uint32 entry_count;
/* Entries pointed to by following word.  */
};

typedef struct dynarray_struct dynarray;

struct dynarray_plus_struct
{
  dynarray dynarray_details;
  void * storage;
};

typedef struct dynarray_plus_struct dynarray_plus;

struct dynarray_uchar_struct GTY (())
{
  dynarray dynarray_details;
  uchar * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_uchar_struct dynarray_uchar;

struct dynarray_uchar_ptr_struct GTY (())
{
  dynarray dynarray_details;
  uchar ** GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_uchar_ptr_struct dynarray_uchar_ptr;

struct dynarray_uint32_struct GTY (())
{
  dynarray dynarray_details;
  uint32 * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_uint32_struct dynarray_uint32;

/* Allocate a dynamic array ENTRIES entries each of size ENTRY_SIZE.  */

#define ALLOC_DYN(entries, entry_size) bliumem_dynarray_init(entries, entry_size);

dynarray *bliumem_dynarray_init (size_t entries, size_t entry_size);

/* Resize DYNARRAY to ensure it is large enough to contain this INDEX.  */

#define GROW_DYN(dynarray1, index) bliumem_dynarray_upsize (dynarray1, index)

void bliumem_dynarray_upsize (dynarray *dynarray1, uint32 entry);

/* Free dynamic array.  */

#define FREE_DYN(dynarray1)  bliumem_dynarray_free(dynarray1)

void bliumem_dynarray_free (dynarray *dynarray1);
