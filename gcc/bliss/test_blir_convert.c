/* 

   BLISS Compiler conversion routines tester

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
#include "blir_convert.h"
#include <stdio.h>
#include <memory.h>

  /* Test alphanumeric to number conversion
     
     Parameters are: 

     a1 data
     z1 size data
     
     U2 sign of result
     n2 y/n to indicate signed result
     z2 size in bits of result
     u2 u or d for [un]signed result
     
     c  what result should be
     d  # digits allowed in result.  */
  
#define T1(a1,z1,U2,n2,z2,u2,c,d)\
{\
  U2##int64 result;\
  result=blir_convert_X_##z2##_##n2((uchar *)a1,z1,d);\
  e1=(uchar *)"";\
  if (result!=c)\
    {\
       e1=(uchar *)" ***>>> ERROR NOT ";\
       error++;\
    }\
  printf(#a1 " " #z1 " " #U2 " " #n2  " " #z2 " " #u2 " " #c ": "\
         "%s"\
         " %s =%" BLI_64_FORMAT_PREFIX #u2\
         " looking for %" BLI_64_FORMAT_PREFIX #u2"\n",\
         a1,\
         e1,\
         result,\
        (U2##int64)c);\
}


  /* Test number to alphanumeric conversion
     
     Parameters are: 

     a1 data (expected output)
     z1 size data (expected output)
     
     z2 size in bits of input
     n2 ny whether input signed
     d  number of digits to use
     
     v  input value.  */
  
#define T2(a1,z1,z2,n2,d,v)\
{\
  memset(workout,0,100);\
  blir_convert_##z2##_##n2##_X(v,d,workout,z1);\
  e1=(uchar *)"";\
  if (memcmp(a1, workout, z1))\
    {\
       e1=(uchar *)" ***>>> ERROR NOT ";\
       error++;\
    }\
  printf(#a1" " #z1 " " #z2 " " #d " " #v ": "\
         "%s"\
         "%s =expected "\
         "%s\n",\
         workout,\
         e1,\
         a1);\
}


int
main (void)
{
  uint32 error = 0;
  uchar *e1;
  uchar workout[100];

  printf ("X->n\n");
  
  /* Test alphanumeric to number conversion
     
     Parameters are: 

     a1 data
     z1 size data
     
     U2 sign of result unsigned if U else empty
     n2 y/n to indicate signed result
     z2 size in bits of result
     u2 u or d for [un]signed result
     
     c  what result should be
     d  # digits allowed in result.  */
  
  T1 ("0", 1, u, n, 32, u, 0, 0);
  T1 ("001", 3, u, n, 32, u, 1, 0);
  T1 ("255", 3, u, n, 32, u, 255, 0);
  T1 ("0", 1,, y, 32, d, 0, 0);
  T1 ("128", 3,, y, 32, d, 128, 0);  /* Not sure what will happen here.  */
     
  T1 ("0", 1, u, n, 32, u, 0, 0);
  T1 ("65535", 5, u, n, 32, u, 65535, 0);
  T1 ("0", 1,, y, 32, d, 0, 0);
  T1 ("32767", 5,, y, 32, d, 32767, 0);
     
  T1 ("0", 1, u, n, 32, u, 0, 0);
  T1 ("4294967295", 10, u, n, 32, u, 4294967295U, 0);
  T1 ("0", 1,, y, 32, d, 0, 0);
  T1 ("2147483647", 10,, y, 32, d, 2147483647, 0);
     
  T1 ("0", 1, u, n, 64, u, 0, 0);
  T1 ("18446744073709551615", 20, u, n, 64, u, 18446744073709551615ULL, 0);
  T1 ("12345678901234567890", 20, u, n, 64, u, 12345678901234567890ULL, 0);
  T1 ("0", 1,, y, 64, d, 0, 0);
  T1 ("9223372036854775807", 19,, y, 64, d, 9223372036854775807LL, 0);

  T1 ("123", 3,, y, 64, d, 23, 2);

  /*---.  */

  printf ("n->X\n");

  /* Test number to alphanumeric conversion
     
     Parameters are: 

     a1 data (expected output)
     z1 size data (expected output)
     
     z2 size in bits of input
     n2 ny whether input signed
     d  number of digits to use
     
     v  input value.  */
  
  T2 ("0", 1, 32, n, 1, 0);
  T2 ("001", 3, 32, n, 3, 1);
  T2 ("255", 3, 32, n, 3, 255);
  T2 ("0", 1, 32, y, 1, 0);
  T2 ("128", 3, 32, y, 3, 128);  /* Not sure what will happen here.  */
     
  T2 ("0", 1, 32, n, 1, 0);
  T2 ("65535", 5, 32, n, 5, 65535);
  T2 ("0", 1, 32, y, 1, 0);
  T2 ("32767", 5, 32, y, 5, 32767);
     
  T2 ("0", 1, 32, n, 1, 0);
  T2 ("4294967295", 10, 32, n, 10, 4294967295U);
  T2 ("0", 1, 32, y, 1, 0);
  T2 ("2147483647", 10, 32, y, 10, 2147483647);
  T2 ("2147483647", 10, 32, y, 10, -2147483647);
     
  T2 ("0", 1, 64, n, 1, 0);
  T2 ("18446744073709551615", 20, 64, n, 20, 18446744073709551615ULL);
  T2 ("12345678901234567890", 20, 64, n, 20, 12345678901234567890ULL);
  T2 ("0", 1, 64, y, 1, 0);
  T2 ("9223372036854775807", 19, 64, y, 19, -9223372036854775807LL);

  printf ("Error count = %d\n", error);
  fflush (stderr);
  return error; 
}



