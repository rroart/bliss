
/**********************************************************************
 *                                                                    *
 *                128 Bit signed binary math routines                 *
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
 * Mail:   Theodore J. Seward, Jr.                                    *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title:  128 bit signed binary math routines                        *
 *                                                                    *
 * Description:    This file consists of a serious of routines to     *
 *                 carry out math using 128 bit signed binary values.  *
 *                 The hardware only requires a 16 bit binary minimum *
 *                 word widith for these routines to work.            *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000     Initial Release  *
 *                 of Routines T. Seward       29/Jul/2000            *
 *                 Replaced *_copy, *_zero, *_ifzero with macros.  T. *
 *                 Seward       11/Aug/2000     Changed LL to LLL,    *
 *                 and ULL to ULLL                                    *
 *                                                                    *
 *********************************************************************  */

/************************************************************************************
 *                Signed 128 bit routine (LLL = stands for Long Long)               *
 ************************************************************************************
 *                                                                                  *
 * typedef name:         binLLL                                                     *
 * typedef pointer name: binLLLPtr                                                  *
 *                                                                                  *
 * Note:  binLLL is the same definition as binULLL                                  *
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
 * blir_LLL_itoi8(&i8, a)     - returns schar    *i8   := a                          *
 * blir_LLL_itoi16(&i16,a)    - returns int16   *i16  := a                          *
 * blir_LLL_itoi32(&i32,a)    - returns BLI_INT32   *i32  := a                          *
 * blir_LLL_itoi64(&i64,a)    - returns BLI_INT64   *i64  := a                          *
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

#include <string.h>
#include <ctype.h>
#include "blir_bin128LLL.h"


/**********************************************************************
 *                                                                    *
 * Cobr_LLL_shl - LLL type Shift Left                                 *
 *                                                                    *
 *********************************************************************  */

int32
blir_LLL_shl (void *a,  /* Number to shift.  */
              const int32 bits) /* Number of bits to shift.  */
{
  binLLLPtr ua;
  uint32 carry;
  uint32 ncarry;
  int32 i, b;

  carry = 0;

  ua = (binLLLPtr)a;
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
 * Cobr_LLL_shra - LLL type Shift Right Arithmetic                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_shra (void *a,  /* Number to shift.  */
               int32 bits) /* Number of bits to shift.  */
{
  binLLLPtr ua;
  uint32 carry;
  uint32 ncarry;
  int32 i, b;

  carry = 0;
  ua = (binLLLPtr)a;

  for (b = 1; b <= bits; b ++)
    {
      carry = ua[_LLLUPPERBYTE] & _LLLUPPERBIT;  /* Propagate sign bit.  */
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
 * Cobr_LLL_shr - LLL type shift right                                *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_shr (void *a,  /* Number to shift.  */
              int32 bits) /* Number of bits to shift.  */
{
  binLLLPtr ua;
  uint32 carry;
  uint32 ncarry;
  int32 i, b;

  carry = 0;
  ua = (binLLLPtr)a;

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
 * Cobr_LLL_complement - LLL type 2's complement of value             *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_complement (void *n) /* Number to complement.  */
{
  binLLLPtr un;
  int32 i;
  uint32 carry;
  uint32 sum;

  un = (binLLLPtr)n;
  carry = 1;

  for (i = _LLLNUMBYTE - 1; i >= 0; i --)
    {
      sum = (un[i] ^ _LLLONEBITS) + carry;
      un[i] = (uchar)(sum & _LLLONEBITS);
      carry = sum >> 8;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_add - LLL type Add routine                                *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_add (void *a,  /* Answer.  */
              const void *b,  /* Operand 1.  */
              const void *c) /* Operand 2.  */
{
  binLLLPtr ua;
  binLLLPtr ub;
  binLLLPtr uc;
  uint32 carry;
  uint32 sum;
  int32 i;

  ua = (binLLLPtr)a;
  ub = (binLLLPtr)b;
  uc = (binLLLPtr)c;

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
 * Cobr_LLL_sub - LLL type Subtract routine                           *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_sub (void *a,  /* Answer.  */
              const void *b,  /* Operand 1.  */
              const void *c) /* Operand 2.  */
{
  binLLLPtr ua;
  binLLLPtr ub;
  binLLLPtr uc;
  uint32 borrow;
  uint32 diff;
  int32 i;

  ua = (binLLLPtr)a;
  ub = (binLLLPtr)b;
  uc = (binLLLPtr)c;

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
 * Cobr_LLL_mul - LLL type Multiply routine                           *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_mul (void *a,  /* Answer.  */
              const void *b,  /* Operand 1.  */
              const void *c) /* Operand 2.  */
{
  binLLLPtr ua;
  binLLL ub;
  binLLL uc;
  binLLL mq;
  int32 i;
  int32 sign;

  blir_LLL_copy (ub, b);
  blir_LLL_copy (uc, c);

  sign = 1;

  if ((ub[_LLLUPPERBYTE] & _LLLUPPERBIT) != 0)
    {
      blir_LLL_complement (ub);
      sign = -1;
    }

  if ((uc[_LLLUPPERBYTE] & _LLLUPPERBIT) != 0)
    {
      blir_LLL_complement (uc);
      if (sign == -1)
        sign = 1;
      else
        sign = -1;
    }

  ua = (binLLLPtr)a;

  blir_LLL_copy (mq, ub);
  blir_LLL_zero (ua);

  for (i = 1; i <= _LLLNUMBITS; i ++)
    {
      blir_LLL_shl (a, 1);
      if (blir_LLL_shl (mq, 1))
        blir_LLL_add (a, a, c);
    }

  if (sign == -1)
    blir_LLL_complement (a);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_div - LLL type Divide routine                             *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_div (void *a,  /* Answer.  */
              void *r,  /* Remainder.  */
              const void *b,  /* Operand 1.  */
              const void *c) /* Operand 2.  */
{
  binLLLPtr ua;
  binLLLPtr ur;
  binLLL ub;
  binLLL uc;
  int32 count;
  uint32 carry;
  int32 i;
  int32 sign;

  if (blir_LLL_ifzero (c)) /* Divide by zero.  */
    return -1;

  blir_LLL_copy (ub, b);
  blir_LLL_copy (uc, c);

  sign = 1;

  if ((ub[_LLLUPPERBYTE] & _LLLUPPERBIT) != 0)
    {
      blir_LLL_complement (ub);
      sign = -1;
    }

  if ((uc[_LLLUPPERBYTE] & _LLLUPPERBIT) != 0)
    {
      blir_LLL_complement (uc);
      if (sign == -1)
        sign = 1;
      else
        sign = -1;
    }

  ua = (binLLLPtr)a;
  ur = (binLLLPtr)r;

  blir_LLL_copy (ur, ub);
  blir_LLL_zero (ua);

  count = 0;
  while (uc[0] == _LLLZEROBITS)
    {
      count += 8;
      for (i = 1; i < _LLLNUMBYTE; i ++)
        uc[i - 1] = uc[i];
      uc[_LLLOWERBYTE] = _LLLZEROBITS;
    }

  while ((uc[0] & _LLLUPPERBIT) == _LLLZEROBITS)
    {
      blir_LLL_shl (uc, 1);
      count ++;
    }

  count ++;

  for (i = 1; i <= count; i ++)
    {
      blir_LLL_shl (ua, 1);
      carry = blir_LLL_sub (ur, ur, uc);
      if (carry)
        blir_LLL_add (ur, ur, uc);
      else
        ua[_LLLOWERBYTE] |= _LLLOWERBIT;
      blir_LLL_shr (uc, 1);
    }

  if (sign == -1)
    {
      blir_LLL_complement (a);
      blir_LLL_complement (r);
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_compare - LLL type Compare routine                        *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_compare (const void *n1,  /* Left of compare.  */
                  const void *n2) /* Right of compare.  */
{
  binLLL compare;
  int32 i;

  blir_LLL_sub (compare, n1, n2);

  if ((compare[_LLLUPPERBYTE] & _LLLUPPERBIT) != 0)
    return -1;  /* < 0.  */
  for (i = 0; i < _LLLNUMBYTE; i ++)
    if (compare[i] != _LLLZEROBITS)
      return 1;  /* > 0.  */
  return 0;  /* = 0.  */
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_dtoi - Packed Decimal to LLL type                         *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_dtoi (void *n,  /* Destination Number.  */
               const void *pd,  /* Packed Decimal.  */
               const int32 pdlen) /* Packed Decimal Length.  */
{
  int32 digit;  /* Digit of decimal number.  */
  int32 sigFlag;  /* Significant digit flag.  */
  binLLLPtr un;
  binLLL wk1;
  binLLL wk2;
  int32 i, j;
  int32 sign;  /* Sign.  */
  uchar work[_LLLMAXLEN + 1];  /* Digits + '\0'.  */
  uchar *upd;  /* Pointer to pd.  */
  uchar ch;  /* Next character.  */

  blir_LLL_zero (n);

  upd = (uchar *)pd;

  ch = upd[pdlen -1];

  switch (ch & 0x0F) /* Get sign if any.  */
    {
    case 0x0C :
    case 0x0F :
      sign = 1;  /* Plus sign.  */
      break;
    case 0x0D :
      sign = -1;  /* Negative sign.  */
      break;
    default :
      sign = 1;  /* Default Plus.  */
      break;
    }
  sigFlag = 0;
  j = 0;
  for (i = 0; i < (pdlen -1); i ++)
    {
      ch = upd[i];
      digit = (ch >> 4) & 0x0F;
      if (sigFlag)
        {
          if (j < _LLLMAXLEN)
            work[j ++] = (uchar)(digit + '0');
          else
            return 1;
        }
      else if (digit > 0)
        {
          sigFlag = 1;
          if (j < _LLLMAXLEN)
            work[j ++] = (uchar)(digit + '0');
          else
            return 1;
        }

      digit = ch & 0x0F;
      if (sigFlag)
        {
          if (j < _LLLMAXLEN)
            work[j ++] = (uchar)(digit + '0');
          else
            return 1;
        }
      else if (digit > 0)
        {
          sigFlag = 1;
          if (j < _LLLMAXLEN)
            work[j ++] = (uchar)(digit + '0');
          else
            return 1;
        }
    }

  digit = (upd[pdlen -1] >> 4) & 0x0F;
  if (j < _LLLMAXLEN)
    work[j ++] = (uchar)(digit + '0');
  else
    return 1;

  work[j] = '\0';

  if (j > _LLLMAXLEN) /* Should have been caught earlier.  */
    {
      blir_LLL_zero (n);
      return 1;
    }

  if (j == _LLLMAXLEN) /* Right at overflow boundary.  */
    {
      if (sign == -1)
        i = strcmp ((char *)work, _LLLMAXNEG);
      else
        i = strcmp ((char *)work, _LLLMAXPOS);
      if (i > 0)
        {
          blir_LLL_zero (n);
          return 1;
        }
    }

  un = (binLLLPtr)n;

  blir_LLL_zero (un);

  i = 0;
  while (work[i] != '\0')
    {
      for (j = 0; j < _LLLNUMBYTE; j ++)
        {
          wk1[j] = un[j];
          wk2[j] = un[j];
        }
      blir_LLL_shl (wk1, 1);  /* Wk1 := wk1 * 2.  */
      blir_LLL_shl (wk2, 3);  /* Wk1 := wk1 * 8.  */
      blir_LLL_add (un, wk1, wk2);  /* Un := un * 10.  */
      blir_LLL_i8toi (wk1, (uchar)(work[i ++] - '0'));
      blir_LLL_add (un, un, wk1);  /* Un := un + ch - '0'.  */
    }

  if (sign == -1) /* If negative, complement the value.  */
    blir_LLL_complement (un);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_atoi - ASCII to LLL type                                  *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_atoi (void *n,  /* Destination Number.  */
               const uchar *str) /* Source string.  */
{
  binLLLPtr un;
  binLLL wk1;
  binLLL wk2;
  int32 i, j;
  int32 sign;  /* Sign.  */
  uchar work[_LLLMAXLEN + 1];  /* Digits + '\0'.  */

  i = 0;

  while (isspace (str[i]))
    i ++;  /* Skip leading spaces.  */

  switch (str[i]) /* Get sign if any.  */
    {
    case '+' :
      sign = 1;  /* Plus sign.  */
      i ++;
      break;
    case '-' :
      sign = -1;  /* Negative sign.  */
      i ++;
      break;
    default :
      sign = 1;  /* Default Plus.  */
      break;
    }

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
          if (j < _LLLMAXLEN)
            work[j ++] = str[i ++];
          else
            {
              blir_LLL_zero (n);
              return 1;
            }
        }
      work[j] = '\0';
    }

  if (j > _LLLMAXLEN) /* Should have been caught earlier.  */
    {
      blir_LLL_zero (n);
      return 1;
    }

  if (j == _LLLMAXLEN) /* Right at overflow boundary.  */
    {
      if (sign == -1)
        i = strcmp ((char *)work, _LLLMAXNEG);
      else
        i = strcmp ((char *)work, _LLLMAXPOS);
      if (i > 0)
        {
          blir_LLL_zero (n);
          return 1;
        }
    }

  un = (binLLLPtr)n;

  blir_LLL_zero (un);

  i = 0;
  while (work[i] != '\0')
    {
      for (j = 0; j < _LLLNUMBYTE; j ++)
        {
          wk1[j] = un[j];
          wk2[j] = un[j];
        }
      blir_LLL_shl (wk1, 1);  /* Wk1 := wk1 * 2.  */
      blir_LLL_shl (wk2, 3);  /* Wk1 := wk1 * 8.  */
      blir_LLL_add (un, wk1, wk2);  /* Un := un * 10.  */
      blir_LLL_i8toi (wk1, (uchar)(work[i ++] - '0'));
      blir_LLL_add (un, un, wk1);  /* Un := un + ch - '0'.  */
    }

  if (sign == -1) /* If negative, complement the value.  */
    blir_LLL_complement (un);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_i8toi - Convert 8 bit integer to LLL                      *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_i8toi (void *a,  /* Destination.  */
                const schar v) /* Byte value.  */
{
  binLLLPtr ua;
  int32 i;
  int32 j;

  ua = (binLLLPtr)a;

  blir_LLL_zero (ua);

  j = _LLLOWERBYTE;
  ua[j --] = (uchar)(v & _LLLONEBITS);
  if (v < 0)
    {
      for (i = _LLLUPPERBYTE; i <= j; i ++)
        ua[i] = _LLLONEBITS;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_i16toi - Convert 16 bit integer to LLL                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_i16toi (void *a,  /* Destination.  */
                 const int16 v) /* Byte value.  */
{
  binLLLPtr ua;
  int16 n;
  int32 i;
  int32 j;
  size_t k;

  ua = (binLLLPtr)a;

  blir_LLL_zero (ua);

  j = _LLLOWERBYTE;
  n = v;

  for (k = 1; k <= sizeof (n); k ++)
    {
      ua[j --] = (uchar)(n & _LLLONEBITS);
      n >>= 8;
    }

  if (v < 0)
    {
      for (i = _LLLUPPERBYTE; i <= j; i ++)
        ua[i] = _LLLONEBITS;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_i32toi - Convert 32 bit integer to LLL                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_i32toi (void *a,  /* Destination.  */
                 const int32 v) /* Byte value.  */
{
  binLLLPtr ua;
  int32 n;
  int32 i;
  int32 j;
  size_t k;

  ua = (binLLLPtr)a;

  blir_LLL_zero (ua);

  j = _LLLOWERBYTE;
  n = v;
  for (k = 1; k <= sizeof (n); k ++)
    {
      ua[j --] = (uchar)(n & _LLLONEBITS);
      n >>= 8;
    }

  if (v < 0)
    {
      for (i = _LLLUPPERBYTE; i <= j; i ++)
        ua[i] = _LLLONEBITS;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_i64toi - Convert 64 bit integer to LLL                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_i64toi (void *a,  /* Destination.  */
                 const int64 v) /* Byte value.  */
{
  binLLLPtr ua;
  int64 n;
  int32 i;
  int32 j;
  size_t k;

  ua = (binLLLPtr)a;

  blir_LLL_zero (ua);

  j = _LLLOWERBYTE;
  n = v;
  for (k = 1; k <= sizeof (n); k ++)
    {
      ua[j --] = (uchar)(n & _LLLONEBITS);
      n >>= 8;
    }

  if (v < 0)
    {
      for (i = _LLLUPPERBYTE; i <= j; i ++)
        ua[i] = _LLLONEBITS;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_itoa - LLL type to ASCII                                  *
 *                                                                    *
 *********************************************************************  */

uchar *
blir_LLL_itoa (uchar *str,  /* Destination string.  */
               const void *n) /* Source number.  */
{
  uchar stack[_LLLNUMBYTE * 4];
  binLLLPtr un;
  binLLL lun;
  binLLL mod;
  binLLL ten;
  int32 sign;
  int32 stackPtr;
  int32 i;

  if (blir_LLL_compare (n, BLI_INT128_MIN) == 0)
    {
      strcpy ((char *)str, "-");
      strcat ((char *)str, _LLLMAXNEG);
      return 0;
    }

  un = (binLLLPtr)n;
  blir_LLL_copy (lun, un);

  if ((lun[_LLLUPPERBYTE] & _LLLUPPERBIT) == 0)
    sign = 1;
  else
    {
      blir_LLL_complement (lun);
      sign = -1;
    }

  blir_LLL_i8toi (ten, 10);
  stackPtr = 0;
  do
    {
      blir_LLL_div (lun, mod, lun, ten);
      stack[stackPtr ++] = (uchar)(mod[_LLLOWERBYTE] + '0');
    }
  while (!blir_LLL_ifzero (lun));

  i = 0;
  stackPtr --;
  if (sign == -1)
    str[i ++] = '-';
  while (stackPtr >= 0)
    str[i ++] = stack[stackPtr --];
  str[i] = '\0';

  return str;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_itod - Convert LLL to packed decimal                      *
 *                                                                    *
 *********************************************************************  */

int32 blir_LLL_itod (void *pd,  /* Packed number.  */
                    int32 pdLen,  /* Packed length.  */
                    void *n) /* Number to convert.  */
{
  uchar stack[_LLLNUMBYTE * 4];
  binLLLPtr un;
  uchar *upd;
  binLLL lun;
  binLLL mod;
  binLLL ten;
  int32 sign;
  schar maxWork[_LLLNUMBYTE * 4];
  int32 stackPtr;
  int32 i, j;
  uchar digit1;
  uchar digit2;

  un = (binLLLPtr)n;
  upd = (uchar *)pd;

  if (blir_LLL_compare (n, BLI_INT128_MIN) == 0)
    {
      sign = -1;
      strcpy ((char *)maxWork, _LLLMAXNEG);
      stackPtr = 0;
      for (i = strlen ((char *)maxWork)-1; i >= 0; i --)
        stack[stackPtr ++] = maxWork[i];
      goto packNumber;
    }

  blir_LLL_copy (lun, un);

  if ((lun[_LLLUPPERBYTE] & _LLLUPPERBIT) == 0)
    sign = 1;
  else
    {
      blir_LLL_complement (lun);
      sign = -1;
    }

  blir_LLL_i8toi (ten, 10);
  stackPtr = 0;
  do
    {
      blir_LLL_div (lun, mod, lun, ten);
      stack[stackPtr ++] = (uchar)(mod[_LLLOWERBYTE] + '0');
    }
  while (!blir_LLL_ifzero (lun));

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
  if (sign == 1)
    digit1 = 0x0C;
  else
    digit1 = 0x0D;
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
 * Cobr_LLL_itoi8 - Convert LLL to 8 bit integer                      *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_itoi8 (schar *v,  /* Results.  */
               void *a) /* Number to convert.  */
{
  binLLLPtr ua;

  ua = (binLLLPtr)a;

  * v = ua[_LLLOWERBYTE];

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_LLL_itoi16 - Convert LLL to 16 bit integer                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_itoi16 (int16 *v,  /* Results.  */
                void *a) /* Number to convert.  */
{
  binLLLPtr ua;
  int16 w;
  int16 byte;
  int32 j;
  size_t k;

  ua = (binLLLPtr)a;

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
 * Cobr_LLL_itoi32 - Convert LLL to 32 bit integer                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_itoi32 (int32 *v,  /* Results.  */
                void *a) /* Number to convert.  */
{
  binLLLPtr ua;
  int32 w;
  int32 byte;
  int32 j;
  size_t k;

  ua = (binLLLPtr)a;

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
 * Cobr_LLL_itoi64 - Convert LLL to 64 bit integer                    *
 *                                                                    *
 *********************************************************************  */

int
blir_LLL_itoi64 (int64 *v,  /* Results.  */
                void *a) /* Number to convert.  */
{
  binLLLPtr ua;
  int64 w;
  int64 byte;
  int32 j;
  size_t k;

  ua = (binLLLPtr)a;

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
