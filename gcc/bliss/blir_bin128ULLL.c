/**********************************************************************
 *                                                                    *
 *               128 Bit unsigned binary math routines                *
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
 * Title:          128 bit unsigned binary math routines              *
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
 *                 Seward       11/Aug/2000Changed LL to LLL, and ULL *
 *                 to ULLL                                            *
 *                                                                    *
 *********************************************************************  */

/************************************************************************************
 *         Unsigned 128 bit routine (ULLL = stands for Unsigned Long Long)          *
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
 * #include "BLIR_BIN128ULLL.h"                                                     *
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

#include <string.h>
#include <ctype.h>

#include "blir_bin128ULLL.h"

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_shl - ULLL type shift left                               *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_shl (void *a,  /* Number to shift.  */
               const int32 bits) /* Number of bits to shift.  */
{
  binULLLPtr ua;
  uint32 carry;
  uint32 ncarry;
  int32 i, b;

  carry = 0;
  ua = (binULLLPtr)a;

  for (b = 1; b <= bits; b ++)
    {
      carry = 0;
      for (i = _LLLOWERBYTE; i >= _LLLUPPERBYTE; i --)
	{
	  ncarry = ua[i] & _LLLUPPERBIT;
	  ua[i] = (uchar)(((ua[i] << 1) & _LLLONEBITS) | carry);
	  if (ncarry)
	    carry = _LLLOWERBIT;
	  else
	    carry = _LLLZEROBITS;
	}
    }

  if (carry != 0)
    carry = 1;
  return carry;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_shr - ULLL type shift right                              *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_shr (void *a,  /* Number to shift.  */
               int32 bits) /* Number of bits to shift.  */
{
  binULLLPtr ua;
  uint32 carry;
  uint32 ncarry;
  int32 i, b;

  carry = 0;
  ua = (binULLLPtr)a;

  for (b = 1; b <= bits; b ++)
    {
      carry = 0;
      for (i = _LLLUPPERBYTE; i <= _LLLOWERBYTE; i ++)
	{
	  ncarry = ua[i] & _LLLOWERBIT;
	  ua[i] = (uchar)(((ua[i] >> 1) & _LLLONEBITS) | carry);
	  if (ncarry)
	    carry = _LLLUPPERBIT;
	  else
	    carry = _LLLZEROBITS;
	}
    }

  if (carry != 0)
    carry = 1;

  return carry;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_add - ULLL type add routine                              *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_add (void *a,  /* Answer.  */
               const void *b,  /* Operand 1.  */
               const void *c) /* Operand 2.  */
{
  binULLLPtr ua;
  binULLLPtr ub;
  binULLLPtr uc;
  uint32 carry;
  uint32 sum;
  int32 i;

  ua = (binULLLPtr)a;
  ub = (binULLLPtr)b;
  uc = (binULLLPtr)c;

  carry = 0;
  for (i = _LLLOWERBYTE; i >= _LLLUPPERBYTE; i --)
    {
      sum = ub[i] + uc[i] + carry;
      if (sum > _LLLONEBITS)
	{
	  carry = 1;
	  ua[i] = (uchar)(sum & _LLLONEBITS);
	}
      else
	{
	  carry = 0;
	  ua[i] = (uchar)sum;
	}
    }

  return carry;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_sub - ULLL type subtract routine                         *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_sub (void *a,  /* Answer.  */
               const void *b,  /* Operand 1.  */
               const void *c) /* Operand 2.  */
{
  binULLLPtr ua;
  binULLLPtr ub;
  binULLLPtr uc;
  uint32 borrow;
  uint32 diff;
  int32 i;

  ua = (binULLLPtr)a;
  ub = (binULLLPtr)b;
  uc = (binULLLPtr)c;

  borrow = 0;
  for (i = _LLLOWERBYTE; i >= _LLLUPPERBYTE; i --)
    {
      diff = (0x100 | ub[i]) - uc[i] - borrow;
      if (diff < 0x100u)
	{
	  borrow = 1;
	  ua[i] = (uchar)diff;
	}
      else
	{
	  borrow = 0;
	  ua[i] = (uchar)(diff & _LLLONEBITS);
	}
    }

  return borrow;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_mul - ULLL type multiply routine                         *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_mul (void *a,  /* Answer.  */
               const void *b,  /* Operand 1.  */
               const void *c) /* Operand 2.  */
{
  binULLLPtr ua;
  binULLLPtr ub;
  binULLL mq;
  int32 i;

  ua = (binULLLPtr)a;
  ub = (binULLLPtr)b;

  blir_ULLL_copy (mq, ub);
  blir_ULLL_zero (ua);

  for (i = 1; i <= _LLLNUMBITS; i ++)
    {
      blir_ULLL_shl (a, 1);
      if (blir_ULLL_shl (mq, 1))
	blir_ULLL_add (a, a, c);
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_div - ULLL type divide routine                           *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_div (void *a,  /* Answer.  */
               void *r,  /* Remainder.  */
               const void *b,  /* Operand 1.  */
               const void *c) /* Operand 2.  */
{
  binULLLPtr ua;
  binULLLPtr ur;
  binULLLPtr ub;
  binULLLPtr uc;
  binULLL luc;
  int32 count;
  uint32 carry;
  int32 i;

  ua = (binULLLPtr)a;
  ur = (binULLLPtr)r;
  ub = (binULLLPtr)b;
  uc = (binULLLPtr)c;

  if (blir_ULLL_ifzero (uc)) /* Divide by zero.  */
    return -1;

  blir_ULLL_copy (luc, uc);
  blir_ULLL_copy (ur, ub);
  blir_ULLL_zero (ua);

  count = 0;
  while (luc[0] == _LLLZEROBITS)
    {
      count += 8;
      for (i = 1; i < _LLLNUMBYTE; i ++)
	luc[i - 1] = luc[i];
      luc[_LLLOWERBYTE] = _LLLZEROBITS;
    }

  while ((luc[0] & _LLLUPPERBIT) == _LLLZEROBITS)
    {
      blir_ULLL_shl (luc, 1);
      count ++;
    }

  count ++;

  for (i = 1; i <= count; i ++)
    {
      blir_ULLL_shl (ua, 1);
      carry = blir_ULLL_sub (ur, ur, luc);
      if (carry)
        blir_ULLL_add (ur, ur, luc);
      else
        ua[_LLLOWERBYTE] |= _LLLOWERBIT;
      blir_ULLL_shr (luc, 1);
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_compare - ULLL type compare routine                      *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_compare (const void *n1,  /* Left of compare.  */
                   const void *n2) /* Right of compare.  */
{
  binULLLPtr un1;
  binULLLPtr un2;
  int32 i;  /* Index.  */

  un1 = (binULLLPtr)n1;
  un2 = (binULLLPtr)n2;

  for (i = 0; i < _LLLNUMBYTE; i ++)
    if (un1[i] != un2[i])
      {
	if (un1[i] < un2[i])
	  return -1;
	else
	  return 1;
      }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_atoi - ASCII to ULLL type                                *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_atoi (void *n,  /* Destination Number.  */
                const uchar *str) /* Source string.  */
{
  binULLLPtr un;
  binULLL wk1;
  binULLL wk2;
  int32 i, j;
  int32 c;
  uchar work[_ULLLMAXLEN + 1];  /* Digits + '\0'.  */

  i = 0;

  while (isspace (str[i]))
    i ++;  /* Skip leading spaces.  */

  while (str[i] == '0')
    i ++;  /* Skip leading zeros.  */
  j = 0;
  if (!isdigit (str[i]))
    {
      work[j ++] = '0';
      work[j] = '\0';
    }
  else
    {
      while (isdigit (str[i]))
	{
	  if (j < _ULLLMAXLEN)
	    work[j ++] = str[i ++];
	  else
	    {
	      blir_ULLL_zero (n);
	      return 1;
	    }
	}
      work[j] = '\0';
    }
  if (j > _ULLLMAXLEN) /* Should have been caught earlier.  */
    {
      blir_ULLL_zero (n);
      return 1;
    }

  if (j == _ULLLMAXLEN) /* Right at overflow boundary.  */
    {
      c = strcmp ((char *)work, _ULLLMAXNUM);
      if (c > 0)
	{
	  blir_ULLL_zero (n);
	  return 1;
	}

      if (c == 0)
        {
          blir_ULLL_copy (n, BLI_UINT128_MAX);
          return 0;
        }
    }

  un = (binULLLPtr)n;

  blir_ULLL_zero (un);

  i = 0;
  while (work[i] != '\0')
    {
      for (j = 0; j < _LLLNUMBYTE; j ++)
	{
	  wk1[j] = un[j];  /* Wk1 := un.  */
	  wk2[j] = un[j];  /* Wk2 := un.  */
	}
      blir_ULLL_shl (wk1, 1);  /* Wk1 := wk1 * 2.  */
      blir_ULLL_shl (wk2, 3);  /* Wk2 := wk2 * 8.  */
      blir_ULLL_add (un, wk1, wk2);  /* Un := un * 10.  */
      blir_ULLL_i8toi (wk1, (uchar)(work[i ++] - '0'));
      blir_ULLL_add (un, un, wk1);  /* Un := un + ch - '0'.  */
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_dtoi - Packed Decimal to ULLL type                       *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_dtoi (void *n,  /* Destination Number.  */
                const void *pd,  /* Packed Decimal.  */
                const int32 pdlen) /* Packed Decimal Length.  */
{
  int32 digit;  /* Digit of decimal number.  */
  int32 sigFlag;  /* Significant digit flag.  */
  binULLLPtr un;
  binULLL wk1;
  binULLL wk2;
  int32 i, j;
  uchar work[_ULLLMAXLEN + 1];  /* Digits + '\0'.  */
  uchar *upd;  /* Pointer to pd.  */
  uchar ch;  /* Next character.  */

  blir_ULLL_zero (n);

  upd = (uchar *)pd;
  sigFlag = 0;
  j = 0;
  for (i = 0; i < (pdlen -1); i ++)
    {
      ch = upd[i];
      digit = (ch >> 4) & 0x0F;
      if (sigFlag)
        {
	  if (j < _ULLLMAXLEN)
	    work[j ++] = (uchar)(digit + '0');
	  else
            return 1;
        }
      else if (digit > 0)
        {
          sigFlag = 1;
	  if (j < _ULLLMAXLEN)
	    work[j ++] = (uchar)(digit + '0');
	  else
            return 1;
        }

      digit = ch & 0x0F;
      if (sigFlag)
        {
	  if (j < _ULLLMAXLEN)
	    work[j ++] = (uchar)(digit + '0');
	  else
            return 1;
        }
      else if (digit > 0)
        {
          sigFlag = 1;
	  if (j < _ULLLMAXLEN)
	    work[j ++] = (uchar)(digit + '0');
	  else
            return 1;
        }
    }

  digit = (upd[pdlen -1] >> 4) & 0x0F;
  if (j < _ULLLMAXLEN)
    work[j ++] = (uchar)(digit + '0');
  else
    return 1;

  work[j] = '\0';

  if (j > _ULLLMAXLEN) /* Should have been caught earlier.  */
    {
      blir_ULLL_zero (n);
      return 1;
    }

  if (j == _ULLLMAXLEN) /* Right at overflow boundary.  */
    {
      if (strcmp ((char *)work, _ULLLMAXNUM) > 0)
	{
	  blir_ULLL_zero (n);
	  return 1;
	}
    }

  un = (binULLLPtr)n;

  blir_ULLL_zero (un);

  i = 0;
  while (work[i] != '\0')
    {
      for (j = 0; j < _LLLNUMBYTE; j ++)
	{
	  wk1[j] = un[j];
	  wk2[j] = un[j];
	}
      blir_ULLL_shl (wk1, 1);  /* Wk1 := wk1 * 2.  */
      blir_ULLL_shl (wk2, 3);  /* Wk1 := wk1 * 8.  */
      blir_ULLL_add (un, wk1, wk2);  /* Un := un * 10.  */
      blir_ULLL_i8toi (wk1, (uchar)(work[i ++] - '0'));
      blir_ULLL_add (un, un, wk1);  /* Un := un + ch - '0'.  */
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_i8toi - unsigned 8 bit integer to ULLL type              *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_i8toi (void *a,  /* Destination.  */
                 const uchar v) /* Byte value.  */
{
  binULLLPtr ua;

  ua = (binULLLPtr)a;

  blir_ULLL_zero (ua);

  ua[_LLLOWERBYTE] = (uchar)(v & _LLLONEBITS);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_i16toi - unsigned 16 bit integer to ULLL type            *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_i16toi (void *a,  /* Destination.  */
                  const uint16 v) /* Byte value.  */
{
  binULLLPtr ua;
  uint16 n;
  int32 j;
  size_t k;

  ua = (binULLLPtr)a;

  blir_ULLL_zero (ua);

  j = _LLLOWERBYTE;
  n = v;
  for (k = 1; k <= sizeof (n); k ++)
    {
      ua[j --] = (uchar)(n & _LLLONEBITS);
      n >>= 8;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_i32toi - unsigned 32 bit integer to ULLL type            *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_i32toi (void *a,  /* Destination.  */
                  const uint32 v) /* Byte value.  */
{
  binULLLPtr ua;
  uint32 n;
  int32 j;
  size_t k;

  ua = (binULLLPtr)a;

  blir_ULLL_zero (ua);

  j = _LLLOWERBYTE;
  n = v;
  for (k = 1; k <= sizeof (n); k ++)
    {
      ua[j --] = (uchar)(n & _LLLONEBITS);
      n >>= 8;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_i64toi - unsigned 64 bit integer to ULLL type            *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_i64toi (void *a,  /* Destination.  */
                  const uint64 v) /* Byte value.  */
{
  binULLLPtr ua;
  uint64 n;
  int32 j;
  size_t k;

  ua = (binULLLPtr)a;

  blir_ULLL_zero (ua);

  j = _LLLOWERBYTE;
  n = v;
  for (k = 1; k <= sizeof (n); k ++)
    {
      ua[j --] = (uchar)(n & _LLLONEBITS);
      n >>= 8;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_itoa - ULLL type to ASCII                                *
 *                                                                    *
 *********************************************************************  */

uchar *
blir_ULLL_itoa (uchar *str,  /* Destination string.  */
                const void *n) /* Source number.  */
{
  uchar stack[_LLLNUMBYTE * 4];
  binULLLPtr un;
  binULLL lun;
  binULLL mod;
  binULLL ten;

  int32 stackPtr;
  int32 i;

  if (blir_ULLL_compare (n, BLI_UINT128_MAX) == 0)
    {
      strcpy ((char *)str, _ULLLMAXNUM);
      return 0;
    }

  un = (binULLLPtr)n;
  blir_ULLL_copy (lun, un);
  blir_ULLL_i8toi (ten, 10);
  stackPtr = 0;
  do
    {
      blir_ULLL_div (lun, mod, lun, ten);
      stack[stackPtr ++] = (uchar)(mod[_LLLOWERBYTE] + '0');
    }
  while (!blir_ULLL_ifzero (lun));

  i = 0;
  stackPtr --;
  while (stackPtr >= 0)
    str[i ++] = stack[stackPtr --];
  str[i] = '\0';

  return str;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_itod - Convert ULLL to packed decimal                    *
 *                                                                    *
 *********************************************************************  */

int32 blir_ULLL_itod (void *pd,  /* Packed number.  */
                     int32 pdLen,  /* Packed length.  */
                     void *n) /* Number to convert.  */
{
  uchar stack[_LLLNUMBYTE * 4];
  binULLLPtr un;
  uchar *upd;
  binULLL lun;
  binULLL mod;
  binULLL ten;
  schar maxWork[_LLLNUMBYTE * 4];
  int32 stackPtr;
  int32 i, j;
  uchar digit1;
  uchar digit2;

  un = (binULLLPtr)n;
  upd = (uchar *)pd;

  if (blir_ULLL_compare (n, BLI_UINT128_MAX) == 0)
    {
      strcpy ((char *)maxWork, _ULLLMAXNUM);
      stackPtr = 0;
      for (i = strlen ((char *)maxWork)-1; i >= 0; i --)
        stack[stackPtr ++] = maxWork[i];
      goto packNumber;
    }

  blir_ULLL_copy (lun, un);
  blir_ULLL_i8toi (ten, 10);
  stackPtr = 0;
  do
    {
      blir_ULLL_div (lun, mod, lun, ten);
      stack[stackPtr ++] = (uchar)(mod[_LLLOWERBYTE] + '0');
    }
  while (!blir_ULLL_ifzero (lun));

 packNumber :
  for (i = 0; i < pdLen; i ++)upd[i] = 0x00;

  if ((stackPtr % 2) == 0)
    i = (stackPtr +2) / 2;
  else
    i = (stackPtr +1) / 2;
  if (i > pdLen)
    {
      upd[pdLen -1] = 0x0C;
      return 1;
    }

  i = 0;
  j = pdLen - 1;
  digit2 = (uchar)(stack[i ++] - '0');
  digit1 = 0x0C;
  upd[j --] = (uchar)((digit2 << 4) | digit1);
  while (i < stackPtr)
    {
      digit1 = (uchar)(stack[i ++] - '0');
      if (i < stackPtr)digit2 = (uchar)(stack[i ++] - '0');
      else digit2 = 0;
      upd[j --] = (uchar)((digit2 << 4) | digit1);
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_itoi8 - Convert ULLL to unsigned 8 bit integer           *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_itoi8 (uchar *v,  /* Results.  */
                void *a) /* Number to convert.  */
{
  binULLLPtr ua;

  ua = (binULLLPtr)a;

  * v = (uchar)ua[_LLLOWERBYTE];

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_itoi16 - Convert ULLL to unsigned 16 bit integer         *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_itoi16 (uint16 *v,  /* Results.  */
                 void *a) /* Number to convert.  */
{
  binULLLPtr ua;
  uint16 w;
  uint16 byte;
  int32 j;
  size_t k;

  ua = (binULLLPtr)a;

  w = 0;
  j = _LLLOWERBYTE - sizeof (w) + 1;
  for (k = 1; k <= sizeof (w); k ++)
    {
      byte = ua[j ++];
      w <<= 8;
      w = w | byte;
    }

  * v = w;
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_itoi32 - Convert ULLL to unsigned 32 bit integer         *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_itoi32 (uint32 *v,  /* Results.  */
                 void *a) /* Number to convert.  */
{
  binULLLPtr ua;
  uint32 w;
  uint32 byte;
  int32 j;
  size_t k;

  ua = (binULLLPtr)a;
  w = 0;
  j = _LLLOWERBYTE - sizeof (w) + 1;
  for (k = 1; k <= sizeof (w); k ++)
    {
      byte = ua[j ++];
      w <<= 8;
      w = w | byte;
    }

  * v = w;
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_ULLL_itoi64 - Convert ULLL to unsigned 64 bit integer         *
 *                                                                    *
 *********************************************************************  */

int
blir_ULLL_itoi64 (uint64 *v,  /* Results.  */
                 void *a) /* Number to convert.  */
{
  binULLLPtr ua;
  uint64 w;
  uint64 byte;
  int32 j;
  size_t k;

  ua = (binULLLPtr)a;

  w = 0;
  j = _LLLOWERBYTE - sizeof (w) + 1;
  for (k = 1; k <= sizeof (w); k ++)
    {
      byte = ua[j ++];
      w <<= 8;
      w = w | byte;
    }

  * v = w;
  return 0;
}

