
/**********************************************************************
 *                                                                    *
 *        BLISS Compare Function Header File - used in sorting        *
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
 * Title:          BLISS Compare Function Header File                 *
 *                                                                    *
 * Description:    This file is part of the BLISS Compare Function.   *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 *                 INT32 blir_compare(item1,item2,length,type,flags)  *
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
 *                 BLI_UINT8 sequence[256];                       *
 *                                                                    *
 *                 blir_compare_sequence_ASCII(sequence);             *
 *                 blir_compare_sequence_EBCDIC(sequence);            *
 *                 blir_compare_sequence_ISO7bit(sequence);           *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   24/Dec/2000  Initial Release of Routines T.            *
 *                          Seward   01/Jan/2001  Repalced with       *
 *                          correct source                            *
 *                                                                    *
 *********************************************************************  */

#ifndef __BLIR_COMPARE__
#define __BLIR_COMPARE__

#include "blir_temp_config.h"


/**********************************************************************
 *                                                                    *
 * Sequence set routines                                              *
 *                                                                    *
 *********************************************************************  */


uchar *blir_compare_sequence_ASCII (uchar *Sequence);  /* Squence set.  */

uchar *blir_compare_sequence_EBCDIC (uchar *Sequence);  /* Sequence set.  */

uchar *blir_compare_sequence_ISO7bit (uchar *Sequence);  /* Sequence set.  */



/**********************************************************************
 *                                                                    *
 * Cobr_compare - BLISS compare function                              *
 *                                                                    *
 * compare function for sort compares ITEM1 to ITEM2, both of the     *
 * same type, returning 0 if the same, -1 if ITEM1 is less and +1 if  *
 * ITEM1 is greater.                                                  *
 *                                                                    *
 *********************************************************************  */


int32 blir_compare (const void *item1,  /* Item1 to copmpare.  */
                                      const void *item2,  /* Item2 to copmpare.  */
                                      BLI_SIZE_T length,  /* Item length.  */
                                      uint32 type,  /* Item type blir_type enum.  */
                                      uint32 flags,  /* Item flags also in blir_config.h.  */
                                      uchar *sequence);  /* Collating sequence.  */

#endif

