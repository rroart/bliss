

/**********************************************************************
 *                                                                    *
 *                       BLISS Sort Header File                       *
 *                                                                    *
 *           Copyright (C) 2000 by Theodore J.  Seward, Jr.            *
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
 * Mail: Theodore J. Seward, Jr.                                      *
 *                 Apt. 343,                                          *
 *                 2008 Deerpark Place,                               *
 *                 Fullerton, CA  92831                               *
 *                                                                    *
 * E-mail: tseward@compuserve.com                                     *
 *                                                                    *
 *********************************************************************  */


/**********************************************************************
 *                                                                    *
 * Title: BLISS Sort Header File                                      *
 *                                                                    *
 * Description: This file is part of the BLISS Header Function.       *
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

#ifndef __BLIR_SORT_H__
#define __BLIR_SORT_H__

/**********************************************************************
 *                                                                    *
 * Sort Return codes                                                  *
 *                                                                    *
 *********************************************************************  */

typedef enum
{
  blir_sort_return_normal = 0,  /* Normal return.  */
  blir_sort_return_error = 1 /* Error return.  */
}
blir_sort_return;  /* Return types.  */

/**********************************************************************
 *                                                                    *
 * Global constants                                                   *
 *                                                                    *
 *********************************************************************  */

#define SORT_DEFBUFSIZE (32*1024)       /* Default buffer size */

#define SORT_SORTTEMP1  "SORT0001.TMP"  /* Temporary sort data set 1 */
#define SORT_SORTTEMP2  "SORT0002.TMP"  /* Temporary sort data set 2 */
#define SORT_SORTTEMP3  "SORT0003.TMP"  /* Temporary sort data set 3 */

/**********************************************************************
 *                                                                    *
 * Sort Direction constants                                           *
 *                                                                    *
 *********************************************************************  */


#define BLIR_SORT_DIR_ASCENDING      'A'        /* sort on field ascending */
#define BLIR_SORT_DIR_DESCENDING     'D'        /* sort on field descending */


/**********************************************************************
 *                                                                    *
 * Sort field description                                             *
 *                                                                    *
 *********************************************************************  */


struct blir_sort_field_desc
{
  UINT32 offset;  /* Offset to field.  */
  UINT32 length;  /* Length of field.  */
  UINT32 decimals;  /* Decimal digits.  */
  UINT32 type;  /* Field type.  */
  UINT32 flags;  /* Field flags.  */
  UINT32 direction;  /* Sort direction.  */
};

typedef struct blir_sort_field_desc blir_sort_field_desc_type;  /* Sort field type.  */
typedef blir_sort_field_desc_type * blir_sort_field_desc_ptr;  /* Sort field pointer.  */
#define SORT_FIELD_DESC_NULL ((blir_sort_field_desc_ptr)NULL)   /* NULL sort pointer */

/**********************************************************************
 *                                                                    *
 * Alphabet Names                                                     *
 *                                                                    *
 *********************************************************************  */

typedef enum
{
  alphabet_none,  /* Sequence not specified.  */
  alphabet_ascii,  /* ASCII Alphabet.  */
  alphabet_ebcdic,  /* EBCDIC Alphabet.  */
  alphabet_native,  /* Native Alphabet.  */
  alphabet_standard1,  /* ASCII Standard.  */
  alphabet_standard2 /* ISO-7 Standard.  */
}
blir_alphabet_name;

/**********************************************************************
 *                                                                    *
 * Sort File Types                                                    *
 *                                                                    *
 *********************************************************************  */

typedef enum
{
  blir_sort_file_Fix,  /* Data type is file of fixed length records.  */
  blir_sort_file_VarNL,  /* Data type is file of variable length records using New Line delimiters.  */
  blir_sort_file_VarRD /* Data type is file of variable length records using ANSI RDW.  */
}
blir_sort_type;  /* Sort data types.  */


/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */

blir_sort_return /* Return code.  */
  blir_sort_array (void *handle, /* Handle returned by blir_sort_init.  */
                   void **records,  /* Array of item pointers to sort.  */
                   UINT32 length,  /* Length of records.  */
                   UINT32 count);  /* Number of items.  */

void * /* Returns handle.  */
                    blir_sort_init (UINT32 sort_field_count,  /* Number of fields.  */
                                    struct blir_sort_field_desc *sort_fields,  /* Fields.  */
                                    unsigned char *sequence,  /* Collating sequence, NULL = default.  */
                                    char *temp_path,  /* Temporary path, NULL = none.  */
                                    UINT32 sortBufSize);  /* Sort buffer size, 0 = default value.  */

int blir_sort_wrap (void *handle);

UINT32 blir_sort_input_record (void *handle, void *record,
                                            UINT32 size);

UINT32 blir_sort_input_array (void *handle, void **array,
                                           UINT32 recLen, UINT32 arrayLen);

UINT32 blir_sort_input_file (void *handle, const char *path,
                                          blir_sort_type fileType,
                                          UINT32 recLen);

UINT32 blir_sort_merge_file (void *handle,  /* Sort handle.  */
                             const char *path,  /* File path.  */
                             blir_sort_type fileType,  /* File type.  */
                             UINT32 recLen);  /* Record length for fixed files.  */

UINT32 blir_sort_merge_record (void *handle,  /* Sort handle.  */
                               void *record,  /* Record.  */
                               UINT32 size);  /* Record size.  */


UINT32 blir_sort_merge_array (void *handle,  /* Sort handle.  */
                              void **array,  /* Array of pointers.  */
                              UINT32 recLen,  /* Record length.  */
                              UINT32 arrayLen);  /* Length of records.  */

UINT32 blir_sort_output_record (void *handle, void *record,
                                             UINT32 maxRecSize);

UINT32 blir_sort_output_file (void *handle, const char *path,
                                           blir_sort_type fileType,
                                           UINT32 recLen);

unsigned char *blir_sort_sequence (unsigned char *Sequence,  /* Sequence to initialize [256 bytes].  */
                                        blir_alphabet_name alphabet);  /* Alphabet name.  */

#endif
