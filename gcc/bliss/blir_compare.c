/**********************************************************************
 *                                                                    *
 *              BLISS Compare Function - used in sorting              *
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
 * Title:          BLISS Compare Function                             *
 *                                                                    *
 * Description:    This file is part of the BLISS Compare Function.   *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 *                 BLI_INT32 blir_compare(item1,item2,length,type,flags)  *
 *                                                                    *
 *                 Field     Type    Description                      *
 *                 -------   ------  ------------------------         *
 *                 item1     void *  Left Item of compare             *
 *                 item2     void *  Right Item of compare            *
 *                 length    BLI_SIZE_T  length of items in bytes         *
 *                 type      BLI_INT32     field types (1)                  *
 *                 flags     BLI_INT32     flags for overpunched (2)        *
 *                 sequence  uchar * collating sequence (3)           *
 *                                                                    *
 *                 (1) Symbols for 'type' are found in blir_config.h. *
 *                 You must code one of these for this field.         *
 *                                                                    *
 *                 Types              Description                     *
 *                 -----------------  ------------------              *
 *                 blir_typeNull      Null type                       *
 *                 blir_typeText      Text Type                       *
 *                 blir_typeDisplay   Numeric Usage Disp              *
 *                 blir_typePacked    Packed Decimal                  *
 *                 blir_typeBinary    Signed binary                   *
 *                 blir_typeUnsigned  Unsigned binary                 *
 *                 blir_typeFloat     Floating point                  *
 *                                                                    *
 *                 (2) Symbols for 'flags' are found in               *
 *                 blir_config.h.  You must code one or more of       *
 *                 these.  If more than one are code, they must be    *
 *                 'or'ed together.  For example:                     *
 *                                                                    *
 *                 blir_flagSignLead | blir_flagOverpunch             *
 *                                                                    *
 *                 Flags               Description                    *
 *                 ------------------  -----------------------------  *
 *                 blir_flagNull       Null type                      *
 *                 blir_flagSignLead   Display with sign is leading   *
 *                 blir_flagSignTrail  Display with sign is trailing  *
 *                 blir_flagOverpunch  Display with sign is punched   *
 *                                                                    *
 *                 (3) Collating sequence to use.  If the native type *
 *                 is desired, then just specify NULL.  This is an    *
 *                 unsigned char array of 256 bytes.  Each byte will  *
 *                 give the order to be used when sorting the byte.   *
 *                 For example, when indexed by the letter character  *
 *                 set to use. There are three convience functions    *
 *                 that are passed an array of 256 bytes to be set to *
 *                 the ASCII, EBCDIC, and ISO7bit.  See:              *
 *                                                                    *
 *                 unsigned char sequence[256];                       *
 *                                                                    *
 *                 blir_compare_sequence_ASCII(sequence);             *
 *                 blir_compare_sequence_EBCDIC(sequence);            *
 *                 blir_compare_sequence_ISO7bit(sequence);           *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   24/Dec/2000  Initial Release of Routines               *
 *                                                                    *
 *********************************************************************  */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "blir_compare.h"
#include "blir_overpunch.h"
#include "blir_temp_config.h"
#include "blir_globals.h"

static int32 blir_compare_rc (int32 compare);

/**********************************************************************
 *                                                                    *
 * Cobr_compare_sequence_ASCII - Set ASCII character set              *
 *                                                                    *
 *********************************************************************  */


uchar *
blir_compare_sequence_ASCII (uchar *Sequence) /* Squence set.  */
{
  int32 i;  /* Index.  */

  for (i = 0; i < 256; i ++)
    Sequence[i] = (uchar)i;

  if ('A' != 65)
    {
      Sequence[' '] = (uchar)32;
      Sequence['!'] = (uchar)33;
      Sequence['\"'] = (uchar)34;
      Sequence['#'] = (uchar)35;
      Sequence['$'] = (uchar)36;
      Sequence['%'] = (uchar)37;
      Sequence['&'] = (uchar)38;
      Sequence['\''] = (uchar)39;
      Sequence['('] = (uchar)40;
      Sequence[')'] = (uchar)41;
      Sequence['*'] = (uchar)42;
      Sequence['+'] = (uchar)43;
      Sequence[','] = (uchar)44;
      Sequence['-'] = (uchar)45;
      Sequence['.'] = (uchar)46;
      Sequence['/'] = (uchar)47;
      Sequence['0'] = (uchar)48;
      Sequence['1'] = (uchar)49;
      Sequence['2'] = (uchar)50;
      Sequence['3'] = (uchar)51;
      Sequence['4'] = (uchar)52;
      Sequence['5'] = (uchar)53;
      Sequence['6'] = (uchar)54;
      Sequence['7'] = (uchar)55;
      Sequence['8'] = (uchar)56;
      Sequence['9'] = (uchar)57;
      Sequence[':'] = (uchar)58;
      Sequence[';'] = (uchar)59;
      Sequence['<'] = (uchar)60;
      Sequence['='] = (uchar)61;
      Sequence['>'] = (uchar)62;
      Sequence['?'] = (uchar)63;
      Sequence['@'] = (uchar)64;
      Sequence['A'] = (uchar)65;
      Sequence['B'] = (uchar)66;
      Sequence['C'] = (uchar)67;
      Sequence['D'] = (uchar)68;
      Sequence['E'] = (uchar)69;
      Sequence['F'] = (uchar)70;
      Sequence['G'] = (uchar)71;
      Sequence['H'] = (uchar)72;
      Sequence['I'] = (uchar)73;
      Sequence['J'] = (uchar)74;
      Sequence['K'] = (uchar)75;
      Sequence['L'] = (uchar)76;
      Sequence['M'] = (uchar)77;
      Sequence['N'] = (uchar)78;
      Sequence['O'] = (uchar)79;
      Sequence['P'] = (uchar)80;
      Sequence['Q'] = (uchar)81;
      Sequence['R'] = (uchar)82;
      Sequence['S'] = (uchar)83;
      Sequence['T'] = (uchar)84;
      Sequence['U'] = (uchar)85;
      Sequence['V'] = (uchar)86;
      Sequence['W'] = (uchar)87;
      Sequence['X'] = (uchar)88;
      Sequence['Y'] = (uchar)89;
      Sequence['Z'] = (uchar)90;
      Sequence['['] = (uchar)91;
      Sequence['\\'] = (uchar)92;
      Sequence[']'] = (uchar)93;
      Sequence['^'] = (uchar)94;
      Sequence['_'] = (uchar)95;
      Sequence['`'] = (uchar)96;
      Sequence['a'] = (uchar)97;
      Sequence['b'] = (uchar)98;
      Sequence['c'] = (uchar)99;
      Sequence['d'] = (uchar)100;
      Sequence['e'] = (uchar)101;
      Sequence['f'] = (uchar)102;
      Sequence['g'] = (uchar)103;
      Sequence['h'] = (uchar)104;
      Sequence['i'] = (uchar)105;
      Sequence['j'] = (uchar)106;
      Sequence['k'] = (uchar)107;
      Sequence['l'] = (uchar)108;
      Sequence['m'] = (uchar)109;
      Sequence['n'] = (uchar)110;
      Sequence['o'] = (uchar)111;
      Sequence['p'] = (uchar)112;
      Sequence['q'] = (uchar)113;
      Sequence['r'] = (uchar)114;
      Sequence['s'] = (uchar)115;
      Sequence['t'] = (uchar)116;
      Sequence['u'] = (uchar)117;
      Sequence['v'] = (uchar)118;
      Sequence['w'] = (uchar)119;
      Sequence['x'] = (uchar)120;
      Sequence['y'] = (uchar)121;
      Sequence['z'] = (uchar)122;
      Sequence['{'] = (uchar)123;
      Sequence['|'] = (uchar)124;
      Sequence['}'] = (uchar)125;
      Sequence['~'] = (uchar)126;
    }

  return Sequence;
}


/**********************************************************************
 *                                                                    *
 * Cobr_compare_sequence_EBCDIC - Set EBCDIC character set            *
 *                                                                    *
 *********************************************************************  */

uchar *
blir_compare_sequence_EBCDIC (uchar *Sequence) /* Sequence set.  */
{
  int32 i;  /* Index.  */

  for (i = 0; i < 256; i ++)
    Sequence[i] = (uchar)i;

  if ('A' != 193)
    {
      Sequence[' '] = (uchar)64;
      Sequence['.'] = (uchar)75;
      Sequence['<'] = (uchar)76;
      Sequence['('] = (uchar)77;
      Sequence['+'] = (uchar)78;
      Sequence['|'] = (uchar)79;
      Sequence['&'] = (uchar)80;
      Sequence['!'] = (uchar)90;
      Sequence['$'] = (uchar)91;
      Sequence['*'] = (uchar)92;
      Sequence[')'] = (uchar)93;
      Sequence[';'] = (uchar)94;
      Sequence['^'] = (uchar)95;
      Sequence['-'] = (uchar)96;
      Sequence['/'] = (uchar)97;
      Sequence[','] = (uchar)107;
      Sequence['%'] = (uchar)108;
      Sequence['_'] = (uchar)109;
      Sequence['>'] = (uchar)110;
      Sequence['?'] = (uchar)111;
      Sequence[':'] = (uchar)122;
      Sequence['#'] = (uchar)123;
      Sequence['@'] = (uchar)124;
      Sequence['\''] = (uchar)125;
      Sequence['='] = (uchar)126;
      Sequence['\"'] = (uchar)127;
      Sequence['a'] = (uchar)129;
      Sequence['b'] = (uchar)130;
      Sequence['c'] = (uchar)131;
      Sequence['d'] = (uchar)132;
      Sequence['e'] = (uchar)133;
      Sequence['f'] = (uchar)134;
      Sequence['g'] = (uchar)135;
      Sequence['h'] = (uchar)136;
      Sequence['i'] = (uchar)137;
      Sequence['j'] = (uchar)145;
      Sequence['k'] = (uchar)146;
      Sequence['l'] = (uchar)147;
      Sequence['m'] = (uchar)148;
      Sequence['n'] = (uchar)149;
      Sequence['o'] = (uchar)150;
      Sequence['p'] = (uchar)151;
      Sequence['q'] = (uchar)152;
      Sequence['r'] = (uchar)153;
      Sequence['s'] = (uchar)162;
      Sequence['t'] = (uchar)163;
      Sequence['u'] = (uchar)164;
      Sequence['v'] = (uchar)165;
      Sequence['w'] = (uchar)166;
      Sequence['x'] = (uchar)167;
      Sequence['y'] = (uchar)168;
      Sequence['z'] = (uchar)169;
      Sequence['{'] = (uchar)192;
      Sequence['A'] = (uchar)193;
      Sequence['B'] = (uchar)194;
      Sequence['C'] = (uchar)195;
      Sequence['D'] = (uchar)196;
      Sequence['E'] = (uchar)197;
      Sequence['F'] = (uchar)198;
      Sequence['G'] = (uchar)199;
      Sequence['H'] = (uchar)200;
      Sequence['I'] = (uchar)201;
      Sequence['}'] = (uchar)208;
      Sequence['J'] = (uchar)209;
      Sequence['K'] = (uchar)210;
      Sequence['L'] = (uchar)211;
      Sequence['M'] = (uchar)212;
      Sequence['N'] = (uchar)213;
      Sequence['O'] = (uchar)214;
      Sequence['P'] = (uchar)215;
      Sequence['Q'] = (uchar)216;
      Sequence['R'] = (uchar)217;
      Sequence['\\'] = (uchar)224;
      Sequence['S'] = (uchar)226;
      Sequence['T'] = (uchar)227;
      Sequence['U'] = (uchar)228;
      Sequence['V'] = (uchar)229;
      Sequence['W'] = (uchar)230;
      Sequence['X'] = (uchar)231;
      Sequence['Y'] = (uchar)232;
      Sequence['Z'] = (uchar)233;
      Sequence['0'] = (uchar)240;
      Sequence['1'] = (uchar)241;
      Sequence['2'] = (uchar)242;
      Sequence['3'] = (uchar)243;
      Sequence['4'] = (uchar)244;
      Sequence['5'] = (uchar)245;
      Sequence['6'] = (uchar)246;
      Sequence['7'] = (uchar)247;
      Sequence['8'] = (uchar)248;
      Sequence['9'] = (uchar)249;
    }

  return Sequence;
}


/**********************************************************************
 *                                                                    *
 * Cobr_compare_sequence_ISO7bit - Set Standard2 character set (ISO   *
 * 7-bit)                                                             *
 * NOTE:  I don't know the ISO 7-bit standard, for now it is set up   *
 * as Native.                                                         *
 *                                                                    *
 *********************************************************************  */


uchar *
blir_compare_sequence_ISO7bit (uchar *Sequence)
{
  int32 i;  /* Index.  */

  for (i = 0; i < 256; i ++)
    Sequence[i] = (uchar)i;

  return Sequence;
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareFloat - local compare of floating point numbers        *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareFloat (const void *item1,  /* Item1 to compare.  */
                   const void *item2,  /* Item2 to compare.  */
                   BLI_SIZE_T length) /* Item length.  */
{
  switch (length)
    {
    case BLI_FLOAT_32_WIDTH :
      if ( * ((bli_float_32 *)item1) <  * ((bli_float_32 *)item2))
        return -1;
      else if ( * ((bli_float_32 *)item1) >  * ((bli_float_32 *)item2))
        return 1;
      else
        return 0;
    case BLI_FLOAT_64_WIDTH :
      if ( * ((bli_float_64 *)item1) <  * ((bli_float_64 *)item2))
        return -1;
      else if ( * ((bli_float_64 *)item1) >  * ((bli_float_64 *)item2))
        return 1;
      else
        return 0;
#if 0
    case FLOAT_LONG_DOUBLE_WIDTH :
      if ( * ((FLOAT_LONG_DOUBLE * )item1) <  * ((FLOAT_LONG_DOUBLE * )item2))
        return -1;
      else if ( * ((FLOAT_LONG_DOUBLE * )item1) >
               * ((FLOAT_LONG_DOUBLE * )item2))
        return 1;
      else
        return 0;
      break;
#endif
    default :
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareUnsigned - local compare of unsigned binary numbers    *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareUnsigned (const void *item1,  /* Item1 to compare.  */
                      const void *item2,  /* Item2 to compare.  */
                      BLI_SIZE_T length) /* Item length.  */
{
  switch (length)
    {
    case BLI_UINT8_WIDTH :
      if ( * ((uchar *)item1) <  * ((uchar *)item2))
        return -1;
      else if ( * ((uchar *)item1) >  * ((uchar *)item2))
        return 1;
      else
        return 0;
    case BLI_UINT16_WIDTH :
      if ( * ((uint16 *)item1) <  * ((uint16 *)item2))
        return -1;
      else if ( * ((uint16 *)item1) >  * ((uint16 *)item2))
        return 1;
      else
        return 0;
    case BLI_UINT32_WIDTH :
      if ( * ((uint32 *)item1) <  * ((uint32 *)item2))
        return -1;
      else if ( * ((uint32 *)item1) >  * ((uint32 *)item2))
        return 1;
      else
        return 0;
    case BLI_UINT64_WIDTH :
      if ( * ((uint64 *)item1) <  * ((uint64 *)item2))
        return -1;
      else if ( * ((uint64 *)item1) >  * ((uint64 *)item2))
        return 1;
      else
        return 0;
    default :
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareBinary - local compare of signed binary numbers        *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareBinary (const void *item1,  /* Item1 to compare.  */
                    const void *item2,  /* Item2 to compare.  */
                    BLI_SIZE_T length) /* Item length.  */
{
  switch (length)
    {
    case BLI_INT8_WIDTH :
      if ( * ((schar *)item1) <  * ((schar *)item2))
        return -1;
      else if ( * ((schar *)item1) >  * ((schar *)item2))
        return 1;
      else
        return 0;
    case BLI_INT16_WIDTH :
      if ( * ((int16 *)item1) <  * ((int16 *)item2))
        return -1;
      else if ( * ((int16 *)item1) >  * ((int16 *)item2))
        return 1;
      else
        return 0;
    case BLI_INT32_WIDTH :
      if ( * ((int32 *)item1) <  * ((int32 *)item2))
        return -1;
      else if ( * ((int32 *)item1) >  * ((int32 *)item2))
        return 1;
      else
        return 0;
    case BLI_INT64_WIDTH :
      if ( * ((int64 *)item1) <  * ((int64 *)item2))
        return -1;
      else if ( * ((int64 *)item1) >  * ((int64 *)item2))
        return 1;
      else
        return 0;
    default :
      return 0;
    }
}



/**********************************************************************
 *                                                                    *
 * Cobr_comparePacked - local compare of signed decimal numbers       *
 *                                                                    *
 *********************************************************************  */


static int32
blir_comparePacked (const void *item1,  /* Item1 to compare.  */
                    const void *item2,  /* Item2 to compare.  */
                    BLI_SIZE_T length) /* Item length.  */
{
  int32 sign1;  /* Item1 sign.  */
  int32 sign2;  /* Item2 sign.  */
  int32 nibble1;  /* Item1 nibble.  */
  int32 nibble2;  /* Item2 nibble.  */
  uchar *cptr1;  /* Item1 pointer.  */
  uchar *cptr2;  /* Item2 pointer.  */
  int32 cmp;  /* Compare results.  */
  BLI_SIZE_T i;  /* Integer.  */

  cptr1 = (uchar *)item1;
  cptr2 = (uchar *)item2;

  sign1 =  * (cptr1 + length - 1) & 0x0F;
  sign2 =  * (cptr2 + length - 1) & 0x0F;

  if (sign1 == 0x0D)
    sign1 = -1;
  else
    sign1 = +1;

  if (sign2 == 0x0D)
    sign2 = -1;
  else
    sign2 = +1;


  /*--------------------------------------------------------------------*
   * If the signs are different, check for rare case of -0              *
   *--------------------------------------------------------------------.  */


  if (sign1 != sign2)
    {
      cmp = sign1 - sign2;  /* Get assumed difference.  */
      for (i = 1; i < length; i ++)
        {
          if ( * (cptr1 ++) != 0)
            return blir_compare_rc (cmp);
          if ( * (cptr2 ++) != 0)
            return blir_compare_rc (cmp);
        }

      if (( * cptr1 & 0xF0) != 0)
        return blir_compare_rc (cmp);
      if (( * cptr2 & 0xF0) != 0)
        return blir_compare_rc (cmp);

      return 0;
    }


  /*--------------------------------------------------------------------*
   * Signs are the same, compare digits                                 *
   *--------------------------------------------------------------------.  */


  for (i = 1; i < length; i ++)
    {
      nibble1 =  * cptr1 & 0xF0;
      nibble2 =  * cptr2 & 0xF0;
      cmp = nibble1 - nibble2;
      if (cmp != 0)
        {
          if (sign1 < 0)
            cmp =  - cmp;  /* If negative, larger number is less.  */
          return blir_compare_rc (cmp);
        }
      nibble1 =  * (cptr1 ++) & 0x0F;
      nibble2 =  * (cptr2 ++) & 0x0F;
      cmp = nibble1 - nibble2;
      if (cmp != 0)
        {
          if (sign1 < 0)
            cmp =  - cmp;  /* If negative, larger number is less.  */
          return blir_compare_rc (cmp);
        }
    }

  nibble1 =  * cptr1 & 0xF0;
  nibble2 =  * cptr2 & 0xF0;
  cmp = nibble1 - nibble2;

  if (sign1 < 0)
    cmp =  - cmp;  /* If negative, larger number is less.  */
  return blir_compare_rc (cmp);
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareDisplayLeadSep -                                       *
 *                                                                    *
 * local compare of display with separate leading sign                *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareDisplayLeadSep (const void *item1,  /* Item1 to compare.  */
                            const void *item2,  /* Item2 to compare.  */
                            BLI_SIZE_T length) /* Item length.  */
{
  int32 sign1;  /* Item1 sign.  */
  int32 sign2;  /* Item2 sign.  */
  int32 byte1;  /* Item1 byte.  */
  int32 byte2;  /* Item2 byte.  */
  uchar *cptr1;  /* Item1 pointer.  */
  uchar *cptr2;  /* Item2 pointer.  */
  int32 cmp;  /* Compare results.  */
  BLI_SIZE_T i;  /* Integer.  */

  cptr1 = (uchar *)item1;
  cptr2 = (uchar *)item2;

  if ( * (cptr1 ++) == '-')
    sign1 = -1;
  else
    sign1 = +1;

  if ( * (cptr2 ++) == '-')
    sign2 = -1;
  else
    sign2 = +1;


  /*--------------------------------------------------------------------*
   * If the signs are different, check for rare case of -0              *
   *--------------------------------------------------------------------.  */


  if (sign1 != sign2)
    {
      cmp = sign1 - sign2;  /* Get assumed difference.  */
      for (i = 1; i < length; i ++)
        {
          if ( * (cptr1 ++) != '0')
            return blir_compare_rc (cmp);
          if ( * (cptr2 ++) != '0')
            return blir_compare_rc (cmp);
        }
      return 0;
    }


  /*--------------------------------------------------------------------*
   * Signs are the same                                                 *
   *--------------------------------------------------------------------.  */


  for (i = 1; i < length; i ++)
    {
      byte1 =  * (cptr1 ++);
      byte2 =  * (cptr2 ++);
      cmp = byte1 - byte2;
      if (cmp != 0)
        {
          if (sign1 < 0)
            cmp =  - cmp;  /* If negative, larger number is less.  */
          return blir_compare_rc (cmp);
        }
    }

  return blir_compare_rc (0);
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareDisplayTrailSep -                                      *
 *                                                                    *
 * local compare of display with separate trailing sign               *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareDisplayTrailSep (const void *item1,  /* Item1 to compare.  */
                             const void *item2,  /* Item2 to compare.  */
                             BLI_SIZE_T length) /* Item length.  */
{
  int32 sign1;  /* Item1 sign.  */
  int32 sign2;  /* Item2 sign.  */
  int32 byte1;  /* Item1 byte.  */
  int32 byte2;  /* Item2 byte.  */
  uchar *cptr1;  /* Item1 pointer.  */
  uchar *cptr2;  /* Item2 pointer.  */
  int32 cmp;  /* Compare results.  */
  BLI_SIZE_T i;  /* Integer.  */

  cptr1 = (uchar *)item1;
  cptr2 = (uchar *)item2;

  if ( * (cptr1 + length - 1) == '-')
    sign1 = -1;
  else
    sign1 = +1;

  if ( * (cptr2 + length - 1) == '-')
    sign2 = -1;
  else
    sign2 = +1;


  /*--------------------------------------------------------------------*
   * If the signs are different, check for rare case of -0              *
   *--------------------------------------------------------------------.  */


  if (sign1 != sign2)
    {
      cmp = sign1 - sign2;  /* Get assumed difference.  */
      for (i = 1; i < length; i ++)
        {
          if ( * (cptr1 ++) != '0')
            return blir_compare_rc (cmp);
          if ( * (cptr2 ++) != '0')
            return blir_compare_rc (cmp);
        }
      return 0;
    }


  /*--------------------------------------------------------------------*
   * Signs are the same                                                 *
   *--------------------------------------------------------------------.  */


  for (i = 1; i < length; i ++)
    {
      byte1 =  * (cptr1 ++);
      byte2 =  * (cptr2 ++);
      cmp = byte1 - byte2;
      if (cmp != 0)
        {
          if (sign1 < 0)
            cmp =  - cmp;  /* If negative, larger number is less.  */
          return blir_compare_rc (cmp);
        }
    }

  return blir_compare_rc (0);
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareDisplayLeadOver -                                      *
 *                                                                    *
 * local compare of display with overpunched leading sign             *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareDisplayLeadOver (const void *item1,  /* Item1 to compare.  */
                             const void *item2,  /* Item2 to compare.  */
                             BLI_SIZE_T length) /* Item length.  */
{
  int32 sign1;  /* Item1 sign.  */
  int32 sign2;  /* Item2 sign.  */
  int32 byte1;  /* Item1 byte.  */
  int32 byte2;  /* Item2 byte.  */
  uchar *cptr1;  /* Item1 pointer.  */
  uchar *cptr2;  /* Item2 pointer.  */
  int32 cmp;  /* Compare results.  */
  BLI_SIZE_T i;  /* Integer.  */

  cptr1 = (uchar *)item1;
  cptr2 = (uchar *)item2;

  blir_overpunch_get ( & sign1,  & byte1,  * (cptr1 ++));
  blir_overpunch_get ( & sign2,  & byte2,  * (cptr2 ++));


  /*--------------------------------------------------------------------*
   * If the signs are different, check for rare case of -0              *
   *--------------------------------------------------------------------.  */


  if (sign1 != sign2)
    {
      cmp = sign1 - sign2;  /* Get assumed difference.  */
      if (byte1 != '0')
        return blir_compare_rc (cmp);
      if (byte2 != '0')
        return blir_compare_rc (cmp);
      for (i = 1; i < length; i ++)
        {
          if ( * (cptr1 ++) != '0')
            return blir_compare_rc (cmp);
          if ( * (cptr2 ++) != '0')
            return blir_compare_rc (cmp);
        }
      return 0;
    }


  /*--------------------------------------------------------------------*
   * Signs are the same                                                 *
   *--------------------------------------------------------------------.  */


  cmp = byte1 - byte2;
  if (cmp != 0)
    {
      if (sign1 < 0)
        cmp =  - cmp;  /* If negative, larger number is less.  */
      return blir_compare_rc (cmp);
    }

  for (i = 1; i < length; i ++)
    {
      byte1 =  * (cptr1 ++);
      byte2 =  * (cptr2 ++);
      cmp = byte1 - byte2;
      if (cmp != 0)
        {
          if (sign1 < 0)
            cmp =  - cmp;  /* If negative, larger number is less.  */
          return blir_compare_rc (cmp);
        }
    }

  return blir_compare_rc (0);
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareDisplayTrailOver -                                     *
 *                                                                    *
 * local compare of display with overpunched trailing sign            *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareDisplayTrailOver (const void *item1,  /* Item1 to compare.  */
                              const void *item2,  /* Item2 to compare.  */
                              BLI_SIZE_T length) /* Item length.  */
{
  int32 sign1;  /* Item1 sign.  */
  int32 sign2;  /* Item2 sign.  */
  int32 byte1;  /* Item1 byte.  */
  int32 byte2;  /* Item2 byte.  */
  int32 last1;  /* Item1 last byte.  */
  int32 last2;  /* Item2 last byte.  */
  uchar *cptr1;  /* Item1 pointer.  */
  uchar *cptr2;  /* Item2 pointer.  */
  int32 cmp;  /* Compare results.  */
  BLI_SIZE_T i;  /* Integer.  */

  cptr1 = (uchar *)item1;
  cptr2 = (uchar *)item2;

  blir_overpunch_get ( & sign1,  & last1,  * (cptr1 + length - 1));
  blir_overpunch_get ( & sign2,  & last2,  * (cptr2 + length - 1));


  /*--------------------------------------------------------------------*
   * If the signs are different, check for rare case of -0              *
   *--------------------------------------------------------------------.  */


  if (sign1 != sign2)
    {
      cmp = sign1 - sign2;  /* Get assumed difference.  */
      for (i = 1; i < length; i ++)
        {
          if ( * (cptr1 ++) != '0')
            return blir_compare_rc (cmp);
          if ( * (cptr2 ++) != '0')
            return blir_compare_rc (cmp);
        }
      if (last1 != '0')
        return blir_compare_rc (cmp);
      if (last2 != '0')
        return blir_compare_rc (cmp);
      return 0;
    }


  /*--------------------------------------------------------------------*
   * Signs are the same                                                 *
   *--------------------------------------------------------------------.  */


  for (i = 1; i < length; i ++)
    {
      byte1 =  * (cptr1 ++);
      byte2 =  * (cptr2 ++);
      cmp = byte1 - byte2;
      if (cmp != 0)
        {
          if (sign1 < 0)
            cmp =  - cmp;  /* If negative, larger number is less.  */
          return blir_compare_rc (cmp);
        }
    }
  cmp = last1 - last2;
  if (cmp != 0)
    {
      if (sign1 < 0)
        cmp =  - cmp;  /* If negative, larger number is less.  */
      return blir_compare_rc (cmp);
    }

  return blir_compare_rc (0);
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareDisplay - local compare of display numbers             *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareDisplay (const void *item1,  /* Item1 to compare.  */
                     const void *item2,  /* Item2 to compare.  */
                     BLI_SIZE_T length,  /* Item length.  */
                     uint32 flags) /* Item flags also in blir_temp_config.h.  */
{

  if ((flags & blir_flagOverpunch) != 0)
    {
      if ((flags & blir_flagSignLead) != 0)
        return blir_compareDisplayLeadOver (item1, item2, length);
      else if ((flags & blir_flagSignTrail) != 0)
        return blir_compareDisplayTrailOver (item1, item2, length);
      else
        return blir_compare_rc (memcmp (item1, item2, length));
    }
  else
    {
      if ((flags & blir_flagSignLead) != 0)
        return blir_compareDisplayLeadSep (item1, item2, length);
      else if ((flags & blir_flagSignTrail) != 0)
        return blir_compareDisplayTrailSep (item1, item2, length);
      else
        return blir_compare_rc (memcmp (item1, item2, length));
    }
}


/**********************************************************************
 *                                                                    *
 * Cobr_compareSequence - compare text using collating sequence       *
 *                                                                    *
 *********************************************************************  */


static int32
blir_compareSequence (const void *item1,  /* Item1 to compare.  */
                      const void *item2,  /* Item2 to compare.  */
                      BLI_SIZE_T length,  /* Item length.  */
                      uchar *sequence) /* Collating sequence.  */
{
  int32 rc;  /* Return code.  */
  uchar *cptr1 = (uchar *)item1;  /* Character pointer 1.  */
  uchar *cptr2 = (uchar *)item2;  /* Character pointer 2.  */
  BLI_SIZE_T i;  /* Index.  */

  rc = 0;

  for (i = 0; (i < length) && (rc == 0); i ++)
    rc = sequence[cptr1[i]] - sequence[cptr2[i]];

  return blir_compare_rc (rc);
}


/**********************************************************************
 *                                                                    *
 * Cobr_compare - BLISS compare function                              *
 *                                                                    *
 * compare function for sort compares ITEM1 to ITEM2, both of the     *
 * same type, returning 0 if the same, -1 if ITEM1 is less and +1 if  *
 * ITEM1 is greater                                                   *
 *                                                                    *
 *********************************************************************  */


int32
blir_compare (const void *item1,  /* Item1 to compare.  */
                    const void *item2,  /* Item2 to compare.  */
                    BLI_SIZE_T length,  /* Item length.  */
                    uint32 type,  /* Item type blir_type enum.  */
                    uint32 flags,  /* Item flags.  */
                    uchar *sequence) /* Collating sequence.  */
{
  switch (type)
    {
    case blir_typeNull :
      return 0;
    case blir_typeText :  /* Text Type.  */
      if (sequence == (uchar *)NULL)
        return blir_compare_rc (memcmp (item1, item2, length));
      else
        return blir_compareSequence (item1, item2, length, sequence);
    case blir_typeDisplay :  /* Numeric Usage Display.  */
      return blir_compareDisplay (item1, item2, length, flags);
    case blir_typePacked :  /* Packed Decimal.  */
      return blir_comparePacked (item1, item2, length);
    case blir_typeBinary :  /* Signed binary.  */
      return blir_compareBinary (item1, item2, length);
    case blir_typeUnsigned :  /* Unsigned binary.  */
      return blir_compareUnsigned (item1, item2, length);
    case blir_typeFloat :  /* Floating point.  */
      return blir_compareFloat (item1, item2, length);
    default :
      return 0;
    };
}

/**********************************************************************
 *                                                                    *
 * Cobr_compare_rc - generate return codes: -1, 0, +1                 *
 *                                                                    *
 * convert return code as follows:                                    *
 *                                                                    *
 * n < 0:  -1                                                         *
 * n = 0:   0                                                         *
 * n > 0:  +1                                                         *
 *                                                                    *
 *********************************************************************  */
/* This is at the end because inlining of it causes bugs.  TJ.  */

static int32 /* Output return code.  */
blir_compare_rc (int32 compare) /* Input return code.  */
{
  if (compare < 0)
    return -1;
  else if (compare > 0)
    return +1;
  else
    return 0;
}


