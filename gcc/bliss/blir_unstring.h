/**********************************************************************
 *                                                                    *
 *                       BLISS UNSTRING Statement                     *
 *               Copyright (C) 2000, 2001 by Steven O.  Ellis          *
 *               Copyright (C) 2001  by Tim Josling                   *
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
 * Title:          BLISS UNSTRING statement                           *
 *                 Include file                                       *
 *                                                                    *
 * Description:    This is a single function that implements the      *
 *                 UNSTRING statement.                                *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * See Changelog for change details                                   *
 *                                                                    *
 *********************************************************************  */

/* 
   Implement unstring, returning 1 on an overflow condition. 

   The item being analzed is FROM_ITEM which has length
   FROM_ITEM_LENGTH.  POINTER is the starting point, 1 based.
   TALLYING is an output field which gets the number of output fields
   written to.

   DELIMITER_ARRAY (addresses) and DELIMITER_LENGTH_ARRAY (lengths),
   specify the addresses of the delimiters to be used to split the
   input. DELIMITER_ALL_FLAG_ARRAY entries, if 1, specify that the
   corresponding delimter has the ALL keyword. 

   INTO_COUNT specifies the number of receiving fields. The reciving
   fields are specified using the INTO_ARRAY (address) and
   INTO_LENGTH_ARRAY (lengths) arrays.

   The delimiters can be placed into a separate receiving area,
   controlled by the DELIMITER_INTO_ARRAY (address or not present if
   NULL) and DELIMITER_INTO_LENGTH_ARRAY arrays. The size of these
   arrays is also controlled by INTO_COUNT. INTO_ARRAY_FLAGS will have
   flags (see usage flags in blir_temp_config.h) specifying the nature
   of any sign and the presence of any right aligned
   fields. INTO_ARRAY_TYPES has the data types for each receiving
   area, as per the enum blir_type in the blir_temp_config.h file. 

   The number of characters put into each receiving field is to be put
   into the COUNT_INTO_ARRAY entry for each field.  */

BLI_INT32
blir_unstring (BLI_UINT8 *from_item, BLI_UINT32 from_item_length, 
              BLI_UINT32 pointer, 
              BLI_UINT32 *tallying, 
              
              BLI_UINT32 delimiter_count,
              BLI_UINT8 **delimiter_array, BLI_UINT32 *delimiter_length_array,
              BLI_UINT32 *delimiter_all_flag_array, 
              
              BLI_UINT32 into_count,
              BLI_UINT8 **into_array, BLI_UINT32 *into_length_array,
              BLI_UINT32 *into_array_types,
              BLI_UINT32 *into_array_flags,
              BLI_UINT8 **delimiter_into_array, BLI_UINT32 *delimiter_into_length_array,
              BLI_UINT32 *count_into_array);
