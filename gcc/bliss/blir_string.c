/**********************************************************************
 *                                                                    *
 *                       BLISS STRING Statement                       *
 *                                                                    *
 *               Copyright (C) 2000, 2001 by Steven O.  Ellis                *
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
 * Mail:           Steven O. Ellis                                    *
 *                 1 Squire Ct.                                       *
 *                 Urbana, IL 61802                                   *
 *                                                                    *
 * E-mail:         soellis@soltec.net                                 *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title:          BLISS STRING statement                             *
 *                                                                    *
 * Description:    This is a single function that implements the      *
 *                 STRING statement.                                  *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * S. Ellis    01/Dec/2000  Initial Release of Routines               *
 * S. Ellis    03/Mar/2001  Changed to return an overflow condition   *
 *                          if pointer offset = 0.  Since parameters  *
 *                          are checked before this function is       *
 *                          called, this code is for DEBUG mode only. *
 * S. Ellis    02/Jun/2001  DEBUG mode requirement removed for check  *
 *                          of pointer offset = 0.  This is a stan-   *
 *                          dard overflow check.                      *
 *                                                                    *
 *********************************************************************  */

#include "blir_temp_config.h"

#include <stddef.h>
#include "blir_string.h"

/**********************************************************************
 *                                                                    *
 * Cobr_string - implements the STRING function.  The full or partial *
 *               contents of one or more source fields are concaten-  *
 *               ated byte-by-byte into a single target field         *
 *               starting at a given position.  If the starting       *
 *               position is not specified, then the first byte of    *
 *               the target field is assumed.  Any byte in the target *
 *               field not overwritten is left unchanged. The source  *
 *               fields remain unchanged.                             *
 *                                                                    *
 * This routine does not access global variables.  The passed         *
 * parameters absolutely determine the source and target fields,      *
 * and the modification, if any, of the target field.  The following  *
 * description of the parameters assumes that the reader is familiar  *
 * with the syntax of the STRING statement.                           *
 *                                                                    *
 *                                                                    *
 *    Parameters Describing the Source Fields                         *
 *                                                                    *
 *    from_item_array - array of pointers to pointers to char,        *
 *                      where each pointer to char points to          *
 *                      one of the source fields                      *
 *    from_item_lenth_array -                                         *
 *                      array of pointers to int, where each          *
 *                      pointer to int points to the length of        *
 *                      the corresponding source field in             *
 *                      from_item_array                               *
 *                                                                    *
 *                                                                    *
 *    Parameters Describing the Delimiters                            *
 *                                                                    *
 *    delimiter_count - the number of delimiters specified in the     *
 *                      STRING statement                              *
 *    delimiter_array - array of pointers to pointers to char,        *
 *                      where each pointer to char points to one      *
 *                      of the delimiters.                            *
 *    delimiter_length_array -                                        *
 *                      array of pointers to int, where each          *
 *                      each pointer to int points to the length      *
 *                      of the corresponding delimiter in             *
 *                      delimiter_array.  If the entire contents      *
 *                      of the source field(s) being acted on by      *
 *                      the delimiter is to be moved into the         *
 *                      target field, then the length will be 0       *
 *                      and the pointer to the delimiter in the       *
 *                      delimiter array will point to NULL.           *
 *                      (This is the case where the original BLISS    *
 *                      statement specifies SIZE as the delimiter.)   *
 *    number_of_items_this_delimiter_controls_array -                 *
 *                      array of pointers to int, where each          *
 *                      pointer to int points to the number of        *
 *                      source fields that the corresponding          *
 *                      delimiter in delimiter_array acts upon        *
 *                                                                    *
 *                                                                    *
 *    Parameter Describing the Starting Position                      *
 *    in the Target Field                                             *
 *                                                                    *
 *    pointer_offset  - int containing the position in the target     *
 *                      field where the action starts.  The           *
 *                      smallest possible value is 1.  If             *
 *                      pointer_offset is not specified, then a       *
 *                      default of 1 is assumed.                      *
 *                                                                    *
 *                                                                    *
 *    Parameters Describing the Target Fields                         *
 *                                                                    *
 *    to_item         - pointer to char that points to the target     *
 *                      field                                         *
 *                                                                    *
 *    to_item_length  - int contintaining the length of the target    *
 *                      field                                         *
 *                                                                    *
 *                                                                    *
 *********************************************************************  */

int32
blir_string (uchar **from_item_array, uint32 *from_item_length_array,
            uint32 delimiter_count,
            uchar **delimiter_array, uint32 *delimiter_length_array,
            uint32 *number_of_items_this_delimiter_controls_array,
            uint32 pointer_offset,
            uchar *to_item, uint32 to_item_length)
{
  uint32 j;
  uint32 from_item_no, from_item_offset, to_item_offset;
  uint32 delimiter_no, delimiter_offset, delimiter_start;


  /*--------------------------------------------------------------------*
   * Check for overflow                                                 *
   *--------------------------------------------------------------------.  */
  
  if (pointer_offset == 0 || pointer_offset > to_item_length)
    return 1;

  /*--------------------------------------------------------------------*
   * Initialize local variables      
   *--------------------------------------------------------------------.  */
  from_item_no = 0;
  to_item_offset = pointer_offset - 1;

  /*--------------------------------------------------------------------*
   * Handle delimiters one-at-a-time
   *--------------------------------------------------------------------.  */
  for (delimiter_no = 0; delimiter_no < delimiter_count; delimiter_no ++)
    if (delimiter_array[delimiter_no] == NULL)

      /*--------------------------------------------------------------------*
       * Delimiter is "SIZE", so move every character of each item          *
       * controlled by this delimiter until done or overflow                *
       *--------------------------------------------------------------------.  */

      for (j = 0; j < number_of_items_this_delimiter_controls_array[delimiter_no]; j ++)
        {
          for (from_item_offset = 0; 
               from_item_offset < from_item_length_array[from_item_no]; 
               from_item_offset ++)
            {
              /*--------------------------------------------------------------------*
               * Check for overflow                                                 *
               *--------------------------------------------------------------------.  */
              if (to_item_offset >= to_item_length)
                return 1;

              to_item[to_item_offset ++] = 
                from_item_array[from_item_no][from_item_offset];
            }
          ++from_item_no;
        }
    else

      /*--------------------------------------------------------------------*
       * Delimiter is a sequence of one or more characters.                 *
       *--------------------------------------------------------------------.  */
      {
        for (j = 0; 
             j < number_of_items_this_delimiter_controls_array[delimiter_no]; 
             j ++)
          {
                
            /*--------------------------------------------------------------------*
             * Find the the offset of the first instance, if any, of the          *
             * delimiter in the item to be moved.  If none found, delimiter_start  *
             * will be equal to the length of the item and will have no effect.   *
             *--------------------------------------------------------------------.  */

            delimiter_start = 0;

            while (delimiter_start <= from_item_length_array[from_item_no])
              {
                delimiter_offset = 0; 
                from_item_offset = delimiter_start;
                while (delimiter_offset < delimiter_length_array[delimiter_no])
                  {
                    if (from_item_offset >= from_item_length_array[from_item_no])
                      break;
                      
                    if (delimiter_array[delimiter_no][delimiter_offset] !=
                        from_item_array[from_item_no][from_item_offset])
                      break;
                        
                    delimiter_offset ++, from_item_offset ++;
                  }

                if (delimiter_offset == delimiter_length_array[delimiter_no])
                  break;

                delimiter_start ++;
              }

            /*--------------------------------------------------------------------*
             * Move characters until the start of the first instance of the       *
             * delimiter.  If none, move characters until done or overflow.       *
             *--------------------------------------------------------------------.  */
            for (from_item_offset = 0; 
                 from_item_offset < from_item_length_array[from_item_no]; 
                 from_item_offset ++)
              {
                /*--------------------------------------------------------------------*
                 * Check for overflow                                                 *
                 *--------------------------------------------------------------------.  */
                if (to_item_offset >= to_item_length)
                  return 1;

                if (from_item_offset >= delimiter_start)
                  break;

                to_item[to_item_offset ++] = 
                  from_item_array[from_item_no][from_item_offset];
              }
            ++from_item_no;
          }
      }
  return 0;
}


