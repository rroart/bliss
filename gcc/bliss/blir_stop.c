
/*                                                                    *
 *                        BLISS Stop Function                         *
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
 * Title:  BLISS Stop Function and Error drop off end                 *
 *                                                                    *
 * Description:    This file is part of the BLISS Stop Function.      *
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

#include "blir_stop.h"
#include "blir_display.h"
#include "blir_temp_config.h"
#include "blir_globals.h"

/**********************************************************************
 *                                                                    *
 * Cobr_stop_run - Exit with the appropriate return code back to the  *
 * OS                                                                 *
 *                                                                    *
 * Later on, the routine will call various cleanup routines, but not  *
 * yet.                                                               *
 *                                                                    *
 *********************************************************************  */

void
blir_stop_run (int return_code) /* Return code.  */
{
  exit (return_code);
}


/**********************************************************************
 *                                                                    *
 * Cobr_stop_literal - Output literal to standard output, wait for    *
 * input character, and return to caller.                             *
 *                                                                    *
 *********************************************************************  */


void
blir_stop_literal (const char *literal,  /* Text to write.  */
                   size_t length) /* Length of text.  */
{
  blir_display (literal, length, 0, blir_typeText, 0, NULL);
  fgetc (stdin);
}


/**********************************************************************
 *                                                                    *
 * Cobr_error_drop_off_end - crash when the programmer drops off the  *
 * end of a program                                                   *
 *                                                                    *
 * Later on, the routine will call various cleanup routines, but not  *
 * yet.                                                               *
 *                                                                    *
 *********************************************************************  */

void
blir_error_drop_off_end ()
{
  const char *msg = "\nError - dropped off end of program\n";
  blir_display (msg, strlen (msg), 0, blir_typeText, 0, NULL);
  fflush (stdout);
  abort ();
}

