/*
  
  BLISS Convert Functions. 

  Numeric to alphanumeric.
  Alphanumeric to numeric.

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

#ifndef __BLIR_CONVERT__
#define __BLIR_CONVERT__

/* Numeric to alpha.  */
void blir_convert_32_n_X (uint32 value, uint32 digits, void *address, uint32 length);
void blir_convert_32_y_X (int32 value, uint32 digits, void *address, uint32 length);
void blir_convert_64_n_X (uint64 value, uint32 digits, void *address, uint32 length);
void blir_convert_64_y_X (int64 value, uint32 digits, void *address, uint32 length);

/* Alpha to numeric.  */
uint32 blir_convert_X_32_n (void *address, uint32 length, uint32 digits);
int32 blir_convert_X_32_y (void *address, uint32 length, uint32 digits);
uint64 blir_convert_X_64_n (void *address, uint32 length, uint32 digits);
int64 blir_convert_X_64_y (void *address, uint32 length, uint32 digits);

#endif
