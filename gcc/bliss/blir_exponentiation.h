/*
  
  BLISS Exponentiation Functions. 

  Integers only supported at present. 
  
  One per pair of data/exponent types. The variables are size (32/64) and sign (unsigned, signed)
  
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

#ifndef __BLIR_EXPONENTIATION__
#define __BLIR_EXPONENTIATION__

uint32 blir_exponentiation_32_n_32_n (uint32, uint32);
uint32 blir_exponentiation_32_n_32_y (uint32, int32);
uint32 blir_exponentiation_32_n_64_n (uint32, uint64);
uint32 blir_exponentiation_32_n_64_y (uint32, int64);
int32 blir_exponentiation_32_y_32_n (int32, uint32);
int32 blir_exponentiation_32_y_32_y (int32, int32);
int32 blir_exponentiation_32_y_64_n (int32, uint64);
int32 blir_exponentiation_32_y_64_y (int32, int64);
uint64 blir_exponentiation_64_n_32_n (uint64, uint32);
uint64 blir_exponentiation_64_n_32_y (uint64, int32);
uint64 blir_exponentiation_64_n_64_n (uint64, uint64);
uint64 blir_exponentiation_64_n_64_y (uint64, int64);
int64 blir_exponentiation_64_y_32_n (int64, uint32);
int64 blir_exponentiation_64_y_32_y (int64, int32);
int64 blir_exponentiation_64_y_64_n (int64, uint64);
int64 blir_exponentiation_64_y_64_y (int64, int64);

#endif
