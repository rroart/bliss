
/**********************************************************************
 *                                                                    *
 *              128 Bit binary math routines test driver              *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.            *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of the *
 * License, or (at your option) any later version.                    *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
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
 * Title:          128 bit binary math routines test driver           *
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blir_bin128LLL.h"
#include "blir_bin128ULLL.h"


/**********************************************************************
 *                                                                    *
 * PutDec - write out number in hex                                   *
 *                                                                    *
 *********************************************************************  */

static int32
putDec (const uchar *t,
        const void *n,
        const int32 nLen)
{
  uchar *un;
  int32 i;

  un = (uchar *)n;
  printf ("%s = ", t);
  for (i = 0; i < nLen; i ++)printf ("%02X ", un[i]);
  printf ("\n");
  fflush (stdout);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutNumLLL - write signed 128 bit binary value                      *
 *                                                                    *
 *********************************************************************  */

static int32
putNumLLL (const uchar *t,
          const void *n)
{
  uchar ascii[80];
  blir_LLL_itoa (ascii, n);
  printf ("%s = %s\n", t, ascii); fflush (stdout);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutNumULLL - write unsigned 128 bit binary value                   *
 *                                                                    *
 *********************************************************************  */

static int32
putNumULLL (const uchar *t,
           const void *n)
{
  uchar ascii[80];
  blir_ULLL_itoa (ascii, n);
  printf ("%s = %s\n", t, ascii); fflush (stdout);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Testbin128ULLL - Test the ULLL routines                            *
 *                                                                    *
 *********************************************************************  */

static int32
testbin128ULLL (void)
{
  binLLL a, b, c, d, e, f;
  binLLL cadd;
  binLLL csub;
  binLLL cmul;
  binLLL cdiv;
  binLLL cmod;
  int32 i, j, k;
  uchar pd[5];
  uchar ascii[80];
  uchar work[80];
  uchar i8;
  uint16 i16;
  uint32 i32;
  uint64 i64;

  printf ("\n--- Testing 128 ULLL ---\n\n");

  printf ("\n--> Testing ULLL to various types\n");

  blir_ULLL_i32toi (a, 123);
  blir_ULLL_itod (pd, sizeof (pd), a);
  blir_ULLL_itoa (ascii, a);
  blir_ULLL_itoi8 ( & i8, a);
  blir_ULLL_itoi16 ( & i16, a);
  blir_ULLL_itoi32 ( & i32, a);
  blir_ULLL_itoi64 ( & i64, a);

  printf ("Each value should display 123\n");
  putNumULLL ((uchar *)"a", a);
  putDec ((uchar *)"pd", pd, sizeof (pd));
  printf ("i8  = %d\n", i8);
  printf ("i16 = %d\n", i16);
  printf ("i32 = %d\n", i32);
  printf ("i64 = %lld\n", i64);

  printf ("\n--> Testing various types to ULLL\n");

  i8 = 127;
  i16 = 127;
  i32 = 127;
  i64 = 127;
  memcpy (pd, "\x00\x00\x00\x12\x7C", sizeof (pd));
  strcpy ((char *)ascii, "127");
  blir_ULLL_i8toi (a, i8);
  blir_ULLL_i16toi (b, i16);
  blir_ULLL_i32toi (c, i32);
  blir_ULLL_i64toi (d, i64);
  blir_ULLL_dtoi (e, pd, sizeof (pd));
  blir_ULLL_atoi (f, ascii);
  printf ("Each value should display 127\n");
  putNumULLL ((uchar *)"i8", a);
  putNumULLL ((uchar *)"i16", b);
  putNumULLL ((uchar *)"i32", c);
  putNumULLL ((uchar *)"i64", d);
  putNumULLL ((uchar *)"Dec", e);
  putNumULLL ((uchar *)"Txt", f);

  printf ("\n--> Testing arithmetic\n");

  blir_ULLL_i32toi (a, 15);
  blir_ULLL_i32toi (b, 10);

  blir_ULLL_add (cadd, a, b);
  blir_ULLL_sub (csub, a, b);
  blir_ULLL_mul (cmul, a, b);
  blir_ULLL_div (cdiv, cmod, a, b);

  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  putNumULLL ((uchar *)"a + b", cadd);
  putNumULLL ((uchar *)"a - b", csub);
  putNumULLL ((uchar *)"a * b", cmul);
  putNumULLL ((uchar *)"a / b", cdiv);
  putNumULLL ((uchar *)"a % b", cmod);

  blir_ULLL_i32toi (a, 33);
  blir_ULLL_i32toi (b, 15);

  blir_ULLL_add (cadd, a, b);
  blir_ULLL_sub (csub, a, b);
  blir_ULLL_mul (cmul, a, b);
  blir_ULLL_div (cdiv, cmod, a, b);

  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  putNumULLL ((uchar *)"a + b", cadd);
  putNumULLL ((uchar *)"a - b", csub);
  putNumULLL ((uchar *)"a * b", cmul);
  putNumULLL ((uchar *)"a / b", cdiv);
  putNumULLL ((uchar *)"a % b", cmod);

  printf ("\n--> Testing compare operations\n");

  blir_ULLL_i32toi (a, 10);
  blir_ULLL_i32toi (b, 5);
  printf ("\n");
  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  printf ("a==b = %d\n", blir_ULLL_eq (a, b));
  printf ("a!=b = %d\n", blir_ULLL_ne (a, b));
  printf ("a< b = %d\n", blir_ULLL_lt (a, b));
  printf ("a<=b = %d\n", blir_ULLL_le (a, b));
  printf ("a> b = %d\n", blir_ULLL_gt (a, b));
  printf ("a>=b = %d\n", blir_ULLL_ge (a, b));

  blir_ULLL_i32toi (a, 10);
  blir_ULLL_i32toi (b, 10);
  printf ("\n");
  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  printf ("a==b = %d\n", blir_ULLL_eq (a, b));
  printf ("a!=b = %d\n", blir_ULLL_ne (a, b));
  printf ("a< b = %d\n", blir_ULLL_lt (a, b));
  printf ("a<=b = %d\n", blir_ULLL_le (a, b));
  printf ("a> b = %d\n", blir_ULLL_gt (a, b));
  printf ("a>=b = %d\n", blir_ULLL_ge (a, b));

  blir_ULLL_i32toi (a, 10);
  blir_ULLL_i32toi (b, 20);
  printf ("\n");
  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  printf ("a==b = %d\n", blir_ULLL_eq (a, b));
  printf ("a!=b = %d\n", blir_ULLL_ne (a, b));
  printf ("a< b = %d\n", blir_ULLL_lt (a, b));
  printf ("a<=b = %d\n", blir_ULLL_le (a, b));
  printf ("a> b = %d\n", blir_ULLL_gt (a, b));
  printf ("a>=b = %d\n", blir_ULLL_ge (a, b));

  printf ("\n--> Testing shift operations\n");

  blir_ULLL_i32toi (a, 10);
  blir_ULLL_i32toi (b, 10);
  blir_ULLL_shl (a, 1);
  blir_ULLL_shr (b, 1);
  putNumULLL ((uchar *)" 10 << 1           ", a);
  putNumULLL ((uchar *)" 10 >> 1           ", b);

  blir_ULLL_i32toi (a, 1);
  for (i = 0; i <= 129; i ++)
    {
      sprintf ((char *)work, "2^%03d", i);
      putNumULLL (work, a);
      blir_ULLL_shl (a, 1);
    }

  blir_ULLL_i32toi (a, 1);
  for (j = 1; j <= 20; j ++)

    blir_ULLL_shl (a, 1);

  for (k = 20; k >= 0; k --)
    {
      sprintf ((char *)work, "2^%03d", k);
      putNumULLL (work, a);
      blir_ULLL_shr (a, 1);
    }

  printf ("\n--> Testing miscellaneous operations\n");

  blir_ULLL_i32toi (a, 0);
  blir_ULLL_i32toi (b, 1);
  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  printf ("ifZero(a) = %d\n", blir_ULLL_ifzero (a));
  printf ("ifZero(b) = %d\n", blir_ULLL_ifzero (b));

  blir_ULLL_i32toi (a, -10);
  blir_ULLL_i32toi (b, 0);
  blir_ULLL_i32toi (c, 10);

  blir_ULLL_i32toi (a, 314159);
  blir_ULLL_i32toi (b, 141421);
  printf ("\n");
  putNumULLL ((uchar *)"a", a);
  putNumULLL ((uchar *)"b", b);
  blir_ULLL_copy (a, b);
  blir_ULLL_zero (b);
  putNumULLL ((uchar *)"a copied from b", a);
  putNumULLL ((uchar *)"b zeroed", b);
  blir_ULLL_ones (b);
  putNumULLL ((uchar *)"b ones", b);

  printf ("\n--> Testing limits of data type\n");

  blir_ULLL_copy (a, BLI_UINT128_MAX);
  blir_ULLL_copy (b, BLI_UINT128_MIN);

  blir_ULLL_itoa (work, a);
  printf ("Maximum Value = %s\n", work);
  blir_ULLL_itoa (work, b);
  printf ("Minimum Value = %s\n", work);
  blir_ULLL_atoi (a, (uchar *)"340282366920938463463374607431768211455");
  blir_ULLL_atoi (b, (uchar *)"0");
  printf ("\n");
  putNumULLL ((uchar *)"Maximum value", a);
  putNumULLL ((uchar *)"Minimum value", b);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Testbin128LLL - Test the LLL routines                              *
 *                                                                    *
 *********************************************************************  */

static int32
testbin128LLL (void)
{
  binLLL a, b, c, d, e, f;
  binLLL cadd;
  binLLL csub;
  binLLL cmul;
  binLLL cdiv;
  binLLL cmod;
  int32 i, j, k;
  uchar pd[5];
  uchar ascii[80];
  uchar work[80];
  schar i8;
  int16 i16;
  int32 i32;
  int64 i64;

  printf ("\n--- Testing 128 LLL ---\n\n");

  printf ("\n--> Testing LLL to various types\n");

  blir_LLL_i32toi (a, -123);
  blir_LLL_itod (pd, sizeof (pd), a);
  blir_LLL_itoa ((uchar *)ascii, a);
  blir_LLL_itoi8 ( & i8, a);
  blir_LLL_itoi16 ( & i16, a);
  blir_LLL_itoi32 ( & i32, a);
  blir_LLL_itoi64 ( & i64, a);

  putNumLLL ((uchar *)"a", a);
  putDec ((uchar *)"pd", pd, sizeof (pd));
  printf ("i8  = %d\n", i8);
  printf ("i16 = %d\n", i16);
  printf ("i32 = %d\n", i32);
  printf ("i64 = %lld\n", i64);

  printf ("\n--> Testing various types to LLL\n");

  i8 = 127;
  i16 = 127;
  i32 = 127;
  i64 = 127;
  memcpy (pd, "\x00\x00\x00\x12\x7C", sizeof (pd));
  strcpy ((char *)ascii, "127");
  blir_LLL_i8toi (a, i8);
  blir_LLL_i16toi (b, i16);
  blir_LLL_i32toi (c, i32);
  blir_LLL_i64toi (d, i64);
  blir_LLL_dtoi (e, pd, sizeof (pd));
  blir_LLL_atoi (f, (uchar *)ascii);
  putNumLLL ((uchar *)"i8", a);
  putNumLLL ((uchar *)"i16", b);
  putNumLLL ((uchar *)"i32", c);
  putNumLLL ((uchar *)"i64", d);
  putNumLLL ((uchar *)"Dec", e);
  putNumLLL ((uchar *)"Txt", f);

  printf ("\n--> Testing arithmetic\n");

  blir_LLL_i32toi (a, 10);
  blir_LLL_i32toi (b, 15);

  blir_LLL_add (cadd, a, b);
  blir_LLL_sub (csub, a, b);
  blir_LLL_mul (cmul, a, b);
  blir_LLL_div (cdiv, cmod, a, b);

  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  putNumLLL ((uchar *)"a + b", cadd);
  putNumLLL ((uchar *)"a - b", csub);
  putNumLLL ((uchar *)"a * b", cmul);
  putNumLLL ((uchar *)"a / b", cdiv);
  putNumLLL ((uchar *)"a % b", cmod);

  blir_LLL_i32toi (a, -33);
  blir_LLL_i32toi (b, 15);

  blir_LLL_add (cadd, a, b);
  blir_LLL_sub (csub, a, b);
  blir_LLL_mul (cmul, a, b);
  blir_LLL_div (cdiv, cmod, a, b);

  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  putNumLLL ((uchar *)"a + b", cadd);
  putNumLLL ((uchar *)"a - b", csub);
  putNumLLL ((uchar *)"a * b", cmul);
  putNumLLL ((uchar *)"a / b", cdiv);
  putNumLLL ((uchar *)"a % b", cmod);

  printf ("\n--> Testing compare operations\n");

  blir_LLL_i32toi (a, 10);
  blir_LLL_i32toi (b, -10);
  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  printf ("a==b = %d\n", blir_LLL_eq (a, b));
  printf ("a!=b = %d\n", blir_LLL_ne (a, b));
  printf ("a< b = %d\n", blir_LLL_lt (a, b));
  printf ("a<=b = %d\n", blir_LLL_le (a, b));
  printf ("a> b = %d\n", blir_LLL_gt (a, b));
  printf ("a>=b = %d\n", blir_LLL_ge (a, b));

  blir_LLL_i32toi (a, 10);
  blir_LLL_i32toi (b, 10);
  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  printf ("a==b = %d\n", blir_LLL_eq (a, b));
  printf ("a!=b = %d\n", blir_LLL_ne (a, b));
  printf ("a< b = %d\n", blir_LLL_lt (a, b));
  printf ("a<=b = %d\n", blir_LLL_le (a, b));
  printf ("a> b = %d\n", blir_LLL_gt (a, b));
  printf ("a>=b = %d\n", blir_LLL_ge (a, b));

  blir_LLL_i32toi (a, 10);
  blir_LLL_i32toi (b, 20);
  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  printf ("a==b = %d\n", blir_LLL_eq (a, b));
  printf ("a!=b = %d\n", blir_LLL_ne (a, b));
  printf ("a< b = %d\n", blir_LLL_lt (a, b));
  printf ("a<=b = %d\n", blir_LLL_le (a, b));
  printf ("a> b = %d\n", blir_LLL_gt (a, b));
  printf ("a>=b = %d\n", blir_LLL_ge (a, b));

  printf ("\n--> Testing shift operations\n");

  blir_LLL_i32toi (a, 10);
  blir_LLL_i32toi (b, 10);
  blir_LLL_i32toi (c, 10);
  blir_LLL_i32toi (d, -10);
  blir_LLL_shl (a, 1);
  blir_LLL_shr (b, 1);
  blir_LLL_shra (c, 1);
  blir_LLL_shra (d, 1);
  putNumLLL ((uchar *)" 10 << 1           ", a);
  putNumLLL ((uchar *)" 10 >> 1           ", b);
  putNumLLL ((uchar *)" 10 >> 1 Arithmetic", c);
  putNumLLL ((uchar *)"-10 >> 1 Arithmetic", d);

  blir_LLL_i32toi (a, 1);
  for (i = 0; i <= 129; i ++)
    {
      sprintf ((char *)work, "2^%03d", i);
      putNumLLL ((uchar *)work, a);
      blir_LLL_shl (a, 1);
    }

  blir_LLL_i32toi (a, 1);
  for (j = 1; j <= 20; j ++)

    blir_LLL_shl (a, 1);

  for (k = 20; k >= 0; k --)
    {
      sprintf ((char *)work, "2^%03d", k);
      putNumLLL ((uchar *)work, a);
      blir_LLL_shr (a, 1);
    }

  printf ("\n--> Testing miscellaneous operations\n");

  blir_LLL_i32toi (a, 0);
  blir_LLL_i32toi (b, 1);
  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  printf ("ifZero(a) = %d\n", blir_LLL_ifzero (a));
  printf ("ifZero(b) = %d\n", blir_LLL_ifzero (b));

  blir_LLL_i32toi (a, -10);
  blir_LLL_i32toi (b, 0);
  blir_LLL_i32toi (c, 10);

  printf ("\n");

  putNumLLL ((uchar *)"a", a);
  blir_LLL_complement (a);
  putNumLLL ((uchar *)"Complement(a)", a);

  putNumLLL ((uchar *)"b", b);
  blir_LLL_complement (b);
  putNumLLL ((uchar *)"complement(b)", b);

  putNumLLL ((uchar *)"c", c);
  blir_LLL_complement (c);
  putNumLLL ((uchar *)"complement(c)", c);

  blir_LLL_i32toi (a, 314159);
  blir_LLL_i32toi (b, 141421);
  printf ("\n");
  putNumLLL ((uchar *)"a", a);
  putNumLLL ((uchar *)"b", b);
  blir_LLL_copy (a, b);
  blir_LLL_zero (b);
  putNumLLL ((uchar *)"a copied from b", a);
  putNumLLL ((uchar *)"b zeroed", b);
  blir_LLL_ones (b);
  putNumLLL ((uchar *)"b ones", b);

  printf ("\n--> Testing limits of data type\n");

  blir_LLL_copy (a, BLI_INT128_MAX);
  blir_LLL_copy (b, BLI_INT128_MIN);

  blir_LLL_itoa (work, a);
  printf ("Maximum Value = %s\n", work);
  blir_LLL_itoa (work, b);
  printf ("Minimum Value = %s\n", work);

  blir_LLL_atoi (a, (uchar *)"+170141183460469231731687303715884105727");
  blir_LLL_atoi (b, (uchar *)"-170141183460469231731687303715884105728");
  printf ("\n");
  putNumLLL ((uchar *)"Maximum value", a);
  putNumLLL ((uchar *)"Minimum value", b);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Main - test mainline                                               *
 *                                                                    *
 *********************************************************************  */

int
main (void)
{

  testbin128ULLL ();
  testbin128LLL ();
  return 0;
}
