
/**********************************************************************
 *                                                                    *
 *        Conversion routines for BLISS Binary / Decimal data         *
 *                                                                    *
 *           Copyright (C) 2000 by Theodore J.  Seward, Jr.            *
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
 *                 Signed 64 bit binary    blir_dtb_BLI_INT64             *
 *                 Unsigned 64 bit binary  blir_dtb_BLI_UINT64            *
 *                                                                    *
 *                 Packed Decimal from:                               *
 *                                                                    *
 *                 Signed 64 bit binary    blir_btd_BLI_INT64             *
 *                 Unsigned 64 bit binary  blir_btd_BLI_UINT64            *
 *                                                                    *
 * System:         BLISS For GCC Runtime Library                      *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000Initial Release of    *
 *                 Routines                                           *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 *          Constants for 64 bit binary / decimal conversion          *
 *                                                                    *
 *********************************************************************  */

#define BLI_INT_WIDTH BLI_INT64_WIDTH   /* 8 for full 64 bit support */

#define BLI_INT_BTD_NAME    blir_btd_BLI_INT64  /* Name of Signed 64 bit Binary to Decimal */
#define BLI_INT_DTB_NAME    blir_dtb_BLI_INT64  /* Name of Decimal to Signed 64 bit Binary */
#define BLI_INT_HANDLER     blir_BLI_INT64_handler      /* Name of Signed 64 bit handler typedef */

#define BLI_INT_TYPE        BLI_INT64   /* Data type for Signed routines */

#define BLI_UINT_BTD_NAME   blir_btd_BLI_UINT64 /* Name of Unsigned 64 bit Binary to Decimal */
#define BLI_UINT_DTB_NAME   blir_dtb_BLI_UINT64 /* Name of Decimal to Unsigned 64 bit Binary */
#define BLI_UINT_HANDLER    blir_BLI_UINT64_handler     /* Name of Unsigned 64 bit handler typedef */

#define BLI_UINT_TYPE      BLI_UINT64   /* Data type for Unsigned routines */

/**********************************************************************
 *                                                                    *
 * Bring in the source code and let the pre-processor carry out the   *
 * replacements                                                       *
 *                                                                    *
 *********************************************************************  */


#include "blir_decbxx.h"
