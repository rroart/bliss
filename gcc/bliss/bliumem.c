/* 
 
   BLISS Compiler utility memory management routines (bliumem.c)

   ------------------------------------------------------------------------------

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
   
   ------------------------------------------------------------------------------.  */

/* In thie file GC = garbage collection.  */


#include "blir_temp_config.h"
#include <stdlib.h>
#include "config.h"
#include "ansidecl.h"
#include "system.h" 
#include "ggc.h"
#include "bliumem.h"
#include "bliumsc.h"
#include "blidebug.h"

/* Flag for garbage collection on.  */

extern native_int ggc_p;

/* Note: all storage is set to 0 when allocated.  This prevents lots of
   bugs and has no discernable effect on performance according to
   gperf.  */


#define EYE "DYN\0"
#define EYE_SIZE 4

  /* Union to make allowance for biggest alignment.  */
typedef union {
  HOST_WIDEST_INT i;
#ifdef HAVE_LONG_DOUBLE
  long double d;
#else
  double d;
#endif
} biggest_atom;

/* Handle out of memory condition.  */

static void outofmemory (void);

static void
outofmemory (void)
{
  bliumsc_fatal ("\nOut of Memory\n");
}

/* Allocate a new dynarray with ENTRIES entries each of size
   ENTRY_SIZE.  */

static dynarray *
bliumem_dynarray_init (size_t entries, size_t entry_size)
{
  size_t extra_size;
  dynarray_plus *dynarray_address;
  size_t atom_size;

  BLI_ASSERT (entries > 0);

  atom_size = sizeof (biggest_atom);

  dynarray_address = BLI_ALLOC (sizeof (dynarray_plus));

  extra_size = (entries * entry_size +  atom_size - 1) / atom_size * atom_size;
  
  memcpy (dynarray_address->dynarray_details.eyec, EYE, EYE_SIZE);
  dynarray_address->dynarray_details.entry_count = entries;
  dynarray_address->dynarray_details.entry_size = entry_size;

  dynarray_address->storage = BLI_ALLOC (extra_size);

  return &dynarray_address->dynarray_details;
}

/* Resize DYNARRAY, if required, to ensure it contains entry INDEX.
   Retirn address of replacement.  */

static void
bliumem_dynarray_upsize (dynarray *dynarray1, uint32 entry)
{
  uint32 new_entry_count;
  void *old_stg;
  
  BLI_ASSERT (dynarray1);

  BLI_ASSERT (!memcmp(dynarray1->eyec, EYE, EYE_SIZE));

  if (entry < dynarray1->entry_count)
    return;
  
  new_entry_count = (entry + 1) * 2;
  if (new_entry_count < (dynarray1->entry_count * 2 + 1))
    new_entry_count = dynarray1->entry_count * 2 + 1;
  
  old_stg = ((dynarray_plus*)dynarray1)->storage;
  ((dynarray_plus*)dynarray1)->storage = 
    BLI_ALLOC (new_entry_count * dynarray1->entry_size);
  memcpy (((dynarray_plus*)dynarray1)->storage,
          old_stg, dynarray1->entry_count * dynarray1->entry_size);
  dynarray1->entry_count = new_entry_count;
}

/* Free DYNARRAY, allocated using dynarray routines.  */

static void
bliumem_dynarray_free (dynarray *dynarray1)
{
  dynarray_plus *d_plus;
  d_plus = (dynarray_plus *) dynarray1;

  BLI_ASSERT(!memcmp(dynarray1->eyec, EYE, EYE_SIZE));
  
  memset (d_plus->storage, 0xeb, dynarray1->entry_size * dynarray1->entry_count);
  memset (dynarray1, 0xea, sizeof (dynarray_plus));
}

/* Allocate SIZE storage.  Return the address.  */

static void *
bliumem_alloc (size_t size)
{
  void *address;
  
  if (ggc_p)
    address = ggc_alloc (size);
  else 
    address = really_call_malloc (size);
  
  if (address)
    {
      memset (address, 0, size);
      return address;
    }
  outofmemory ();
  /* Never return.  */
  return address;  /* Keep -Wall quiet.  */
}

