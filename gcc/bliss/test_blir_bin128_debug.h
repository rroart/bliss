
/**********************************************************************
 *                                                                    *
 *         128 Bit binary math routines debugging header file         *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.            *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License        *
 * as published by the Free Software Foundation; either version 2     *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public          *
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
 * Title:          128 bit binary math routines debugging header file *
 *                                                                    *
 * Description:    This file consists of a serious of routines to     *
 *                 carry out math using 128 bit binary values.  The   *
 *                 hardware only requires a 16 bit binary minimum     *
 *                 word widith for these routines to work.            *
 *                                                                    *
 * System:         BLISS For GCC Runtime Library                      *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   09/Jul/2000  Initial Release of Routines               *
 * T. Josling  01/08//2000  debug -> blir_bin128_debug                *
 *                                                                    *
 *********************************************************************  */


#ifndef __DEBUG_H__
#define __DEBUG_H__

/**********************************************************************
 *                                                                    *
 *                  Debug global types and variables                  *
 *                                                                    *
 *********************************************************************  */

int
setCobr_Bin128_Debug (int flag);

int
getCobr_Bin128_Debug (void);

int
blir_bin128_debugPutNum (const char *t,
                         const void *n);

int
blir_bin128_debugPause (char *prompt);

#endif
