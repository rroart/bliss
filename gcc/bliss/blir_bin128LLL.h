
/**********************************************************************
 *                                                                    *
 *          128 Bit signed binary math routines header file           *
 *                                                                    *
 *           Copyright (C) 2000 by Theodore J.  Seward, Jr.            *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public License *
 * as published by the Free Software Foundation; either version 2.1   *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * Lesser General Public License for more details.                    *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  *
 * 02111-1307 USA                                                     *
 *                                                                    *
 * Mail:           Theodore J. Seward, Jr.                            *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail:         tseward@compuserve.com                             *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title:          128 bit signed binary math routines header file    *
 *                                                                    *
 * Description:    This file consists of a serious of routines to     *
 *                 carry out math using 128 bit signed binary values.  *
 *                 The hardware only requires a 16 bit binary minimum *
 *                 word widith for these routines to work.            *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000Initial Release of    *
 *                 Routines T. Seward       29/Jul/2000Replaced       *
 *                 *_copy, *_zero, *_ifzero with macros.  T.          *
 *                 Seward       29/Jul/2000Added define for           *
 *                 BLI_UINT128_ZERO (used by *_ifzero macro) T.       *
 *                 Seward       11/Aug/2000Changed LL to LLL, and ULL *
 *                 to ULLL                                            *
 *                                                                    *
 *********************************************************************  */

/************************************************************************************
 *                Signed 128 bit routine (LLL = stands for Long Long)               *
 ************************************************************************************
 *                                                                                  *
 * typedef name:         binLLL                                                     *
 * typedef pointer name: binLLLPtr                                                  *
 *                                                                                  *
 * Note:  binLLL is the same defintion as binULLL                                   *
 *        binLLLPtr is the same definition as binULLLPtr                            *
 *                                                                                  *
 * BLI_INT128_MIN  = Binary minimum value: -170141183460469231731687303715884105728     *
 * BLI_INT128_MAX  = Binary maximum value: +170141183460469231731687303715884105727     *
 * BLI_INT128_ZERO = Binary value:          0                                           *
 *                                                                                  *
 * Header File:  blir_bin128LLL.h                                                   *
 * Routines:     blir_bin128LLL.c                                                   *
 *                                                                                  *
 *                                                                                  *
 * Example                                                                          *
 * -------                                                                          *
 *                                                                                  *
 * #include "blir_bin128LLL.H"                                                      *
 *                                                                                  *
 * binLLL a,b,c,r;                                                                  *
 * char  str[50];                                                                   *
 *                                                                                  *
 * blir_LLL_i32toi(b,57);                                                           *
 * blir_LLL_i32toi(c,10);                                                           *
 * blir_LLL_div(a,r,b,c);                                                           *
 * printf("a = %s\n",blir_LLL_itoa(str,a));  / * prints 5 * /                       *
 * printf("r = %s\n",blir_LLL_itoa(str,r));  / * prints 7 * /                       *
 *                                                                                  *
 *                                                                                  *
 * Arithmetic                                                                       *
 * ----------                                                                       *
 *                                                                                  *
 * blir_LLL_add(a,b,c)        - a := b + c                                          *
 * blir_LLL_sub(a,b,c)        - a := b - c                                          *
 * blir_LLL_mul(a,b,c)        - a := b * c                                          *
 * blir_LLL_div(a,r,b,c)      - a := b / c; r := b % c                              *
 * blir_LLL_complement(a)     - a := -a                                             *
 * blir_LLL_copy(a,b)         - a := b                        [Macro]               *
 * blir_LLL_zero(a)           - a := 0                        [Macro]               *
 * blir_LLL_ones(a)           - a := -1                       [Macro]               *
 *                                                                                  *
 *                                                                                  *
 * Comparisons                                                                      *
 * -----------                                                                      *
 *                                                                                  *
 * blir_LLL_compare(a,b)      - a::b; -1 := a<b,  0 := a==b, +1 := a>b              *
 * blir_LLL_eq(a,b)           - a::b,  1 := a==b, 0 := a!=b   [Macro]               *
 * blir_LLL_ne(a,b)           - a::b,  1 := a!=b, 0 := a==b   [Macro]               *
 * blir_LLL_lt(a,b)           - a::b,  1 := a<b,  0 := a>=b   [Macro]               *
 * blir_LLL_le(a,b)           - a::b,  1 := a<=b, 0 := a>b    [Macro]               *
 * blir_LLL_gt(a,b)           - a::b,  1 := a>b,  0 := a<=b   [Macro]               *
 * blir_LLL_ge(a,b)           - a::b,  1 := a>=b, 0 := a<b    [Macro]               *
 * blir_LLL_ifzero(a)         - a::0,  1 := a==0, 0 := a!=0   [Macro]               *
 *                                                                                  *
 *                                                                                  *
 * Convert from LLL to other type                                                   *
 * -----------------------------                                                    *
 *                                                                                  *
 * blir_LLL_itoa(&ascii,a)    - returns ASCII   ascii := a (returns ptr to ascii)   *
 * blir_LLL_itod(&pd,pdlen,a) - returns Packed  pd    := a                          *
 * blir_LLL_itoi8(&i8, a)     - returns schar    *i8   := a                          *
 * blir_LLL_itoi16(&i16,a)    - returns int16   *i16  := a                          *
 * blir_LLL_itoi32(&i32,a)    - returns BLI_INT32   *i32  := a                          *
 * blir_LLL_itoi64(&i64,a)    - returns int64   *i64  := a                          *
 *                                                                                  *
 *                                                                                  *
 * Convert other types to LLL                                                       *
 * -------------------------                                                        *
 *                                                                                  *
 * blir_LLL_dtoi(a,&pd,pdlen) - sets Packed  a := pd                                *
 * blir_LLL_atoi(a,&ascii)    - sets ASCII   a := ASCII                             *
 * blir_LLL_i8toi(a,i8)       - sets schar    a := i8                                *
 * blir_LLL_i16toi(a,i16)     - sets int16   a := i16                               *
 * blir_LLL_i32toi(a,i32)     - sets BLI_INT32   a := i32                               *
 * blir_LLL_i64toi(a,i64)     - sets int64   a := i64                               *
 *                                                                                  *
 *                                                                                  *
 * Shifts                                                                           *
 * ------                                                                           *
 *                                                                                  *
 * blir_LLL_shl(a,n)          - a := a << n                                         *
 * blir_LLL_shra(a,n)         - a := a >> n  [arithmetic]                           *
 * blir_LLL_shr(a,n)          - a := a >> n  [logical]                              *
 *                                                                                  *
 ***********************************************************************************  */

#ifndef __BLIR_BIN128LLL_H__
#define __BLIR_BIN128LLL_H__

#include <string.h>
#include "blir_temp_config.h"
#include "blir_bin128.h"

/**********************************************************************
 *                                                                    *
 *                         Internal constants                         *
 *                                                                    *
 *********************************************************************  */

#define _LLLMAXPOS	"170141183460469231731687303715884105727"
#define _LLLMAXNEG	"170141183460469231731687303715884105728"
#define _LLLMAXLEN	(39)

/**********************************************************************
 *                                                                    *
 *                      User constants and types                      *
 *                                                                    *
 *********************************************************************  */

#define BLI_INT128_MAX      "\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define BLI_INT128_MIN      "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define BLI_INT128_ZERO     "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

/**********************************************************************
 *                                                                    *
 *         Global binary types - LLL & ULLL have same layout          *
 *                                                                    *
 *********************************************************************  */

typedef uchar binLLL[_LLLNUMBYTE];  /* LLL number.  */
typedef uchar *binLLLPtr;  /* LLL Pointer type.  */

/**********************************************************************
 *                                                                    *
 *                        Macros for LLL types                        *
 *                                                                    *
 *********************************************************************  */

#define blir_LLL_eq(a,b) 	(blir_LLL_compare(a,b)==0)
#define blir_LLL_ne(a,b) 	(blir_LLL_compare(a,b)!=0)
#define blir_LLL_lt(a,b) 	(blir_LLL_compare(a,b)<0)
#define blir_LLL_le(a,b) 	(blir_LLL_compare(a,b)<=0)
#define blir_LLL_gt(a,b) 	(blir_LLL_compare(a,b)>0)
#define blir_LLL_ge(a,b) 	(blir_LLL_compare(a,b)>=0)
#define blir_LLL_ifzero(a)	((memcmp(a,BLI_INT128_ZERO,_LLLNUMBYTE)==0)?1:0)
#define blir_LLL_zero(a) 	memset(a,_LLLZEROBITS,_LLLNUMBYTE)
#define blir_LLL_ones(a)        memset(a,_LLLONEBITS,_LLLNUMBYTE)
#define blir_LLL_copy(a,b)	memcpy(a,b,_LLLNUMBYTE)

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */


uchar *
blir_LLL_itoa (uchar *str,  /* Destination string.  */
		 const void *n);  /* Source number.  */

int32 blir_LLL_itod (void *pd,  /* Packed number.  */
                    int32 pdLen,  /* Packed length.  */
                    void *n);  /* Number to convert.  */

int32 blir_LLL_itoi8 (schar *v,  /* Results.  */
                      void *a);  /* Number to convert.  */

int32 blir_LLL_itoi16 (int16 *v,  /* Results.  */
void *a);  /* Number to convert.  */

int32 blir_LLL_itoi32 (int32 *v,  /* Results.  */
                      void *a);  /* Number to convert.  */

int32 blir_LLL_itoi64 (int64 *v,  /* Results.  */
void *a);  /* Number to convert.  */

int
blir_LLL_dtoi (void *n,  /* Destination Number.  */
       const void *pd,  /* Packed Decimal.  */
	const int pdlen);  /* Packed Decimal Length.  */

int
          blir_LLL_atoi (void *n,  /* Destination Number.  */
		 const uchar *str);  /* Source string.  */

int
          blir_LLL_i8toi (void *a,  /* Destination.  */
		  const schar v);  /* Byte value.  */

int
          blir_LLL_i16toi (void *a,  /* Destination.  */
		   const int16 v);  /* Byte value.  */

int
          blir_LLL_i32toi (void *a,  /* Destination.  */
		   const int32 v);  /* Byte value.  */

int
          blir_LLL_i64toi (void *a,  /* Destination.  */
		   const int64 v);  /* Byte value.  */

int
          blir_LLL_shl (void *a,  /* Number to shift.  */
		 int bits);  /* Number of bits to shift.  */

int
blir_LLL_shra (void *a,  /* Number to shift.  */
       int bits);  /* Number of bits to shift.  */

int
          blir_LLL_shr (void *a,  /* Number to shift.  */
		 int bits);  /* Number of bits to shift.  */

int
          blir_LLL_complement (void *n);  /* Number to complement.  */

int
          blir_LLL_add (void *a,  /* Answer.  */
		 const void *b,  /* Operand 1.  */
		 const void *c);  /* Operand 2.  */

int
          blir_LLL_sub (void *a,  /* Answer.  */
		 const void *b,  /* Operand 1.  */
		 const void *c);  /* Operand 2.  */

int
          blir_LLL_mul (void *a,  /* Answer.  */
		 const void *b,  /* Operand 1.  */
		 const void *c);  /* Operand 2.  */

int
          blir_LLL_div (void *a,  /* Answer.  */
		 void *r,  /* Remainder.  */
		 const void *b,  /* Operand 1.  */
		 const void *c);  /* Operand 2.  */

int
          blir_LLL_compare (const void *n1,  /* Left of compare.  */
		     const void *n2);  /* Right of compare.  */


#endif
