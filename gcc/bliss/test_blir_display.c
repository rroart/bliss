
/*                                                                    *
 *                       BLISS Display Function                       *
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
 *  */


/**********************************************************************
 *                                                                    *
 * Title:          BLISS Display Function                             *
 *                                                                    *
 * Description:    This file is part of the BLISS Display Function.   *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   21/Aug/2000  Initial Release of Routines T.            *
 *                          Josling  11/Nov/2000  Move stop           *
 *                          run/literal test out                      *
 *                                                                    *
 *********************************************************************  */


#include <string.h>
#include "blir_display.h"
#include "blir_globals.h"


/**********************************************************************
 *                                                                    *
 * Main - Program mainline                                            *
 *                                                                    *
 *********************************************************************  */

int
main (void)
{
  schar i8;  /* 8 Bit signed value.  */
  int16 i16;  /* 16 Bit signed value.  */
  int32 i32;  /* 32 Bit signed value.  */
  int64 i64;  /* 64 Bit signed value.  */
  uchar u8;  /* 8 Bit unsigned value.  */
  uint16 u16;  /* 16 Bit unsigned value.  */
  uint32 u32;  /* 32 Bit unsigned value.  */
  uint64 u64;  /* 64 Bit unsigned value.  */
  bli_float_32 fshort;  /* 32 Bit floating point.  */
  bli_float_64 fdouble;  /* 64 Bit floating point.  */
  const char *cptr;  /* Character pointer.  */
  const char *pptr;  /* Picture pointer.  */


  /**********************************************************************
   *                                                                    *
   * Test text display                                                  *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Text ==\n\n");
  blir_display ("This is a message", 17, 0, blir_typeText, blir_flagNull,
                NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test Signed Binary display                                         *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Binary ==\n\n");
  i8 = -123;
  blir_display ( & i8, sizeof (i8), 8, blir_typeBinary, blir_flagNull, NULL);
  printf ("    ");
  i16 = -3141;
  blir_display ( & i16, sizeof (i16), 8, blir_typeBinary, blir_flagNull, NULL);
  printf ("    ");
  i32 = -314159;
  blir_display ( & i32, sizeof (i32), 8, blir_typeBinary, blir_flagNull, NULL);
  printf ("    ");
  i64 = -31415926535LL;
  blir_display ( & i64, sizeof (i64), 8, blir_typeBinary, blir_flagNull, NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test Unsigned Binary display                                       *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Unsigned Binary ==\n\n");
  u8 = 123;
  blir_display ( & u8, sizeof (u8), 8, blir_typeUnsigned, blir_flagNull, NULL);
  printf ("    ");
  u16 = 3141;
  blir_display ( & u16, sizeof (u16), 8, blir_typeUnsigned, blir_flagNull,
                NULL);
  printf ("    ");
  u32 = 314159;
  blir_display ( & u32, sizeof (u32), 8, blir_typeUnsigned, blir_flagNull,
                NULL);
  printf ("    ");
  u64 = 31415926535ULL;
  blir_display ( & u64, sizeof (u64), 8, blir_typeUnsigned, blir_flagNull,
                NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test Floating point short display                                  *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Floating point short ==\n\n");
  fshort = 1.234567;
  printf ("Image = %64.32f\n", fshort);
  blir_display ( & fshort, sizeof (fshort), 4, blir_typeFloat, blir_flagNull, NULL);
  printf ("    ");
  blir_display ( & fshort, sizeof (fshort), 3, blir_typeFloat, blir_flagNull, NULL);
  printf ("    ");
  blir_display ( & fshort, sizeof (fshort), 5, blir_typeFloat, blir_flagNull, NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test Floating point double display                                 *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Floating point double ==\n\n");
  fdouble = 1234.5678901234;
  printf ("Image = %64.32f\n", fdouble);
  blir_display ( & fdouble, sizeof (fdouble), 4, blir_typeFloat, blir_flagNull, NULL);
  printf ("    ");
  blir_display ( & fdouble, sizeof (fdouble), 3, blir_typeFloat, blir_flagNull, NULL);
  printf ("    ");
  blir_display ( & fdouble, sizeof (fdouble), 5, blir_typeFloat, blir_flagNull, NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test Floating point long double display                            *
   *                                                                    *
   *********************************************************************  */

#if 0
  printf ("\n== Testing Floating point long ==\n\n");
  flongdouble = 1234.567890123456789012 L;
  printf ("Image = %64.32Lf\n", flongdouble);
  blir_display ( & flongdouble, BLI_FLOAT_LONG_DOUBLE_WIDTH, 4, blir_typeFloat, blir_flagNull, NULL);
  printf ("    ");
  blir_display ( & flongdouble, BLI_FLOAT_LONG_DOUBLE_WIDTH, 3, blir_typeFloat, blir_flagNull, NULL);
  printf ("    ");
  blir_display ( & flongdouble, BLI_FLOAT_LONG_DOUBLE_WIDTH, 5, blir_typeFloat, blir_flagNull, NULL);
  printf ("\n");
#endif

  /**********************************************************************
   *                                                                    *
   * Test usage display output                                          *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Display ==\n\n");
  cptr = "123.456";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("\n");
  cptr = "-654.321";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay, blir_flagNull,
                NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test usage display leading overpunched sign output                 *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Display with leading overpunched sign ==\n\n");
  cptr = "A23.456";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("\n");
  cptr = "O54.321";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay,
                blir_flagSignLead | blir_flagOverpunch, NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test usage display trailing overpunched sign output                *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Display with trailing overpunched sign ==\n\n");
  cptr = "123.45F";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("\n");
  cptr = "654.32J";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay,
                blir_flagSignTrail | blir_flagOverpunch, NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test usage display leading sign output                             *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Display with leading sign ==\n\n");
  cptr = "+123.456";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("\n");
  cptr = "-654.321";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay, blir_flagSignLead,
                NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test usage display trailing sign output                            *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Display with trailing sign ==\n\n");
  cptr = "123.456+";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("\n");
  cptr = "654.321-";
  printf ("Image = %s\n", cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typeDisplay, blir_flagSignTrail,
                NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test usage packed output                                           *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Packed ==\n\n");
  printf ("Image = 0123456C\n");
  cptr = "\x01\x23\x45\x6C";
  blir_display (cptr, strlen (cptr), 0, blir_typePacked, blir_flagNull, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typePacked, blir_flagNull, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typePacked, blir_flagNull, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typePacked, blir_flagNull, NULL);
  printf ("\n");
  printf ("Image = 0654321D\n");
  cptr = "\x06\x54\x32\x1D";
  blir_display (cptr, strlen (cptr), 0, blir_typePacked, blir_flagNull, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 2, blir_typePacked, blir_flagNull, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 3, blir_typePacked, blir_flagNull, NULL);
  printf ("    ");
  blir_display (cptr, strlen (cptr), 4, blir_typePacked, blir_flagNull, NULL);
  printf ("\n");

  /**********************************************************************
   *                                                                    *
   * Test usage display with format picture                             *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Display with Picture ==\n\n");
  cptr = "-123456.78";
  pptr = "---,---,--9.99";
  printf ("Picture = \"%s\"  Type = Display  Image = %s\n", pptr, cptr);
  blir_display (cptr, strlen (cptr), 0, blir_typeDisplay, blir_flagNull,
                pptr);
  printf ("\n----+----1----+----2----+----3----+----4-----+----5\n");

  i32 = 12345678;
  pptr = "+++,+++,++9.99";
  printf ("Picture = \"%s\"  Type = Binary  Image = %d\n", pptr, i32);
  blir_display ( & i32, sizeof (i32), 0, blir_typeBinary, blir_flagNull,
                pptr);
  printf ("\n----+----1----+----2----+----3----+----4-----+----5\n");

  fshort = 12345.678;
  pptr = "$$$,$$$,$$9.99";
  printf ("Picture = \"%s\"  Type = Float  Image = %f\n", pptr, fshort);
  blir_display ( & fshort, sizeof (fshort), 0, blir_typeFloat, blir_flagNull,
                pptr);
  printf ("\n----+----1----+----2----+----3----+----4-----+----5\n");

  cptr = "\x12\x34\x56\x78\x0C";
  pptr = "+***,***,**9.99";
  printf ("Picture = \"%s\"  Type = Packed  Image = 123456780+\n", pptr);
  blir_display (cptr, strlen (cptr), 0, blir_typePacked, blir_flagNull,
                pptr);
  printf ("\n----+----1----+----2----+----3----+----4-----+----5\n");

  return 0;
}
