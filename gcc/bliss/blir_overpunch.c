

/**********************************************************************
 *                                                                    *
 *                    Convert Overpunch characters                    *
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
 * Mail: Theodore J. Seward, Jr.                                      *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title: Convert overpunch characters                                *
 *                                                                    *
 * Description: These routines will convert ot and from overpunched   *
 *                 characters in the prevailing characterset (EBCDIC  *
 *                 or ASCII).                                         *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward 30/Dec/2000 Initial Release of Routines                  *
 *                                                                    *
 *********************************************************************  */

#include "blir_temp_config.h"
#include "blir_overpunch.h"

/**********************************************************************
 *                                                                    *
 * CheckASCII - Check ASCII character set                             *
 *                                                                    *
 *********************************************************************  */


static int32
checkASCII (int32 charA) /* Character A value.  */
{
  if (charA == 65)
    return 1;
  else
    return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_overpunch_set - Generate overpunched sign                     *
 *                                                                    *
 *********************************************************************  */

uchar /* Overpunched character.  */
blir_overpunch_set (int32 inSign,  /* Input Sign.  */
                    int32 inChar) /* Input Character.  */
{
  uchar ch;  /* To character.  */

  if (!checkASCII ('A')) /* EBCDIC.  */
    {
      if (inSign == '+')
        ch = (uchar)((inChar & 0x0F) | 0xC0);
      else
        ch = (uchar)((inChar & 0x0F) | 0xD0);
    }
  else /* ASCII.  */
    {
      if (inSign == '+')
        {
          if (inChar == '0')
            ch = '{';
          else
            ch = (uchar)(inChar - '1' + 'A');
        }
      else
        {
          if (inChar == '0')
            ch = '}';
          else
            ch = (uchar)(inChar - '1' + 'J');
        }
    }

  return ch;
}


/**********************************************************************
 *                                                                    *
 * Cobr_overpunch_get - Translate overpunch characters                *
 *                                                                    *
 * It seems that ASCII uses the same characters for the overpunched   *
 * signs, but it uses the ASCII equivalent.  For example, +1 would be *
 * A (0xC1) in EBCDIC, in an ASCII display value it is still A, but   *
 * the ASCII A (0x41).                                                *
 *                                                                    *
 *********************************************************************  */

int32
blir_overpunch_get (int32 *outSign,  /* Output Sign.  */
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
       * Positive overpunchs                                                *
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
       * Negative overpunches                                               *
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
