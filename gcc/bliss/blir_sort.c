
/**********************************************************************
 *                                                                    *
 *                        BLISS Sort Routines                         *
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
 * E-mail:         tseward@compuserve.com                             *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title:          BLISS Sort Routines                                *
 *                                                                    *
 * Description:    These routines will supply the the needed runtime  *
 *                 options for a BLISS sort..                         *
 *                                                                    *
 * System:         GNU BLISS Runtime Library                          *
 *                                                                    *
 * Modification Log:                                                  *
 *                                                                    *
 * Programmer  Date         Description of Change                     *
 * ----------  -----------  ----------------------------------------- *
 * T. Seward   13/Jan/2001  Initial Release of Routines               *
 *                                                                    *
 *********************************************************************  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "blidebug.h"
#include "blir_temp_config.h"
#include "blir_sort.h"
#include "blir_sort_io.h"
#include "blir_compare.h"


/**********************************************************************
 *                                                                    *
 *                         Various constants                          *
 *                                                                    *
 *********************************************************************  */

#define SORT_FOOTPRINT "SORTFOOT"       /* Sort footprint */

typedef enum
{
  False = 0,  /* Logical False.  */
  True = 1 /* Logical True.  */
}
Boolean;  /* Logical daa type.  */

struct sort_control
{
  char footprint[8];  /* Footprint to verify control block.  */
  UINT32 maxRecSize;  /* Maximum record written.  */
  UINT32 maxBufSize;  /* Maximum buffer size.  */
  UINT32 curBufSize;  /* Current buffer size.  */
  UINT32 curPtrCount;  /* Current pointer count.  */
  UINT32 curPtrRec;  /* Current pointer record.  */
  char *buffer;  /* Buffer.  */
  char *bufCurRec;  /* Pointer for record.  */
  char **bufCurPtr;  /* Pointer record array.  */
  fileSys_ptr fileSysId;  /* File system id.  */
  fileCtl_ptr sort1;  /* Sort file 1.  */
  fileCtl_ptr sort2;  /* Sort file 2.  */
  fileCtl_ptr sort3;  /* Sort file 3.  */
  Boolean sortFilesUsed;  /* Sort files used flag.  */
  fileCtl_ptr sortCur;  /* Current output file.  */
  UINT32 fieldCount;  /* Number of fields.  */
  blir_sort_field_desc_ptr fieldDesc;  /* Field descriptions.  */
  UINT32 minRecLength;  /* Minimum record length implied by sort fields.  */
  unsigned char *sequence;  /* Collating sequence.  */
  char *sortTemp1;  /* Sort tempoary 1.  */
  char *sortTemp2;  /* Sort tempoary 2.  */
  char *sortTemp3;  /* Sort tempoary 3.  */
  enum
  {
    st_null = 0,  /* Null state.  */
    st_init,  /* Intialized.  */
    st_adding,  /* Adding record.  */
    st_retarray,  /* Return from array.  */
    st_retfile,  /* Return from file.  */
    st_eof /* At eof.  */
  }
  state;  /* Current state.  */
};

typedef struct sort_control sort_control_type;  /* Sort_control type.  */
typedef sort_control_type * sort_control_ptr;  /* Sort_control pointer.  */
#define SORT_CONTROL_NULL  ((sort_control_ptr)NULL)     /* sort_control null pointer */

/**********************************************************************
 *                                                                    *
 * PutMessage - write a severe error message                          *
 *                                                                    *
 *********************************************************************  */

static int
putMessage (const char *message) /* Error message.  */
{
  fprintf (stderr, "\n*** %s ***\n", message);
  fflush (stderr);

  return 0;
}

/**********************************************************************
 *                                                                    *
 * Sort_SetBufferPointers - reset the various buffer pointers         *
 *                                                                    *
 * This is called to initalize and after each buffer is written to    *
 * disk                                                               *
 *                                                                    *
 *********************************************************************  */


static int
sort_SetBufferPointers (sort_control_ptr sortHandle) /* Sort handle.  */
{
  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  sortHandle->curBufSize = sortHandle->maxBufSize;
  sortHandle->bufCurRec = sortHandle->buffer;
  sortHandle->bufCurPtr =
    (char **)(sortHandle->buffer + sortHandle->maxBufSize - 1);
  sortHandle->curPtrCount = 0;
  sortHandle->curPtrRec = 0;

  return 0;
}



/**********************************************************************
 *                                                                    *
 * Cobr_sort_init - Initialize Sort logic                             *
 *                                                                    *
 *********************************************************************  */

void * /* Returns handle.  */
blir_sort_init (UINT32 sort_field_count,  /* Number of fields.  */
                blir_sort_field_desc_ptr sort_fields,  /* Fields.  */
                unsigned char *sequence,  /* Collating sequence, NULL = none.  */
                char *temp_path,  /* Tempoary path, NULL = none.  */
                UINT32 sortBufSize) /* Sort buffer size, 0 = default value.  */
{
  sort_control_ptr sortHandle;  /* Sorthandle.  */
  fileSys_ptr fileSysId;  /* File system id.  */
  int fieldSize;  /* Size of fields in bytes.  */
  UINT32 i;  /* Index.  */

/*--------------------------------------------------------------------*
 * Allocate sort handle area                                          *
 *--------------------------------------------------------------------.  */

  sortHandle = (sort_control_ptr)malloc (sizeof (sort_control_type));

  if (sortHandle == SORT_CONTROL_NULL)
    {
      putMessage ("blir_sort_init-1:  Unable to allocate sort control area");
      return (void *)NULL;
    }

  memcpy (sortHandle->footprint, SORT_FOOTPRINT,
          sizeof (sortHandle->footprint));


/*--------------------------------------------------------------------*
 * Set up the sort fields                                             *
 *--------------------------------------------------------------------.  */

  sortHandle->fieldCount = sort_field_count + 1;
  fieldSize = sizeof (blir_sort_field_desc_type) * sortHandle->fieldCount;
  sortHandle->fieldDesc = (blir_sort_field_desc_ptr)malloc (fieldSize);
  if (sortHandle->fieldDesc == SORT_FIELD_DESC_NULL)
    {
      free (sortHandle);
      putMessage
        ("blir_sort_init-2:  Unable to allocate sort field description area");
      return (void *)NULL;
    }


/*--------------------------------------------------------------------*
 * Generate sort keys being used by this program                      *
 *--------------------------------------------------------------------.  */


  for (i = 0; i < sortHandle->fieldCount; i ++)
    {

/*--------------------------------------------------------------------*
 * User supplied sort fields                                          *
 *--------------------------------------------------------------------.  */

      if (i < (sortHandle->fieldCount - 1))
        {
          sortHandle->fieldDesc[i].offset =
            sort_fields[i].offset + sizeof (sortPfx_type);
          sortHandle->fieldDesc[i].length = sort_fields[i].length;
          sortHandle->fieldDesc[i].decimals = sort_fields[i].decimals;
          sortHandle->fieldDesc[i].type = sort_fields[i].type;
          sortHandle->fieldDesc[i].flags = sort_fields[i].flags;
          sortHandle->fieldDesc[i].direction = sort_fields[i].direction;
        }

/*--------------------------------------------------------------------*
 * Added sort fields for record position                              *
 *--------------------------------------------------------------------.  */

      else
        {
          sortHandle->fieldDesc[i].offset =
            offsetof (sortPfx_type, sortPfx_number);
          sortHandle->fieldDesc[i].length = sizeof (UINT32);
          sortHandle->fieldDesc[i].decimals = 0;
          sortHandle->fieldDesc[i].type = blir_typeUnsigned;
          sortHandle->fieldDesc[i].flags = blir_flagNull;
          sortHandle->fieldDesc[i].direction = BLIR_SORT_DIR_ASCENDING;
        }
    }

  sortHandle->minRecLength = 0;
  sortHandle->maxRecSize = 0;

/*--------------------------------------------------------------------*
 * Set up buffer                                                      *
 *--------------------------------------------------------------------.  */

  if (sortBufSize == 0)
    sortHandle->maxBufSize = SORT_DEFBUFSIZE;
  else
    sortHandle->maxBufSize = sortBufSize;
  sortHandle->buffer = (char *)malloc (sortHandle->maxBufSize);
  if (sortHandle->buffer == (char *)NULL)

    {
      free (sortHandle);
      return SORT_CONTROL_NULL;
    }

  sort_SetBufferPointers (sortHandle);

  fileSysId = blir_sort_io_Init ();

  if (fileSysId == FILESYS_NULL)

    {
      free (sortHandle->buffer);
      free (sortHandle);
      return SORT_CONTROL_NULL;
    }


/*--------------------------------------------------------------------*
 * Set up collating sequence                                          *
 *--------------------------------------------------------------------.  */

  if (sequence == (unsigned char *)NULL)
    sortHandle->sequence = (unsigned char *)NULL;
  else
    {
      sortHandle->sequence = (unsigned char *)malloc (256);
      memcpy (sortHandle->sequence, sequence, 256);
    }

/*--------------------------------------------------------------------*
 * Set up temporary files                                             *
 *--------------------------------------------------------------------.  */

  if (temp_path == (char *)NULL)
    {
      sortHandle->sortTemp1 = (char *)malloc (strlen (SORT_SORTTEMP1) + 1);
      strcpy (sortHandle->sortTemp1, SORT_SORTTEMP1);

      sortHandle->sortTemp2 = (char *)malloc (strlen (SORT_SORTTEMP2) + 1);
      strcpy (sortHandle->sortTemp2, SORT_SORTTEMP2);

      sortHandle->sortTemp3 = (char *)malloc (strlen (SORT_SORTTEMP3) + 1);
      strcpy (sortHandle->sortTemp3, SORT_SORTTEMP3);
    }
  else
    {
      sortHandle->sortTemp1 =
        (char *)malloc (strlen (temp_path) + strlen (SORT_SORTTEMP1) + 1);
      strcpy (sortHandle->sortTemp1, temp_path);
      strcat (sortHandle->sortTemp1, SORT_SORTTEMP1);

      sortHandle->sortTemp2 =
        (char *)malloc (strlen (temp_path) + strlen (SORT_SORTTEMP2) + 1);
      strcpy (sortHandle->sortTemp2, temp_path);
      strcat (sortHandle->sortTemp2, SORT_SORTTEMP2);

      sortHandle->sortTemp3 =
        (char *)malloc (strlen (temp_path) + strlen (SORT_SORTTEMP3) + 1);
      strcpy (sortHandle->sortTemp3, temp_path);
      strcat (sortHandle->sortTemp3, SORT_SORTTEMP3);
    }

/*--------------------------------------------------------------------*
 * Set up files                                                       *
 *--------------------------------------------------------------------.  */

  sortHandle->fileSysId = fileSysId;
  sortHandle->sort1 = FILECTL_NULL;
  sortHandle->sort2 = FILECTL_NULL;
  sortHandle->sort3 = FILECTL_NULL;
  sortHandle->sortFilesUsed = False;
  sortHandle->sortCur = sortHandle->sort2;
  sortHandle->state = st_init;
  return sortHandle;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_wrap - Wrap-up sort procedure                            *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_wrap (void *handle) /* Sort handle.  */
{
  sort_control_ptr sortHandle = (sort_control_ptr)handle;  /* Sort handle.  */
  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);
  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);

  if (sortHandle->sequence != (unsigned char *)NULL)
    free (sortHandle->sequence);

  if (sortHandle->sortTemp1 != (char *)NULL)
    free (sortHandle->sortTemp1);

  if (sortHandle->sortTemp2 != (char *)NULL)
    free (sortHandle->sortTemp2);

  if (sortHandle->sortTemp3 != (char *)NULL)
    free (sortHandle->sortTemp3);

  if (sortHandle->fieldDesc != SORT_FIELD_DESC_NULL)
    {
      free (sortHandle->fieldDesc);
    }

  sortHandle->fieldDesc = SORT_FIELD_DESC_NULL;

  if (sortHandle->sort1 != FILECTL_NULL)
    {
      blir_sort_io_close (sortHandle->sort1);
      sortHandle->sort1 = FILECTL_NULL;
    }

  if (sortHandle->sort2 != FILECTL_NULL)
    {
      blir_sort_io_close (sortHandle->sort2);
      sortHandle->sort2 = FILECTL_NULL;
    }

  if (sortHandle->sort3 != FILECTL_NULL)
    {
      blir_sort_io_close (sortHandle->sort3);
      sortHandle->sort3 = FILECTL_NULL;
    }

  if (sortHandle->sortFilesUsed)
    {
      blir_sort_io_delete (sortHandle->fileSysId, sortHandle->sortTemp1);
      blir_sort_io_delete (sortHandle->fileSysId, sortHandle->sortTemp2);
      blir_sort_io_delete (sortHandle->fileSysId, sortHandle->sortTemp3);
    }

  if (sortHandle->fileSysId != FILESYS_NULL)
    {
      blir_sort_io_Wrap (sortHandle->fileSysId);
      sortHandle->fileSysId = FILESYS_NULL;
    }

  free (sortHandle->buffer);
  free (sortHandle);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Sort_input_record_prim - primitive to add record                   *
 *                                                                    *
 *********************************************************************  */


static UINT32
sort_input_record_prim (sort_control_ptr sortHandle,  /* Sort handle.  */
                        void *record,  /* Record.  */
                        UINT32 size) /* Record size.  */
{
  UINT32 newRecLen;  /* Record length.  */
  sortPfx_type prefix;  /* Record prefix.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  newRecLen = sizeof (sortPfx_type) + size;
  sortHandle->bufCurPtr --;
  sortHandle->bufCurPtr[0] = sortHandle->bufCurRec;
  sortHandle->curPtrCount ++;
  prefix.sortPfx_length = newRecLen;
  prefix.sortPfx_number = blir_sort_io_highRecNum (sortHandle->fileSysId);
  memcpy (sortHandle->bufCurRec,  & prefix, sizeof (sortPfx_type));
  sortHandle->bufCurRec += sizeof (sortPfx_type);
  memcpy (sortHandle->bufCurRec, record, size);
  sortHandle->bufCurRec += size;
  sortHandle->curBufSize -= newRecLen + sizeof (unsigned char *);

  return 0;
}


/**********************************************************************
 *                                                                    *
 * QsortNoPrefix_compare - Local compare fields of two records        *
 *                                                                    *
 *********************************************************************  */

static int
qsortNoPrefix_compare (sort_control_ptr sortHandle,  /* Sort handle.  */
                       const void *item1,  /* Left item.  */
                       const void *item2) /* Right item.  */
{
  UINT32 i;  /* Index.  */
  INT32 n;  /* Compare results.  */
  char *ptr1 = (char *)item1;  /* Left pointer.  */
  char *ptr2 = (char *)item2;  /* Right pointer.  */
  int adjOffset;  /* Adjustment to offset to undo prefix adjustment.  */
  UINT32 adjCount;  /* Adjustment to count to undo prefix adjustment.  */

  adjOffset = sizeof (sortPfx_type);
  adjCount = sortHandle->fieldCount - 1;

  for (i = 0; i < adjCount; i ++)
    {
      n = blir_compare ( & ptr1[sortHandle->fieldDesc[i].offset - adjOffset],  /* Undo prefix adjustment.  */
                        & ptr2[sortHandle->fieldDesc[i].offset - adjOffset],
                        sortHandle->fieldDesc[i].length,
                        sortHandle->fieldDesc[i].type,
                        sortHandle->fieldDesc[i].flags, sortHandle->sequence);
      if (n != 0)
        {
          if (sortHandle->fieldDesc[i].direction == BLIR_SORT_DIR_ASCENDING)
            return n;
          else
            return - n;
        }
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * QsortNoPrefix - Local quick sort routine to sort array of pointers *
 *                                                                    *
 *********************************************************************  */

static void
qsortNoPrefix (sort_control_ptr sortHandle,  /* Sort handle.  */
               void *array[],  /* Array being sorted.  */
               int left,  /* Left item of sort range.  */
               int right) /* Right item of sort range.  */
{
  void *swap;  /* Swap value.  */
  void *sentinel;  /* Piviot value.  */
  int leftNew, rightNew;  /* Index.  */

  leftNew = left;
  rightNew = right;
  sentinel = array[(left + right) / 2];

  do
    {
      while (qsortNoPrefix_compare (sortHandle, array[leftNew], sentinel) < 0)
        leftNew ++;
      while (qsortNoPrefix_compare (sortHandle, sentinel, array[rightNew]) <
             0)
        rightNew --;
      if (leftNew <= rightNew)
        {
          swap = array[leftNew];
          array[leftNew] = array[rightNew];
          array[rightNew] = swap;
          leftNew ++;
          rightNew --;
        }
    }
  while (leftNew <= rightNew);
  if (left < rightNew)
    qsortNoPrefix (sortHandle, array, left, rightNew);
  if (leftNew < right)
    qsortNoPrefix (sortHandle, array, leftNew, right);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_array - Sort array of pointers to records                *
 *                                                                    *
 *********************************************************************  */

blir_sort_return /* Return code.  */
  blir_sort_array (void *handle, /* Sort handle returned by blir_sort_init().  */
                   void **records,  /* Array of item pointers to sort.  */
                   UINT32 length,  /* Minimum length of records.  */
                   UINT32 count) /* Number of items.  */
{
  sort_control_ptr sortHandle;  /* Sort handle.  */
  void *swap;  /* Swap pointer.  */

/*--------------------------------------------------------------------*
 * Error Checks                                                       *
 *--------------------------------------------------------------------.  */

  sortHandle = (sort_control_ptr)handle;

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);

  if (count <= 1)
    return blir_sort_return_normal;

  BLI_ASSERT (sortHandle->minRecLength <= length);

  if (count == 2)
    {
      if (qsortNoPrefix_compare (sortHandle,  & records[0],  & records[1]) > 0)
        {
          swap = records[0];
          records[0] = records[1];
          records[1] = swap;
        }
    }
  else
    {
      qsortNoPrefix (sortHandle, records, 0, count - 1);
    }

  return blir_sort_return_normal;
}

/**********************************************************************
 *                                                                    *
 * QsortPrefix_compare - Local compare fields of two records          *
 *                                                                    *
 *********************************************************************  */

static int
qsortPrefix_compare (sort_control_ptr sortHandle,  /* Sort handle.  */
                     const void *item1,  /* Left item.  */
                     const void *item2) /* Right item.  */
{
  UINT32 i;  /* Index.  */
  INT32 n;  /* Compare results.  */
  char *ptr1 = (char *)item1;  /* Left pointer.  */
  char *ptr2 = (char *)item2;  /* Right pointer.  */

  for (i = 0; i < sortHandle->fieldCount; i ++)
    {
      n = blir_compare ( & ptr1[sortHandle->fieldDesc[i].offset],
                        & ptr2[sortHandle->fieldDesc[i].offset],
                        sortHandle->fieldDesc[i].length,
                        sortHandle->fieldDesc[i].type,
                        sortHandle->fieldDesc[i].flags, sortHandle->sequence);

      if (sortHandle->fieldDesc[i].direction == BLIR_SORT_DIR_ASCENDING)
        return n;
      else
        return - n;

    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * QsortPrefix - Local quick sort routine to sort array of pointers   *
 *                                                                    *
 *********************************************************************  */

static void
qsortPrefix (sort_control_ptr sortHandle,  /* Sort handle.  */
             void *array[],  /* Array being sorted.  */
             int left,  /* Left item of sort range.  */
             int right) /* Right item of sort range.  */
{
  void *swap;  /* Swap value.  */
  void *sentinel;  /* Piviot value.  */
  int leftNew, rightNew;  /* Index.  */

  leftNew = left;
  rightNew = right;
  sentinel = array[(left + right) / 2];

  do
    {
      while (qsortPrefix_compare (sortHandle, array[leftNew], sentinel) < 0)
        leftNew ++;
      while (qsortPrefix_compare (sortHandle, sentinel, array[rightNew]) < 0)
        rightNew --;
      if (leftNew <= rightNew)
        {
          swap = array[leftNew];
          array[leftNew] = array[rightNew];
          array[rightNew] = swap;
          leftNew ++;
          rightNew --;
        }
    }
  while (leftNew <= rightNew);  /* For do.  */

  if (left < rightNew)
    qsortPrefix (sortHandle, array, left, rightNew);
  if (leftNew < right)
    qsortPrefix (sortHandle, array, leftNew, right);
}

/**********************************************************************
 *                                                                    *
 * Sort_EmptyBuffer - Sort, write, and empty sort buffer              *
 *                                                                    *
 *********************************************************************  */


static int
sort_EmptyBuffer (sort_control_ptr sortHandle) /* Sort handle.  */
{
  UINT32 i;  /* Index.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  if (sortHandle->curPtrCount == 0)
    {
      sort_SetBufferPointers (sortHandle);
      return 0;
    }

  if (!sortHandle->sortFilesUsed)
    {
      sortHandle->sort2 =
        blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp2,
                           "wb");
      sortHandle->sort3 =
        blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp3,
                           "wb");
      sortHandle->sortFilesUsed = True;
      if ((sortHandle->sort2 == FILECTL_NULL)
          || (sortHandle->sort3 == FILECTL_NULL))
        {
          blir_sort_io_close (sortHandle->sort2);
          sortHandle->sort2 = FILECTL_NULL;
          blir_sort_io_close (sortHandle->sort3);
          sortHandle->sort3 = FILECTL_NULL;
          return 1;
        }
    }

  qsortPrefix (sortHandle, (void **)sortHandle->bufCurPtr, 0,
               sortHandle->curPtrCount - 1);

  for (i = 0; i < sortHandle->curPtrCount; i ++)
    blir_sort_io_write ((void *)sortHandle->bufCurPtr[i],
                        sortHandle->sortCur);

  sort_SetBufferPointers (sortHandle);

  if (sortHandle->sortCur == sortHandle->sort2)
    sortHandle->sortCur = sortHandle->sort3;
  else
    sortHandle->sortCur = sortHandle->sort2;

  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_input_record - sort input record                         *
 *                                                                    *
 *********************************************************************  */


UINT32
blir_sort_input_record (void *handle,  /* Sort handel.  */
                        void *record,  /* Record.  */
                        UINT32 size) /* Record size.  */
{
  UINT32 newNeeded;  /* How many new bytes needed.  */
  sort_control_ptr sortHandle = (sort_control_ptr)handle;  /* Sort handle.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);
  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);

  if (sortHandle->state == st_init)
    sortHandle->state = st_adding;

  if (sortHandle->state != st_adding)
    return 1;

  newNeeded = sizeof (sortPfx_type) + size + sizeof (unsigned char *);

  if (newNeeded > sortHandle->maxBufSize)
    {
      size =
        sortHandle->maxBufSize - sizeof (sortPfx_type) -
        sizeof (unsigned char *);
      newNeeded = sizeof (sortPfx_type) + size + sizeof (unsigned char *);
    }

  if (newNeeded > sortHandle->maxRecSize)
    sortHandle->maxRecSize = newNeeded;

  if (newNeeded > sortHandle->curBufSize)
    sort_EmptyBuffer (sortHandle);

  return sort_input_record_prim (sortHandle, record, size);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_input_array - input array                                *
 *                                                                    *
 *********************************************************************  */

UINT32 blir_sort_input_array (void *handle,  /* Sort handle.  */
                              void **array,  /* Array of pointers.  */
                              UINT32 recLen,  /* Record length.  */
                              UINT32 arrayLen) /* Length of records.  */
{
  UINT32 i;
  sort_control_ptr sortHandle = (sort_control_ptr)handle;
  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);
  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);
  for (i = 0; i < arrayLen; i ++)
    blir_sort_input_record (sortHandle, array[i], recLen);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Sort_input_file_VarNL - Variable length input records, New Line    *
 *                                                                    *
 *********************************************************************  */


static UINT32
sort_input_file_VarNL (sort_control_ptr sortHandle,  /* Sort handle.  */
                       const char *path,  /* File path.  */
                       UINT32 maxRecLen) /* Maximum record length.  */
{
  int recLen;
  FILE *inFile;
  char *record;
  inFile = fopen (path, "r");
  if (inFile == (FILE *)NULL)
    return 1;
  record = (char *)malloc (maxRecLen + 2);  /* Rec + \n + \0.  */
  if (record == (char *)NULL)

    {
      fclose (inFile);
      return 2;
    }
  while (fgets (record, maxRecLen + 2, inFile) != NULL)
    {
      recLen = strlen (record);
      if (record[recLen - 1] == '\n')
        recLen --;
      blir_sort_input_record (sortHandle, record, recLen);
    }
  free (record);
  fclose (inFile);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Sort_input_file_Fix - input fixed length records                   *
 *                                                                    *
 *********************************************************************  */


static UINT32
sort_input_file_Fix (sort_control_ptr sortHandle,  /* Sort handle.  */
                     const char *path,  /* File path.  */
                     UINT32 recLen) /* Record length.  */
{
  FILE *inFile;  /* Input file.  */
  char *record;  /* Input record.  */

  inFile = fopen (path, "rb");
  if (inFile == (FILE *)NULL)
    return 1;
  record = (char *)malloc (recLen);

  if (record == (char *)NULL)
    {
      fclose (inFile);
      return 2;
    }

  while (fread (record, sizeof (char), recLen, inFile) == (size_t)recLen)
    {
      blir_sort_input_record (sortHandle, record, recLen);
    }

  free (record);
  fclose (inFile);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Sort_input_file_VarRDW - Variable length, ASCII RDW                *
 *                                                                    *
 *********************************************************************  */


static UINT32
sort_input_file_VarRDW (sort_control_ptr sortHandle,  /* Sort handle.  */
                        const char *path,  /* File path.  */
                        UINT32 maxRecLen) /* Maximum record length.  */
{
  FILE *inFile;  /* Input file.  */
  char *record;  /* Record.  */
  UINT32 recLen;  /* Record length.  */
  unsigned char rdw[4];  /* ASCII RDW.  */


/*--------------------------------------------------------------------*
 * Open files and allocate record area                                *
 *--------------------------------------------------------------------.  */

  inFile = fopen (path, "rb");
  if (inFile == (FILE *)NULL)
    return 1;
  record = (char *)malloc (maxRecLen);
  if (record == (char *)NULL)

    {
      fclose (inFile);
      return 2;
    }


/*--------------------------------------------------------------------*
 * Read records using ASCII RDW for length                            *
 *--------------------------------------------------------------------.  */

  while (fread (rdw, sizeof (char), sizeof (rdw), inFile) == sizeof (rdw))

    {
      recLen = rdw[0];
      recLen = (recLen * 256) + rdw[1];
      recLen -= 4;
      if (fread (record, sizeof (char), recLen, inFile) == (size_t)recLen)
        {
          blir_sort_input_record (sortHandle, record, recLen);
        }
    }

/*--------------------------------------------------------------------*
 * Close file and free record                                         *
 *--------------------------------------------------------------------.  */


  free (record);
  fclose (inFile);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_input_file - supply input file for sorting               *
 *                                                                    *
 *********************************************************************  */

UINT32 blir_sort_input_file (void *handle,  /* Sort handle.  */
                             const char *path,  /* File path.  */
                             blir_sort_type fileType,  /* File type.  */
                             UINT32 recLen) /* Record length for fixed files.  */
{
  sort_control_ptr sortHandle = (sort_control_ptr)handle;  /* Sort handle.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);
  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);

  switch (fileType)

    {
    case blir_sort_file_Fix :
      return sort_input_file_Fix (sortHandle, path, recLen);
    case blir_sort_file_VarNL :
      return sort_input_file_VarNL (sortHandle, path, recLen);
    case blir_sort_file_VarRD :
      return sort_input_file_VarRDW (sortHandle, path, recLen);
    }

  return 1;
}

/**********************************************************************
 *                                                                    *
 * Sort_sortMerge - merge sort2/sort3 onto sort1                      *
 *                                                                    *
 *********************************************************************  */


static int
sort_sortMerge (sort_control_ptr sortHandle) /* Sort handle.  */
{
  int runs;  /* Number of runs.  */
  int eof1;  /* End of split file1, e.g.  sort2.  */
  int eof2;  /* End of split file2, e.g.  sort3.  */
  int need1;  /* Need record on file1.  */
  int need2;  /* Need record on file2.  */
  char *record1;  /* Record from file1.  */
  char *record2;  /* Record from file2.  */
  char *recordb;  /* Current base record for testing.  */
  char *swap;  /* Used to swap records.  */
  int c;  /* Compare results.  */
  UINT32 maxRecSize;  /* Maximum record size.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

/*--------------------------------------------------------------------*
 * Allocate records and open files                                    *
 *--------------------------------------------------------------------.  */

  maxRecSize = sortHandle->maxRecSize;

  record1 = (char *)malloc (maxRecSize);
  record2 = (char *)malloc (maxRecSize);
  recordb = (char *)malloc (maxRecSize);

  sortHandle->sort1 =
    blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp1, "wb");

  sortHandle->sort2 =
    blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp2, "rb");

  sortHandle->sort3 =
    blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp3, "rb");

  sortHandle->sortFilesUsed = True;


/*--------------------------------------------------------------------*
 * Issue the first read                                               *
 *--------------------------------------------------------------------.  */


  eof1 = eof2 = 0;
  need1 = need2 = 0;

  if (blir_sort_io_read (record1, maxRecSize, sortHandle->sort2) == 0)
    eof1 = 1;

  if (blir_sort_io_read (record2, maxRecSize, sortHandle->sort3) == 0)
    eof2 = 1;

  runs = 1;
  c = qsortPrefix_compare (sortHandle, record1, record2);
  if (c < 0)
    memcpy (recordb, record1, maxRecSize);
  else
    memcpy (recordb, record2, maxRecSize);


/*--------------------------------------------------------------------*
 * Process while both input files have data                           *
 *--------------------------------------------------------------------.  */

  while ((eof1 == 0) && (eof2 == 0))

    {
      c = qsortPrefix_compare (sortHandle, record1, record2);
      if (c < 0)

        {
          c = qsortPrefix_compare (sortHandle, record1, recordb);
          if (c >= 0)

            {
              blir_sort_io_write (record1, sortHandle->sort1);
              swap = record1;
              record1 = recordb;
              recordb = swap;
              need1 = 1;
            }

          else

            {
              c = qsortPrefix_compare (sortHandle, record2, recordb);
              if (c >= 0)

                {
                  blir_sort_io_write (record2, sortHandle->sort1);
                  swap = record2;
                  record2 = recordb;
                  recordb = swap;
                  need2 = 1;
                }

              else

                {
                  runs ++;
                  memcpy (recordb, record1, maxRecSize);
                }
            }
        }

      else

        {
          c = qsortPrefix_compare (sortHandle, record2, recordb);
          if (c >= 0)

            {
              blir_sort_io_write (record2, sortHandle->sort1);
              swap = record2;
              record2 = recordb;
              recordb = swap;
              need2 = 1;
            }

          else

            {
              c = qsortPrefix_compare (sortHandle, record1, recordb);
              if (c >= 0)

                {
                  blir_sort_io_write (record1, sortHandle->sort1);
                  swap = record1;
                  record1 = recordb;
                  recordb = swap;
                  need1 = 1;
                }

              else

                {
                  runs ++;
                  memcpy (recordb, record2, maxRecSize);
                }
            }
        }

/*--------------------------------------------------------------------*
 * Need input record from file 1                                      *
 *--------------------------------------------------------------------.  */

      if (need1)

        {
          need1 = 0;
          if (blir_sort_io_read (record1, maxRecSize, sortHandle->sort2) == 0)
            eof1 = 1;
        }

/*--------------------------------------------------------------------*
 * Need input record from file 2                                      *
 *--------------------------------------------------------------------.  */

      if (need2)

        {
          need2 = 0;
          if (blir_sort_io_read (record2, maxRecSize, sortHandle->sort3) == 0)
            eof2 = 1;
        }
    }


/*--------------------------------------------------------------------*
 * Process any remaining data in file 1                               *
 *--------------------------------------------------------------------.  */

  while (!eof1)

    {
      c = qsortPrefix_compare (sortHandle, record1, recordb);
      if (c < 0)
        runs ++;
      blir_sort_io_write (record1, sortHandle->sort1);
      swap = record1;
      record1 = recordb;
      recordb = swap;
      if (blir_sort_io_read (record1, maxRecSize, sortHandle->sort2) == 0)
        eof1 = 1;
    }

/*--------------------------------------------------------------------*
 * Process any remaining data in file 2                               *
 *--------------------------------------------------------------------.  */

  while (!eof2)

    {
      c = qsortPrefix_compare (sortHandle, record2, recordb);
      if (c < 0)
        runs ++;
      blir_sort_io_write (record2, sortHandle->sort1);
      swap = record2;
      record2 = recordb;
      recordb = swap;
      if (blir_sort_io_read (record2, maxRecSize, sortHandle->sort3) == 0)
        eof2 = 1;
    }

/*--------------------------------------------------------------------*
 * Close files and free records                                       *
 *--------------------------------------------------------------------.  */

  blir_sort_io_close (sortHandle->sort1);
  sortHandle->sort1 = FILECTL_NULL;

  blir_sort_io_close (sortHandle->sort2);
  sortHandle->sort2 = FILECTL_NULL;

  blir_sort_io_close (sortHandle->sort3);
  sortHandle->sort3 = FILECTL_NULL;

  free (record1);
  free (record2);
  free (recordb);

  return runs;
}


/**********************************************************************
 *                                                                    *
 * Sort_sortSplit - Split sort1 into sort2/sort3                      *
 *                                                                    *
 *********************************************************************  */


static int
sort_sortSplit (sort_control_ptr sortHandle) /* Sort handle.  */
{
  int eof;  /* End of file flag.  */
  char *record0;  /* Record.  */
  char *recordb;  /* Base record.  */
  int c;  /* Compare results.  */
  fileCtl_ptr fcb_id;  /* File pointer.  */
  UINT32 maxRecSize;  /* Maximum record size.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);


/*--------------------------------------------------------------------*
 * Allocate record areas                                              *
 *--------------------------------------------------------------------.  */


  maxRecSize = sortHandle->maxRecSize;
  record0 = (char *)malloc (maxRecSize);
  recordb = (char *)malloc (maxRecSize);

/*--------------------------------------------------------------------*
 * Open files                                                         *
 *--------------------------------------------------------------------.  */

  sortHandle->sort1 =
    blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp1, "rb");

  sortHandle->sort2 =
    blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp2, "wb");

  sortHandle->sort3 =
    blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp3, "wb");

  sortHandle->sortFilesUsed = True;

/*--------------------------------------------------------------------*
 * Set the current output file to be sort2                            *
 *--------------------------------------------------------------------.  */


  fcb_id = sortHandle->sort2;


/*--------------------------------------------------------------------*
 * Read first record                                                  *
 *--------------------------------------------------------------------.  */


  if (blir_sort_io_read (record0, maxRecSize, sortHandle->sort1) == 0)
    eof = 1;
  else
    eof = 0;

  memcpy (recordb, record0, maxRecSize);


/*--------------------------------------------------------------------*
 * Read until end of sort1, e.g.  until all the data has been spread   *
 * over sort2 and sort3.                                              *
 *--------------------------------------------------------------------.  */


  while (!eof)

    {
      c = qsortPrefix_compare (sortHandle, record0, recordb);
      if (c >= 0)
        blir_sort_io_write (record0, fcb_id);
      else
        {

/*--------------------------------------------------------------------*
 * Swap output files                                                  *
 *--------------------------------------------------------------------.  */

          if (fcb_id == sortHandle->sort2)
            fcb_id = sortHandle->sort3;
          else
            fcb_id = sortHandle->sort2;
          blir_sort_io_write (record0, fcb_id);
        }


/*--------------------------------------------------------------------*
 * Reset base                                                         *
 *--------------------------------------------------------------------.  */

      memcpy (recordb, record0, maxRecSize);
      if (blir_sort_io_read (record0, maxRecSize, sortHandle->sort1) == 0)
        eof = 1;
    }

/*--------------------------------------------------------------------*
 * Close up files and free work areas                                 *
 *--------------------------------------------------------------------.  */

  blir_sort_io_close (sortHandle->sort1);
  sortHandle->sort1 = FILECTL_NULL;

  blir_sort_io_close (sortHandle->sort2);
  sortHandle->sort2 = FILECTL_NULL;

  blir_sort_io_close (sortHandle->sort3);
  sortHandle->sort3 = FILECTL_NULL;

  free (record0);
  free (recordb);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Sort_sortData - Carry out the sort                                 *
 *                                                                    *
 *********************************************************************  */


static int
sort_sortData (sort_control_ptr sortHandle) /* Sort handle.  */
{
  int runs;  /* Number of runs.  */

  sort_EmptyBuffer (sortHandle);

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  blir_sort_io_close (sortHandle->sort2);
  sortHandle->sort2 = FILECTL_NULL;

  blir_sort_io_close (sortHandle->sort3);
  sortHandle->sort3 = FILECTL_NULL;


/*--------------------------------------------------------------------*
 * Repeat the Merge/Split until there is only one run                 *
 *--------------------------------------------------------------------.  */


  runs = sort_sortMerge (sortHandle);

  while (runs > 1)

    {
      sort_sortSplit (sortHandle);
      runs = sort_sortMerge (sortHandle);
    }

  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_output_record - return an output record                  *
 *                                                                    *
 *********************************************************************  */


UINT32 blir_sort_output_record (void *handle,  /* Sort handel.  */
                                void *record,  /* Record.  */
                                UINT32 maxRecSize) /* Maximum record size.  */
{
  sort_control_ptr sortHandle = (sort_control_ptr)handle;  /* Sort handle.  */
  UINT32 len;  /* Hold length.  */
  sortPfx_ptr prefix;  /* Sort prefix.  */
  char *recPtr;  /* Record pointer.  */


  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);
  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);


/*--------------------------------------------------------------------*
 * Check for first entry case after adding records                    *
 *--------------------------------------------------------------------.  */


  if (sortHandle->state == st_adding)
    {

/*--------------------------------------------------------------------*
 * No files, just use the array                                       *
 *--------------------------------------------------------------------.  */

      if (!sortHandle->sortFilesUsed)
        {
          if (sortHandle->curPtrCount == 0)
            {
              sortHandle->state = st_eof;
            }
          else
            {
              if (sortHandle->curPtrCount > 1)
                {
                  qsortPrefix (sortHandle, (void **)sortHandle->bufCurPtr, 0,
                               sortHandle->curPtrCount - 1);
                }
              sortHandle->state = st_retarray;
            }
        }

/*--------------------------------------------------------------------*
 * File sort                                                          *
 *--------------------------------------------------------------------.  */

      else
        {
          sort_sortData (sortHandle);
          sortHandle->sort1 =
            blir_sort_io_open (sortHandle->fileSysId, sortHandle->sortTemp1,
                               "rb");
          sortHandle->sortFilesUsed = True;
          sortHandle->state = st_retfile;
        }
    }


/*--------------------------------------------------------------------*
 * Process request based on current state                             *
 *--------------------------------------------------------------------.  */


  switch (sortHandle->state)
    {
    case st_null :
    case st_init :
    case st_adding :
      sortHandle->state = st_eof;
      return 0;
    case st_retarray :
      if (sortHandle->curPtrRec < sortHandle->curPtrCount)
        {
          recPtr = sortHandle->bufCurPtr[sortHandle->curPtrRec];
          prefix = (sortPfx_ptr)recPtr;
          recPtr += sizeof (sortPfx_type);
          len = prefix->sortPfx_length - sizeof (sortPfx_type);
          memcpy (record, recPtr, len);
          sortHandle->curPtrRec += 1;
        }
      else
        {
          sortHandle->state = st_eof;
          len = 0;
        }
      return len;
    case st_retfile :
      len = blir_sort_io_readPrefix (sortHandle->sort1);
      if (len == 0)

        {
          sortHandle->state = st_eof;
          blir_sort_io_close (sortHandle->sort1);
          sortHandle->sort1 = FILECTL_NULL;
          return 0;
        }
      if (len > maxRecSize)
        len = maxRecSize;
      len = blir_sort_io_readData (record, len, sortHandle->sort1);
      return len;
    case st_eof :
      return 0;
    default :
      sortHandle->state = st_eof;
      return 0;
    }
}


/**********************************************************************
 *                                                                    *
 * Sort_output_file_fix - output file, fixed length records           *
 *                                                                    *
 *********************************************************************  */


static int
sort_output_file_fix (sort_control_ptr sortHandle,  /* Sort handle.  */
                      const char *path,  /* File path.  */
                      UINT32 fixRecLen) /* Record length.  */
{
  FILE *oFile;  /* Output file.  */
  char *record;  /* Output record.  */
  UINT32 maxRecSize;  /* Maximum record size.  */
  UINT32 recLen;  /* Record length.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  oFile = fopen (path, "wb");
  maxRecSize = sortHandle->maxRecSize + 2;  /* \N \0.  */
  record = (char *)malloc (maxRecSize);
  while ((recLen = blir_sort_output_record (sortHandle, record, maxRecSize))
         > 0)

    {
      if (recLen < fixRecLen)

        {
          fwrite (record, sizeof (char), recLen, oFile);
          while (recLen < fixRecLen)

            {
              fwrite (" ", sizeof (char), 1, oFile);
              recLen ++;
            }
        }

      else

        {
          fwrite (record, sizeof (char), fixRecLen, oFile);
        }
    }
  free (record);
  fclose (oFile);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Sort_output_file_VarNL - Output file, variable with New Line       *
 *                                                                    *
 *********************************************************************  */

static int
sort_output_file_VarNL (sort_control_ptr sortHandle,  /* Sort handle.  */
                        const char *path) /* File path.  */
{
  FILE *oFile;  /* Output file.  */
  char *record;  /* Record.  */
  UINT32 maxRecSize;  /* Maximum record size.  */
  UINT32 recLen;  /* Record length.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  oFile = fopen (path, "w");
  maxRecSize = sortHandle->maxRecSize + 2;  /* \N \0.  */
  record = (char *)malloc (maxRecSize);
  while ((recLen = blir_sort_output_record (sortHandle, record, maxRecSize)) >
         0)
    {
      record[recLen] = '\0';
      fprintf (oFile, "%s\n", record);
    }
  free (record);
  fclose (oFile);
  return 0;
}

/**********************************************************************
 *                                                                    *
 * Sort_output_file_VarRD - File output, variable with RDW            *
 *                                                                    *
 *********************************************************************  */


static int
sort_output_file_VarRD (sort_control_ptr sortHandle,  /* Sort handle.  */
                        const char *path) /* File path.  */
{
  FILE *oFile;  /* Output file.  */
  char *record;  /* Record.  */
  UINT32 maxRecSize;  /* Maximum record size.  */
  UINT32 recLen;  /* Record length.  */
  unsigned char asciiRDW[4];  /* Working ASCII RDW.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);

  oFile = fopen (path, "wb");
  maxRecSize = sortHandle->maxRecSize + 2;  /* \N \0.  */
  record = (char *)malloc (maxRecSize);
  while ((recLen = blir_sort_output_record (sortHandle, record, maxRecSize))
         > 0)
    {
      asciiRDW[0] = (unsigned char)(recLen / 256);
      asciiRDW[1] = (unsigned char)(recLen % 256);
      asciiRDW[2] = 0;
      asciiRDW[3] = 0;
      fwrite (asciiRDW, sizeof (char), sizeof (asciiRDW), oFile);
      fwrite (record, sizeof (char), recLen, oFile);
    }
  free (record);
  fclose (oFile);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_output_file - specify sort output file                   *
 *                                                                    *
 *********************************************************************  */


UINT32 blir_sort_output_file (void *handle,  /* Sort handle.  */
                              const char *path,  /* File path.  */
                              blir_sort_type fileType,  /* File type.  */
                              UINT32 recLen) /* Record length - for fixed.  */
{
  sort_control_ptr sortHandle = (sort_control_ptr)handle;  /* Sort handle.  */

  BLI_ASSERT (sortHandle != SORT_CONTROL_NULL);
  BLI_ASSERT (memcmp
          (sortHandle->footprint, SORT_FOOTPRINT,
           sizeof (sortHandle->footprint)) == 0);

  switch (fileType)
    {
    case blir_sort_file_Fix :
      return sort_output_file_fix (sortHandle, path, recLen);
    case blir_sort_file_VarNL :
      return sort_output_file_VarNL (sortHandle, path);
    case blir_sort_file_VarRD :
      return sort_output_file_VarRD (sortHandle, path);
    }

  return 1;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_sequence - Intialize a sort sequence                     *
 *                                                                    *
 *********************************************************************  */


unsigned char *
blir_sort_sequence (unsigned char *Sequence,  /* Sequence to initialize [256 bytes].  */
                    blir_alphabet_name alphabet) /* Alphabet name.  */
{
  int i;  /* Index.  */

  switch (alphabet)
    {
    case alphabet_none :
    case alphabet_native :
      for (i = 0; i < 256; i ++)
        Sequence[i] = (unsigned char)i;
      break;
    case alphabet_ascii :
      blir_compare_sequence_ASCII (Sequence);
      break;
    case alphabet_ebcdic :
      blir_compare_sequence_EBCDIC (Sequence);
      break;
    case alphabet_standard1 :
      blir_compare_sequence_ASCII (Sequence);
      break;
    case alphabet_standard2 :
      blir_compare_sequence_ISO7bit (Sequence);
      break;
    }

  return Sequence;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_merge_file - supply input file for merging               *
 *                                                                    *
 *********************************************************************  */

UINT32 blir_sort_merge_file (void *handle,  /* Sort handle.  */
                             const char *path,  /* File path.  */
                             blir_sort_type fileType,  /* File type.  */
                             UINT32 recLen) /* Record length for fixed files.  */
{
  return blir_sort_input_file (handle, path, fileType, recLen);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_merge_record - merge input record                        *
 *                                                                    *
 *********************************************************************  */


UINT32
blir_sort_merge_record (void *handle,  /* Sort handel.  */
                        void *record,  /* Record.  */
                        UINT32 size) /* Record size.  */
{
  return blir_sort_input_record (handle, record, size);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_merge_array - merge array                                *
 *                                                                    *
 *********************************************************************  */

UINT32 blir_sort_merge_array (void *handle,  /* Sort handle.  */
                              void **array,  /* Array of pointers.  */
                              UINT32 recLen,  /* Record length.  */
                              UINT32 arrayLen) /* Length of records.  */
{
  return blir_sort_input_array (handle, array, recLen, arrayLen);
}
