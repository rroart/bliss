
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
 *                 Routines T. Seward       29/Jul/2000Repaird error  *
 *                 in testing unsigned decimal values.  A statement   *
 *                 was just returning for unsigned without validating *
 *                 it.  T. Seward       29/Jul/2000Adjusted to loop   *
 *                 so it only checked for two digit values in the     *
 *                 first N-1 bytes.  For the Nth byte, it checked for *
 *                 digit and sign.                                    *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 *                       Packed-Decimal Format                        *
 *                       -------------- ------                        *
 *                                                                    *
 * A typical Zoned-Decimal representation of an EBCDIC '12345'        *
 * Hexadecimal would be:                                              *
 *                                                                    *
 * +----+----+----+----+----+                                         *
 * | F1 | F2 | F3 | F4 | F5 |                                         *
 * +----+----+----+----+----+                                         *
 *                                                                    *
 * The same number in Packed-Decimal notation in Hexadecimal would    *
 * be:                                                                *
 *                                                                    *
 * +----+----+----+                                                   *
 * | 12 | 34 | 5F |                                                   *
 * +----+----+----+                                                   *
 *                                                                    *
 * Note that the for the first 4 bytes, the zoned portion (first 4    *
 * bits of each byte) has been stripped off and the numbers packed    *
 * together two digits to the byte.  The last number had its zoned    *
 * and decimal portions swapped and stored in the byte.  This         *
 * swapping of the zoned and decimal portion allows the sign (set by  *
 * an over-punch) to become the sign of a packed decimal number.  If  *
 * this had been a -12345, represented by 0xF1F2F3F4D5, the packed    *
 * value would be 0x12345D.  The sign values are therefore the same   *
 * as for Zoned-Decimals:                                             *
 *                                                                    *
 * value  Sign                                                        *
 * -----  ----                                                        *
 * 0xC     +                                                          *
 * 0xD     -                                                          *
 * 0xF     +                                                          *
 *                                                                    *
 * The Packed-Decimal format may be 1 to 16 bytes in length,          *
 * representing 1 to 31 decimal digits.  Each digit occupies 4 bits,  *
 * each having the binary value of 0 to 9.  The right most nibble     *
 * (last 4 bits) of a Packed-Decimal number represents the sign.  A   *
 * Packed Decimal number always starts on a byte boundary and         *
 * occupies a finite number of bytes.  This gives an even number of   *
 * nibbles, where the right most nibble is the sign.  Since it starts *
 * with an even number of nibbles and losses one nibble for a sign,   *
 * the maximum number of digits in any packed decimal number is odd.  *
 *                                                                    *
 * A Packed-Decimal number is basically a Zoned-Decimal number with   *
 * the Zoned portion (zone punch) removed.  On an IBM, the conversion *
 * of a Zoned-Decimal number to a Packed-Decimal number is done with  *
 * the machine language instruction PACK.  The PACK instruction       *
 * starts by swapping the zone and sign portion of the right most     *
 * digit and storing it in the right most byte of the packed number   *
 * (0xF5 -> 0x5F & 0xD8 -> 0x8D).  Starting with the next digit to    *
 * the left, the instruction basically goes from right to left,       *
 * packing the digits by stripping off the zoned portion.  If the     *
 * packed area has room for more digits than the source field         *
 * provides, the remaining nibbles to the left are filled with zeros. *
 *                                                                    *
 * An interesting note on packed numbers.  Typically, a positive      *
 * number starts off with a sign nibble of 0xF.  The output of packed *
 * decimal machine instruction (other than PACK) will use the 0xC for *
 * positive numbers.                                                  *
 *                                                                    *
 *********************************************************************  */

#include "stddef.h"
#include "blir_globals.h"
#include "blir_decbin.h"

/**********************************************************************
 *                                                                    *
 * Cobr_validate_packed - Validate that the value is a legitimate     *
 *                        packed decimal number                       *
 *                                                                    *
 * This routine will verify the following:                            *
 *                                                                    *
 * *  If the length is greater than 1, then all leading bytes, prior  *
 *    to the last byte, consists solely of nibbles containing the     *
 *    values 0 through 9.  The first nibble of a byte is defined as   *
 *    the upper 4 bits (most significant), the second nibble of a     *
 *    byte is defined as the lower 4 bits (least significant).        *
 *    Hence, 0x45 is a valid while 0xF0 is invalid due to the upper   *
 *    nibble not being in the range 0 through 9.                      *
 *                                                                    *
 * *  The last or only byte will be tested for a mixture of a digit   *
 *    and sign.  The first nibble is defined as the upper 4 bits      *
 *    (most significant) and must be in the range 0 through 9.  The   *
 *    second nibble is defined as the lower 4 bits (least             *
 *    significant) and must contain the one of the values:  0x0C,     *
 *    0xD, 0x0F.                                                      *
 *                                                                    *
 *********************************************************************  */

int
blir_validate_packed (int length,  /* Decimal value length.  */
                      void *value,  /* Decimal value.  */
                      int signed_flag) /* Signed decimal flag.  */
{
  uchar *decNum;  /* Decimal number pointer.  */
  int digit;  /* Digit.  */
  int i;  /* Index.  */

  /*--------------------------------------------------------------------*
   * Check for errors too egregious to continue                         *
   *--------------------------------------------------------------------.  */

  if (length <= 0 || value == NULL)
    return 1;

  /*--------------------------------------------------------------------*
   * Check all but the last byte for only having decimal digits in the  *
   * range 0 through 9                                                  *
   *--------------------------------------------------------------------.  */

  decNum = (uchar *)value;

  for (i = 0; i < (length -1); i ++)
    {
      digit = decNum[i] >> 4;
      if (digit > 9)
        return 1;
      digit = decNum[i] & 0x0F;
      if (digit > 9)
        return 1;
    }


  /*--------------------------------------------------------------------*
   * Check the last byte based on the signed flag Check the last byte   *
   * for a decimal digit in the upper 4 bits and a sign in the lower 4  *
   * bits.                                                              *
   *--------------------------------------------------------------------.  */

  digit = decNum[length -1] >> 4;
  if (digit > 9)
    return 1;
  digit = decNum[length -1] & 0x0F;
  if (signed_flag)
    {
      if (digit == 0x0C || digit == 0x0D || digit == 0x0F)
        return 0;
    }
  else if (digit == 0x0F)return 0;

  return 1;
}
