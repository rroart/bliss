
/*                                                                    *
 *                       BLISS Display Function                       *
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
 * Mail:   Theodore J. Seward, Jr.                                    *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *  */


/**********************************************************************
 *                                                                    *
 * Title:  BLISS Display Function                                     *
 *                                                                    *
 * Description:    This file is part of the BLISS Display Function.   *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward       21/Aug/2000     Initial Release of Routines        *
 *                                                                    *
 *********************************************************************  */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "blir_display.h"
#include "blir_editMove.h"
#include "blir_temp_config.h"
#include "blir_globals.h"

#define outFile stdout          /* Output File */

/**********************************************************************
 *                                                                    *
 * GetOverpunch - Translate overpunch characters                      *
 *                                                                    *
 * It seems that ASCII uses the same characters for the overpunched   *
 * signs, but it uses the ASCII equivalent.  For example, +1 would be *
 * A (0xC1) in EBCDIC, in an ASCII display value it is still A, but   *
 * the ASCII A (0x41).                                                *
 *                                                                    *
 *********************************************************************  */

static int32
getOverpunch (int32 *outSign,  /* Output Sign.  */
              int32 *outChar,  /* Output Character.  */
              int32 inChar) /* Input Character.  */
{
  int32 ch;  /* To character.  */
  int32 sign;  /* To Sign.  */

  switch (inChar)
    {

      /*--------------------------------------------------------------------*
       * Standard Numbers                                                   *
       *--------------------------------------------------------------------.  */

    case 0x30 :  /* ASCII.  */
    case 0xF0 :  /* EBCDIC.  */
      ch = '0';
      sign = 1;
      break;
    case 0x31 :  /* ASCII.  */
    case 0xF1 :  /* EBCDIC.  */
      ch = '1';
      sign = 1;
      break;
    case 0x32 :  /* ASCII.  */
    case 0xF2 :  /* EBCDIC.  */
      ch = '2';
      sign = 1;
      break;
    case 0x33 :  /* ASCII.  */
    case 0xF3 :  /* EBCDIC.  */
      ch = '3';
      sign = 1;
      break;
    case 0x34 :  /* ASCII.  */
    case 0xF4 :  /* EBCDIC.  */
      ch = '4';
      sign = 1;
      break;
    case 0x35 :  /* ASCII.  */
    case 0xF5 :  /* EBCDIC.  */
      ch = '5';
      sign = 1;
      break;
    case 0x36 :  /* ASCII.  */
    case 0xF6 :  /* EBCDIC.  */
      ch = '6';
      sign = 1;
      break;
    case 0x37 :  /* ASCII.  */
    case 0xF7 :  /* EBCDIC.  */
      ch = '7';
      sign = 1;
      break;
    case 0x38 :  /* ASCII.  */
    case 0xF8 :  /* EBCDIC.  */
      ch = '8';
      sign = 1;
      break;
    case 0x39 :  /* ASCII.  */
    case 0xF9 :  /* EBCDIC.  */
      ch = '9';
      sign = 1;
      break;

      /*--------------------------------------------------------------------*
       * Positive overpunch                                                 *
       *--------------------------------------------------------------------.  */

    case 0x7B :  /* ASCII.  */
    case 0xC0 :  /* EBCDIC.  */
      ch = '0';
      sign = 1;
      break;
    case 0x41 :  /* ASCII.  */
    case 0xC1 :  /* EBCDIC.  */
      ch = '1';
      sign = 1;
      break;
    case 0x42 :  /* ASCII.  */
    case 0xC2 :  /* EBCDIC.  */
      ch = '2';
      sign = 1;
      break;
    case 0x43 :  /* ASCII.  */
    case 0xC3 :  /* EBCDIC.  */
      ch = '3';
      sign = 1;
      break;
    case 0x44 :  /* ASCII.  */
    case 0xC4 :  /* EBCDIC.  */
      ch = '4';
      sign = 1;
      break;
    case 0x45 :  /* ASCII.  */
    case 0xC5 :  /* EBCDIC.  */
      ch = '5';
      sign = 1;
      break;
    case 0x46 :  /* ASCII.  */
    case 0xC6 :  /* EBCDIC.  */
      ch = '6';
      sign = 1;
      break;
    case 0x47 :  /* ASCII.  */
    case 0xC7 :  /* EBCDIC.  */
      ch = '7';
      sign = 1;
      break;
    case 0x48 :  /* ASCII.  */
    case 0xC8 :  /* EBCDIC.  */
      ch = '8';
      sign = 1;
      break;
    case 0x49 :  /* ASCII.  */
    case 0xC9 :  /* EBCDIC.  */
      ch = '9';
      sign = 1;
      break;

      /*--------------------------------------------------------------------*
       * Negative overpunch                                                 *
       *--------------------------------------------------------------------.  */

    case 0x7D :  /* ASCII.  */
    case 0xD0 :  /* EBCDIC.  */
      ch = '0';
      sign = -1;
      break;
    case 0x4A :  /* ASCII.  */
    case 0xD1 :  /* EBCDIC.  */
      ch = '1';
      sign = -1;
      break;
    case 0x4B :  /* ASCII.  */
    case 0xD2 :  /* EBCDIC.  */
      ch = '2';
      sign = -1;
      break;
    case 0x4C :  /* ASCII.  */
    case 0xD3 :  /* EBCDIC.  */
      ch = '3';
      sign = -1;
      break;
    case 0x4D :  /* ASCII.  */
    case 0xD4 :  /* EBCDIC.  */
      ch = '4';
      sign = -1;
      break;
    case 0x4E :  /* ASCII.  */
    case 0xD5 :  /* EBCDIC.  */
      ch = '5';
      sign = -1;
      break;
    case 0x4F :  /* ASCII.  */
    case 0xD6 :  /* EBCDIC.  */
      ch = '6';
      sign = -1;
      break;
    case 0x50 :  /* ASCII.  */
    case 0xD7 :  /* EBCDIC.  */
      ch = '7';
      sign = -1;
      break;
    case 0x51 :  /* ASCII.  */
    case 0xD8 :  /* EBCDIC.  */
      ch = '8';
      sign = -1;
      break;
    case 0x52 :  /* ASCII.  */
    case 0xD9 :  /* EBCDIC.  */
      ch = '9';
      sign = -1;
      break;

      /*--------------------------------------------------------------------*
       * Unknown, assume +0                                                 *
       *--------------------------------------------------------------------.  */

    default :
      ch = '0';
      sign = 1;
      break;
    }
  * outChar = ch;
  * outSign = sign;

  return sign;
}


/**********************************************************************
 *                                                                    *
 * GetDisplayOverpunch - Get Display Overpunch text to internal       *
 * format                                                             *
 *                                                                    *
 *********************************************************************  */

static int32
getDisplayOverpunch (uchar *destExtract,  /* Internal format.  */
                     int32 *fractLen,  /*  Fraction Length.  */
                     const void *source,  /* Area to convert.  */
                     int32 sourceLen,  /* Length of area.  */
                     int32 lead) /* Leading sign.  */
{
  uchar workExtract[128];  /* Work area.  */
  uchar *display;  /* Display area.  */
  int32 i, j, s, p;  /* Indexes.  */
  uchar sign;  /* Sign.  */
  uchar ch;  /* Work character.  */
  uchar *cptr;  /* Work pointer.  */
  int32 oSign;  /* Overpunch sign.  */
  int32 oChar;  /* Overpunch char.  */

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
          getOverpunch ( & oSign,  & oChar, workExtract[0]);
          workExtract[0] = oChar;
          if (oSign < 0)
            sign = '-';
          else
            sign = '+';
        }
      else
        {
          i = j - 1;

          getOverpunch ( & oSign,  & oChar, workExtract[i]);
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
    }

  i = 0;
  * fractLen = 0;
  destExtract[i ++] = sign;
  for (p = 0; p < j; p ++)
    if (isdigit (workExtract[p]))
      destExtract[i ++] = workExtract[p];
  for (p = j + 1; workExtract[p] != '\0'; p ++)
    if (isdigit (workExtract[p]))
      {
        destExtract[i ++] = workExtract[p];
        * fractLen += 1;
      }
  destExtract[p] = '\0';

  return 0;
}


/**********************************************************************
 *                                                                    *
 * GetDisplay - Get Display text to internal format                   *
 *                                                                    *
 *********************************************************************  */

static int32
getDisplay (uchar *destExtract,  /* Internal format.  */
            int32 *fractLen,  /* Fraction Length.  */
            const void *source,  /* Area to convert.  */
            int32 sourceLen) /* Length of area.  */
{
  uchar workExtract[128];  /* Work area.  */
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
    }

  i = 0;
  * fractLen = 0;
  destExtract[i ++] = sign;
  for (p = 0; p < j; p ++)
    if (isdigit (workExtract[p]))
      destExtract[i ++] = workExtract[p];
  for (p = j + 1; workExtract[p] != '\0'; p ++)
    if (isdigit (workExtract[p]))
      {
        destExtract[i ++] = workExtract[p];
        * fractLen += 1;
      }
  destExtract[p] = '\0';

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
  uchar workExtract[128];  /* Work area.  */
  uchar *pd;  /* Pointer to packed.  */
  int32 i, p;  /* Index.  */
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

  i = 0;
  destExtract[i ++] = sign;
  for (p = 0; workExtract[p] != '\0'; p ++)
    destExtract[i ++] = workExtract[p];
  destExtract[i] = '\0';

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutHex - Write literal in hex                                      *
 *                                                                    *
 *********************************************************************  */

static int32
putHex (FILE *file,  /* Output File.  */
        const void *value,  /* Value.  */
        int32 length) /* Length of value.  */
{
  int32 i;  /* Index.  */
  uchar *cptr;  /* Character pointer.  */

  cptr = (uchar *)value;

  if (length >= 1)
    {
      for (i = 0; i < length; i ++)
        fprintf (file, "\\x%.02X", cptr[i]);
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutText - Write out text                                           *
 *                                                                    *
 *********************************************************************  */

static int32
putText (FILE *file,  /* Output File.  */
         const void *value,  /* Value.  */
         int32 length) /* Length of value.  */
{
  int32 i;  /* Index.  */
  uchar *cptr;  /* Character pointer.  */

  cptr = (uchar *)value;

  for (i = 0; i < length; i ++)
    fputc (cptr[i], file);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutItem - Write a item with fraction                               *
 *                                                                    *
 *********************************************************************  */

static int32
putItem (FILE *file,  /* Output File.  */
         uchar *field,  /* Character version of field.  */
         uchar sign,  /* Sign of field.  */
         int32 fraction) /* Digits in fraction.  */
{
  uchar work[128];  /* Work area 1.  */
  int32 lenMin;  /* Minumum output length.  */
  int32 lenCur;  /* Current output length.  */
  int32 partInt;  /* Length of integer part.  */
  int32 partFract;  /* Length of fraction part.  */
  uchar ch;  /* Work character.  */
  int32 sigFig;  /* Significant digit.  */
  int32 i;  /* Integer.  */

  lenCur = strlen ((char *)field);
  lenMin = fraction + 1;

  if (lenCur < lenMin)
    {
      memset (work, '0', lenMin);
      work[lenMin] = '\0';
      memcpy (work + lenMin - lenCur, field, lenCur);
      lenCur = strlen ((char *)work);
    }
  else
    strcpy ((char *)work, (char *)field);

  if (sign != '+')
    fputc (sign, file);

  partInt = lenCur - fraction;
  partFract = fraction;

  ch = work[partInt];
  work[partInt] = '\0';
  sigFig = FALSE;
  for (i = 0; work[i] != '\0'; i ++)
    if (sigFig)
      fputc (work[i], file);
    else
      {
        if (work[i] != '0')
          sigFig = TRUE;
        if (sigFig)
          fputc (work[i], file);
      }
  if (!sigFig)
    fputc ('0', file);

  if (fraction > 0)
    {
      work[partInt] = ch;
      fputc ('.', file);
      fputs ((native_char*)& work[partInt], file);
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutBLI_INT - Write Signed Integer                                  *
 *                                                                    *
 *********************************************************************  */

static int32
putBLI_INT (FILE *file,  /* Output File.  */
            const void *value,  /* Value.  */
            int32 length,  /* Length.  */
            int32 fraction) /* Fraction.  */
{
  uchar sign;  /* Sign.  */
  uchar work[128];  /* Work area.  */
  uchar *str;  /* Pointer to digits.  */
  int32 i32;  /* 32 Bit value.  */

  if (fraction <= 0)
    {
      if (length == BLI_INT8_WIDTH)
        {
          i32 =  * ((schar *)value);
          fprintf (file, "%d", i32);
        }
      else if (length == BLI_INT16_WIDTH)
        {
          i32 =  * ((int16 *)value);
          fprintf (file, "%d", i32);
        }
      else if (length == BLI_UINT32_WIDTH)
        fprintf (file, "%d",  * ((int32 *)value));
      else if (length == BLI_INT64_WIDTH)
        fprintf (file, "%lld",  * ((int64 *)value));
#if 0
      else if (length == BLI_INT128_WIDTH)
        fprintf (file, "%lld",  * ((BLI_INT128 * )value));
#endif
      else
        fprintf (file, "%d",  * ((int32 *)value));
      return 0;
    }

  if (length == BLI_INT8_WIDTH)
    {
      i32 =  * ((schar *)value);
      sprintf ((char *)work, "%d", i32);
    }
  else if (length == BLI_INT16_WIDTH)
    {
      i32 =  * ((int16 *)value);
      sprintf ((char *)work, "%d", i32);
    }
  else if (length == BLI_UINT32_WIDTH)
    sprintf ((char *)work, "%d",  * ((int32 *)value));
  else if (length == BLI_INT64_WIDTH)
    sprintf ((char *)work, "%lld",  * ((int64 *)value));
#if 0
  else if (length == BLI_INT128_WIDTH)
    sprintf (work, "%lld",  * ((BLI_INT128 * )value));
#endif
  else
    sprintf ((char *)work, "%d",  * ((int32 *)value));

  if (work[0] == '+')
    {
      sign = '+';
      str =  & work[1];
    }
  else if (work[0] == '-')
    {
      sign = '-';
      str =  & work[1];
    }
  else
    {
      sign = '+';
      str = work;
    }

  putItem (file, str, sign, fraction);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutBLI_UINT - Write Unsigned Integer                               *
 *                                                                    *
 *********************************************************************  */

static int32
putBLI_UINT (FILE *file,  /* Output File.  */
             const void *value,  /* Value.  */
             int32 length,  /* Length.  */
             int32 fraction) /* Fraction.  */
{
  uchar sign;  /* Sign.  */
  uchar work[128];  /* Work area.  */
  uint32 u32;  /* 32 Bit value.  */

  if (fraction <= 0)
    {
      if (length == BLI_UINT8_WIDTH)
        {
          u32 =  * ((uchar *)value);
          fprintf (file, "%u", u32);
        }
      else if (length == BLI_UINT16_WIDTH)
        {
          u32 =  * ((uint16 *)value);
          fprintf (file, "%u", u32);
        }
      else if (length == BLI_UINT32_WIDTH)
        fprintf (file, "%u",  * ((uint32 *)value));
      else if (length == BLI_UINT64_WIDTH)
        fprintf (file, "%llu",  * ((uint64 *)value));
#if 0
      else if (length == BLI_UINT128_WIDTH)
        fprintf (file, "%llu",  * ((BLI_UINT128 * )value));
#endif
      else
        fprintf (file, "%u",  * ((uint32 *)value));
      return 0;
    }

  if (length == BLI_UINT8_WIDTH)
    {
      u32 =  * ((uchar *)value);
      sprintf ((char *)work, "%u", u32);
    }
  else if (length == BLI_UINT16_WIDTH)
    {
      u32 =  * ((uint16 *)value);
      sprintf ((char *)work, "%u", u32);
    }
  else if (length == BLI_UINT32_WIDTH)
    sprintf ((char *)work, "%u",  * ((uint32 *)value));
  else if (length == BLI_UINT64_WIDTH)
    sprintf ((char *)work, "%llu",  * ((uint64 *)value));
#if 0
  else if (length == BLI_UINT128_WIDTH)
    sprintf (work, "%llu",  * ((BLI_UINT128 * )value));
#endif
  else
    sprintf ((char *)work, "%u",  * ((uint32 *)value));

  sign = '+';

  putItem (file, work, sign, fraction);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutBLI_FLOAT - Write floating point                                *
 *                                                                    *
 *********************************************************************  */

static int32
putBLI_FLOAT (FILE *file,  /* Output File.  */
              const void *value,  /* Field.  */
              int32 length,  /* Length of field.  */
              int32 fraction) /* Fraction.  */
{
  uchar work[128];  /* Work area 1.  */
  uchar sign;  /* Sign of number.  */
  uchar *str;  /* Pointer to number portion.  */
  uchar *p;  /* Position of period.  */
  uchar *s;  /* Position of source of copy.  */
  int32 lenFract;  /* Length of fraction.  */
  int32 i;  /* Index.  */

  if (length == BLI_FLOAT_32_WIDTH)
    sprintf ((char *)work, "%f",  * ((bli_float_32 *)value));
  else if (length == BLI_FLOAT_64_WIDTH)
    sprintf ((char *)work, "%f",  * ((bli_float_64 *)value));
#if 0
  else if (length == BLI_FLOAT_LONG_DOUBLE_WIDTH)
    sprintf ((char *)work, "%?l?l?f",  * ((BLI_FLOAT_LONG_DOUBLE *)value));
#endif

  if (work[0] == '+')
    {
      sign = '+';
      str =  & work[1];
    }
  else if (work[0] == '-')
    {
      sign = '-';
      str =  & work[1];
    }
  else
    {
      sign = '+';
      str = work;
    }

  p = (uchar *)strchr ((char *)str, '.');
  if (p == NULL)
    {
      strcat ((char *)str, ".0");
      p = (uchar *)strchr ((char *)str, '.');
    }

  lenFract = strlen ((char *)p + 1);

  if (fraction <= 0)
    {
      * p = '\0';
      fraction = 0;
    }
  else if (lenFract > fraction)
    {
      * (p + fraction + 1) = '\0';
      s = p + 1;
      while ( * s != '\0')
        * (p ++) =  * (s ++);
      * p = '\0';
    }
  else if (lenFract < fraction)
    {
      for (i = lenFract + 1; i <= fraction; i ++)
        strcat ((char *)str, "0");
      s = p + 1;
      while ( * s != '\0')
        * (p ++) =  * (s ++);
      * p = '\0';
    }

  putItem (file, str, sign, fraction);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutDisplay - Write Display version of number                       *
 *                                                                    *
 *********************************************************************  */

static int32
putDisplay (FILE *file,  /* Output File.  */
            const void *value,  /* Field.  */
            int32 length,  /* Length of field.  */
            int32 fraction) /* Fraction.  */
{
  uchar work[128];  /* Work area.  */
  uchar sign;  /* Sign of number.  */
  uchar *str;  /* Pointer to number portion.  */
  int32 fractLen;  /* Fraction Length.  */
  int32 fullLen;  /* Full Length.  */
  int32 i;  /* Index.  */

  getDisplay (work,  & fractLen, value, length);

  if (fraction <= 0)
    {
      if (work[0] == '+')
        {
          sign = '+';
          str =  & work[1];
        }
      else if (work[1] == '-')
        {
          sign = '-';
          str =  & work[1];
        }
      else
        {
          sign = '+';
          str = work;
        }
      putItem (file, str, sign, fractLen);
      return 0;
    }

  if (fraction > fractLen)
    {
      for (i = fractLen + 1; i <= fraction; i ++)
        strcat ((char *)work, "0");
    }
  else if (fraction < fractLen)
    {
      fullLen = strlen ((char *)work);
      i = fullLen - fractLen;  /* Length without a fraction.  */
      i += fraction;  /* Add requested fraction.  */
      if (i < 2)
        i = 2;
      work[i] = '\0';  /* Trim to correct length.  */
    }

  if (work[0] == '+')
    {
      sign = '+';
      str =  & work[1];
    }
  else if (work[0] == '-')
    {
      sign = '-';
      str =  & work[1];
    }
  else
    {
      sign = '+';
      str = work;
    }

  putItem (file, str, sign, fraction);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutDisplayOverpunch - Write overpunched display number             *
 *                                                                    *
 *********************************************************************  */

static int32
putDisplayOverpunch (FILE *file,  /* Output File.  */
                     const void *value,  /* Field.  */
                     int32 length,  /* Length of field.  */
                     int32 fraction,  /* Fraction.  */
                     int32 lead) /* Leading sign flag.  */
{
  uchar work[128];  /* Work area.  */
  uchar sign;  /* Sign of number.  */
  uchar *str;  /* Pointer to number portion.  */
  int32 fractLen;  /* Fraction Length.  */
  int32 fullLen;  /* Full Length.  */
  int32 i;  /* Index.  */

  getDisplayOverpunch (work,  & fractLen, value, length, lead);

  if (fraction <= 0)
    {
      if (work[0] == '+')
        {
          sign = '+';
          str =  & work[1];
        }
      else if (work[1] == '-')
        {
          sign = '-';
          str =  & work[1];
        }
      else
        {
          sign = '+';
          str = work;
        }
      putItem (file, str, sign, fractLen);
      return 0;
    }

  if (fraction > fractLen)
    {
      for (i = fractLen + 1; i <= fraction; i ++)
        strcat ((char *)work, "0");
    }
  else if (fraction < fractLen)
    {
      fullLen = strlen ((char *)work);
      i = fullLen - fractLen;  /* Length without a fraction.  */
      i += fraction;  /* Add requested fraction.  */
      if (i < 2)
        i = 2;
      work[i] = '\0';  /* Trim to correct length.  */
    }

  if (work[0] == '+')
    {
      sign = '+';
      str =  & work[1];
    }
  else if (work[0] == '-')
    {
      sign = '-';
      str =  & work[1];
    }
  else
    {
      sign = '+';
      str = work;
    }

  putItem (file, str, sign, fraction);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutPacked - Write Packed Decimal Number                            *
 *                                                                    *
 *********************************************************************  */

static int32
putPacked (FILE *file,  /* Output File.  */
           const void *value,  /* Field.  */
           int32 length,  /* Length of field.  */
           int32 fraction) /* Fraction.  */
{
  uchar work[128];  /* Work area.  */
  uchar sign;  /* Sign of number.  */
  uchar *str;  /* Pointer to number portion.  */

  getPacked (work, value, length);  /* Get character version of Packed.  */

  if (fraction == 0)
    {
      if (work[0] == '+')
        fputs ( (native_char*)& work[1], file);
      else
        fputs ((native_char*)work, file);
      return 0;
    }

  if (work[0] == '+')
    {
      sign = '+';
      str =  & work[1];
    }
  else if (work[0] == '-')
    {
      sign = '-';
      str =  & work[1];
    }
  else
    {
      sign = '+';
      str = work;
    }

  putItem (file, str, sign, fraction);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_display_nl - Display newline                                  *
 *                                                                    *
 *********************************************************************  */


void
blir_display_nl ()
{
  blir_display ("\n", 1, 0, blir_typeText, 0, NULL);
}

/**********************************************************************
 *                                                                    *
 * Cobr_display - Display one item                                    *
 *                                                                    *
 *********************************************************************  */


void
blir_display (const void *item,  /* Item to print.  */
              size_t length,  /* Item length.  */
              size_t fraction,  /* Digits after decimal.  */
              int32 type,  /* Item type.  */
              int32 flags,  /* Item flags.  */
              const void *picture) /* Optional Picture, NULL = N/A.  */
{
  int32 testType;  /* Test supported data types.  */
  uchar work[256];  /* Work area.  */
  int32 digTotal;  /* Total Digits.  */
  int32 digLeft;  /* Digits left.  */
  int32 digRight;  /* Digits right.  */
  uchar outPic[128];  /* Output Picture.  */

  if (picture != NULL)
    {
      blir_emEditMove_Analyze (outPic,  & digTotal,  & digLeft,  & digRight,
                               picture, picture);

      blir_emEditMove (work, sizeof (work),
                       blir_typeDisplay, blir_flagSignLead,
                       outPic, digTotal, digLeft, digRight,
                       item, length, type, flags);

      fputs ((native_char *)work, outFile);

      return;
    }

  testType =
    type;  /* & (Cobr_typeText | blir_typeDisplay | blir_typeBinary |
              blir_typeUnsigned | blir_typePacked | blir_typeFloat);
              these aren't flags they are an enumeration.  */

  switch (testType)
    {
    case blir_typeNull :
    case blir_typeText :
      putText (outFile, item, length);
      break;

    case blir_typeDisplay :
      if ((flags & blir_flagOverpunch) == 0)
        putDisplay (outFile, item, length, fraction);
      else if ((flags & blir_flagSignLead) != 0)
        putDisplayOverpunch (outFile, item, length, fraction, TRUE);
      else if ((flags & blir_flagSignTrail) != 0)
        putDisplayOverpunch (outFile, item, length, fraction, FALSE);
      else
        putDisplayOverpunch (outFile, item, length, fraction, TRUE);
      break;
    case blir_typeBinary :
      putBLI_INT (outFile, item, length, fraction);
      break;
    case blir_typeUnsigned :
      putBLI_UINT (outFile, item, length, fraction);
      break;
    case blir_typePacked :
      putPacked (outFile, item, length, fraction);
      break;
    case blir_typeFloat :
      putBLI_FLOAT (outFile, item, length, fraction);
      break;

    case blir_typePointer :
      /* Drop through.  */
    default :
      putHex (outFile, item, length);
      break;
    }

}
