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
#include "config.h"

void
blir_inspect_converting (
                         uchar *identifier1,
                         uint32 identifier1_length,  /* Identifier 1 below, not null terminated.  */
                         uchar *identifier6,  /* Identifier-6 or literal-4 to
                                                             convert from.  */
                         uint32 identifier6_length,  /* Length of above.  */
                         uchar *identifier7,  /* Identifier-7 or literal5 to
                                                             convert to.  */
                         uint32 identifier7_length ATTRIBUTE_UNUSED,  /* Length of above.  */
                         uint32 before_after_count ATTRIBUTE_UNUSED,  /* Number of before after
                                                             details.  */
                         struct blir_before_after_details *details /* Array of details.  */
                         )
{
         
  uchar *ident6;  /* Points to identifier6.  */
  uchar *ident7;  /* Points to identifier7.  */
  uchar *ident1;  /* Points to identifier1.  */
  uchar *ident2;  /* Points to identifier1 
                                 at start replacing position.  */
  uint32 convert_index;  /* Position in identifier6 & 7.  */
  uint32 before_index;  /* Position of before string.  */
  uint32 after_index;  /* Position of after string.  */
  uint32 index, i;  /* Auxiliar indexes.  */

  convert_index = 1;
  ident6 = identifier6;
  ident7 = identifier7;
  ident1 = identifier1;
  after_index = 1;  /* First approach start converting.  */
  before_index = identifier1_length;  /* First approach end converting.  */
  if (details->after_value_length != 0)
    {
      index = blir_inspect_indexof (identifier1,
                                 identifier1_length,
                                 details->after_value,
                                 details->after_value_length);
      if (index != 0)
        {
          i = 1;
          while (i <= index) /* Go to initial converting.  */
            {
              ident1 ++;
              i ++;
            }
          index ++;  /* After_value should be 1 char length.  */
          after_index = index;
        }
    }
  if (details->before_value_length != 0)
    {
      index = blir_inspect_indexof (identifier1,
                                 identifier1_length,
                                 details->before_value,
                                 details->before_value_length);
      before_index = index;
    }
  while (convert_index <= identifier6_length)
    {
      ident2 = ident1;  /* Save position for multiple replacing.  */
      index = after_index;
      while (index < before_index)
        {
          if ( * ident2 == * ident6)
            {
              * ident2 = * ident7;
            }
          ident2 ++;
          index ++;
        }
      convert_index ++;
      if (convert_index <= identifier6_length)
        {

          ident6 ++;
          ident7 ++;
        }
    }

}

