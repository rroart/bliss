/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU General Public License    *
 *     as published by the Free Software Foundation; either version   *
 *     2 of the License, or (at your option) any later version.       *
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H. Ardison  dardison@hotmail.com    *
 *====================================================================.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blir_inspect.h"

void main (void)
{
  char identifier1[100];
  char identifier3[10][2];
  char identifier5[10][2];
  char before[10][2];
  char after[10][2];
  char all_leading_first[2];
  unsigned int replace_count;
  int process_rep = 1;
  int i;
  struct blir_inspect_replacing_struct replace_details[10],  * rd;
  struct blir_before_after_details details[10];
        
  rd = & replace_details[0];
  printf ("Test Inspect Replacing....\n");
  printf ("Identifier1:");
  i = scanf ("%s", identifier1);
  replace_count = 0;
  while (process_rep)
    {
      if (replace_count == 10)
        {
          break;
        }
      printf ("All[A] Characters[C] Leading[L] First[F] exit[X]:");
      i = scanf ("%s", all_leading_first);
      if (all_leading_first[0] == 'X')
        {
          break;
        }
      if (all_leading_first[0] != 'C')
        {
          printf ("\nIdentifier3:");
          i = scanf ("%s", identifier3[replace_count]);
        }
      else
        {
          strcpy (identifier3[replace_count], " ");
        }
      printf ("\nIdentifier5:");
      i = scanf ("%s", identifier5[replace_count]);
      printf ("\nBefore[X=nothing]:");
      i = scanf ("%s", before[replace_count]);
      printf ("\nAfter[X=nothing]:");
      i = scanf ("%s", after[replace_count]);
      if (before[replace_count][0] == 'X')
        {
          details[replace_count].before_value_length = 0;
        }
      else
        {
          details[replace_count].before_value_length = strlen (before[replace_count]);
        }
      details[replace_count].before_value = before[replace_count];
      if (after[replace_count][0] == 'X')
        {
          details[replace_count].after_value_length = 0;
        }
      else
        {
          details[replace_count].after_value_length = strlen (before[replace_count]);
        }
      details[replace_count].after_value = after[replace_count];
      replace_details[replace_count].identifier3 = identifier3[replace_count];
      replace_details[replace_count].identifier3_length = strlen (identifier3[replace_count]);
      replace_details[replace_count].identifier5 = identifier5[replace_count];
      replace_details[replace_count].identifier5_length = strlen (identifier5[replace_count]);
      replace_details[replace_count].details.after_value = details[replace_count].after_value;
      replace_details[replace_count].details.after_value_length = details[replace_count].after_value_length;
      replace_details[replace_count].details.before_value = details[replace_count].before_value;
      replace_details[replace_count].details.before_value_length = details[replace_count].before_value_length;
      if (all_leading_first[0] == 'A')
        {
          replace_details[replace_count].type = blir_replacing_type_all;
        }
      if (all_leading_first[0] == 'L')
        {
          replace_details[replace_count].type = blir_replacing_type_leading;
        }
      if (all_leading_first[0] == 'F')
        {
          replace_details[replace_count].type = blir_replacing_type_first;
        }
      if (all_leading_first[0] == 'C')
        {
          replace_details[replace_count].type = blir_replacing_type_characters;
        }
      replace_count ++;
    }
  blir_inspect_replacing (identifier1,
                         strlen (identifier1),
                         replace_count,
                         rd);
  printf ("Results:%s\n", identifier1);
  return;
}
