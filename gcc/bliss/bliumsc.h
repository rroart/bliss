/* 
 
   BLISS Compiler utility miscellaneous header (bliumsc.h)

   --------------------------------------------------------------------

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
   
   --------------------------------------------------------------------.  */

#define SHOULD_BE_ENOUGH_FOR_INT 400  /* int to string plenty big enough size for 1024 bits */

void bliumsc_set_progname (uchar *progname);

uchar *bliumsc_get_progname (void);

void *read_file (uchar *filename, unsigned *flength /* Returned.  */);

void bliumsc_pfatal_with_name (uchar *name);

void bliumsc_fatal (const char *msg, ...);

void bliumsc_dynarray_always_used (void *m);

void bliumsc_storage_always_used (void *m);

void bliumsc_storage_always_used_maybe (void *m);

uint32 bliumsc_hash_string (uchar *name, uint32 size);

extern int ggc_p;

