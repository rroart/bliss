/* 

   BLISS Compiler compare helper routines tester

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
#include "blidebug.h"
#include "blir_cmp.h"
#include <stdio.h> 
#include <string.h>


int
main (void)
{
  int32 result = 0;
  uint32 error = 0;
  uchar *e1;

/*

  Parameters are: 
  
  s1 string 
  f1 filler character
  c1 value result should be.  

*/ 

#define T(s1, f1, c1)\
{\
  result = blir_cmpfill((uchar *)s1, strlen ((native_char *)s1), (uint32) f1);\
  e1 = (uchar*)"";\
  if (result != c1)\
    {\
       e1=(uchar*)" ERROR NOT ";\
       error++;\
    }\
  printf("<" #s1 "> <" #f1 "> <" #c1 "> : %s = %d\n", e1, result);\
}

  printf("Fill\n");

  T (" ", 'K', -1);  
  T ("K", 'K', 0);  
  T ("Z", 'K', 1);  
  T ("  ", 'K', -1);  
  T ("KK", 'K', 0);  
  T ("Z ", 'K', 1);  
  T ("K ", 'K', -1);  
  T ("  ", ' ', 0);  
  T ("KZ", 'K', 1);  
  T ("   ", 'K', -1);  
  T ("KKK", 'K', 0);  
  T ("KZ ", 'K', 1);  

/*

  Parameters are: 
  
  s1 string 1
  s2 string 2
  c1 value result should be.  

*/ 

#undef T
#define T(s1, s2, c1)\
{\
  result = blir_cmpall((uchar *)s1, strlen (s1), (native_char *)s2, strlen (s2));\
  e1 = (uchar*)"";\
  if (result != c1)\
    {\
       e1=(uchar*)" ERROR NOT ";\
       error++;\
    }\
  printf("<" #s1 "> <" #s2 "> <" #c1 "> : %s = %d\n", e1, result);\
}

  printf("All\n");

  T (" ", " ", 0);  
  T ("AA", "A", 0);  
  T ("AAA", "AA", 0);  
  T ("B", " ", 1);  
  T ("AB", "A", 1);  
  T ("AAB", "AA", 1);  
  T ("AAB", "AB", -1);  
  T ("AAB", "AAB", 0);  
  T ("AAB", "AABD", 0);  

  printf ("Error count = %d\n", error);
  fflush (stderr);
  return error; 
}
