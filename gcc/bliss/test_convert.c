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
  char identifier6[20];
  char identifier7[20];
  char before[2];
  char after[2];
  int i;
  struct blir_before_after_details details;

  printf ("Test Inspect Converting....\n");
  printf ("Identifier1:");
  i = scanf ("%s", identifier1);
  printf ("\nIdentifier6:");
  i = scanf ("%s", identifier6);
  printf ("\nIdentifier7:");
  i = scanf ("%s", identifier7);
  printf ("\nBefore:");
  i = scanf ("%s", before);
  printf ("\nAfter:");
  i = scanf ("%s", after);
  if (before[0] == 'X')
    {
      details.before_value_length = 0;
    }
  else
    {
      details.before_value_length = strlen (before);
    }
  details.before_value = before;
  if (after[0] == 'X')
    {
      details.after_value_length = 0;
    }
  else
    {
      details.after_value_length = strlen (before);
    }

  details.after_value_length = strlen (after);
  details.after_value = after;
  blir_inspect_converting (identifier1,
                          strlen (identifier1),
                          identifier6,
                          strlen (identifier6),
                          identifier7,
                          strlen (identifier7),
                          1,
                          & details);
  printf ("Results:%s\n", identifier1);
  return;
}
    
