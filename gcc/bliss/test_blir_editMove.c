/*                                                                    *
 *               BLISS Numeric Edited Moves Test Driver               *
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
 * Mail:   Theodore J. Seward, Jr.                                    *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *  */


/**********************************************************************
 *                                                                    *
 * Title:  BLISS Numeric Edited Moves Test Driver                     *
 *                                                                    *
 * Description:    These routines provide the logic needed to         *
 *                 carryout a BLISS Numeric Edited Move.              *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward       19/Aug/2000     Initial Release of Routines        *
 *                                                                    *
 *********************************************************************  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "blir_temp_config.h"
#include "blir_globals.h"
#include "blir_editMove.h"

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */

int32 putColumns (int32 width);  /* Width of column line.  */

int32 testSchar (const uchar *outPic,  /* Output Picture.  */
                    const uchar *inPic,  /* Input Picture.  */
                    schar inValue);  /* Input Number.  */

int32 testUchar (const uchar *outPic,  /* Output Picture.  */
                     const uchar *inPic,  /* Input Picture.  */
                     uchar inValue);  /* Input Number.  */

int32 testInt16 (const uchar *outPic,  /* Output Picture.  */
                     const uchar *inPic,  /* Input Picture.  */
                     int16 inValue);  /* Input Number.  */

int32 testUint16 (const uchar *outPic,  /* Output Picture.  */
                      const uchar *inPic,  /* Input Picture.  */
                      uint16 inValue);  /* Input Number.  */

int32 testInt32 (const uchar *outPic,  /* Output Picture.  */
                     const uchar *inPic,  /* Input Picture.  */
                     int32 inValue);  /* Input Number.  */

int32 testUint32 (const uchar *outPic,  /* Output Picture.  */
                      const uchar *inPic,  /* Input Picture.  */
                      uint32 inValue);  /* Input Number.  */

int32 testSingle (const uchar *outPic,  /* Output Picture.  */
                      const uchar *inPic,  /* Input Picture.  */
                      float inValue);  /* Input Number.  */

int32 testDouble (const uchar *outPic,  /* Output Picture.  */
                      const uchar *inPic,  /* Input Picture.  */
                      double inValue);  /* Input Number.  */

int32 testPacked (const uchar *outPic,  /* Output Picture.  */
                      const uchar *inPic,  /* Input Picture.  */
                      const void *inValue,  /* Input Number.  */
                      int32 inLen);  /* Input Length.  */

int32 testDisplay (const uchar *outPic,  /* Output Picture.  */
                       const uchar *inPic,  /* Input Picture.  */
                       const void *inValue,  /* Input Number.  */
                       int32 inLen);  /* Input Length.  */

int32 testDispSignLeadSep (const uchar *outPic,  /* Output Picture.  */
                               const uchar *inPic,  /* Input Picture.  */
                               const void *inValue,  /* Input Number.  */
                               int32 inLen);  /* Input Length.  */

int32 testDispSignTrailSep (const uchar *outPic,  /* Output Picture.  */
                                const uchar *inPic,  /* Input Picture.  */
                                const void *inValue,  /* Input Number.  */
                                int32 inLen);  /* Input Length.  */

int32 testDispSignTrail (const uchar *outPic,  /* Output Picture.  */
                             const uchar *inPic,  /* Input Picture.  */
                             const void *inValue,  /* Input Number.  */
                             int32 inLen);  /* Input Length.  */

int32 testDispSignLead (const uchar *outPic,  /* Output Picture.  */
                            const uchar *inPic,  /* Input Picture.  */
                            const void *inValue,  /* Input Number.  */
                            int32 inLen);  /* Input Length.  */

int32 testScharOut (const uchar *outPic,  /* Output Picture.  */
                       const uchar *inPic,  /* Input Picture.  */
                       schar inValue);  /* Input Number.  */

int32 testInt16Out (const uchar *outPic,  /* Output Picture.  */
                        const uchar *inPic,  /* Input Picture.  */
                        int16 inValue);  /* Input Number.  */

int32 testInt32Out (const uchar *outPic,  /* Output Picture.  */
                        const uchar *inPic,  /* Input Picture.  */
                        int32 inValue);  /* Input Number.  */

int32 testUcharOut (const uchar *outPic, /* Output Picture.  */
                        const uchar *inPic,  /* Input Picture.  */
                        uchar inValue);  /* Input Number.  */

int32 testUInt16Out (const uchar *outPic, /* Output Picture.  */
                         const uchar *inPic,  /* Input Picture.  */
                         uint16 inValue);  /* Input Number.  */

int32 testUInt32Out (const uchar *outPic, /* Output Picture.  */
                         const uchar *inPic,  /* Input Picture.  */
                         uint32 inValue);  /* Input Number.  */

int32 testPackedOut (const uchar *outPic,  /* Output Picture.  */
                        const uchar *inPic,  /* Input Picture.  */
                        const void *inValue,  /* Input Number.  */
                        int32 inLen);  /* Input Length.  */

int32 testSingleOut (const uchar *outPic, /* Output Picture.  */
                         const uchar *inPic,  /* Input Picture.  */
                         float inValue);  /* Input Number.  */

int32 testDoubleOut (const uchar *outPic,  /* Output Picture.  */
                         const uchar *inPic,  /* Input Picture.  */
                         double inValue);  /* Input Number.  */
int
testUnedit (const uchar *sourcePic,  /* Source Picture used to edit.  */
            const void *source,  /* Source Value.  */
            int32 sourceLen,  /* Source Length.  */
            int32 sourceType,  /* Source Type.  */
            int32 sourceFlags);  /* Source Flags.  */

int32 testUneditMove (const uchar *outPic,  /* Output Picture.  */
                          const uchar *inPic,  /* Input Picture.  */
                          const void *inValue);  /* Input Number.  */

int
doTestAll (void);

int
doTest (int32 testNum,  /* Test number.  */
        const void *value,  /* Source Value.  */
        int32 width,  /* Source Width.  */
        int32 idxFrom,  /* Table index for from entry.  */
        int32 idxTo);  /* Table index for to entry.  */

int
doTestValue (int32 type,  /* Field Type.  */
            const void *value,  /* Value to print.  */
            int32 size);  /* Size of value.  */

/**********************************************************************
 *                                                                    *
 * TestType - structure for a single test entry                       *
 *                                                                    *
 *********************************************************************  */

typedef struct
{
  int32 ttType;  /* Usage Type.  */
  const uchar *ttName;  /* Type name.  */
  int32 ttFlags;  /* Flags.  */
  const uchar *ttFlagName;  /* Flag name.  */
  int32 ttLength;  /* Length.  */
  const uchar *ttPicture;  /* Associated Picture.  */
  const void *ttMinValue;  /* Minimum Value.  */
  int32 ttMinWidth;  /* Minimum Value width.  */
  const void *ttAvgValue;  /* Average Value.  */
  int32 ttAvgWidth;  /* Average Value Width.  */
  const void *ttMaxValue;  /* Maximum Value.  */
  int32 ttMaxWidth;  /* Maximum Value Width.  */
}
testType;

/**********************************************************************
 *                                                                    *
 * Value & Pictures for all the tests                                 *
 *                                                                    *
 *********************************************************************  */

const uchar valDisplayPic[] = "+999999999";
const uchar valDisplayMin[] = "-999999999";
const uchar valDisplayAvg[] = "+000000000";
const uchar valDisplayMax[] = "+999999999";

const uchar valDispSLPic[] = "S999999999";
const uchar valDispSLMin[] = "I99999999";
const uchar valDispSLAvg[] = "{00000000";
const uchar valDispSLMax[] = "R99999999";

const uchar valDispSLSPic[] = "S999999999";
const uchar valDispSLSMin[] = "-999999999";
const uchar valDispSLSAvg[] = "+000000000";
const uchar valDispSLSMax[] = "+999999999";

const uchar valDispSTPic[] = "S999999999";
const uchar valDispSTMin[] = "99999999I";
const uchar valDispSTAvg[] = "00000000{";
const uchar valDispSTMax[] = "99999999R";

const uchar valDispSTSPic[] = "S999999999";
const uchar valDispSTSMin[] = "999999999-";
const uchar valDispSTSAvg[] = "000000000+";
const uchar valDispSTSMax[] = "999999999+";

const uchar valPackedPic[] = "+999999999";
const uchar valPackedMin[] = "\x99\x99\x99\x99\x9D";
const uchar valPackedAvg[] = "\x00\x00\x00\x00\x0C";
const uchar valPackedMax[] = "\x99\x99\x99\x99\x9C";

const uchar valscharPic[] = "+99";
schar valscharMin = -99;
schar valscharAvg = 0;
schar valscharMax = +99;

const uchar valint16Pic[] = "+9999";
int16 valint16Min = -9999;
int16 valint16Avg = 0;
int16 valint16Max = +9999;

const uchar valint32Pic[] = "+999999999";
int32 valint32Min = -999999999;
int32 valint32Avg = 0;
int32 valint32Max = +999999999;

const uchar valint64Pic[] = "+999999999999999999";
int64 valint64Min = -999999999999999999LL;
int64 valint64Avg = 0L;
int64 valint64Max = +999999999999999999LL;
const uchar valucharPic[] = "99";
uchar valucharMin = 0u;
uchar valucharAvg = 50u;
uchar valucharMax = 99u;

const uchar valuint16Pic[] = "9999";
uint16 valuint16Min = 0u;
uint16 valuint16Avg = 5000u;
uint16 valuint16Max = 9999u;

const uchar valuint32Pic[] = "999999999";
uint32 valuint32Min = 0u;
uint32 valuint32Avg = 500000000u;
uint32 valuint32Max = 999999999u;

const uchar valuint64Pic[] = "999999999999999999";
uint64 valuint64Min = 0L;
uint64 valuint64Avg = 500000000000000000LL;
uint64 valuint64Max = 999999999999999999LL;
const uchar valBLI_FLOAT_SHORTPic[] = "+999999";
bli_float_32 valbli_float_32Min = -999999.0;
bli_float_32 valbli_float_32Avg = 0.0;
bli_float_32 valbli_float_32Max = +999999.0;

const uchar valBLI_FLOAT_DOUBLEPic[] = "+999999999999999";
bli_float_64 valbli_float_64Min = -999999999999999.0;
bli_float_64 valbli_float_64Avg = 0.0;
bli_float_64 valbli_float_64Max = +999999999999999.0;

/**********************************************************************
 *                                                                    *
 * TestTypes - Table of all the tests and values                      *
 *                                                                    *
 *********************************************************************  */

testType testTypes[] = {
  {blir_typeDisplay,
   (uchar *)"blir_typeDisplay",
   blir_flagNull,
   (uchar *)"blir_FlagNull",
   sizeof (valDisplayMax),
   valDisplayPic,
   valDisplayMin, sizeof (valDisplayMin),
   valDisplayAvg, sizeof (valDisplayAvg),
   valDisplayMax, sizeof (valDisplayMax)}
  ,
  {blir_typeDisplay,
   (uchar *)"blir_typeDisplay",
   blir_flagSignLead | blir_flagOverpunch,
   (uchar *)"blir_flagSignLead|blir_flagOverpunch",
   sizeof (valDispSLMax),
   valDispSLPic,
   valDispSLMin, sizeof (valDispSLMin),
   valDispSLAvg, sizeof (valDispSLAvg),
   valDispSLMax, sizeof (valDispSLMax)}
  ,

  {blir_typeDisplay,
   (uchar *)"blir_typeDisplay",
   blir_flagSignLead,
   (uchar *)"blir_flagSignLead",
   sizeof (valDispSLSMax),
   valDispSLSPic,
   valDispSLSMin, sizeof (valDispSLSMin),
   valDispSLSAvg, sizeof (valDispSLSAvg),
   valDispSLSMax, sizeof (valDispSLSMax)}
  ,
  {blir_typeDisplay,
   (uchar *)"blir_typeDisplay",
   blir_flagSignTrail | blir_flagOverpunch,
   (uchar *)"blir_flagSignTrail|blir_flagOverpunch",
   sizeof (valDispSTMax),
   valDispSTPic,
   valDispSTMin, sizeof (valDispSTMin),
   valDispSTAvg, sizeof (valDispSTAvg),
   valDispSTMax, sizeof (valDispSTMax)}
  ,

  {blir_typeDisplay,
   (uchar *)"blir_typeDisplay",
   blir_flagSignTrail,
   (uchar *)"blir_flagSignTrail",
   sizeof (valDispSTSMax),
   valDispSTSPic,
   valDispSTSMin, sizeof (valDispSTSMin),
   valDispSTSAvg, sizeof (valDispSTSAvg),
   valDispSTSMax, sizeof (valDispSTSMax)}
  ,
  {blir_typePacked,
   (uchar *)"blir_typePacked",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valPackedMax),
   valPackedPic,
   valPackedMin, sizeof (valPackedMin),
   valPackedAvg, sizeof (valPackedAvg),
   valPackedMax, sizeof (valPackedMax)}
  ,
  {blir_typeBinary,
   (uchar *)"blir_typeBinary",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valscharMax),
   valscharPic,
   & valscharMin, sizeof (valscharMin),
   & valscharAvg, sizeof (valscharAvg),
   & valscharMax, sizeof (valscharMax)}
  ,
  {blir_typeBinary,
   (uchar *)"blir_typeBinary",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valint16Max),
   valint16Pic,
   & valint16Min, sizeof (valint16Min),
   & valint16Avg, sizeof (valint16Avg),
   & valint16Max, sizeof (valint16Max)}
  ,
  {blir_typeBinary,
   (uchar *)"blir_typeBinary",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valint32Max),
   valint32Pic,
   & valint32Min, sizeof (valint32Min),
   & valint32Avg, sizeof (valint32Avg),
   & valint32Max, sizeof (valint32Max)}
  ,
  {blir_typeBinary,
   (uchar *)"blir_typeBinary",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valint64Max),
   valint64Pic,
   & valint64Min, sizeof (valint64Min),
   & valint64Avg, sizeof (valint64Avg),
   & valint64Max, sizeof (valint64Max)}
  ,
  {blir_typeUnsigned,
   (uchar *)"blir_typeUnsigned",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valucharMax),
   valucharPic,
   & valucharMin, sizeof (valucharMin),
   & valucharAvg, sizeof (valucharAvg),
   & valucharMax, sizeof (valucharMax)}
  ,
  {blir_typeUnsigned,
   (uchar *)"blir_typeUnsigned",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valuint16Max),
   valuint16Pic,
   & valuint16Min, sizeof (valuint16Min),
   & valuint16Avg, sizeof (valuint16Avg),
   & valuint16Max, sizeof (valuint16Max)}
  ,
  {blir_typeUnsigned,
   (uchar *)"blir_typeUnsigned",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valuint32Max),
   valuint32Pic,
   & valuint32Min, sizeof (valuint32Min),
   & valuint32Avg, sizeof (valuint32Avg),
   & valuint32Max, sizeof (valuint32Max)}
  ,
  {blir_typeUnsigned,
   (uchar *)"blir_typeUnsigned",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valuint64Max),
   valuint64Pic,
   & valuint64Min, sizeof (valuint64Min),
   & valuint64Avg, sizeof (valuint64Avg),
   & valuint64Max, sizeof (valuint64Max)}
  ,
  {blir_typeFloat,
   (uchar *)"blir_typeFloat",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valbli_float_32Max),
   valBLI_FLOAT_SHORTPic,
   & valbli_float_32Min, sizeof (valbli_float_32Min),
   & valbli_float_32Avg, sizeof (valbli_float_32Avg),
   & valbli_float_32Max, sizeof (valbli_float_32Max)}
  ,
  {blir_typeFloat,
   (uchar *)"blir_typeFloat",
   blir_flagNull,
   (uchar *)"blir_flagNull",
   sizeof (valbli_float_64Max),
   valBLI_FLOAT_DOUBLEPic,
   & valbli_float_64Min, sizeof (valbli_float_64Min),
   & valbli_float_64Avg, sizeof (valbli_float_64Avg),
   & valbli_float_64Max, sizeof (valbli_float_64Max)}
};

#define NUMTESTTYPES (sizeof(testTypes)/sizeof(testType))

/**********************************************************************
 *                                                                    *
 * TestScharOut - Test output of schar                              *
 *                                                                    *
 *********************************************************************  */

int
testScharOut (const uchar *outPic,  /* Output Picture.  */
             const uchar *inPic,  /* Input Picture.  */
             schar inValue) /* Input Number.  */
{
  schar outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nSCHAR Output Test:\n\n");
  printf ("  Input Value  = %d\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeBinary, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeBinary, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %d\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestInt16Out - Test output of int16                            *
 *                                                                    *
 *********************************************************************  */

int
testInt16Out (const uchar *outPic,  /* Output Picture.  */
              const uchar *inPic,  /* Input Picture.  */
              int16 inValue) /* Input Number.  */
{
  int16 outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nINT16 Output Test:\n\n");
  printf ("  Input Value  = %d\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeBinary, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeBinary, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %d\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestInt32Out - Test output of int32                            *
 *                                                                    *
 *********************************************************************  */

int
testInt32Out (const uchar *outPic,  /* Output Picture.  */
              const uchar *inPic,  /* Input Picture.  */
              int32 inValue) /* Input Number.  */
{
  int32 outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nINT32 Output Test:\n\n");
  printf ("  Input Value  = %d\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeBinary, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeBinary, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %d\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestUcharOut - Test output of uchar                            *
 *                                                                    *
 *********************************************************************  */

int
testUcharOut (const uchar *outPic,  /* Output Picture.  */
              const uchar *inPic,  /* Input Picture.  */
              uchar inValue) /* Input Number.  */
{
  uchar outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nUCHAR Output Test:\n\n");
  printf ("  Input Value  = %u\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeUnsigned, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeUnsigned, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeUnsigned, blirFlagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeUnsigned, blir_flagNull);\n");

  printf ("  Output Image = %u\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestUInt16Out - Test output of uint16                          *
 *                                                                    *
 *********************************************************************  */

int
testUInt16Out (const uchar *outPic,  /* Output Picture.  */
               const uchar *inPic,  /* Input Picture.  */
               uint16 inValue) /* Input Number.  */
{
  uint16 outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nUINT16 Output Test:\n\n");
  printf ("  Input Value  = %u\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeUnsigned, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeUnsigned, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeUnsigned, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeUnsigned, blir_flagNull);\n");

  printf ("  Output Image = %u\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestUInt32Out - Test output of int32                           *
 *                                                                    *
 *********************************************************************  */

int
testUInt32Out (const uchar *outPic,  /* Output Picture.  */
               const uchar *inPic,  /* Input Picture.  */
               uint32 inValue) /* Input Number.  */
{
  uint32 outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nUINT32 Output Test:\n\n");
  printf ("  Input Value  = %u\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeUnsigned, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeUnsigned, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeUnsigned, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeUnsigned, blir_flagNull);\n");

  printf ("  Output Image = %u\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestPackedOut - Test Packed Decimal Output                         *
 *                                                                    *
 *********************************************************************  */

int32 testPackedOut (const uchar *outPic,  /* Output Picture.  */
                        const uchar *inPic,  /* Input Picture.  */
                        const void *inValue,  /* Input Number.  */
                        int32 inLen) /* Input Length.  */
{
  uchar outArea[16];  /* Output Area.  */
  int32 i, n;  /* Index.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nPacked Output Test:\n\n");
  printf ("  Input Value  = ");
  for (i = 0; i < inLen; i ++)
    printf ("%02X ", cptr[i]);
  fputc ('\n', stdout);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  n = ((destDigitTotal - 1) / 2) + 3;  /* Actually only +1 needed, use +3 to check padding.  */

  blir_emEditMove (outArea, n, blir_typePacked, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typePacked, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typePacked, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typePacked, blir_flagNull);\n");

  printf ("  Output Image = ");
  cptr = (uchar *)outArea;
  for (i = 0; i < n; i ++)printf ("%02X ", cptr[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestSingleOut - Test Single Precision output                       *
 *                                                                    *
 *********************************************************************  */

int32 testSingleOut (const uchar *outPic,  /* Output Picture.  */
                         const uchar *inPic,  /* Input Picture.  */
                         float inValue) /* Input Number.  */
{
  float outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nSingle Float Output Test:\n\n");
  printf ("  Input Value  = %g\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeFloat, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeFloat, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeFloat, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeFloat, blir_flagNull);\n");

  printf ("  Output Image = %g\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestDoubleOut - Test Double Precision output                       *
 *                                                                    *
 *********************************************************************  */

int32 testDoubleOut (const uchar *outPic,  /* Output Picture.  */
                         const uchar *inPic,  /* Input Picture.  */
                         double inValue) /* Input Number.  */
{
  double outArea;  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nDouble Float Output Test:\n\n");
  printf ("  Input Value  = %f\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove ( & outArea, sizeof (outArea), blir_typeFloat, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeFloat, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeFloat, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeFloat, blir_flagNull);\n");

  printf ("  Output Image = %f\n", outArea);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutColumns - Write out columns                                     *
 *                                                                    *
 *********************************************************************  */

int
putColumns (int32 width) /* Width of column line.  */
{
  int32 i;  /* Index.  */

  for (i = 1; i <= width; i ++)
    {
      if ((i % 10) == 0)
        fputc ('0' + (i / 10), stdout);
      else if ((i % 5) == 0)
        fputc ('+', stdout);
      else
        fputc ('-', stdout);
    }

  fputc ('\n', stdout);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestSchar - Test schar input                                     *
 *                                                                    *
 *********************************************************************  */

int
testSchar (const uchar *outPic,  /* Output Picture.  */
          const uchar *inPic,  /* Input Picture.  */
          schar inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nSCHAR Test:\n\n");
  printf ("  Input Value  = %d\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestUchar - Test uchar input                                   *
 *                                                                    *
 *********************************************************************  */

int
testUchar (const uchar *outPic,  /* Output Picture.  */
           const uchar *inPic,  /* Input Picture.  */
           uchar inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nUCHAR Test:\n\n");
  printf ("  Input Value  = %u\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeUnsigned, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeUnsigned, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestInt16 - Test int16 input                                   *
 *                                                                    *
 *********************************************************************  */

int
testInt16 (const uchar *outPic,  /* Output Picture.  */
           const uchar *inPic,  /* Input Picture.  */
           int16 inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nINT16 Test:\n\n");
  printf ("  Input Value  = %d\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestUint16 - Test uint16 input                                 *
 *                                                                    *
 *********************************************************************  */

int
testUint16 (const uchar *outPic,  /* Output Picture.  */
            const uchar *inPic,  /* Input Picture.  */
            uint16 inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nUINT16 Test:\n\n");
  printf ("  Input Value  = %u\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeUnsigned, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeUnsigned, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestInt32 - Test int32 input                                   *
 *                                                                    *
 *********************************************************************  */

int
testInt32 (const uchar *outPic,  /* Output Picture.  */
           const uchar *inPic,  /* Input Picture.  */
           int32 inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nINT32 Test:\n\n");
  printf ("  Input Value  = %d\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestUint32 - Test uint32 input                                 *
 *                                                                    *
 *********************************************************************  */

int
testUint32 (const uchar *outPic,  /* Output Picture.  */
            const uchar *inPic,  /* Input Picture.  */
            uint32 inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nUINT32 Test:\n\n");
  printf ("  Input Value  = %u\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeBinary, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeBinary, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestSingle - Test Float input                                      *
 *                                                                    *
 *********************************************************************  */

int
testSingle (const uchar *outPic,  /* Output Picture.  */
            const uchar *inPic,  /* Input Picture.  */
            float inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nSingle Float Test:\n\n");
  printf ("  Input Value  = %g\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeFloat, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeFloat, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDouble - Test Double input                                     *
 *                                                                    *
 *********************************************************************  */

int
testDouble (const uchar *outPic,  /* Output Picture.  */
            const uchar *inPic,  /* Input Picture.  */
            double inValue) /* Input Number.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\nDouble Float Test:\n\n");
  printf ("  Input Value  = %g\n", inValue);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   & inValue, sizeof (inValue), blir_typeFloat, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeFloat, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestPacked - Test Packed input                                     *
 *                                                                    *
 *********************************************************************  */

int
testPacked (const uchar *outPic,  /* Output Picture.  */
            const uchar *inPic,  /* Input Picture.  */
            const void *inValue,  /* Input Number.  */
            int32 inLen) /* Input Length.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 i;  /* Index.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nPacked Test:\n\n");
  printf ("  Input Value  = ");
  for (i = 0; i < inLen; i ++)
    printf ("%02X ", cptr[i]);
  fputc ('\n', stdout);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typePacked, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), usagePacked);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}


/**********************************************************************
 *                                                                    *
 * TestDisplay - Test Display input                                   *
 *                                                                    *
 *********************************************************************  */

int
testDisplay (const uchar *outPic,  /* Output Picture.  */
             const uchar *inPic,  /* Input Picture.  */
             const void *inValue,  /* Input Number.  */
             int32 inLen) /* Input Length.  */
{
  uchar outArea[256];  /* Output Area.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nDisplay Test:\n\n");
  printf ("  Input Value  = %s\n", cptr);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typeDisplay, blir_flagNull);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeDisplay, blir_flagNull);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestDispSignLeadSep - Test Display input with leading separated    *
 * sign                                                               *
 *                                                                    *
 *********************************************************************  */

int
testDispSignLeadSep (const uchar *outPic,  /* Output Picture.  */
                     const uchar *inPic,  /* Input Picture.  */
                     const void *inValue,  /* Input Number.  */
                     int32 inLen) /* Input Length.  */
{
  uchar outArea[256];  /* Output Area.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nDisplay Leading Separated Sign Test:\n\n");
  printf ("  Input Value  = %s\n", cptr);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typeDisplay, blir_flagSignLead);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeDisplay, blir_flagSignLead);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestDispSignTrailSep - Test Display input with trailing separated  *
 * sign                                                               *
 *                                                                    *
 *********************************************************************  */

int
testDispSignTrailSep (const uchar *outPic,  /* Output Picture.  */
                      const uchar *inPic,  /* Input Picture.  */
                      const void *inValue,  /* Input Number.  */
                      int32 inLen) /* Input Length.  */
{
  uchar outArea[256];  /* Output Area.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nDisplay Trailing Separated Sign Test:\n\n");
  printf ("  Input Value  = %s\n", cptr);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typeDisplay, blir_flagSignTrail);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeDisplay, blir_flagSignTrail);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestDispSignTrail - Test Display input with trailing overpunched   *
 * sign                                                               *
 *                                                                    *
 *********************************************************************  */

int
testDispSignTrail (const uchar *outPic,  /* Output Picture.  */
                   const uchar *inPic,  /* Input Picture.  */
                   const void *inValue,  /* Input Number.  */
                   int32 inLen) /* Input Length.  */
{
  uchar outArea[256];  /* Output Area.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nDisplay Trailing Overpunched Sign Test:\n\n");
  printf ("  Input Value  = %s\n", cptr);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typeDisplay, blir_flagSignTrail | blir_flagOverpunch);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeDisplay, blir_flagSignTrail|blir_flagOverpunch);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestDispSignLead - Test Display input with leading overpunched     *
 * sign                                                               *
 *                                                                    *
 *********************************************************************  */

int
testDispSignLead (const uchar *outPic,  /* Output Picture.  */
                  const uchar *inPic,  /* Input Picture.  */
                  const void *inValue,  /* Input Number.  */
                  int32 inLen) /* Input Length.  */
{
  uchar outArea[256];  /* Output Area.  */
  uchar *cptr;  /* Character pointer.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  cptr = (uchar *)inValue;

  printf ("\nDisplay Leading Overpunched Sign Test:\n\n");
  printf ("  Input Value  = %s\n", cptr);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight, outPic, inPic);

  blir_emEditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   inValue, inLen, blir_typeDisplay, blir_flagSignLead | blir_flagOverpunch);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), blir_typeDisplay, blir_flagSignLead|blir_flagOverpunch);\n");

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestUnedit - Test the Unedit routines                              *
 *                                                                    *
 *********************************************************************  */

int
testUnedit (const uchar *sourcePic,  /* Source Picture used to edit.  */
            const void *source,  /* Source Value.  */
            int32 sourceLen,  /* Source Length.  */
            int32 sourceType,  /* Source Type.  */
            int32 sourceFlags) /* Source Flags.  */
{
  uchar dest[100];  /* Destination.  */
  uchar destPic[100];  /* Destination Picture.  */
  uchar uneditPic[100];  /* Unedit Picture.  */
  uchar uneditType[100];  /* Usage type.  */
  int32 uneditDig;  /* Unedit digits.  */
  uchar *cptr;  /* Character Pointer.  */
  int32 i;  /* Index.  */

  printf ("\n");
  printf ("----------------------\n");
  printf ("Unedit / Edit test Set\n");
  printf ("----------------------\n");
  printf ("\n");

  printf ("\nUnedit Test:\n\n");

  cptr = (uchar *)source;
  printf ("  Input Value  = ");
  for (i = 0; i < sourceLen; i ++)
    fputc (cptr[i], stdout);
  printf ("\n");

  printf ("  Source PIC   = %s\n", sourcePic);
  printf ("  Source Usage = ");

  if ((sourceType & blir_typeDisplay) == 0)
    strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagNull");
  else if ((sourceFlags & blir_flagOverpunch) == 0)
    {
      if ((sourceFlags & blir_flagSignLead) != 0)
        strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagSignLead");
      else if ((sourceFlags & blir_flagSignTrail) != 0)
        strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagSignTrail");
      else
        strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagNull");
    }
  else
    {
      if ((sourceFlags & blir_flagSignLead) != 0)
        strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagSignLead|blir_flagOverpunch");
      else if ((sourceFlags & blir_flagSignTrail) != 0)
        strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagSignTrail|blir_flagOverpunch");
      else
        strcpy ((char *)uneditType, "blir_typeDisplay, blir_flagOverpunch");
    }

  printf ("%s\n", (native_char*)uneditType);

  blir_emUnedit_Analyze (destPic, uneditPic,  & uneditDig, sourcePic);
  blir_emUnedit (dest, sizeof (dest), uneditPic, uneditDig, source, sourceType, sourceFlags);

  printf ("  Unedit PIC   = %s\n", uneditPic);
  printf ("  Dest   PIC   = %s\n", destPic);
  printf ("  Unedit Digit = %d\n", uneditDig);
  printf ("  Unedit Value = %s\n", dest);
  printf ("  Emited Code  = blir_emUnedit(uneditArea, sizeof(uneditArea), \"%s\",\n", uneditPic);
  printf ("                               %d, \"%s\", %s);\n", uneditDig, dest, uneditType);

  testDisplay ((uchar *)"***,***,***,**9.99", destPic, dest, strlen ((char *)dest));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * TestUneditMove - Test the unedit move                              *
 *                                                                    *
 *********************************************************************  */

int32 testUneditMove (const uchar *outPic,  /* Output Picture.  */
                          const uchar *inPic,  /* Input Picture.  */
                          const void *inValue) /* Input Number.  */
{
  uchar Picture[100];  /* Analyze picture.  */
  int32 digitTotal;  /* Destination total digits.  */
  int32 digitLeft;  /* Destination digits left.  */
  int32 digitRight;  /* Source digits right.  */
  uchar uneditPic[100];  /* Unedited picture.  */
  int32 uneditDigit;  /* Digits in unedited picture.  */
  uchar outArea[256];  /* Output area.  */
  uchar *cptr;  /* Character pointer.  */

  cptr = (uchar *)inValue;

  printf ("\nUnedit/Move Test:\n\n");
  printf ("  Input Value  = %s\n", cptr);
  printf ("  Input PIC    = %s\n", inPic);
  printf ("  Output PIC   = %s\n", outPic);

  blir_emUneditMove_Analyze (Picture,  & digitTotal,  & digitLeft,  & digitRight,
                             uneditPic,  & uneditDigit, outPic, inPic);

  blir_emUneditMove (outArea, sizeof (outArea), blir_typeDisplay, blir_flagNull,
                     Picture, digitTotal, digitLeft, digitRight,
                     uneditPic, uneditDigit, inValue, blir_typeDisplay, blir_flagNull);

  printf ("  Emited Code  = blir_emUneditMove(outArea, sizeof(outArea), blir_typeDisplay, blir_flagNull,\n");
  printf ("                                   \"%s\", %d, %d, %d,\n",
         Picture, digitTotal, digitLeft, digitRight);
  printf ("                                   \"%s\", %d, inArea, sizeof(inArea), blir_typeDisplay, blir_flagNull);\n",
         uneditPic, uneditDigit);

  printf ("  Output Image = %s\n", outArea);
  printf ("                 ");
  putColumns (strlen ((char *)outArea));

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestValue - Print a value based on type                          *
 *                                                                    *
 *********************************************************************  */

int
doTestValue (int32 fieldType,  /* Field Type.  */
            const void *value,  /* Value to print.  */
            int32 size) /* Size of value.  */
{
  int32 i;  /* Index.  */
  uchar *uptr;  /* Unsigned character pointer.  */
  schar i8;  /* 8 Bit signed.  */
  int16 i16;  /* 16 Bit signed.  */
  int32 i32;  /* 32 Bit signed.  */
  int64 i64;  /* 64 Bit signed.  */
  uchar u8;  /* 8 Bit unsigned.  */
  uint16 u16;  /* 16 Bit unsigned.  */
  uint32 u32;  /* 32 Bit unsigned.  */
  uint64 u64;  /* 64 Bit unsigned.  */
  bli_float_32 fshort;  /* 32 Bit floating point.  */
  bli_float_64 fdouble;  /* 64 Bit floating point.  */
  int32 type;  /* Field type.  */

  type = fieldType & (blir_typeDisplay | blir_typePacked | blir_typeBinary | blir_typeUnsigned | blir_typeFloat);

  switch (type)
    {
    case blir_typeDisplay :
      uptr = (uchar *)value;
      for (i = 0; uptr[i] != '\0'; i ++)fputc (uptr[i], stdout);
      break;
    case blir_typePacked :
      uptr = (uchar *)value;
      printf ("%02X", uptr[0]);
      for (i = 1; i < size; i ++)printf (" %02X", uptr[i]);
      break;
    case blir_typeBinary :
      if (size == BLI_INT8_WIDTH)
        {
          i8 =  * ((schar *)value);
          printf ("%d\n", i8);
        }
      else if (size == BLI_INT16_WIDTH)
        {
          i16 =  * ((int16 *)value);
          printf ("%d\n", i16);
        }
      else if (size == BLI_INT32_WIDTH)
        {
          i32 =  * ((int32 *)value);
          printf ("%d\n", i32);
        }
      else if (size == BLI_INT64_WIDTH)
        {
          i64 =  * ((int64 *)value);
          printf ("%lld\n", i64);
        }
      else
        {
          uptr = (uchar *)value;
          printf ("%02X", uptr[0]);
          for (i = 1; i < size; i ++)printf (" %02X", uptr[i]);
        }
      break;
    case blir_typeUnsigned :

      if (size == BLI_UINT8_WIDTH)
        {
          u8 =  * ((uchar *)value);
          printf ("%u\n", u8);
        }
      else if (size == BLI_UINT16_WIDTH)
        {
          u16 =  * ((uint16 *)value);
          printf ("%u\n", u16);
        }
      else if (size == BLI_UINT32_WIDTH)
        {
          u32 =  * ((uint32 *)value);
          printf ("%u\n", u32);
        }
      else if (size == BLI_UINT64_WIDTH)
        {
          u64 =  * ((uint64 *)value);
          printf ("%llu\n", u64);
        }
      else
        {
          uptr = (uchar *)value;
          printf ("%02X", uptr[0]);
          for (i = 1; i < size; i ++)printf (" %02X", uptr[i]);
        }
      break;

    case blir_typeFloat :

      if (size == BLI_FLOAT_32_WIDTH)
        {
          fshort =  * ((bli_float_32 *)value);
          printf ("%f\n", fshort);
        }
      else if (size == BLI_FLOAT_64_WIDTH)
        {
          fdouble =  * ((bli_float_64 *)value);
          printf ("%f\n", fdouble);
        }
      else
        {
          uptr = (uchar *)value;
          printf ("%02X", uptr[0]);
          for (i = 1; i < size; i ++)printf (" %02X", uptr[i]);
        }
      break;
    default :
      uptr = (uchar *)value;
      printf ("%02X", uptr[0]);
      for (i = 1; i < size; i ++)printf (" %02X", uptr[i]);
      break;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTest - Do A single test                                          *
 *                                                                    *
 *********************************************************************  */

int
doTest (int32 testNum,  /* Test number.  */
        const void *value,  /* Source Value.  */
        int32 width,  /* Source Width.  */
        int32 idxFrom,  /* Table index for from entry.  */
        int32 idxTo) /* Table index for to entry.  */
{
  uchar outArea[256];  /* Output Area.  */
  int32 destDigitTotal;  /* Total digits in destination.  */
  int32 destDigitLeft;  /* Digits to left of decimal in destination.  */
  int32 sourceDigitRight;  /* Digits to right of decimal in soruce.  */
  uchar Picture[256];  /* Encoded desintation picture.  */

  printf ("\n");
  printf ("Test-%04d\n\n", testNum);
  printf ("  From Type    = %s\n", testTypes[idxFrom].ttName);
  printf ("  To Type      = %s\n", testTypes[idxTo].ttName);
  printf ("  From value   = ");
  doTestValue (testTypes[idxFrom].ttType, value, width);
  printf ("\n");
  printf ("  Input PIC    = %s\n", testTypes[idxFrom].ttPicture);
  printf ("  Output PIC   = %s\n", testTypes[idxTo].ttPicture);

  blir_emEditMove_Analyze (Picture,  & destDigitTotal,  & destDigitLeft,  & sourceDigitRight,
                          testTypes[idxTo].ttPicture,
                          testTypes[idxFrom].ttPicture);

  blir_emEditMove (outArea, testTypes[idxTo].ttLength, testTypes[idxTo].ttType, testTypes[idxTo].ttFlags,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   value, width, testTypes[idxFrom].ttType, testTypes[idxFrom].ttFlags);

  printf ("  Emited Code  = blir_emEditMove(outArea, sizeof(outArea), %s, %s,\n", testTypes[idxTo].ttName, testTypes[idxTo].ttFlagName);
  printf ("                                 \"%s\", %d, %d, %d,\n",
         Picture, destDigitTotal, destDigitLeft, sourceDigitRight);
  printf ("                                 inArea, sizeof(inArea), %s, %s);\n", testTypes[idxFrom].ttName, testTypes[idxFrom].ttFlagName);

  printf ("  Output Image = ");
  doTestValue (testTypes[idxTo].ttType, outArea, testTypes[idxTo].ttMaxWidth);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestAll - Drive all types versus all types                       *
 *                                                                    *
 *********************************************************************  */

int
doTestAll (void)
{
  size_t i, j;
  int32 nTest;  /* Test Number.  */

  nTest = 0;

  for (i = 0; i < NUMTESTTYPES; i ++)
    for (j = 0; j < NUMTESTTYPES; j ++)
      {
        doTest ( ++nTest,
                testTypes[i].ttMinValue, testTypes[i].ttLength, i, j);
        doTest ( ++nTest,
                testTypes[i].ttAvgValue, testTypes[i].ttLength, i, j);
        doTest ( ++nTest,
                testTypes[i].ttMaxValue, testTypes[i].ttLength, i, j);
      }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Main - Testing Mainline for blir_emEditMove.c                      *
 *                                                                    *
 *********************************************************************  */

int
main (void)
{

  printf ("\n");
  printf ("--------------------------------------------------------------\n");
  printf ("Testing currency symbol = $, and DECIMAL IS DECIMAL (defaults)\n");
  printf ("--------------------------------------------------------------\n");
  printf ("\n");

  blir_emSetCurrencySymbol ('$');
  blir_emSetDecimalIsComma (0);

  /*
    The test drivers below, based on type, will have 3 or 4 arguments.  Those drivers that
    process data widths that are understood (integer & floating) do not have a length, hence
    only 3 arguments.  All the other functions have 4 arguments, the fourth being the length
    of the value.  The arguments are:
    .sp
    Output Picture - BLISS Picture to format output
    .br
    Input Picutre  - BLISS Picutre used to to determine decimal
    .br
    Input Value    - Value to be formatted
    .br
    Input Length   - Length of input (not for integer or floating).  */
  testInt32 ((uchar *)"$$$,$$$,$$$", (uchar *)"999999999.", 10000);
  testInt32 ((uchar *)"$--,---,--9", (uchar *)"999999999.", -10000);
  testInt32 ((uchar *)"$***,***,***", (uchar *)"999999999.", 10000);
  testInt32 ((uchar *)"99.99", (uchar *)"999.9", 10);
  testInt32 ((uchar *)"99.99", (uchar *)"9v999", -10);
  testUint32 ((uchar *)"99.99", (uchar *)"999V9", 10u);
  testInt16 ((uchar *)"99.99", (uchar *)"9.999", 10);
  testInt16 ((uchar *)"99.99", (uchar *)"99.99", -10);
  testUint16 ((uchar *)"99.99", (uchar *)"99V99", 10u);
  testSchar ((uchar *)"+99.99", (uchar *)"99V9", -100);
  testUchar ((uchar *)"+99.99", (uchar *)"99V9", 100u);
  testSingle ((uchar *)"99.99", (uchar *)"9.99V9", 10.0);
  testSingle ((uchar *)"99.99", (uchar *)"9V99.9", -10.0);
  testDouble ((uchar *)"99.99", (uchar *)"99V.99", 10.0);
  testDouble ((uchar *)"99.99", (uchar *)"V9999.", -10.0);
  testDouble ((uchar *)"99.99", (uchar *)".9999V", 1.0e-50);
  testDouble ((uchar *)"99.99", (uchar *)"99V99", 1.0e+50);
  testPacked ((uchar *)"ZZZ,ZZZ,ZZ9.999", (uchar *)"99999.99", "\x31\x41\x59\x1D", 4);
  testDisplay ((uchar *)"ZZZ,ZZZ,ZZ9.999", (uchar *)"999999.99", (uchar *)"+12345678", 9);
  testDisplay ((uchar *)"Z(3),Z(3),Z(2)9.9(3)", (uchar *)"999999.99", (uchar *)"+12345678", 9);
  testDispSignLeadSep ((uchar *)"---,---,--9.999", (uchar *)"999999.99", (uchar *)"-12345678", 9);
  testDispSignTrailSep ((uchar *)"---,---,--9.999", (uchar *)"999999.99", (uchar *)"12345678-", 9);
  testDispSignLead ((uchar *)"---,---,--9.999", (uchar *)"999999.99", (uchar *)"J2345678", 8);  /* -12345678.  */
  testDispSignTrail ((uchar *)"---,---,--9.999", (uchar *)"999999.99", (uchar *)"1234567Q", 8);  /* 12345678-.  */

  printf ("\n");
  printf ("-------------------------------------------------\n");
  printf ("Testing currency symbol = %%, and DECIMAL IS COMMA\n");
  printf ("-------------------------------------------------\n");
  printf ("\n");

  blir_emSetCurrencySymbol ('%');
  blir_emSetDecimalIsComma (1);

  testDispSignTrail ((uchar *)"$$$.$$$.$$$.$$9,999PBDB", (uchar *)"999999.99", (uchar *)"1234567Q", 8);  /* 12345678-.  */
  testDispSignTrail ((uchar *)"$$$.$$$.$$$.$$9,999PBDB", (uchar *)"999999.99", (uchar *)"1234567H", 8);  /* 12345678+.  */

  printf ("\n");
  printf ("-----------------------------------\n");
  printf ("Testing various output binary types\n");
  printf ("-----------------------------------\n");
  printf ("\n");

  blir_emSetCurrencySymbol ('$');
  blir_emSetDecimalIsComma (0);

  testScharOut ((uchar *)"999.99", (uchar *)"999.99", -121);
  testInt16Out ((uchar *)"9999.99", (uchar *)"9999.99", -1357);
  testInt32Out ((uchar *)"9999999.99", (uchar *)"9999999.99", -123456789);
  testUcharOut ((uchar *)"9.99", (uchar *)"9.99", 181);
  testUInt16Out ((uchar *)"99.99", (uchar *)"99.99", 6789);
  testUInt32Out ((uchar *)"9999999.99", (uchar *)"9999999.99", 123456789);
  testPackedOut ((uchar *)"9999.9", (uchar *)"9999.9", (uchar *)"\x31\x41\x5D", 3);
  testSingleOut ((uchar *)"999.999", (uchar *)"999.999", 123.456);
  testDoubleOut ((uchar *)"999999.999999", (uchar *)"999999.999999", 123456.654321);

  printf ("\n");
  printf ("-----------------------------------\n");
  printf ("Testing Unedit functions           \n");
  printf ("-----------------------------------\n");
  printf ("\n");

  testUnedit ((uchar *)"ZZZ,ZZZ,ZZ9", (uchar *)" 31,415,926", 11, blir_typeDisplay, blir_flagNull);
  testUnedit ((uchar *)"$$$,$$$,$$9.99", (uchar *)"   $314,159.26", 14, blir_typeDisplay, blir_flagNull);
  testUneditMove ((uchar *)"ZZZ,ZZZ,ZZ9.999", (uchar *)"***,**9.99", (uchar *)"****123.45");

  printf ("\n");
  printf ("------------------------------------------\n");
  printf ("Carry out detailed cross test of all types\n");
  printf ("------------------------------------------\n");
  printf ("\n");


  doTestAll ();

  return 0;

}
