
/*
  
  Temporary Global Data Types for BLISS Runtime system        
  
  Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.        
  Copyright (C) 2000, 2001, 2002 Free Software Foundation, Inc.
  
  This library is free software; you can redistribute it and/or      
  modify it under the terms of the GNU Lesser General Public License 
  as published by the Free Software Foundation; either version 2.1   
  of the License, or (at your option) any later version.             
  
  This library is distributed in the hope that it will be useful,    
  but WITHOUT ANY WARRANTY; without even the implied warranty of     
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   
  Lesser General Public License for more details.                    
  
  You should have received a copy of the GNU Lesser General Public   
  License along with this library; if not, write to the Free         
  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  
  02111-1307 USA                                                     
  
  Mail:   
  Theodore J. Seward, Jr.                                    
  Apt. 343,                                          
  2008 Deerpark Place,                               
  Fullerton, CA  92831                               
  
  E-mail: tseward@compuserve.com                                     
  
  */


/*
  
  Title:  Temporary Global data types for BLISS runtime system       
  
  Description:    This is a header file for the temporary global     
  data types.  This file will be replaced by a       
  configuration file of some kind.                   
  
  System: GNU BLISS Runtime Library                                  
  
  Modification Log:                                                  
  
  Programmer  Date         Description of Change                     
  ----------  -----------  ----------------------------------------- 
  T. Seward       19/Aug/2000     Initial Release of Routines        
  See ChangeLog for subsequent changes. 
  
*/

#ifndef __BLIR_TEMP_CONFIG_H__
#define __BLIR_TEMP_CONFIG_H__

#ifndef GTY
#define GTY(a)
#define _BLIR_GTY_SET_
#endif

/* Constants.  */

/* Bits per Byte/Word.  */

#define BLI_BITS_PER_BYTE 8
#define LOG_BLI_BITS_PER_BYTE 3
#define BLI_BITS_PER_WORD 32

/* Generic types.  */

#define BLI_SIZE_T size_t /* size type */

/* Signed Integer Binary Data Types */

typedef signed char schar;  /* 8 bit signed binary */
typedef short int16;        /* 16 bit signed binary */
typedef int int32;          /* 32 bit signed binary */
typedef long long int64;    /* 64 bit signed binary */

#define BLI_INT8_WIDTH 1 /* Width of BLI_INT8 type */
#define BLI_INT16_WIDTH 2 /* Width of BLI_INT16 type */
#define BLI_INT32_WIDTH 4 /* Width of BLI_INT32 type */
#define BLI_INT64_WIDTH 8 /* Width of BLI_INT64 type */
#define BLI_INT128_WIDTH 16 /* Width of BLI_INT128 type */

#define BLI_INT32_MASK 0xffffffffULL
#define BLI_INT32_SHIFT_TO_LOW 32

#define BLI_INT8_MAX  ((int32)0x0000007fUL)
#define BLI_INT8_MIN  ((int32)0xffffff80UL)
#define BLI_INT16_MAX  ((int32)0x00007fffUL)
#define BLI_INT16_MIN  ((int32)0xFFFF8000L)
#define BLI_INT32_MAX  ((int32)0x7fffffffUL)
#define BLI_INT32_MIN  ((int32)0x80000000UL)
#define BLI_INT64_MAX  ((int64)0x7fffffffffffffffULL)
#define BLI_INT64_MIN  ((int64)0x8000000000000000ULL)

/* Unsigned Integer Binary Data Types */
typedef unsigned char uchar;      /* 8 bit unsigned binary */
typedef unsigned short uint16;     /* 16 bit unsigned binary */
typedef unsigned int uint32;       /* 32 bit unsigned binary */
typedef unsigned long long uint64; /* 64 bit unsigned binary */

/* Gengtype.c can cope with these as pointers to integral types, but
   it can't cope with much else.  */
struct ucharptr_struct GTY(())
{
  uchar * ptr;
};

struct uint32_struct GTY(())
{
  unsigned int value;
};

#define BLI_UINT8_WIDTH 1 /* Width of BLI_UINT8 type */
#define BLI_UINT16_WIDTH 2 /* Width of BLI_UINT16 type */
#define BLI_UINT32_WIDTH 4 /* Width of BLI_UINT32 type */
#define BLI_UINT64_WIDTH 8 /* Width of BLI_UINT64 type */
#define BLI_UINT128_WIDTH 16 /* Width of BLI_UINT128 type */

#define BLI_UINT8_MAX  0xffUL
#define BLI_UINT16_MAX 0xffffUL
#define BLI_UINT32_MAX 0xffffffffUL
#define BLI_UINT64_MAX 0xffffffffffffffffULL

/* Pointer.  */

#define BLI_POINTER_SIZE 32

#define BLI_CHAR_SIZE 8
typedef char native_char;

/* Native int.  */

/* Use these when you really need the native int e.g. for optimising
   runtime speed of numeric routines.  */
typedef int native_int;

#define BLI_NATIVE_INT_BITS 32
#define BLI_NATIVE_INT BLI_INT32
#define BLI_NATIVE_INT_WIDTH BLI_INT32_WIDTH
#define BLI_NATIVE_INT_SIZE  BLI_INT32_SIZE
#define BLI_NATIVE_INT_MAX BLI_INT32_MAX
#define BLI_NATIVE_INT_HIBIT (0x80000000) /* High bit mask.  */ 

#define BLI_NATIVE_UINT_WIDTH BLI_NATIVE_INT_WIDTH
#define BLI_NATIVE_UINT_SIZE BLI_NATIVE_INT_SIZE
#define BLI_NATIVE_UINT_MAX BLI_UINT32_MAX

/* Formats for various Integer Sizes.  */

#define BLI_INT8_FORMAT "c"
#define BLI_INT16_FORMAT "d"
#define BLI_INT32_FORMAT "d"
#define BLI_INT64_FORMAT "lld"

#define BLI_64_FORMAT_PREFIX "ll"

#define BLI_UINT8_FORMAT "c"
#define BLI_UINT16_FORMAT "u"
#define BLI_UINT32_FORMAT "u"
#define BLI_UINT64_FORMAT "llu"

/* Floating point Binary Data Types.  */

typedef float bli_float_32;   /* 32 bit floating point */
typedef double bli_float_64;  /* 64 bit floating point */

#define BLI_FLOAT_32_WIDTH 4 /* Width of BLI_FLOAT_SHORT type */
#define BLI_FLOAT_32_TARGET_WIDTH 4 /* Width of BLI_FLOAT_SHORT type on target machine */
#define BLI_FLOAT_64_WIDTH 8 /* Width of BLI_FLOAT_DOUBLE type */
#define BLI_FLOAT_64_TARGET_WIDTH 8 /* Width of BLI_FLOAT_DOUBLE type on target machine */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef _BLIR_GTY_SET_
#undef GTY
#endif

#endif
