/*                                                                    *
 *                          W A R N I N G !                           *
 *                          - - - - - - - -                           *
 *                                                                    *
 * At the time that I coded this file, I did not know what the        *
 * int128 (int64) constant suffix would be.  I used LL for    *
 * int128 signed binary value.  These suffixes will need to be    *
 * changed coorisponding to the correct values.  Look for the         *
 * following line:                                                    *
 *                                                                    *
 * #define int_MAXNEG_BIN                                         *
 * (-170141183460469231731687303715884105727LL-1LL)                   *
 *                                                                    *
 * and change the two occurances of "LL" to the correct letter.  For  *
 * example, if the letter is "X", then you would code:                *
 *                                                                    *
 * #define int_MAXNEG_BIN                                         *
 * (-170141183460469231731687303715884105727X-1X)                     *
 *                                                                    *
 * After making this change, delete this warning from the file.       *
 *  */


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
 * Mail:   Theodore J. Seward, Jr.                                    *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title:  Conversion routines for BLISS Binary / Decimal data        *
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
 *                 Packed Decimal to Signed binary                    *
 *                 Packed Decimal to Unsigned binary                  *
 *                                                                    *
 *                 Packed Decimal from Signed binary                  *
 *                 Packed Decimal form Unsigned binary                *
 *                                                                    *
 * Suggestions:    These routines can be optimized by replacing the   *
 *                 indexing with the user of an incremented /         *
 *                 decremented pointer declared as a register type.   *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000     Initial Release  *
 *                 of Routines                                        *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 *      Define the following prior to the inclusion of this file      *
 *                                                                    *
 * int_WIDTH       This is the width in bytes of the integer type *
 *                 in question.  A typical value would be:            *
 *                                                                    *
 *                 #define int_WIDTH 4                            *
 *                                                                    *
 * int_BTD_NAME    This is the name to generate for the signed    *
 *                 binary to decimal routine.  A typical value would  *
 *                 be:                                                *
 *                                                                    *
 *                 #define int_BTD_NAME blir_btd_int32        *
 *                                                                    *
 * int_DTB_NAME    This is the name to generate for the signed    *
 *                 decimal to binary routine.  A typical value would  *
 *                 be:                                                *
 *                                                                    *
 *                 #define int_DTB_NAME blir_dtb_int32        *
 *                                                                    *
 * int_HANDLER     This is the name of the typedef for the error  *
 *                 handler to call.  A typical value would be:        *
 *                                                                    *
 *                 #define int_HANDLER blir_int16_handler     *
 *                                                                    *
 * int_TYPE        This is the data type for the signed routines. *
 *                 Typical values would be:                           *
 *                                                                    *
 *                 #define int_TYPE int32                     *
 *                 #define int_TYPE int32                     *
 *                                                                    *
 * uint_BTD_NAME   This is the name to generate for the unsigned  *
 *                 binary to decimal routine.  A typical value would  *
 *                 be:                                                *
 *                                                                    *
 *                 #define uint_BTD_NAME blir_btd_uint32      *
 *                                                                    *
 * uint_DTB_NAME   This is the name to generate for the unsigned  *
 *                 decimal to binary routine.  A typical value would  *
 *                 be:                                                *
 *                                                                    *
 *                 #define uint_DTB_NAME blir_dtb_uint32      *
 *                                                                    *
 * uint_HANDLER    This is the name of the typedef for the error  *
 *                 handler to call.  A typical value would be:        *
 *                                                                    *
 *                 #define uint_HANDLER blir_uint16_handler   *
 *                                                                    *
 * uint_TYPE       This is the data type for the unsigned         *
 *                 routines.  Typical values would be:                *
 *                                                                    *
 *                 #define uint_TYPE uint32                   *
 *                 #define uint_TYPE uint32                   *
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
 *            Various constants based on numeric precision            *
 *                                                                    *
 * There are four sets of constants based on the number of bytes in   *
 * the integer: 2, 4, 8, or 16.  The symbol int_WIDTH, which is   *
 * set to 2, 4, 8, or 16 will determine which set of constants are    *
 * employed.                                                          *
 *                                                                    *
 *********************************************************************  */


/*--------------------------------------------------------------------*
 *          Constants for 16 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#if int_WIDTH == 2

#define int_MAXBLI_CHAR     (5)     /* Maximum character positions for int16 */
#define int_MAXPOS_BLI_CHAR "32767" /* Maximum positive character value */
#define int_MAXNEG_BLI_CHAR "32768" /* Maximum negative character value */
#define int_MAXNEG_BIN  -32768  /* Maximum negative binary value */
#define int_MAXNEG_PACK "\x32\x76\x8D"  /* Maximum negative packed value */
#define int_MAXNEG_PACKL (3)    /* Maximum negative packed value length */

#define uint_MAXBLI_CHAR     (5)    /* Maximum character positions for uint16 */
#define uint_MAXPOS_BLI_CHAR "65535"        /* Maximum character value */

/*--------------------------------------------------------------------*
 *          Constants for 32 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#elif int_WIDTH == 4

#define int_MAXBLI_CHAR     (10)    /* Maximum character positions for int32 */
#define int_MAXPOS_BLI_CHAR "2147483647"    /* Maximum positive character value */
#define int_MAXNEG_BLI_CHAR "2147483648"    /* Maximum negative character value */
#define int_MAXNEG_BIN  (-2147483647-1)         /* Maximum negative binary value */
#define int_MAXNEG_PACK "\x02\x14\x74\x83\x64\x8D"      /* Maximum negative packed value */
#define int_MAXNEG_PACKL (6)    /* Maximum negative packed value length */

#define uint_MAXBLI_CHAR     (10)   /* Maximum character positions for uint32 */
#define uint_MAXPOS_BLI_CHAR "4294967295"   /* Maximum character value */

/*--------------------------------------------------------------------*
 *          Constants for 64 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#elif int_WIDTH == 8

#define int_MAXBLI_CHAR     (19)    /* Maximum character positions for int64 */
#define int_MAXPOS_BLI_CHAR "9223372036854775807"   /* Maximum positive character value */
#define int_MAXNEG_BLI_CHAR "9223372036854775808"   /* Maximum negative character value */
#define int_MAXNEG_BIN  (-9223372036854775807LL-1LL) /* Maximum negative binary value */
#define int_MAXNEG_PACK "\x92\x23\x37\x20\x36\x85\x47\x75\x80\x8D"      /* Maximum negative packed value */
#define int_MAXNEG_PACKL (10)   /* Maximum negative packed value length */

#define uint_MAXBLI_CHAR     (20)   /* Maximum character positions for uint64 */
#define uint_MAXPOS_BLI_CHAR "18446744073709551615"         /* Maximum character value */

/*--------------------------------------------------------------------*
 *         Constants for 128 bit binary / decimal conversion          *
 *--------------------------------------------------------------------.  */

#elif int_WIDTH == 16
#error no 16 digit numbers yet
#define int_MAXBLI_CHAR     (39)    /* Maximum character positions for int128 */
#define int_MAXPOS_BLI_CHAR "170141183460469231731687303715884105727"       /* Maximum positive character value */
#define int_MAXNEG_BLI_CHAR "170141183460469231731687303715884105728"       /* Maximum negative character value */
#define int_MAXNEG_BIN  (-170141183460469231731687303715884105727LL-1LL)        /* Maximum negative binary value */
#define int_MAXNEG_PACK "\x17\x01\x41\x18\x34\x60\x46\x92\x31\x73\x16\x87\x30\x37\x15\x88\x41\x05\x72\x8D"      /* Maximum negative packed value */
#define int_MAXNEG_PACKL (20)   /* Maximum negative packed value length */

#define uint_MAXBLI_CHAR     (39)   /* Maximum character positions for uint128 */
#define uint_MAXPOS_BLI_CHAR "340282366920938463463374607431768211455"              /* Maximum character value */
#else
#error int_WIDTH value incorrect

#endif


/**********************************************************************
 *                                                                    *
 * int_Error_Handler - Internal Error Effector for Signed         *
 *                    Integer conversion.                             *
 *                                                                    *
 * This routine will be passed all the arguments that an error        *
 * handler will require and the address of the error handler.  If the *
 * pointer to the handler is NULL, then blir_error_termination will   *
 * be called.  If it is not NULL, the error handler will be called    *
 * and based on its return code, the BLISS  error termination routine *
 * will be called.                                                    *
 *                                                                    *
 *********************************************************************  */

static void
int_Error_Handler (enum blir_error_codes error_code,  /* Error Code.  */
                       const char *error_string,  /* Error String.  */
                       int_TYPE * binary,  /* Binary Value.  */
                       int decimal_length_in_bytes,  /* Length of decimal value.  */
                       void *decimal_value,  /* Decimal Value.  */
                       int_HANDLER ErrorRout) /* Decimal error routine.  */
{
  int int_error_code;  /* Integer error code.  */
  int retCode;  /* Return Code.  */

  if (ErrorRout == (int_HANDLER)NULL)
    {
      blir_error_termination ((int)error_code, error_string);
      return;
    }

  int_error_code = (int)error_code;
  retCode = ( * ErrorRout)( & int_error_code,
                          error_string,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value);

  if (retCode)
    blir_error_termination ((int)error_code, error_string);

}

/**********************************************************************
 *                                                                    *
 * int_BTD_NAME - Convert a signed binary to a packed decimal     *
 *                                                                    *
 * This routine will convert a signed binary number to a packed       *
 * decimal number.  An error handler will be called for various types *
 * of errors:                                                         *
 *                                                                    *
 * *  Arguments too invalid to proceed:  Null Pointers or impossible  *
 *    lengths.                                                        *
 *                                                                    *
 * *  Overflow during conversion, i.e. the value exceeds the limit of *
 *    an Signed  Integer number                                       *
 *                                                                    *
 * This routine employs several goto statements to jump to error      *
 * logic.  While this may seem objectionable to some, it does reduce  *
 * the vertical space of the routine.  If the error calls were added  *
 * to each place the gotos exist, the predominate instruction path    *
 * would be obscured.                                                 *
 *                                                                    *
 *********************************************************************  */

void
int_BTD_NAME (int_TYPE * binary,  /* Binary value.  */
                  int decimal_length_in_bytes,  /* Length of decimal area.  */
                  void *decimal_value,  /* Decimal value.  */
                  int_HANDLER ErrorRout) /* Decimal error routine.  */
{
  int_TYPE numBin;  /* Binary Number.  */
  uchar *numDec;  /* Decimal Number.  */
  uchar sign;  /* Packed Sign.  */
  int i, j;  /* Index.  */
  int digit0;  /* High order digit of byte.  */
  int digit1;  /* Low order digit of byte.  */
  const uchar *maxNeg_Pack = (uchar *)int_MAXNEG_PACK;  /* Maximum Packed negative.  */

  /*--------------------------------------------------------------------*
   * Check for argument errors that are so egregious that execution is  *
   * impossible                                                         *
   *--------------------------------------------------------------------.  */

  if (binary == NULL ||
      decimal_length_in_bytes <= 0 ||
      decimal_value == NULL)goto Error_Bad_Arguments;

  /*--------------------------------------------------------------------*
   * Initialize variables                                               *
   *--------------------------------------------------------------------.  */

  numBin =  * binary;  /* Get local copy.  */
  numDec = (uchar *)decimal_value;

  for (i = 0; i < decimal_length_in_bytes; i ++)
    numDec[i] = '\0';

  /*--------------------------------------------------------------------*
   * Check for negative numbers and the maximum negative number for     *
   * Signed  Integer                                                    *
   *--------------------------------------------------------------------.  */

  if (numBin >= 0)
    sign = '\x0C';
  else
    {
      if (numBin == int_MAXNEG_BIN) /* Too small to negate.  */
        {
          if (decimal_length_in_bytes < int_MAXNEG_PACKL)goto Error_Overflow;
          for (i = 0, j = decimal_length_in_bytes - int_MAXNEG_PACKL;
              i < int_MAXNEG_PACKL;
              i ++, j ++)
            numDec[j] = maxNeg_Pack[i];
          return;
        }

      sign = '\x0D';
      numBin = (int_TYPE) - numBin;
    }

  /*--------------------------------------------------------------------*
   * Store the right most digit of the number and the sign into the     *
   * packed decimal number.                                             *
   *--------------------------------------------------------------------.  */

  i = decimal_length_in_bytes - 1;

  digit0 = numBin % 10;
  numDec[i --] = (uchar)((digit0 << 4) | sign);
  numBin /= (int_TYPE)10;

  /*--------------------------------------------------------------------*
   * Store the remaining bytes of the decimal number (right to left)    *
   * until the work area is zero.  Note that it is possible that a      *
   * number from -9 through +9 will have never enter this logic.  The   *
   * decimal value was initialized to all zeros, so leading zeros are   *
   * already in place.                                                  *
   *--------------------------------------------------------------------.  */

  while (numBin != 0)
    {
      if (i < 0)goto Error_Overflow;

      digit1 = numBin % 10;
      numBin /= (int_TYPE)10;
      digit0 = numBin % 10;
      numBin /= (int_TYPE)10;
      numDec[i --] = (uchar)((digit0 << 4) | digit1);

    }
  return;  /* Normal Return *  */

  /*--------------------------------------------------------------------*
   * An argument error too egregious to continue has occurred           *
   *--------------------------------------------------------------------.  */

 Error_Bad_Arguments :

  int_Error_Handler (BLIR_ERROR_CODE_INVARGS,
                         BLIR_ERROR_MSG_INVARGS,
                         binary,
                         decimal_length_in_bytes,
                         decimal_value,
                         ErrorRout);
  return;  /* Bad Argument Return.  */

  /*--------------------------------------------------------------------*
   * An overflow would have occurred                                    *
   *--------------------------------------------------------------------.  */

 Error_Overflow :

  int_Error_Handler (BLIR_ERROR_CODE_OVERFLOW,
                     BLIR_ERROR_MSG_OVERFLOW,
                     binary,
                     decimal_length_in_bytes,
                     decimal_value,
                     ErrorRout);
  return;  /* Overflow Return.  */
}

/**********************************************************************
 *                                                                    *
 * int_DTB_NAME - Convert a packed decimal to a signed binary     *
 *                                                                    *
 * This routine will convert a packed decimal number to a signed      *
 * binary number.  An error handler will be called for various types  *
 * of errors:                                                         *
 *                                                                    *
 * *  Arguments too invalid to proceed:  Null Pointers or impossible  *
 *    lengths.                                                        *
 *                                                                    *
 * *  Invalid decimal format                                          *
 *                                                                    *
 * *  Overflow during conversion, i.e. the value exceeds the limit of *
 *    an Signed  Integer number                                       *
 *                                                                    *
 * This routine employs several goto statements to jump to error      *
 * logic.  While this may seem objectionable to some, it does reduce  *
 * the vertical space of the routine.  If the error calls were added  *
 * to each place the gotos exist, the predominate instruction path    *
 * would be obscured.                                                 *
 *                                                                    *
 *********************************************************************  */

void
int_DTB_NAME (int_TYPE * binary,  /* Binary value.  */
              int decimal_length_in_bytes,  /* Length of decimal value.  */
              void *decimal_value,  /* Decimal value.  */
              int_HANDLER ErrorRout) /* Decimal error routine.  */
{

  const char *maxPos = int_MAXPOS_BLI_CHAR;  /* Maximum Positive.  */
  const char *maxNeg = int_MAXNEG_BLI_CHAR;  /* Maximum Negative.  */
  int_TYPE numBin;  /* Binary Number.  */
  uchar *numDec;  /* Decimal Number.  */
  const char *str1;  /* String 1 pointer.  */
  const char *str2;  /* String 2 pointer.  */
  int digit;  /* Digit.  */
  int sign;  /* Packed Sign.  */
  int i, n1, n2;  /* Index.  */
  int sigFlag;  /* Significant digit detected.  */
  uchar numImage[int_MAXBLI_CHAR +1];  /* Number image.  */
  int numImageIdx;  /* Number image index.  */

  /*--------------------------------------------------------------------*
   * Check for argument errors that are so egregious that execution is  *
   * impossible                                                         *
   *--------------------------------------------------------------------.  */

  if (binary == NULL ||
      decimal_length_in_bytes <= 0 ||
      decimal_value == NULL)goto Error_Bad_Arguments;

  /*--------------------------------------------------------------------*
   * Process all but the last byte of the decimal number.  These bytes  *
   * contain decimal digits in both the upper and lower nibble.         *
   *--------------------------------------------------------------------.  */

  numDec = (uchar *)decimal_value;
  sigFlag = 0;
  numImageIdx = 0;
  for (i = 0; i < (decimal_length_in_bytes - 1); i ++)
    {
      digit = numDec[i] >> 4;
      if (digit > 0)
        sigFlag = 1;
      if (sigFlag)
        {
          if (numImageIdx >= int_MAXBLI_CHAR)goto Error_Overflow;
          if (digit > 9)goto Error_Bad_Decimal;
          numImage[numImageIdx ++] = (char)('0' + digit);
        }
      digit = numDec[i] & 0x0F;
      if (digit > 0)
        sigFlag = 1;
      if (sigFlag)
        {
          if (numImageIdx >= int_MAXBLI_CHAR)goto Error_Overflow;
          if (digit > 9)goto Error_Bad_Decimal;
          numImage[numImageIdx ++] = (char)('0' + digit);
        }
    }

  /*--------------------------------------------------------------------*
   * Process the last byte of the decimal number.  This byte contains a *
   * digit in the upper nibble and a sign code in the lower nibble.     *
   *--------------------------------------------------------------------.  */

  digit = numDec[decimal_length_in_bytes - 1] >> 4;

  if (numImageIdx >= int_MAXBLI_CHAR)goto Error_Overflow;

  if (digit > 9)goto Error_Bad_Decimal;

  numImage[numImageIdx ++] = (char)('0' + digit);
  numImage[numImageIdx] = '\0';

  switch (numDec[decimal_length_in_bytes - 1] & 0x0F)
    {
    case 0x0D :
      sign = -1;  /* Negative.  */
      break;
    case 0x0C :
    case 0x0F :
      sign = +1;  /* Positive.  */
      break;
    default :
      goto Error_Bad_Decimal;
    }

  /*--------------------------------------------------------------------*
   * While the number does not exceed the maximum number of digits, the *
   * high value is less than all 9's.  Verify that the exact value does *
   * not exceed the maximum width of an Signed  Integer value.          *
   *--------------------------------------------------------------------.  */

  if (numImageIdx == int_MAXBLI_CHAR)
    {
      str1 = (char *)numImage;
      if (sign >= 0)
        str2 = maxPos;
      else
        str2 = maxNeg;
      for (; ( * str1 ==  * str2) && ( * str1 != '\0'); str1 ++, str2 ++);
      n1 =  * str1;  /* Make sure signed value.  */
      n2 =  * str2;  /* Make sure signed value.  */
      if ((n1 - n2) > 0)goto Error_Overflow;
    }

  /*--------------------------------------------------------------------*
   * Everything is okay.  Convert the collected components to an Signed *
   * Integer number and return to caller.                               *
   *--------------------------------------------------------------------.  */

  numBin = 0;
  for (i = 0; i < numImageIdx; i ++)
    numBin = (int_TYPE)(numBin * 10 + numImage[i] - '0');
  if (sign < 0)
    numBin = (int_TYPE) - numBin;

  * binary = numBin;
  return;  /* Normal Return *  */

  /*--------------------------------------------------------------------*
   * An argument error too egregious to continue has occurred           *
   *--------------------------------------------------------------------.  */

 Error_Bad_Arguments :

  int_Error_Handler (BLIR_ERROR_CODE_INVARGS,
                         BLIR_ERROR_MSG_INVARGS,
                         binary,
                         decimal_length_in_bytes,
                         decimal_value,
                         ErrorRout);
  return;  /* Bad Argument Return.  */

  /*--------------------------------------------------------------------*
   * The decimal number is invalid                                      *
   *--------------------------------------------------------------------.  */

 Error_Bad_Decimal :

  int_Error_Handler (BLIR_ERROR_CODE_INVDEC,
                         BLIR_ERROR_MSG_INVDEC,
                         binary,
                         decimal_length_in_bytes,
                         decimal_value,
                         ErrorRout);
  return;  /* Bad Decimal Return.  */

  /*--------------------------------------------------------------------*
   * An overflow would have occurred                                    *
   *--------------------------------------------------------------------.  */

 Error_Overflow :

  int_Error_Handler (BLIR_ERROR_CODE_OVERFLOW,
                         BLIR_ERROR_MSG_OVERFLOW,
                         binary,
                         decimal_length_in_bytes,
                         decimal_value,
                         ErrorRout);
  return;  /* Overflow Return.  */
}

/**********************************************************************
 *                                                                    *
 * uint_Error_Handler - Internal Error Effector for Unsigned      *
 *                    Integer conversion.                             *
 *                                                                    *
 * This routine will be passed all the arguments that an error        *
 * handler will require and the address of the error handler.  If the *
 * pointer to the handler is NULL, then blir_error_termination will   *
 * be called.  If it is not NULL, the error handler will be called    *
 * and based on its return code, the BLISS  error termination routine *
 * will be called.                                                    *
 *                                                                    *
 *********************************************************************  */

static void
uint_Error_Handler (enum blir_error_codes error_code,  /* Error Code.  */
                        const char *error_string,  /* Error String.  */

                        uint_TYPE * binary,  /* Binary Value.  */
                        int decimal_length_in_bytes,  /* Length of decimal value.  */
                        void *decimal_value,  /* Decimal Value.  */
                        uint_HANDLER ErrorRout) /* Decimal error routine.  */
{
  int int_error_code;  /* Integer error code.  */
  int retCode;  /* Return Code.  */

  if (ErrorRout == (uint_HANDLER)NULL)
    {
      blir_error_termination ((int)error_code, error_string);
      return;
    }

  int_error_code = (int)error_code;
  retCode = ( * ErrorRout)( & int_error_code,
                          error_string,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value);

  if (retCode)
    blir_error_termination ((int)error_code, error_string);

}

/**********************************************************************
 *                                                                    *
 * uint_BTD_NAME - Convert an unsigned binary to a packed decimal *
 *                                                                    *
 * This routine will convert an unsigned binary number to a packed    *
 * decimal number.  An error handler will be called for various types *
 * of errors:                                                         *
 *                                                                    *
 * *  Arguments too invalid to proceed:  Null Pointers or impossible  *
 *    lengths.                                                        *
 *                                                                    *
 * *  Overflow during conversion, i.e. the value exceeds the limit of *
 *    an Unsigned  Integer number                                     *
 *                                                                    *
 * This routine employs several goto statements to jump to error      *
 * logic.  While this may seem objectionable to some, it does reduce  *
 * the vertical space of the routine.  If the error calls were added  *
 * to each place the gotos exist, the predominate instruction path    *
 * would be obscured.                                                 *
 *                                                                    *
 *********************************************************************  */

void
uint_BTD_NAME (uint_TYPE * binary,  /* Binary value.  */
                   int decimal_length_in_bytes,  /* Length of decimal area.  */
                   void *decimal_value,  /* Decimal value.  */
                   uint_HANDLER ErrorRout) /* Decimal error routine.  */
{
  uint_TYPE numBin;  /* Binary Number.  */
  uchar *numDec;  /* Decimal Number.  */
  int digit0;  /* High order digit of byte.  */
  int digit1;  /* Low order digit of byte.  */
  int i;  /* Index.  */

  /*--------------------------------------------------------------------*
   * Check for argument errors that are so egregious that execution is  *
   * impossible                                                         *
   *--------------------------------------------------------------------.  */

  if (binary == NULL ||
      decimal_length_in_bytes <= 0 ||
      decimal_value == NULL)goto Error_Bad_Arguments;

  /*--------------------------------------------------------------------*
   * Initialize variables                                               *
   *--------------------------------------------------------------------.  */

  numBin =  * binary;  /* Get local copy.  */
  numDec = (uchar *)decimal_value;

  for (i = 0; i < decimal_length_in_bytes; i ++)
    numDec[i] = '\0';

  /*--------------------------------------------------------------------*
   * Store the right most digit of the number and the sign into the     *
   * packed decimal number.                                             *
   *--------------------------------------------------------------------.  */

  i = decimal_length_in_bytes - 1;

  digit0 = numBin % 10;
  numDec[i --] = (uchar)((digit0 << 4) | 0x0F);  /* Use 0x0F as sign.  */
  numBin /= (uint_TYPE)10;

  /*--------------------------------------------------------------------*
   * Store the remaining bytes of the decimal number (right to left)    *
   * until the work area is zero.  Note that it is possible that a      *
   * number from -9 through +9 will have never enter this logic.  The   *
   * decimal value was initialized to all zeros, so leading zeros are   *
   * already in place.                                                  *
   *--------------------------------------------------------------------.  */

  while (numBin != 0)
    {
      if (i < 0)goto Error_Overflow;

      digit1 = numBin % 10;
      numBin /= (uint_TYPE)10;
      digit0 = numBin % 10;
      numBin /= (uint_TYPE)10;
      numDec[i --] = (uchar)((digit0 << 4) | digit1);

    }
  return;  /* Normal Return *  */

  /*--------------------------------------------------------------------*
   * An argument error too egregious to continue has occurred           *
   *--------------------------------------------------------------------.  */

 Error_Bad_Arguments :

  uint_Error_Handler (BLIR_ERROR_CODE_INVARGS,
                          BLIR_ERROR_MSG_INVARGS,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value,
                          ErrorRout);
  return;  /* Bad Argument Return.  */

  /*--------------------------------------------------------------------*
   * An overflow would have occurred                                    *
   *--------------------------------------------------------------------.  */

 Error_Overflow :

  uint_Error_Handler (BLIR_ERROR_CODE_OVERFLOW,
                          BLIR_ERROR_MSG_OVERFLOW,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value,
                          ErrorRout);
  return;  /* Overflow Return.  */
}

/**********************************************************************
 *                                                                    *
 * uint_DTB_NAME - Convert a packed decimal to an unsigned binary *
 *                                                                    *
 * This routine will convert a packed decimal number to an uint32 *
 * binary number.  An error handler will be called for various types  *
 * of errors:                                                         *
 *                                                                    *
 * *  Arguments too invalid to proceed:  Null Pointers or impossible  *
 *    lengths.                                                        *
 *                                                                    *
 * *  Invalid decimal format                                          *
 *                                                                    *
 * *  Overflow during conversion, i.e. the value exceeds the limit of *
 *    an Unsigned  Integer number                                     *
 *                                                                    *
 * This routine employs several goto statements to jump to error      *
 * logic.  While this may seem objectionable to some, it does reduce  *
 * the vertical space of the routine.  If the error calls were added  *
 * to each place the gotos exist, the predominate instruction path    *
 * would be obscured.                                                 *
 *                                                                    *
 *********************************************************************  */

void
uint_DTB_NAME (uint_TYPE * binary,  /* Binary value.  */
                   int decimal_length_in_bytes,  /* Length of decimal value.  */
                   void *decimal_value,  /* Decimal value.  */
                   uint_HANDLER ErrorRout) /* Decimal error routine.  */
{

  const char *maxPos = uint_MAXPOS_BLI_CHAR;  /* Maximum Positive.  */
  uint_TYPE numBin;  /* Binary Number.  */
  uchar *numDec;  /* Decimal Number.  */
  const char *str1;  /* String 1 pointer.  */
  const char *str2;  /* String 2 pointer.  */
  int digit;  /* Digit.  */
  int i, n1, n2;  /* Index.  */
  int sigFlag;  /* Significant digit detected.  */
  uchar numImage[uint_MAXBLI_CHAR +1];  /* Number image.  */
  int numImageIdx;  /* Number image index.  */

  /*--------------------------------------------------------------------*
   * Check for argument errors that are so egregious that execution is  *
   * impossible                                                         *
   *--------------------------------------------------------------------.  */

  if (binary == NULL ||
      decimal_length_in_bytes <= 0 ||
      decimal_value == NULL)goto Error_Bad_Arguments;

  /*--------------------------------------------------------------------*
   * Process all but the last byte of the decimal number.  These bytes  *
   * contain decimal digits in both the upper and lower nibble.         *
   *--------------------------------------------------------------------.  */

  numDec = (uchar *)decimal_value;
  sigFlag = 0;
  numImageIdx = 0;
  for (i = 0; i < (decimal_length_in_bytes - 1); i ++)
    {
      digit = numDec[i] >> 4;
      if (digit > 0)
        sigFlag = 1;
      if (sigFlag)
        {
          if (numImageIdx >= uint_MAXBLI_CHAR)goto Error_Overflow;
          if (digit > 9)goto Error_Bad_Decimal;
          numImage[numImageIdx ++] = (char)('0' + digit);
        }
      digit = numDec[i] & 0x0F;
      if (digit > 0)
        sigFlag = 1;
      if (sigFlag)
        {
          if (numImageIdx >= uint_MAXBLI_CHAR)goto Error_Overflow;
          if (digit > 9)goto Error_Bad_Decimal;
          numImage[numImageIdx ++] = (char)('0' + digit);
        }
    }

  /*--------------------------------------------------------------------*
   * Process the last byte of the decimal number.  This byte contains a *
   * digit in the upper nibble and a sign code in the lower nibble      *
   * which is ignored for uint32 conversion.                        *
   *--------------------------------------------------------------------.  */

  digit = numDec[decimal_length_in_bytes - 1] >> 4;

  if (numImageIdx >= uint_MAXBLI_CHAR)goto Error_Overflow;

  if (digit > 9)goto Error_Bad_Decimal;

  numImage[numImageIdx ++] = (char)('0' + digit);
  numImage[numImageIdx] = '\0';

  /*--------------------------------------------------------------------*
   * While the number does not exceed the maximum number of digits, the *
   * high value is less than all 9's.  Verify that the exact value does *
   * not exceed the maximum width of an Unsigned  Integer value.        *
   *--------------------------------------------------------------------.  */

  if (numImageIdx == uint_MAXBLI_CHAR)
    {
      str1 = (char *)numImage;
      str2 = maxPos;
      for (; ( * str1 ==  * str2) && ( * str1 != '\0'); str1 ++, str2 ++);
      n1 =  * str1;  /* Make sure signed value.  */
      n2 =  * str2;  /* Make sure signed value.  */
      if ((n1 - n2) > 0)goto Error_Overflow;
    }

  /*--------------------------------------------------------------------*
   * Everything is okay.  Convert the collected components to an        *
   * Unsigned  Integer number and return to caller.                     *
   *--------------------------------------------------------------------.  */

  numBin = 0;
  for (i = 0; i < numImageIdx; i ++)
    numBin = (uint_TYPE)(numBin * 10 + numImage[i] - '0');

  * binary = numBin;
  return;  /* Normal Return *  */

  /*--------------------------------------------------------------------*
   * An argument error too egregious to continue has occurred           *
   *--------------------------------------------------------------------.  */

 Error_Bad_Arguments :

  uint_Error_Handler (BLIR_ERROR_CODE_INVARGS,
                          BLIR_ERROR_MSG_INVARGS,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value,
                          ErrorRout);
  return;  /* Bad Argument Return.  */

  /*--------------------------------------------------------------------*
   * The decimal number is invalid                                      *
   *--------------------------------------------------------------------.  */

 Error_Bad_Decimal :

  uint_Error_Handler (BLIR_ERROR_CODE_INVDEC,
                          BLIR_ERROR_MSG_INVDEC,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value,
                          ErrorRout);
  return;  /* Bad Decimal Return.  */

  /*--------------------------------------------------------------------*
   * An overflow would have occurred                                    *
   *--------------------------------------------------------------------.  */

 Error_Overflow :

  uint_Error_Handler (BLIR_ERROR_CODE_OVERFLOW,
                          BLIR_ERROR_MSG_OVERFLOW,
                          binary,
                          decimal_length_in_bytes,
                          decimal_value,
                          ErrorRout);
  return;  /* Overflow Return.  */
}

