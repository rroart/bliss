
/**********************************************************************
 *                                                                    *
 *               128 Bit binary math debugging routines               *
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
 * Title:          128 bit binary math debugging routines             *
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
 * T. Josling  01/Aug/2000  debug->blir_bin128_debug                  *
 *                                                                    *
 *********************************************************************  */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "blir_bin128.h"
#include "blir_bin128LLL.h"
#include "blir_bin128ULLL.h"
#include "test_blir_bin128_debug.h"

/**********************************************************************
 *                                                                    *
 *                  Debug global types and variables                  *
 *                                                                    *
 *********************************************************************  */


typedef enum
{
  False = 0, True = 1
}
Boolean;

static Boolean blir_bin128_debug = False;

/**********************************************************************
 *                                                                    *
 * SetCobr_Bin128_Debug - Set the blir_bin128_debug flag              *
 *                                                                    *
 *********************************************************************  */

int
setCobr_Bin128_Debug (int flag)
{
  if (flag)
    blir_bin128_debug = True;
  else
    blir_bin128_debug = False;
  return flag;
}

/**********************************************************************
 *                                                                    *
 * GetCobr_Bin128_Debug - Introgate the blir_bin128_debug flag        *
 *                                                                    *
 *********************************************************************  */

int
getCobr_Bin128_Debug (void)
{
  if (blir_bin128_debug)
    return 1;
  else
    return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_bin128_debugPutNum - Write a binLLL type number                *
 *                                                                    *
 *********************************************************************  */

int
blir_bin128_debugPutNum (const char *t,
                         const void *n)
{
  binLLLPtr un;
  int i;

  if (!blir_bin128_debug)
    return 0;

  un = (binLLLPtr)n;
  printf ("%s = ", t);
  for (i = _LLLUPPERBYTE; i <= _LLLOWERBYTE; i ++)
    printf ("%02X ", un[i]);
  printf ("\n");
  fflush (stdout);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Cobr_bin128_debugPause - Pause waiting for return                  *
 *                                                                    *
 *********************************************************************  */

int
blir_bin128_debugPause (char *prompt)
{
  char line[80];

  if (!blir_bin128_debug)
    return 0;

  printf ("%s: ", prompt);
  fflush (stdout);
  fgets (line, sizeof (line), stdin);

  return 0;
}
