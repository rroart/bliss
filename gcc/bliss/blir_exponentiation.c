/*
  
  BLISS Exponentiation Functions. 

  Integers only supported at present. 
  
  One per pair of data/exponent types. The variables are size (32/64)
  and sign (unsigned, signed)
  
  All functions return the value of data to the power of
  exponent. Overflows and invalid arguments are not detected and will
  result in truncation of the result. Divides by 0 will probably
  result in a machine trap; they are not explicitly checked for. There
  is no provision for rounding.

  Copyright (C) 2001 Free Software Foundation, Inc. 

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
#include "blir_exponentiation.h"

/* Unsigned/unsigned.  */
#define UU(z1,z2)\
  uint##z1 power;\
  uint##z1 ret1=1;\
\
  if (exp==0)\
    return 1;\
  for (power=data; exp; power*=power, exp>>=1)\
    {\
      if (exp&1)\
        ret1*=power;\
\
    }\
  return ret1;

uint32 
blir_exponentiation_32_n_32_n (uint32 data, uint32 exp)
{
  UU (32, 32);
}

uint32 
blir_exponentiation_32_n_64_n (uint32 data, uint64 exp)
{
  UU (32, 64);
}
 
uint64 
blir_exponentiation_64_n_32_n (uint64 data, uint32 exp)
{
  UU (64, 32);
}

uint64 
blir_exponentiation_64_n_64_n (uint64 data, uint64 exp)
{
  UU (64, 64);
}

/* Unsigned/signed.  */
#define US(z1,z2)\
  if (exp>=0)\
    return blir_exponentiation_##z1##_n_##z2##_n(data, exp);\
  if (data==1)\
    return data;\
  return 0;


uint32 
blir_exponentiation_32_n_32_y (uint32 data, int32 exp)
{
  US (32, 32);
}

uint32 
blir_exponentiation_32_n_64_y (uint32 data, int64 exp)
{
  US (32, 64);
}
 
uint64 
blir_exponentiation_64_n_32_y (uint64 data, int32 exp)
{
  US (64, 32);
}

uint64 
blir_exponentiation_64_n_64_y (uint64 data, int64 exp)
{
  US (64, 64);
}

/* Signed/unsigned.  */
#define SU(z1,z2)\
  int##z1 power;\
  int##z1 ret1=1;\
\
  if (exp==0)\
     {\
        BLI_ASSERT(data>0);\
        return 1;\
     }\
  for (power=data; exp; power*=power, exp=(((uint##z2)exp)>>1))\
    {\
      if (exp&1)\
        ret1*=power;\
    }\
  return ret1;


int32 
blir_exponentiation_32_y_32_n (int32 data, uint32 exp)
{
  SU (32, 32);
}

int32 
blir_exponentiation_32_y_64_n (int32 data, uint64 exp)
{
  SU (32, 64);
}

int64 
blir_exponentiation_64_y_32_n (int64 data, uint32 exp)
{
  SU (64, 32);
}
 
int64 
blir_exponentiation_64_y_64_n (int64 data, uint64 exp)
{
  SU (64, 64);
}

/* Signed/signed.  */
#define SS(z1,z2)\
  if (exp>=0)\
    return blir_exponentiation_##z1##_y_##z2##_n(data, exp);\
  if (data==1)\
    return data;\
  return 0;
 
int32 
blir_exponentiation_32_y_32_y (int32 data, int32 exp)
{
  SS (32, 32);
}
 
int32 
blir_exponentiation_32_y_64_y (int32 data, int64 exp)
{
  SS (32, 64);
}

int64 
blir_exponentiation_64_y_32_y (int64 data, int32 exp)
{
  SS (64, 32);
}

int64 
blir_exponentiation_64_y_64_y (int64 data, int64 exp)
{
  SS (64, 64);
}
