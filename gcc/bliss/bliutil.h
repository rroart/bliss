/*  

   BLISS Compiler utility routines header (bliutil.h)

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999,2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Converted to ascii to ebcdic converter by Tim Josling
   
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
   what you give them.   Help stamp out software-hoarding!  */
#include <stdlib.h>

#include "ansidecl.h"
#include "bli_version.h"

#if 0
extern unsigned count_severities[];
extern unsigned display_severities[];
extern unsigned fail_severities[];
extern unsigned display_messages[];

#define SEV_Fatal 0
#define SEV_Error 1
#define SEV_Warning 2
#define SEV_FYI 3
#define SEV_Efficiency 4
#define SEV_Obsolete 5
#define SEV_Unsupported 6
#define SEV_NonStandard 7
#define SEV_MAX 7

#define MSG_MAX 50 /* max message number of error etc messages used to size arrays */
#endif

extern void *
read_file (uchar *filename, unsigned *flength /* Returned.  */);

extern void
my_fatal (uchar *msg, ...);

extern void
nonfatal (uchar *msg, ...);

extern void
perror_with_name (uchar *name);

extern void
my_pfatal_with_name (uchar *name);

extern void
tj_malloc (void **buffer, size_t size);

extern void
tj_realloc (void **oldbuffer, size_t * oldsize, size_t newsize);

extern void
tj_realloc_if_needed (void **oldbuffer, size_t * oldsize, size_t newsize);

extern uchar *
tj_strdup (const uchar *s);

extern void
tj_free (void **buffer);

extern schar *
my_strerror (unsigned errnum);

extern void
set_progname (uchar *progname);

extern uchar *
get_progname (void);

void
tj_memok (void);
