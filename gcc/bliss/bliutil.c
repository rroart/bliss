/* 
 
   BLISS Compiler utility routines (bliutil)

   ----------------------------------------------------------------------

   *** This is only used in the stand alone utilities not in the main
   *** compiler or preprocessor ***

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
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
   what you give them.   Help stamp out software-hoarding!  
   
   ----------------------------------------------------------------------.  */

#include "blidebug.h"
#include "blir_temp_config.h"

#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

#include "ansidecl.h"

#include "bliutil.h"

/* Memory housekeeping.  */

#define MAGIC1 505707909
#define MAGIC2 606808010

struct malloc_header
{
  uint32 magic;
  struct malloc_header *prev;
  struct malloc_header *next;
  uint32 size;  /* Of data only, excluding header and footer.  */
};

struct malloc_trailer
{
  uint32 magic;
};

static struct malloc_header *first_malloc = NULL;
static struct malloc_header *last_malloc = NULL;

static void
memory_full (void);

#if 0
uint32 count_severities[SEV_MAX +1] = {0};
uint32 display_severities[SEV_MAX +1] = {0};
uint32 display_messages[MSG_MAX +1] = {0};
uint32 fail_severities[SEV_MAX +1] = {0};
#endif

#ifndef NDEBUG
static uint32 malloc_count = 0;
static uint32 malloc_total = 0;
static uint32 free_count = 0;
static uint32 free_total = 0;
#endif

/* Output nonfatal error message MSG using printf functionality.  */

void
nonfatal (uchar *msg, ...)
{

  va_list args;

  va_start (args, msg);
  vfprintf (stderr, (char *)msg, args);
  va_end (args);
  fprintf (stderr, "\n");
}

/* Output fatal error message MSG using printf functionality and
   terminate with error.  */

void
my_fatal (uchar *msg, ...)
{

  va_list args;

  fprintf (stderr, "%s: ", get_progname ());
  va_start (args, msg);
  vfprintf (stderr, (char *)msg, args);
  va_end (args);
  fprintf (stderr, "\n");
  exit (EXIT_FAILURE);
}

/* My_strerror - return the descriptive text associated with an
   `errno' code ERRNUM.  */

schar *
my_strerror (uint32 errnum)
{
  schar *result;
  result = (schar *)strerror (errnum);
  if (result)return result;
  return (schar *)"undocumented error";
}

/* Check tj_malloc integrity using BLI_ASSERTs to crash if a problem found.  */

void
tj_memok (void)
{
  struct malloc_header *mem;
  struct malloc_trailer *memend;
  uint32 counter = 0;
  uint32 totaller = 0;

  for (mem = first_malloc; mem; mem = mem->next)
{

    counter ++;
    totaller += mem->size;

    BLI_ASSERT (mem->size > 0);
    BLI_ASSERT (mem->magic == MAGIC1);

    if (mem == first_malloc)
      {
        BLI_ASSERT (!(mem->prev));
      }
    else
      {
        BLI_ASSERT (mem->prev);
        BLI_ASSERT (mem->prev->next == mem);
      }
    if (mem == last_malloc)
      {
        BLI_ASSERT (!(mem->next));
      }
    else
      {
        BLI_ASSERT (mem->next);
        BLI_ASSERT (mem->next->prev == mem);
      }
    
    memend = (struct malloc_trailer *)(((char *)mem) + (mem->size) + sizeof (struct malloc_header));
    BLI_ASSERT (memend->magic == MAGIC2);
    
  }

  BLI_ASSERT (counter == (malloc_count - free_count));
  BLI_ASSERT (totaller == (malloc_total - free_total));
  
}

/* Allocate cleared storage, save address in *BUFFER.  Size is SIZE.  */

extern void
tj_malloc (void **buffer, size_t size)
{
  uint32 worksize;
  struct malloc_header *mem;
  struct malloc_trailer *memend;

  worksize = size;

#ifndef NDEBUG
  worksize += sizeof (struct malloc_header)
    + sizeof (struct malloc_trailer);
#endif  

  BLI_ASSERT (worksize > 0);
  BLI_ASSERT (buffer);

  * buffer = malloc (worksize);
  if ( * buffer)
    {
      memset ( * buffer, 0, worksize);
#ifndef NDEBUG
      malloc_count ++;
      malloc_total += size;
      mem = * buffer;
      if (first_malloc)
        {
          BLI_ASSERT (first_malloc->magic == MAGIC1);
          first_malloc->prev = mem;
          mem->next = first_malloc;
          first_malloc = mem;
        }
      else
        {
          first_malloc = * buffer;
          last_malloc = * buffer;
        }
      mem->magic = MAGIC1;
      mem->size = size;
      memend = (struct malloc_trailer *)((char *)mem + size + sizeof (struct malloc_header));
      memend->magic = MAGIC2;
      * buffer = ((char *)*buffer) + sizeof (struct malloc_header);
#endif  
      return;
    }
  memory_full ();
  /* Never return.  */
  return;  /* Keep -Wall quiet.  */
}


/* Free storage *BUFFER allocated by TJ_MALLOC.  */

void
tj_free (void **buffer)
{
  struct malloc_header *mem;
  struct malloc_trailer *memend;
  void *tofree;
  
  tofree = * buffer;
  
  BLI_ASSERT (buffer);
  BLI_ASSERT ( * buffer);
  
#ifndef NDEBUG  
  mem = (struct malloc_header *)((char *)*buffer - sizeof (struct malloc_header));
  BLI_ASSERT (mem->magic == MAGIC1);
  BLI_ASSERT (mem->size > 0);
  memend = (struct malloc_trailer *)((char *)*buffer + mem->size);
  BLI_ASSERT (memend->magic == MAGIC2);
  tofree = mem;
  if (mem->prev)
    {
      mem->prev->next = mem->next;
    }
  else
    {
      first_malloc = mem->next;
    }
  if (mem->next)
    {
      mem->next->prev = mem->prev;
    }
  else
    {
      last_malloc = mem->prev;
    }
  mem->magic = 0x11111111;
  BLI_ASSERT (mem->size);
  free_total += mem->size;
  mem->size = 0;
  memend->magic = 0x22222222;
#endif
  free (tofree);
  * buffer = NULL;
#ifndef NDEBUG  
  free_count ++;
#endif
  * buffer = (void *)0xffffffff;  /* Make ptr unusable.  */
}

/* Realloc storage in *BUFFER and update 'OLDSIZE' with new size NEWSIZE.  */

void
tj_realloc (void **buffer, size_t * oldsize, size_t newsize)
{

  size_t copysize;
  void *save_buffer;

  BLI_ASSERT (oldsize);
  BLI_ASSERT ( * oldsize > 0);
  BLI_ASSERT (newsize > 0);
  BLI_ASSERT ( * buffer);

  save_buffer = * buffer;
  copysize = * oldsize;
  if (newsize < copysize)copysize = newsize;
  tj_malloc (buffer, newsize);
  memcpy ( * buffer, save_buffer, copysize);
  tj_free ( & save_buffer);
  * oldsize = newsize;
  return;
}

/* Realloc storage *OLDBUFFER if size needed (NEWSIZE) > current size (OLDSIZE).  */

void
tj_realloc_if_needed (void **oldbuffer, size_t * oldsize, size_t newsize)
{
  if ( * oldsize >= newsize)return;
  tj_realloc (oldbuffer, oldsize, newsize * 2 +20);
  return;
}

/* Return dup copy of string S.  */

extern uchar *
tj_strdup (const uchar *s)
{
  uchar *new_string;
  uint32 string_length;
  string_length = strlen ((char *)s);
  tj_malloc ((void **) & new_string, string_length +1);
  memcpy (new_string, s, string_length);
  new_string[string_length] = 0;
  return new_string;
}

/* Error message and crash due to out of memory.  */

static void
memory_full (void)
{
  my_fatal ((uchar *)"Memory exhausted.");
}

void
perror_with_name (uchar *name)
{
  fprintf (stderr, "toy: ");
  fprintf (stderr, ":%s: %s\n", name, strerror (errno));
}

/* Fatal error: print last libc error message plus NAM, and terminate
   with errorE.  */

void
my_pfatal_with_name (uchar *name)
{
  perror_with_name (name);
  exit (EXIT_FAILURE);
}

/* Read a whole file FILENAME and return buffer address, and fill in
   length into *FLENGTH.  */

void *
read_file (uchar *filename, uint32 *flength /* Returned.  */)
{
  
  void *fbuffer = NULL;
  struct stat st;
  uint32 nchars = 0;  /* Chars in file.  */
  int32 read_result;
  uint32 bufsize;  /* Stdin buffer size.  */
  int32 f;
  
  if (strcmp ((char *)filename, ""))
    {
      
      /* BUG: assumption it is a real file or stdin see cccp safe_read in gcc.  */
      if (stat ((char *)filename,  & st) != 0)
        {
          my_pfatal_with_name (filename);
        }
    
      if ((f = open ((char *)filename, O_RDONLY) < 0))
        {
          goto perror;
        }
      
      * flength = st.st_size;
      if (! * flength)
        return NULL;
      
      tj_malloc ( & fbuffer,  * flength +2);
      
      nchars = read (f, fbuffer,  * flength);
      if (nchars < * flength)
        {
          my_fatal ((uchar *)"Read failed %d %d %d %s ", * flength, nchars, errno,
                    strerror (errno));
        }
      close (f);
      return fbuffer;
    }
  
  /* Stdin - can't get length.  */
  
  bufsize = 1000000;
  tj_malloc ( & fbuffer, bufsize);
  
  nchars = 0;
  while (1)
    {
      uint32 sizetoread;
      sizetoread = bufsize - nchars;
      if (sizetoread > SSIZE_MAX)
        {
          sizetoread = SSIZE_MAX;
        }
    read_result = read (0, (char *)fbuffer + nchars, sizetoread);
    if (read_result == 0)
      break;
    if (read_result < 0)
      {
        if (read_result == EOF)
          {
            break;
          }
        my_fatal ((uchar *)"Read failed %d %d %d %s",
              nchars, read_result, errno, strerror (errno));
      }
    nchars += read_result;
    if (nchars == bufsize)
      { /* Out of space.  */
        tj_realloc ( & fbuffer,  & bufsize, bufsize * 4);
      }
    }
  
  if (!nchars)
    {
      /* Empty file - return 0/NULL.  */
      tj_free ( & fbuffer);
      * flength = 0;
      return NULL;
    }
  
  /* Trim it to used size  + 1 for pre1.  */
  tj_realloc ( & fbuffer,  & bufsize, nchars +1);
  
  * flength = nchars;
  return fbuffer;
  
 perror :
  my_pfatal_with_name (filename);
  return NULL;
  
}

/* Record name of the program PROGNAME for later error messages.  */

static uchar *save_progname = NULL;

extern void
set_progname (uchar *progname)
{
  BLI_ASSERT (!save_progname);
  save_progname = progname;
}

extern uchar *
get_progname (void)
{
  BLI_ASSERT (save_progname);
  return save_progname;
}

/* Abort but provide call trace.  */

void
myabort (const char * const msg, const char * const file, int line)
{
  fprintf (stderr, "abort line %d file %s: %s\n", line, file, msg);
  abort ();
}

