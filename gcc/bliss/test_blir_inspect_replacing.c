/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU General Public License    *
 *     as published by the Free Software Foundation; either version   *
 *     2 of the License, or (at your option) any later version.       *
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H. Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/*====================================================================*
 *      Test program for inspect replacing                            *
 *====================================================================.  */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_inspect.h"

/* Data structures comprising the test inputs.  */

#define TEST_COUNT 3 /* # tests */
#define MAX_CLAUSES 4 /* max clauses in a test */

static uint32 replace_count[TEST_COUNT];

static uchar const * identifier1[TEST_COUNT] =
{(uchar *)"BABABC", (uchar *)"BBEABDABABBCABEE", (uchar *)"ABAABX"};

static uchar *identifier1_copy = NULL;

static uchar const * identifier3[TEST_COUNT][MAX_CLAUSES]
= {{(uchar *)"AB", (uchar *)"BC"}, {(uchar *)"AB", (uchar *)"B", (uchar *)"E", NULL}, {NULL, (uchar *)"A"}};

static uchar const * identifier5[TEST_COUNT][MAX_CLAUSES]
= {{(uchar *)"XY", (uchar *)"VW"}, {(uchar *)"XY", (uchar *)"W", (uchar *)"V", (uchar *)"Z"}, {(uchar *)"Z", (uchar *)"W"}};

static uchar const * before[TEST_COUNT][MAX_CLAUSES]
= {{NULL}, {(uchar *)"BC", NULL, NULL, (uchar *)"C"}, {NULL}};

static uchar const * after[TEST_COUNT][MAX_CLAUSES]
= {{NULL}, {NULL, (uchar *)"D", (uchar *)"D", (uchar *)"A"}, {NULL}};

static enum blir_replacing_type_enum type[TEST_COUNT][MAX_CLAUSES]
= {{blir_replacing_type_all, blir_replacing_type_all},
  {blir_replacing_type_all, blir_replacing_type_leading,
   blir_replacing_type_first, blir_replacing_type_characters}, 
  {blir_replacing_type_characters, blir_replacing_type_all}};

static struct blir_inspect_replacing_struct replace_details[TEST_COUNT][MAX_CLAUSES];

/* Outputs.  */
uint32 replace_status[MAX_CLAUSES];

/* Work.  */
uint32 after_bound[MAX_CLAUSES];
uint32 before_bound[MAX_CLAUSES];

void
test_blir_replace_fill_struct (uint32 test,
                               uint32 replace_index);
void
test_blir_replace_fill (uint32 test);

void
test_blir_replace (uchar *identifier1,
                   uint32 identifier1_length,
                   uint32 replacing_count,
                   struct blir_inspect_replacing_struct *replacing_details,
                   uint32 *replace_status, 
                   uint32 *after_bound,
                   uint32 *before_bound);


/* Fill the necesary strings for each test.  */

void
test_blir_replace_fill (uint32 test /* Test index to be performed.  */)
{
  uint32 clause;
  int32 len;

  len = strlen ((native_char *)identifier1[test]);
  identifier1_copy = malloc (len);
  BLI_ASSERT (identifier1_copy);
  memcpy (identifier1_copy, identifier1[test], len);
  
  for (clause = 0; clause < MAX_CLAUSES; clause ++)
    {
      if (!type[test][clause])
        break;
      test_blir_replace_fill_struct (test, clause);
      replace_count[test] = clause +1;
    }
}
           

void
test_blir_replace_fill_struct (uint32 test,  /* Test to be filled.  */
                               uint32 replace_index /* Replacing index to be filled.  */)
{
  
  memset ( & (replace_details[test][replace_index]), 0, sizeof (replace_details[test][replace_index]));

  if (identifier3[test][replace_index])
    {
      replace_details[test][replace_index].identifier3 = (uchar *)identifier3[test][replace_index];
      replace_details[test][replace_index].identifier3_length
        = strlen ((native_char *)identifier3[test][replace_index]);
    }
  
  if (identifier5[test][replace_index])
    {
      replace_details[test][replace_index].identifier5 = (uchar *)identifier5[test][replace_index];
      replace_details[test][replace_index].identifier5_length
        = strlen ((native_char *)identifier5[test][replace_index]);
    }

  if (after[test][replace_index])
    {
      replace_details[test][replace_index].details.after_value = (uchar *)after[test][replace_index];
      replace_details[test][replace_index].details.after_value_length
        = strlen ((native_char *)after[test][replace_index]);
    }
  
  if (before[test][replace_index])
    {
      replace_details[test][replace_index].details.before_value = (uchar *)before[test][replace_index];
      replace_details[test][replace_index].details.before_value_length
        = strlen ((native_char *)before[test][replace_index]);
    }

  replace_details[test][replace_index].type = type[test][replace_index];

}

void
test_blir_replace
(uchar *identifier1,
 uint32 identifier1_length,  /* Identifier 1 below, not null terminated.  */
 uint32 replacing_count,  /* Number of replacing details.  */
 struct blir_inspect_replacing_struct *replacing_details,  /* Array of replacing details.  */
 uint32 *replace_status,  /* Vector to store status of replacement tasks.  */
 /* Values are : 1 --> initial value means no replacements have done.  */
 /*              2 --> Replacement has been initiated.  */
 /*              0 --> Replacement is finished.  */
 uint32 *after_bound,  /* Vector store index of after position for each replacement.  */
 uint32 *before_bound /* Vector store index of before position for each replacement.  */
 /* All this three, upper defined vectors occurs replacing_count times.  */)
{
  uint32 replace_index;
  uchar text[100];
  struct blir_inspect_replacing_struct *replace_detail;

  /* Print input.  */
  printf ("Testing inspect replacing\n");
  printf ("Input:\n");
  strncpy ((native_char *)text, (native_char *)identifier1, identifier1_length);
  text[identifier1_length] = '\0';
  printf ("INSPECT %s REPLACING \n", text);
  replace_index = 0;
  replace_detail = replacing_details;
  while (replace_index < replacing_count)
    {
      if (replace_detail->type == blir_replacing_type_characters)
        {
          strncpy ((native_char *)text, (native_char *)replace_detail->identifier5,
                  replace_detail->identifier5_length);
          text[replace_detail->identifier5_length] = '\0';
          printf ("  CHARACTERS BY %s \n", text);
          if (replace_detail->details.after_value_length != 0)
            {
              strncpy ((native_char *)text, (native_char *)replace_detail->details.after_value,
                      replace_detail->details.after_value_length);
              text[replace_detail->details.after_value_length] = '\0';
              printf ("    AFTER %s \n", text);
            }
          if (replace_detail->details.before_value_length != 0)
            {
              strncpy ((native_char *)text, (native_char *)replace_detail->details.before_value,
                      replace_detail->details.before_value_length);
              text[replace_detail->details.before_value_length] = '\0';
              printf ("    BEFORE %s \n", text);
            }
        }
      else
        {
          if (replace_detail->type == blir_replacing_type_all)
            {
              printf ("   ALL ");
            }
          if (replace_detail->type == blir_replacing_type_leading)
            {
              printf ("   LEADING ");
            }
          if (replace_detail->type == blir_replacing_type_first)
            {
              printf ("   FIRST ");
            }
          strncpy ((native_char *)text, (native_char *)replace_detail->identifier3,
                  replace_detail->identifier3_length);
          text[replace_detail->identifier3_length] = '\0';
          printf ("%s BY ", text);
          strncpy ((native_char *)text, (native_char *)replace_detail->identifier5,
                  replace_detail->identifier5_length);
          text[replace_detail->identifier5_length] = '\0';
          printf ("%s ", text);
          if (replace_detail->details.after_value_length != 0)
            {
              strncpy ((native_char *)text, (native_char *)replace_detail->details.after_value,
                      replace_detail->details.after_value_length);
              text[replace_detail->details.after_value_length] = '\0';
              printf (" AFTER %s ", text);
            }
          if (replace_detail->details.before_value_length != 0)
            {
              strncpy ((native_char *)text, (native_char *)replace_detail->details.before_value,
                      replace_detail->details.before_value_length);
              text[replace_detail->details.before_value_length] = '\0';
              printf (" BEFORE %s ", text);
            }
          printf ("\n");
        }
      replace_detail ++;
      replace_index ++;
    }
  /* Call replacing function.  */
  blir_inspect_replacing (identifier1,
                         identifier1_length,
                         replacing_count,
                         replacing_details,
                         replace_status,
                         after_bound,
                         before_bound);

  BLI_ASSERT (identifier1_length < 100);
  strncpy ((native_char *)text, (native_char *)identifier1, identifier1_length);
  text[identifier1_length] = '\0';
  printf ("Output : %s \n \n", text);
  for (replace_index = 0; replace_index < replacing_count; replace_index ++)
    printf ("status %u = %u\n", replace_index, replace_status[replace_index]);
  printf ("\n");
  return;
}

int
main (void)
{
  uint32 test;
  
  for (test = 0; test < TEST_COUNT; test ++)
    {
      test_blir_replace_fill (test);
      test_blir_replace ((uchar *)identifier1_copy,
                        strlen ((native_char *)identifier1_copy),
                        replace_count[test],
                        & (replace_details[test][0]),
                        replace_status,
                        after_bound,
                        before_bound);
      free (identifier1_copy);
    }
  return 0;
}
