/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU Lesser General Public     *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/* Utility routines for inspect verb runtime.  */

#include "stdlib.h"
#include "blir_inspect.h"
#include "blir_temp_config.h"

/* Returns a substring of string Source, starting at Start_position   
   with length Substring_len, if Start_position+Substring_len is grater than Source_length
   it will truncate to Source_length.  */

BLI_UINT8 *
blir_inspect_substring (BLI_UINT8 *source,
                         BLI_UINT32 source_length, 
                         BLI_UINT32 start_position,  /* Initial source position is 1.  */
                         BLI_UINT32 substring_len) /* Substring lenght of returned string.  */
     /* Substring above is null terminated.  */
{
  BLI_UINT32 i, j;  /* Source and Target iteration index.  */
  BLI_UINT8 *sub_string;  /* Returned substring.  */
  BLI_UINT8 *p,  * q;  /* Aux Source and substring pointiers.  */
        
  p = source;
  sub_string = (char *)malloc (substring_len +1);  /* Get some memory for the Substring.  */
  q = sub_string;
  i = 1;
  while (i <= source_length && i < start_position) /* Go to Start_position.  */
    {
      p ++;
      i ++;
    }
  if (i > source_length) /* Source string too small.  */
    {
      * sub_string = '\0';
    }
  else
    {
      j = 1;
      while (i <= source_length && j <= substring_len) /* Get substring.  */
        {
          * q ++ = * p ++;  /* Copy Source into sub_string.  */
          i ++;
          j ++;
        }
      * q = '\0';
    }
  return sub_string;
}
