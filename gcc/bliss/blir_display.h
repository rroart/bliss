
/**********************************************************************
 *                                                                    *
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
 * Title:          BLISS Display Function                             *
 *                                                                    *
 * Description:    This file is part of the BLISS Display Function.   *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   21/Aug/2000  Initial Release of Routines               *
 *                                                                    *
 *********************************************************************  */

#ifndef __BLIR_DISPLAY__
#define __BLIR_DISPLAY__

#include <stdio.h>

#include "blir_temp_config.h"

void blir_display (const void *item,  /* Item to print.  */
                   size_t length,  /* Item length.  */
                   size_t fraction,  /* Digits after decimal, 0 = None.  */
                   int type,  /* Item type blir_type enum.  */
                   int flags,  /* Item flags see blir_temp_config.h.  */
                   const void *picture);  /* Optional Picture, NULL = N/A.  */

void blir_display_nl (void);
#endif
