

/**********************************************************************
 *                                                                    *
 *               BLISS Sort I/O primitives header file                *
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
 * Title: BLISS Sort I/O primitive header file                        *
 *                                                                    *
 * Description: This file is part of the BLISS Sort Function.         *
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

#ifndef __BLIR_SORT_IO_H__
#define __BLIR_SORT_IO_H__


/**********************************************************************
 *                                                                    *
 * File System Area                                                   *
 *                                                                    *
 *********************************************************************  */


struct fileSys
{
  UINT32 highRecNum;
  UINT32 removes;
};
typedef struct fileSys fileSys_type;  /* FileSys type.  */
typedef fileSys_type * fileSys_ptr;  /* FileSys pointer.  */
#define FILESYS_NULL  ((fileSys_ptr)NULL)       /* fileSys null pointer */


/**********************************************************************
 *                                                                    *
 * File Control area                                                  *
 *                                                                    *
 *********************************************************************  */


typedef enum
{
  io_close = 0,  /* File closed.  */
  io_read,  /* File open for input.  */
  io_write /* File open for output.  */
}
fileCtl_iotypes;  /* Open type.  */
struct fileCtl
{
  fileSys_ptr fileSys_id;  /* System id.  */
  FILE *file;  /* File pointer.  */
  fileCtl_iotypes iotype;  /* Open types.  */
};

typedef struct fileCtl fileCtl_type;  /* FileCtl type.  */
typedef fileCtl_type * fileCtl_ptr;  /* FileCtl pointer.  */
#define FILECTL_NULL ((fileCtl_ptr)NULL)        /* fileCtl null pointer */


/**********************************************************************
 *                                                                    *
 * Sort record prefix                                                 *
 *                                                                    *
 *********************************************************************  */


struct sortPfx
{
  UINT32 sortPfx_length;
  UINT32 sortPfx_number;
};
typedef struct sortPfx sortPfx_type;  /* SortPfx type.  */
typedef sortPfx_type * sortPfx_ptr;  /* SortPfx pointer.  */
#define SORTPFX_NULL ((sortPfx_ptr)NULL)        /* sortPfx null pointer */


/**********************************************************************
 *                                                                    *
 *                             Prototypes                             *
 *                                                                    *
 *********************************************************************  */


fileSys_ptr blir_sort_io_Init (void);
int blir_sort_io_Wrap (fileSys_ptr fileSys_id);
fileCtl_ptr blir_sort_io_open (fileSys_ptr fileSys_id,
                                       const char *path, const char *mode);
int blir_sort_io_close (fileCtl_ptr fileCtl_id);
int blir_sort_io_read (void *record, UINT32 size,
                                       fileCtl_ptr fileCtl_id);
int blir_sort_io_readPrefix (fileCtl_ptr fileCtl_id);
int blir_sort_io_readData (void *record, UINT32 size,
                                           fileCtl_ptr fileCtl_id);
int blir_sort_io_write (void *record, fileCtl_ptr fileCtl_id);
int blir_sort_io_feof (fileCtl_ptr fileCtl_id);
int blir_sort_io_ferror (fileCtl_ptr fileCtl_id);
int blir_sort_io_delete (fileSys_ptr fileSys_id,
                                         const char *name);
UINT32 blir_sort_io_highRecNum (fileSys_ptr fileSys_id);

#endif
