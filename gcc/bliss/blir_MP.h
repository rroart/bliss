
/**********************************************************************
 *                                                                    *
 *          Multiple-Precision math routines common header file       *
 *                                                                    *
 *           Copyright (C) 2002 by David C. Sadler                    *
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
 * Mail:           David C. Sadler                                    *
 *                 PO Box 186                                         *
 *                 Cottekill, NY 12419                                *
 *                                                                    *
 * E-mail:         dsadler@hvc.rr.com                                 *
 *                                                                    *
 **********************************************************************/

/**********************************************************************
 *                                                                    *
 * Title:          Multiple-Precision binary math routines            *
 *                 common header file                                 *
 *                                                                    *
 * Description:    This file contains the data definitions used       *
 *                 by teh various Multiple-Precision math routines    *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * D> Sadler   08/May/2002  Initial Release of Routines               *
 * See ChangeLog for subsequent changes.                              *
 *                                                                    *
 **********************************************************************/

#ifndef __BLIR_MP_H__
#define __BLIR_MP_H__

#include <string.h>
#include "blir_temp_config.h"

/**********************************************************************
 *                                                                    *
 *                         Internal constants                         *
 *                                                                    *
 **********************************************************************/

#define _MP_MAX_DIGIT_LEN   (BLI_NATIVE_UINT_WIDTH)     /* number of digits in DIGIT_A  */
#define _MP_MAX_DIGIT_VALUE (BLI_NATIVE_UINT_MAX)       /* max value of a digit  */
#define _MP_BITS_PER_DIGIT  (BLI_NATIVE_UINT_SIZE)      /* number of bits in a DIGIT   */
typedef native_int BLI_MP_DIGIT; /* Type of digit.  */

uint32
blir_MP_add (BLI_MP_DIGIT w[], BLI_MP_DIGIT u[], BLI_MP_DIGIT v[]);
#endif
