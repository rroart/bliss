/*====================================================================*
 *     This program is free software; you can redistribute it and/or  *
 *     modify it under the terms of the GNU General Public License    *
 *     as published by the Free Software Foundation; either version   *
 *     2 of the License, or (at your option) any later version.       *
 *                                                                    *
 *     Copyright (c) 2000, 2001  Daniel H. Ardison  dardison@hotmail.com    *
 *====================================================================.  */

/*====================================================================*
 *      Test program for blir_insrep.c testing.                       *
 *      The tests data is defined by a block of #defines which states:*
 *          TESTN               --> Test number                       *
 *          TESTN_REPLACE_COUNT --> Test replace conditions count     *
 *          TESTN_IDENT1        --> Source identifier                 *
 *          TESTN_REP0MM        --> Replacing condition number MM     *
 *          TESTN_REP0MM_COND   --> Condition All, Leading, First..   *
 *          TESTN_REP0MM_IDENT3 --> String to replace                 *
 *          TESTN_REP0MM_IDENT5 --> Replacing string                  *
 *          TESTN_REP0MM_AFTER  --> After string (optional)           *
 *          TESTN_REP0MM_BEFORE --> Before string (optional)          *
 *--------------------------------------------------------------------*
 *      This test program is currently supporting 3 different tests   *
 *      but it would be easy to define more, just copy/paste and      *
 *      replace the #define structure, increase the size of the arrays*
 *      involved and add the corresponding tests to the main function *
 *====================================================================.  */

#include <string.h>
#include <stdio.h>
#include "blir_inspect.h"

#define TEST1 1
#define TEST1_REPLACE_COUNT 2
#define TEST1_IDENT1 "BABABC"
#define TEST1_REP001 0
#define TEST1_REP001_COND blir_replacing_type_all
#define TEST1_REP001_IDENT3 "AB"
#define TEST1_REP001_IDENT5 "XY"
#define TEST1_REP002 1
#define TEST1_REP002_COND blir_replacing_type_all
#define TEST1_REP002_IDENT3 "BC"
#define TEST1_REP002_IDENT5 "VW"
#define TEST2 2
#define TEST2_REPLACE_COUNT 4
#define TEST2_IDENT1 "BBEABDABABBCABEE"
#define TEST2_REP001 0
#define TEST2_REP001_COND blir_replacing_type_all
#define TEST2_REP001_IDENT3 "AB"
#define TEST2_REP001_IDENT5 "XY"
#define TEST2_REP001_BEFORE "BC"
#define TEST2_REP002 1
#define TEST2_REP002_COND blir_replacing_type_leading
#define TEST2_REP002_IDENT3 "B"
#define TEST2_REP002_IDENT5 "W"
#define TEST2_REP002_AFTER  "D"
#define TEST2_REP003 2
#define TEST2_REP003_COND blir_replacing_type_first
#define TEST2_REP003_IDENT3 "E"
#define TEST2_REP003_IDENT5 "V"
#define TEST2_REP003_AFTER  "D"
#define TEST2_REP004 3
#define TEST2_REP004_COND blir_replacing_type_characters
#define TEST2_REP004_IDENT5 "Z"
#define TEST2_REP004_AFTER  "A"
#define TEST2_REP004_BEFORE "C"
#define TEST3 3
#define TEST3_REPLACE_COUNT 2
#define TEST3_IDENT1 "ABAABX"
#define TEST3_REP001 0
#define TEST3_REP001_COND blir_replacing_type_characters
#define TEST3_REP001_IDENT5 "Z"
#define TEST3_REP002 1
#define TEST3_REP002_COND blir_replacing_type_all
#define TEST3_REP002_IDENT3 "A"
#define TEST3_REP002_IDENT5 "W"

unsigned char identifier1[3][100];
unsigned char identifier3[3][10][2];
unsigned char identifier5[3][10][2];
unsigned char before[3][10][2];
unsigned char after[3][10][2];

enum blir_replacing_type_enum type[3][10];
struct blir_inspect_replacing_struct replace_details[3][10],  * rd;
unsigned int replace_status[10];
unsigned int after_bound[10];
unsigned int before_bound[10];


/* Test_blir_replace_fill_struct fill the structure needed for call.  */
/* Cobr_inspect_replacing function for each test.  */

void
test_blir_replace_fill_struct (
     unsigned int test,  /* Test to be filled.  */
     unsigned int replace_index /* Replacing index to be filled.  */)
{
      replace_details[test -1][replace_index].identifier3 = identifier3[test -1][replace_index];
      replace_details[test -1][replace_index].identifier3_length = strlen (identifier3[test -1][replace_index]);
      replace_details[test -1][replace_index].identifier5 = identifier5[test -1][replace_index];
      replace_details[test -1][replace_index].identifier5_length = strlen (identifier5[test -1][replace_index]);
      replace_details[test -1][replace_index].details.after_value = after[test -1][replace_index];
      replace_details[test -1][replace_index].details.after_value_length = strlen (after[test -1][replace_index]);
      replace_details[test -1][replace_index].details.before_value = before[test -1][replace_index];
      replace_details[test -1][replace_index].details.before_value_length = strlen (before[test -1][replace_index]);
      replace_details[test -1][replace_index].type = type[test -1][replace_index];
}

/* Test_blir_replace_fill fill the necesary strings depending on *
 * the defines tests, it support 3 test with up to 10 replacing  *
 * conditions each, but it should be very easy to extends it to  *
 * support more test cases.  just cut/paste & replace the test    *
 * blocks.  */
void
test_blir_replace_fill (
     unsigned int test /* Test index to be performed.  */)
{

    switch (test)
    {
       case TEST1 :
           strcpy (identifier1[test -1], TEST1_IDENT1);
#ifdef TEST1_REP001
           type[test -1][TEST1_REP001] = TEST1_REP001_COND;
           if (type[test -1][TEST1_REP001] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP001_IDENT3
              strcpy (identifier3[test -1][TEST1_REP001], TEST1_REP001_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP001], TEST1_REP001_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP001], " ");
               strcpy (identifier5[test -1][TEST1_REP001], TEST1_REP001_IDENT5);
           }
#ifdef TEST1_REP001_AFTER
           strcpy (after[test -1][TEST1_REP001], TEST1_REP001_AFTER);
#endif
#ifdef TEST1_REP001_BEFORE
           strcpy (before[test -1][TEST1_REP001], TEST1_REP001_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP001);
#endif
#ifdef TEST1_REP002
           type[test -1][TEST1_REP002] = TEST1_REP002_COND;
           if (type[test -1][TEST1_REP002] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP002_IDENT3
              strcpy (identifier3[test -1][TEST1_REP002], TEST1_REP002_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP002], TEST1_REP002_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP002], " ");
               strcpy (identifier5[test -1][TEST1_REP002], TEST1_REP002_IDENT5);
           }
#ifdef TEST1_REP002_AFTER
           strcpy (after[test -1][TEST1_REP002], TEST1_REP002_AFTER);
#endif
#ifdef TEST1_REP002_BEFORE
           strcpy (before[test -1][TEST1_REP002], TEST1_REP002_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP002);
#endif
#ifdef TEST1_REP003
           type[test -1][TEST1_REP003] = TEST1_REP003_COND;
           if (type[test -1][TEST1_REP003] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP003_IDENT3
              strcpy (identifier3[test -1][TEST1_REP003], TEST1_REP003_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP003], TEST1_REP003_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP003], " ");
               strcpy (identifier5[test -1][TEST1_REP003], TEST1_REP003_IDENT5);
           }
#ifdef TEST1_REP003_AFTER
           strcpy (after[test -1][TEST1_REP003], TEST1_REP003_AFTER);
#endif
#ifdef TEST1_REP003_BEFORE
           strcpy (before[test -1][TEST1_REP003], TEST1_REP003_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP003);
#endif
#ifdef TEST1_REP004
           type[test -1][TEST1_REP004] = TEST1_REP004_COND;
           if (type[test -1][TEST1_REP004] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP004_IDENT3
              strcpy (identifier3[test -1][TEST1_REP004], TEST1_REP004_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP004], TEST1_REP004_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP004], " ");
               strcpy (identifier5[test -1][TEST1_REP004], TEST1_REP004_IDENT5);
           }
#ifdef TEST1_REP004_AFTER
           strcpy (after[test -1][TEST1_REP004], TEST1_REP004_AFTER);
#endif
#ifdef TEST1_REP004_BEFORE
           strcpy (before[test -1][TEST1_REP004], TEST1_REP004_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP004);
#endif
#ifdef TEST1_REP005
           type[test -1][TEST1_REP005] = TEST1_REP005_COND;
           if (type[test -1][TEST1_REP005] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP005_IDENT3
              strcpy (identifier3[test -1][TEST1_REP005], TEST1_REP005_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP005], TEST1_REP005_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP005], " ");
               strcpy (identifier5[test -1][TEST1_REP005], TEST1_REP005_IDENT5);
           }
#ifdef TEST1_REP005_AFTER
           strcpy (after[test -1][TEST1_REP005], TEST1_REP005_AFTER);
#endif
#ifdef TEST1_REP005_BEFORE
           strcpy (before[test -1][TEST1_REP005], TEST1_REP005_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP005);
#endif
#ifdef TEST1_REP006
           type[test -1][TEST1_REP006] = TEST1_REP006_COND;
           if (type[test -1][TEST1_REP006] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP006_IDENT3
              strcpy (identifier3[test -1][TEST1_REP006], TEST1_REP006_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP006], TEST1_REP006_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP006], " ");
               strcpy (identifier5[test -1][TEST1_REP006], TEST1_REP006_IDENT5);
           }
#ifdef TEST1_REP006_AFTER
           strcpy (after[test -1][TEST1_REP006], TEST1_REP006_AFTER);
#endif
#ifdef TEST1_REP006_BEFORE
           strcpy (before[test -1][TEST1_REP006], TEST1_REP006_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP006);
#endif
#ifdef TEST1_REP007
           type[test -1][TEST1_REP007] = TEST1_REP007_COND;
           if (type[test -1][TEST1_REP007] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP007_IDENT3
              strcpy (identifier3[test -1][TEST1_REP007], TEST1_REP007_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP007], TEST1_REP007_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP007], " ");
               strcpy (identifier5[test -1][TEST1_REP007], TEST1_REP007_IDENT5);
           }
#ifdef TEST1_REP007_AFTER
           strcpy (after[test -1][TEST1_REP007], TEST1_REP007_AFTER);
#endif
#ifdef TEST1_REP007_BEFORE
           strcpy (before[test -1][TEST1_REP007], TEST1_REP007_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP007);
#endif
#ifdef TEST1_REP008
           type[test -1][TEST1_REP008] = TEST1_REP008_COND;
           if (type[test -1][TEST1_REP008] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP008_IDENT3
              strcpy (identifier3[test -1][TEST1_REP008], TEST1_REP008_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP008], TEST1_REP008_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP008], " ");
               strcpy (identifier5[test -1][TEST1_REP008], TEST1_REP008_IDENT5);
           }
#ifdef TEST1_REP008_AFTER
           strcpy (after[test -1][TEST1_REP008], TEST1_REP008_AFTER);
#endif
#ifdef TEST1_REP008_BEFORE
           strcpy (before[test -1][TEST1_REP008], TEST1_REP008_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP008);
#endif
#ifdef TEST1_REP009
           type[test -1][TEST1_REP009] = TEST1_REP009_COND;
           if (type[test -1][TEST1_REP009] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP009_IDENT3
              strcpy (identifier3[test -1][TEST1_REP009], TEST1_REP009_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP009], TEST1_REP009_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP009], " ");
               strcpy (identifier5[test -1][TEST1_REP009], TEST1_REP009_IDENT5);
           }
#ifdef TEST1_REP009_AFTER
           strcpy (after[test -1][TEST1_REP009], TEST1_REP009_AFTER);
#endif
#ifdef TEST1_REP009_BEFORE
           strcpy (before[test -1][TEST1_REP009], TEST1_REP009_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP009);
#endif
#ifdef TEST1_REP010
           type[test -1][TEST1_REP010] = TEST1_REP010_COND;
           if (type[test -1][TEST1_REP010] != blir_replacing_type_characters)
           {
#ifdef TEST1_REP010_IDENT3
              strcpy (identifier3[test -1][TEST1_REP010], TEST1_REP010_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST1_REP010], TEST1_REP010_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST1_REP010], " ");
               strcpy (identifier5[test -1][TEST1_REP010], TEST1_REP010_IDENT5);
           }
#ifdef TEST1_REP010_AFTER
           strcpy (after[test -1][TEST1_REP010], TEST1_REP010_AFTER);
#endif
#ifdef TEST1_REP010_BEFORE
           strcpy (before[test -1][TEST1_REP010], TEST1_REP010_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST1, TEST1_REP010);
#endif
           break;

       case TEST2 :
           strcpy (identifier1[test -1], TEST2_IDENT1);
#ifdef TEST2_REP001
           type[test -1][TEST2_REP001] = TEST2_REP001_COND;
           if (type[test -1][TEST2_REP001] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP001_IDENT3
              strcpy (identifier3[test -1][TEST2_REP001], TEST2_REP001_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP001], TEST2_REP001_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP001], " ");
               strcpy (identifier5[test -1][TEST2_REP001], TEST2_REP001_IDENT5);
           }
#ifdef TEST2_REP001_AFTER
           strcpy (after[test -1][TEST2_REP001], TEST2_REP001_AFTER);
#endif
#ifdef TEST2_REP001_BEFORE
           strcpy (before[test -1][TEST2_REP001], TEST2_REP001_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP001);
#endif
#ifdef TEST2_REP002
           type[test -1][TEST2_REP002] = TEST2_REP002_COND;
           if (type[test -1][TEST2_REP002] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP002_IDENT3
              strcpy (identifier3[test -1][TEST2_REP002], TEST2_REP002_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP002], TEST2_REP002_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP002], " ");
               strcpy (identifier5[test -1][TEST2_REP002], TEST2_REP002_IDENT5);
           }
#ifdef TEST2_REP002_AFTER
           strcpy (after[test -1][TEST2_REP002], TEST2_REP002_AFTER);
#endif
#ifdef TEST2_REP002_BEFORE
           strcpy (before[test -1][TEST2_REP002], TEST2_REP002_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP002);
#endif
#ifdef TEST2_REP003
           type[test -1][TEST2_REP003] = TEST2_REP003_COND;
           if (type[test -1][TEST2_REP003] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP003_IDENT3
              strcpy (identifier3[test -1][TEST2_REP003], TEST2_REP003_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP003], TEST2_REP003_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP003], " ");
               strcpy (identifier5[test -1][TEST2_REP003], TEST2_REP003_IDENT5);
           }
#ifdef TEST2_REP003_AFTER
           strcpy (after[test -1][TEST2_REP003], TEST2_REP003_AFTER);
#endif
#ifdef TEST2_REP003_BEFORE
           strcpy (before[test -1][TEST2_REP003], TEST2_REP003_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP003);
#endif
#ifdef TEST2_REP004
           type[test -1][TEST2_REP004] = TEST2_REP004_COND;
           if (type[test -1][TEST2_REP004] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP004_IDENT3
              strcpy (identifier3[test -1][TEST2_REP004], TEST2_REP004_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP004], TEST2_REP004_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP004], " ");
               strcpy (identifier5[test -1][TEST2_REP004], TEST2_REP004_IDENT5);
           }
#ifdef TEST2_REP004_AFTER
           strcpy (after[test -1][TEST2_REP004], TEST2_REP004_AFTER);
#endif
#ifdef TEST2_REP004_BEFORE
           strcpy (before[test -1][TEST2_REP004], TEST2_REP004_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP004);
#endif
#ifdef TEST2_REP005
           type[test -1][TEST2_REP005] = TEST2_REP005_COND;
           if (type[test -1][TEST2_REP005] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP005_IDENT3
              strcpy (identifier3[test -1][TEST2_REP005], TEST2_REP005_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP005], TEST2_REP005_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP005], " ");
               strcpy (identifier5[test -1][TEST2_REP005], TEST2_REP005_IDENT5);
           }
#ifdef TEST2_REP005_AFTER
           strcpy (after[test -1][TEST2_REP005], TEST2_REP005_AFTER);
#endif
#ifdef TEST2_REP005_BEFORE
           strcpy (before[test -1][TEST2_REP005], TEST2_REP005_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP005);
#endif
#ifdef TEST2_REP006
           type[test -1][TEST2_REP006] = TEST2_REP006_COND;
           if (type[test -1][TEST2_REP006] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP006_IDENT3
              strcpy (identifier3[test -1][TEST2_REP006], TEST2_REP006_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP006], TEST2_REP006_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP006], " ");
               strcpy (identifier5[test -1][TEST2_REP006], TEST2_REP006_IDENT5);
           }
#ifdef TEST2_REP006_AFTER
           strcpy (after[test -1][TEST2_REP006], TEST2_REP006_AFTER);
#endif
#ifdef TEST2_REP006_BEFORE
           strcpy (before[test -1][TEST2_REP006], TEST2_REP006_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP006);
#endif
#ifdef TEST2_REP007
           type[test -1][TEST2_REP007] = TEST2_REP007_COND;
           if (type[test -1][TEST2_REP007] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP007_IDENT3
              strcpy (identifier3[test -1][TEST2_REP007], TEST2_REP007_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP007], TEST2_REP007_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP007], " ");
               strcpy (identifier5[test -1][TEST2_REP007], TEST2_REP007_IDENT5);
           }
#ifdef TEST2_REP007_AFTER
           strcpy (after[test -1][TEST2_REP007], TEST2_REP007_AFTER);
#endif
#ifdef TEST2_REP007_BEFORE
           strcpy (before[test -1][TEST2_REP007], TEST2_REP007_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP007);
#endif
#ifdef TEST2_REP008
           type[test -1][TEST2_REP008] = TEST2_REP008_COND;
           if (type[test -1][TEST2_REP008] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP008_IDENT3
              strcpy (identifier3[test -1][TEST2_REP008], TEST2_REP008_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP008], TEST2_REP008_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP008], " ");
               strcpy (identifier5[test -1][TEST2_REP008], TEST2_REP008_IDENT5);
           }
#ifdef TEST2_REP008_AFTER
           strcpy (after[test -1][TEST2_REP008], TEST2_REP008_AFTER);
#endif
#ifdef TEST2_REP008_BEFORE
           strcpy (before[test -1][TEST2_REP008], TEST2_REP008_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP008);
#endif
#ifdef TEST2_REP009
           type[test -1][TEST2_REP009] = TEST2_REP009_COND;
           if (type[test -1][TEST2_REP009] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP009_IDENT3
              strcpy (identifier3[test -1][TEST2_REP009], TEST2_REP009_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP009], TEST2_REP009_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP009], " ");
               strcpy (identifier5[test -1][TEST2_REP009], TEST2_REP009_IDENT5);
           }
#ifdef TEST2_REP009_AFTER
           strcpy (after[test -1][TEST2_REP009], TEST2_REP009_AFTER);
#endif
#ifdef TEST2_REP009_BEFORE
           strcpy (before[test -1][TEST2_REP009], TEST2_REP009_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP009);
#endif
#ifdef TEST2_REP010
           type[test -1][TEST2_REP010] = TEST2_REP010_COND;
           if (type[test -1][TEST2_REP010] != blir_replacing_type_characters)
           {
#ifdef TEST2_REP010_IDENT3
              strcpy (identifier3[test -1][TEST2_REP010], TEST2_REP010_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST2_REP010], TEST2_REP010_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST2_REP010], " ");
               strcpy (identifier5[test -1][TEST2_REP010], TEST2_REP010_IDENT5);
           }
#ifdef TEST2_REP010_AFTER
           strcpy (after[test -1][TEST2_REP010], TEST2_REP010_AFTER);
#endif
#ifdef TEST2_REP010_BEFORE
           strcpy (before[test -1][TEST2_REP010], TEST2_REP010_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST2, TEST2_REP010);
#endif
           break;
       case TEST3 :
           strcpy (identifier1[test -1], TEST3_IDENT1);
#ifdef TEST3_REP001
           type[test -1][TEST3_REP001] = TEST3_REP001_COND;
           if (type[test -1][TEST3_REP001] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP001_IDENT3
              strcpy (identifier3[test -1][TEST3_REP001], TEST3_REP001_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP001], TEST3_REP001_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP001], " ");
               strcpy (identifier5[test -1][TEST3_REP001], TEST3_REP001_IDENT5);
           }
#ifdef TEST3_REP001_AFTER
           strcpy (after[test -1][TEST3_REP001], TEST3_REP001_AFTER);
#endif
#ifdef TEST3_REP001_BEFORE
           strcpy (before[test -1][TEST3_REP001], TEST3_REP001_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP001);
#endif
#ifdef TEST3_REP002
           type[test -1][TEST3_REP002] = TEST3_REP002_COND;
           if (type[test -1][TEST3_REP002] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP002_IDENT3
              strcpy (identifier3[test -1][TEST3_REP002], TEST3_REP002_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP002], TEST3_REP002_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP002], " ");
               strcpy (identifier5[test -1][TEST3_REP002], TEST3_REP002_IDENT5);
           }
#ifdef TEST3_REP002_AFTER
           strcpy (after[test -1][TEST3_REP002], TEST3_REP002_AFTER);
#endif
#ifdef TEST3_REP002_BEFORE
           strcpy (before[test -1][TEST3_REP002], TEST3_REP002_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP002);
#endif
#ifdef TEST3_REP003
           type[test -1][TEST3_REP003] = TEST3_REP003_COND;
           if (type[test -1][TEST3_REP003] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP003_IDENT3
              strcpy (identifier3[test -1][TEST3_REP003], TEST3_REP003_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP003], TEST3_REP003_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP003], " ");
               strcpy (identifier5[test -1][TEST3_REP003], TEST3_REP003_IDENT5);
           }
#ifdef TEST3_REP003_AFTER
           strcpy (after[test -1][TEST3_REP003], TEST3_REP003_AFTER);
#endif
#ifdef TEST3_REP003_BEFORE
           strcpy (before[test -1][TEST3_REP003], TEST3_REP003_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP003);
#endif
#ifdef TEST3_REP004
           type[test -1][TEST3_REP004] = TEST3_REP004_COND;
           if (type[test -1][TEST3_REP004] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP004_IDENT3
              strcpy (identifier3[test -1][TEST3_REP004], TEST3_REP004_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP004], TEST3_REP004_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP004], " ");
               strcpy (identifier5[test -1][TEST3_REP004], TEST3_REP004_IDENT5);
           }
#ifdef TEST3_REP004_AFTER
           strcpy (after[test -1][TEST3_REP004], TEST3_REP004_AFTER);
#endif
#ifdef TEST3_REP004_BEFORE
           strcpy (before[test -1][TEST3_REP004], TEST3_REP004_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP004);
#endif
#ifdef TEST3_REP005
           type[test -1][TEST3_REP005] = TEST3_REP005_COND;
           if (type[test -1][TEST3_REP005] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP005_IDENT3
              strcpy (identifier3[test -1][TEST3_REP005], TEST3_REP005_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP005], TEST3_REP005_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP005], " ");
               strcpy (identifier5[test -1][TEST3_REP005], TEST3_REP005_IDENT5);
           }
#ifdef TEST3_REP005_AFTER
           strcpy (after[test -1][TEST3_REP005], TEST3_REP005_AFTER);
#endif
#ifdef TEST3_REP005_BEFORE
           strcpy (before[test -1][TEST3_REP005], TEST3_REP005_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP005);
#endif
#ifdef TEST3_REP006
           type[test -1][TEST3_REP006] = TEST3_REP006_COND;
           if (type[test -1][TEST3_REP006] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP006_IDENT3
              strcpy (identifier3[test -1][TEST3_REP006], TEST3_REP006_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP006], TEST3_REP006_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP006], " ");
               strcpy (identifier5[test -1][TEST3_REP006], TEST3_REP006_IDENT5);
           }
#ifdef TEST3_REP006_AFTER
           strcpy (after[test -1][TEST3_REP006], TEST3_REP006_AFTER);
#endif
#ifdef TEST3_REP006_BEFORE
           strcpy (before[test -1][TEST3_REP006], TEST3_REP006_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP006);
#endif
#ifdef TEST3_REP007
           type[test -1][TEST3_REP007] = TEST3_REP007_COND;
           if (type[test -1][TEST3_REP007] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP007_IDENT3
              strcpy (identifier3[test -1][TEST3_REP007], TEST3_REP007_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP007], TEST3_REP007_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP007], " ");
               strcpy (identifier5[test -1][TEST3_REP007], TEST3_REP007_IDENT5);
           }
#ifdef TEST3_REP007_AFTER
           strcpy (after[test -1][TEST3_REP007], TEST3_REP007_AFTER);
#endif
#ifdef TEST3_REP007_BEFORE
           strcpy (before[test -1][TEST3_REP007], TEST3_REP007_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP007);
#endif
#ifdef TEST3_REP008
           type[test -1][TEST3_REP008] = TEST3_REP008_COND;
           if (type[test -1][TEST3_REP008] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP008_IDENT3
              strcpy (identifier3[test -1][TEST3_REP008], TEST3_REP008_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP008], TEST3_REP008_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP008], " ");
               strcpy (identifier5[test -1][TEST3_REP008], TEST3_REP008_IDENT5);
           }
#ifdef TEST3_REP008_AFTER
           strcpy (after[test -1][TEST3_REP008], TEST3_REP008_AFTER);
#endif
#ifdef TEST3_REP008_BEFORE
           strcpy (before[test -1][TEST3_REP008], TEST3_REP008_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP008);
#endif
#ifdef TEST3_REP009
           type[test -1][TEST3_REP009] = TEST3_REP009_COND;
           if (type[test -1][TEST3_REP009] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP009_IDENT3
              strcpy (identifier3[test -1][TEST3_REP009], TEST3_REP009_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP009], TEST3_REP009_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP009], " ");
               strcpy (identifier5[test -1][TEST3_REP009], TEST3_REP009_IDENT5);
           }
#ifdef TEST3_REP009_AFTER
           strcpy (after[test -1][TEST3_REP009], TEST3_REP009_AFTER);
#endif
#ifdef TEST3_REP009_BEFORE
           strcpy (before[test -1][TEST3_REP009], TEST3_REP009_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP009);
#endif
#ifdef TEST3_REP010
           type[test -1][TEST3_REP010] = TEST3_REP010_COND;
           if (type[test -1][TEST3_REP010] != blir_replacing_type_characters)
           {
#ifdef TEST3_REP010_IDENT3
              strcpy (identifier3[test -1][TEST3_REP010], TEST3_REP010_IDENT3);
#endif
              strcpy (identifier5[test -1][TEST3_REP010], TEST3_REP010_IDENT5);
           }
           else
           {
               strcpy (identifier3[test -1][TEST3_REP010], " ");
               strcpy (identifier5[test -1][TEST3_REP010], TEST3_REP010_IDENT5);
           }
#ifdef TEST3_REP010_AFTER
           strcpy (after[test -1][TEST3_REP010], TEST3_REP010_AFTER);
#endif
#ifdef TEST3_REP010_BEFORE
           strcpy (before[test -1][TEST3_REP010], TEST3_REP010_BEFORE);
#endif
           test_blir_replace_fill_struct (TEST3, TEST3_REP010);
#endif
           break;
           
    }
}
           

void
test_blir_replace (
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
     unsigned int replace_index;
     unsigned char text[100];
     struct blir_inspect_replacing_struct *replace_detail;

     /* Print input.  */
     printf ("Testing inspect replacing\n");
     printf ("Input:\n");
     strncpy (text, identifier1, identifier1_length);
     text[identifier1_length] = '\0';
     printf ("INSPECT %s REPLACING \n", text);
     replace_index = 0;
     replace_detail = replacing_details;
     while (replace_index < replacing_count)
     {
         if (replace_detail->type == blir_replacing_type_characters)
         {
             strncpy (text, replace_detail->identifier5,
                     replace_detail->identifier5_length);
             text[replace_detail->identifier5_length] = '\0';
             printf ("  CHARACTERS BY %s \n", text);
             if (replace_detail->details.after_value_length != 0)
             {
                 strncpy (text, replace_detail->details.after_value,
                         replace_detail->details.after_value_length);
                 text[replace_detail->details.after_value_length] = '\0';
                 printf ("    AFTER %s \n", text);
             }
             if (replace_detail->details.before_value_length != 0)
             {
                 strncpy (text, replace_detail->details.before_value,
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
             strncpy (text, replace_detail->identifier3,
                     replace_detail->identifier3_length);
             text[replace_detail->identifier3_length] = '\0';
             printf ("%s BY ", text);
             strncpy (text, replace_detail->identifier5,
                     replace_detail->identifier5_length);
             text[replace_detail->identifier5_length] = '\0';
             printf ("%s ", text);
             if (replace_detail->details.after_value_length != 0)
               {
                 strncpy (text, replace_detail->details.after_value,
                         replace_detail->details.after_value_length);
                 text[replace_detail->details.after_value_length] = '\0';
                 printf (" AFTER %s ", text);
               }
             if (replace_detail->details.before_value_length != 0)
               {
                 strncpy (text, replace_detail->details.before_value,
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
     strncpy (text, identifier1, identifier1_length);
     text[identifier1_length] = '\0';
     printf ("Output : %s \n \n", text);
     return;
}

void main (void)
{
  /*================================================================*
   *      TEST 1                                                    *
 *================================================================.  */
  test_blir_replace_fill (TEST1);
  rd = replace_details[0];
  test_blir_replace (identifier1[0],
                    strlen (identifier1[0]),
                    TEST1_REPLACE_COUNT,
                    rd,
                    replace_status,
                    after_bound,
                    before_bound);
  /*================================================================*
 *      TEST 2                                                    *
 *================================================================.  */
  test_blir_replace_fill (TEST2);
  rd = replace_details[1];
  test_blir_replace (identifier1[1],
                    strlen (identifier1[1]),
                    TEST2_REPLACE_COUNT,
                    rd,
                    replace_status,
                    after_bound,
                    before_bound);
  /*================================================================*
 *      TEST 3                                                    *
 *================================================================.  */
  test_blir_replace_fill (TEST3);
  rd = replace_details[2];
  test_blir_replace (identifier1[2],
                    strlen (identifier1[2]),
                    TEST3_REPLACE_COUNT,
                    rd,
                    replace_status,
                    after_bound,
                    before_bound);
}





