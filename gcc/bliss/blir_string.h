/**********************************************************************
 *                                                                    *
 *                       BLISS STRING Statement                       *
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
 *                 Include file                                       *
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
 *                                                                    *
 *********************************************************************  */

int32
blir_string (uchar **from_item_array, uint32 *from_item_length_array,
            uint32 delimiter_count,
            uchar **delimiter_array, uint32 *delimiter_length_array,
            uint32 *number_of_items_this_delimiter_controls_array,
            uint32 pointer_offset,
            uchar *to_item, unsigned to_item_length);
