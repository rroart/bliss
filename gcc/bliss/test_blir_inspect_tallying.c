/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU General Public            *
 *     License as published by the Free Software Foundation; either   *
 *     version 2 of the License, or (at your option) any later version*
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H.  Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/* Test routine for inspect replacing runtime.  */

#include <string.h>
#include <stdio.h>
#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_inspect.h"

static uint32 identifier2[10];

void test_blir_tally (uchar *identifier1, uint32 identifier1_length,
                       uint32 tallying_count, struct blir_inspect_tallying_struct *tallying_details);

void test_blir_tally (uchar *identifier1,
                      uint32 identifier1_length,
                      uint32 tallying_count,
                      struct blir_inspect_tallying_struct *tallying_details)
{
  uint32 tally_index;
  uint32 all_leading_index;
  uint32 work_index;
  uchar text[100];
  uchar identifier1_copy[100];
  uint32 workarea_detail[300];
  uint32 *workarea[30];  /* Work area for tallying 3*10*10.  */
  struct blir_all_leading_details *all_leading;
  struct blir_inspect_tallying_struct *tallying_details_work;

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
  strncpy ((native_char *)text, (native_char *)identifier1, identifier1_length);
  text[identifier1_length] = '\0';
  printf ("INSPECT %s TALLYING \n", text);
  tally_index = 0;
  tallying_details_work = tallying_details;
  while (tally_index < tallying_count)
    {
      sprintf ((native_char *)text, "COUNT-%2d", tally_index);
      printf ("   %s \n", text);
      if (tallying_details_work->char_flag)
        {
          printf ("      FOR CHARACTERS ");
          if (tallying_details_work->details.characters.after_value_length != 0)
            {
              strncpy ((native_char *)text, (native_char *)tallying_details_work->details.characters.after_value,
                      tallying_details_work->details.characters.after_value_length);
              text[tallying_details_work->details.characters.after_value_length] = '\0';
              printf ("AFTER %s ", text);
            }
          if (tallying_details_work->details.characters.before_value_length != 0)
            {
              strncpy ((native_char *)text, (native_char *)tallying_details_work->details.characters.before_value,
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
              strncpy ((native_char *)text, (native_char *)all_leading->identifier3, 
                     all_leading->identifier3_length);
              text[all_leading->identifier3_length] = '\0';
              printf ("%s ", text);
              if (all_leading->details.after_value_length != 0)
                {
                  strncpy ((native_char *)text, (native_char *)all_leading->details.after_value, 
                          all_leading->details.after_value_length);
                  text[all_leading->details.after_value_length] = '\0';
                  printf ("AFTER %s ", text);
                }
              if (all_leading->details.before_value_length != 0)
                {
                  strncpy ((native_char *)text, (native_char *)all_leading->details.before_value, 
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
      printf ("COUNT-%d : %d\n", tally_index,  * tallying_details_work->identifier2);
      tally_index ++;
      tallying_details_work ++;
    }
  printf ("\n");
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
      {(uchar *)"AB", 2,
       {(uchar *)"BC", 2, (uchar *)"BA", 2, 0, 0}
      }
    };
    struct blir_inspect_tallying_struct tally_details[TEST_TALLY_COUNT] = 
    {
      {NULL /*&Identifier2[0].  */, 
        { /* Union.  */
          {1, NULL /*&All_leading[0].  */, 1}
        } /* End union.  */
        , 0
      }
    };
    
    tally_details[0].identifier2 = & identifier2[0];
    tally_details[0].details.all_leading.details = & (all_leading[0]);

    test_blir_tally ((uchar *)TEST_IDENT1,
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
      {(uchar *)"AB", 2,
       {(uchar *)"BC", 2, (uchar *)"", 0, 0, 0}
      },
      {(uchar *)"B", 1,
       {(uchar *)"", 0, (uchar *)"D", 1, 0, 0}
      }
    };

    struct blir_inspect_tallying_struct tally_details[TEST_TALLY_COUNT] = 
    {
      {NULL /* &Identifier2[1].  */, 
        { /* Union.  */
          {1, NULL /*&All_leading[0].  */, 1}
        } /* End union.  */
        , 0
      },
      {NULL /*&Identifier2[3].  */,
        { /* Union.  */
          {1, NULL /*&All_leading[1].  */, 0}
        }
        , 0
      }
    };

    tally_details[0].identifier2 = & identifier2[1];
    tally_details[1].identifier2 = & identifier2[3];
      
    tally_details[0].details.all_leading.details = & (all_leading[0]);
    tally_details[1].details.all_leading.details = & (all_leading[1]);

    test_blir_tally ((uchar *)TEST_IDENT1,
                    strlen (TEST_IDENT1),
                    TEST_TALLY_COUNT,
                    & tally_details[0]);
  }
  return 0;

}
