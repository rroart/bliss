/*
  
  BLISS comparison helper functions.  These are too hard to generate
  inline or there is no value in doing so.
  
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

#include "blir_temp_config.h"
#include "blidebug.h"
#include "blir_cmp.h"

/* Compare the data item at ADDRESS of length LENGTH with an implied
string of the same length filled with FULL.  Return -1, 0, 1 if the
data item is less than, equal to or greater than the implied item.  */

int32 
blir_cmpfill (void * address, uint32 length, uchar fill)
{
  uchar *s;
  uchar *end;
  s = address;
  end = s + length;
  
  while (s != end)
    {
      if (* s == fill)
        {
          s++;
          continue;
        }
      if (* s < fill)
        return -1;
      return +1;
    }
  
  return 0;
}

/* Compare two data items the first at ADDRESS1 and of length LENGTH1,
   the second at ADDRESS2 and of length LENGTH2.  The second item
   should be conceptually repeated until all of the first item has
   been compared.  */

int32
blir_cmpall (void * address1, uint32 length1, void * address2, uint32 length2)
{

  uchar *s1;
  uchar *end1;
  
  uchar *s2;
  uchar *end2;
  
  s2 = address2;

 /* Compiler checks all literal to ensure literal is not zero length.  */
  BLI_ASSERT (length2);
  
  if (length2 == 1)
    return blir_cmpfill (address1, length1, *s2);

  s1 = address1;
  end1 = s1 + length1;

  end2 = s2 + length2;

  while (s1 != end1)
    {
      if (* s1 == * s2)
        {
          s1++;
          s2++;
          if (s2 == end2)
            s2 = address2;
          continue;
        }
      else
        if (* s1 < *s2)
          return -1;
        else
          return +1;
    }
  return 0;
}

