
/**********************************************************************
 *                                                                    *
 *              Global Routines for BLISS Runtime system              *
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
 * License aBLI_INT32u with this library; if not, write to the Free   *
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
 * Title:          Global routines for BLISS runtime system           *
 *                                                                    *
 * Description:    This file contains the global routines for the     *
 *                 BLISS runtime library                              *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log: T.  Seward       09/Jul/2000Initial Release of    *
 *                 Routines                                           *
 *                                                                    *
 *********************************************************************  */

#include <stdio.h>

#include "blir_globals.h"

/**********************************************************************
 *                                                                    *
 * Cobr_error_termination - This routine will be called when BLISS is *
 *                          to terminate.  It will be passed the      *
 *                          error code and message causing the        *
 *                          termination.                              *
 *                                                                    *
 *********************************************************************  */

void
blir_error_termination (int error_code,  /* Error Code.  */
                        const char *error_string) /* Error Message.  */
{
  printf ("\tblir_error_termination called:  BLI%04dE %s\n", error_code, error_string);

  return;
}

