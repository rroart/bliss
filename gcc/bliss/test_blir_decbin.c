
/**********************************************************************
 *                                                                    *
 *                          W A R N I N G !                           *
 *                          - - - - - - - -                           *
 *                                                                    *
 * I have turned of the 128 but testing until we sort out the printf  *
 * etc issues Ted describes below.  Tim Josling 4 Nov 2000.            *
 *                                                                    *
 * At the time that I coded this file, I did not know what the        *
 * int128 (int64) constant suffix would be.  I used LL for    *
 * int128 signed binary value and LLU for uint128 uint32  *
 * binary values.  I also did not know how to code the printf format, *
 * so I coded %llld and %lllu.  These suffixes and print formats will *
 * need to be changed coorisponding to the correct values.  Look for  *
 * the following line:                                                *
 *                                                                    *
 * #define TEST128_BIN_FMT "%llld" #define TEST128_BIN_MAXPOS         *
 * (170141183460469231731687303715884105727LL) #define                *
 * TEST128_BIN_MAXNEG                                                 *
 * (-170141183460469231731687303715884105727LL-1LL) #define           *
 * TEST128_UBIN_FMT "%lllu" #define TEST128_UBIN_MAX                  *
 * 340282366920938463463374607431768211455ULL                         *
 *                                                                    *
 * and change the occurances of "LL", "ULL", "%llld", and "%lllu" to  *
 * the correct values.  For example, if the letter is "X", then you   *
 * would code:                                                        *
 *                                                                    *
 * #define TEST128_BIN_FMT "%Xd" #define TEST128_BIN_MAXPOS           *
 * (170141183460469231731687303715884105727X) #define                 *
 * TEST128_BIN_MAXNEG  (-170141183460469231731687303715884105727X-1X) *
 * #define TEST128_UBIN_FMT "%Xu" #define TEST128_UBIN_MAX            *
 * 340282366920938463463374607431768211455UX                          *
 *                                                                    *
 * After making this change, delete this warning from the file.       *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 *        Conversion routines for BLISS Binary / Decimal data         *
 *                                                                    *
 *           Copyright (C) 2000 by Theodore J.  Seward, Jr.            *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of the *
 * License, or (at your option) any later version.                    *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
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
 * Description:    This consists of a series of routines to convert   *
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
 *                            Test The following Routines             *
 *                                                                    *
 *                 Packed Decimal to:                                 *
 *                                                                    *
 *                 Signed 16 bit binary    blir_dtb_int16         *
 *                 Unsigned 16 bit binary  blir_dtb_uint16        *
 *                 Signed 32 bit binary    blir_dtb_int32         *
 *                 Unsigned 32 bit binary  blir_dtb_uint32        *
 *                 Signed 64 bit binary    blir_dtb_int64         *
 *                 Unsigned 64 bit binary  blir_dtb_uint64        *
 *                 Signed 128 bit binary   blir_dtb_int128        *
 *                 Unsigned 128 bit binary blir_dtb_uint128       *
 *                                                                    *
 *                 Packed Decimal from:                               *
 *                                                                    *
 *                 Signed 16 bit binary    blir_btd_int16         *
 *                 Unsigned 16 bit binary  blir_btd_uint16        *
 *                 Signed 32 bit binary    blir_btd_int32         *
 *                 Unsigned 32 bit binary  blir_btd_uint32        *
 *                 Signed 64 bit binary    blir_btd_int64         *
 *                 Unsigned 64 bit binary  blir_btd_uint64        *
 *                 Signed 128 bit binary   blir_btd_int128        *
 *                 Unsigned 128 bit binary blir_btd_uint128       *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blir_temp_config.h"
#include "blir_globals.h"
#include "blir_decbin.h"

/**********************************************************************
 *                                                                    *
 *      Set 64 bit testing values based on configured data width      *
 *                                                                    *
 *********************************************************************  */


/*--------------------------------------------------------------------*
 *          Constants for 32 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#if BLI_INT64_WIDTH == 4
#define TEST64_BIN_FMT  "%d"    /* Format to print signed binary number */
#define TEST64_BIN_MAXPOS  (2147483647)         /* Maximum positive binary value */
#define TEST64_BIN_MAXNEG  (-2147483647-1)              /* Maximum negative binary value */
#define TEST64_DEC_MAXPOS (uchar*)"\x02\x14\x74\x83\x64\x7C"        /* Maximum positive packed value */
#define TEST64_DEC_MAXNEG (uchar*)"\x02\x14\x74\x83\x64\x8D"        /* Maximum negative packed value */
#define TEST64_DEC_MAXPOS_P1 (uchar*)"\x02\x14\x74\x83\x64\x8C"
/* Maximum positive packed value + 1.  */
#define TEST64_DEC_MAXNEG_P1 (uchar*)"\x02\x14\x74\x83\x64\x9D"     /* Maximum negative packed value - 1*/
#define TEST64_DEC_LENGTH (6)   /* Maximum packed value length */

#define TEST64_UBIN_FMT "%u"    /* Format to print unsigned binary number */
#define TEST64_UBIN_MAX 4294967295U     /* Maximum unsigned binary value */
#define TEST64_UDEC_MAX (uchar*)"\x04\x29\x49\x67\x29\x5F"  /* Maximum unsigned decimal value */
#define TEST64_UDEC_MAX_P1 (uchar*)"\x04\x29\x49\x67\x29\x6F"
/* Maximum unsigned decimal value + 1.  */
#define TEST64_UDEC_LENGTH (6) /* Maximum packed value length */

/*--------------------------------------------------------------------*
 *          Constants for 64 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#elif BLI_INT64_WIDTH == 8

#define TEST64_BIN_FMT "%lld" /* Format to print signed binary number */
#define TEST64_BIN_MAXPOS  (9223372036854775807LL)      /* Maximum positive binary value */
#define TEST64_BIN_MAXNEG  (-9223372036854775807LL-1LL) /* Maximum negative binary value */
#define TEST64_DEC_MAXPOS (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x7C"
/* Maximum negative positive value.  */
#define TEST64_DEC_MAXNEG (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x8D"
/* Maximum negative packed value.  */
#define TEST64_DEC_MAXPOS_P1 (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x8C"
/* Maximum negative positive value + 1.  */
#define TEST64_DEC_MAXNEG_P1 (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x9D"
/* Maximum negative packed value - 1.  */
#define TEST64_DEC_LENGTH (10)  /* Maximum packed value length */

#define TEST64_UBIN_FMT "%llu" /* Format to print unsigned binary number */
#define TEST64_UBIN_MAX  18446744073709551615ULL        /* Maximum unsigned binary value */
#define TEST64_UDEC_MAX (uchar*)"\x01\x84\x46\x74\x40\x73\x70\x95\x51\x61\x5F"
/* Maximum unsigned binary value.  */
#define TEST64_UDEC_MAX_P1 (uchar*)"\x01\x84\x46\x74\x40\x73\x70\x95\x51\x61\x6F"
/* Maximum unsigned binary value + 1.  */
#define TEST64_UDEC_LENGTH (11) /* Maximum packed value length */

#endif

/**********************************************************************
 *                                                                    *
 *     Set 128 bit testing values based on configured data width      *
 *                                                                    *
 *********************************************************************  */


/*--------------------------------------------------------------------*
 *          Constants for 32 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#if int128_WIDTH == 4
#define TEST128_BIN_FMT (uchar*)"%d" /* Format to print signed binary number */
#define TEST128_BIN_MAXPOS  (2147483647)                /* Maximum positive binary value */
#define TEST128_BIN_MAXNEG  (-2147483647-1)             /* Maximum negative binary value */
#define TEST128_DEC_MAXPOS (uchar*)"\x02\x14\x74\x83\x64\x7C"       /* Maximum positive packed value */
#define TEST128_DEC_MAXNEG (uchar*)"\x02\x14\x74\x83\x64\x8D"       /* Maximum negative packed value */
#define TEST128_DEC_MAXPOS_P1 (uchar*)"\x02\x14\x74\x83\x64\x8C"    /* Maximum positive packed value + 1*/
#define TEST128_DEC_MAXNEG_P1 (uchar*)"\x02\x14\x74\x83\x64\x9D"    /* Maximum negative packed value - 1*/
#define TEST128_DEC_LENGTH (6)  /* Maximum packed value length */

#define TEST128_UBIN_FMT "%u" /* Format to print signed binary number */
#define TEST128_UBIN_MAX 4294967295U    /* Maximum unsigned binary value */
#define TEST128_UDEC_MAX (uchar*)"\x04\x29\x49\x67\x29\x5F" /* Maximum unsigned decimal value */
#define TEST128_UDEC_MAX_P1 (uchar*)"\x04\x29\x49\x67\x29\x6F"      /* Maximum unsigned decimal value + 1 */
#define TEST128_UDEC_LENGTH (6) /* Maximum packed value length */

/*--------------------------------------------------------------------*
 *          Constants for 64 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#elif int128_WIDTH == 8

#define TEST128_BIN_FMT (uchar*)"%lld" /* Format to print signed binary number */
#define TEST128_BIN_MAXPOS  (9223372036854775807LL)     /* Maximum positive binary value */
#define TEST128_BIN_MAXNEG  (-9223372036854775807LL-1LL) /* Maximum negative binary value */
#define TEST128_DEC_MAXPOS (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x7C"       /* Maximum negative positive value */
#define TEST128_DEC_MAXNEG (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x8D"       /* Maximum negative packed value */
#define TEST128_DEC_MAXPOS_P1 (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x8C"    /* Maximum negative positive value + 1 */
#define TEST128_DEC_MAXNEG_P1 (uchar*)"\x92\x23\x37\x20\x36\x85\x47\x75\x80\x9D"    /* Maximum negative packed value - 1 */
#define TEST128_DEC_LENGTH (10) /* Maximum packed value length */

#define TEST128_UBIN_FMT "%llu" /* Format to print signed binary number */
#define TEST128_UBIN_MAX  18446744073709551615UL                /* Maximum unsigned binary value */
#define TEST128_UDEC_MAX (uchar*)"\x01\x84\x46\x74\x40\x73\x70\x95\x51\x61\x5F"             /* Maximum unsigned binary value */
#define TEST128_UDEC_MAX_P1 (uchar*)"\x01\x84\x46\x74\x40\x73\x70\x95\x51\x61\x6F"          /* Maximum unsigned binary value + 1 */
#define TEST128_UDEC_LENGTH (11) /* Maximum packed value length */

/*--------------------------------------------------------------------*
 *         Constants for 128 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#elif int128_WIDTH == 16

#define TEST128_BIN_FMT (uchar*)"%llld" /* Format to print signed binary number */
#define TEST128_BIN_MAXPOS  (170141183460469231731687303715884105727LL) /* Maximum positive binary value */
#define TEST128_BIN_MAXNEG  (-170141183460469231731687303715884105727LL-1LL)    /* Maximum negative binary value */
#define TEST128_DEC_MAXPOS (uchar*)"\x17\x01\x41\x18\x34\x60\x46\x92\x31\x73\x16\x87\x30\x37\x15\x88\x41\x05\x72\x7C"       /* Maximum positive packed value */
#define TEST128_DEC_MAXNEG (uchar*)"\x17\x01\x41\x18\x34\x60\x46\x92\x31\x73\x16\x87\x30\x37\x15\x88\x41\x05\x72\x8D"       /* Maximum negative packed value */
#define TEST128_DEC_MAXPOS_P1 (uchar*)"\x17\x01\x41\x18\x34\x60\x46\x92\x31\x73\x16\x87\x30\x37\x15\x88\x41\x05\x72\x8C"    /* Maximum positive packed value + 1 */
#define TEST128_DEC_MAXNEG_P1 (uchar*)"\x17\x01\x41\x18\x34\x60\x46\x92\x31\x73\x16\x87\x30\x37\x15\x88\x41\x05\x72\x9D"    /* Maximum negative packed value - 1 */
#define TEST128_DEC_LENGTH (20) /* Maximum negative packed value length */

#define TEST128_UBIN_FMT "%lllu" /* Format to print signed binary number */
#define TEST128_UBIN_MAX 340282366920938463463374607431768211455ULL             /* Maximum unsigned binary value */
#define TEST128_UDEC_MAX (uchar*)"\x34\x02\x82\x36\x69\x20\x93\x84\x63\x46\x33\x74\x60\x74\x31\x76\x82\x11\x45\x5F"         /* Maximum character value */
#define TEST128_UDEC_MAX_P1 (uchar*)"\x34\x02\x82\x36\x69\x20\x93\x84\x63\x46\x33\x74\x60\x74\x31\x76\x82\x11\x45\x6F"              /* Maximum character value + 1 */
#define TEST128_UDEC_LENGTH (20) /* Maximum packed value length */

#endif

/**********************************************************************
 *                                                                    *
 * Error_report_dumpVar - Dump variables in error routine             *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_dumpVar (int32 binary,  /* Binary Value.  */
                     int length,  /* Decimal length.  */
                     void *value) /* Decimal value.  */
{
  int32 i;  /* Index.  */
  uchar *cptr;  /* Pointer to value.  */

  printf ("\t\tbinary=%d  length=%d  value=", binary, length);
  cptr = (uchar *)value;
  for (i = 0; i < length; i ++)
    printf ("%02X ", cptr[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_int16 - Error handler for Decimal/Binary          *
 *                      conversion using a 16 bit signed binary       *
 *                      number.                                       *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_int16 (int *error_code,  /* Error Code.  */
                        const char *error_string,  /* Error String.  */
                        int16 *binary,  /* Binary Value.  */
                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                        void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_INT16 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_uint16 - Error handler for Decimal/Binary         *
 *                      conversion using a 16 bit uint32 binary   *
 *                      number.                                       *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_uint16 (int *error_code,  /* Error Code.  */
                         const char *error_string,  /* Error String.  */
                         uint16 *binary,  /* Binary Value.  */
                         int decimal_length_in_bytes,  /* Length of decimal value.  */
                         void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_UINT16 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_validate_packed - Carry out a test of the                *
 *                             blir_validate_packed routine.  After   *
 *                             the call, the input and output values  *
 *                             will be displayed.                     *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_validate_packed (int length,  /* Decimal value length.  */
                           const void *value,  /* Decimal value.  */
                           int signed_flag) /* Signed decimal flag.  */
{
  int retCode;  /* Return Code.  */
  int i;  /* Index.  */
  uchar locDec[128];

  printf ("\nblir_validate_packed:\n\n");
  memcpy (locDec, value, length);

  retCode = blir_validate_packed (length, locDec, signed_flag);

  printf ("\tsgn = %d\n\tRet = %d\n\tDec = ", signed_flag, retCode);
  for (i = 0; i < length; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_int16 - Carry out a test of the                  *
 *                       blir_btd_int16 routine (Signed 16 bit    *
 *                       binary to packed decimal).  After the call,  *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_int16 (int16 binary,  /* Binary Value.  */
                         int decLength,  /* Decimal Length.  */
                         blir_int16_handler ErrorRout) /* Decimal error routine.  */
{

  int16 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_INT16:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_int16 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", binary);
  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_int16 - Carry out a test of the                  *
 *                       blir_dtb_int16 routine (Packed decimal   *
 *                       to Signed 16 bit binary).  After the call,   *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_int16 (const uchar *dec,  /* Decimal Value.  */
                         int decLength,  /* Decimal Length.  */
                         blir_int16_handler ErrorRout) /* Decimal error routine.  */
{

  int16 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_INT16:\n\n");

  blir_dtb_int16 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", locBinary);

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_uint16 - Carry out a test of the                 *
 *                       blir_btd_uint16 routine (Unsigned 16 bit *
 *                       binary to packed decimal).  After the call,  *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_uint16 (uint16 binary,  /* Binary Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_uint16_handler ErrorRout) /* Decimal error routine.  */
{

  uint16 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_UINT16:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_uint16 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", binary);
  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_uint16 - Carry out a test of the                 *
 *                       blir_dtb_uint16 routine (Packed decimal  *
 *                       to Unsigned 16 bit binary).  After the call, *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_uint16 (const uchar *dec,  /* Decimal Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_uint16_handler ErrorRout) /* Decimal error routine.  */
{

  uint16 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_UINT16:\n\n");

  blir_dtb_uint16 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", locBinary);

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_int32 - Error handler for Decimal/Binary          *
 *                      conversion using a 32 bit signed binary       *
 *                      number.                                       *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_int32 (int *error_code,  /* Error Code.  */
                        const char *error_string,  /* Error String.  */
                        int32 *binary,  /* Binary Value.  */
                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                        void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_INT32 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_uint32 - Error handler for Decimal/Binary         *
 *                      conversion using a 32 bit uint32 binary   *
 *                      number.                                       *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_uint32 (int *error_code,  /* Error Code.  */
                         const char *error_string,  /* Error String.  */
                         uint32 *binary,  /* Binary Value.  */
                         int decimal_length_in_bytes,  /* Length of decimal value.  */
                         void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_UINT32 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_int32 - Carry out a test of the                  *
 *                       blir_btd_int32 routine (Signed 32 bit    *
 *                       binary to packed decimal).  After the call,  *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_int32 (int32 binary,  /* Binary Value.  */
                         int decLength,  /* Decimal Length.  */
                         blir_int32_handler ErrorRout) /* Decimal error routine.  */
{

  int32 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_INT32:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_int32 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", binary);
  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_int32 - Carry out a test of the                  *
 *                       blir_dtb_int32 routine (Packed decimal   *
 *                       to Signed 32 bit binary).  After the call,   *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_int32 (const uchar *dec,  /* Decimal Value.  */
                         int decLength,  /* Decimal Length.  */
                         blir_int32_handler ErrorRout) /* Decimal error routine.  */
{

  int32 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_INT32:\n\n");

  blir_dtb_int32 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", locBinary);

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_uint32 - Carry out a test of the                 *
 *                       blir_btd_uint32 routine (Unsigned 32 bit *
 *                       binary to packed decimal).  After the call,  *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_uint32 (uint32 binary,  /* Binary Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_uint32_handler ErrorRout) /* Decimal error routine.  */
{

  uint32 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_UINT32:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_uint32 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", binary);
  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_uint32 - Carry out a test of the                 *
 *                       blir_dtb_uint32 routine (Packed decimal  *
 *                       to Unsigned 32 bit binary).  After the call, *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_uint32 (const uchar *dec,  /* Decimal Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_uint32_handler ErrorRout) /* Decimal error routine.  */
{

  uint32 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_UINT32:\n\n");

  blir_dtb_uint32 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = %d\n\tDec = ", locBinary);

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_int64 - Error handler for Decimal/Binary          *
 *                      conversion using a 64 bit signed binary       *
 *                      number.                                       *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_int64 (int *error_code,  /* Error Code.  */
                        const char *error_string,  /* Error String.  */
                        int64 *binary,  /* Binary Value.  */
                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                        void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_INT64 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_uint64 - Error handler for Decimal/Binary         *
 *                      conversion using a 64 bit uint32 binary   *
 *                      number.                                       *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_uint64 (int *error_code,  /* Error Code.  */
                         const char *error_string,  /* Error String.  */
                         uint64 *binary,  /* Binary Value.  */
                         int decimal_length_in_bytes,  /* Length of decimal value.  */
                         void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_UINT64 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_int64 - Carry out a test of the                  *
 *                       blir_btd_int64 routine (Signed 64 bit    *
 *                       binary to packed decimal).  After the call,  *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_int64 (int64 binary,  /* Binary Value.  */
                         int decLength,  /* Decimal Length.  */
                         blir_int64_handler ErrorRout) /* Decimal error routine.  */
{

  int64 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_INT64:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_int64 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST64_BIN_FMT, binary);
  printf ("\n\tDec = ");
  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_int64 - Carry out a test of the                  *
 *                       blir_dtb_int64 routine (Packed decimal   *
 *                       to Signed 64 bit binary).  After the call,   *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_int64 (const uchar *dec,  /* Decimal Value.  */
                         int decLength,  /* Decimal Length.  */
                         blir_int64_handler ErrorRout) /* Decimal error routine.  */
{

  int64 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_INT64:\n\n");

  blir_dtb_int64 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST64_BIN_FMT, locBinary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_uint64 - Carry out a test of the                 *
 *                       blir_btd_uint64 routine (Unsigned 64 bit *
 *                       binary to packed decimal).  After the call,  *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_uint64 (uint64 binary,  /* Binary Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_uint64_handler ErrorRout) /* Decimal error routine.  */
{

  uint64 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_UINT64:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_uint64 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST64_UBIN_FMT, binary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_uint64 - Carry out a test of the                 *
 *                       blir_dtb_uint64 routine (Packed decimal  *
 *                       to Unsigned 64 bit binary).  After the call, *
 *                       the input and output values will be          *
 *                       displayed.                                   *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_uint64 (const uchar *dec,  /* Decimal Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_uint64_handler ErrorRout) /* Decimal error routine.  */
{

  uint64 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_UINT64:\n\n");

  blir_dtb_uint64 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST64_UBIN_FMT, locBinary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_int128 - Error handler for Decimal/Binary         *
 *                       conversion using a 128 bit signed binary     *
 *                       number.                                      *
 *                                                                    *
 *********************************************************************  */

#if 0
static int32
error_report_int128 (int *error_code,  /* Error Code.  */
                         const char *error_string,  /* Error String.  */
                         int128 * binary,  /* Binary Value.  */
                         int decimal_length_in_bytes,  /* Length of decimal value.  */
                         void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_INT128 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Error_report_uint128 - Error handler for Decimal/Binary        *
 *                       conversion using a 128 bit uint32 binary *
 *                       number.                                      *
 *                                                                    *
 *********************************************************************  */

static int32
error_report_uint128 (int *error_code,  /* Error Code.  */
                          const char *error_string,  /* Error String.  */
                          uint128 * binary,  /* Binary Value.  */
                          int decimal_length_in_bytes,  /* Length of decimal value.  */
                          void *decimal_value) /* Decimal Value.  */
{
  printf ("\terror_report_UINT128 called:  BLI%04dW %s\n",  * error_code, error_string);
  error_report_dumpVar ((int32) *binary, decimal_length_in_bytes, decimal_value);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_int128 - Carry out a test of the                 *
 *                        blir_btd_int128 routine (Signed 128 bit *
 *                        binary to packed decimal).  After the call, *
 *                        the input and output values will be         *
 *                        displayed.                                  *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_int128 (int128 binary,  /* Binary Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_int128_handler ErrorRout) /* Decimal error routine.  */
{

  int128 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_INT128:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_int128 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST128_BIN_FMT, binary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_int128 - Carry out a test of the                 *
 *                        blir_dtb_int128 routine (Packed decimal *
 *                        to Signed 128 bit binary).  After the call, *
 *                        the input and output values will be         *
 *                        displayed.                                  *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_int128 (const uchar *dec,  /* Decimal Value.  */
                          int decLength,  /* Decimal Length.  */
                          blir_int128_handler ErrorRout) /* Decimal error routine.  */
{

  int128 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_INT128:\n\n");

  blir_dtb_int128 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST128_BIN_FMT, locBinary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_btd_uint128 - Carry out a test of the                *
 *                        blir_btd_uint128 routine (Unsigned 128  *
 *                        bit binary to packed decimal).  After the   *
 *                        call, the input and output values will be   *
 *                        displayed.                                  *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_btd_uint128 (uint128 binary,  /* Binary Value.  */
                           int decLength,  /* Decimal Length.  */
                           blir_uint128_handler ErrorRout) /* Decimal error routine.  */
{

  uint128 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  printf ("\nblir_btd_UINT128:\n\n");

  locBinary = binary;
  locDecLen = decLength;

  blir_btd_uint128 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST128_UBIN_FMT, binary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_blir_dtb_uint128 - Carry out a test of the                *
 *                        blir_dtb_uint128 routine (Packed        *
 *                        decimal to Unsigned 128 bit binary).  After *
 *                        the call, the input and output values will  *
 *                        be displayed.                               *
 *                                                                    *
 *********************************************************************  */

static int32
test_blir_dtb_uint128 (const uchar *dec,  /* Decimal Value.  */
                           int decLength,  /* Decimal Length.  */
                           blir_uint128_handler ErrorRout) /* Decimal error routine.  */
{

  uint128 locBinary;  /* Local binary value.  */
  int locDecLen;  /* Local decimal length.  */
  uchar locDec[128];  /* Local decimal number.  */
  int i;  /* Index.  */

  locBinary = 0;
  locDecLen = decLength;
  memcpy (locDec, dec, decLength);

  printf ("\nblir_dtb_UINT128:\n\n");

  blir_dtb_uint128 ( & locBinary, locDecLen, locDec, ErrorRout);

  printf ("\tNum = ");
  printf (TEST128_UBIN_FMT, locBinary);
  printf ("\n\tDec = ");

  for (i = 0; i < decLength; i ++)
    printf ("%02X ", locDec[i]);
  printf ("\n");

  return 0;
}
#endif

/**********************************************************************
 *                                                                    *
 * Main - Program Mainline.  This routine will call the various test  *
 *        drivers with assorted values to carry out verifications.    *
 *                                                                    *
 *********************************************************************  */

int
main (void)
{

  /*--------------------------------------------------------------------*
   * Test test_blir_validate_packed                                     *
   *--------------------------------------------------------------------.  */

  test_blir_validate_packed (2, "\x12\x3d", 1);
  test_blir_validate_packed (2, "\x12\x34", 1);  /* Error.  */
  test_blir_validate_packed (2, "\x12\x3d", 0);  /* Error.  */
  test_blir_validate_packed (2, "\x12\x3f", 0);

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_int16                                       *
   *--------------------------------------------------------------------.  */

  test_blir_btd_int16 (-9, 1, error_report_int16);
  test_blir_btd_int16 (0, 1, error_report_int16);
  test_blir_btd_int16 (+9, 1, error_report_int16);

  test_blir_btd_int16 (32767, 3, error_report_int16);
  test_blir_btd_int16 (-32768, 3, error_report_int16);

  test_blir_btd_int16 (-32768, 2, error_report_int16);  /* Error.  */
  test_blir_btd_int16 (-32768, 2, (blir_int16_handler)NULL);  /* Error.  */

  test_blir_btd_int16 (0, 0, (blir_int16_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_int16                                       *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_int16 ((uchar *)"\x9D", 1, error_report_int16);
  test_blir_dtb_int16 ((uchar *)"\x0F", 1, error_report_int16);
  test_blir_dtb_int16 ((uchar *)"\x9C", 1, error_report_int16);

  test_blir_dtb_int16 ((uchar *)"\x32\x76\x7C", 3, error_report_int16);
  test_blir_dtb_int16 ((uchar *)"\x32\x76\x8D", 3, error_report_int16);

  test_blir_dtb_int16 ((uchar *)"\x32\x76\x8C", 3, error_report_int16);  /* Error.  */
  test_blir_dtb_int16 ((uchar *)"\x32\x76\x9D", 3, error_report_int16);  /* Error.  */

  test_blir_dtb_int16 ((uchar *)"\xAA", 1, error_report_int16);  /* Error.  */
  test_blir_dtb_int16 ((uchar *)"\x0C", 0, (blir_int16_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_uint16                                      *
   *--------------------------------------------------------------------.  */

  test_blir_btd_uint16 (0, 1, error_report_uint16);
  test_blir_btd_uint16 (9, 1, error_report_uint16);

  test_blir_btd_uint16 (65535, 16, error_report_uint16);

  test_blir_btd_uint16 (0, 0, (blir_uint16_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_uint16                                      *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_uint16 ((uchar *)"\x0F", 1, error_report_uint16);
  test_blir_dtb_uint16 ((uchar *)"\x9F", 1, error_report_uint16);

  test_blir_dtb_uint16 ((uchar *)"\x65\x53\x5F", 3, error_report_uint16);

  test_blir_dtb_uint16 ((uchar *)"\x65\x53\x6F", 3, error_report_uint16);  /* Error.  */

  test_blir_dtb_uint16 ((uchar *)"\xAA", 1, error_report_uint16);  /* Error.  */
  test_blir_dtb_uint16 ((uchar *)"\x0F", 0, (blir_uint16_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_int32                                       *
   *--------------------------------------------------------------------.  */

  test_blir_btd_int32 (-9, 1, error_report_int32);
  test_blir_btd_int32 (0, 1, error_report_int32);
  test_blir_btd_int32 (+9, 1, error_report_int32);

  test_blir_btd_int32 (2147483647, 12, error_report_int32);
  test_blir_btd_int32 (-2147483647 -1, 12, error_report_int32);

  test_blir_btd_int32 (-2147483647 -1, 2, error_report_int32);  /* Error.  */
  test_blir_btd_int32 (-2147483647 -1, 2, (blir_int32_handler)NULL);  /* Error.  */

  test_blir_btd_int32 (0, 0, (blir_int32_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_int32                                       *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_int32 ((uchar *)"\x9D", 1, error_report_int32);
  test_blir_dtb_int32 ((uchar *)"\x0F", 1, error_report_int32);
  test_blir_dtb_int32 ((uchar *)"\x9C", 1, error_report_int32);

  test_blir_dtb_int32 ((uchar *)"\x02\x14\x74\x83\x64\x7C", 6, error_report_int32);
  test_blir_dtb_int32 ((uchar *)"\x02\x14\x74\x83\x64\x8D", 6, error_report_int32);

  test_blir_dtb_int32 ((uchar *)"\x02\x14\x74\x83\x64\x8C", 6, error_report_int32);  /* Error.  */
  test_blir_dtb_int32 ((uchar *)"\x02\x14\x74\x83\x64\x9D", 6, error_report_int32);  /* Error.  */

  test_blir_dtb_int32 ((uchar *)"\xAA", 1, error_report_int32);  /* Error.  */
  test_blir_dtb_int32 ((uchar *)"\x0C", 0, (blir_int32_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_uint32                                      *
   *--------------------------------------------------------------------.  */

  test_blir_btd_uint32 (0, 1, error_report_uint32);
  test_blir_btd_uint32 (9, 1, error_report_uint32);

  test_blir_btd_uint32 (4294967295U, 12, error_report_uint32);

  test_blir_btd_uint32 (0, 0, (blir_uint32_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_uint32                                      *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_uint32 ((uchar *)"\x0F", 1, error_report_uint32);
  test_blir_dtb_uint32 ((uchar *)"\x9F", 1, error_report_uint32);

  test_blir_dtb_uint32 ((uchar *)"\x04\x29\x49\x67x29\x5F", 6, error_report_uint32);

  test_blir_dtb_uint32 ((uchar *)"\x04\x29\x49\x67x29\x6F", 6, error_report_uint32);  /* Error.  */

  test_blir_dtb_uint32 ((uchar *)"\xAA", 1, error_report_uint32);  /* Error.  */
  test_blir_dtb_uint32 ((uchar *)"\x0F", 0, (blir_uint32_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_int64                                       *
   *--------------------------------------------------------------------.  */

  test_blir_btd_int64 (-9, 1, error_report_int64);
  test_blir_btd_int64 (0, 1, error_report_int64);
  test_blir_btd_int64 (+9, 1, error_report_int64);

  test_blir_btd_int64 (TEST64_BIN_MAXPOS, TEST64_DEC_LENGTH, error_report_int64);
  test_blir_btd_int64 (TEST64_BIN_MAXNEG, TEST64_DEC_LENGTH, error_report_int64);

  test_blir_btd_int64 (TEST64_BIN_MAXNEG, 2, error_report_int64);  /* Error.  */
  test_blir_btd_int64 (TEST64_BIN_MAXNEG, 2, (blir_int64_handler)NULL);  /* Error.  */

  test_blir_btd_int64 (0, 0, (blir_int64_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_int64                                       *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_int64 ((uchar *)"\x9D", 1, error_report_int64);
  test_blir_dtb_int64 ((uchar *)"\x0F", 1, error_report_int64);
  test_blir_dtb_int64 ((uchar *)"\x9C", 1, error_report_int64);

  test_blir_dtb_int64 (TEST64_DEC_MAXPOS, TEST64_DEC_LENGTH, error_report_int64);
  test_blir_dtb_int64 (TEST64_DEC_MAXNEG, TEST64_DEC_LENGTH, error_report_int64);

  test_blir_dtb_int64 (TEST64_DEC_MAXPOS_P1, TEST64_DEC_LENGTH, error_report_int64);  /* Error.  */
  test_blir_dtb_int64 (TEST64_DEC_MAXNEG_P1, TEST64_DEC_LENGTH, error_report_int64);  /* Error.  */

  test_blir_dtb_int64 ((uchar *)"\xAA", 1, error_report_int64);  /* Error.  */
  test_blir_dtb_int64 ((uchar *)"\x0C", 0, (blir_int64_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_uint64                                      *
   *--------------------------------------------------------------------.  */

  test_blir_btd_uint64 (0, 1, error_report_uint64);
  test_blir_btd_uint64 (9, 1, error_report_uint64);

  test_blir_btd_uint64 (TEST64_UBIN_MAX -1, TEST64_UDEC_LENGTH, error_report_uint64);

  test_blir_btd_uint64 (TEST64_UBIN_MAX, TEST64_UDEC_LENGTH, error_report_uint64);

  test_blir_btd_uint64 (0, 0, (blir_uint64_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_uint64                                      *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_uint64 ((uchar *)"\x0F", 1, error_report_uint64);
  test_blir_dtb_uint64 ((uchar *)"\x9F", 1, error_report_uint64);

  test_blir_dtb_uint64 (TEST64_UDEC_MAX, TEST64_UDEC_LENGTH, error_report_uint64);

  test_blir_dtb_uint64 (TEST64_UDEC_MAX_P1, TEST64_UDEC_LENGTH, error_report_uint64);  /* Error.  */

  test_blir_dtb_uint64 ((uchar *)"\xAA", 1, error_report_uint64);  /* Error.  */
  test_blir_dtb_uint64 ((uchar *)"\x0F", 0, (blir_uint64_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_int128                                      *
   *--------------------------------------------------------------------.  */

#if 0
  test_blir_btd_int128 (-9, 1, error_report_int128);
  test_blir_btd_int128 (0, 1, error_report_int128);
  test_blir_btd_int128 (+9, 1, error_report_int128);

  test_blir_btd_int128 (TEST128_BIN_MAXPOS, TEST128_DEC_LENGTH, error_report_int128);
  test_blir_btd_int128 (TEST128_BIN_MAXNEG, TEST128_DEC_LENGTH, error_report_int128);

  test_blir_btd_int128 (TEST128_BIN_MAXNEG, 2, error_report_int128);  /* Error.  */
  test_blir_btd_int128 (TEST128_BIN_MAXNEG, 2, (blir_int128_handler)NULL);  /* Error.  */

  test_blir_btd_int128 (0, 0, (blir_int128_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_int128                                      *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_int128 ((uchar *)"\x9D", 1, error_report_int128);
  test_blir_dtb_int128 ((uchar *)"\x0F", 1, error_report_int128);
  test_blir_dtb_int128 ((uchar *)"\x9C", 1, error_report_int128);

  test_blir_dtb_int128 (TEST128_DEC_MAXPOS, TEST128_DEC_LENGTH, error_report_int128);
  test_blir_dtb_int128 (TEST128_DEC_MAXNEG, TEST128_DEC_LENGTH, error_report_int128);

  test_blir_dtb_int128 (TEST128_DEC_MAXPOS_P1, TEST128_DEC_LENGTH, error_report_int128);  /* Error.  */
  test_blir_dtb_int128 (TEST128_DEC_MAXNEG_P1, TEST128_DEC_LENGTH, error_report_int128);  /* Error.  */

  test_blir_dtb_int128 ((uchar *)"\xAA", 1, error_report_int128);  /* Error.  */
  test_blir_dtb_int128 ((uchar *)"\x0C", 0, (blir_int128_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_btd_uint128                                     *
   *--------------------------------------------------------------------.  */

  test_blir_btd_uint128 (0, 1, error_report_uint128);
  test_blir_btd_uint128 (9, 1, error_report_uint128);

  test_blir_btd_uint128 (TEST128_UBIN_MAX -1, TEST128_UDEC_LENGTH, error_report_uint128);

  test_blir_btd_uint128 (TEST128_UBIN_MAX, TEST128_UDEC_LENGTH, error_report_uint128);

  test_blir_btd_uint128 (0, 0, (blir_uint128_handler)NULL);  /* Error.  */

  /*--------------------------------------------------------------------*
   * Test test_blir_dtb_uint128                                     *
   *--------------------------------------------------------------------.  */

  test_blir_dtb_uint128 ((uchar *)"\x0F", 1, error_report_uint128);
  test_blir_dtb_uint128 ((uchar *)"\x9F", 1, error_report_uint128);

  test_blir_dtb_uint128 (TEST128_UDEC_MAX, TEST128_UDEC_LENGTH, error_report_uint128);

  test_blir_dtb_uint128 (TEST128_UDEC_MAX_P1, TEST128_UDEC_LENGTH, error_report_uint128);  /* Error.  */

  test_blir_dtb_uint128 ((uchar *)"\xAA", 1, error_report_uint128);  /* Error.  */
  test_blir_dtb_uint128 ((uchar *)"\x0F", 0, (blir_uint128_handler)NULL);  /* Error.  */
#endif
  return 0;
}
