
/**********************************************************************
 *                                                                    *
 *          128 Bit unsigned binary math routine header file          *
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
 * Title:          128 bit unsigned binary math routine header file   *
 *                                                                    *
 * Description:    This file consists of a serious of routines to     *
 *                 carry out math using 128 bit unsigned binary       *
 *                 values.  The hardware only requires a 16 bit       *
 *                 binary minimum word widith for these routines to   *
 *                 work.                                              *
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
 *          Unsigned 128 bit routine (ULLL = stands for Unsigned Long Long)         *
 ************************************************************************************
 *                                                                                  *
 * typedef name:         binULLL                                                    *
 * typedef pointer name: binULLLPtr                                                 *
 *                                                                                  *
 * Note:  binULLL is the same defintion as binLLL                                   *
 *        binULLLPtr is the same definition as binLLLPtr                            *
 *                                                                                  *
 * BLI_UINT128_MIN  = Binary minimum value: 0                                           *
 * BLI_UINT128_MAX  = Binary maximum value: 340282366920938463463374607431768211455     *
 * BLI_UINT128_ZERO = Binary value:         0                                           *
 *                                                                                  *
 * Header File:  blir_bin128ULLL.h                                                  *
 * Routines:     blir_bin128ULLL.c                                                  *
 *                                                                                  *
 *                                                                                  *
 * Example                                                                          *
 * -------                                                                          *
 *                                                                                  *
 * #include "blir_bin128ULLL.h"                                                     *
 *                                                                                  *
 * binULLL a,b,c,r;                                                                 *
 * char  str[50];                                                                   *
 *                                                                                  *
 * blir_ULLL_i32toi(b,57);                                                          *
 * blir_ULLL_i32toi(c,10);                                                          *
 * blir_ULLL_div(a,r,b,c);                                                          *
 * printf("a = %s\n",blir_ULLL_itoa(str,a));  / * prints 5 * /                      *
 * printf("r = %s\n",blir_ULLL_itoa(str,r));  / * prints 7 * /                      *
 *                                                                                  *
 *                                                                                  *
 * Arithmetic                                                                       *
 * ----------                                                                       *
 *                                                                                  *
 * blir_ULLL_add(a,b,c)        - a := b + c                                         *
 * blir_ULLL_sub(a,b,c)        - a := b - c                                         *

 * blir_ULLL_mul(a,b,c)        - a := b * c                                         *
 * blir_ULLL_div(a,r,b,c)      - a := b / c; r := b % c                             *
 * blir_ULLL_copy(a,b)         - a := b                        [Macro]              *
 * blir_ULLL_zero(a)           - a := 0                        [Macro]              *
 * blir_ULLL_ones(a)           - a := 1's                      [Macro]              *
 *                                                                                  *
 *                                                                                  *
 * Comparisons                                                                      *
 * -----------                                                                      *
 *                                                                                  *
 * blir_ULLL_compare(a,b)      - a::b; -1 := a<b,  0 := a==b, +1 := a>b             *
 * blir_ULLL_eq(a,b)           - a::b,  1 := a==b, 0 := a!=b   [Macro]              *
 * blir_ULLL_ne(a,b)           - a::b,  1 := a!=b, 0 := a==b   [Macro]              *
 * blir_ULLL_lt(a,b)           - a::b,  1 := a<b,  0 := a>=b   [Macro]              *
 * blir_ULLL_le(a,b)           - a::b,  1 := a<=b, 0 := a>b    [Macro]              *
 * blir_ULLL_gt(a,b)           - a::b,  1 := a>b,  0 := a<=b   [Macro]              *
 * blir_ULLL_ge(a,b)           - a::b,  1 := a>=b, 0 := a<b    [Macro]              *
 * blir_ULLL_ifzero(a)         - a::0,  1 := a==0, 0 := a!=0   [Macro]              *
 *                                                                                  *
 *                                                                                  *
 * Convert from ULLL to other type                                                  *
 * ------------------------------                                                   *
 *                                                                                  *
 * blir_ULLL_itoa(&ascii,a)    - returns ASCII   ascii := a (returns ptr to ascii)  *
 * blir_ULLL_itod(&pd,pdlen,a) - returns Packed  pd    := a                         *
 * blir_ULLL_itoi8(&i8, a)     - returns BLI_INT8    *i8   := a                         *
 * blir_ULLL_itoi16(&i16,a)    - returns BLI_INT16   *i16  := a                         *
 * blir_ULLL_itoi32(&i32,a)    - returns int32   *i32  := a                         *
 * blir_ULLL_itoi64(&i64,a)    - returns BLI_INT64   *i64  := a                         *
 *                                                                                  *
 *                                                                                  *
 * Convert other types to ULLL                                                      *
 * --------------------------                                                       *
 *                                                                                  *
 * blir_ULLL_dtoi(a,&pd,pdlen) - sets Packed  a := pd                               *
 * blir_ULLL_atoi(a,&ascii)    - sets ASCII   a := ASCII                            *
 * blir_ULLL_i8toi(a,i8)       - sets BLI_INT8    a := i8                               *
 * blir_ULLL_i16toi(a,i16)     - sets BLI_INT16   a := i16                              *
 * blir_ULLL_i32toi(a,i32)     - sets int32   a := i32                              *
 * blir_ULLL_i64toi(a,i64)     - sets BLI_INT64   a := i64                              *
 *                                                                                  *
 *                                                                                  *
 * Shifts                                                                           *
 * ------                                                                           *
 *                                                                                  *
 * blir_ULLL_shl(a,n)          - a := a << n                                        *
 * blir_ULLL_shr(a,n)          - a := a >> n                                        *
 *                                                                                  *
 ***********************************************************************************  */

#ifndef __BLIR_BIN128ULLL_H__
#define __BLIR_BIN128ULLL_H__

#include <string.h>
#include "blir_temp_config.h"
#include "blir_bin128.h"

/**********************************************************************
 *                                                                    *
 *                         Internal constants                         *
 *                                                                    *
 *********************************************************************  */

#define _ULLLMAXNUM	"340282366920938463463374607431768211455"
#define _ULLLMAXLEN	(39)

/**********************************************************************
 *                                                                    *
 *                      User constants and types                      *
 *                                                                    *
 *********************************************************************  */

#define BLI_UINT128_MIN     "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define BLI_UINT128_MAX     "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define BLI_UINT128_ZERO    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

/**********************************************************************
 *                                                                    *
 *                    Global binary types for ULLL                    *
 *                                                                    *
 *********************************************************************  */

typedef uchar binULLL[_LLLNUMBYTE];  /* ULLL number.  */
typedef uchar *binULLLPtr;  /* ULLL Pointer type.  */

/**********************************************************************
 *                                                                    *
 *                       Macros for ULLL types                        *
 *                                                                    *
 *********************************************************************  */

#define blir_ULLL_eq(a,b) 	(blir_ULLL_compare(a,b)==0)
#define blir_ULLL_ne(a,b) 	(blir_ULLL_compare(a,b)!=0)
#define blir_ULLL_lt(a,b) 	(blir_ULLL_compare(a,b)<0)
#define blir_ULLL_le(a,b) 	(blir_ULLL_compare(a,b)<=0)
#define blir_ULLL_gt(a,b) 	(blir_ULLL_compare(a,b)>0)
#define blir_ULLL_ge(a,b) 	(blir_ULLL_compare(a,b)>=0)
#define blir_ULLL_ifzero(a)	((memcmp(a,BLI_UINT128_ZERO,_LLLNUMBYTE)==0)?1:0)
#define blir_ULLL_zero(a) 	memset(a,_LLLZEROBITS,_LLLNUMBYTE)
#define blir_ULLL_ones(a)       memset(a,_LLLONEBITS,_LLLNUMBYTE)
#define blir_ULLL_copy(a,b)	memcpy(a,b,_LLLNUMBYTE)



/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */

uchar *
blir_ULLL_itoa (uchar *str,  /* Destination string.  */
                const void *n);  /* Source number.  */

int32 blir_ULLL_itod (void *pd,  /* Packed number.  */
                     int32 pdLen,  /* Packed length.  */
                     void *n);  /* Number to convert.  */

int32 blir_ULLL_itoi8 (uchar *v,  /* Results.  */
                       void *a);  /* Number to convert.  */

int32 blir_ULLL_itoi16 (uint16 *v,  /* Results.  */
                       void *a);  /* Number to convert.  */

int32 blir_ULLL_itoi32 (uint32 *v,  /* Results.  */
                       void *a);  /* Number to convert.  */

int32 blir_ULLL_itoi64 (uint64 *v,  /* Results.  */
                       void *a);  /* Number to convert.  */

int32
blir_ULLL_dtoi (void *n,  /* Destination Number.  */
                const void *pd,  /* Packed Decimal.  */
                const int32 pdlen);  /* Packed Decimal Length.  */

int32
blir_ULLL_atoi (void *n,  /* Destination Number.  */
                const uchar *str);  /* Source string.  */

int32
blir_ULLL_i8toi (void *a,  /* Destination.  */
                 const uchar v);  /* Byte value.  */

int32
blir_ULLL_i16toi (void *a,  /* Destination.  */
                  const uint16 v);  /* Byte value.  */

int32
blir_ULLL_i32toi (void *a,  /* Destination.  */
                  const uint32 v);  /* Byte value.  */

int32
blir_ULLL_i64toi (void *a,  /* Destination.  */
                  const uint64 v);  /* Byte value.  */

int32
blir_ULLL_shl (void *a,  /* Number to shift.  */
               int bits);  /* Number of bits to shift.  */

int32
blir_ULLL_shr (void *a,  /* Number to shift.  */
               int bits);  /* Number of bits to shift.  */

int32
blir_ULLL_add (void *a,  /* Answer.  */
               const void *b,  /* Operand 1.  */
               const void *c);  /* Operand 2.  */

int32
blir_ULLL_sub (void *a,  /* Answer.  */
               const void *b,  /* Operand 1.  */
               const void *c);  /* Operand 2.  */

int32
blir_ULLL_mul (void *a,  /* Answer.  */
               const void *b,  /* Operand 1.  */
               const void *c);  /* Operand 2.  */

int32
blir_ULLL_div (void *a,  /* Answer.  */
               void *r,  /* Remainder.  */
               const void *b,  /* Operand 1.  */
               const void *c);  /* Operand 2.  */

int32
blir_ULLL_compare (const void *n1,  /* Left of compare.  */
                   const void *n2);  /* Right of compare.  */

#endif
