/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU Lesser General Public     *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

#include "blir_temp_config.h"
#include "blir_inspect.h"

/* Inspect verb runtime utilitiy routines.  */

/* Looks  for the index of string Target in string Source and returns it
   index returned starting at 1
   if not found returns 0.  */

uint32
blir_inspect_indexof (uchar *source,
                       uint32 source_length, 
                       uchar *target,  /* Target to look for.  */
                       uint32 target_length)
{
  uint32 index;  /* Index position.  */
  uint32 i, j;  /* Source and Target iteration index.  */
  uchar *p,  * t,  * s;  /* Aux Source and Target pointiers.  */
        
  p = source;
  t = target;
  index = 0;
  i = 1;
  while (i <= source_length)
    {
      if ( * p == * t) /* First match.  */
        {
          index = i;  /* Save index position.  */
          s = p;
          j = 1;
          while (( * p == * t) && (j <= target_length)
                && (i <= source_length)) /* Look for next matches.  */
            {
              p ++;
              t ++;
              i ++;
              j ++;
            }
          if (j > target_length) /* Found the index go away.  */
            {
              break;
            }
          else
            {
              if (i > source_length) /* Sorry not match.  */
                {
                  index = 0;
                  break;
                }
              else /* There is still room in source
                                    to continue seeking.  */
                {
                  t = target;  /* Put back t at the begining of target.  */
                  p = s;  /* Reset source position at first match.  */
                  i = index;  /* Reset index source position at 
                                     first match.  */
                }
            }
        } /* End first match.  */
      p ++;
      i ++;
    } /* End search while.  */
  return index;
}
