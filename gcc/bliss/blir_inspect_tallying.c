/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU Lesser General Public     *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/* BLISS inspect clause tallying functionality implementation.  */

#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_inspect.h"

/* Cobr_inspect_tallying routine implements tallying functionality.  */
/* Of BLISS inspect clause, supporting ANSI 85 syntax.  */
/* It counts identifier1, tallying_count occurrences defined in.  */
/* Cobr_inspect_tallying_struct.  */

void
blir_inspect_tallying (
                       uchar *identifier1,
                       uint32 identifier1_length,  /* Identifier 1 below, not null terminated.  */
                       uint32 tallying_count,  /* Number of tallying details.  */
                       struct blir_inspect_tallying_struct *tallying_details,  /* Array
                                                                                  of tallying details.  */
                       uint32 **tally_status,  /* Vector to store status of tallying tasks.  */
                       /* Values are : 1 --> initial value means no replacements have done.  */
                       /*              2 --> Replacement has been initiated.  */
                       /*              0 --> Replacement is finished.  */
                       uint32 **after_bound,  /* Vector store index of after position for each replacement.  */
                       uint32 **before_bound /* Vector store index of before position for each replacement.  */
                       /* All this three, upper defined vectors occurs 
                          tallying_count times x all_leading_details_count.  */)

{
  uint32 ident1_index;  /* Identifier1 index.  */
  uint32 aux_ident1_index;  /* Identifier1 auxiliar index.  */
  uchar *ident1;  /* Points to identifier1.  */
  uchar *ident3;  /* Points to identifier3.  */
  uchar *aux_ident1;  /* Points to identifier1 
                                  for match look up.  */
  struct blir_inspect_tallying_struct *details;  /* Points to tallying_details.  */
  struct blir_all_leading_details *all_leading_details;  /* Points to all/leading tallying details.  */
  uint32 counter;  /* Tallying function's counter.  */
  uint32 details_index;
  uint32 all_leading_index;
  uint32 before_index;
  uint32 after_index;
  uint32 match_index;
  uint32 start_over = 0;  /* Start over condition flag.  */

  details = tallying_details;
  for (details_index = 0; 
      details_index < tallying_count;
      details_index ++, details ++)
    {
      if (details->char_flag)
        {
          tally_status[details_index][0] = 1;  /* Initialize tally_status for characters.  */
        }
      else
        {
          for (all_leading_index = 0;
              all_leading_index < details->details.all_leading.all_leading_details_count;
              all_leading_index ++)
            tally_status[details_index][all_leading_index] = 1;  /* Initialize tally_status true.  */
        }
    }
  details_index = 0;
  details = tallying_details;
  all_leading_index = 0;
  /* Determine after and before bounds for each tallying detail.  */
  /* Save info in arrays for future use.  */
  while (details_index < tallying_count)
    {
      after_index = 0;
      before_index = 0;
      if (details->char_flag) /* Tallying characters.  */
        {
          if (details->details.characters.after_value_length != 0)
            {
              after_index = blir_inspect_indexof (identifier1,
                                               identifier1_length,
                                               details->details.characters.after_value,
                                               details->details.characters.after_value_length);
              after_index = after_index + details->details.characters.after_value_length -1;
            }
          if (details->details.characters.before_value_length != 0) /* Find before bound.  */
            {
              before_index = blir_inspect_indexof (identifier1,
                                                identifier1_length,
                                                details->details.characters.before_value,
                                                details->details.characters.before_value_length);
              before_index --;
            }
          after_bound[details_index][0] = after_index;
          before_bound[details_index][0] = before_index;
        }
      else /* Tallying all/leading.  */
        {
          all_leading_details = details->details.all_leading.details;
          all_leading_index = 0;
          while (all_leading_index < details->details.all_leading.all_leading_details_count)
            {
              if (all_leading_details->details.after_value_length != 0)
                {
                  after_index = blir_inspect_indexof (identifier1,
                                                   identifier1_length,
                                                   all_leading_details->details.after_value,
                                                   all_leading_details->details.after_value_length);
                  after_index = after_index + all_leading_details->details.after_value_length -1;
                }
              if (all_leading_details->details.before_value_length != 0)
                {
                  before_index = blir_inspect_indexof (identifier1,
                                                    identifier1_length,
                                                    all_leading_details->details.before_value,
                                                    all_leading_details->details.before_value_length);
                  before_index --;
                }
              after_bound[details_index][all_leading_index] = after_index;
              before_bound[details_index][all_leading_index] = before_index;
              all_leading_details ++;
              all_leading_index ++;
            }
        }
      details_index ++;
      details ++;
    }
  ident1_index = 0;
  ident1 = identifier1;
  counter = 0;
  while (ident1_index < identifier1_length) /* Identifier1 loop.  */
    {
      details_index = 0;
      details = tallying_details;
      while (details_index < tallying_count) /* Tallying details loop.  */
        {
          if (details->char_flag) /* Tallying characters detail.  */
            {
              after_index = after_bound[details_index][0];  /* Assign corresponding
                                                            detail bound.  */
              before_index = before_bound[details_index][0];
              if (((after_index == 0)
                    || ((after_index != 0) && (ident1_index > after_index)))
                   && ((before_index == 0)
                      || ((before_index != 0) && (ident1_index < before_index))) /* We are in-bounds.  */
                   )
                {
                  counter ++;  /* Match occurrance increment counter.  */
                }
            }
          else
            { /* Is all/leading tallying.  */
              all_leading_details = details->details.all_leading.details;
              all_leading_index = 0;
              start_over = 0;  /* Start over condition false.  */
              while (all_leading_index < details->details.all_leading.all_leading_details_count)
                {
                  after_index = after_bound[details_index][all_leading_index];
                  before_index = before_bound[details_index][all_leading_index];
                  if (((after_index == 0)
                        || ((after_index != 0) && (ident1_index > after_index)))
                       && ((before_index == 0)
                          || ((before_index != 0) && (ident1_index < before_index))) /* We are in-bounds.  */
                       )
                    {
                      aux_ident1 = ident1;
                      match_index = 0;
                      ident3 = all_leading_details->identifier3;
                      aux_ident1_index = ident1_index;
                      while (match_index < all_leading_details->identifier3_length) /* Matching loop.  */
                        {
                          if ( * aux_ident1 != * ident3)
                            {
                              if (details->details.all_leading.all_flag != 1 /* Leading tallying.  */
                                    && tally_status[details_index][all_leading_index] == 2)
                                /* No match so this is the end.  */
                                /* For leading condition.  */
                                {
                                  tally_status[details_index][all_leading_index] = 0;  /* Flag end of tally
                                                                                       leading.  */
                                  /* So we don't do
                                     tally again.  */
                                }
                              break;  /* No match, go away to next tallying_detail.  */
                            }
                          else
                            { /* Check to see if get the before bound.  */
                              if (before_index != 0 && aux_ident1_index == before_index)
                                { /* We found the boundary go away.  */
                                  break;
                                }
                            }
                          match_index ++;  /* Go to next position to match.  */
                          ident3 ++;
                          aux_ident1 ++;
                          aux_ident1_index ++;
                        }
                      if (match_index == all_leading_details->identifier3_length) /* Ident3 match.  */
                        {
                          if (tally_status[details_index][all_leading_index])
                            { /* All/leading condition true.  */
                              /* So lets increment counter.  */
                              counter ++;
                              /* Move ident1, identifier3_length.  */
                              for (match_index = 0; 
                                  match_index < all_leading_details->identifier3_length;
                                  match_index ++)
                                {
                                  ident1 ++;
                                  ident1_index ++;
                                }
                              /* Check leading condition.  */
                              /* All no needs checking.  */
                              if (details->details.all_leading.all_flag != 1)
                                {
                                /* Never tally this before.  */
                                  if (tally_status[details_index][all_leading_index] == 1)
                                    {
                                      /* Flag tally_status for this.  */
                                      /* Instance so we will know when.  */
                                      /* Leading is finish.  */
                                      tally_status[details_index][all_leading_index] = 2; 
                                    }
                                }
                              ident1 --;
                              ident1_index --;
                              start_over = 1;
                              break;  /* Start again from the first occurance of
                                        tallying_details.  */
                            } /* End all/leading condition.  */
                        } /* End ident3 match.  */
                    } /* End in Bound search.  */
                  all_leading_index ++;
                  all_leading_details ++;
                } /* End all/leading details loop.  */
            } /* End all/leading tallying.  */
          * (details->identifier2) = * (details->identifier2) + counter;
          counter = 0;
          if (start_over)break;  /* If start over true, lets do it.  */
          details ++;
          details_index ++;
        } /* End tallying details loop.  */
      ident1 ++;
      ident1_index ++;
    } /* End identifier1 loop.  */
}
