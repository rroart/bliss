/* 

   BLISS Compiler exponentiation routines tester

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
#include "blir_exponentiation.h"
#include <stdio.h>

/* 
   Parameters are: 

   z1 size data, 
   s1 sign data, 
   U1 U for unsigned data else nothing , 
   u1 u for unsigned data else nothing, 

   z2 size exponent, 
   s2 sign exponent, 
   U2 for unsigned exponent else nothing,
   u2 for unsigned exponent else nothing, 

   v1 value of data, 
   v2 value of exponent, 
   c1 value result should be.  */

#define T(z1,s1,U1,u1,z2,s2,U2,u2,v1,v2,c1)\
{\
  U1##int##z1 result;\
  result=blir_exponentiation_##z1##_##s1##_##z1##_##s2(v1, v2);\
  e1=(uchar *)"";\
  if (result!=c1)\
    {\
       e1=(uchar *)" ERROR NOT ";\
       error++;\
    }\
  printf(#z1 " " #s1 " " #U1 " " #u1  " " #z2 " " #s2 " " #U2 " " #u2 " " #v1 " " #v2 " " #c1 ": "\
         "%" BLI_64_FORMAT_PREFIX #u1\
         "**%"BLI_64_FORMAT_PREFIX #u2\
         " =%" BLI_64_FORMAT_PREFIX #u1\
         "%s =expected "\
         "%" BLI_64_FORMAT_PREFIX #u1 "\n",\
         (U1##int64)v1,\
         (U2##int64)v2,\
         (U1##int64)result,\
         (native_char *)e1,\
         (U1##int64)c1);\
}


int
main (void)
{
  uint32 error = 0;
  uchar *e1;
  T (32, n, u, u, 32, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (32, n, u, u, 32, n, u, u, 2, 0, 1);  /* 2**0=1.  */
  T (32, n, u, u, 32, y,, d, 1, -1, 1);  /* 1**-1=1.  */
  T (32, n, u, u, 32, y,, d, 3, -1, 0);  /* 3**-1=0.  */
  T (32, y,, d, 32, y,, d, -1, 0x010000, 1);  /* -1**64K=1.  */

  T (64, n, u, u, 64, n, u, u, 2, 32, 4294967296ULL);  /* 2**32=4G.  */
  T (64, n, u, u, 64, n, u, u, 2, 63, 9223372036854775808ULL);  /* 2**63=BIG.  */

  T (64, y,, d, 64, y,, d, 2, 32, 4294967296LL);  /* 2**32=4G.  */

  /* Various sizes etc.  */
  T (32, n, u, u, 32, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (32, n, u, u, 32, y,, d, 2, 5, 32);  /* 2**5=32.  */
  T (32, n, u, u, 64, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (32, n, u, u, 64, y,, d, 2, 5, 32);  /* 2**5=32.  */

  T (32, y,, d, 32, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (32, y,, d, 32, y,, d, 2, 5, 32);  /* 2**5=32.  */
  T (32, y,, d, 64, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (32, y,, d, 64, y,, d, 2, 5, 32);  /* 2**5=32.  */

  T (64, n, u, u, 32, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (64, n, u, u, 32, y,, d, 2, 5, 32);  /* 2**5=32.  */
  T (64, n, u, u, 64, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (64, n, u, u, 64, y,, d, 2, 5, 32);  /* 2**5=32.  */

  T (64, y,, d, 32, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (64, y,, d, 32, y,, d, 2, 5, 32);  /* 2**5=32.  */
  T (64, y,, d, 64, n, u, u, 2, 5, 32);  /* 2**5=32.  */
  T (64, y,, d, 64, y,, d, 2, 5, 32);  /* 2**5=32.  */

  /* 2**N.  */
  T (64, n, u, u, 64, n, u, u, 2, 0, 1); 
  T (64, n, u, u, 64, n, u, u, 2, 1, 2); 
  T (64, n, u, u, 64, n, u, u, 2, 2, 4); 
  T (64, n, u, u, 64, n, u, u, 2, 3, 8); 
  T (64, n, u, u, 64, n, u, u, 2, 4, 16); 
  T (64, n, u, u, 64, n, u, u, 2, 5, 32); 
  T (64, n, u, u, 64, n, u, u, 2, 6, 64); 
  T (64, n, u, u, 64, n, u, u, 2, 7, 128); 
  T (64, n, u, u, 64, n, u, u, 2, 8, 256); 
  T (64, n, u, u, 64, n, u, u, 2, 9, 512); 
  
  T (64, n, u, u, 64, n, u, u, 2, 10, 1024); 
  T (64, n, u, u, 64, n, u, u, 2, 11, 2048); 
  T (64, n, u, u, 64, n, u, u, 2, 12, 4096); 
  T (64, n, u, u, 64, n, u, u, 2, 13, 8192); 
  T (64, n, u, u, 64, n, u, u, 2, 14, 16384); 
  T (64, n, u, u, 64, n, u, u, 2, 15, 32768ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 16, 65536ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 17, 131072ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 18, 262144ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 19, 524288ULL); 
  
  T (64, n, u, u, 64, n, u, u, 2, 20, 1048576ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 21, 2097152ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 22, 4194304ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 23, 8388608ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 24, 16777216ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 25, 33554432ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 26, 67108864ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 27, 134217728ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 28, 268435456ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 29, 536870912ULL); 
  
  T (64, n, u, u, 64, n, u, u, 2, 30, 1073741824ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 31, 2147483648ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 32, 4294967296ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 33, 8589934592ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 34, 17179869184ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 35, 34359738368ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 36, 68719476736ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 37, 137438953472ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 38, 274877906944ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 39, 549755813888ULL); 
  
  T (64, n, u, u, 64, n, u, u, 2, 40, 1099511627776ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 41, 2199023255552ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 42, 4398046511104ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 43, 8796093022208ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 44, 17592186044416ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 45, 35184372088832ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 46, 70368744177664ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 47, 140737488355328ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 48, 281474976710656ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 49, 562949953421312ULL); 
  
  T (64, n, u, u, 64, n, u, u, 2, 50, 1125899906842624ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 51, 2251799813685248ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 52, 4503599627370496ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 53, 9007199254740992ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 54, 18014398509481984ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 55, 36028797018963968ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 56, 72057594037927936ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 57, 144115188075855872ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 58, 288230376151711744ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 59, 576460752303423488ULL); 
  
  T (64, n, u, u, 64, n, u, u, 2, 60, 1152921504606846976ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 61, 2305843009213693952ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 62, 4611686018427387904ULL); 
  T (64, n, u, u, 64, n, u, u, 2, 63, 9223372036854775808ULL);

  T (64, n, u, u, 64, n, u, u, 2, 64, 0ULL);  /* Mod 2**64.  */

  printf ("Error count = %d\n", error);
  fflush (stderr);
  return error; 
}
