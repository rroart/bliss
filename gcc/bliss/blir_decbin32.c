/**********************************************************************
 *                                                                    *
 *        Conversion routines for BLISS Binary / Decimal data         *
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
 * Title:          Conversion routines for BLISS Binary / Decimal     *
 *                 data                                               *
 *                                                                    *
 * Description:    This consists of a serious of routines to convert  *
 *                 BLISS Binary / Decimal data.  These routines will  *
 *                 permit:                                            *
 *                 *  Conversion from packed decimal to various       *
 *                    binary types                                    *
 *                                                                    *
 *                 *  Conversion from various binary types to packed  *
 *                    decimal                                         *
 *                                                                    *
 *                 *  Validate of packed decimal values               *
 *                                                                    *
 *                 Packed Decimal to:                                 *
 *                                                                    *
 *                 Signed 32 bit binary    blir_dtb_int32         *
 *                 Unsigned 32 bit binary  blir_dtb_uint32        *
 *                                                                    *
 *                 Packed Decimal from:                               *
 *                                                                    *
 *                 Signed 32 bit binary    blir_btd_int32         *
 *                 Unsigned 32 bit binary  blir_btd_uint32        *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000Initial Release of    *
 *                 Routines                                           *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 *          Constants for 32 bit binary / decimal conversion          *
 *                                                                    *
 *********************************************************************  */

#define int_WIDTH BLI_INT32_WIDTH /* 4 for full 32 bit support */

#define int_BTD_NAME    blir_btd_int32  /* Name of Signed 32 bit Binary to Decimal */
#define int_DTB_NAME    blir_dtb_int32  /* Name of Decimal to Signed 32 bit Binary */
#define int_HANDLER     blir_int32_handler      /* Name of Signed 32 bit handler typedef */

#define int_TYPE        int32   /* Data type for Signed routines */

#define uint_BTD_NAME   blir_btd_uint32         /* Name of Unsigned 32 bit Binary to Decimal */
#define uint_DTB_NAME   blir_dtb_uint32         /* Name of Decimal to Unsigned 32 bit Binary */
#define uint_HANDLER    blir_uint32_handler     /* Name of Unsigned 32 bit handler typedef */

#define uint_TYPE      uint32   /* Data type for Unsigned routines */

/**********************************************************************
 *                                                                    *
 * Bring in the source code and let the pre-processor carry out the   *
 * replacements                                                       *
 *                                                                    *
 *********************************************************************  */


#include "blir_decbxx.h"
