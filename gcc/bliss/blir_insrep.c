/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU Lesser General Public     *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

#include <stdlib.h>
#include "blir_inspect.h"

/* Cobr_inspect_replacing routine implements replacing functionality.  */
/* Of BLISS inspect clause, supporting ANSI 85 syntax.  */
/* It replace identifier1, replacing_count occurrences defined in.  */
/* Cobr_inspect_replacing_struct.  */
      
void
blir_inspect_replacing (
                        unsigned char *identifier1,
                        unsigned int identifier1_length,  /* Identifier 1 below, not null terminated.  */
                        unsigned int replacing_count,  /* Number of replacing details.  */
                        struct blir_inspect_replacing_struct *replacing_details,  /* Array of replacing details.  */
                        unsigned int *replace_status,  /* Vector to store status of replacement tasks.  */
                        /* Values are : 1 --> initial value means no replacements have done.  */
                        /*              2 --> Replacement has been initiated.  */
                        /*              0 --> Replacement is finished.  */
                        unsigned int *after_bound,  /* Vector store index of after position for each replacement.  */
                        unsigned int *before_bound /* Vector store index of before position for each replacement.  */
                        /* All this three, upper defined vectors occurs replacing_count times.  */)
{
  unsigned int ident1_index;  /* Identifier1 index.  */
  unsigned char *ident1;  /* Points to identifier1.  */
  unsigned char *aux_ident1;  /* Points to identifier1 
                                          for match look up.  */
  struct blir_inspect_replacing_struct *details;  /* Points to replacing_details.  */
  unsigned char *ident3;
  unsigned char *ident5;
  unsigned int details_index;
  unsigned int before_index;
  unsigned int after_index;
  unsigned int match_index;

  for (details_index = 0; 
      details_index < replacing_count;
      details_index ++)replace_status[details_index] = 1;  /* Initialize replace_status true.  */
  details = replacing_details;
  details_index = 0;
  /* Determine after and before bounds for each replacing detail.  */
  /* Save info in arrays for future use.  */
  while (details_index < replacing_count)
    {
      after_index = 0;
      before_index = 0;
      if (details->details.after_value_length != 0) /* Find after bound.  */
        {
          after_index = blir_inspect_indexof (identifier1,
                                           identifier1_length,
                                           details->details.after_value,
                                           details->details.after_value_length);
          after_index --;
        }
      if (details->details.before_value_length != 0) /* Find before bound.  */
        {
          before_index = blir_inspect_indexof (identifier1,
                                            identifier1_length,
                                            details->details.before_value,
                                            details->details.before_value_length);
          before_index --;
        }
      after_bound[details_index] = after_index;
      before_bound[details_index] = before_index;
      details_index ++;
      details ++;
    }
  ident1_index = 0;
  ident1 = identifier1;
  while (ident1_index < identifier1_length) /* Identifier1 loop.  */
    {
      details_index = 0;
      details = replacing_details;
      while (details_index < replacing_count) /* Replacement details loop.  */
        {
          after_index = after_bound[details_index];  /* Assign corresponding
                                                     detail bound.  */
          before_index = before_bound[details_index];
          if (details->type == blir_replacing_type_characters) /* Characters replace.  */
            {
              if (((after_index == 0)
                    || ((after_index != 0) && (ident1_index > after_index)))
                   && ((before_index == 0)
                      || ((before_index != 0) && (ident1_index < before_index))) /* We are in-bounds.  */
                   )
                {
                  ident5 = details->identifier5;
                  * ident1 = * ident5;
                  break;  /* Start again from the first occurance of replacing_details.  */
                }
            }
          else /* It's leading or all or first replace.  */
            {
              if (((after_index == 0)
                    || ((after_index != 0) && (ident1_index > after_index)))
                   && ((before_index == 0)
                      || ((before_index != 0) && (ident1_index < before_index))) /* We are in-bounds.  */
                   )
                {
                  aux_ident1 = ident1;
                  match_index = 0;
                  ident3 = details->identifier3;
                  while (match_index < details->identifier3_length) /* Matching loop.  */
                    {
                      if ( * aux_ident1 != * ident3)
                        {
                          if (details->type == blir_replacing_type_leading
                                && replace_status[details_index] == 2)
                            /* No match so this is the end.  */
                            /* For leading condition.  */
                            {
                              replace_status[details_index] = 0;  /* Flag end of replace
                                                                  leading.  */
                              /* So we don't do
                                 replacement again.  */
                            }
                          break;  /* No match, go away to next replacing_detail.  */
                        }
                      match_index ++;  /* Go to next position to match.  */
                      ident3 ++;
                      aux_ident1 ++;
                    }
                  if (match_index == details->identifier3_length) /* Ident3 match.  */
                    {
                      if (replace_status[details_index]) /* All, leading or first
                                                           condition true.  */
                        { /* So lets do the
                                                            replacement.  */
                          match_index = 0;
                          ident5 = details->identifier5;
                          while (match_index < details->identifier3_length)
                            {
                              if (match_index < details->identifier5_length)
                                {
                                  * ident1 = * ident5;
                                }
                              else
                                { /* Identifier5_length < identifier3_length.  */
                                  /* So repeat the pattern again to support.  */
                                  /* Figurative constants.  */
                                  ident5 = details->identifier5; 
                                  * ident1 = * ident5; 
                                }
                              ident1 ++;
                              ident1_index ++;
                              ident5 ++;
                              match_index ++;
                            }
                          /* Check leading or first condition.  */
                          /* All no needs checking.  */
                          if (details->type == blir_replacing_type_leading)
                            {
                              /* Never replace this before.  */
                              if (replace_status[details_index] == 1)
                                {
                                  /* Flag replace_status for this.  */
                                  /* Instance so we will know when.  */
                                  /* Leading is finish.  */
                                  replace_status[details_index] = 2; 
                                }
                            }
                          if (details->type == blir_replacing_type_first)
                            {
                              /* Flag replace_status for this.  */
                              /* Instance so we will know not to.  */
                              /* Do this replacement again.  */
                              replace_status[details_index] = 0; 
                            }
                          ident1 --;
                          ident1_index --;
                          break;  /* Start again from the first occurance of
                                    replacing_details.  */
                        } /* End all, leading or first condition.  */
                    } /* End ident3 match.  */
                } /* End in bounds search.  */
            } /* End all, leading or first replacement.  */
          details ++;
          details_index ++;
        } /* End replacement details loop.  */
      ident1 ++;
      ident1_index ++;
    } /* End identifier1 loop.  */
}
