/*
  
  BLISS comparison helper functions.
  
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

#ifndef __BLIR_CMP__
#define __BLIR_CMP__

int32 blir_cmpfill (void * address, uint32 length, uchar fill);
int32 blir_cmpall (void * address1, uint32 length1, void * address2, uint32 length2);

#endif
