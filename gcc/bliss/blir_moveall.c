/*
  
  BLISS Move All Function - alphanumeric Used for > 1 character alpha to alpha move

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
#include "blir_moveall.h"
#include <memory.h>

/* Move repeated copies of FROM, which is of length FROM_LENGTH into
   TO which has length TO_LENGTH.  */

void
blir_moveall (void *to, uint32 to_length, void *from, uint32 from_length)
{
  uchar *to_char;
  uchar *to_end;
  uint32 slop;
  uint32 round_units;
  
  to_char = to;
  
  BLI_ASSERT (from_length);
  round_units = to_length / from_length * from_length;
  slop = to_length % from_length;
  to_end = to_char + round_units;
  
  while (to_char != to_end)
    {
      memcpy (to_char, from, from_length);
      to_char += from_length;
    }
  
  if (slop)
    memcpy (to_char, from, slop);
    
}
