

/**********************************************************************
 *                                                                    *
 *                     BLISS Sort I/O primitives                      *
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
 * Title: BLISS Sort I/O primitives                                   *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blir_temp_config.h"
#include "blir_sort_io.h"


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_Init - Intialize a virtual file system                *
 *                                                                    *
 *********************************************************************  */


fileSys_ptr blir_sort_io_Init (void)
{
  fileSys_ptr fileSys_id;
  fileSys_id = (fileSys_ptr)malloc (sizeof (fileSys_type));
  fileSys_id->highRecNum = 0;
  fileSys_id->removes = 0;
  return fileSys_id;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_Wrap - Terminate a virtual file system                *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_Wrap (fileSys_ptr fileSys_id) /* File System Id.  */
{
  if (fileSys_id == FILESYS_NULL)
    return 0;
  free (fileSys_id);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_feof - Test for end of file                           *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_feof (fileCtl_ptr fileCtl_id) /* File id.  */
{
  return feof (fileCtl_id->file);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_ferror - Test for error of file                       *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_ferror (fileCtl_ptr fileCtl_id) /* File id.  */
{
  return ferror (fileCtl_id->file);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_open - Open file                                      *
 *                                                                    *
 *********************************************************************  */


fileCtl_ptr /* Returned file id.  */
  blir_sort_io_open (fileSys_ptr fileSys_id,  /* Virtual system id.  */
                     const char *path,  /* File path.  */
                     const char *mode) /* Open mode.  */
{
  fileCtl_ptr fileCtl_id;
  FILE *file;
  int iotype;
  if (fileSys_id == FILESYS_NULL)
    return FILECTL_NULL;
  if (path == (char *)NULL)
    return FILECTL_NULL;
  if (mode == (char *)NULL)
    return FILECTL_NULL;
  if (strcmp (mode, "rb") == 0)

    {
      file = fopen (path, "rb");
      if (file == (FILE *)NULL)
        return FILECTL_NULL;
      iotype = 0;
    }

  else if (strcmp (mode, "wb") == 0)

    {
      file = fopen (path, "wb");
      if (file == (FILE *)NULL)
        return FILECTL_NULL;
      iotype = 1;
    }

  else
    return FILECTL_NULL;
  fileCtl_id = (fileCtl_ptr)malloc (sizeof (fileCtl_type));
  if (fileCtl_id == FILECTL_NULL)
    return FILECTL_NULL;
  fileCtl_id->fileSys_id = fileSys_id;
  fileCtl_id->file = file;
  if (iotype)
    fileCtl_id->iotype = io_write;

  else
    fileCtl_id->iotype = io_read;
  return fileCtl_id;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_close - close file                                    *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_close (fileCtl_ptr fileCtl_id) /* File id.  */
{
  if (fileCtl_id == FILECTL_NULL)
    return 1;
  if (fileCtl_id->file != (FILE *)NULL)
    fclose (fileCtl_id->file);
  free (fileCtl_id);
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_read - read record from file                          *
 *                                                                    *
 *********************************************************************  */


int /* Bytes read.  */
blir_sort_io_read (void *record,  /* Record return area.  */
                   UINT32 size,  /* Number of bytes to read.  */
                   fileCtl_ptr fileCtl_id) /* File id.  */
{
  sortPfx_ptr prefix;
  char *recData;
  UINT32 dataSize;
  UINT32 bytes;
  if (record == (void *)NULL)
    return 0;
  if (size < (sizeof (sortPfx_type) + 1))
    return 0;
  if (fileCtl_id == FILECTL_NULL)
    return 0;
  if (fileCtl_id->iotype != io_read)
    return 0;
  prefix = (sortPfx_ptr)record;
  bytes =
    fread (prefix, sizeof (char), sizeof (sortPfx_type), fileCtl_id->file);
  if (bytes != sizeof (sortPfx_type))
    return 0;
  if (size < prefix->sortPfx_length)
    return 0;
  dataSize = prefix->sortPfx_length - sizeof (sortPfx_type);
  recData = (char *)record;
  recData += sizeof (sortPfx_type);
  bytes = fread (recData, sizeof (char), dataSize, fileCtl_id->file);
  if (bytes != dataSize)
    return 0;
  return prefix->sortPfx_length;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_readPrefix - Read a record prefix                     *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_readPrefix (fileCtl_ptr fileCtl_id) /* File id.  */
{
  sortPfx_type prefix;
  UINT32 bytes;
  if (fileCtl_id == FILECTL_NULL)
    return 0;
  if (fileCtl_id->iotype != io_read)
    return 0;
  bytes =
    fread ( & prefix, sizeof (char), sizeof (sortPfx_type), fileCtl_id->file);
  if (bytes != sizeof (sortPfx_type))
    return 0;
  return prefix.sortPfx_length - sizeof (prefix);
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_readData - Read record data                           *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_readData (void *record,  /* Record area.  */
                       UINT32 size,  /* Size.  */
                       fileCtl_ptr fileCtl_id) /* File id.  */
{
  UINT32 bytes;
  if (record == (void *)NULL)
    return 0;
  if (fileCtl_id == FILECTL_NULL)
    return 0;
  if (fileCtl_id->iotype != io_read)
    return 0;
  bytes = fread (record, sizeof (char), size, fileCtl_id->file);
  if (bytes != size)
    return 0;
  return size;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_write - write a record                                *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_write (void *record,  /* Record area with prefix.  */
                    fileCtl_ptr fileCtl_id) /* File id.  */
{
  sortPfx_ptr prefix;
  UINT32 bytes;
  if (record == (void *)NULL)
    return 0;
  if (fileCtl_id == FILECTL_NULL)
    return 0;
  if (fileCtl_id->iotype != io_write)
    return 0;
  prefix = (sortPfx_ptr)record;
  bytes =
    fwrite (record, sizeof (char), prefix->sortPfx_length, fileCtl_id->file);
  if (bytes != prefix->sortPfx_length)
    return 0;
  return prefix->sortPfx_length;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_delete - delete a file                                *
 *                                                                    *
 *********************************************************************  */


int
blir_sort_io_delete (fileSys_ptr fileSys_id,  /* File system id.  */
                     const char *path) /* File name.  */
{
  remove (path);
  fileSys_id->removes ++;
  return 0;
}


/**********************************************************************
 *                                                                    *
 * Cobr_sort_io_highRecNum - Return high record number                *
 *                                                                    *
 *********************************************************************  */


UINT32 /* High number.  */
  blir_sort_io_highRecNum (fileSys_ptr fileSys_id) /* File system id.  */
{
  if (fileSys_id == FILESYS_NULL)
    return 0;
  return fileSys_id->highRecNum ++;
}
