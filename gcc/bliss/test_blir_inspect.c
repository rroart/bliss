/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU General Public            *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

#include <string.h>
#include <stdio.h>
#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_inspect.h"

UINT32 identifier2[10];

void test_blir_tally (UINT8 * identifier1, UINT32 identifier1_length,
                       UINT32 tallying_count, struct blir_inspect_tallying_struct *tallying_details);

void test_blir_tally
(
 UINT8 * identifier1,
 UINT32 identifier1_length,
 UINT32 tallying_count,
 struct blir_inspect_tallying_struct *tallying_details)
{
  UINT32 tally_index;
  UINT32 all_leading_index;
  UINT32 work_index;
  UINT8 text[100];
  UINT8 identifier1_copy[100];
  UINT32 workarea_detail[300];
  UINT32 * workarea[30];  /* Work area for tallying 3*10*10.  */

  struct blir_inspect_tallying_struct *tallying_details_work;
  struct blir_all_leading_details *all_leading;

  BLI_ASSERT (tallying_count < 11);
  /* Initialize workarea.  */
  for (work_index = 0; work_index < 10; work_index ++)
    {
      workarea[work_index] = & workarea_detail[work_index * 10];
      workarea[work_index +10] = & workarea_detail[work_index * 10 +100];
      workarea[work_index +20] = & workarea_detail[work_index * 10 +200];
    }
  
  /* Print input.  */
  printf ("Testing inspect tallying\n");
  printf ("Input:\n");
  strncpy (text, identifier1, identifier1_length);
  text[identifier1_length] = '\0';
  printf ("INSPECT %s TALLYING \n", text);
  tally_index = 0;
  tallying_details_work = tallying_details;
  while (tally_index < tallying_count)
    {
      sprintf (text, "COUNT-%2d", tally_index);
      printf ("   %s \n", text);
      if (tallying_details_work->char_flag)
        {
          printf ("      FOR CHARACTERS ");
          if (tallying_details_work->details.characters.after_value_length != 0)
            {
              strncpy (text, tallying_details_work->details.characters.after_value,
                      tallying_details_work->details.characters.after_value_length);
              text[tallying_details_work->details.characters.after_value_length] = '\0';
              printf ("AFTER %s ", text);
            }
          if (tallying_details_work->details.characters.before_value_length != 0)
            {
              strncpy (text, tallying_details_work->details.characters.before_value,
                      tallying_details_work->details.characters.before_value_length);
              text[tallying_details_work->details.characters.before_value_length] = '\0';
              printf ("BEFORE %s ", text);
            }
          printf ("\n");
        }
      else
        {
          all_leading_index = 0;
          all_leading = tallying_details_work->details.all_leading.details;
          while (all_leading_index
                  < tallying_details_work->details.all_leading.all_leading_details_count)
            {
              if (tallying_details_work->details.all_leading.all_flag)
                {
                  printf ("      ALL ");
                }
              else
                {
                  printf ("      LEADING ");
                }
              strncpy (text, all_leading->identifier3, 
                      all_leading->identifier3_length);
              text[all_leading->identifier3_length] = '\0';
              printf ("%s ", text);
              if (all_leading->details.after_value_length != 0)
                {
                  strncpy (text, all_leading->details.after_value, 
                          all_leading->details.after_value_length);
                  text[all_leading->details.after_value_length] = '\0';
                  printf ("AFTER %s ", text);
                }
              if (all_leading->details.before_value_length != 0)
                {
                  strncpy (text, all_leading->details.before_value, 
                          all_leading->details.before_value_length);
                  text[all_leading->details.before_value_length] = '\0';
                  printf ("BEFORE %s ", text);
                }
              printf ("\n");
              all_leading_index ++;
              all_leading ++;
            }
        }
      tally_index ++;
      tallying_details_work ++;
    }
  memcpy (identifier1_copy, identifier1, identifier1_length);
  blir_inspect_tallying (identifier1_copy,
                        identifier1_length,
                        tallying_count,
                        tallying_details,
                        workarea,
                        workarea +10,
                        workarea +20);
  tally_index = 0;
  printf ("Results:\n");
  tallying_details_work = tallying_details;
  while (tally_index < tallying_count)
    {
      printf ("COUNT-%2d : %d\n", tally_index,  * tallying_details_work->identifier2);
      tally_index ++;
      tallying_details_work ++;
    }
  return;
}

int
main (void)
{
  /*================================================================*
   *      TEST 1                                                    *
   *================================================================.  */
  {
#undef TEST_IDENT1
#undef TEST_TALLY_COUNT
#undef TEST_ALL_LEADING_COUNT

#define TEST_IDENT1 "ABABABABC"
#define TEST_TALLY_COUNT 1
#define TEST_ALL_LEADING_COUNT 1
    

    struct blir_all_leading_details all_leading[TEST_ALL_LEADING_COUNT] =
    {
      {"AB", 2,
       {"BC", 2, "BA", 2, 0, 0}
      }
    };
    struct blir_inspect_tallying_struct tally_details[TEST_TALLY_COUNT] = 
    {
      { & identifier2[0], 
        { /* Union.  */
          {1,  & all_leading[0], 1}
        } /* End union.  */
        , 0
      }
    };

    test_blir_tally (TEST_IDENT1,
                    strlen (TEST_IDENT1),
                    TEST_TALLY_COUNT,
                    & tally_details[0]);
  }
  /*================================================================*
   *      TEST 2                                                   *
   *================================================================.  */
  {
#undef TEST_IDENT1
#undef TEST_TALLY_COUNT
#undef TEST_ALL_LEADING_COUNT

#define TEST_IDENT1 "BBEABDABABBCABEE"
#define TEST_TALLY_COUNT 2
#define TEST_ALL_LEADING_COUNT 1
    

    struct blir_all_leading_details all_leading[TEST_ALL_LEADING_COUNT * TEST_TALLY_COUNT] =
    {
      {"AB", 2,
       {"BC", 2, "", 0, 0, 0}
      },
      {"B", 1,
       {"", 0, "D", 1, 0, 0}
      }
    };
    struct blir_inspect_tallying_struct tally_details[TEST_TALLY_COUNT] = 
    {
      { & identifier2[1], 
        { /* Union.  */
          {1,  & all_leading[0], 1}
        } /* End union.  */
        , 0
      },
      { & identifier2[3],
        { /* Union.  */
          {1,  & all_leading[1], 0}
        }
        , 0
      }
    };

    test_blir_tally (TEST_IDENT1,
                    strlen (TEST_IDENT1),
                    TEST_TALLY_COUNT,
                    & tally_details[0]);
  }
  return 0;

}
