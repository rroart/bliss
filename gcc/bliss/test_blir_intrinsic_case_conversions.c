/***********************************************************************
**
**  Test_blir_intrinsic_case_conversions.c
**
**********************************************************************  */

/*                                                                    *
 *        BLISS intrinsic case conversions routine test driver        *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Curt Timmerman               *
 *								      *
 * 	     Modified by Linga Ramakrishna  on 27/Aug/2001	      *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of the *
 * License, or (at your option) any later version.                    *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
 * 02111-1307 USA                                                     *
 *                                                                    *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "blir_temp_config.h"
#include "blir_intrinsic_case_conversions.h"

#define FIELD_LENGTH_MAX    500

static const uchar *upper_case = (uchar *) "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
static const uchar *lower_case = (uchar *) "abcdefghijklmnopqrstuvwxyz" ;

int 
test_case_conversions
(const char *test_id,
 uchar * from_field,
 uint32 length,
 uint16 case_conversion_type) ;

/*---------------------------------------------------------------------
**  test_case_conversions
**--------------------------------------------------------------------.  */
int 
test_case_conversions
(const char *test_id,
 uchar *from_field,
 uint32 length,
 uint16 case_conversion_type)
{
  static uchar test_pattern[] = {0277, 0377, 0177, 0277};
  uchar to_field[FIELD_LENGTH_MAX + (2 * sizeof (test_pattern)) + 1000];
  uchar *to_ptr;
  uint32 counter;
  const uchar *upper_ptr;
  const uchar *lower_ptr;
  uchar expected_field[FIELD_LENGTH_MAX];

  /*--------------------------------------------
  **  Build test pattern prior to result field
  **------------------------------------------.  */
  to_ptr = to_field;

  for (counter = 0;
       counter < sizeof (test_pattern);
       counter ++, to_ptr ++)
    {
      * to_ptr = test_pattern[counter];
    }

  /*--------------------------------------
  **  Clear to field, build expected result
  **------------------------------------.  */
  for (counter = 0;
       counter < length;
       counter ++, to_ptr ++)
    {
      * to_ptr = counter;

    /*--------------------------------------
    **  Build the  expected result for lower case
    **----------------------------------------*/
 	
      if (case_conversion_type == TO_LOWER)
     	{
	   for (upper_ptr = upper_case, lower_ptr = lower_case;
                * upper_ptr;
           	upper_ptr ++, lower_ptr ++)
            {
          	if ( * upper_ptr == from_field[counter])
	        break;
            }
	   if ( * upper_ptr)
	     {
          	expected_field[counter] =  * lower_ptr;
             }
      	   else
             {
         	expected_field[counter] = from_field[counter];
             }
        }

    /*--------------------------------------
    **  Build the  expected result for upper case
    **----------------------------------------*/
      if (case_conversion_type == TO_UPPER)
	{
	   for (upper_ptr = upper_case, lower_ptr = lower_case;
                * lower_ptr;
                upper_ptr ++, lower_ptr ++)
            {
                if ( * lower_ptr == from_field[counter])
                break;
            }
      	   if ( * lower_ptr)
             {
          	expected_field[counter] =  * upper_ptr;
             }
      	   else
             {
          	expected_field[counter] = from_field[counter];
             }
        }
     }  

  /*---------------------------------------------
  **  Build test pattern following result field
  **-------------------------------------------.  */
  for (counter = 0;
       counter < sizeof (test_pattern);
       counter ++, to_ptr ++)
    {
      * to_ptr = test_pattern[counter];
    }


  /*--------------------------------------
  **  Call case conversions intrinsic
  **------------------------------------.  */
  blir_intrinsic_case_conversions
    (from_field,
     & to_field[sizeof (test_pattern)],
     length,
     case_conversion_type);

  to_ptr = to_field;

  /*------------------------------------------------
  **  Examine test pattern prior to result field
  **----------------------------------------------.  */
  for (counter = 0;
       counter < sizeof (test_pattern);
       counter ++, to_ptr ++)
    {
      if ( * to_ptr != test_pattern[counter])
        {
          fprintf (stderr, "Corrupted lower result for test: %s\n", test_id);
          fprintf (stderr, "    Offset %d\n", counter);
          return (1);
        }
    }

  /*--------------------------------------------
  **  Examine actual result to expected result
  **------------------------------------------.  */
  for (counter = 0;
       counter < length;
       counter ++, to_ptr ++)
    {
      if ( * to_ptr != expected_field[counter])
        {
          fprintf (stderr, "Result mismatch for test: %s\n", test_id);
          fprintf (stderr, "    Offset %d\n", counter);
          return (1);
        }
    }

  /*--------------------------------------------
  **  Examine test pattern following result field
  **------------------------------------------.  */
  for (counter = 0;
       counter < sizeof (test_pattern);
       counter ++, to_ptr ++)
    {
      if ( * to_ptr != test_pattern[counter])
        {
          fprintf (stderr, "Corrupted upper result for test: %s\n", test_id);
          fprintf (stderr, "    Offset %d\n", counter);
          return (1);
        }
    }


  return (0);  /* OK result.  */

} /* End test_case_conversions.  */

/*---------------------------------------------------------------------
**  Main
**--------------------------------------------------------------------.  */
int 
main (void)
{
  int32 result = 0;
  uchar from_field[FIELD_LENGTH_MAX];
  uint32 idx;
  uint16 case_conversion_type;
  uint32 char_idx;

  /* Try convert to upper first.  */
  case_conversion_type = TO_UPPER; 

  printf ("\nTesting blir_intrinsic_case_conversions - UPPER\n");

  /*--------------------------------------
  **  Test simple string tranlation.
  **------------------------------------.  */
  printf ("\nSimple translation\n");
  strcpy ((char *)from_field, "Now Is The Time For All Good People");
  if (test_case_conversions
      ("Simple translation",
       from_field,
       strlen ((char *)from_field) + 1,
       case_conversion_type))
    {
      fprintf (stderr, "Failure\n");
      result = 1;
    }

  /*--------------------------------------
  **  Test all 1 character tranlations.
  **------------------------------------.  */
  printf ("\nSingle character translation (values 0-255)\n");
  for (idx = 0; idx < 256; idx ++)
    {
      from_field[0] = idx;
      if (test_case_conversions
          ("Single character translation",
           from_field,
           1,
	   TO_LOWER))
        {
          fprintf (stderr, "Failure\n");
          result = 1;
          break;
        }
    }

  /* Now try convert to lower.  */
  case_conversion_type = TO_LOWER; 

  printf ("\nTesting blir_intrinsic_case_conversions - LOWER\n");

  /*--------------------------------------
  **  Test simple string tranlation.
  **------------------------------------.  */
  printf ("\nSimple translation\n");
  strcpy ((char *)from_field, "Now Is The Time For All Good People");
  if (test_case_conversions
      ("Simple translation",
       from_field,
       strlen ((char *)from_field) + 1,
       case_conversion_type))
    {
      fprintf (stderr, "Failure\n");
      result = 1;
    }

  /*--------------------------------------
  **  Test all 1 character tranlations.
  **------------------------------------.  */
  printf ("\nSingle character translation (values 0-255)\n");
  for (idx = 0; idx < 256; idx ++)
    {
      from_field[0] = idx;
      if (test_case_conversions
          ("Single character translation",
           from_field,
           1,
	   TO_LOWER))
        {
          fprintf (stderr, "Failure\n");
          result = 1;
          break;
        }
    }

  /*--------------------------------------
  **  Test lower case only translation
  **------------------------------------.  */
  printf ("\nAll lower case (length = %d)\n", FIELD_LENGTH_MAX);
  for (idx = 0, char_idx = 0; idx < FIELD_LENGTH_MAX; idx ++, char_idx ++)
    {
      if (lower_case[char_idx] == 0)
        {
          char_idx = 0;
        }
      from_field[idx] = lower_case[char_idx];
    }
  if (test_case_conversions
      ("All lower case translation",
       from_field,
       FIELD_LENGTH_MAX,
       TO_LOWER))
    {
      fprintf (stderr, "Failure\n");
      result = 1;
    }

  /*--------------------------------------
  **  Test upper case only tranlation
  **------------------------------------.  */
  printf ("\nAll upper case (length = %d)\n", FIELD_LENGTH_MAX);
  for (idx = 0, char_idx = 0; idx < FIELD_LENGTH_MAX; idx ++, char_idx ++)
    {
      if (lower_case[char_idx] == 0)
        {
          char_idx = 0;
        }
      from_field[idx] = upper_case[char_idx];
    }
  if (test_case_conversions
      ("All upper case translation",
       from_field,
       FIELD_LENGTH_MAX,
       TO_UPPER))
    {
      fprintf (stderr, "Failure\n");
      result = 1;
    }

  /*--------------------------------------
  **  Test all 8 bit characters
  **------------------------------------.  */
  printf ("\nAll characters (length = %d)\n", FIELD_LENGTH_MAX);
  for (idx = 0; idx < FIELD_LENGTH_MAX; idx ++)
    {
      from_field[idx] = idx;
    }
  if (test_case_conversions
      ("All characters translation",
       from_field,
       FIELD_LENGTH_MAX,
       TO_LOWER))
    {
      fprintf (stderr, "Failure\n");
      result = 1;
    }

  if (result != 0)
    {
      abort ();  /* Failure exit.  */
    }

  printf ("\nAll tests completed OK\n");
  return (0);  /* OK exit.  */

} /* End main.  */

