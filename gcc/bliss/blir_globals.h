
/**********************************************************************
 *                                                                    *
 *            Global Definitions for BLISS Runtime system             *
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
 * Title:  Global definitions for BLISS runtime system                *
 *                                                                    *
 * Description:    This is a header file for the global runtime       *
 *                 definitions and routine prototypes.                *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log: T. Seward       09/Jul/2000     Initial Release  *
 *                 of Routines                                        *
 *                                                                    *
 *********************************************************************  */


#ifndef __BLIR_GLOBALS_H__
#define __BLIR_GLOBALS_H__

/*--------------------------------------------------------------------*
 *                            Error Codes                             *
 *--------------------------------------------------------------------.  */

enum blir_error_codes
{
  BLIR_ERROR_CODE_OK = 0,  /* No Error.  */
  BLIR_ERROR_CODE_OVERFLOW = 1,  /* Overflow.  */
  BLIR_ERROR_CODE_INVDEC = 2,  /* Invalid Decimal Number.  */
  BLIR_ERROR_CODE_INVARGS = 3 /* Invalid Arguments.  */
};

/*--------------------------------------------------------------------*
 *                           Error Messages                           *
 *--------------------------------------------------------------------.  */

#define BLIR_ERROR_MSG_OK       "Okay"
#define BLIR_ERROR_MSG_OVERFLOW "Overflow"
#define BLIR_ERROR_MSG_INVDEC   "Invalid Decimal Number"
#define BLIR_ERROR_MSG_INVARGS  "Invalid Arguments"


/*--------------------------------------------------------------------*
 *    Usage Types                                                     *
 *--------------------------------------------------------------------.  */

enum blir_type {
  blir_typeNull = 0,  /* Null type.  */
  blir_typeText = 1,  /* Text Type.  */
  blir_typeDisplay = 2,  /* Numeric Usage Display.  */
  blir_typePacked = 3,  /* Packed Decimal.  */
  blir_typeBinary = 4,  /* Signed binary.  */
  blir_typeUnsigned = 5,  /* Unsigned binary.  */
  blir_typeFloat = 6,  /* Floating point.  */
  blir_typePointer = 7 /* Pointer.  */
};


/*--------------------------------------------------------------------*
 *                            Usage Flags                             *
 *--------------------------------------------------------------------.  */

#define blir_flagNull         0x00000000        /* No flags set */
#define blir_flagSignLead     0x80000000        /* Display with SIGN IS LEADING */
#define blir_flagSignTrail    0x40000000        /* Display with SIGN IS TRAILING */
#define blir_flagOverpunch    0x20000000        /* Display with SIGN IS OVERPUNCHED */
#define blir_flagRightAligned 0x10000000        /* Data item is right aligned */

/**********************************************************************
 *                                                                    *
 * Prototypes                                                         *
 *                                                                    *
 *********************************************************************  */

void blir_error_termination (int code,  /* Error Code.  */
                             const char *message);  /* Error Message.  */

#endif

