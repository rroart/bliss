
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
 *                 Signed 16 bit binary    blir_dtb_int16         *
 *                 Unsigned 16 bit binary  blir_dtb_uint16        *
 *                 Signed 32 bit binary    blir_dtb_int32         *
 *                 Unsigned 32 bit binary  blir_dtb_uint32        *
 *                 Signed 64 bit binary    blir_dtb_int64         *
 *                 Unsigned 64 bit binary  blir_dtb_uint64        *
 *                 Signed 128 bit binary   blir_dtb_int128 suppressed *
 *                 Unsigned 128 bit binary blir_dtb_uint128 suppressed *
 *                                                                    *
 *                 Packed Decimal from:                               *
 *                                                                    *
 *                 Signed 16 bit binary    blir_btd_int16         *
 *                 Unsigned 16 bit binary  blir_btd_uint16        *
 *                 Signed 32 bit binary    blir_btd_int32         *
 *                 Unsigned 32 bit binary  blir_btd_uint32        *
 *                 Signed 64 bit binary    blir_btd_int64         *
 *                 Unsigned 64 bit binary  blir_btd_uint64        *
 *                 Signed 128 bit binary   blir_btd_int128 suppressed *
 *                 Unsigned 128 bit binary blir_btd_uint128 suppressed *
 *                                                                    *
 *                 Validation:                                        *
 *                                                                    *
 *                 Validate Packed Number  blir_validate_packed       *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000Initial Release of    *
 *                 Routines                                           *
 *                                                                    *
 *********************************************************************  */


#ifndef __BLIR_DECBIN_H__
#define __BLIR_DECBIN_H__

#include "blir_temp_config.h"
#include "blir_globals.h"


/**********************************************************************
 *                                                                    *
 *                   Prototypes for Error Handlers                    *
 *                                                                    *
 *********************************************************************  */

typedef int ( *blir_int16_handler)(int *error_code,  /* Error Code.  */
                                       const char *error_string,  /* Error String.  */
                                       int16 *binary,  /* Binary Value.  */
                                       int decimal_length_in_bytes,  /* Length of decimal value.  */
                                       void *decimal_value);  /* Decimal Value.  */

typedef int ( *blir_uint16_handler)(int *error_code,  /* Error Code.  */
                                        const char *error_string,  /* Error String.  */
                                        uint16 *binary,  /* Binary Value.  */
                                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                                        void *decimal_value);  /* Decimal Value.  */

typedef int ( *blir_int32_handler)(int *error_code,  /* Error Code.  */
                                       const char *error_string,  /* Error String.  */
                                       int32 *binary,  /* Binary Value.  */
                                       int decimal_length_in_bytes,  /* Length of decimal value.  */
                                       void *decimal_value);  /* Decimal Value.  */

typedef int ( *blir_uint32_handler)(int *error_code,  /* Error Code.  */
                                        const char *error_string,  /* Error String.  */
                                        uint32 *binary,  /* Binary Value.  */
                                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                                        void *decimal_value);  /* Decimal Value.  */

typedef int ( *blir_int64_handler)(int *error_code,  /* Error Code.  */
                                       const char *error_string,  /* Error String.  */
                                       int64 *binary,  /* Binary Value.  */
                                       int decimal_length_in_bytes,  /* Length of decimal value.  */
                                       void *decimal_value);  /* Decimal Value.  */

typedef int ( *blir_uint64_handler)(int *error_code,  /* Error Code.  */
                                        const char *error_string,  /* Error String.  */
                                        uint64 *binary,  /* Binary Value.  */
                                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                                        void *decimal_value);  /* Decimal Value.  */
#if 0
typedef int ( *blir_int128_handler)(int *error_code,  /* Error Code.  */
                                        const char *error_string,  /* Error String.  */
                                        int128 * binary,  /* Binary Value.  */
                                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                                        void *decimal_value);  /* Decimal Value.  */

typedef int ( *blir_uint128_handler)(int *error_code,  /* Error Code.  */
                                         const char *error_string,  /* Error String.  */
                                         uint128 * binary,  /* Binary Value.  */
                                         int decimal_length_in_bytes,  /* Length of decimal value.  */
                                         void *decimal_value);  /* Decimal Value.  */
#endif

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */

int
blir_validate_packed (int length,  /* Decimal value length.  */
                      void *value,  /* Decimal value.  */
                      int signed_flag);  /* Signed decimal flag.  */

void
blir_btd_int16 (int16 *binary,  /* Binary value.  */
                    int decimal_length_in_bytes,  /* Length of decimal area.  */
                    void *decimal_value,  /* Decimal value.  */
                    blir_int16_handler ErrorRout);  /* Decimal error routine.  */

void
blir_btd_uint16 (uint16 *binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal area.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_uint16_handler ErrorRout);  /* Decimal error routine.  */

void
blir_btd_int32 (int32 *binary,  /* Binary value.  */
                    int decimal_length_in_bytes,  /* Length of decimal area.  */
                    void *decimal_value,  /* Decimal value.  */
                    blir_int32_handler ErrorRout);  /* Decimal error routine.  */

void
blir_btd_uint32 (uint32 *binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal area.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_uint32_handler ErrorRout);  /* Decimal error routine.  */

void
blir_btd_int64 (int64 *binary,  /* Binary value.  */
                    int decimal_length_in_bytes,  /* Length of decimal area.  */
                    void *decimal_value,  /* Decimal value.  */
                    blir_int64_handler ErrorRout);  /* Decimal error routine.  */

void
blir_btd_uint64 (uint64 *binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal area.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_uint64_handler ErrorRout);  /* Decimal error routine.  */
#if 0
void
blir_btd_int128 (int128 * binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal area.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_int128_handler ErrorRout);  /* Decimal error routine.  */

void
blir_btd_uint128 (uint128 * binary,  /* Binary value.  */
                      int decimal_length_in_bytes,  /* Length of decimal area.  */
                      void *decimal_value,  /* Decimal value.  */
                      blir_uint128_handler ErrorRout);  /* Decimal error routine.  */
#endif
void
blir_dtb_int16 (int16 *binary,  /* Binary value.  */
                    int decimal_length_in_bytes,  /* Length of decimal value.  */
                    void *decimal_value,  /* Decimal value.  */
                    blir_int16_handler ErrorRout);  /* Decimal error routine.  */

void
blir_dtb_uint16 (uint16 *binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal value.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_uint16_handler ErrorRout);  /* Decimal error routine.  */

void
blir_dtb_int32 (int32 *binary,  /* Binary value.  */
                    int decimal_length_in_bytes,  /* Length of decimal value.  */
                    void *decimal_value,  /* Decimal value.  */
                    blir_int32_handler ErrorRout);  /* Decimal error routine.  */

void
blir_dtb_uint32 (uint32 *binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal value.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_uint32_handler ErrorRout);  /* Decimal error routine.  */

void
blir_dtb_int64 (int64 *binary,  /* Binary value.  */
                    int decimal_length_in_bytes,  /* Length of decimal value.  */
                    void *decimal_value,  /* Decimal value.  */
                    blir_int64_handler ErrorRout);  /* Decimal error routine.  */

void
blir_dtb_uint64 (uint64 *binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal value.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_uint64_handler ErrorRout);  /* Decimal error routine.  */
#if 0
void
blir_dtb_int128 (int128 * binary,  /* Binary value.  */
                     int decimal_length_in_bytes,  /* Length of decimal value.  */
                     void *decimal_value,  /* Decimal value.  */
                     blir_int128_handler ErrorRout);  /* Decimal error routine.  */

void
blir_dtb_uint128 (uint128 * binary,  /* Binary value.  */
                      int decimal_length_in_bytes,  /* Length of decimal value.  */
                      void *decimal_value,  /* Decimal value.  */
                      blir_uint128_handler ErrorRout);  /* Decimal error routine.  */
#endif
#endif

