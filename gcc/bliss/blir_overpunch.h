

/**********************************************************************
 *                                                                    *
 *              Convert Overpunch characters Header File              *
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
 * Title: Convert overpunch characters header file                    *
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


#ifndef __BLIR_OVERPUNCH_H__
#define __BLIR_OVERPUNCH_H__

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */


uchar /* Overpunched character.  */
blir_overpunch_set (int32 inSign,  /* Input Sign.  */
                    int32 inChar);  /* Input Character.  */

int32
blir_overpunch_get (int32 *outSign,  /* Output Sign.  */
                    int32 *outChar,  /* Output Character.  */
                    int32 inChar);  /* Input Character.  */


#endif
