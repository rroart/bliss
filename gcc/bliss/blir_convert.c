/*
  
  BLISS Conversion Functions. 

  Integers only supported at present. Signs are dropped. No validation
  is done.
  
  One per data type. The variables are size (32/64)
  and sign (unsigned, signed).
  
  Copyright (C) 2001 by Free Software Foundation, Inc.

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
  02111-1307 USA.  */

#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_convert.h"
#include <memory.h>

/* Numeric to alpha.  */

/* Always use the unsigned conversions to avoid getting signs (eg with max -ve value.  */

/* XXX; functions need comments.  */

#define CC(MAX,BITS)\
  uchar chars[MAX+1];\
  int##BITS uvalue;\
  int32 ix;\
  if (value<0)\
     uvalue=-value;\
  else\
     uvalue=value;\
  memset(chars, '0', MAX);\
  chars[MAX]=0;\
  for (ix=MAX-1; uvalue&&(ix>=0); ix--)\
     {\
        /* gcc optimizes away the divides */\
        chars[ix]=uvalue%10+(uchar)'0';\
        uvalue/=10;\
     }\
  BLI_ASSERT (digits<=MAX);\
  if (length<=digits)\
    memcpy(address, chars+MAX-digits, length);\
  else \
  {\
     memcpy(address, chars+MAX-digits, digits);\
     memset(((native_char*)address)+digits, ' ', length-digits);\
  }

#define CCU(MAX,BITS)\
  uchar chars[MAX+1];\
  uint##BITS uvalue;\
  int32 ix;\
  uvalue=value;\
  memset(chars, '0', MAX);\
  chars[MAX]=0;\
  for (ix=MAX-1; uvalue&&(ix>=0); ix--)\
     {\
        /* gcc optimizes away the divides */\
        chars[ix]=uvalue%10+(uchar)'0';\
        uvalue/=10;\
     }\
  BLI_ASSERT (digits<=MAX);\
  if (length<=digits)\
    memcpy(address, chars+MAX-digits, length);\
  else \
  {\
     memcpy(address,chars+MAX-digits, length);\
     memset(((native_char*)address)+digits, ' ', length-digits);\
  }


void 
blir_convert_32_n_X (uint32 value, uint32 digits, void *address, uint32 length)
{
  CCU (10, 32);
}

void 
blir_convert_32_y_X (int32 value, uint32 digits, void *address, uint32 length)
{
  CC (10, 32);
}

void 
blir_convert_64_n_X (uint64 value, uint32 digits, void *address, uint32 length)
{
  CCU (20, 64);
}

void 
blir_convert_64_y_X (int64 value, uint32 digits, void *address, uint32 length)
{
  CC (19, 64);
}

/* Alpha to numeric.  */

#define DD(U,BITS)\
  U##int##BITS ret1=0;\
  uchar* chars;\
  uchar* chars_end;\
  chars=address;\
  chars_end=chars+length;\
  if (digits&&(length>digits))\
     chars+=(length-digits);\
  while (chars!=chars_end)\
     ret1=ret1*10+*(chars++)-(uchar)'0';\
  return ret1;\

uint32
blir_convert_X_32_n (void *address, uint32 length, uint32 digits)
{
  DD (u, 32);
}

int32
blir_convert_X_32_y (void *address, uint32 length, uint32 digits)
{
  DD (, 32);
}

uint64
blir_convert_X_64_n (void *address, uint32 length, uint32 digits)
{
  DD (u, 64);
}

int64
blir_convert_X_64_y (void *address, uint32 length, uint32 digits)
{
  DD (, 64);
}


