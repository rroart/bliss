/* 

   BLISS Compiler moveall routine tester

   Copyright (C) 1999, 2000, 2001 Tim Josling tej@melbc.org.au
   
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
#include "blidebug.h"
#include "blir_moveall.h"
#include <stdio.h>
#include <memory.h>

  /* Test move all
     
     Parameters are: 

     f  from data
     fl from length
     tl to length
     c  check value should be.  */
  
#define T1(f,fl,tl,c)\
{\
  uchar cval[tl+1];\
  uchar tval[tl+1];\
  uchar fval[fl+1];\
  memset(cval,0,tl+1);\
  memset(tval,0,tl+1);\
  memset(fval,0,tl+1);\
  memcpy(fval,f,fl);\
  memcpy(cval,c,tl);\
  blir_moveall(tval,tl,fval,fl);\
  e1=(uchar*)" OK ";\
  if (memcmp(cval, tval, tl))\
     {\
        error++;\
        e1=(uchar*)" ***>>> ERROR NOT ";\
     }\
 \
  printf(f " " #fl " " #tl " " c ": %s " \
         "%s\n",\
         e1, cval);\
}

int
main (void)
{
  uint32 error = 0;
  uchar *e1;

  printf ("Test move all\n");

  T1 ("0", 1, 3, "000");
  T1 ("0", 1, 1, "0");
  T1 ("000", 3, 1, "0");
  T1 ("abc", 3, 9, "abcabcabc");
  T1 ("abc", 3, 10, "abcabcabca");
  T1 ("abc", 3, 8, "abcabcab");
  T1 ("abc", 3, 3, "abc");

  printf ("Error count = %d\n", error);
  fflush (stderr);
  return error; 
}



