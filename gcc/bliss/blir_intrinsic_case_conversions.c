

/**********************************************************************
 *                                                                    *
 *                       BLISS CASE CONVERSIONS intrinsic             *
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
 * Mail:           Curt Timmerman                                     *
 *                 PO Box 520153                                      *
 *                 Big Lake,  AK  99652                               *
 *                                                                    *
 * E-mail:         curt@curttimmerman.net                             *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title:          BLISS CASE CONVERSIONS intrinsic                   *
 *                                                                    *
 * Description:    This is a single function that implements the      *
 *                 CASE CONVERSIONS intrinsic.                        *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer     	Date         Description of Change            *
 * -------------  	-----------  -------------------------------- *
 * C. Timmerman   	29/Dec/2000  Initial Release of Routines      *
 *								      *
 * Linga Ramakrishna    18/Aug/2001 				      * 
 *			       Changed the function name from         * 
 *                             blir_intrinsic_lower_case to           *
 *                             blir_intrinsic_case_conversions.       *
 *                             And added the code for handling upper  *
 *                             case conversions in addition to the    *
 *                             existing lower case conversion         *
 *                             capabilities.                          *
 *								      *
 **********************************************************************/

#include "blir_temp_config.h"
#include "blir_intrinsic_case_conversions.h"

#include <ctype.h>
#include <stdlib.h>

/**********************************************************************
 *                                                                    *
 * Cobr_intrinsic_case_conversions -				      *
 *	Copies from address_from to address_to converting to either   *
 * 	lower / upper case depending on the value of 		      *
 *	case_conversion_type					      * 
 *								      *
 **********************************************************************/

void 
blir_intrinsic_case_conversions
(uchar *address_from,
 uchar *address_to,
 uint32 length,
 uint16 case_conversion_type)
{
  switch (case_conversion_type)
    {
    case TO_LOWER:
      for (; length > 0; length --)
        {
          * address_to = tolower (*address_from);
          address_from ++;
          address_to ++;
        }
      break;
    
    case TO_UPPER:
      for (; length > 0; length --)
        {
          * address_to = toupper (*address_from);
          address_from ++;
          address_to ++;
        }
      break;
      
    default:
      abort ();
    }

} /* End blir_intrinsic_case_conversions.  */

