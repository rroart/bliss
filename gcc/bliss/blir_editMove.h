
/*                                                                    *
 *                     BLISS Numeric Edited Moves                     *
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
 *                                                                    *
 *********************************************************************  */


#ifndef __BLIR_EDITMOVE_H__
#define __BLIR_EDITMOVE_H__

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */


uchar * /* Output Picture.  */
blir_emEditMove_Analyze (uchar *Picture,  /* Output Picture.  */
                         int32 *destDigitTotal,  /* Digits in destination.  */
                         int32 *destDigitLeft,  /* Destination digits left of decimal.  */
                         int32 *sourceDigitRight,  /* Source digits right of decimal.  */
                         const uchar *destPic,  /* Destination Picture.  */
                         const uchar *sourcePic);  /* Source Picture.  */

int32
blir_emEditMove (void *dest,  /* Destination.  */
                 size_t destLen,  /* Destination Length.  */
                 int32 destType,  /* Destination Type.  */
                 int32 destFlags,  /* Destination flags.  */
                 const uchar *picture,  /* Destination Encoded Picture (from blir_emEditMove_Analyze).  */
                 int32 destDigitTotal,  /* Total digits in number (from blir_emEditMove_Analyze).  */
                 int32 destDigitLeft,  /* Digits to left of decimal in destination (from blir_emEditMove_Analyze).  */
                 int32 sourceDigitRight,  /* Digits to right of decimal in source (from blir_emEditMove_Analyze).  */
                 const void *source,  /* Source.  */
                 size_t sourceLen,  /* Source Length.  */
                 int32 sourceType,  /* Source Type.  */
                 int32 sourceFlags);  /* Source Flags.  */

uchar * /* DestPicture.  */
blir_emUnedit_Analyze (uchar *editPicture,  /* Edit Picture (for blir_em_editMove).  */
                       uchar *uneditPicture,  /* Unedit picture (for blir_emUnedit).  */
                       int32 *uneditDigits,  /* Digits in unedited picture (for blir_emUnedit).  */
                       const uchar *sourcePic);  /* Source Picture.  */

uchar * /* Destination.  */
blir_emUnedit (void *dest,  /* Destination.  */
               size_t destLen,  /* Destination Length.  */
               const uchar *uneditPic,  /* Unedit Encoded Picture (from blir_emUnedit_Analyze).  */
               int32 uneditDigits, /* Unedit Digits (from blir_emUnedit_Analyze).  */
               const void *source,  /* Source.  */
               int32 sourceType,  /* Source Type.  */
               int32 sourceFlags);  /* Source Flags.  */

int32
blir_emUneditMove_Analyze (uchar *Picture,  /* Encoded Output Picture.  */
                           int32 *destDigitTotal,  /* Digits in destination.  */
                           int32 *destDigitLeft,  /* Destination digits left of decimal.  */
                           int32 *sourceDigitRight,  /* Source digits right of decimal.  */
                           uchar *uneditPicture,  /* Unedit Picture.  */
                           int32 *uneditDigits,  /* Digits in unedited picture.  */
                           const uchar *destPic,  /* Original Destination Picture.  */
                           const uchar *sourcePic);  /* Original Source Picture.  */

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
                   int32 sourceFlags);  /* Source Flags.  */

int
blir_emSetCurrencySymbol (uchar symbol);

int
blir_emSetDecimalIsComma (int32 flag);

#endif
