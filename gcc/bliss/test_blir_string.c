/**********************************************************************
 *                                                                    *
 *    Basic  Test Driver for BLISS STRING Statement                   *
 *                                                                    *
 *               Copyright (C) 2000, 2001 by Steven O.  Ellis                *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License        *
 * as published by the Free Software Foundation; either version 2     *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public          *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  *
 * 02111-1307 USA                                                     *
 *                                                                    *
 * Mail:           Steven O. Ellis                                    *
 *                 1 Squire Ct.                                       *
 *                 Urbana, IL 61802                                   *
 *                                                                    *
 * E-mail:         soellis@soltec.net                                 *
 *                                                                    *
 *********************************************************************  */

/*

Notes: When a figurative constant is used as the delimiter, 
        it is a single character nonnumeric literal.

       It was originally considered to add a checks in blir_string.c
        to ensure that the number of from items being passed was
	equal to the sum of the items each delimiter controls.  This
	would require an extra parameter to be passed (number of
	from items), which is not otherwise needed.  The check was
	deemed unnecessary because the runtime will be checking
	parameters, and the processing is ultimately dictated by
	the delimiters anyway.  Another check considered was to
	ensure that the pointer value was not greater than the
	length of the target item, but this is simply an OVERFLOW
	condition, per the standard, so an extra check is not needed
	(just return 1).  */

#include "blir_temp_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "blir_string.h" 

/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */

/**********************************************************************
 * Test01 - Test single delimiter: SIZE                               *
 *********************************************************************  */

int32 test01 (void);

/**********************************************************************
 * Test02 - Test single delimiter: null character '\0'                *
 *********************************************************************  */

int32 test02 (void);

/**********************************************************************
 * Test03 - Test single delimiter: linefeed character '\n'            *
 *********************************************************************  */

int32 test03 (void);

/**********************************************************************
 * Test04 - Test single delimiter: single space ' '                   *
 *********************************************************************  */

int32 test04 (void);

/**********************************************************************
 * Test05 - Test single delimiter: zero (0)                           *
 *********************************************************************  */

int32 test05 (void);

/**********************************************************************
 * Test06 - Test single delimiter: 'x'                                *
 *********************************************************************  */

int32 test06 (void);

/**********************************************************************
 * Test07 - Test single delimiter: 'XA'                               *
 *********************************************************************  */

int32 test07 (void);

/**********************************************************************
 * Test08 - Test single delimiter: 'XI'                               *
 *********************************************************************  */

int32 test08 (void);

/**********************************************************************
 * Test09 - Test single delimiter: 'ABRA'                             *
 *********************************************************************  */

int32 test09 (void);

/**********************************************************************
 * Test10 - Test single delimiter: 'CADAB'                            *
 *********************************************************************  */

int32 test10 (void);

/**********************************************************************
 * Test11 - Test multiple delimiters                                  *
 *********************************************************************  */

int32 test11 (void);

/**********************************************************************
 * Test12 - Test multiple delimiters acting on multiple items         *
 *********************************************************************  */

int32 test12 (void);

/**********************************************************************
 * Test13 - Test pointer offset of 0 (underflow)                      *
 *********************************************************************  */

int32 test13 (void);

/**********************************************************************
 * Test14 - Test pointer offset is 1 greater than length of target    *
 *          field (overflow)                                          *
 *********************************************************************  */

int32 test14 (void);

/**********************************************************************
 * Test15 - Test pointer offset is much greater than length of target *
 *          field (overflow)                                          *
 *********************************************************************  */

int32 test15 (void);

/**********************************************************************
 * Test16 - Test pointer offset is greater than 1 but less than the   *
 *          length of the target area                                 *
 *********************************************************************  */

int32 test16 (void);

/**********************************************************************
 * Test17 - Test pointer offset is equal to the length of the target  *
 *          area                                                      *
 *********************************************************************  */

int32 test17 (void);

/**********************************************************************
 * Test18 - Number of characters to be moved is less than the length  *
 *          of the target area                                        *
 *********************************************************************  */

int32 test18 (void);

/**********************************************************************
 * Test19 - Number of characters to be moved is equal to the length   *
 *          of the target area                                        *
 *********************************************************************  */

int32 test19 (void);

/**********************************************************************
 * Test20 - Number of characters to be moved is greater than the      *
 *          length of the target area                                 *
 *********************************************************************  */

int32 test20 (void);

/**********************************************************************
 * Test21 - Characters to be moved include string end characters      *
 *          ('\0')                                                    *
 *********************************************************************  */

int32 test21 (void);

/**********************************************************************
 * Test22 - Each item to be moved ends in a string end character      *
 *          (\0') which should be included                            *
 *********************************************************************  */

int32 test22 (void);

/**********************************************************************
 * Test23 - Each item to be moved ends in a string end character, as  *
 *          above, but target area all string end characters          *
 *********************************************************************  */

int32 test23 (void);

/**********************************************************************
 * Test24 - Characters to be moved include line feed characters       *
 *          ('\0')                                                    *
 *********************************************************************  */

int32 test24 (void);

/**********************************************************************
 * Test99 - Test several types of delimiters                          *
 *********************************************************************  */

int32 test99 (void);


/**********************************************************************
 *                                                                    *
 * Test01 - Test delimiter SIZE                                       *
 *                                                                    *
 *********************************************************************  */

int32
test01 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";
  static uchar item_2[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing with delimiter = SIZE:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}

/**********************************************************************
 *                                                                    *
 * Test02 - Test delimiter '\0'                                       *
 *                                                                    *
 *********************************************************************  */

int32
test02 (void)
{
  uint32 i;
  
  static uchar item_0[6] = "ABRARA";
  static uchar item_1[2] = "CA";
  static uchar item_2[8] = "DABRABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {1};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;

  delimiter_0 = (uchar *)malloc (1);
 
  * delimiter_0 = '\0';

  delimiter_array[0] = delimiter_0;

/*--------------------------------------------------------------------*
 * Insert '\0' into strings for testing                               *
 *--------------------------------------------------------------------.  */
  item_0[4] = '\0';
  item_2[5] = '\0';


  printf ("\n");
  printf ("Testing with delimiter = null character:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test03 - Test delimiter '\n'                                       *
 *                                                                    *
 *********************************************************************  */

int32
test03 (void)
{
  uint32 i;
  
  static uchar item_0[6] = "ABRARA";
  static uchar item_1[2] = "CA";
  static uchar item_2[8] = "DABRABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {1};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;

  delimiter_0 = (uchar *)malloc (1);
 
  * delimiter_0 = '\n';

  delimiter_array[0] = delimiter_0;

/*--------------------------------------------------------------------*
 * Insert '\n' into strings for testing                               *
 *--------------------------------------------------------------------.  */
  item_0[4] = '\n';
  item_2[5] = '\n';


  printf ("\n");
  printf ("Testing with delimiter = line feed:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test04 - Test delimiter ' '                                        *
 *                                                                    *
 *********************************************************************  */

int32
test04 (void)
{
  uint32 i;
  
  static uchar item_0[6] = "ABRARA";
  static uchar item_1[2] = "CA";
  static uchar item_2[8] = "DABRABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {1};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;

  delimiter_0 = (uchar *)malloc (1);
 
  * delimiter_0 = ' ';

  delimiter_array[0] = delimiter_0;

/*--------------------------------------------------------------------*
 * Insert ' ' into strings for testing                                *
 *--------------------------------------------------------------------.  */
  item_0[4] = ' ';
  item_2[5] = ' ';


  printf ("\n");
  printf ("Testing with delimiter = single space:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test05 - Test delimiter zero (0)                                   *
 *                                                                    *
 *********************************************************************  */

int32
test05 (void)
{
  uint32 i;
  
  static uchar item_0[6] = "ABRARA";
  static uchar item_1[2] = "CA";
  static uchar item_2[8] = "DABRABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {1};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;

  delimiter_0 = (uchar *)malloc (1);
 
  * delimiter_0 = '0';

  delimiter_array[0] = delimiter_0;

/*--------------------------------------------------------------------*
 * Insert '0' into strings for testing                               *
 *--------------------------------------------------------------------.  */
  item_0[4] = '0';
  item_2[5] = '0';


  printf ("\n");
  printf ("Testing with delimiter = '0' (zero):\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test06 - Test delimiter 'x'                                        *
 *                                                                    *
 *********************************************************************  */

int32
test06 (void)
{
  uint32 i;
  
  static uchar item_0[6] = "abraxa";
  static uchar item_1[2] = "ca";
  static uchar item_2[8] = "dabraxra";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "abracadabra";
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {1};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;

  delimiter_0 = (uchar *)malloc (1);
 
  * delimiter_0 = 'x';

  delimiter_array[0] = delimiter_0;

  printf ("\n");
  printf ("Testing with delimiter = 'x':\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test07 - Test delimiter 'XA'                                       *
 *                                                                    *
 *********************************************************************  */

int32
test07 (void)
{
  uint32 i;
  
  static uchar item_0[10] = "ABRAXILAXA";
  static uchar item_1[2] = "CA";
  static uchar item_2[9] = "DABRAXILA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[21] = "ABRAXILACADABRAXILAXX";
 
  uchar to_item[21] = "XXXXXXXXXXXXXXXXXXXXX";
 
  uint32 to_item_length = 21;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {2};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar delimiter_0[2] = "XA";

  delimiter_array[0] = delimiter_0;

  printf ("\n");
  printf ("Testing with delimiter = 'XA':\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test08 - Test delimiter 'XI'                                       *
 *                                                                    *
 *********************************************************************  */

int32
test08 (void)
{
  uint32 i;
  
  static uchar item_0[10] = "ABRAXILAXI";
  static uchar item_1[2] = "CA";
  static uchar item_2[9] = "DABRAXILA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[21] = "ABRACADABRAXXXXXXXXXX";
 
  uchar to_item[21] = "XXXXXXXXXXXXXXXXXXXXX";
 
  uint32 to_item_length = 21;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {2};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar delimiter_0[2] = "XI";

  delimiter_array[0] = delimiter_0;

  printf ("\n");
  printf ("Testing with delimiter = 'XI':\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test09 - Test delimiter 'ABRA'                                     *
 *                                                                    *
 *********************************************************************  */

int32
test09 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[3] = "CAD";
  static uchar item_2[6] = "ABRALA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar objective_to_item[11] = "CAD        ";
 
  uchar to_item[11] = "           ";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {4};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar delimiter_0[4] = "ABRA";

  delimiter_array[0] = delimiter_0;

  printf ("\n");
  printf ("Testing with delimiter = 'ABRA':\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test10 - Test delimiter 'CADAB'                                    *
 *                                                                    *
 *********************************************************************  */

int32
test10 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[3] = "CAD";
  static uchar item_2[13] = "ABRACADABABRA";
  static uchar item_3[9] = "CADABABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2,
    item_3
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2),
				      sizeof (item_3)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {5};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {4};
 
  uchar delimiter_0[5] = "CADAB";

  delimiter_array[0] = delimiter_0;

  printf ("\n");
  printf ("Testing with delimiter = 'CADAB':\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test11 - Test multiple delimiters                                  *
 *                                                                    *
 *********************************************************************  */

int32
test11 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[7] = "CAD HEX";
  static uchar item_2[2] = "AB";
  static uchar item_3[2] = "RA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2,
    item_3
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2),
				      sizeof (item_3)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 4;
 
  uchar *delimiter_array[4];
 
  uint32 delimiter_length_array[] = {0, 1, 0, 0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {1, 1, 1, 1};

  uchar *delimiter_0;
  uchar *delimiter_1;
  uchar *delimiter_2;
  uchar *delimiter_3;
 
  delimiter_0 = NULL;

  delimiter_1 = (uchar *)malloc (1);

  * delimiter_1 = ' ';

  delimiter_2 = NULL;

  delimiter_3 = NULL;

  delimiter_array[0] = delimiter_0;
  delimiter_array[1] = delimiter_1;
  delimiter_array[2] = delimiter_2;
  delimiter_array[3] = delimiter_3;

  printf ("\n");
  printf ("Testing with multiple delimeters:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test12 - Test multiple delimiters acting on multiple items         *
 *                                                                    *
 *********************************************************************  */

int32
test12 (void)
{
  uint32 i;
  
  static uchar item_0[2] = "AB";
  static uchar item_1[2] = "RA";
  static uchar item_2[7] = "CAD HEX";
  static uchar item_3[6] = "AB HEX";
  static uchar item_4[2] = "RA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2,
    item_3,
    item_4
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2),
				      sizeof (item_3),
				      sizeof (item_4)};
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uchar to_item[11] = "           ";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 3;
 
  uchar *delimiter_array[3];
 
  uint32 delimiter_length_array[] = {0, 1, 0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {2, 2, 1};

  uchar *delimiter_0;
  uchar *delimiter_1;
  uchar *delimiter_2;
 
  delimiter_0 = NULL;

  delimiter_1 = (uchar *)malloc (1);

  * delimiter_1 = ' ';

  delimiter_2 = NULL;

  delimiter_array[0] = delimiter_0;
  delimiter_array[1] = delimiter_1;
  delimiter_array[2] = delimiter_2;

  printf ("\n");
  printf ("Testing with multiple delimeters acting on multiple items:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test13 - Test pointer offset of 0 (underflow)                      *
 *                                                                    *
 *********************************************************************  */

int32
test13 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";
  static uchar item_2[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 0;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing with pointer = 0:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting error:    \"OVERFLOW OCCURRED\"\n");

  if (blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length))
    {
      printf ("Received error:     \"OVERFLOW OCCURRED\"\n");
      return 1;
    }
  else
    {
      printf ("After, target     = \"");
      for (i = 0; i < to_item_length; i ++)
        printf ("%c", to_item[i]);
      printf ("\"\n");
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Test14 - Test pointer offset one greater than length of target     *
 *          field (overflow)                                          *
 *                                                                    *
 *********************************************************************  */

int32
test14 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";
  static uchar item_2[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 12;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing with pointer = (length of target) + 1:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting error:    \"OVERFLOW OCCURRED\"\n");

  if (blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length))
    {
      printf ("Received error:     \"OVERFLOW OCCURRED\"\n");
      return 1;
    }
  else
    {
      printf ("After, target     = \"");
      for (i = 0; i < to_item_length; i ++)
        printf ("%c", to_item[i]);
      printf ("\"\n");
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Test15 - Test pointer offset much greater than length of target    *
 *          field (overflow)                                          *
 *                                                                    *
 *********************************************************************  */

int32
test15 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";
  static uchar item_2[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[11] = "           ";
 
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 99;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing with pointer much greater than length of target:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting error:    \"OVERFLOW OCCURRED\"\n");

  if (blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length))
    {
      printf ("Received error:     \"OVERFLOW OCCURRED\"\n");
      return 1;
    }
  else
    {
      printf ("After, target     = \"");
      for (i = 0; i < to_item_length; i ++)
        printf ("%c", to_item[i]);
      printf ("\"\n");
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Test16 - Test pointer offset greater than 1 but less than the      *
 *          length of the target field                                *
 *                                                                    *
 *********************************************************************  */

int32
test16 (void)
{
  uint32 i;
  
  static uchar item_0[2] = "CA";
  static uchar item_1[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1)};
 
  uchar to_item[11] = "ABRAXXXXXXX";
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 5;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing with pointer within range of target area:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test17 - Test pointer offset equal to the length of the target     *
 *          field                                                     *
 *                                                                    *
 *********************************************************************  */

int32
test17 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "AXXX";
  static uchar item_1[2] = "YY";
  static uchar item_2[5] = "ZZZZZ";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[11] = "ABRACADABRA";
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 11;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing with pointer equal to length of target area:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test18 - Number of characters to be moved is less than the length  *
 *          of the target area                                        *
 *                                                                    *
 *********************************************************************  */

int32
test18 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1)};
 
  uchar to_item[11] = "           ";
 
  uchar objective_to_item[11] = "ABRACA     ";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {2};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing moving fewer characters than target area has:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test19 - Number of characters to be moved is equal to the length   *
 *          of the target area                                        *
 *                                                                    *
 *********************************************************************  */

int32
test19 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";
  static uchar item_2[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[11] = "           ";
 
  uchar objective_to_item[11] = "ABRACADABRA";
 
  uint32 to_item_length = 11;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing moving same number of characters as target area has:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test20 - Number of characters to be moved is greater than the      *
 *          length of the target area                                 *
 *                                                                    *
 *********************************************************************  */

int32
test20 (void)
{
  uint32 i;
  
  static uchar item_0[4] = "ABRA";
  static uchar item_1[2] = "CA";
  static uchar item_2[5] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[6] = "ABRACA";
 
  uint32 to_item_length = 6;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;


  printf ("\n");
  printf ("Testing moving more characters than the target area has:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < to_item_length; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting error:    \"OVERFLOW OCCURRED\"\n");

  if (blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length))
    {
      printf ("Received error:     \"OVERFLOW OCCURRED\"\n");
      return 1;
    }
  else
    {
      printf ("After, target     = \"");
      for (i = 0; i < to_item_length; i ++)
        printf ("%c", to_item[i]);
      printf ("\"\n");
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Test21 - Some of the characters to be moved are string end         *
 *          characters '\0'                                           *
 *                                                                    *
 *********************************************************************  */

int32
test21 (void)
{
  uint32 i;
  
  static uchar item_0[8] = "ONE AND ";
  static uchar item_1[8] = "TWO AND ";
  static uchar item_2[5] = "THREE";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[21] = "                     ";
 
  uchar objective_to_item[21] = "ONE AND TWO AND THREE";
 
  uint32 to_item_length = 21;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;

  item_0[3] = 0x0;
  item_1[3] = 0x0;

  objective_to_item[3] = 0x0;
  objective_to_item[11] = 0x0;

  printf ("\n");
  printf ("Testing where some of the characters are string end characters:\n");
  printf ("Before, target    = \"");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  printf ("%X", objective_to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test22 - Each item to be moved ends in a string end character      *
 *          ('\0') which should be included                           *
 *                                                                    *
 *********************************************************************  */

int32
test22 (void)
{
  uint32 i;
  
  static uchar item_0[5] = "ABRA";
  static uchar item_1[3] = "CA";
  static uchar item_2[6] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[14] = "              ";
 
  uchar objective_to_item[14] = "ABRA CA DABRA ";

  uint32 to_item_length = 14;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;
 
  objective_to_item[4] = 0x0;
  objective_to_item[7] = 0x0;
  objective_to_item[13] = 0x0;


  printf ("\n");
  printf ("Testing where each item ends in a string end characters:\n");
  printf ("Before, target    = ");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf (" in hex\n");

  printf ("Expecting, target = ");
  printf ("%X", objective_to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", objective_to_item[i]);
  printf (" in hex\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = ");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf (" in hex\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test23 - Each item to be moved ends in a string end character and  *
 *          target area is all string end characters.                 *
 *                                                                    *
 *********************************************************************  */

int32
test23 (void)
{
  uint32 i;
  
  static uchar item_0[5] = "ABRA";
  static uchar item_1[3] = "CA";
  static uchar item_2[6] = "DABRA";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[14] = "              ";
 
  uchar objective_to_item[14] = "ABRA CA DABRA ";

  uint32 to_item_length = 14;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;
 
  for (i = 0; i < to_item_length; i ++)
    to_item[i] = 0x0;

  objective_to_item[4] = 0x0;
  objective_to_item[7] = 0x0;
  objective_to_item[13] = 0x0;


  printf ("\n");
  printf ("Testing where the target area is all string end characters:\n");
  printf ("Before, target    = ");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf (" in hex\n");

  printf ("Expecting, target = ");
  printf ("%X", objective_to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", objective_to_item[i]);
  printf (" in hex\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = ");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf (" in hex\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test24 - Some of the characters to be moved are line feed          *
 *          characters ('\n')                                         *
 *                                                                    *
 *********************************************************************  */

int32
test24 (void)
{
  uint32 i;
  
  static uchar item_0[8] = "ONE AND ";
  static uchar item_1[8] = "TWO AND ";
  static uchar item_2[5] = "THREE";

  uchar *from_item_array[] =
  {
    item_0,
    item_1,
    item_2
  };

  uint32 from_item_length_array[] = {sizeof (item_0),
				      sizeof (item_1),
				      sizeof (item_2)};
 
  uchar to_item[21] = "                     ";
 
  uchar objective_to_item[21] = "ONE AND TWO AND THREE";
 
  uint32 to_item_length = 21;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 1;
 
  uchar *delimiter_array[1];
 
  uint32 delimiter_length_array[] = {0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {3};
 
  uchar *delimiter_0;
 
  delimiter_0 = NULL;

  delimiter_array[0] = delimiter_0;

  item_0[3] = 0x10;
  item_1[3] = 0x10;

  objective_to_item[3] = 0x10;
  objective_to_item[11] = 0x10;

  printf ("\n");
  printf ("Testing where some of the characters are line feed characters:\n");
  printf ("Before, target    = \"");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  printf ("%X", objective_to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", objective_to_item[i]);
  printf ("\"\n");

  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  printf ("%X", to_item[0]);
  for (i = 1; i < to_item_length; i ++)
    printf (" %X", to_item[i]);
  printf ("\"\n");

/*--------------------------------------------------------------------*
 * Is to_item now identical to objective_to_item?  If not, return 1.  *
 *--------------------------------------------------------------------.  */

  if ((memcmp (objective_to_item, to_item, to_item_length)))
    return 1;
  else
    return 0;
}


/**********************************************************************
 *                                                                    *
 * Test99 - Test several types of delimiters                          *
 *                                                                    *
 *********************************************************************  */

int32
test99 (void)
{

  int32 i;

  /* THERE ARE VERY MANY SOLDIERS DESIRING/WANTING TO COME IN THE DOOR AND GAILY DISMEMBER US.  */

  static uchar item_0[6] = "THERE "; 
  static uchar item_1[8] = "ARE VERY"; 
  static uchar item_2[1] = " ";
  static uchar item_3[4] = "MANY";
  static uchar item_4[1] = " ";
  static uchar item_5[9] = "SOLDIERS ";
  static uchar item_6[16] = "DESIRING/WANTING";
  static uchar item_7[1] = " ";
  static uchar item_8[7] = "TO COME";
  static uchar item_9[1] = " ";
  static uchar item_10[11] = "IN THE DOOR";
  static uchar item_11[1] = " ";
  static uchar item_12[9] = "AND GAILY";
  static uchar item_13[1] = " ";
  static uchar item_14[9] = "DISMEMBER";
  static uchar item_15[1] = " ";
  static uchar item_16[3] = "US.";

  uchar *from_item_array[] = 
  {
    item_0,
    item_1,
    item_2,
    item_3,
    item_4,
    item_5,
    item_6,
    item_7,
    item_8,
    item_9,
    item_10,
    item_11,
    item_12,
    item_13,
    item_14,
    item_15,
    item_16
  };

  /* 
  uint32 from_item_length_array[] = { 6,8,1,4,1,9,16,1,7,1,11,1,9,1,9,1,3 };.  */

  uint32 from_item_length_array[] = 
  {
    sizeof (item_0),
    sizeof (item_1),
    sizeof (item_2),
    sizeof (item_3),
    sizeof (item_4),
    sizeof (item_5),
    sizeof (item_6),
    sizeof (item_7),
    sizeof (item_8),
    sizeof (item_9),
    sizeof (item_10),
    sizeof (item_11),
    sizeof (item_12),
    sizeof (item_13),
    sizeof (item_14),
    sizeof (item_15),
    sizeof (item_16)
  };
 
 
  uchar objective_to_item[70] = "THERE ARE MANY SOLDIERS DESIRING TO COME IN THE DOOR AND DISMEMBER US.";
 
  uchar to_item[70] = "THERE ARE MANY INFIDELS DESIRING TO COME IN THE DOOR AND SLAUGHTER US.";
 
  uint32 to_item_length = 70;
 
  uint32 pointer_offset = 1;
 
  uint32 delimiter_count = 7;
 
 
  uchar *delimiter_0;
  uchar *delimiter_1;
  uchar *delimiter_2;
  uchar *delimiter_3;
  uchar *delimiter_4;
  uchar *delimiter_5;
  uchar *delimiter_6;
 
  uint32 delimiter_length_array[] = {0, 1, 0, 1, 0, 1, 0};
  
  uint32 number_of_items_this_delimiter_controls_array[] = {1, 1, 4, 1, 5, 1, 3};
 
  uchar *delimiter_array[7];
 
  delimiter_0 = NULL;

  delimiter_1 = (uchar *)malloc (1);

  delimiter_2 = NULL;

  delimiter_3 = (uchar *)malloc (1);

  delimiter_4 = NULL;

  delimiter_5 = (uchar *)malloc (1);

  delimiter_6 = NULL;

  * delimiter_1 = ' ';

  * delimiter_3 = '/';

  * delimiter_5 = ' ';

  delimiter_array[0] = delimiter_0;
  delimiter_array[1] = delimiter_1;
  delimiter_array[2] = delimiter_2;
  delimiter_array[3] = delimiter_3;
  delimiter_array[4] = delimiter_4;
  delimiter_array[5] = delimiter_5;
  delimiter_array[6] = delimiter_6;


  printf ("\n");
  printf ("Testing several different delimiters:\n");
  printf ("Before, target    = \"");
  for (i = 0; i < 70; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  printf ("Expecting, target = \"");
  for (i = 0; i < 70; i ++)
    printf ("%c", objective_to_item[i]);
  printf ("\"\n");


  blir_string (from_item_array, from_item_length_array,
              delimiter_count,
              delimiter_array, delimiter_length_array,
              number_of_items_this_delimiter_controls_array,
              pointer_offset,
              to_item, to_item_length);

  printf ("After, target     = \"");
  for (i = 0; i < 70; i ++)
    printf ("%c", to_item[i]);
  printf ("\"\n");

  return 0;
}


int32
main (void)
{
  printf ("test 1\n");
  if (test01 ())
    {
      printf ("  test01: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 2\n");
  if (test02 ())
    {
      printf ("  test02: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 3\n");
  if (test03 ())
    {
      printf ("  test03: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 4\n");
  if (test04 ())
    {
      printf ("  test04: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 5\n");
  if (test05 ())
    {
      printf ("  test05: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 6\n");
  if (test06 ())
    {
      printf ("  test06: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 7\n");
  if (test07 ())
    {
      printf ("  test07: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 8\n");
  if (test08 ())
    {
      printf ("  test08: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 9\n");
  if (test09 ())
    {
      printf ("  test09: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 10\n");
  if (test10 ())
    {
      printf ("  test10: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 11\n");
  if (test11 ())
    {
      printf ("  test11: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 12\n");
  if (test12 ())
    {
      printf ("  test12: Target value does not match expected value!\n");
      return 1;
    }

#ifndef NDEBUG
  printf ("test 13\n");
  if (!(test13 ()))
    {
      printf ("  test13: Overflow condition should have occurred!\n");
      return 1;
    }
#endif

  printf ("test 14\n");
  if (!(test14 ()))
    {
      printf ("  test14: Overflow condition should have occurred!\n");
      return 1;
    }

  printf ("test 15\n");
  if (!(test15 ()))
    {
      printf ("  test15: Overflow condition should have occurred!\n");
      return 1;
    }

  printf ("test 16\n");
  if (test16 ())
    {
      printf ("  test16: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 17\n");
  if (test17 ())
    {
      printf ("  test17: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 18\n");
  if (test18 ())
    {
      printf ("  test18: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 19\n");
  if (test19 ())
    {
      printf ("  test19: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 20\n");
  if (!(test20 ()))
    {
      printf ("  test20: Overflow condition should have occurred!\n");
      return 1;
    }

  printf ("test 21\n");
  if (test21 ())
    {
      printf ("  test21: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 22\n");
  if (test22 ())
    {
      printf ("  test22: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 23\n");
  if (test23 ())
    {
      printf ("  test23: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 24\n");
  if (test24 ())
    {
      printf ("  test24: Target value does not match expected value!\n");
      return 1;
    }

  printf ("test 99\n");
  if (test99 ())
    {
      printf ("  test99: Target value does not match expected value!\n");
      return 1;
    }

  return 0;
}
