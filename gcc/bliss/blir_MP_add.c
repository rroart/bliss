
/**********************************************************************
 *                                                                    *
 *        Multiple-Precision Add Routine for BLISS Runtime system     *
 *                                                                    *
 *           Copyright (C) 2002 by David C. Sadler       .            *
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
 * Title:          Multiple-Precision Add Routine for BLISS runtime   *
 *                 system                                             *
 *                                                                    *
 * Description:    This file consists of a single routine to perform  *
 *                 multiple-precesion add on unsigned n-place         *
 *                 integers. The "term n-place integer" means any     *
 *                 integer less than b**n, where b is the hardware's  *
 *                 word size. For example, an integer that fills 4    *
 *                 words on a computer whose word size is 2**32       *
 *                 we be considered a 4-place numer to the base       *
 *                 2**32.                                             *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * D. Sadler   09/May/2002  Initial Release of Routine                *
 * See ChangeLog for subsequent changes.                              *
 *                                                                    *
 **********************************************************************/

#include "blir_MP.h"

/**********************************************************************
 *    blir_MP_add  Calculates w = u + v                               *
 *    where w, u, v are multiprecision integers of ndigits each       *
 *    Returns carry if overflow. Carry = 0 or 1.                      *
 *                                                                    *
 *    Ref: Knuth Vol 2 Ch 4.3.1 p 231 Algorithm A.                    *
 **********************************************************************/

uint32
blir_MP_add (BLI_MP_DIGIT w[],  /* result of adding u+v */
             BLI_MP_DIGIT u[],  /* operand one */
             BLI_MP_DIGIT v[])  /* operand two */
{

  BLI_MP_DIGIT k;               /* single digit */
  native_int j;             /* loop control index */

/*--------------------------------------------------------------------*
 *           Step A1. Initialize                                      *
 *--------------------------------------------------------------------*/

  k = 0;

  for (j = _MP_MAX_DIGIT_LEN - 1; j > 0; j--)
    {

/*--------------------------------------------------------------------*        
 *      Step A2. Add digits w_j = (u_j + v_j + k)                     *
 *      Set k = 1 if carry (overflow) occurs                          *
 *--------------------------------------------------------------------*/

      w[j] = u[j] + k;
      if (w[j] < k)
        k = 1;
      else
        k = 0;

      w[j] += v[j];
      if (w[j] < v[j])
        k++;

    }                           /* Step A3. Loop on j */

  return k;                     /* w_n & k */
}
