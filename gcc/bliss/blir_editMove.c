/*                                                                    *
 *                     BLISS Numeric Edited Moves                     *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.      *
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
 *  */


/**********************************************************************
 *                                                                    *
 * Title:  BLISS Numeric Edited Moves                                 *
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
 * See ChangeLog file for subsequent modifications                    *
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
#include "blir_overpunch.h"
#include "blidebug.h"


/**********************************************************************
 *                                                                    *
 * Special Edit Characters to be added to picture to indicate special *
 * effects                                                            *
 *                                                                    *
 *********************************************************************  */

#define SUPSPACE         'a'    /* Suppressed space Z */
#define SUPASTERISK      'b'    /* Suppressed asterisk * */
#define SUPCURRENCY      'c'    /* Suppressed currency $$ */
#define SUPPLUS          'd'    /* Suppressed plus ++ */
#define SUPMINUS         'e'    /* Suppressed minus -- */

#define SUPCOMMASPACE    'f'    /* Comma within suppresed space Z,Z */
#define SUPCOMMAASTERISK 'g'    /* Comma within suppressed asterisk *,* */
#define SUPCOMMACUR      'h'    /* Comma within suppressed currency $,$ */
#define SUPCOMMAPLUS     'i'    /* Comma within suppressed plus +,+ */
#define SUPCOMMAMINUS    'j'    /* Comma within suppressed minus -,- */

#define SIGNS            'k'    /* Leading S character */
#define SIGNPLUS         'l'    /* + Sign character */
#define SIGNMINUS        'm'    /* - Sign character */
#define SIGNCR           'n'    /* Credit symbol CR */
#define SIGNDB           'o'    /* Debit symbol DB */

/**********************************************************************
 *                                                                    *
 *                      Miscellaneous variables                       *
 *                                                                    *
 *********************************************************************  */

static uchar CurrencySymbol = '$';  /* Currency Symbol.  */
static uchar PeriodSymbol = '.';  /* Period Symbol.  */
static uchar CommaSymbol = ',';  /* Comma Symbol.  */
static int32 DecimalIsComma = FALSE;  /* Decimal Point Is Comma.  */

#define posSign   (0)           /* Position of sign in internal format */
#define posInt0   (1)           /* First integer digit */
#define posIntN   (40)          /* last integer digit */
#define posFract0 (41)          /* First Fraction digit */
#define posFractN (80)          /* Last Fraction digit */
#define posEOS    (81)          /* End of String */
#define posWIDTH  (82)          /* Width of internal format */
#define floatMin  (1.0e-30)     /* Minimum absolute floating value */
#define floatMax  (1.0e+30)     /* Maximum absolute floating value */
#define posWORK   (256)         /* Work size */

/**********************************************************************
 *                                                                    *
 *            Internal usage types that merge type & flags            *
 *                                                                    *
 *********************************************************************  */

typedef enum
{
  usageNull,  /* Null type.  */
  usageText,  /* Text type.  */
  usageDisplay,  /* Display.  */
  usageDispSignLead,  /* Display with SIGN IS LEADING.  */
  usageDispSignLeadSep,  /* Display with SIGN IS LEADING SEPARATE BLI_CHARACTER.  */
  usageDispSignTrail,  /* Display with SIGN IS TRAILING.  */
  usageDispSignTrailSep,  /* Display with SIGN IS TRAILING SEPARATE BLI_CHARACTER.  */
  usagePacked,  /* Packed Decimal.  */
  usageint8,  /* Signed 8 bit binary.  */
  usageint16,  /* Signed 16 bit binary.  */
  usageint32,  /* Signed 32 bit binary.  */
  usageint64,  /* Signed 64 bit binary.  */
#if 0
  usageint128,  /* Signed 128 bit binary.  */
#endif
  usageuchar,  /* Unsigned 8 bit binary.  */
  usageuint16,  /* Unsigned 16 bit binary.  */
  usageuint32,  /* Unsigned 32 bit binary.  */
  usageuint64,  /* Unsigned 64 bit binary.  */
#if 0
  usageuint128,  /* Unsigned 128 bit binary.  */
#endif
  usagebli_float_32,  /* Floating point 32 bit (float).  */
  usagebli_float_64 /* Floating point 64 bit (double).  */
#if 0
  usageBLI_FLOAT_LONG_DOUBLE /* Floating point 128 bit (long double).  */
#endif
}
usageType;

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */

static void
stub (const uchar *msg);  /* Message.  */
static int32
getDisplayOverpunch (uchar *destExtract,  /* Internal format.  */
                     const void *source,  /* Area to convert.  */
                     int32 sourceLen,  /* Length of area.  */
                     int32 *decimalFound,  /* Decimal Found.  */
                     int32 lead);  /* Leading sign.  */

static int32
getDisplay (uchar *destExtract,  /* Internal format.  */
            const void *source,  /* Area to convert.  */
            int32 sourceLen,  /* Length of area.  */
            int32 *decimalFound);  /* Decimal Found.  */

static int32
getPacked (uchar *destExtract,  /* Internal format.  */
           const void *source,  /* Packed area to convert.  */
           int32 sourceLen);  /* Length of packed area.  */

static int32
getInt32 (uchar *destExtract,  /* Internal format.  */
          int32 i32);  /* Number to convert.  */

static int32
getInt64 (uchar *destExtract,  /* Internal format.  */
          int64 i64);  /* Number to convert.  */
#if 0
static int32
getInt128 (uchar *destExtract,  /* Internal format.  */
           int128 i128);  /* Number to convert.  */
#endif
static int32
getUInt32 (uchar *destExtract,  /* Internal format.  */
           uint32 u32);  /* Number to convert.  */

static int32
getUInt64 (uchar *destExtract,  /* Internal format.  */
           uint64 u64);  /* Number to convert.  */
#if 0
static int32
getUInt128 (uchar *destExtract,  /* Internal format.  */
            uint128 u128);  /* Number to convert.  */
#endif
static int32
getbli_float_64 (uchar *destExtract,  /* Internal format.  */
                     bli_float_64 fdouble);  /* Number to convert.  */
#if 0
static int32
getBLI_FLOAT_LONG_DOUBLE (uchar *destExtract,  /* Internal format.  */
                          BLI_FLOAT_LONG_DOUBLE flongdouble);  /* Number to convert.  */
#endif

static int32
putInt8 (void *dest,  /* Destination area.  */
         int32 destDigitTotal,  /* Total digits in destination.  */
         uchar sign,  /* Sign of source.  */
         uchar *destExtract);  /* Extracted data.  */

static int32
putInt16 (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar sign,  /* Sign of source.  */
          uchar *destExtract);  /* Extracted data.  */

static int32
putInt32 (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar sign,  /* Sign of source.  */
          uchar *destExtract);  /* Extracted data.  */

static int32
putInt64 (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar sign,  /* Sign of source.  */
          uchar *destExtract);  /* Extracted data.  */
#if 0
static int32
putInt128 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar sign,  /* Sign of source.  */
           uchar *destExtract);  /* Extracted data.  */
#endif
static int32
putUchar (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar *destExtract);  /* Extracted data.  */

static int32
putUInt16 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar *destExtract);  /* Extracted data.  */

static int32
putUInt32 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar *destExtract);  /* Extracted data.  */

static int32
putUInt64 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar *destExtract);  /* Extracted data.  */
#if 0
static int32
putUInt128 (void *dest,  /* Destination area.  */
            int32 destDigitTotal,  /* Total digits in destination.  */
            uchar *destExtract);  /* Extracted data.  */
#endif
static int32
putPacked (void *dest,  /* Destination area.  */
           int32 destLen,  /* Destination length.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar sign,  /* Sign of source.  */
           uchar *destExtract);  /* Extracted data.  */

static int32
putbli_float_32 (void *dest,  /* Destination area.  */
                    int32 destDigitTotal,  /* Total digists in destination.  */
                    uchar *destExtract,  /* Extract Area.  */
                    int32 selPos);  /* Position to start extract.  */

static int32
putbli_float_64 (void *dest,  /* Destination area.  */
                     int32 destDigitTotal,  /* Total digists in destination.  */
                     uchar *destExtract,  /* Extract Area.  */
                     int32 selPos);  /* Position to start extract.  */
#if 0
static int32
putBLI_FLOAT_LONG_DOUBLE (void *dest,  /* Destination area.  */
                          int32 destDigitTotal,  /* Total digists in destination.  */
                          uchar *destExtract,  /* Extract Area.  */
                          int32 selPos);  /* Position to start extract.  */
#endif

static int32
encodePicture (uchar *outPic,  /* Output Picture.  */
               const uchar *inPic);  /* Input picture.  */

static int32
analyzePicture (uchar *outPic,  /* Output Picture.  */
                const uchar *inPic,  /* Intput Picture.  */
                int32 *destDigitLeft,  /* Digits left of assumed decimal point.  */
                int32 *sourceDigitRight);  /* Digits right of assumed decimal point.  */

static int32
checkFloat (const uchar *picture,  /* Current picture.  */
            int32 idxPic,  /* Picture index.  */
            uchar *dest,  /* Destination.  */
            int32 idxDest,  /* Destination index.  */
            uchar sign);  /* Current sign.  */

static int32
putDisplay (uchar *outArea,  /* Output Area.  */
            size_t outLen,  /* Output Area Length.  */
            const uchar *picture,  /* Picture to edit in the number with.  */
            uchar sign,  /* Sign of number.  */
            const uchar *source,  /* Source area to extract numbers from.  */
            int32 leadSign,  /* Leading sign.  */
            int32 overpunchSign);  /* Overpunched sign.  */

static usageType
mergeFlags (int32 fieldType,  /* Field Type.  */
            int32 fieldFlag,  /* Field Flags.  */
            int32 fieldLength);  /* Field Length.  */


/**********************************************************************
 *                                                                    *
 * Stub - stubbed out routine for the not supported message           *
 *                                                                    *
 *********************************************************************  */

static void
stub (const uchar *msg) /* Message.  */
{
  BLI_ASSERT (0 && msg);
}

/**********************************************************************
 *                                                                    *
 * MergeFlags - convert field flags to intenal types                  *
 *                                                                    *
 *********************************************************************  */

static usageType
mergeFlags (int32 fieldType,  /* Field Type.  */
            int32 fieldFlag,  /* Field Flags.  */
            int32 fieldLength) /* Field Length.  */
{
  int32 type;  /* Clean field type.  */

  type =
    fieldType /* & (Cobr_typeText | blir_typeDisplay | blir_typePacked |
                 blir_typeBinary | blir_typeUnsigned | blir_typeFloat).  */;
  switch (fieldType)
    {
    case blir_typeText :
      return usageText;
      break;
    case blir_typeDisplay :
      if ((fieldFlag & blir_flagOverpunch) == 0)
        {
          if ((fieldFlag & blir_flagSignLead) != 0)
            return usageDispSignLeadSep;
          else if ((fieldFlag & blir_flagSignTrail) != 0)
            return usageDispSignTrailSep;
        }
      else
        {
          if ((fieldFlag & blir_flagSignLead) != 0)
            return usageDispSignLead;
          else if ((fieldFlag & blir_flagSignTrail) != 0)
            return usageDispSignTrail;
        }
      return usageDisplay;
      break;
    case blir_typePacked :
      return usagePacked;
      break;
    case blir_typeBinary :
      if (fieldLength == BLI_INT8_WIDTH)
        return usageint8;
      else if (fieldLength == BLI_INT16_WIDTH)
        return usageint16;
      else if (fieldLength == BLI_INT32_WIDTH)
        return usageint32;
      else if (fieldLength == BLI_INT64_WIDTH)
        return usageint64;
      else 
        abort ();
      break;
    case blir_typeUnsigned :
      if (fieldLength == BLI_UINT8_WIDTH)
        return usageuchar;
      else if (fieldLength == BLI_UINT16_WIDTH)
        return usageuint16;
      else if (fieldLength == BLI_UINT32_WIDTH)
        return usageuint32;
      else if (fieldLength == BLI_UINT64_WIDTH)
        return usageuint64;
      else 
        abort ();
      break;
    case blir_typeFloat :
      if (fieldLength == BLI_FLOAT_32_WIDTH)
        return usagebli_float_32;
      else if (fieldLength == BLI_FLOAT_64_WIDTH)
        return usagebli_float_64;
      else 
        abort ();
      break;
    default :
      BLI_ASSERT (0 && "invalid field type");
      break;
    }

  return usageNull;
}

/**********************************************************************
 *                                                                    *
 * GetDisplayOverpunch - Get Display Overpunch text to internal       *
 * format                                                             *
 *                                                                    *
 *********************************************************************  */

static int32
getDisplayOverpunch (uchar *destExtract,  /* Internal format.  */
                     const void *source,  /* Area to convert.  */
                     int32 sourceLen,  /* Length of area.  */
                     int32 *decimalFound,  /* Decimal Found.  */
                     int32 lead) /* Leading sign.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  uchar *display;  /* Display area.  */
  int32 i, j, s, p;  /* Indexes.  */
  uchar sign;  /* Sign.  */
  uchar ch;  /* Work character.  */
  uchar *cptr;  /* Work pointer.  */
  int32 oSign;  /* Overpunch sign.  */
  int32 oChar;  /* Overpunch uchar.  */

  display = (uchar *)source;

  j = 0;
  s = 0;
  for (i = 0; i < sourceLen; i ++)
    {
      ch = display[i];
      if (!isspace (ch))
        {
          if (j < (int)(sizeof (workExtract) - 1))
            workExtract[j ++] = ch;
        }
    }

  workExtract[j] = '\0';

  sign = '+';

  if (j > 0)
    {
      if (lead)
        {
          blir_overpunch_get ( & oSign,  & oChar, workExtract[0]);
          workExtract[0] = oChar;
          if (oSign < 0)
            sign = '-';
          else
            sign = '+';
        }
      else
        {
          i = j - 1;

          blir_overpunch_get ( & oSign,  & oChar, workExtract[i]);
          workExtract[i] = oChar;
          if (oSign < 0)
            sign = '-';
          else
            sign = '+';
        }
    }

  cptr = (uchar *)strchr ((char *)workExtract, '.');
  if (cptr == NULL)
    j = -1;
  else
    j = cptr - workExtract;

  if (j == -1)
    {
      j = strlen ((char *)workExtract);
      strcat ((char *)workExtract, ".0");
      * decimalFound = FALSE;
    }
  else
    * decimalFound = TRUE;

  for (p = posFract0, s = j + 1; (p <= posFractN) && isdigit (workExtract[s]); p ++, s ++)
    destExtract[p] = workExtract[s];
  for (p = posIntN, s = j - 1; (p >= posInt0) && isdigit (workExtract[s]); p --, s --)
    destExtract[p] = workExtract[s];
  destExtract[posSign] = sign;
  destExtract[posEOS] = '\0';

  return 0;
}


/**********************************************************************
 *                                                                    *
 * GetDisplay - Get Display text to internal format                   *
 *                                                                    *
 *********************************************************************  */

static int32
getDisplay (uchar *destExtract,  /* Internal format.  */
            const void *source,  /* Area to convert.  */
            int32 sourceLen,  /* Length of area.  */
            int32 *decimalFound) /* Decimal Found.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  uchar *display;  /* Display area.  */
  int32 i, j, s, p;  /* Indexes.  */
  uchar sign;  /* Sign.  */
  uchar ch;  /* Work character.  */
  uchar *cptr;  /* Work pointer.  */

  display = (uchar *)source;

  sign = '+';
  j = 0;
  s = 0;
  for (i = 0; i < sourceLen; i ++)
    {
      ch = display[i];
      if ((ch == '+') || (ch == '-'))
        sign = ch;
      if (isdigit (ch) || (ch == '.'))
        {
          if (j < (int)(sizeof (workExtract) - 1))
            workExtract[j ++] = ch;
        }
    }

  workExtract[j] = '\0';

  cptr = (uchar *)strchr ((char *)workExtract, '.');
  if (cptr == NULL)
    j = -1;
  else
    j = cptr - workExtract;

  if (j == -1)
    {
      j = strlen ((char *)workExtract);
      strcat ((char *)workExtract, ".0");
      * decimalFound = FALSE;
    }
  else
    * decimalFound = TRUE;

  for (p = posFract0, s = j + 1; (p <= posFractN) && isdigit (workExtract[s]); p ++, s ++)
    destExtract[p] = workExtract[s];
  for (p = posIntN, s = j - 1; (p >= posInt0) && isdigit (workExtract[s]); p --, s --)
    destExtract[p] = workExtract[s];
  destExtract[posSign] = sign;
  destExtract[posEOS] = '\0';

  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetPacked - Get internal format from Packed Decimal                *
 *                                                                    *
 *********************************************************************  */

static int32
getPacked (uchar *destExtract,  /* Internal format.  */
           const void *source,  /* Packed area to convert.  */
           int32 sourceLen) /* Length of packed area.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  uchar *pd;  /* Pointer to packed.  */
  int32 i, i0, i1, p, s;  /* Index.  */
  int32 posWk;  /* Index to work area.  */
  int32 pdIdx;  /* Index to packed area.  */
  uchar sign;  /* Sign.  */

  pd = (uchar *)source;
  posWk = 0;
  pdIdx = 0;
  for (i = 1; i < sourceLen; i ++)
    {
      workExtract[posWk ++] = (char)('0' + (pd[pdIdx] >> 4));
      workExtract[posWk ++] = (char)('0' + (pd[pdIdx ++] & 0x0F));
    }

  workExtract[posWk ++] = (char)('0' + (pd[pdIdx] >> 4));
  workExtract[posWk] = '\0';

  switch (pd[pdIdx] & 0x0F)
    {
    case 0x0C :
      sign = '+';
      break;
    case 0x0D :
      sign = '-';
      break;
    case 0x0F :
      sign = '+';
      break;
    default :
      sign = '+';
      break;
    }

  destExtract[posSign] = sign;
  i = 0;
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetInt32 -  Get internal format from Signed 32 bit binary          *
 *                                                                    *
 *********************************************************************  */

static int32
getInt32 (uchar *destExtract,  /* Internal format.  */
          int32 i32) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, i0, i1, s, p;  /* Indexes.  */

  sprintf ((char *)workExtract, "%+d", i32);
  i = 0;
  while (isspace (workExtract[i]))
    i ++;
  if ((workExtract[i] == '+') || (workExtract[i] == '-'))
    destExtract[posSign] = workExtract[i ++];
  else
    destExtract[posSign] = '+';
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetInt64 -  Get internal format from Signed 64 bit binary          *
 *                                                                    *
 *********************************************************************  */

static int32
getInt64 (uchar *destExtract,  /* Internal format.  */
          int64 i64) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, i0, i1, s, p;  /* Indexes.  */

  sprintf ((char *)workExtract, "%+lld", i64);
  i = 0;
  while (isspace (workExtract[i]))
    i ++;
  if ((workExtract[i] == '+') || (workExtract[i] == '-'))
    destExtract[posSign] = workExtract[i ++];
  else
    destExtract[posSign] = '+';
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetInt128 -  Get internal format from Signed 128 bit binary        *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
getInt128 (uchar *destExtract,  /* Internal format.  */
           int128 i128) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, i0, i1, s, p;  /* Indexes.  */

  sprintf (workExtract, "%+Ld", i128);
  i = 0;
  while (isspace (workExtract[i]))
    i ++;
  if ((workExtract[i] == '+') || (workExtract[i] == '-'))
    destExtract[posSign] = workExtract[i ++];
  else
    destExtract[posSign] = '+';
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }
  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * GetUInt32 -  Get internal format from Unsigned 32 bit binary       *
 *                                                                    *
 *********************************************************************  */

static int32
getUInt32 (uchar *destExtract,  /* Internal format.  */
           uint32 u32) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, i0, i1, s, p;  /* Indexes.  */

  sprintf ((char *)workExtract, "%u", u32);
  destExtract[posSign] = '+';
  i = 0;
  while (isspace (workExtract[i]))
    i ++;
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetUInt64 -  Get internal format from Unsigned 64 bit binary       *
 *                                                                    *
 *********************************************************************  */

static int32
getUInt64 (uchar *destExtract,  /* Internal format.  */
           uint64 u64) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, i0, i1, s, p;  /* Indexes.  */

  sprintf ((char *)workExtract, "%llu", u64);
  destExtract[posSign] = '+';
  i = 0;
  while (isspace (workExtract[i]))
    i ++;
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetUInt128 -  Get internal format from Unsigned 128 bit binary     *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
getUInt128 (uchar *destExtract,  /* Internal format.  */
            uint128 u128) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, i0, i1, s, p;  /* Indexes.  */

  sprintf (workExtract, "%llu", u128);
  destExtract[posSign] = '+';
  i = 0;
  while (isspace (workExtract[i]))
    i ++;
  if (isdigit (workExtract[i]))
    {
      i0 = i;
      while (isdigit (workExtract[i]))
        i ++;
      i1 = i - 1;
      for (p = posIntN, s = i1; (p >= posInt0) && (s >= i0); p --, s --)
        destExtract[p] = workExtract[s];
    }
  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * Getbli_float_64 -  Get internal format from Floating point 64  *
 * bit                                                                *
 *                                                                    *
 *********************************************************************  */

static int32
getbli_float_64 (uchar *destExtract,  /* Internal format.  */
                     bli_float_64 fdouble) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, j, s, p;  /* Indexes.  */
  uchar sign;  /* Sign.  */

  if (fdouble < 0.0)
    {
      sign = '-';
      fdouble = fabs (fdouble);
    }
  else
    sign = '+';
  if ((fdouble < floatMin) || (fdouble > floatMax))
    {
      destExtract[posSign] = '+';
      destExtract[posEOS] = '\0';
    }
  else
    {
      sprintf ((char *)workExtract, "%90.45f", fdouble);
      j = -1;
      for (i = 0; (workExtract[i] != '\0') && (j == -1); i ++)
        if (workExtract[i] == '.')
          j = i;
      if (j == -1)
        {
          destExtract[posSign] = sign;
          destExtract[posEOS] = '\0';
        }
      else
        {
          for (p = posFract0, s = j + 1; (p <= posFractN) && isdigit (workExtract[s]); p ++, s ++)
            destExtract[p] = workExtract[s];
          for (p = posIntN, s = j - 1; (p >= posInt0) && isdigit (workExtract[s]); p --, s --)
            destExtract[p] = workExtract[s];
          destExtract[posSign] = sign;
          destExtract[posEOS] = '\0';
        }
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * GetBLI_FLOAT_LONG_DOUBLE -  Get internal format from Floating      *
 * point 128 bit                                                      *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
getBLI_FLOAT_LONG_DOUBLE (uchar *destExtract,  /* Internal format.  */
                          BLI_FLOAT_LONG_DOUBLE flongdouble) /* Number to convert.  */
{
  uchar workExtract[posWORK];  /* Work area.  */
  int32 i, j, s, p;  /* Indexes.  */
  uchar sign;  /* Sign.  */

  if (flongdouble < 0.0)
    {
      sign = '-';
      flongdouble = fabs (flongdouble);
    }
  else
    sign = '+';
  if ((flongdouble < floatMin) || (flongdouble > floatMax))
    {
      destExtract[posSign] = '+';
      destExtract[posEOS] = '\0';
    }
  else
    {
      sprintf ((char *)workExtract, "%90.45?l?l?f", flongdouble);
      j = -1;
      for (i = 0; (workExtract[i] != '\0') && (j == -1); i ++)
        if (workExtract[i] == '.')
          j = i;
      if (j == -1)
        {
          destExtract[posSign] = sign;
          destExtract[posEOS] = '\0';
        }
      else
        {
          for (p = posFract0, s = j + 1; (p <= posFractN) && isdigit (workExtract[s]); p ++, s ++)
            destExtract[p] = workExtract[s];
          for (p = posIntN, s = j - 1; (p >= posInt0) && isdigit (workExtract[s]); p --, s --)
            destExtract[p] = workExtract[s];
          destExtract[posSign] = sign;
          destExtract[posEOS] = '\0';
        }
    }
  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * PutInt8 - write 8 bit value                                        *
 *                                                                    *
 *********************************************************************  */

static int32
putInt8 (void *dest,  /* Destination area.  */
         int32 destDigitTotal,  /* Total digits in destination.  */
         uchar sign,  /* Sign of source.  */
         uchar *destExtract) /* Extracted data.  */
{
  schar w8;  /* Work 8 bit area.  */
  schar *p8;  /* Destination pointer.  */

  destExtract[destDigitTotal] = '\0';
  w8 = atoi ((char *)destExtract);
  if (sign == '-')
    w8 =  - w8;
  p8 = (schar *)dest;
  * p8 = w8;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutInt16 - write 16 bit value                                      *
 *                                                                    *
 *********************************************************************  */

static int32
putInt16 (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar sign,  /* Sign of source.  */
          uchar *destExtract) /* Extracted data.  */
{
  int16 w16;  /* Work 16 bit area.  */
  int16 *p16;  /* Destination pointer.  */

  destExtract[destDigitTotal] = '\0';
  w16 = atoi ((char *)destExtract);
  if (sign == '-')
    w16 =  - w16;
  p16 = (int16 *)dest;
  * p16 = w16;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutInt32 - write 32 bit value                                      *
 *                                                                    *
 *********************************************************************  */

static int32
putInt32 (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar sign,  /* Sign of source.  */
          uchar *destExtract) /* Extracted data.  */
{
  int32 w32;  /* Work 32 bit area.  */
  int32 *p32;  /* Destination pointer.  */

  destExtract[destDigitTotal] = '\0';
  w32 = atoi ((char *)destExtract);
  if (sign == '-')
    w32 =  - w32;
  p32 = (int32 *)dest;
  * p32 = w32;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutInt64 - write 64 bit value                                      *
 *                                                                    *
 *********************************************************************  */

static int32
putInt64 (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar sign,  /* Sign of source.  */
          uchar *destExtract) /* Extracted data.  */
{
  int64 w64;  /* Work 64 bit area.  */
  int64 *p64;  /* Destination pointer.  */

  destExtract[destDigitTotal] = '\0';
  w64 = atol ((char *)destExtract);
  if (sign == '-')
    w64 =  - w64;
  p64 = (int64 *)dest;
  * p64 = w64;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutInt128 - write 128 bit value                                    *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
putInt128 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar sign,  /* Sign of source.  */
           uchar *destExtract) /* Extracted data.  */
{
  int128 w128;  /* Work 128 bit area.  */
  int128 * p128;  /* Destination pointer.  */
  int32 i;  /* Index.  */

  destExtract[destDigitTotal] = '\0';
  w128 = 0L;
  for (i = 0; destExtract[i] != '\0'; i ++)
    w128 = w128 * 10L + (int128)(destExtract[i] - '0');
  if (sign == '-')
    w128 =  - w128;
  p128 = (int128 * )dest;
  * p128 = w128;

  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * PutUchar - write unsigned 8 bit value                              *
 *                                                                    *
 *********************************************************************  */

static int32
putUchar (void *dest,  /* Destination area.  */
          int32 destDigitTotal,  /* Total digits in destination.  */
          uchar *destExtract) /* Extracted data.  */
{
  uchar w8;  /* Work 8 bit area.  */
  uchar *p8;  /* Destination pointer.  */
  int32 i;  /* Index.  */

  destExtract[destDigitTotal] = '\0';
  w8 = 0u;
  for (i = 0; destExtract[i] != '\0'; i ++)
    w8 = w8 * 10u + (uchar)(destExtract[i] - '0');
  p8 = (uchar *)dest;
  * p8 = w8;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutUInt16 - write unsigned 16 bit value                            *
 *                                                                    *
 *********************************************************************  */

static int32
putUInt16 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar *destExtract) /* Extracted data.  */
{
  uint16 w16;  /* Work 16 bit area.  */
  uint16 *p16;  /* Destination pointer.  */
  int32 i;  /* Index.  */

  destExtract[destDigitTotal] = '\0';
  w16 = 0u;
  for (i = 0; destExtract[i] != '\0'; i ++)
    w16 = w16 * 10u + (uint16)(destExtract[i] - '0');
  p16 = (uint16 *)dest;
  * p16 = w16;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutUInt32 - write unsigned 32 bit value                            *
 *                                                                    *
 *********************************************************************  */

static int32
putUInt32 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar *destExtract) /* Extracted data.  */
{
  uint32 w32;  /* Work 32 bit area.  */
  uint32 *p32;  /* Destination pointer.  */
  int32 i;  /* Index.  */

  destExtract[destDigitTotal] = '\0';
  w32 = 0u;
  for (i = 0; destExtract[i] != '\0'; i ++)
    w32 = w32 * 10u + (uint32)(destExtract[i] - '0');
  p32 = (uint32 *)dest;
  * p32 = w32;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutUInt64 - write unsigned 64 bit value                            *
 *                                                                    *
 *********************************************************************  */

static int32
putUInt64 (void *dest,  /* Destination area.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar *destExtract) /* Extracted data.  */
{
  uint64 w64;  /* Work 64 bit area.  */
  uint64 *p64;  /* Destination pointer.  */
  int32 i;  /* Index.  */

  destExtract[destDigitTotal] = '\0';
  w64 = 0u;
  for (i = 0; destExtract[i] != '\0'; i ++)
    w64 = w64 * 10u + (uint64)(destExtract[i] - '0');
  p64 = (uint64 *)dest;
  * p64 = w64;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutUInt128 - write unsigned 128 bit value                          *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
putUInt128 (void *dest,  /* Destination area.  */
            int32 destDigitTotal,  /* Total digits in destination.  */
            uchar *destExtract) /* Extracted data.  */
{
  uint128 w128;  /* Work 128 bit area.  */
  uint128 * p128;  /* Destination pointer.  */
  int32 i;  /* Index.  */

  destExtract[destDigitTotal] = '\0';
  w128 = 0u;
  for (i = 0; destExtract[i] != '\0'; i ++)
    w128 = w128 * 10u + (uint128)(destExtract[i] - '0');
  p128 = (uint128 * )dest;
  * p128 = w128;

  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * PutPacked - write packed decimal value                             *
 *                                                                    *
 *********************************************************************  */

static int32
putPacked (void *dest,  /* Destination area.  */
           int32 destLen,  /* Destination length.  */
           int32 destDigitTotal,  /* Total digits in destination.  */
           uchar sign,  /* Sign of source.  */
           uchar *destExtract) /* Extracted data.  */
{
  int32 digit1;  /* High order packed digit.  */
  int32 digit2;  /* Low order packed digit.  */
  int32 d;  /* Index for destination.  */
  int32 s;  /* Index for source.  */
  uchar *uptr;  /* Pointer to destination.  */

  memset (dest, 0, destLen);  /* Initialize to zeros.  */
  d = destLen - 1;  /* Last byte of destination.  */
  s = destDigitTotal - 1;  /* Last byte of source.  */
  digit1 = destExtract[s --] - '0';
  uptr = (uchar *)dest;
  if (sign == '+')
    uptr[d --] = (uchar)((digit1 << 4) | 0x0C);
  else
    uptr[d --] = (uchar)((digit1 << 4) | 0x0D);
  while ((d >= 0) && (s >= 0))
    {
      digit2 = destExtract[s --] - '0';
      if (s >= 0)
        digit1 = destExtract[s --] - '0';
      else
        digit1 = 0;
      uptr[d --] = (uchar)((digit1 << 4) | digit2);
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Putbli_float_32 - write short floating point value              *
 *                                                                    *
 *********************************************************************  */

static int32
putbli_float_32 (void *dest,  /* Destination area.  */
                    int32 destDigitTotal,  /* Total digists in destination.  */
                    uchar *destExtract,  /* Extract Area.  */
                    int32 selPos) /* Position to start extract.  */
{
  bli_float_32 w32;  /* Work 32 bit area.  */
  bli_float_32 *p32;  /* Destination pointer.  */
  uchar source[posWIDTH];  /* Source of convert.  */
  int32 s;  /* Index to source.  */
  int32 i;  /* Index.  */
  int32 bp, ep;  /* Begin / End position.  */

  s = 0;
  source[s ++] = destExtract[posSign];
  bp = selPos;
  ep = selPos + destDigitTotal;  /* Actually 1 too large, but allows rounding.  */
  if (ep > posFractN)
    ep = posFractN;
  if (bp > posFract0)
    {
      source[s ++] = '0';
      source[s ++] = '.';
      for (i = posFract0; i < bp; i ++)
        source[s ++] = '0';
    }

  for (i = bp; i <= ep; i ++)
    if (i != posFract0)
      source[s ++] = destExtract[i];
    else
      {
        source[s ++] = '.';
        source[s ++] = destExtract[i];
      }

  if (ep < posFract0)
    {
      for (i = ep + 1; i < posFract0; i ++)
        source[s ++] = '0';
      source[s ++] = '.';
    }

  source[s] = '\0';

  w32 = atof ((char *)source);
  p32 = (bli_float_32 *)dest;
  * p32 = w32;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Putbli_float_64 - write double floating point value            *
 *                                                                    *
 *********************************************************************  */

static int32
putbli_float_64 (void *dest,  /* Destination area.  */
                     int32 destDigitTotal,  /* Total digists in destination.  */
                     uchar *destExtract,  /* Extract Area.  */
                     int32 selPos) /* Position to start extract.  */
{
  bli_float_64 w64;  /* Work 64 bit area.  */
  bli_float_64 *p64;  /* Destination pointer.  */
  uchar source[posWIDTH];  /* Source of convert.  */
  int32 s;  /* Index to source.  */
  int32 i;  /* Index.  */
  int32 bp, ep;  /* Begin / End position.  */

  s = 0;
  source[s ++] = destExtract[posSign];
  bp = selPos;
  ep = selPos + destDigitTotal;  /* Actually 1 too large, but allows rounding.  */
  if (ep > posFractN)
    ep = posFractN;
  if (bp > posFract0)
    {
      source[s ++] = '0';
      source[s ++] = '.';
      for (i = posFract0; i < bp; i ++)
        source[s ++] = '0';
    }

  for (i = bp; i <= ep; i ++)
    if (i != posFract0)
      source[s ++] = destExtract[i];
    else
      {
        source[s ++] = '.';
        source[s ++] = destExtract[i];
      }

  if (ep < posFract0)
    {
      for (i = ep + 1; i < posFract0; i ++)
        source[s ++] = '0';
      source[s ++] = '.';
    }

  source[s] = '\0';


  w64 = atof ((char *)source);
  p64 = (bli_float_64 *)dest;
  * p64 = w64;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutBLI_FLOAT_LONG_DOUBLE - write long double floating point value  *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
putBLI_FLOAT_LONG_DOUBLE (void *dest,  /* Destination area.  */
                          int32 destDigitTotal,  /* Total digists in destination.  */
                          uchar *destExtract,  /* Extract Area.  */
                          int32 selPos) /* Position to start extract.  */
{
  BLI_FLOAT_LONG_DOUBLE w128;  /* Work 128 bit area.  */
  BLI_FLOAT_LONG_DOUBLE *p128;  /* Destination pointer.  */
  uchar source[posWIDTH];  /* Source of convert.  */
  int32 s;  /* Index to source.  */
  int32 i;  /* Index.  */
  int32 bp, ep;  /* Begin / End position.  */

  s = 0;
  source[s ++] = destExtract[posSign];
  bp = selPos;
  ep = selPos + destDigitTotal;  /* Actually 1 too large, but allows rounding.  */
  if (ep > posFractN)
    ep = posFractN;
  if (bp > posFract0)
    {
      source[s ++] = '0';
      source[s ++] = '.';
      for (i = posFract0; i < bp; i ++)
        source[s ++] = '0';
    }

  for (i = bp; i <= ep; i ++)
    if (i != posFract0)
      source[s ++] = destExtract[i];
    else
      {
        source[s ++] = '.';
        source[s ++] = destExtract[i];
      }

  if (ep < posFract0)
    {
      for (i = ep + 1; i < posFract0; i ++)
        source[s ++] = '0';
      source[s ++] = '.';
    }

  source[s] = '\0';


  w128 = atof ((char *)source);
  p128 = (BLI_FLOAT_LONG_DOUBLE *)dest;
  * p128 = w128;

  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * EncodePicture - Encode Picture clause text                         *
 *                                                                    *
 *********************************************************************  */

static int32
encodePicture (uchar *outPic,  /* Output Picture.  */
               const uchar *inPic) /* Input picture.  */
{
  int32 i, j, k, l;  /* Index.  */
  int32 n;  /* Counter.  */
  uchar workPic1[posWORK];  /* Work area 1.  */
  uchar workPic2[posWORK];  /* Work area 2.  */
  uchar workSet[10];  /* Working search set.  */
  uchar lastChar;  /* Last character processed.  */
  uchar ch;  /* Held character for testing.  */
  uchar chSupSym;  /* Suppression character when symbol matches.  */
  uchar chSupCom;  /* Suppression character when comma matches.  */

  /*
    Set up pseudo picture in case of syntax error.  */
  memset (outPic, '9', strlen ((char *)inPic));
  outPic[strlen ((char *)inPic)] = '\0';
  /*
    Just for safty sake, remove all spaces and make sure parentheses are
    matched;.  */
  i = j = n = 0;
  workPic1[j ++] = ' ';  /* Leading sentinal.  */
  while ((inPic[i] != '\0') && (j < (posWORK - 1)))
    {
      if (isspace (inPic[i]))
        i ++;  /* Just skip spaces.  */
      else if (inPic[i] == '(')
        {
          if (n != 0)
            return 1;  /* No nested ')'.  */
          n ++;
          workPic1[j ++] = inPic[i ++];
        }
      else if (inPic[i] == ')')
        {
          if (n != 1)
            return 1;  /* No matching '('.  */
          n --;
          workPic1[j ++] = inPic[i ++];
        }
      else if (n != 0)
        {
          if (!isdigit (inPic[i]))
            return 1;  /* Expected digits.  */
          workPic1[j ++] = inPic[i ++];
        }
      else
        workPic1[j ++] = (char)toupper (inPic[i ++]);
    }
  workPic1[j] = '\0';
  if (j == 0)
    return 1;  /* No picture.  */
  /*
    Expand parenthetical notation.  */
  i = j = 0;
  lastChar = ' ';
  while ((workPic1[i] != '\0') && (j < (posWORK - 1)))
    {
      if (workPic1[i] == '(')
        {
          if (lastChar == ' ')
            return 1;  /* Must have leading picture character.  */
          i ++;
          n = 0;
          while (isdigit (workPic1[i]))
            n = n * 10 + workPic1[i ++] - '0';
          if (workPic1[i] != ')')
            return 1;  /* Expected ')'.  */
          i ++;
          for (k = 2; k <= n; k ++)
            if (j < (posWORK - 1))
              workPic2[j ++] = lastChar;
        }
      else
        workPic2[j ++] = lastChar = workPic1[i ++];
    }
  workPic2[j] = '\0';
  strcpy ((char *)workPic1, (char *)workPic2);
  /*
    Look for actual sign flags, not suppression.  */
  k = 1;
  if (workPic1[k] == 'S')
    {
      if (strchr ((char *) & workPic1[k + 1], 'S') != NULL)
        return 1;  /* Only 1 'S' is permitted, must be first.  */
      workPic1[k ++] = SIGNS;
    }

  if (strchr ((char *)workPic1, 'S') != NULL)
    return 1;  /* 'S' must be first character.  */

  if (memcmp ( & workPic1[k], "CR", 2) == 0)
    {
      workPic1[k + 0] = SIGNCR;
      workPic1[k + 1] = SIGNCR;
    }
  else if (memcmp ( & workPic1[k], "DB", 2) == 0)
    {
      workPic1[k + 0] = SIGNDB;
      workPic1[k + 1] = SIGNDB;
    }
  else if (workPic1[k] == '-')
    {
      j = k + 1;
      n = 1;
      while (strchr ("-.,", workPic1[j]) != NULL)
        {
          if (workPic1[j] == '-')
            n ++;
          j ++;
        }

      if (n == 1)
        workPic1[k] = SIGNMINUS;
    }
  else if (workPic1[k] == '+')
    {
      j = k + 1;
      n = 1;
      while (strchr ("+.,", workPic1[j]) != NULL)
        {
          if (workPic1[j] == '+')
            n ++;
          j ++;
        }

      if (n == 1)
        workPic1[k] = SIGNPLUS;
    }

  k = strlen ((char *)workPic1) - 1;
  if (memcmp ( & workPic1[k - 1], "CR", 2) == 0)
    {
      workPic1[k - 1] = SIGNCR;
      workPic1[k] = SIGNCR;
    }
  else if (memcmp ( & workPic1[k - 1], "DB", 2) == 0)
    {
      workPic1[k - 1] = SIGNDB;
      workPic1[k] = SIGNDB;
    }
  else if (workPic1[k] == '-')
    {
      j = k - 1;
      n = 1;
      while (strchr ("-.,", workPic1[j]) != NULL)
        {
          if (workPic1[j] == '-')
            n ++;
          j --;
        }

      if (n == 1)
        workPic1[k] = SIGNMINUS;
    }
  else if (workPic1[k] == '+')
    {
      j = k - 1;
      n = 1;
      while (strchr ("+.,", workPic1[j]) != NULL)
        {
          if (workPic1[j] == '+')
            n ++;
          j --;
        }

      if (n == 1)
        workPic1[k] = SIGNPLUS;
    }
  /*.B
    Convert suppression characters and auxillaries to special characters.  */
  i = 0;
  l = strlen ((char *)workPic1);
  strcat ((char *)workPic1, " ");
  while (workPic1[i] != '\0')
    {
      switch (workPic1[i])
        {
        case '*' :
          k = i + 1;
          while (strchr ("*.,", workPic1[k]) != NULL)
            k ++;
          while (i < k)
            {
              switch (workPic1[i])
                {
                case '*' :
                  workPic1[i ++] = SUPASTERISK;
                  break;
                case ',' :
                  workPic1[i ++] = SUPCOMMAASTERISK;
                  break;
                default :
                  i ++;
                  break;
                }
            }
          break;
        case '$' :
        case 'Z' :
        case '+' :
        case '-' :
          ch = workPic1[i];
          k = i + 1;
          n = 1;
          strcpy ((char *)workSet, "X.,");  /* Set last 3 characters.  */
          workSet[0] = ch;  /* Set first character.  */
          while (strchr ((char *)workSet, workPic1[k]) != NULL)
            {
              if (workPic1[k] == ch)
                n ++;
              k ++;
            }
          if (n > 1)
            {
              switch (ch)
                {
                case '$' :
                  chSupSym = SUPCURRENCY;
                  chSupCom = SUPCOMMACUR;
                  break;
                case 'Z' :
                  chSupSym = SUPSPACE;
                  chSupCom = SUPCOMMASPACE;
                  break;
                case '+' :
                  chSupSym = SUPPLUS;
                  chSupCom = SUPCOMMAPLUS;
                  break;
                case '-' :
                  chSupSym = SUPMINUS;
                  chSupCom = SUPCOMMAMINUS;
                  break;
                default :
                  chSupSym = '-';
                  chSupCom = ',';
                  break;
                }

              while (i < k)
                {
                  if (workPic1[i] == ch)
                    workPic1[i ++] = chSupSym;
                  else if (workPic1[i] == CommaSymbol)
                    workPic1[i ++] = chSupCom;
                  else
                    i ++;
                }
            }
          else
            i ++;
          break;
        default :
          i ++;
          break;
        }
    }

  workPic1[i] = '\0';
  strcpy ((char *)outPic, (char *) & workPic1[1]);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * AnalyzePicture - Analyze picture for digits before & after decimal *
 * point                                                              *
 *                                                                    *
 *********************************************************************  */

static int32
analyzePicture (uchar *outPic,  /* Output Picture.  */
                const uchar *inPic,  /* Intput Picture.  */
                int32 *destDigitLeft,  /* Digits left of assumed decimal point.  */
                int32 *sourceDigitRight) /* Digits right of assumed decimal point.  */
{
  int32 ip;  /* Input Picture index.  */
  int32 op;  /* Output Picture index.  */
  int32 left, right;  /* Digits left and right of decimal point.  */
  int32 posP = 0;  /* Position of decimal point.  */
  int32 posV = 0 ;  /* Position of assumed decimal point.  */
  int32 posDP = 0;  /* Position of decimal point to use.  */
  uchar workPic[posWORK];  /* Work area.  */
  int32 nextDigP;  /* Next digit set posP.  */
  int32 nextDigV;  /* Next digit set posV.  */
  int32 nextDigDP;  /* Next digit set posDP.  */
  int32 decimalFound;  /* Decimal Point Found.  */
  int32 digitOnly = 0;  /* Digit only index.  */

  encodePicture (workPic, inPic);

  ip = op = 0;
  left = right = 0;
  posV = posP = posDP = -1;
  nextDigP = nextDigV = nextDigDP = FALSE;
  decimalFound = FALSE;
  digitOnly = 0;

  while (workPic[ip] != '\0')
    {
      switch (workPic[ip])
        {
        case SUPSPACE :
        case SUPASTERISK :
        case SUPCURRENCY :
        case SUPPLUS :
        case SUPMINUS :
        case '9' :
        case '*' :
        case '+' :
        case '-' :
          digitOnly ++;
          if (decimalFound)
            right ++;
          else
            left ++;
          if (nextDigP)
            {
              posP = digitOnly;
              nextDigP = FALSE;
            }
          if (nextDigV)
            {
              posV = digitOnly;
              nextDigV = FALSE;
            }
          if (nextDigDP)
            {
              posDP = digitOnly;
              nextDigDP = FALSE;
            }
          outPic[op ++] = (char)workPic[ip ++];
          break;
        case 'V' :
          if (decimalFound)
            {
              left += right;
              right = 0;
            }
          decimalFound = TRUE;
          nextDigDP = TRUE;
          nextDigV = TRUE;
          ip ++;  /* Remove the V from the picture.  */
          break;
        case SUPCOMMASPACE :
        case SUPCOMMAASTERISK :
        case SUPCOMMACUR :
        case SUPCOMMAPLUS :
        case SUPCOMMAMINUS :
          outPic[op ++] = workPic[ip ++];
          break;
        case ',' :
          if (PeriodSymbol == ',')
            {
              decimalFound = TRUE;
              nextDigDP = TRUE;
              nextDigP = TRUE;
            }
          outPic[op ++] = workPic[ip ++];
          break;
        case '.' :
          if (PeriodSymbol == '.')
            {
              decimalFound = TRUE;
              nextDigDP = TRUE;
              nextDigP = TRUE;
            }
          outPic[op ++] = workPic[ip ++];
          break;
        default :
          if (!isspace (workPic[ip]))
            outPic[op ++] = workPic[ip ++];
          else
            ip ++;
          break;
        }
    }
  outPic[op] = '\0';

  if (nextDigP)
    posP = digitOnly + 1;
  if (nextDigV)
    posV = digitOnly + 1;
  if (nextDigDP)
    posDP = digitOnly + 1;

  if (posDP == -1)
    posV = posP = 0;
  else
    {
      if (posV == -1)
        posV = posP;
      else if (posP == -1)
        posP = posV;
    }

  * destDigitLeft = left;
  * sourceDigitRight = right;

  return 0;
}

/**********************************************************************
 *                                                                    *
 * CheckFloat - Check floating character insertion:  $,Z,+,-          *
 *                                                                    *
 *********************************************************************  */

static int32
checkFloat (const uchar *picture,  /* Current picture.  */
            int32 idxPic,  /* Picture index.  */
            uchar *dest,  /* Destination.  */
            int32 idxDest,  /* Destination index.  */
            uchar sign) /* Current sign.  */
{
  if (idxPic <= 0)
    return 0;
  if (idxDest <= 0)
    return 0;

  switch (picture[idxPic - 1])
    {
    case SUPCURRENCY :
    case SUPCOMMACUR :
      dest[idxDest - 1] = CurrencySymbol;
      break;
    case SUPPLUS :
    case SUPCOMMAPLUS :
      dest[idxDest - 1] = sign;
      break;
    case SUPMINUS :
    case SUPCOMMAMINUS :
      if (sign == '-')
        dest[idxDest - 1] = '-';
      break;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * PutDisplay - Store Image into area using encoded picture and       *
 * internal value                                                     *
 *                                                                    *
 *********************************************************************  */

static int32
putDisplay (uchar *outArea,  /* Output Area.  */
            size_t outLen,  /* Output Area Length.  */
            const uchar *picture,  /* Picture to edit in the number with.  */
            uchar sign,  /* Sign of number.  */
            const uchar *source,  /* Source area to extract numbers from.  */
            int32 leadSign,  /* Leading sign.  */
            int32 overpunchSign) /* Overpunched sign.  */
{
  int32 idxPic, idxDest, idxSource;  /* Index.  */
  int32 maxIdxDest;  /* Maximum destination index.  */
  int32 maxPic;  /* Maximum picture length.  */
  int32 maxWidth;  /* Maximum width to process.  */
  int32 sigFlag;  /* Position of first stored digit.  */
  int32 sigFirst;  /* Position of first stored digit.  */
  int32 sigLast;  /* Position of last stored digit.  */
  int32 FoundSignFlag;  /* Flag indicating uchar 'S'.  */
  int32 i, j, n;  /* Indexes.  */
  uchar *cptr;  /* Character Pointer.  */

  maxIdxDest = outLen - 1;
  maxPic = strlen ((char *)picture);
  maxWidth = (maxIdxDest <= maxPic) ? maxIdxDest : maxPic;

  /*
    Test for full zero value & full zero suppression.  */
  cptr = (uchar *)strchr ((char *)picture, '9');
  if (cptr == (uchar *)NULL)
    {
      n = 0;
      for (i = posInt0; (i <= posFractN) && (n == 0); i ++)
        if (source[i] != '0')
          n ++;
      if (n == 0)
        {
          memset (outArea, ' ', maxWidth);
          outArea[maxWidth] = '\0';
          return 0;
        }
    }
  /*
    Edit in the picture.  */
  idxSource = 0;
  idxDest = 0;
  FoundSignFlag = FALSE;
  sigFlag = FALSE;
  sigFirst = -1;
  sigLast = -1;

  for (idxPic = 0; idxPic < maxWidth; idxPic ++)
    {
      switch (picture[idxPic])
        {
        case '9' :
          if (!sigFlag)
            {
              checkFloat (picture, idxPic, outArea, idxDest, sign);
              sigFlag = TRUE;
              sigFirst = idxDest;
            }

          sigLast = idxDest;
          outArea[idxDest ++] = source[idxSource ++];
          break;
        case SUPSPACE :
        case SUPASTERISK :
        case SUPCURRENCY :
        case SUPPLUS :
        case SUPMINUS :
          if ((source[idxSource] != '0') && (!sigFlag))
            {
              checkFloat (picture, idxPic, outArea, idxDest, sign);
              sigFlag = TRUE;
              sigFirst = idxDest;
            }

          if (sigFlag)
            {
              sigLast = idxDest;
              outArea[idxDest ++] = source[idxSource];
            }
          else
            {
              if (picture[idxPic] == SUPASTERISK)
                outArea[idxDest ++] = '*';
              else
                outArea[idxDest ++] = ' ';
            }

          idxSource ++;
          break;
        case SUPCOMMASPACE :
        case SUPCOMMAASTERISK :
        case SUPCOMMACUR :
        case SUPCOMMAPLUS :
        case SUPCOMMAMINUS :
          if (sigFlag)
            outArea[idxDest ++] = CommaSymbol;
          else if (picture[idxPic] == SUPCOMMAASTERISK)
            outArea[idxDest ++] = '*';
          else
            outArea[idxDest ++] = ' ';
          break;
        case SIGNPLUS :
          outArea[idxDest ++] = sign;
          break;
        case SIGNMINUS :
          if (sign == '-')
            outArea[idxDest ++] = '-';
          break;
        case SIGNCR :
          if (sign == '-')
            {
              idxPic ++;
              outArea[idxDest ++] = 'C';
              outArea[idxDest ++] = 'R';
            }
          else
            {
              idxPic ++;
              outArea[idxDest ++] = ' ';
              outArea[idxDest ++] = ' ';
            }
          break;
        case SIGNDB :
          if (sign == '-')
            {
              idxPic ++;
              outArea[idxDest ++] = 'D';
              outArea[idxDest ++] = 'B';
            }
          else
            {
              idxPic ++;
              outArea[idxDest ++] = ' ';
              outArea[idxDest ++] = ' ';
            }
          break;
        case SIGNS :
          FoundSignFlag = TRUE;
          break;
        case '0' :
          outArea[idxDest ++] = '0';
          break;
        case '/' :
        case '+' :
        case '-' :
          outArea[idxDest ++] = picture[idxPic];
          break;
        case '$' :
          outArea[idxDest ++] = CurrencySymbol;
          break;
        case 'P' :
          outArea[idxDest ++] = '0';
          break;
        case 'B' :
          outArea[idxDest ++] = ' ';
          break;
        case '.' :
        case ',' :
          if (picture[idxPic] == PeriodSymbol)
            {
              checkFloat (picture, idxPic, outArea, idxDest, sign);
              sigFlag = TRUE;
            }

          outArea[idxDest ++] = picture[idxPic];
          break;
        default :
          outArea[idxDest ++] = picture[idxPic];
          break;
        }
    }
  outArea[idxDest] = '\0';
  /*
    Add special leading / trailing sign flag.  */
  if (FoundSignFlag)
    {
      if (sigFlag)
        {
          if (overpunchSign)
            {
              if (leadSign)
                i = sigFirst;
              else
                i = sigLast;
              if (sign == '-')
                outArea[i] = (char)blir_overpunch_set ('-', outArea[i]);
              else
                outArea[i] = (char)blir_overpunch_set ('+', outArea[i]);
            }
          else
            {
              if (leadSign)
                i = sigFirst;
              else
                i = sigLast + 1;
              for (j = idxDest + 1; j >= i; j --)
                outArea[j] = outArea[j - 1];
              outArea[i] = sign;
            }
        }
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emEditMove - carry out an numeric edit move                   *
 *                                                                    *
 *********************************************************************  */

int32
blir_emEditMove (void *dest,  /* Destination.  */
                 size_t destLen,  /* Destination Length.  */
                 int32 destType,  /* Destination Type.  */
                 int32 destFlags,  /* Destination Flags.  */
                 const uchar *picture,  /* Destination Encoded Picture (from blir_emEditMove_Analyze).  */
                 int32 destDigitTotal,  /* Total digits in number (from blir_emEditMove_Analyze).  */
                 int32 destDigitLeft,  /* Digits to left of decimal in destination (from blir_emEditMove_Analyze).  */
                 int32 sourceDigitRight,  /* Digits to right of decimal in source (from blir_emEditMove_Analyze).  */
                 const void *source,  /* Source.  */
                 size_t sourceLen,  /* Source Length.  */
                 int32 sourceType,  /* Source Type.  */
                 int32 sourceFlags) /* Source Flags.  */
{
  uchar destExtract[posWIDTH];  /* Destination Number.  */
  int32 i;  /* Index.  */
  int32 i32;  /* Integer 32.  */
  int64 i64;  /* Integer 64.  */
#if 0
  int128 i128;  /* Integer 128.  */
#endif
  uint32 u32;  /* Unsigned Integer 32.  */
  uint64 u64;  /* Unsigned Integer 64.  */
#if 0
  uint128 u128;  /* Unsigned Integer 128.  */
#endif
  bli_float_64 fdouble;  /* Double precision.  */
#if 0
  BLI_FLOAT_LONG_DOUBLE flongdouble;  /* Long double precision.  */
#endif
  int32 decimalPresent;  /* Data form implies decimal.  */
  int32 selPos;  /* Select Position.  */
  int32 leadSign;  /* Leading sign.  */
  int32 overSign;  /* Overpunch sign.  */
  /*
    Intialize variables.  */
  memset (destExtract, '0', sizeof (destExtract));
  destExtract[posSign] = '+';
  destExtract[posEOS] = '\0';
  leadSign = FALSE;  /* Default leading sign, also requires 'S' in Picture.  */
  overSign = FALSE;  /* No overpunch of sign, also requires 'S' in Picture.  */
  /*
    Convert input to standard format.  */
  switch (mergeFlags (sourceType, sourceFlags, sourceLen))
    {
    case usageint8 :
      i32 =  * ((schar *)source);
      getInt32 (destExtract, i32);
      decimalPresent = FALSE;
      break;
    case usageint16 :
      i32 =  * ((int16 *)source);
      getInt32 (destExtract, i32);
      decimalPresent = FALSE;
      break;
    case usageint32 :
      i32 =  * ((int32 *)source);
      getInt32 (destExtract, i32);
      decimalPresent = FALSE;
      break;
    case usageint64 :
      i64 =  * ((int64 *)source);
      getInt64 (destExtract, i64);
      decimalPresent = FALSE;
      break;
#if 0
    case usageint128 :
      i128 =  * ((int128 * )source);
      getInt128 (destExtract, i128);
      decimalPresent = FALSE;
      break;
#endif
    case usageuchar :
      u32 =  * ((uchar *)source);
      getUInt32 (destExtract, u32);
      decimalPresent = FALSE;
      break;
    case usageuint16 :
      u32 =  * ((uint16 *)source);
      getUInt32 (destExtract, u32);
      decimalPresent = FALSE;
      break;
    case usageuint32 :
      u32 =  * ((uint32 *)source);
      getUInt32 (destExtract, u32);
      decimalPresent = FALSE;
      break;
    case usageuint64 :
      u64 =  * ((uint64 *)source);
      getUInt64 (destExtract, u64);
      decimalPresent = FALSE;
      break;
#if 0
    case usageuint128 :
      u128 =  * ((uint128 * )source);
      getUInt128 (destExtract, u128);
      decimalPresent = FALSE;
      break;
#endif
    case usagebli_float_32 :
      fdouble =  * ((bli_float_32 *)source);
      getbli_float_64 (destExtract, fdouble);
      decimalPresent = TRUE;
      break;
    case usagebli_float_64 :
      fdouble =  * ((bli_float_64 *)source);
      getbli_float_64 (destExtract, fdouble);
      decimalPresent = TRUE;
      break;
#if 0
    case usageBLI_FLOAT_LONG_DOUBLE :
      flongdouble =  * ((BLI_FLOAT_LONG_DOUBLE *)source);
      getBLI_FLOAT_LONG_DOUBLE (destExtract, flongdouble);
      decimalPresent = TRUE;
      break;
#endif
    case usagePacked :
      getPacked (destExtract, source, sourceLen);
      decimalPresent = FALSE;
      break;
    case usageDisplay :
      getDisplay (destExtract, source, sourceLen,  & decimalPresent);
      break;
    case usageDispSignLeadSep :
      leadSign = TRUE;
      getDisplay (destExtract, source, sourceLen,  & decimalPresent);
      break;
    case usageDispSignTrailSep :
      leadSign = FALSE;
      getDisplay (destExtract, source, sourceLen,  & decimalPresent);
      break;
    case usageDispSignLead :
      leadSign = TRUE;
      overSign = TRUE;
      getDisplayOverpunch (destExtract, source, sourceLen,  & decimalPresent, TRUE);
      break;
    case usageDispSignTrail :
      leadSign = FALSE;
      overSign = TRUE;
      getDisplayOverpunch (destExtract, source, sourceLen,  & decimalPresent, FALSE);
      break;
    default :
      stub ((uchar *)"Unknown Input Format was passed!");
      break;
    }

  if (decimalPresent)
    selPos = posFract0 - destDigitLeft;
  else
    selPos = posFract0 - sourceDigitRight - destDigitLeft;

  while (selPos < posInt0)
    {
      for (i = posFractN; i > posInt0; i --) /* Shift right.  */
        destExtract[i] = destExtract[i - 1];
      destExtract[posInt0] = '0';
      selPos ++;
    }
  /*
    Process the output format.  */
  switch (mergeFlags (destType, destFlags, destLen))
    {
    case usageDisplay :
      putDisplay (dest,
                  destLen,
                  picture,
                  * destExtract,
                  & destExtract[selPos],
                  leadSign,
                  overSign);
      break;
    case usageint8 :
      putInt8 (dest, destDigitTotal,  * destExtract,  & destExtract[selPos]);
      break;
    case usageint16 :
      putInt16 (dest, destDigitTotal,  * destExtract,  & destExtract[selPos]);
      break;
    case usageint32 :
      putInt32 (dest, destDigitTotal,  * destExtract,  & destExtract[selPos]);
      break;
    case usageint64 :
      putInt64 (dest, destDigitTotal,  * destExtract,  & destExtract[selPos]);
      break;
#if 0
    case usageint128 :
      putInt128 (dest, destDigitTotal,  * destExtract,  & destExtract[selPos]);
      break;
#endif
    case usageuchar :
      putUchar (dest, destDigitTotal,  & destExtract[selPos]);
      break;
    case usageuint16 :
      putUInt16 (dest, destDigitTotal,  & destExtract[selPos]);
      break;
    case usageuint32 :
      putUInt32 (dest, destDigitTotal,  & destExtract[selPos]);
      break;
    case usageuint64 :
      putUInt64 (dest, destDigitTotal,  & destExtract[selPos]);
      break;
#if 0
    case usageuint128 :
      putUInt128 (dest, destDigitTotal,  & destExtract[selPos]);
      break;
#endif
    case usagebli_float_32 :
      putbli_float_32 (dest, destDigitTotal, destExtract, selPos);
      break;
    case usagebli_float_64 :
      putbli_float_64 (dest, destDigitTotal, destExtract, selPos);
      break;
#if 0
    case usageBLI_FLOAT_LONG_DOUBLE :
      putBLI_FLOAT_LONG_DOUBLE (dest, destDigitTotal, destExtract, selPos);
      break;
#endif
    case usagePacked :
      putPacked (dest, destLen, destDigitTotal,  * destExtract,  & destExtract[selPos]);
      break;
    case usageDispSignLeadSep :
      leadSign = TRUE;
      overSign = FALSE;
      putDisplay (dest,
                  destLen,
                  picture,
                  * destExtract,
                  & destExtract[selPos],
                  leadSign,
                  overSign);
      break;
    case usageDispSignTrailSep :
      leadSign = FALSE;
      overSign = FALSE;
      putDisplay (dest,
                  destLen,
                  picture,
                  * destExtract,
                  & destExtract[selPos],
                  leadSign,
                  overSign);
      break;
    case usageDispSignLead :
      leadSign = TRUE;
      overSign = TRUE;
      putDisplay (dest,
                  destLen,
                  picture,
                  * destExtract,
                  & destExtract[selPos],
                  leadSign,
                  overSign);
      break;
    case usageDispSignTrail :
      leadSign = FALSE;
      overSign = TRUE;
      putDisplay (dest,
                  destLen,
                  picture,
                  * destExtract,
                  & destExtract[selPos],
                  leadSign,
                  overSign);
      break;
    default :
      stub ((uchar *)"Unknown Output Format!");
      break;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emSetCurrencySymbol - Set the currency symbol                 *
 *                                                                    *
 *********************************************************************  */

int32
blir_emSetCurrencySymbol (uchar symbol)
{
  CurrencySymbol = symbol;
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emSetDecimalIsComma - Set the Decimal Is Comma flag           *
 *                                                                    *
 *********************************************************************  */

int32
blir_emSetDecimalIsComma (int32 flag)
{
  if (flag)
    {
      PeriodSymbol = ',';
      CommaSymbol = '.';
      DecimalIsComma = TRUE;
    }
  else
    {
      PeriodSymbol = '.';
      CommaSymbol = ',';
      DecimalIsComma = FALSE;
    }
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emUnedit - De-edit an edited display field                    *
 *                                                                    *
 *********************************************************************  */


uchar * /* Destination.  */
blir_emUnedit (void *dest,  /* Destination.  */
               size_t destLen,  /* Destination Length.  */
               const uchar *uneditPic,  /* Unedit Encoded Picture (from blir_emUnedit_Analyze).  */
               int32 uneditDigits,  /* Unedit Digits (from blir_emUnedit_Analyze).  */
               const void *source,  /* Source.  */
               int32 sourceType,  /* Source Type.  */
               int32 sourceFlags) /* Source Flags.  */
{
  uchar *outArea;  /* Destination pointer.  */
  uchar *inArea;  /* Source area.  */
  int32 idxPic;  /* Picture Index.  */
  int32 idxDest;  /* Destination Index.  */
  uchar sign;  /* Implied Sign.  */
  uchar ch;  /* Work character.  */
  int32 SignFlag;  /* Flags special sign uchar.  */
  int32 SignOver;  /* Flags overpunch sign.  */
  int32 SignLead;  /* Flags leading sign.  */
  int32 digit;  /* Current digit being processed.  */
  int32 digitMax;  /* Maximum digits index.  */
  int32 FoundSignFlag;  /* Found 'S' picture element.  */
  int32 overSign;  /* Overpunched sign.  */
  int32 overChar;  /* Overpunched uchar.  */

  switch (mergeFlags (sourceType, sourceFlags, destLen))
    {
    case usageDisplay :
    case usageDispSignLeadSep :
    case usageDispSignTrailSep :
      SignFlag = FALSE;
      SignOver = FALSE;
      SignLead = FALSE;
      break;
    case usageDispSignLead :
      SignFlag = TRUE;
      SignOver = TRUE;
      SignLead = TRUE;
      break;
    case usageDispSignTrail :
      SignFlag = TRUE;
      SignOver = TRUE;
      SignLead = FALSE;
      break;
    default :
      stub ((uchar *)"Only display types supported for blir_emUnedit");
      return (uchar *)NULL;
      break;
    }

  if (destLen < 3) /* Need room for at least +9\0.  */
    return (uchar *)NULL;

  inArea = (uchar *)source;
  outArea = (uchar *)dest;
  idxDest = 0;
  sign = outArea[idxDest ++] = '+';  /* Save place to hold sign.  */
  digit = 0;
  digitMax = uneditDigits + 1;
  FoundSignFlag = FALSE;

  for (idxPic = 0; uneditPic[idxPic] != '\0'; idxPic ++)
    {
      switch (uneditPic[idxPic])
        {
        case '9' :
        case SUPSPACE :
        case SUPASTERISK :
        case SUPCURRENCY :
        case SUPPLUS :
        case SUPMINUS :
          digit ++;
          ch = inArea[idxPic];

          if (SignFlag && FoundSignFlag)
            {
              if (SignLead && (digit == 1))
                {
                  blir_overpunch_get ( & overSign,  & overChar, ch);
                  ch = overChar;
                  if (overSign < 0)
                    sign = '-';
                  else
                    sign = '+';
                }
              else if (digit == digitMax)
                {
                  blir_overpunch_get ( & overSign,  & overChar, ch);
                  ch = overChar;
                  if (overSign < 0)
                    sign = '-';
                  else
                    sign = '+';
                }
            }

          if (isdigit (ch))
            {
              if (idxDest < ((int)(destLen - 1)))
                outArea[idxDest ++] = ch;
            }
          else if (ch == '+')
            sign = '+';
          else if (ch == '-')
            sign = '-';
          else
            {
              blir_overpunch_get ( & overSign,  & overChar, ch);
              ch = overChar;
              if (overSign < 0)
                sign = '-';
              else
                sign = '+';
              if (idxDest < ((int)(destLen - 1)))
                outArea[idxDest ++] = overChar;
            }

          break;
        case SUPCOMMAPLUS :
        case SUPCOMMAMINUS :
          ch = inArea[idxPic];
          if (ch == '+')
            sign = '+';
          else if (ch == '-')
            sign = '-';
          break;
        case SIGNPLUS :
          ch = inArea[idxPic];
          if (ch == '+')
            sign = '+';
          else if (ch == '-')
            sign = '-';
          break;
        case SIGNMINUS :
          ch = inArea[idxPic];
          if (ch == '-')
            sign = '-';
          else
            sign = '+';
          break;
        case SIGNCR :
          if (memcmp ( & inArea[idxPic], "CR", 2) == 0)
            sign = '-';
          else
            sign = '+';
          idxPic ++;  /* Skip "R" of "CR".  */
          break;
        case SIGNDB :
          if (memcmp ( & inArea[idxPic], "DB", 2) == 0)
            sign = '-';
          else
            sign = '+';
          idxPic ++;  /* Skip "B" of "DB".  */
          break;
        case SIGNS :
          FoundSignFlag = TRUE;
          break;
        default :
          break;  /* Ignored.  */
        }
    }
  outArea[idxDest] = '\0';
  outArea[0] = sign;

  return outArea;
}


/**********************************************************************
 *                                                                    *
 * Cobr_emEditMove_Analyze - Analyze the source & destination         *
 * pictures                                                           *
 *                                                                    *
 *********************************************************************  */

uchar *
blir_emEditMove_Analyze (uchar *picture,  /* Output Picture.  */
                         int32 *destDigitTotal,  /* Digits in destination.  */
                         int32 *destDigitLeft,  /* Destination digits left of decimal.  */
                         int32 *sourceDigitRight,  /* Source digits right of decimal.  */
                         const uchar *destPic,  /* Destination Picture.  */
                         const uchar *sourcePic) /* Source Picture.  */
{
  uchar workPic[256];  /* Work picture.  */
  int32 DigitLeft;  /* Digits to left.  */
  int32 DigitRight;  /* Digits to right.  */

  analyzePicture (workPic, sourcePic,  & DigitLeft,  & DigitRight);

  * sourceDigitRight = DigitRight;

  analyzePicture (picture, destPic,  & DigitLeft,  & DigitRight);

  * destDigitLeft = DigitLeft;

  * destDigitTotal = DigitLeft + DigitRight;

  return picture;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emUnedit_Analyze - Analyze picture for unediting (de-editing) *
 * a display field                                                    *
 *                                                                    *
 *********************************************************************  */


uchar * /* UneditPicture.  */
blir_emUnedit_Analyze (uchar *editPicture,  /* Edit Picture (for blir_em_editMove).  */
                       uchar *uneditPicture,  /* Unedit picture (for blir_emUnedit).  */
                       int32 *uneditDigits,  /* Digits in unedited picture (for blir_emUnedit).  */
                       const uchar *sourcePic) /* Source Picture.  */
{
  int32 DigitLeft;  /* Digits to left.  */
  int32 DigitRight;  /* Digits to right.  */
  int32 i;  /* Destination pointer.  */

  analyzePicture (uneditPicture, sourcePic,  & DigitLeft,  & DigitRight);

  * uneditDigits = DigitLeft + DigitRight;

  i = 0;

  editPicture[i ++] = '+';

  while ((DigitLeft --) > 0)
    editPicture[i ++] = '9';

  if (DigitRight > 0)
    {
      editPicture[i ++] = '.';
      while ((DigitRight --) > 0)
        editPicture[i ++] = '9';
    }

  editPicture[i] = '\0';

  return uneditPicture;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emUneditMove_Analyze - Analysis routine for move of preedited *
 * field                                                              *
 *                                                                    *
 *********************************************************************  */

int32
blir_emUneditMove_Analyze (uchar *Picture,  /* Encoded Output Picture.  */
                           int32 *destDigitTotal,  /* Digits in destination.  */
                           int32 *destDigitLeft,  /* Destination digits left of decimal.  */
                           int32 *sourceDigitRight,  /* Source digits right of decimal.  */
                           uchar *uneditPicture,  /* Unedit Picture.  */
                           int32 *uneditDigits,  /* Digits in unedited picture.  */
                           const uchar *destPic,  /* Original Destination Picture.  */
                           const uchar *sourcePic) /* Original Source Picture.  */
{
  uchar newSourcePicture[256];  /* New source picture.  */

  blir_emUnedit_Analyze (newSourcePicture, uneditPicture, uneditDigits, sourcePic);

  blir_emEditMove_Analyze (Picture, destDigitTotal, destDigitLeft, sourceDigitRight, destPic, newSourcePicture);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_emUneditMove - move a preedited field                         *
 *                                                                    *
 *********************************************************************  */

int32
blir_emUneditMove (void *dest,  /* Destination.  */
                   size_t destLen,  /* Destination Length.  */
                   int32 destType,  /* Destination Type.  */
                   int32 destFlags,  /* Destination Flags.  */
                   const uchar *Picture,  /* Destination Encoded Picture.  */
                   int32 destDigitTotal,  /* Total digits in number.  */
                   int32 destDigitLeft,  /* Digits to left of decimal in destination.  */
                   int32 sourceDigitRight,  /* Digits to right of decimal in source.  */
                   const uchar *uneditPicture,  /* Picture for unedited data.  */
                   int32 uneditDigits,  /* Digits in unedited picture.  */
                   const void *source,  /* Source.  */
                   int32 sourceType,  /* Source Type.  */
                   int32 sourceFlags) /* Source Flags.  */
{
  uchar newSource[256];  /* Destination.  */

  blir_emUnedit (newSource, sizeof (newSource),
                 uneditPicture, uneditDigits, source, sourceType, sourceFlags);

  blir_emEditMove (dest, destLen, destType, destFlags,
                   Picture, destDigitTotal, destDigitLeft, sourceDigitRight,
                   newSource, strlen ((char *)newSource), blir_typeDisplay, blir_flagNull);

  return 0;
}
