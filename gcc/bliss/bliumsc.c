/* 
 
   BLISS Compiler utility miscellaneous (bliumsc.c)

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
   what you give them.   Help stamp out software-hoarding!  */

#include "blir_temp_config.h"
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "ansidecl.h"
#include "config.h"
/*#include "ggc.h"*/

#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "blidebug.h"

/* File buffer address - used for GC del_root when no interface required.  */

dynarray_uchar *bliu_fbuffer = NULL;

/* Terminate with failure, outputting the error message MSG first.  */

void
bliumsc_fatal (const char *msg, ...)
{
  va_list args;

  output_msgs ();
  fprintf (stderr, "%s: ", bliumsc_get_progname ());
  va_start (args, msg);
  vfprintf (stderr, (char *)msg, args);
  va_end (args);
  fprintf (stderr, "\n");
  output_summary ();
  exit (EXIT_FAILURE);
}

/* Saved program name.  */

static uchar *save_progname = NULL;

/* Save program name.  */

void
bliumsc_set_progname (progname)
     uchar *progname;
{
  BLI_ASSERT (!save_progname);
  save_progname = progname;
}

/* Obtain program name.  */

uchar *
bliumsc_get_progname ()
{
  BLI_ASSERT (save_progname);
  return save_progname;
}


/* Read a file called FILENAME, return buffer, and also place length
   in *FLENGTH.  */

void *
read_file (filename, flength /* Returned.  */)
     uchar *filename;
     uint32 *flength /* Returned.  */;
{
  struct stat st;
  int32 nchars = 0;  /* Chars in file.  */
  int32 read_result;
  int32 bufsize;  /* Stdin buffer size.  */
  int32 f;

  int32 current_size = 0;
  
  if (strcmp ((char *)filename, ""))
    {
      
      /* BUG: assumption it is a real file or stdin see cccp safe_read in gcc.  */
      if (stat ((char *)filename,  & st) != 0)
        {
          bliumsc_pfatal_with_name (filename);
        }
    
    if ((f = open ((char *)filename, O_RDONLY)) < 0)
      {
        goto perror;
      }
    
    * flength = st.st_size;
    if (! * flength)
      return NULL;

    current_size = (*flength +2);
    bliu_fbuffer = (dynarray_uchar *) ALLOC_DYN (current_size, sizeof(uchar));
    
    nchars = read (f, bliu_fbuffer->elem, *flength);
    if ((uint32)nchars < *flength)
      {
        bliumsc_fatal ("Read failed %d %d %d %s ",
                       *flength, nchars, errno,
              strerror (errno));
      }
    close (f);
    return bliu_fbuffer->elem;
    }
  
  /* Stdin - can't get length.  */
  
  bufsize = 1000000;
  bliu_fbuffer = (dynarray_uchar *) ALLOC_DYN (bufsize, sizeof(uchar))  
  nchars = 0;
  while (1)
    {
      int32 sizetoread;
      sizetoread = bufsize - nchars;
      if (sizetoread > SSIZE_MAX)
        {
          sizetoread = SSIZE_MAX;
        }
    read_result = read (0, (char *)bliu_fbuffer + nchars, sizetoread);
    if (read_result == 0)
      break;
    if (read_result < 0)
      {
        if (read_result == EOF)
          {
            break;
          }
        bliumsc_fatal ("Read failed %d %d %d %s",
              nchars, read_result, errno, strerror (errno));
      }
    nchars += read_result;
    if (nchars == bufsize)
      {
        bufsize *= 4;
        GROW_DYN (&bliu_fbuffer->dynarray_details, bufsize);
      }
    }
  
  if (!nchars)
    {
      /* Empty file - return 0/NULL.  */
      FREE_DYN (&bliu_fbuffer->dynarray_details);
      bliu_fbuffer = NULL;
    }
  
  * flength = nchars;
  return bliu_fbuffer;
  
 perror :
  bliumsc_pfatal_with_name (filename);
  return NULL;
  
}

/* Print program name, another NAME, and last error message then fail.  */

void
bliumsc_pfatal_with_name (name)
     uchar *name;
{
  fprintf (stderr, "%s: ", bliumsc_get_progname ());
  fprintf (stderr, ":%s: %s\n", name, strerror (errno));
  exit (EXIT_FAILURE);
}

/* 
   Cobumsc_hash_string - temp hash routine.  string BLI_CHAR length SIZE
   gets hashed to an uint32 and hash value is returned.  At least
   it's in one place and can get fixed/replaced later on.  */

uint32
bliumsc_hash_string (uchar *name, uint32 size)
{

  uint32 hashval = 0, char_index;

  for (char_index = 0; char_index < size; char_index ++)
    hashval = (hashval << 1) ^ (hashval >> 3) ^ name[char_index];

  return hashval;
}

/* Abort but provide call trace.  */

void
myabort (const char * const msg, const char * const file, int line)
{
  fprintf (stderr, "abort line %d file %s: %s\n", line, file, msg);
  abort ();
}
