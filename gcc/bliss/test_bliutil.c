/* 

   BLISS Compiler utility routines tester

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

#ifdef NDEBUG
#undef NDEBUG  /* We need BLI_ASSERTs to work.  */
#endif
#include "blir_temp_config.h"
#include "blidebug.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>

#include "ansidecl.h"
#include "bliutil.h"

void
check_value (uchar *b, int32 size, uchar char1);

/* Verify that s buffer B length SIZE is all character BLI_CHAR1.  */

void
check_value (uchar *b, int32 size, uchar char1)
{
  uchar *end1 = b + size;
  while (b < end1){
    BLI_ASSERT ( * b == char1);
    b ++;
  }
}

/* Test driver main.  */

int32
main (int argc ATTRIBUTE_UNUSED, char **argv)
{
  
  uchar *old_buffer;
  uint32 current_size;
  uchar *test_save;
  
  set_progname ((uchar *)argv[0]);
  
  test_save = tj_strdup ((uchar *)argv[0]);
  BLI_ASSERT (test_save != (uchar *)argv[0]);
  BLI_ASSERT (test_save);
  BLI_ASSERT (strlen ((char *)test_save) == strlen (argv[0]));
  BLI_ASSERT (!strcmp ((char *)test_save, argv[0]));
  
  current_size = 1000;
  tj_malloc ((void *) & old_buffer, current_size);
  tj_memok ();
  check_value (old_buffer, 1000, 0);
  memset (old_buffer, 'c', 1000);
  tj_memok ();
  tj_realloc ((void *) & old_buffer,  & current_size, 2000);
  tj_memok ();
  check_value (old_buffer, 1000, 'c');
  check_value (old_buffer +1000, 1000, 0);
  tj_free ((void *) & old_buffer);
  tj_memok ();
  
  current_size = 100000;
  tj_malloc ((void *) & old_buffer, current_size);
  tj_memok ();
  check_value (old_buffer, 100000, 0);
  memset (old_buffer, 'd', 100000);
  tj_memok ();

  tj_realloc ((void *) & old_buffer,  & current_size, 1000);
  tj_memok ();
  tj_realloc ((void *) & old_buffer,  & current_size, 1001);
  tj_memok ();
  BLI_ASSERT (current_size == 1001);
  BLI_ASSERT (old_buffer[0] == 'd');
  BLI_ASSERT (old_buffer[999] == 'd');
  BLI_ASSERT (old_buffer[1000] == 0);
  tj_free ((void *) & old_buffer);
  tj_memok ();
  
  current_size = 2;
  tj_malloc ((void *) & old_buffer, current_size);
  tj_memok ();
  check_value (old_buffer, 2, 0);
  memset (old_buffer, 'w', 2);
  tj_memok ();

  tj_realloc_if_needed ((void *) & old_buffer,  & current_size, 4);
  tj_memok ();
  BLI_ASSERT (current_size >= 4);
  BLI_ASSERT (old_buffer[0] == 'w');
  BLI_ASSERT (old_buffer[1] == 'w');
  BLI_ASSERT (old_buffer[3] == 0);
  old_buffer[3] = 'x';
  tj_memok ();

  tj_realloc_if_needed ((void *) & old_buffer,  & current_size, 2);
  tj_memok ();
  BLI_ASSERT (current_size >= 4);
  BLI_ASSERT (old_buffer[0] == 'w');
  BLI_ASSERT (old_buffer[1] == 'w');
  BLI_ASSERT (old_buffer[3] == 'x');

  tj_free ((void *) & old_buffer);
  tj_memok ();
  
  fprintf (stdout, "\nUtil test ALL OK for %s\n", get_progname ());

  exit (EXIT_SUCCESS);
}



