
/**********************************************************************
 *                                                                    *
 *              BLISS Compare Function testing mainline               *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.            *
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
 * Title:          BLISS Compare Function testing mainline            *
 *                                                                    *
 * Description:    This file is part of the BLISS Compare Function.   *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   24/Dec/2000  Initial Release of Routines T.            *
 *                          Seward   01/Jan/2001  Added use overpunch *
 *                          routines                                  *
 *                                                                    *
 *********************************************************************  */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "blir_temp_config.h"
#include "blir_globals.h"
#include "blir_compare.h"
#include "blir_overpunch.h"

#define MAX_REPLINE     34

FILE *repFile;  /* Report file.  */
int32 repLine;  /* Report line number.  */
int32 repPage;  /* Report page number.  */

static uint32 error_count = 0;

/**********************************************************************
 *                                                                    *
 * PutReportHead - Put a report header                                *
 *                                                                    *
 *********************************************************************  */


static int32
putReportHead (void)
{
  if (repPage == 0)
    fprintf (repFile, "\n\n");
  else
    fprintf (repFile, "\f\n\n");
  repPage ++;
  fprintf (repFile,
           "                                    Test Report For blir_compare()                            Page %3d\n",
           repPage);
  fprintf (repFile, "\n");
  fprintf (repFile,
           "Item 1      Item 1, 10 hex bytes  Item 2      Item 2, 10 hex bytes  Return  Length  Type      Flags   \n");
  fprintf (repFile,
           "----------  --------------------  ----------  --------------------  ------  ------  --------  --------\n");
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportLine - Prepare to put a line                              *
 *                                                                    *
 *********************************************************************  */


static int32
putReportLine (void)
{
  if (repLine > MAX_REPLINE)
    {
      putReportHead ();
      repLine = 0;
    }
  else if ((repLine % 5) == 0)
    {
      fprintf (repFile, "\n");
    }
  repLine ++;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportEnd - put an End of Report                                *
 *                                                                    *
 *********************************************************************  */


static int32
putReportEnd (void)
{
  repLine += 2;
  if (repLine > MAX_REPLINE)
    {
      putReportHead ();
    }
  fprintf (repFile, "\n");
  fprintf (repFile,
           "                                        *** End Of Report ***\n");
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutValue - Put a single item value                                 *
 *                                                                    *
 *********************************************************************  */


static int32
putValue (schar *image,  /* Image.  */
          const void *value,  /* Item pionter.  */
          BLI_SIZE_T length) /* Length of value.  */
{

  uchar *cptr;  /* Pointer to value.  */
  BLI_SIZE_T i;  /* Index.  */

  fprintf (repFile, "%10s  ", image);
  cptr = (uchar *)value;
  for (i = 1; i <= 10; i ++)
    if (i < length)
      fprintf (repFile, "%02X",  * (cptr ++));
    else if (i == length)
      fprintf (repFile, "%02X",  * cptr);
    else
      fprintf (repFile, "  ");
  fprintf (repFile, "  ");
  return 0;
}


/**********************************************************************
 *                                                                    *
 * DoTest - Do a blir_compare() test                                  *
 *                                                                    *
 *********************************************************************  */


static int32
doTest (int32 expect,  /* Value to expect.  */
        schar *image1,  /* Image of item 1.  */
        schar *image2,  /* Image of item 2.  */
        const void *item1,  /* Address of item 1.  */
        const void *item2,  /* Address of item 2.  */
        BLI_SIZE_T length,  /* Item length.  */
        uint32 type,  /* Item type blir_type enum.  */
        uint32 flags) /* Item flags also in blir_temp_config.  */
{
  int32 cmp;  /* Compare results.  */
  int32 comma;  /* Comma needed flag.  */

  putReportLine ();

  cmp = blir_compare (item1, item2, length, type, flags, NULL);

  putValue (image1, item1, length);
  putValue (image2, item2, length);

  if (cmp == expect)
    fprintf (repFile, "%2d OK   ", cmp);
  else
    {
      fprintf (repFile, "%2d ERR  ", cmp);
      error_count++;
    }
  fprintf (repFile, "%6d  ", length);

  switch (type)
    {
    case blir_typeNull :
      fprintf (repFile, "Null      ");
      break;
    case blir_typeText :
      fprintf (repFile, "Text      ");
      break;
    case blir_typeDisplay :
      fprintf (repFile, "Display   ");
      break;
    case blir_typePacked :
      fprintf (repFile, "Packed    ");
      break;
    case blir_typeBinary :
      fprintf (repFile, "Binary    ");
      break;
    case blir_typeUnsigned :
      fprintf (repFile, "Unsigned  ");
      break;
    case blir_typeFloat :
      fprintf (repFile, "Float     ");
      break;
    }

  comma = 0;

  if ((flags & blir_flagSignLead) != 0)
    {
      if (comma)
        fprintf (repFile, ", ");
      fprintf (repFile, "Lead");
      comma = 1;
    }

  if ((flags & blir_flagSignTrail) != 0)
    {
      if (comma)
        fprintf (repFile, ", ");
      fprintf (repFile, "Trail");
      comma = 1;
    }

  if ((flags & blir_flagOverpunch) != 0)
    {
      if (comma)
        fprintf (repFile, ", ");
      fprintf (repFile, "Over");
    }

  if (comma == 0)
    {
      fprintf (repFile, "Null");
    }

  fprintf (repFile, "\n");

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * TestSCHAR - Test a SCHAR type                                        *
 *                                                                    *
 *********************************************************************  */


static int32
testSCHAR (schar n1,  /* Left number of compare.  */
          schar n2,  /* Right number of compare.  */
          int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10d", n1);
  sprintf ((native_char *)image2, "%10d", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_INT8_WIDTH, blir_typeBinary,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieSCHAR - Drive a full set of SCHAR tests                          *
 *                                                                    *
 *********************************************************************  */


static int32
driveSCHAR (schar zero,  /* Zero.  */
           schar lowpos,  /* Low positive.  */
           schar lowneg,  /* Low negative.  */
           schar highpos,  /* High positive.  */
           schar highneg) /* High negative.  */
{
  testSCHAR (zero, zero, 0);

  testSCHAR (zero, lowpos, -1);
  testSCHAR (lowpos, zero, +1);
  testSCHAR (lowpos, lowpos, 0);

  testSCHAR (zero, lowneg, +1);
  testSCHAR (lowneg, zero, -1);
  testSCHAR (lowneg, lowneg, 0);

  testSCHAR (lowpos, highpos, -1);
  testSCHAR (highpos, lowpos, +1);

  testSCHAR (lowpos, highneg, +1);
  testSCHAR (highneg, lowpos, -1);

  testSCHAR (highpos, lowneg, +1);
  testSCHAR (lowneg, highpos, -1);

  testSCHAR (lowneg, highneg, +1);
  testSCHAR (highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestUCHAR - Test a UCHAR type                                      *
 *                                                                    *
 *********************************************************************  */


static int32
testUCHAR (uchar n1,  /* Left number of compare.  */
           uchar n2,  /* Right number of compare.  */
           int32 expect) /* Return code expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10u", n1);
  sprintf ((native_char *)image2, "%10u", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_UINT8_WIDTH, blir_typeUnsigned,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieUCHAR - Drive a full set of UCHAR tests                        *
 *                                                                    *
 *********************************************************************  */


static int32
driveUCHAR (uchar zero,  /* Zero.  */
            uchar lowpos,  /* Low positive.  */
            uchar highpos) /* High positive.  */
{
  testUCHAR (zero, zero, 0);

  testUCHAR (zero, lowpos, -1);
  testUCHAR (lowpos, zero, +1);
  testUCHAR (lowpos, lowpos, 0);

  testUCHAR (zero, highpos, -1);
  testUCHAR (highpos, zero, +1);
  testUCHAR (highpos, highpos, 0);

  testUCHAR (lowpos, highpos, -1);
  testUCHAR (highpos, lowpos, +1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestINT16 - Test a int16 type                                      *
 *                                                                    *
 *********************************************************************  */


static int32
testINT16 (int16 n1,  /* Left number of compare.  */
           int16 n2,  /* Right number of compare.  */
           int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10d", n1);
  sprintf ((native_char *)image2, "%10d", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_INT16_WIDTH, blir_typeBinary,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieINT16 - Drive a full set of INT16 tests                        *
 *                                                                    *
 *********************************************************************  */


static int32
driveINT16 (int16 zero,  /* Zero.  */
            int16 lowpos,  /* Low positive.  */
            int16 lowneg,  /* Low negative.  */
            int16 highpos,  /* High positive.  */
            int16 highneg) /* High negative.  */
{
  testINT16 (zero, zero, 0);

  testINT16 (zero, lowpos, -1);
  testINT16 (lowpos, zero, +1);
  testINT16 (lowpos, lowpos, 0);

  testINT16 (zero, lowneg, +1);
  testINT16 (lowneg, zero, -1);
  testINT16 (lowneg, lowneg, 0);

  testINT16 (lowpos, highpos, -1);
  testINT16 (highpos, lowpos, +1);

  testINT16 (lowpos, highneg, +1);
  testINT16 (highneg, lowpos, -1);

  testINT16 (highpos, lowneg, +1);
  testINT16 (lowneg, highpos, -1);

  testINT16 (lowneg, highneg, +1);
  testINT16 (highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestUINT16 - Test a UINT16 type                                    *
 *                                                                    *
 *********************************************************************  */


static int32
testUINT16 (uint16 n1,  /* Left number of compare.  */
            uint16 n2,  /* Right number of compare.  */
            int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10u", n1);
  sprintf ((native_char *)image2, "%10u", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_UINT16_WIDTH, blir_typeUnsigned,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieUINT16 - Drive a full set of uint16 tests                      *
 *                                                                    *
 *********************************************************************  */


static int32
driveUINT16 (uint16 zero,  /* Zero.  */
             uint16 lowpos,  /* Low positive.  */
             uint16 highpos) /* High positive.  */
{
  testUINT16 (zero, zero, 0);

  testUINT16 (zero, lowpos, -1);
  testUINT16 (lowpos, zero, +1);
  testUINT16 (lowpos, lowpos, 0);

  testUINT16 (zero, highpos, -1);
  testUINT16 (highpos, zero, +1);
  testUINT16 (highpos, highpos, 0);

  testUINT16 (lowpos, highpos, -1);
  testUINT16 (highpos, lowpos, +1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestINT32 - Test a INT32 type                                      *
 *                                                                    *
 *********************************************************************  */


static int32
testINT32 (int32 n1,  /* Left number of compare.  */
           int32 n2,  /* Right number of compare.  */
           int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10d", n1);
  sprintf ((native_char *)image2, "%10d", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_INT32_WIDTH, blir_typeBinary,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieINT32 - Drive a full set of INT32 tests                        *
 *                                                                    *
 *********************************************************************  */


static int32
driveINT32 (int32 zero,  /* Zero.  */
            int32 lowpos,  /* Low positive.  */
            int32 lowneg,  /* Low negative.  */
            int32 highpos,  /* High positive.  */
            int32 highneg) /* High negative.  */
{
  testINT32 (zero, zero, 0);

  testINT32 (zero, lowpos, -1);
  testINT32 (lowpos, zero, +1);
  testINT32 (lowpos, lowpos, 0);

  testINT32 (zero, lowneg, +1);
  testINT32 (lowneg, zero, -1);
  testINT32 (lowneg, lowneg, 0);

  testINT32 (lowpos, highpos, -1);
  testINT32 (highpos, lowpos, +1);

  testINT32 (lowpos, highneg, +1);
  testINT32 (highneg, lowpos, -1);

  testINT32 (highpos, lowneg, +1);
  testINT32 (lowneg, highpos, -1);

  testINT32 (lowneg, highneg, +1);
  testINT32 (highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestUINT32 - Test a uint32 type                                    *
 *                                                                    *
 *********************************************************************  */


static int32
testUINT32 (uint32 n1,  /* Left number of compare.  */
            uint32 n2,  /* Right number of compare.  */
            int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10u", n1);
  sprintf ((native_char *)image2, "%10u", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_UINT32_WIDTH, blir_typeUnsigned,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieUINT32 - Drive a full set of uint32 tests                      *
 *                                                                    *
 *********************************************************************  */


static int32
driveUINT32 (uint32 zero,  /* Zero.  */
             uint32 lowpos,  /* Low positive.  */
             uint32 highpos) /* High positive.  */
{
  testUINT32 (zero, zero, 0);

  testUINT32 (zero, lowpos, -1);
  testUINT32 (lowpos, zero, +1);
  testUINT32 (lowpos, lowpos, 0);

  testUINT32 (zero, highpos, -1);
  testUINT32 (highpos, zero, +1);
  testUINT32 (highpos, highpos, 0);

  testUINT32 (lowpos, highpos, -1);
  testUINT32 (highpos, lowpos, +1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestINT64 - Test a INT64 type                                      *
 *                                                                    *
 *********************************************************************  */


static int32
testINT64 (int64 n1,  /* Left number of compare.  */
           int64 n2,  /* Right number of compare.  */
           int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10lld", n1);
  sprintf ((native_char *)image2, "%10lld", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_INT64_WIDTH, blir_typeBinary,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveINT64 - Drive a full set of INT64 tests                       *
 *                                                                    *
 *********************************************************************  */


static int32
driveINT64 (int64 zero,  /* Zero.  */
            int64 lowpos,  /* Low positive.  */
            int64 lowneg,  /* Low negative.  */
            int64 highpos,  /* High positive.  */
            int64 highneg) /* High negative.  */
{
  testINT64 (zero, zero, 0);

  testINT64 (zero, lowpos, -1);
  testINT64 (lowpos, zero, +1);
  testINT64 (lowpos, lowpos, 0);

  testINT64 (zero, lowneg, +1);
  testINT64 (lowneg, zero, -1);
  testINT64 (lowneg, lowneg, 0);

  testINT64 (lowpos, highpos, -1);
  testINT64 (highpos, lowpos, +1);

  testINT64 (lowpos, highneg, +1);
  testINT64 (highneg, lowpos, -1);

  testINT64 (highpos, lowneg, +1);
  testINT64 (lowneg, highpos, -1);

  testINT64 (lowneg, highneg, +1);
  testINT64 (highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestUINT64 - Test a uint64 type                                    *
 *                                                                    *
 *********************************************************************  */


static int32
testUINT64 (uint64 n1,  /* Left number of compare.  */
            uint64 n2,  /* Right number of compare.  */
            int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10llu", n1);
  sprintf ((native_char *)image2, "%10llu", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_UINT64_WIDTH, blir_typeUnsigned,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrieUINT64 - Drive a full set of uint64 tests                      *
 *                                                                    *
 *********************************************************************  */


static int32
driveUINT64 (uint64 zero,  /* Zero.  */
             uint64 lowpos,  /* Low positive.  */
             uint64 highpos) /* High positive.  */
{
  testUINT64 (zero, zero, 0);

  testUINT64 (zero, lowpos, -1);
  testUINT64 (lowpos, zero, +1);
  testUINT64 (lowpos, lowpos, 0);

  testUINT64 (zero, highpos, -1);
  testUINT64 (highpos, zero, +1);
  testUINT64 (highpos, highpos, 0);

  testUINT64 (lowpos, highpos, -1);
  testUINT64 (highpos, lowpos, +1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestFloatShort - Test a FLOAT_SHORT type                           *
 *                                                                    *
 *********************************************************************  */


static int32
testFloatShort (bli_float_32 n1,  /* Left number of compare.  */
                bli_float_32 n2,  /* Right number of compare.  */
                int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10.2f", n1);
  sprintf ((native_char *)image2, "%10.2f", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_FLOAT_32_WIDTH,
            blir_typeFloat, blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveFloatShort - Drive a full set of FLOAT_SHORT tests            *
 *                                                                    *
 *********************************************************************  */


static int32
driveFloatShort (bli_float_32 zero,  /* Zero.  */
                 bli_float_32 lowpos,  /* Low positive.  */
                 bli_float_32 lowneg,  /* Low negative.  */
                 bli_float_32 highpos,  /* High positive.  */
                 bli_float_32 highneg) /* High negative.  */
{
  testFloatShort (zero, zero, 0);

  testFloatShort (zero, lowpos, -1);
  testFloatShort (lowpos, zero, +1);
  testFloatShort (lowpos, lowpos, 0);

  testFloatShort (zero, lowneg, +1);
  testFloatShort (lowneg, zero, -1);
  testFloatShort (lowneg, lowneg, 0);

  testFloatShort (lowpos, highpos, -1);
  testFloatShort (highpos, lowpos, +1);

  testFloatShort (lowpos, highneg, +1);
  testFloatShort (highneg, lowpos, -1);

  testFloatShort (highpos, lowneg, +1);
  testFloatShort (lowneg, highpos, -1);

  testFloatShort (lowneg, highneg, +1);
  testFloatShort (highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestFloatDouble - Test a FLOAT_DOUBLE type                         *
 *                                                                    *
 *********************************************************************  */


static int32
testFloatDouble (bli_float_64 n1,  /* Left number of compare.  */
                 bli_float_64 n2,  /* Right number of compare.  */
                 int32 expect) /* Return code to expect.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */

  sprintf ((native_char *)image1, "%10.2f", n1);
  sprintf ((native_char *)image2, "%10.2f", n2);
  cmp =
    doTest (expect, image1, image2,  & n1,  & n2, BLI_FLOAT_64_WIDTH,
            blir_typeFloat, blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveFloatDouble - Drive a full set of FLOAT_DOUBLE tests          *
 *                                                                    *
 *********************************************************************  */


static int32
driveFloatDouble (bli_float_64 zero,  /* Zero.  */
                  bli_float_64 lowpos,  /* Low positive.  */
                  bli_float_64 lowneg,  /* Low negative.  */
                  bli_float_64 highpos,  /* High positive.  */
                  bli_float_64 highneg) /* High negative.  */
{
  testFloatDouble (zero, zero, 0);

  testFloatDouble (zero, lowpos, -1);
  testFloatDouble (lowpos, zero, +1);
  testFloatDouble (lowpos, lowpos, 0);

  testFloatDouble (zero, lowneg, +1);
  testFloatDouble (lowneg, zero, -1);
  testFloatDouble (lowneg, lowneg, 0);

  testFloatDouble (lowpos, highpos, -1);
  testFloatDouble (highpos, lowpos, +1);

  testFloatDouble (lowpos, highneg, +1);
  testFloatDouble (highneg, lowpos, -1);

  testFloatDouble (highpos, lowneg, +1);
  testFloatDouble (lowneg, highpos, -1);

  testFloatDouble (lowneg, highneg, +1);
  testFloatDouble (highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestPacked - Test a Packed type                                    *
 *                                                                    *
 *********************************************************************  */


static int32
testPacked (BLI_SIZE_T length,  /* Length of field.  */
            const native_char *n1,  /* Item 1.  */
            const native_char *n2,  /* Item 2.  */
            int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  schar work[10];  /* Work area.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */

  cptr = (uchar *)n1;
  strcpy ((native_char *)image1, "");
  for (i = 1; i < length; i ++)
    {
      sprintf ((native_char *)work, "%02X",  * (cptr ++));
      strcat ((native_char *)image1, (native_char *)work);
    }
  sprintf ((native_char *)work, "%02X",  * cptr);
  if (work[1] == 'D')
    work[1] = '-';
  else
    work[1] = '+';
  strcat ((native_char *)image1, (native_char *)work);

  cptr = (uchar *)n2;
  strcpy ((native_char *)image2, "");
  for (i = 1; i < length; i ++)
    {
      sprintf ((native_char *)work, "%02X",  * (cptr ++));
      strcat ((native_char *)image2, (native_char *)work);
    }
  sprintf ((native_char *)work, "%02X",  * cptr);
  if (work[1] == 'D')
    work[1] = '-';
  else
    work[1] = '+';
  strcat ((native_char *)image2, (native_char *)work);

  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typePacked,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DrivePacked - Drive a full set of Packed tests                     *
 *                                                                    *
 *********************************************************************  */


static int32
drivePacked (BLI_SIZE_T length,  /* Length.  */
             const native_char *zero,  /* Zero.  */
             const native_char *lowpos,  /* Low positive.  */
             const native_char *lowneg,  /* Low negative.  */
             const native_char *highpos,  /* High positive.  */
             const native_char *highneg) /* High negative.  */
{
  testPacked (length, zero, zero, 0);

  testPacked (length, zero, lowpos, -1);
  testPacked (length, lowpos, zero, +1);
  testPacked (length, lowpos, lowpos, 0);

  testPacked (length, zero, lowneg, +1);
  testPacked (length, lowneg, zero, -1);
  testPacked (length, lowneg, lowneg, 0);

  testPacked (length, lowpos, highpos, -1);
  testPacked (length, highpos, lowpos, +1);

  testPacked (length, lowpos, highneg, +1);
  testPacked (length, highneg, lowpos, -1);

  testPacked (length, highpos, lowneg, +1);
  testPacked (length, lowneg, highpos, -1);

  testPacked (length, lowneg, highneg, +1);
  testPacked (length, highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDispLeadSep - Test a Display Lead Sign Separate type           *
 *                                                                    *
 *********************************************************************  */


static int32
testDispLeadSep (BLI_SIZE_T length,  /* Length of field.  */
                 const native_char *n1,  /* Item 1.  */
                 const native_char *n2,  /* Item 2.  */
                 int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */

  cptr = (uchar *)n1;
  for (i = 0; i < length; i ++)
    {
      image1[i] = cptr[i];
    }
  image1[length] = '\0';

  cptr = (uchar *)n2;
  for (i = 0; i < length; i ++)
    {
      image2[i] = cptr[i];
    }
  image2[length] = '\0';

  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typeDisplay,
            blir_flagNull | blir_flagSignLead);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveDispLeadSep - Drive a full set of Display Lead Sign Separate  *
 * tests                                                              *
 *                                                                    *
 *********************************************************************  */


static int32
driveDispLeadSep (BLI_SIZE_T length,  /* Length.  */
                  const native_char *zero,  /* Zero.  */
                  const native_char *lowpos,  /* Low positive.  */
                  const native_char *lowneg,  /* Low negative.  */
                  const native_char *highpos,  /* High positive.  */
                  const native_char *highneg) /* High negative.  */
{
  testDispLeadSep (length, zero, zero, 0);

  testDispLeadSep (length, zero, lowpos, -1);
  testDispLeadSep (length, lowpos, zero, +1);
  testDispLeadSep (length, lowpos, lowpos, 0);

  testDispLeadSep (length, zero, lowneg, +1);
  testDispLeadSep (length, lowneg, zero, -1);
  testDispLeadSep (length, lowneg, lowneg, 0);

  testDispLeadSep (length, lowpos, highpos, -1);
  testDispLeadSep (length, highpos, lowpos, +1);

  testDispLeadSep (length, lowpos, highneg, +1);
  testDispLeadSep (length, highneg, lowpos, -1);

  testDispLeadSep (length, highpos, lowneg, +1);
  testDispLeadSep (length, lowneg, highpos, -1);

  testDispLeadSep (length, lowneg, highneg, +1);
  testDispLeadSep (length, highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDispLeadOver - Test a Display Lead Sign Separate type          *
 *                                                                    *
 *********************************************************************  */


static int32
testDispLeadOver (BLI_SIZE_T length,  /* Length of field.  */
                  const native_char *n1,  /* Item 1.  */
                  const native_char *n2,  /* Item 2.  */
                  int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */
  int32 sign;  /* Overpunch sign.  */
  int32 byte;  /* Input byte.  */

  cptr = (uchar *)n1;

  blir_overpunch_get ( & sign,  & byte,  * cptr);

  if (sign > 0)
    image1[0] = '+';
  else
    image1[0] = '-';
  image1[1] = (char)byte;

  for (i = 1; i < length; i ++)
    {
      image1[i + 1] = cptr[i];
    }
  image1[length + 1] = '\0';

  cptr = (uchar *)n2;

  blir_overpunch_get ( & sign,  & byte,  * cptr);

  if (sign > 0)
    image2[0] = '+';
  else
    image2[0] = '-';
  image2[1] = (char)byte;

  for (i = 1; i < length; i ++)
    {
      image2[i + 1] = cptr[i];
    }
  image2[length + 1] = '\0';

  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typeDisplay,
            blir_flagNull | blir_flagSignLead | blir_flagOverpunch);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveDispLeadOver - Drive a full set of Display Lead Sign Separate *
 * tests                                                              *
 *                                                                    *
 *********************************************************************  */


static int32
driveDispLeadOver (BLI_SIZE_T length,  /* Length.  */
                   const native_char *zero,  /* Zero.  */
                   const native_char *lowpos,  /* Low positive.  */
                   const native_char *lowneg,  /* Low negative.  */
                   const native_char *highpos,  /* High positive.  */
                   const native_char *highneg) /* High negative.  */
{
  testDispLeadOver (length, zero, zero, 0);

  testDispLeadOver (length, zero, lowpos, -1);
  testDispLeadOver (length, lowpos, zero, +1);
  testDispLeadOver (length, lowpos, lowpos, 0);

  testDispLeadOver (length, zero, lowneg, +1);
  testDispLeadOver (length, lowneg, zero, -1);
  testDispLeadOver (length, lowneg, lowneg, 0);

  testDispLeadOver (length, lowpos, highpos, -1);
  testDispLeadOver (length, highpos, lowpos, +1);

  testDispLeadOver (length, lowpos, highneg, +1);
  testDispLeadOver (length, highneg, lowpos, -1);

  testDispLeadOver (length, highpos, lowneg, +1);
  testDispLeadOver (length, lowneg, highpos, -1);

  testDispLeadOver (length, lowneg, highneg, +1);
  testDispLeadOver (length, highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDispTrailSep - Test a Display Trail Sign Separate type         *
 *                                                                    *
 *********************************************************************  */


static int32
testDispTrailSep (BLI_SIZE_T length,  /* Length of field.  */
                  const native_char *n1,  /* Item 1.  */
                  const native_char *n2,  /* Item 2.  */
                  int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */

  cptr = (uchar *)n1;
  for (i = 0; i < length; i ++)
    {
      image1[i] = cptr[i];
    }
  image1[length] = '\0';

  cptr = (uchar *)n2;
  for (i = 0; i < length; i ++)
    {
      image2[i] = cptr[i];
    }
  image2[length] = '\0';

  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typeDisplay,
            blir_flagNull | blir_flagSignTrail);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveDispTrailSep - Drive a full set of Display Trail Sign         *
 * Separate tests                                                     *
 *                                                                    *
 *********************************************************************  */


static int32
driveDispTrailSep (BLI_SIZE_T length,  /* Length.  */
                   const native_char *zero,  /* Zero.  */
                   const native_char *lowpos,  /* Low positive.  */
                   const native_char *lowneg,  /* Low negative.  */
                   const native_char *highpos,  /* High positive.  */
                   const native_char *highneg) /* High negative.  */
{
  testDispTrailSep (length, zero, zero, 0);

  testDispTrailSep (length, zero, lowpos, -1);
  testDispTrailSep (length, lowpos, zero, +1);
  testDispTrailSep (length, lowpos, lowpos, 0);

  testDispTrailSep (length, zero, lowneg, +1);
  testDispTrailSep (length, lowneg, zero, -1);
  testDispTrailSep (length, lowneg, lowneg, 0);

  testDispTrailSep (length, lowpos, highpos, -1);
  testDispTrailSep (length, highpos, lowpos, +1);

  testDispTrailSep (length, lowpos, highneg, +1);
  testDispTrailSep (length, highneg, lowpos, -1);

  testDispTrailSep (length, highpos, lowneg, +1);
  testDispTrailSep (length, lowneg, highpos, -1);

  testDispTrailSep (length, lowneg, highneg, +1);
  testDispTrailSep (length, highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDispTrailOver - Test a Display Trail Sign Separate type        *
 *                                                                    *
 *********************************************************************  */


static int32
testDispTrailOver (BLI_SIZE_T length,  /* Length of field.  */
                   const native_char *n1,  /* Item 1.  */
                   const native_char *n2,  /* Item 2.  */
                   int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */
  int32 sign;  /* Overpunch sign.  */
  int32 byte;  /* Input byte.  */

  cptr = (uchar *)n1;

  for (i = 0; i < (length - 1); i ++)
    {
      image1[i] = cptr[i];
    }

  blir_overpunch_get ( & sign,  & byte,  * (cptr + length - 1));

  image1[length - 1] = (char)byte;
  if (sign > 0)
    image1[length] = '+';
  else
    image1[length] = '-';
  image1[length + 1] = '\0';


  cptr = (uchar *)n2;

  for (i = 0; i < (length - 1); i ++)
    {
      image2[i] = cptr[i];
    }

  blir_overpunch_get ( & sign,  & byte,  * (cptr + length - 1));

  image2[length - 1] = (char)byte;
  if (sign > 0)
    image2[length] = '+';
  else
    image2[length] = '-';
  image2[length + 1] = '\0';


  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typeDisplay,
            blir_flagNull | blir_flagSignTrail | blir_flagOverpunch);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveDispTrailOver - Drive a full set of Display Trail Sign        *
 * Separate tests                                                     *
 *                                                                    *
 *********************************************************************  */


static int32
driveDispTrailOver (BLI_SIZE_T length,  /* Length.  */
                    const native_char *zero,  /* Zero.  */
                    const native_char *lowpos,  /* Low positive.  */
                    const native_char *lowneg,  /* Low negative.  */
                    const native_char *highpos,  /* High positive.  */
                    const native_char *highneg) /* High negative.  */
{
  testDispTrailOver (length, zero, zero, 0);

  testDispTrailOver (length, zero, lowpos, -1);
  testDispTrailOver (length, lowpos, zero, +1);
  testDispTrailOver (length, lowpos, lowpos, 0);

  testDispTrailOver (length, zero, lowneg, +1);
  testDispTrailOver (length, lowneg, zero, -1);
  testDispTrailOver (length, lowneg, lowneg, 0);

  testDispTrailOver (length, lowpos, highpos, -1);
  testDispTrailOver (length, highpos, lowpos, +1);

  testDispTrailOver (length, lowpos, highneg, +1);
  testDispTrailOver (length, highneg, lowpos, -1);

  testDispTrailOver (length, highpos, lowneg, +1);
  testDispTrailOver (length, lowneg, highpos, -1);

  testDispTrailOver (length, lowneg, highneg, +1);
  testDispTrailOver (length, highneg, lowneg, -1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDisplay - Test a Display type                                  *
 *                                                                    *
 *********************************************************************  */


static int32
testDisplay (BLI_SIZE_T length,  /* Length of field.  */
             const native_char *n1,  /* Item 1.  */
             const native_char *n2,  /* Item 2.  */
             int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */

  cptr = (uchar *)n1;
  for (i = 0; i < length; i ++)
    {
      image1[i] = cptr[i];
    }
  image1[length] = '\0';

  cptr = (uchar *)n2;
  for (i = 0; i < length; i ++)
    {
      image2[i] = cptr[i];
    }
  image2[length] = '\0';

  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typeDisplay,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveDisplay - Drive a full set of Display tests                   *
 *                                                                    *
 *********************************************************************  */


static int32
driveDisplay (BLI_SIZE_T length,  /* Length.  */
              const native_char *zero,  /* Zero.  */
              const native_char *lowpos,  /* Low positive.  */
              const native_char *highpos) /* High positive.  */
{
  testDisplay (length, zero, zero, 0);

  testDisplay (length, zero, lowpos, -1);
  testDisplay (length, lowpos, zero, +1);
  testDisplay (length, lowpos, lowpos, 0);

  testDisplay (length, zero, highpos, -1);
  testDisplay (length, highpos, zero, +1);
  testDisplay (length, highpos, highpos, 0);

  testDisplay (length, lowpos, highpos, -1);
  testDisplay (length, highpos, lowpos, +1);

  return 0;
}



/**********************************************************************
 *                                                                    *
 * TestText - Test a Text type                                        *
 *                                                                    *
 *********************************************************************  */


static int32
testText (BLI_SIZE_T length,  /* Length of field.  */
          const native_char *n1,  /* Item 1.  */
          const native_char *n2,  /* Item 2.  */
          int32 expect) /* Value expected.  */
{
  schar image1[64];  /* Image of first item.  */
  schar image2[64];  /* Image of second item.  */
  int32 cmp;  /* Compare results.  */
  uchar *cptr;  /* Unsigned character pointer.  */
  BLI_SIZE_T i;  /* Index.  */

  cptr = (uchar *)n1;
  for (i = 0; i < length; i ++)
    {
      image1[i] = cptr[i];
    }
  image1[length] = '\0';

  cptr = (uchar *)n2;
  for (i = 0; i < length; i ++)
    {
      image2[i] = cptr[i];
    }
  image2[length] = '\0';

  cmp =
    doTest (expect, image1, image2, n1, n2, length, blir_typeText,
            blir_flagNull);

  return cmp;
}


/**********************************************************************
 *                                                                    *
 * DriveText - Drive a full set of Text tests                         *
 *                                                                    *
 *********************************************************************  */


static int32
driveText (BLI_SIZE_T length,  /* Length.  */
           const native_char *spaces,  /* Spaces.  */
           const native_char *lowvalue,  /* Low positive.  */
           const native_char *highvalue) /* High positive.  */
{
  testText (length, spaces, spaces, 0);

  testText (length, spaces, lowvalue, -1);
  testText (length, lowvalue, spaces, +1);
  testText (length, lowvalue, lowvalue, 0);

  testText (length, spaces, highvalue, -1);
  testText (length, highvalue, spaces, +1);
  testText (length, highvalue, highvalue, 0);

  testText (length, lowvalue, highvalue, -1);
  testText (length, highvalue, lowvalue, +1);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * Main - Program Mainline                                            *
 *                                                                    *
 *********************************************************************  */


int
main (void)
{

  repFile = stdout;

  repPage = 0;
  repLine = MAX_REPLINE + 1;

  driveText (4, "    ", "AAAA", "AAAB");
  driveText (4, "    ", "ABBB", "BBBB");

  driveSCHAR (0, +10, -10, +20, -20);
  driveUCHAR (0, 10, 20);

  driveINT16 (0, +10, -10, +20, -20);
  driveUINT16 (0, 10, 20);

  driveINT32 (0, +10, -10, +20, -20);
  driveUINT32 (0, 10, 20);

  driveINT64 (0, +10, -10, +20, -20);
  driveUINT64 (0, 10, 20);

  driveFloatShort (0.0, +10.0, -10.0, +20.0, -20.0);

  driveFloatDouble (0.0, +10.0, -10.0, +20.0, -20.0);

  drivePacked (3, "\x00\x00\x0C", "\x99\x99\x8C", "\x99\x99\x8D",
               "\x99\x99\x9C", "\x99\x99\x9D");

  drivePacked (3, "\x00\x00\x0C", "\x89\x99\x9C", "\x89\x99\x9D",
               "\x99\x99\x9C", "\x99\x99\x9D");

  driveDisplay (5, "00000", "99998", "99999");

  driveDisplay (5, "00000", "89999", "99999");

  driveDispLeadSep (6, "+00000", "+99998", "-99998", "+99999", "-99999");

  driveDispLeadSep (6, "+00000", "+89999", "-89999", "+99999", "-99999");

  driveDispTrailSep (6, "00000+", "99998+", "99998-", "99999+", "99999-");

  driveDispTrailSep (6, "00000+", "89999+", "89999-", "99999+", "99999-");

  driveDispLeadOver (5, "{0000", "I9998", "R9998", "I9999", "R9999");

  driveDispLeadOver (5, "{0000", "H9999", "Q9999", "I9999", "R9999");

  driveDispTrailOver (5, "0000{", "9999H", "9999Q", "9999I", "9999R");

  driveDispTrailOver (5, "0000{", "8999I", "8999R", "9999I", "9999R");

  putReportEnd ();

  if (repFile != stdout)
    fclose (repFile);

  return 0;
}
