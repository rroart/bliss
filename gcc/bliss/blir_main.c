/*
  
  BLISS main - calls the bliss main program.

  Copyright (C) 2001 by Free Software Foundation, Inc. 

  This library is free software; you can redistribute it and/or      
  modify it under the terms of the GNU Lesser General Public License 
  as published by the Free Software Foundation; either version 2.1   
  of the License, or (at your option) any later version.             
  
  This library is distributed in the hope that it will be useful,    
  but WITHOUT ANY WARRANTY; without even the implied warranty of     
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   
  Lesser General Public License for more details.                    
  
  You should have received a copy of the GNU Lesser General Public   
  License along with this library; if not, write to the Free         
  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  
  02111-1307 USA.  */

#include <stdio.h>
#include "blidebug.h"

extern int (*blir_main_address) (void);

/* Ignore ARGC and ARGV.  Call the main BLISS program. If this is
   missing, someone forgot to compile a program with -fmain=NAME.  */

int
main (int argc, char **args)
{
  int dummy;
  dummy = argc + (int)args;
  return (blir_main_address)();
}

/* Abort but provide call trace.  */

void
myabort (const char * const msg, const char * const file, int line)
{
  fprintf (stderr, "abort line %d file %s: %s\n", line, file, msg);
  abort ();
}
