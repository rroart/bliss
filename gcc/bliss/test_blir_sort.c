

/**********************************************************************
 *                                                                    *
 *                      BLISS Sort Test Routines                      *
 *                                                                    *
 *           Copyright (C) 2000, 2001 by Theodore J.  Seward, Jr.            *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public License *
 * as published by the Free Software Foundation; either version 2.1   *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * Lesser General Public License for more details.                    *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  *
 * 02111-1307 USA                                                     *
 *                                                                    *
 * Mail:           Theodore J. Seward, Jr.                            *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title: BLISS Sort test routines                                    *
 *                                                                    *
 * Description: This file will test the BLISS sort routines.          *
 *                                                                    *
 * System: GNU BLISS Runtime Library                                  *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward 13/Jan/2001 Initial Release of Routines                  *
 *                                                                    *
 *********************************************************************  */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_overpunch.h"
#include "blir_sort.h"


/**********************************************************************
 *                                                                    *
 *                          Local Variables                           *
 *                                                                    *
 *********************************************************************  */


#define TEMPSORT        "TEMP.SORT"
#define TEMPNL          "TEMP.NL"
#define TEMPRDW         "TEMP.RDW"
#define TEMPFIX         "TEMP.FIX"

#define MAXARRAY (100)          /* Maximum array size */
#define MAXREC   (100)          /* Maximum records for file */
#define MAXRECSIZE (24)         /* Maximum record size */

#define MAX_FIELD_COUNT (32)    /* maximum fields */

blir_sort_field_desc_ptr sortField;  /* Array fields fields.  */
UINT32 sortFieldCount = 0;  /* Number of fields.  */

#define MAX_ARRAY_REC_LEN (80)  /* Maximum record length */
#define MAX_ARRAY_REC_COUNT  (100)      /* Maximum record count */

char *recArray[MAX_ARRAY_REC_COUNT];  /* Array of records.  */
UINT32 recCount = 0;  /* Number of records.  */

#define MAX_REPLINE     60

FILE *repFile;  /* Report file.  */
int repLine;  /* Report line number.  */
int repPage;  /* Report page number.  */



/**********************************************************************
 *                                                                    *
 * PutReportHead - Put a report header                                *
 *                                                                    *
 *********************************************************************  */


static int
putReportHead (void)
{
  if (repPage == 0)
    fprintf (repFile, "\n\n");
  else
    fprintf (repFile, "\f\n\n");
  repPage ++;
  fprintf (repFile,
           "                          Test Report For blir_sort()                   Page %3d\n",
           repPage);
  fprintf (repFile, "\n");
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportLine - Prepare to put a line                              *
 *                                                                    *
 *********************************************************************  */


static int
putReportLine (void)
{
  if (repLine > MAX_REPLINE)
    {
      putReportHead ();
      repLine = 0;
    }
  repLine ++;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportTerm - put an End of Report                               *
 *                                                                    *
 *********************************************************************  */


static int
putReportTerm (void)
{
  repLine += 2;
  if (repLine > MAX_REPLINE)
    {
      putReportHead ();
    }
  fprintf (repFile, "\n");
  fprintf (repFile, "                            *** End Of Report ***\n");
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportInit - Initialize Report                                  *
 *                                                                    *
 *********************************************************************  */

static int
putReportInit (void)
{
  repFile = stdout;
  repPage = 0;
  repLine = MAX_REPLINE + 1;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportNewPage - Force new page                                  *
 *                                                                    *
 *********************************************************************  */

static int
putReportNewPage (void)
{
  repLine = MAX_REPLINE + 1;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportBlankLine - Write a blank line                            *
 *                                                                    *
 *********************************************************************  */

static int
putReportBlankLine (void) /* Number of lines.  */
{
  fprintf (repFile, "\n");
  putReportLine ();

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutReportNeed - Specify minimum needed lines for a page            *
 *                                                                    *
 *********************************************************************  */

static int
putReportNeed (int n) /* Lines needed.  */
{
  if ((repLine + n) > MAX_REPLINE)
    putReportNewPage ();
  return 0;
}

/**********************************************************************
 *                                                                    *
 * SortFieldInit - Intialize sort fields                              *
 *                                                                    *
 *********************************************************************  */

static int
sortFieldInit (void)
{

  sortField =
    (blir_sort_field_desc_ptr)malloc (sizeof (blir_sort_field_desc_type) *
                                       MAX_FIELD_COUNT);
  sortFieldCount = 0;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * SortFieldAdd - Add a sort field                                    *
 *                                                                    *
 *********************************************************************  */

static int
sortFieldAdd (int offset,  /* Field offset.  */
              int length,  /* Field length.  */
              int decimals,  /* Fraction length.  */
              int type,  /* Field type.  */
              int flags,  /* Field flag.  */
              int direction) /* Sort direction.  */
{

  if (sortFieldCount < MAX_FIELD_COUNT)
    {
      sortField[sortFieldCount].offset = offset;
      sortField[sortFieldCount].length = length;
      sortField[sortFieldCount].decimals = decimals;
      sortField[sortFieldCount].type = type;
      sortField[sortFieldCount].flags = flags;
      sortField[sortFieldCount].direction = direction;
      sortFieldCount ++;
      return 0;
    }
  else
    return 1;
}


/**********************************************************************
 *                                                                    *
 * SortFieldTerm - Terminate sort fields                              *
 *                                                                    *
 *********************************************************************  */

static int
sortFieldTerm (void)
{
  free (sortField);
  sortFieldCount = 0;
  return 0;
}

/**********************************************************************
 *                                                                    *
 * SortGenField - Generate a sort field                               *
 *                                                                    *
 *********************************************************************  */

static int
sortGenField (char *field,  /* Field to gen.  */
              int length,  /* Length of field.  */
              int type,  /* Field type.  */
              int flag) /* Field flag.  */
{
  int i;
  int l;
  double d;
  unsigned int byte;
  unsigned int nibble1;
  unsigned int nibble2;
  FLOAT_DOUBLE d1;
  FLOAT_DOUBLE d2;
  const char *textChar =
    " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  switch (type)
    {
    case blir_typeNull :
      memset (field, ' ', length);
      break;
    case blir_typeText :
      l = strlen (textChar);
      for (i = 0; i < length; i ++)
        {
          field[i] = textChar[rand () % l];
        }
      break;
    case blir_typeDisplay :
      if ((flag & blir_flagOverpunch) != 0)
        {
          for (i = 0; i < length; i ++)
            {
              field[i] = (char)('0' + (rand () % 10));
            }

          if ((flag & blir_flagSignLead) != 0)
            {
              if ((rand () % 2) == 0)
                field[0] = blir_overpunch_set ('+', field[0]);
              else
                field[0] = blir_overpunch_set ('-', field[0]);
            }
          else if ((flag & blir_flagSignTrail) != 0)
            {
              if ((rand () % 2) == 0)
                field[length - 1] =
                  blir_overpunch_set ('+', field[length - 1]);
              else
                field[length - 1] =
                  blir_overpunch_set ('-', field[length - 1]);
            }
        }
      else
        {
          for (i = 0; i < length; i ++)
            {
              field[i] = (char)('0' + (rand () % 10));
            }
          if ((flag & blir_flagSignLead) != 0)
            {
              if ((rand () % 2) == 0)
                field[0] = '+';
              else
                field[0] = '-';
            }
          else if ((flag & blir_flagSignTrail) != 0)
            {
              if ((rand () % 2) == 0)
                field[length - 1] = '+';
              else
                field[length - 1] = '-';
            }
        }
      break;
    case blir_typePacked :
      for (i = 0; i < (length - 1); i ++)
        {
          nibble1 = rand () % 10;
          nibble2 = rand () % 10;
          byte = (nibble1 << 4) | nibble2;
          field[i] = (char)byte;
        }
      nibble1 = rand () % 10;
      if ((rand () % 2) == 0)
        nibble2 = 0x0C;
      else
        nibble2 = 0x0D;
      byte = (nibble1 << 4) | nibble2;
      field[length - 1] = (char)byte;
      break;
    case blir_typeBinary :
    case blir_typeUnsigned :
      for (i = 0; i < length; i ++)
        {
          byte = rand () % 256;
          * ((unsigned char *) & field[i]) = (unsigned char)byte;
        }
      break;
    case blir_typeFloat :
      if ((rand () % 2) == 0)
        d = rand () * 1.0;
      else
        d = rand () * -1.0;
      switch (length)
        {
        case FLOAT_SHORT_WIDTH :
          * ((FLOAT_SHORT * )field) = (FLOAT_SHORT)(d / 100.0);
          break;
        case FLOAT_DOUBLE_WIDTH :
          d1 = (FLOAT_DOUBLE)(d / 100.0);
          d2 = d * 1000000.0;
          * ((FLOAT_DOUBLE * )field) = d1 + d2;
          break;
        default :
          * ((FLOAT_SHORT * )field) = (FLOAT_SHORT)(d / 100.0);
          break;
        }
      break;
    default :
      memset (field, ' ', length);
      return 1;
    }

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Test_BuildRec - Build Record                                       *
 *                                                                    *
 *********************************************************************  */

static int
test_BuildRec (char *record,  /* Record.  */
               size_t recLen) /* Record length.  */
{
  UINT32 j;

  memset (record, ' ', recLen);
  for (j = 0; j < sortFieldCount; j ++)
    {
      sortGenField ( & record[sortField[j].offset], sortField[j].length,
                    sortField[j].type, sortField[j].flags);
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * BuildSortKeys - Build the sort keys                                *
 *                                                                    *
 *********************************************************************  */

static int
buildSortKeys (void)
{
  int length;
  int offset;

  sortFieldInit ();
  length = 1;
  offset = 0;
  sortFieldAdd (offset, length, 0, blir_typeText, blir_flagNull,
                BLIR_SORT_DIR_ASCENDING);
  offset += length;
  length = 8;
  sortFieldAdd (offset, length, 0, blir_typeText, blir_flagNull,
                BLIR_SORT_DIR_DESCENDING);
  offset += length;
  sortFieldAdd (offset, length, 0, blir_typeText, blir_flagNull,
                BLIR_SORT_DIR_ASCENDING);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * ColLength - Get column length                                      *
 *                                                                    *
 *********************************************************************  */

static int
lenColumn (blir_sort_field_desc_type field) /* Field description.  */
{
  switch (field.type)
    {
    case blir_typeNull :
      return field.length;
    case blir_typeText :
      return field.length;
    case blir_typeDisplay :
      if ((field.flags & blir_flagOverpunch) != 0)
        return field.length + 1;
      else
        return field.length;
    case blir_typePacked :
      return field.length * 2;
    case blir_typeBinary :
      switch (field.length)
        {
        case INT8_WIDTH :
          return 4;
        case INT16_WIDTH :
          return 6;
        case INT32_WIDTH :
          return 11;
        case INT64_WIDTH :
          return 20;
        default :
          return 11;
        }
    case blir_typeUnsigned :
      switch (field.length)
        {
        case UINT8_WIDTH :
          return 4;
        case UINT16_WIDTH :
          return 6;
        case UINT32_WIDTH :
          return 11;
        case UINT64_WIDTH :
          return 20;
        default :
          return 11;
        }
    case blir_typeFloat :
      switch (field.length)
        {
        case FLOAT_SHORT_WIDTH :
          return 14;
        case FLOAT_DOUBLE_WIDTH :
          return 22;
        default :
          return 14;
        }
    }
  return field.length;
}


/**********************************************************************
 *                                                                    *
 * PutColumn - Put Column Data                                        *
 *                                                                    *
 *********************************************************************  */

static int
putColumn (blir_sort_field_desc_type field,  /* Field description.  */
           void *ptr) /* Field Pointer.  */
{
  UINT32 i;
  int nibble1, nibble2;
  char *cptr;
  unsigned char *ucptr;
  int sign;
  int ch;

  switch (field.type)
    {
    case blir_typeNull :
    case blir_typeText :
      cptr = (char *)ptr;
      for (i = 0; i < field.length; i ++)
        fputc (cptr[i], repFile);
      break;
    case blir_typeDisplay :
      if ((field.flags & blir_flagOverpunch) != 0)
        {
          if ((field.flags & blir_flagSignLead) != 0)
            {
              cptr = (char *)ptr;
              blir_overpunch_get ( & sign,  & ch,  * cptr);
              if (sign > 0)
                fputc ('+', repFile);
              else
                fputc ('-', repFile);
              fputc (ch, repFile);
              for (i = 1; i < field.length; i ++)
                fputc (cptr[i], repFile);
            }
          else if ((field.flags & blir_flagSignTrail) != 0)
            {
              cptr = (char *)ptr;
              for (i = 0; i < (field.length - 1); i ++)
                fputc (cptr[i], repFile);
              blir_overpunch_get ( & sign,  & ch, cptr[field.length - 1]);
              fputc (ch, repFile);
              if (sign > 0)
                fputc ('+', repFile);
              else
                fputc ('-', repFile);
            }
          else
            {
              cptr = (char *)ptr;
              for (i = 0; i < field.length; i ++)
                fputc (cptr[i], repFile);
            }
        }
      else
        {
          cptr = (char *)ptr;
          for (i = 0; i < field.length; i ++)
            fputc (cptr[i], repFile);
        }
      break;
    case blir_typePacked :
      ucptr = (unsigned char *)ptr;
      for (i = 0; i < (field.length - 1); i ++)
        {
          nibble1 = ucptr[i] >> 4;
          nibble2 = ucptr[i] & 0x0F;
          fputc (nibble1 + '0', repFile);
          fputc (nibble2 + '0', repFile);
        }
      nibble1 = ucptr[field.length - 1] >> 4;
      nibble2 = ucptr[field.length - 1] & 0x0F;
      fputc (nibble1 + '0', repFile);
      if (nibble2 == 0x0D)
        fputc ('-', repFile);
      else
        fputc ('+', repFile);
      break;
    case blir_typeBinary :
      switch (field.length)
        {
        case INT8_WIDTH :
          fprintf (repFile, "%4d",  * ((INT8 * )ptr));
          break;
        case INT16_WIDTH :
          fprintf (repFile, "%6d",  * ((INT16 * )ptr));
          break;
        case INT32_WIDTH :
          fprintf (repFile, "%11d",  * ((INT32 * )ptr));
          break;
        case INT64_WIDTH :
          fprintf (repFile, "%20lld",  * ((INT64 * )ptr));
          break;
        default :
          fprintf (repFile, "%11d",  * ((INT32 * )ptr));
          break;
        }
      break;
    case blir_typeUnsigned :
      switch (field.length)
        {
        case INT8_WIDTH :
          fprintf (repFile, "%4u",  * ((UINT8 * )ptr));
          break;
        case INT16_WIDTH :
          fprintf (repFile, "%6u",  * ((UINT16 * )ptr));
          break;
        case INT32_WIDTH :
          fprintf (repFile, "%11u",  * ((UINT32 * )ptr));
          break;
        case INT64_WIDTH :
          fprintf (repFile, "%20llu",  * ((UINT64 * )ptr));
          break;
        default :
          fprintf (repFile, "%11u",  * ((UINT32 * )ptr));
          break;
        }
      break;
    case blir_typeFloat :
      switch (field.length)
        {
        case FLOAT_SHORT_WIDTH :
          fprintf (repFile, "%14.3f",  * ((FLOAT_SHORT * )ptr));
          break;
        case FLOAT_DOUBLE_WIDTH :
          fprintf (repFile, "%22.3f",  * ((FLOAT_DOUBLE * )ptr));
          break;
        default :
          fprintf (repFile, "%14.3f",  * ((FLOAT_SHORT * )ptr));
          break;
        }
      break;
    default :
      cptr = (char *)ptr;
      for (i = 0; i < field.length; i ++)
        fputc (cptr[i], repFile);
      break;
    }
  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutEntry - Display record entry                                    *
 *                                                                    *
 *********************************************************************  */


static int
putEntry (const char *title,  /* Title.  */
          const int recCount,  /* Total record.  */
          const int recNum,  /* Record number.  */
          const char *record) /* Record.  */
{
  UINT32 i, j;
  char column[64];
  char work[64];
  UINT32 length;

  if (recNum == 1)
    {
      putReportNeed (8);
      putReportBlankLine ();
      fprintf (repFile, "%s\n", title);
      putReportLine ();
      putReportBlankLine ();
      fprintf (repFile, "Total Records = %d\n", recCount);
      putReportLine ();
      putReportBlankLine ();

      fprintf (repFile, "Rec");
      for (i = 0; i < sortFieldCount; i ++)
        {
          length = lenColumn (sortField[i]);
          sprintf (column, "%d", i + 1);
          while (strlen (column) < length)
            {
              strcpy (work, " ");
              strcat (work, column);
              strcpy (column, work);
              if (strlen (column) < length)
                strcat (column, " ");
            }
          fprintf (repFile, "  %s", column);
        }
      fprintf (repFile, "\n");
      putReportLine ();

      fprintf (repFile, "---");
      for (i = 0; i < sortFieldCount; i ++)
        {
          length = lenColumn (sortField[i]);
          memset (column, '-', length);
          column[length] = '\0';
          fprintf (repFile, "  %s", column);
        }
      fprintf (repFile, "\n");
      putReportLine ();
    }

  fprintf (repFile, "%03d", recNum);
  for (j = 0; j < sortFieldCount; j ++)
    {
      fprintf (repFile, "  ");
      putColumn (sortField[j], (void *) & record[sortField[j].offset]);
    }
  fputc ('\n', repFile);
  putReportLine ();

  return 0;
}


/**********************************************************************
 *                                                                    *
 * PutFields - Display array of fields                                *
 *                                                                    *
 *********************************************************************  */


static int
putFields (const char *title)
{
  UINT32 i, j;

  char flagImage[32];
  char typeImage[32];

  putReportNeed (8);
  putReportBlankLine ();
  fprintf (repFile, "%s\n", title);
  putReportLine ();
  putReportBlankLine ();
  fprintf (repFile, "Total Fields = %d\n", sortFieldCount);
  putReportLine ();
  putReportBlankLine ();

  fprintf (repFile, "Field  Len  Dec  Dir  Type      Flags\n");
  putReportLine ();
  fprintf (repFile, "-----  ---  ---  ---  --------  ----------------\n");
  putReportLine ();
  for (i = 0; i < sortFieldCount; i ++)
    {
      fprintf (repFile, " %03d   %03d  %03d   %c   ",
               sortField[i].offset, sortField[i].length,
               sortField[i].decimals, sortField[i].direction);
      switch (sortField[i].type)
        {
        case blir_typeNull :
          strcpy (typeImage, "Null");
          break;
        case blir_typeText :
          strcpy (typeImage, "Text");
          break;
        case blir_typeDisplay :
          strcpy (typeImage, "Display");
          break;
        case blir_typePacked :
          strcpy (typeImage, "Packed");
          break;
        case blir_typeBinary :
          strcpy (typeImage, "Binary");
          break;
        case blir_typeUnsigned :
          strcpy (typeImage, "Unsigned");
          break;
        case blir_typeFloat :
          strcpy (typeImage, "Float");
          break;
        default :
          strcpy (typeImage, "Unknown");
          break;
        }

      strcpy (flagImage, "");

      if ((sortField[i].flags & blir_flagSignLead) != 0)
        strcat (flagImage, "LeadSign ");

      if ((sortField[i].flags & blir_flagSignTrail) != 0)
        strcat (flagImage, "TrailSign ");

      if ((sortField[i].flags & blir_flagOverpunch) != 0)
        strcat (flagImage, "Overpunch");

      if (strlen (flagImage) == 0)
        strcpy (flagImage, "None");

      j = strlen (flagImage) - 1;
      if (flagImage[j] == ' ')
        flagImage[j] = '\0';

      fprintf (repFile, "%-8s  %s\n", typeImage, flagImage);
      putReportLine ();
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * DoTestHeader - Write a test header line                            *
 *                                                                    *
 *********************************************************************  */


static int
doTestHeader (const char *title)
{
  UINT32 i, j;  /* Index.  */
  char workTitle[80];  /* Work title.  */


/*--------------------------------------------------------------------*
 * Write the test title                                               *
 *--------------------------------------------------------------------.  */

  putReportNewPage ();
  putReportBlankLine ();

  strcpy (workTitle, title);
  j = 0;
  for (i = 0; workTitle[i] != '\0'; i ++)
    if (isspace (workTitle[i]))
      {
        workTitle[i] = ' ';
      }
    else
      {
        workTitle[i] = (char)toupper (workTitle[i]);
        j = i;
      }
  workTitle[j + 1] = '\0';

  fprintf (repFile, "%s\n", workTitle);
  putReportLine ();

  for (i = 0; workTitle[i] != '\0'; i ++)
    if (workTitle[i] != ' ')
      workTitle[i] = '-';

  fprintf (repFile, "%s\n", workTitle);
  putReportLine ();

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestArrayInput - Run Array Input Test                            *
 *                                                                    *
 *********************************************************************  */

static int
doTestArrayInput (void)
{
  char line[MAXRECSIZE + 2];
  char *array[MAXARRAY];
  int i;
  void *handle;
  char *record;
  int maxRecSize;
  int recLen;

  doTestHeader ("Test Array Sort");

/*--------------------------------------------------------------------*
 * Initialize the sort environment                                    *
 *--------------------------------------------------------------------.  */

  buildSortKeys ();

  handle = blir_sort_init (sortFieldCount, sortField, NULL, NULL, 0);

/*--------------------------------------------------------------------*
 * Build test data                                                    *
 *--------------------------------------------------------------------.  */

  for (i = 0; i < MAXARRAY; i ++)
    {
      test_BuildRec (line, MAXRECSIZE);
      line[MAXRECSIZE] = '\0';
      array[i] = (char *)malloc (strlen (line) + 1);
      strcpy (array[i], line);
      putEntry ("Data Prior to Sort:", MAXARRAY, i + 1, line);
    }

/*--------------------------------------------------------------------*
 * Provide array to sort routines                                     *
 *--------------------------------------------------------------------.  */

  blir_sort_input_array (handle, (void **)array, MAXRECSIZE, MAXARRAY);

/*--------------------------------------------------------------------*
 * Free up the array                                                  *
 *--------------------------------------------------------------------.  */

  for (i = 0; i < MAXARRAY; i ++)
    free (array[i]);

/*--------------------------------------------------------------------*
 * Get back the sorted data                                           *
 *--------------------------------------------------------------------.  */

  putFields ("Sort Fields");

  maxRecSize = MAXRECSIZE + 2;

  record = (char *)malloc (maxRecSize + 2);

  i = 0;
  while ((recLen = blir_sort_output_record (handle, record, maxRecSize)) > 0)
    {
      record[recLen] = '\0';
      putEntry ("Data After Sort:", MAXARRAY,  ++i, record);
    }

  free (record);

/*--------------------------------------------------------------------*
 * Close up sort and end                                              *
 *--------------------------------------------------------------------.  */

  blir_sort_wrap (handle);

  sortFieldTerm ();

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestVarNL - Run variable length records delimited by New Line    *
 *                                                                    *
 *********************************************************************  */

static int
doTestVarNL (void)
{
  char line[MAXRECSIZE + 2];
  int i, l;
  void *handle;
  FILE *oFile;
  FILE *iFile;

  doTestHeader ("Test Variable Length Records Delimited by New Line");

/*--------------------------------------------------------------------*
 * Initialize the sort environment                                    *
 *--------------------------------------------------------------------.  */

  buildSortKeys ();

  handle = blir_sort_init (sortFieldCount, sortField, NULL, NULL, 0);

/*--------------------------------------------------------------------*
 * Build test data                                                    *
 *--------------------------------------------------------------------.  */

  oFile = fopen (TEMPNL, "w");
  for (i = 1; i <= MAXREC; i ++)
    {
      test_BuildRec (line, MAXRECSIZE);
      line[MAXRECSIZE] = '\0';
      fprintf (oFile, "%s\n", line);
      putEntry ("Data Prior to Sort:", MAXREC, i, line);
    }

  fclose (oFile);

/*--------------------------------------------------------------------*
 * Provide file to sort routines                                      *
 *--------------------------------------------------------------------.  */

  blir_sort_input_file (handle, TEMPNL, blir_sort_file_VarNL, MAXRECSIZE);

/*--------------------------------------------------------------------*
 * Write sorted data to output file                                   *
 *--------------------------------------------------------------------.  */

  blir_sort_output_file (handle, TEMPSORT, blir_sort_file_VarNL, MAXRECSIZE);

/*--------------------------------------------------------------------*
 * Get back the sorted data                                           *
 *--------------------------------------------------------------------.  */

  putFields ("Sort Fields");

  iFile = fopen (TEMPSORT, "r");

  i = 0;
  while (fgets (line, sizeof (line), iFile) != NULL)
    {
      l = strlen (line);
      if (line[l - 1] == '\n')
        line[l - 1] = '\0';
      putEntry ("Data After Sort:", MAXREC,  ++i, line);
    }

  fclose (iFile);
  remove (TEMPNL);
  remove (TEMPSORT);

/*--------------------------------------------------------------------*
 * Close up sort and end                                              *
 *--------------------------------------------------------------------.  */

  blir_sort_wrap (handle);

  sortFieldTerm ();

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestFix - Run Fixed length records                               *
 *                                                                    *
 *********************************************************************  */

static int
doTestFix (void)
{
  char line[MAXRECSIZE + 2];
  int i;
  void *handle;
  FILE *oFile;
  FILE *iFile;

  doTestHeader ("Test Fixed Length Records");

/*--------------------------------------------------------------------*
 * Initialize the sort environment                                    *
 *--------------------------------------------------------------------.  */

  buildSortKeys ();

  handle = blir_sort_init (sortFieldCount, sortField, NULL, NULL, 0);

/*--------------------------------------------------------------------*
 * Build test data                                                    *
 *--------------------------------------------------------------------.  */

  oFile = fopen (TEMPFIX, "wb");
  for (i = 1; i <= MAXREC; i ++)
    {
      test_BuildRec (line, MAXRECSIZE);
      line[MAXRECSIZE] = '\0';
      fwrite (line, sizeof (char), MAXRECSIZE, oFile);
      putEntry ("Data Prior to Sort:", MAXREC, i, line);
    }

  fclose (oFile);

/*--------------------------------------------------------------------*
 * Provide file to sort routines                                      *
 *--------------------------------------------------------------------.  */

  blir_sort_input_file (handle, TEMPFIX, blir_sort_file_Fix, MAXRECSIZE);

/*--------------------------------------------------------------------*
 * Write sorted data to output file                                   *
 *--------------------------------------------------------------------.  */

  blir_sort_output_file (handle, TEMPSORT, blir_sort_file_Fix, MAXRECSIZE);

/*--------------------------------------------------------------------*
 * Get back the sorted data                                           *
 *--------------------------------------------------------------------.  */

  putFields ("Sort Fields");

  iFile = fopen (TEMPSORT, "rb");

  i = 0;
  while (fread (line, sizeof (char), MAXRECSIZE, iFile) == MAXRECSIZE)
    {
      line[MAXRECSIZE] = '\0';
      putEntry ("Data After Sort:", MAXREC,  ++i, line);
    }

  fclose (iFile);
  remove (TEMPFIX);
  remove (TEMPSORT);

/*--------------------------------------------------------------------*
 * Close up sort and end                                              *
 *--------------------------------------------------------------------.  */

  blir_sort_wrap (handle);

  sortFieldTerm ();

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestVarRDW - Run variable length records delimited by Record     *
 * Descriptor Words                                                   *
 *                                                                    *
 *********************************************************************  */

static int
doTestVarRDW (void)
{
  char line[MAXRECSIZE + 2];
  int i;
  void *handle;
  FILE *oFile;
  FILE *iFile;
  unsigned char asciiRDW[4];
  size_t recLen;

  doTestHeader
    ("Test Variable Length Records Delimited by Record Descriptor Words");

/*--------------------------------------------------------------------*
 * Initialize the sort environment                                    *
 *--------------------------------------------------------------------.  */

  buildSortKeys ();

  handle = blir_sort_init (sortFieldCount, sortField, NULL, NULL, 0);

/*--------------------------------------------------------------------*
 * Build test data                                                    *
 *--------------------------------------------------------------------.  */

  oFile = fopen (TEMPRDW, "wb");
  for (i = 1; i <= MAXREC; i ++)
    {
      test_BuildRec (line, MAXRECSIZE);
      line[MAXRECSIZE] = '\0';
      asciiRDW[0] = 0;
      asciiRDW[1] = 28;
      asciiRDW[2] = 0;
      asciiRDW[3] = 0;
      fwrite (asciiRDW, sizeof (char), sizeof (asciiRDW), oFile);
      fwrite (line, sizeof (char), MAXRECSIZE, oFile);
      putEntry ("Data Prior to Sort:", MAXREC, i, line);
    }

  fclose (oFile);

/*--------------------------------------------------------------------*
 * Provide file to sort routines                                      *
 *--------------------------------------------------------------------.  */

  blir_sort_input_file (handle, TEMPRDW, blir_sort_file_VarRD, MAXRECSIZE);

/*--------------------------------------------------------------------*
 * Write sorted data to output file                                   *
 *--------------------------------------------------------------------.  */

  blir_sort_output_file (handle, TEMPSORT, blir_sort_file_VarRD, MAXRECSIZE);

/*--------------------------------------------------------------------*
 * Get back the sorted data                                           *
 *--------------------------------------------------------------------.  */

  putFields ("Sort Fields");

  iFile = fopen (TEMPSORT, "rb");

  i = 0;
  while (fread (asciiRDW, sizeof (char), sizeof (asciiRDW), iFile) ==
         sizeof (asciiRDW))
    {
      recLen = (asciiRDW[0] * 256) + asciiRDW[1];
      if (fread (line, sizeof (char), recLen, iFile) != recLen)
        {
          printf ("*** Bad Data Read ***\n");
        }
      line[recLen] = '\0';
      putEntry ("Data After Sort:", MAXREC,  ++i, line);
    }

  fclose (iFile);
  remove (TEMPRDW);
  remove (TEMPSORT);

/*--------------------------------------------------------------------*
 * Close up sort and end                                              *
 *--------------------------------------------------------------------.  */

  blir_sort_wrap (handle);

  sortFieldTerm ();

  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestProvide - Provide records one at a time                      *
 *                                                                    *
 *********************************************************************  */

static int
doTestProvide (void)
{
  char line[MAXRECSIZE + 2];
  int i;
  void *handle;
  char *record;
  int maxRecSize;
  int recLen;

  doTestHeader ("Provide records one at a time");

/*--------------------------------------------------------------------*
 * Initialize the sort environment                                    *
 *--------------------------------------------------------------------.  */

  buildSortKeys ();

  handle = blir_sort_init (sortFieldCount, sortField, NULL, NULL, 0);

/*--------------------------------------------------------------------*
 * Build test data                                                    *
 *--------------------------------------------------------------------.  */

  for (i = 1; i <= MAXREC; i ++)
    {
      test_BuildRec (line, MAXRECSIZE);
      line[MAXRECSIZE] = '\0';
      blir_sort_input_record (handle, line, strlen (line));
      putEntry ("Data Prior to Sort:", MAXREC, i, line);
    }

/*--------------------------------------------------------------------*
 * Get back the sorted data                                           *
 *--------------------------------------------------------------------.  */

  putFields ("Sort Fields");

  maxRecSize = MAXRECSIZE + 2;

  record = (char *)malloc (maxRecSize + 2);

  i = 0;
  while ((recLen = blir_sort_output_record (handle, record, maxRecSize)) > 0)
    {
      record[recLen] = '\0';
      putEntry ("Data After Sort:", MAXREC,  ++i, record);
    }

  free (record);

/*--------------------------------------------------------------------*
 * Close up sort and end                                              *
 *--------------------------------------------------------------------.  */

  blir_sort_wrap (handle);

  sortFieldTerm ();

  return 0;
}


/**********************************************************************
 *                                                                    *
 * DoTestArrayPut - Display array of records                          *
 *                                                                    *
 *********************************************************************  */


static int
doTestArrayPut (const char *title)
{
  UINT32 i, j;
  char *cptr;
  char column[64];
  char work[64];
  UINT32 length;

  putReportNeed (8);
  putReportBlankLine ();
  fprintf (repFile, "%s\n", title);
  putReportLine ();
  putReportBlankLine ();
  fprintf (repFile, "Total Records = %d\n", recCount);
  putReportLine ();
  putReportBlankLine ();

  fprintf (repFile, "Rec");
  for (i = 0; i < sortFieldCount; i ++)
    {
      length = lenColumn (sortField[i]);
      sprintf (column, "%d", i + 1);
      while (strlen (column) < length)
        {
          strcpy (work, " ");
          strcat (work, column);
          strcpy (column, work);
          if (strlen (column) < length)
            strcat (column, " ");
        }
      fprintf (repFile, "  %s", column);
    }
  fprintf (repFile, "\n");
  putReportLine ();

  fprintf (repFile, "---");
  for (i = 0; i < sortFieldCount; i ++)
    {
      length = lenColumn (sortField[i]);
      memset (column, '-', length);
      column[length] = '\0';
      fprintf (repFile, "  %s", column);
    }
  fprintf (repFile, "\n");
  putReportLine ();

  for (i = 0; i < recCount; i ++)
    {
      fprintf (repFile, "%03d", i + 1);
      cptr = (char *)recArray[i];
      for (j = 0; j < sortFieldCount; j ++)
        {
          fprintf (repFile, "  ");
          putColumn (sortField[j], (void *) & cptr[sortField[j].offset]);
        }
      fputc ('\n', repFile);
      putReportLine ();
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * SortRecordFree - Free up sort records                              *
 *                                                                    *
 *********************************************************************  */


static int
sortRecordFree (void)
{
  UINT32 i;  /* Index.  */

  if (recCount > 0)
    {
      for (i = 0; i < recCount; i ++)
        free (recArray[i]);
    }

  recCount = 0;

  return 0;
}


/**********************************************************************
 *                                                                    *
 * SortRecordInit - Initialize record addition                        *
 *                                                                    *
 *********************************************************************  */


static int
sortRecordInit (void)
{
  sortRecordFree ();
  recCount = 0;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * SortRecordAdd - Add a sort record                                  *
 *                                                                    *
 *********************************************************************  */


static int
sortRecordAdd (char *record,  /* Record to add.  */
               UINT32 length) /* Length of record.  */
{
  if (recCount < MAX_ARRAY_REC_COUNT)
    {
      recArray[recCount] = (char *)malloc (length);
      memcpy (recArray[recCount], record, length);
      recCount ++;
      return 0;
    }
  else
    return 1;
}


/**********************************************************************
 *                                                                    *
 * DoTestArrayOne - Run sort test with various data types             *
 *                                                                    *
 *********************************************************************  */

static void
doTestArrayOne (UINT32 numRec,  /* Number of records.  */
                const char *title,  /* Test title.  */
                UINT32 length,  /* Field widths.  */
                UINT32 type,  /* Field type.  */
                UINT32 flag,  /* Field flag.  */
                unsigned char *sequence) /* Collating Sequence.  */
{
  UINT32 offset;  /* Field offset.  */
  UINT32 i, j;  /* Index.  */
  char record[MAX_ARRAY_REC_LEN];  /* Record.  */
  void *handle;  /* Sort handle.  */
  char workTitle[80];  /* Work title.  */
  blir_sort_return sortRC;


/*--------------------------------------------------------------------*
 * Write the test title                                               *
 *--------------------------------------------------------------------.  */

  putReportNewPage ();
  putReportBlankLine ();

  strcpy (workTitle, title);
  j = 0;
  for (i = 0; workTitle[i] != '\0'; i ++)
    if (isspace (workTitle[i]))
      {
        workTitle[i] = ' ';
      }
    else
      {
        workTitle[i] = (char)toupper (workTitle[i]);
        j = i;
      }
  workTitle[j + 1] = '\0';

  fprintf (repFile, "%s\n", workTitle);
  putReportLine ();

  for (i = 0; workTitle[i] != '\0'; i ++)
    if (workTitle[i] != ' ')
      workTitle[i] = '-';

  fprintf (repFile, "%s\n", workTitle);
  putReportLine ();


/*--------------------------------------------------------------------*
 * Generate the sort fields                                           *
 *--------------------------------------------------------------------.  */

  sortFieldInit ();
  offset = 0;
  sortFieldAdd (offset, length, 0, type, flag, BLIR_SORT_DIR_ASCENDING);
  offset += length;
  sortFieldAdd (offset, length, 0, type, flag, BLIR_SORT_DIR_DESCENDING);
  offset += length;
  sortFieldAdd (offset, length, 0, type, flag, BLIR_SORT_DIR_ASCENDING);

  sortRecordInit ();

/*--------------------------------------------------------------------*
 * Generate the records                                               *
 *--------------------------------------------------------------------.  */

  for (i = 1; i <= numRec; )
    {
      memset (record, ' ', sizeof (record));
      for (j = 0; j < sortFieldCount; j ++)
        {
          sortGenField ( & record[sortField[j].offset], sortField[j].length,
                        sortField[j].type, sortField[j].flags);
        }
      sortRecordAdd (record, MAX_ARRAY_REC_LEN);
      i ++;

/*--------------------------------------------------------------------*
 * If this is not a special low volume test, then make sure there are *
 * records where the first field is duplicated.  This should force    *
 * half the compares to be done on at least the second field.         *
 *--------------------------------------------------------------------.  */

      if (numRec > 3)
        {
          for (j = 1; j < sortFieldCount; j ++)
            {
              sortGenField ( & record[sortField[j].offset], sortField[j].length,
                            sortField[j].type, sortField[j].flags);
            }
          sortRecordAdd (record, MAX_ARRAY_REC_LEN);
          i ++;
        }
    }


/*--------------------------------------------------------------------*
 * Echo the generated data                                            *
 *--------------------------------------------------------------------.  */

  doTestArrayPut ("Data Prior To Sort:");

/*--------------------------------------------------------------------*
 * Sort the data                                                      *
 *--------------------------------------------------------------------.  */

  handle = blir_sort_init (sortFieldCount, sortField, sequence, NULL, 0);

  if (handle == NULL)
    {
      fprintf (stderr, "*** blir_sort_init() failed ***\n");
      exit (4);
    }

  sortRC =
    blir_sort_array (handle, (void **)recArray, MAX_ARRAY_REC_LEN, recCount);
  if (sortRC == blir_sort_return_error)
    {
      fprintf (stderr, "*** blir_sort_array() failed ****\n");
    }

  blir_sort_wrap (handle);

/*--------------------------------------------------------------------*
 * Display the sorted data                                            *
 *--------------------------------------------------------------------.  */

  putFields ("Sort Fields:");

  doTestArrayPut ("Data After Sort:");

  sortFieldTerm ();
}


/**********************************************************************
 *                                                                    *
 * DoTestArrayAll - Test array sort routines                          *
 *                                                                    *
 *********************************************************************  */

static int
doTestArrayAll (void)
{
  UINT32 numRec;  /* Number of records.  */

  numRec = 0;
  doTestArrayOne (numRec, "Test Text Field Sorts:  Zero records", 8,
                  blir_typeText, blir_flagNull, NULL);

  numRec = 1;
  doTestArrayOne (numRec, "Test Text Field Sorts:  1 Record", 8,
                  blir_typeText, blir_flagNull, NULL);

  numRec = 2;
  doTestArrayOne (numRec, "Test Text Field Sorts:  2 Records", 8,
                  blir_typeText, blir_flagNull, NULL);

  numRec = 3;
  doTestArrayOne (numRec, "Test Text Field Sorts:  3 Records", 8,
                  blir_typeText, blir_flagNull, NULL);

  numRec = 100;

  doTestArrayOne (numRec, "Test Text Field Sorts", 8, blir_typeText,
                  blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Numeric Display Field Sorts", 8,
                  blir_typeDisplay, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Numeric Display with leading separate signs",
                  8, blir_typeDisplay, blir_flagSignLead, NULL);

  doTestArrayOne (numRec,
                  "Test Numeric Display with leading overpunched signs", 8,
                  blir_typeDisplay, blir_flagSignLead | blir_flagOverpunch,
                  NULL);

  doTestArrayOne (numRec, "Test Numeric Display with trailing separate signs",
                  8, blir_typeDisplay, blir_flagSignTrail, NULL);

  doTestArrayOne (numRec,
                  "Test Numeric Display with trailing overpunched signs", 8,
                  blir_typeDisplay, blir_flagSignTrail | blir_flagOverpunch,
                  NULL);

  doTestArrayOne (numRec, "Test packed decimal data", 4, blir_typePacked,
                  blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Signed 8 bit binary data", INT8_WIDTH,
                  blir_typeBinary, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Signed 16 bit binary data", INT16_WIDTH,
                  blir_typeBinary, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Signed 32 bit binary data", INT32_WIDTH,
                  blir_typeBinary, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Signed 64 bit binary data", INT64_WIDTH,
                  blir_typeBinary, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Unsigned 8 bit binary data", UINT8_WIDTH,
                  blir_typeUnsigned, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Unsigned 16 bit binary data", UINT16_WIDTH,
                  blir_typeUnsigned, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Unsigned 32 bit binary data", UINT32_WIDTH,
                  blir_typeUnsigned, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Unsigned 64 bit binary data", UINT64_WIDTH,
                  blir_typeUnsigned, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Short Floating Point data", FLOAT_SHORT_WIDTH,
                  blir_typeFloat, blir_flagNull, NULL);

  doTestArrayOne (numRec, "Test Double Floating Point data",
                  FLOAT_DOUBLE_WIDTH, blir_typeFloat, blir_flagNull, NULL);

  sortRecordFree ();

  return 0;
}


/**********************************************************************
 *                                                                    *
 * DoTestEBCDIC - Test with EBCDIC Collating Sequence                 *
 *                                                                    *
 *********************************************************************  */

static int
doTestEBCDIC (void)
{

  int numRec;  /* Number of records.  */
  unsigned char sequence[256];  /* Collating sequence.  */

  numRec = 100;

  doTestArrayOne (numRec,
                  "EBCDIC Test",
                  8,
                  blir_typeText,
                  blir_flagNull, blir_sort_sequence (sequence,
                                                     alphabet_ebcdic));
  sortRecordFree ();
  return 0;
}

/**********************************************************************
 *                                                                    *
 * DoTestASCII - Test with ASCII Collating Sequence                   *
 *                                                                    *
 *********************************************************************  */

static int
doTestASCII (void)
{

  int numRec;  /* Number of records.  */
  unsigned char sequence[256];  /* Collating sequence.  */

  numRec = 100;

  doTestArrayOne (numRec,
                  "ASCII Test",
                  8,
                  blir_typeText,
                  blir_flagNull, blir_sort_sequence (sequence,
                                                     alphabet_ascii));
  sortRecordFree ();
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Main - Program mainline                                            *
 *                                                                    *
 *********************************************************************  */

int
main (void)
{
  time_t tTime;

/*--------------------------------------------------------------------*
 * Initialize Random Number Generator                                 *
 *--------------------------------------------------------------------.  */

  srand ((unsigned)time ( & tTime));

  putReportInit ();

/*--------------------------------------------------------------------*
 * Execute the various tests                                          *
 *--------------------------------------------------------------------.  */

  doTestArrayAll ();
  doTestArrayInput ();
  doTestVarNL ();
  doTestVarRDW ();
  doTestFix ();
  doTestProvide ();
  doTestASCII ();
  doTestEBCDIC ();

/*--------------------------------------------------------------------*
 * Close up and return                                                *
 *--------------------------------------------------------------------.  */

  putReportTerm ();

  if (repFile != stdout)
    fclose (repFile);

  return 0;
}
