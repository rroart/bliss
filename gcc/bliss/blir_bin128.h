
/**********************************************************************
 *                                                                    *
 *          128 Bit binary math routines common header file           *
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
 * Title:          128 bit binary math routines common header file    *
 *                                                                    *
 * Description:    This file consists of a serious of routines to     *
 *                 carry out math using 128 bit binary values.  The   *
 *                 hardware only requires a 16 bit binary minimum     *
 *                 word widith for these routines to work.            *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000Initial Release of    *
 *                 Routines T. Seward       11/Aug/2000Changed LL to  *
 *                 LLL, and ULL to ULLL                               *
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
 * #include "blir_bin128LLL.h"                                                      *
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
 * blir_LLL_itoi8(&i8, a)     - returns BLI_INT8    *i8   := a                          *
 * blir_LLL_itoi16(&i16,a)    - returns BLI_INT16   *i16  := a                          *
 * blir_LLL_itoi32(&i32,a)    - returns BLI_INT32   *i32  := a                          *
 * blir_LLL_itoi64(&i64,a)    - returns BLI_INT64   *i64  := a                          *
 *                                                                                  *
 *                                                                                  *
 * Convert other types to LLL                                                       *
 * -------------------------                                                        *
 *                                                                                  *
 * blir_LLL_dtoi(a,&pd,pdlen) - sets Packed  a := pd                                *
 * blir_LLL_atoi(a,&ascii)    - sets ASCII   a := ASCII                             *
 * blir_LLL_i8toi(a,i8)       - sets BLI_INT8    a := i8                                *
 * blir_LLL_i16toi(a,i16)     - sets BLI_INT16   a := i16                               *
 * blir_LLL_i32toi(a,i32)     - sets BLI_INT32   a := i32                               *
 * blir_LLL_i64toi(a,i64)     - sets BLI_INT64   a := i64                               *
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
 * blir_ULLL_itoi32(&i32,a)    - returns BLI_INT32   *i32  := a                         *
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
 * blir_ULLL_i32toi(a,i32)     - sets BLI_INT32   a := i32                              *
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

#ifndef __BLIR_BIN128_H__
#define __BLIR_BIN128_H__

#include "blir_temp_config.h"

/**********************************************************************
 *                                                                    *
 *                         Internal constants                         *
 *                                                                    *
 *********************************************************************  */

#define _LLLNUMBYTE	(16)	/* Bytes in number */
#define _LLLNUMBITS	(_LLLNUMBYTE*8)	/* Bits in number */
#define _LLLUPPERBIT	(0x80u)	/* High order bit */
#define _LLLOWERBIT	(0x01u)	/* Low order bit */
#define _LLLZEROBITS	(0x00u)	/* Byte of all 0's */
#define _LLLONEBITS	(0xFFu)	/* Byte of all 1's */
#define _LLLUPPERBYTE	(0)	/* High order byte of number */
#define _LLLOWERBYTE	(_LLLNUMBYTE-1)	/* Low order byte of number */
#endif
