/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU General Public License    *
 *     as published by the Free Software Foundation; either version   *
 *     2 of the License, or (at your option) any later version.       *
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H. Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/* Test inspect converting runtime.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blir_temp_config.h"
#include "blir_inspect.h"

int main (void)
{
  uchar identifier1[100];
  uchar identifier6[20];
  uchar identifier7[20];
  uchar before[2];
  uchar after[2];
  uint32 i;
  struct blir_before_after_details details;

  printf ("Test Inspect Converting....\n");
  printf ("Identifier1:");
  abort ();  /* Don't want any input from file.  Program should have test data.  */
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
      details.before_value_length = strlen ((native_char *)before);
    }
  details.before_value = before;
  if (after[0] == 'X')
    {
      details.after_value_length = 0;
    }
  else
    {
      details.after_value_length = strlen ((native_char *)before);
    }

  details.after_value_length = strlen ((native_char *)after);
  details.after_value = after;
  blir_inspect_converting (identifier1,
                          strlen ((native_char *)identifier1),
                          identifier6,
                          strlen ((native_char *)identifier6),
                          identifier7,
                          strlen ((native_char *)identifier7),
                          1,
                          & details);
  printf ("Results:%s\n", identifier1);
  return 0;
}
    
