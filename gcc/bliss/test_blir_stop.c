
/*                                                                    *
 *                        BLISS Stop Function                         *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.            *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of the *
 * License, or (at your option) any later version.                    *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
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
 * Title:  BLISS Stop Function                                        *
 *                                                                    *
 * Description:    This file is part of the BLISS Stop Function.      *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward       21/Aug/2000     Initial Release of Routines T.     *
 *                          Josling      25/Nov/2000     Split out    *
 *                          stop from display                         *
 *                                                                    *
 *********************************************************************  */


#include <string.h>
#include "blir_display.h"
#include "blir_stop.h"


/**********************************************************************
 *                                                                    *
 * Main - Program mainline                                            *
 *                                                                    *
 *********************************************************************  */

int
main (void)
{

  /**********************************************************************
   *                                                                    *
   * Test stop literal text                                             *
   *                                                                    *
   *********************************************************************  */

  printf ("\n== Testing Stop Literal ==\n\n");
  blir_stop_literal ("Press Enter To Continue:", 25);

  /**********************************************************************
   *                                                                    *
   * Test exit code text                                                *
   *                                                                    *
   *********************************************************************  */

  printf ("\n\n== Testing Stop Run (rc=1000) ==\n\n");
  blir_stop_run (1000);

  return 0;
}
