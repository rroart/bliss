/* 
 
   BLISS Compiler utility message handling routines (bliumsg.c)

   ---------------------------------------------------------------------------

   Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  
   
   ---------------------------------------------------------------------------.  */

#include "config.h"
#include "system.h"
#include "diagnostic.h"
#include "libiberty.h"

#include "blir_temp_config.h"
#include "blidebug.h"

#include "ansidecl.h"
#include "ggc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliumsc.h"
/*#include "blicc1.h"*/

#include <stdlib.h>
#include <stdio.h>

/* File names.  */
dynarray_uchar_ptr *file_names = NULL;
/* File name lengths.  */
dynarray_uint32 *file_names_lengths = NULL;
/* ... Copied from ...  string for that file including whole hierarchy.  */
dynarray_uchar_ptr *file_names_copied = NULL;
/* Length of above.  */
dynarray_uint32 *file_names_copied_lengths = NULL;
/* File name hierarchy in concise form (ie not one line per file).  */
dynarray_uchar_ptr *file_names_copied_original = NULL;
/* Length of above.  */
dynarray_uint32 *file_names_copied_original_lengths = NULL;

uint32 file_count = 0;  /* Number of files = number of last file (number 0 is not used.  */

/* Flag to say whether to output error messages right away or saving
   them and outputting them in sorted order at the end.  */

extern int32 option_immediate_msg_output;

/* Stack of file numbers in COPYs.  */
uint32 * work_array = NULL;

/* All the info about error messages.  */

extern struct bli_message_info message_info_array[]; 

/* The max message ID number.  */

static uint32 max_message_id_nbr = 0;

/* Pointers to the messages, indexed by message number.  */

struct GTY(()) dynarray_message_info_ptr_struct 
{
  struct dynarray_struct dynarray_details;
  struct bli_message_info ** GTY((length ("%1.dynarray_details.entry_count"))) elem;
};

struct dynarray_message_info_ptr_struct *message_info_ptr = NULL;

/* Flag to say whether to output error messages right away ve saving
   them and outputting them in sorted order at the end.  */

extern int32 option_immediate_msg_output;

/* Save an error number MESSAGE_NBR message TEXT for later output, file_nbr, line_nbr,
   char_nbr tell us which part of the input text the error relates to
   so we can sort the messages.  */

/* How many messages there are to output.  */
static uint32 message_accumulation_count = 0;

/* The messages to output.  */

struct GTY(()) dynarray_message_occurrence_struct 
{
  struct dynarray_struct dynarray_details;
  message_occurrence * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

struct dynarray_message_occurrence_struct *message_accumulation = NULL;

/* Count of messages output.  */

static int32 message_output_count = 0;

/* Output a message MSG.  */

static void
output_msg (message_occurrence *msg);

static void
output_msg (message_occurrence *msg)
{
  const uchar *warning;
  static uint32 last_file_nbr = 0xffffffffU;

  message_output_count ++;

  if (last_file_nbr != msg->file_nbr)
    {
      last_file_nbr = msg->file_nbr;
      if ((msg->file_nbr) && (file_names_copied->elem[msg->file_nbr]))
        fprintf (stderr, "\nIn file copied from %s", file_names_copied->elem[msg->file_nbr]);
      else
        fprintf (stderr, "\nIn main file");
    }

  if ((message_info_ptr->elem[msg->message_id_nbr]->fatal_flag)
      || (message_info_ptr->elem[msg->message_id_nbr]->error_flag)
      || (message_info_ptr->elem[msg->message_id_nbr]->filename))
    warning = (uchar *)"";
  else
    warning = (uchar *)"warning: ";
  
  if (msg->char_nbr)
    {
      fprintf (stderr, "\n%s:%i:%i: %s%s", 
              file_names->elem[msg->file_nbr],
              msg->line_nbr,
              msg->char_nbr,
              warning,
              msg->text
              );
    }
  else
    if (msg->line_nbr)
      {
        fprintf (stderr, "\n%s:%i: %s%s", 
                file_names->elem[msg->file_nbr],
                msg->line_nbr,
                warning,
                msg->text
                );
      }
    else
      if (msg->file_nbr)
        {
          fprintf (stderr, "\n%s: %s%s",
                  file_names->elem[msg->file_nbr],
                  warning,
                  msg->text
                  );
        }
      else
        {
          int32 ix;
          int32 warning_sent = 0;
          fprintf (stderr, "\n");
          for (ix = 0; msg->text[ix]; ix ++)
            {
              fprintf (stderr, "%c", msg->text[ix]);
              if (msg->text[ix] == ' ')
                if (!warning_sent)
                  {
                    fprintf (stderr, "%s", (native_char*)warning);
                    warning_sent ++;
                  }
            }
          if (!warning_sent)
            fprintf (stderr, " - (%s)", warning);
        }
  if (msg->reading_text)
    fprintf (stderr, ", reading (%s)",
            msg->reading_text
            );
  return;
}

/* Record a message with i.d.  MESSAGE_NBR from the FILE_NBRth file
opened related to line LINE_NBR and column native_char_NBR. TEXT overrides
the default message text for that message_id if not NULL. Error if no
default text and no supplied text.  */

void
record_msg (message_id_nbr, file_nbr, line_nbr, char_nbr, text, reading_text)
     uint32 message_id_nbr; 
     uint32 file_nbr;
     uint32 line_nbr;
     uint32 char_nbr; 
     const uchar *text;
     const uchar *reading_text;
{
  message_occurrence *msg;
  struct bli_message_info *msg_info;
  uint32 text_size;
  uchar *text_address;
     
  BLI_ASSERT (((uint32)message_id_nbr) <= max_message_id_nbr);
  msg_info = message_info_ptr->elem[message_id_nbr];
  BLI_ASSERT (msg_info);
  BLI_ASSERT (msg_info->message_id_nbr);

  if (!msg_info->output_flag)
    return;
  
  if ((msg_info->error_flag)
       || (msg_info->fatal_flag))
    errorcount ++;
  else
    if (!msg_info->filename)
      warningcount ++;
  
  BLI_ASSERT (message_accumulation);

  GROW_DYN (&message_accumulation->dynarray_details, message_accumulation_count);
  BLI_ASSERT (text || msg_info->message_text);

  msg = message_accumulation->elem + message_accumulation_count;

  message_accumulation_count ++;

  if (text)
    msg->text = text;
  else
    msg->text = msg_info->message_text;

  /* Zack Weinberg removed the ability to have things that may or may
     not be in GC-able storage, so we have to copy everything now.  */

  if (msg->text)
    {
      text_size = strlen ((native_char *)msg->text);
      text_address = BLI_ALLOC (text_size +1);
      text_address[text_size] = 0;
      memcpy (text_address, msg->text, text_size);
      msg->text = text_address;
    }
  else
    {
      BLI_ASSERT (msg->text);
    }
  
  msg->file_nbr = file_nbr; 
  msg->line_nbr = line_nbr; 
  msg->char_nbr = char_nbr; 
  if (reading_text)
    {
      text_size = strlen ((native_char *)reading_text);
      text_address = BLI_ALLOC (text_size +1);
      text_address[text_size] = 0;
      memcpy (text_address, reading_text, text_size);
      msg->reading_text = text_address; 
    }
  else
    {
      BLI_ASSERT (!msg->reading_text);
    }
  msg->message_id_nbr = msg_info->message_id_nbr;

  if ((option_immediate_msg_output) || (msg_info->immediate))
    {
      output_msg (msg);
      /* Cancel previous update of message accumulation count.  */
      message_accumulation_count --;
    }

}

/* Save an error number MESSAGE_NBR message TEXT for a token TOKEN for
   later output.  The preprocessor and the main compiler have a
   different idea of what a token is; thus the void* token.  */

void
record_msg_token (message_nbr, token, text)

     uint32 message_nbr;
     void *token;
     uchar *text;
{
  uint32 file_nbr;
  uint32 line_nbr;
  uint32 char_nbr;
  uchar *token_text;
  
  BLI_GET_TOKEN_DETAILS (token, file_nbr, line_nbr, char_nbr, token_text);
  
  MSG (message_nbr, file_nbr, line_nbr, char_nbr, text, token_text);
}

/* Build full hierarchical sort key (incorporating all the files in the nested COPY chain for a message M.  */

void build_full_sort_key (message_occurrence *m);

void
build_full_sort_key (message_occurrence *m)
{
  uint32 ix;
  int32 ix2 = 0;
  uint32 ix3 = 0;
  uint32 line_nbr;
  uint32 file_nbr;
  static uint32 old_file_count = 0;

  if (work_array)
    BLI_ASSERT (old_file_count == file_count);
  else
    {
      work_array = BLI_ALLOC (file_count * sizeof (uint32) *2);
      old_file_count = file_count;
    }
  
  m->full_sort_key = BLI_ALLOC (file_count * sizeof (uint32) *2);

  line_nbr = m->line_nbr;
  file_nbr = m->file_nbr;
  
  for (ix = 0; file_nbr; ix += 2)
    {
      BLI_ASSERT (ix < (file_count * 2));
      work_array[ix] = file_nbr;
      work_array[ix +1] = line_nbr;
      get_owning_details (&file_nbr,  &line_nbr);
    }

  for (ix2 = ix - 2, ix3 = 0; ix2 >= 0; ix2 -= 2, ix3 += 2)
    {
      m->full_sort_key[ix2] = work_array[ix3];
      m->full_sort_key[ix2 + 1] = work_array[ix3 + 1];
    }
  
}

/* Comparison function for the message sort.  See doc for qsort.  */

static int msg_compare (message_occurrence *a, message_occurrence *b);

static int32
msg_compare (a, b)
     message_occurrence *a;
     message_occurrence *b;
{
  int32 diff = 0;

  /* Hierarchy of file#/line# from the top of the copy hierarchy.  */
  
  /* To sort in textual order we need to sort the COPY hierarchy.  This
     means array of file#line# from the top.  */
  
  if (!a->full_sort_key)
      build_full_sort_key (a);

  if (!b->full_sort_key)
      build_full_sort_key (b);

  {
    uint32 i;
    for (i = 0; i < file_count * 2; i ++)
      {
        diff = a->full_sort_key[i] - b->full_sort_key[i];
        if (diff)
          break;
      }
  }
  if (diff < 0)
    diff = -1;
  else
    if (diff > 0)
      diff = 1;

  /* Diff=memcmp(a->full_sort_key, b->full_sort_key, file_count*sizeof(uint32)*2);
   not on little endian machines you don't.  */
  return diff;

}

/* Output accumulated messages from the record_msg functions.  */

void
output_msgs ()

{
  uint32 message_index;
  
  if (!message_accumulation_count)
    return;

  BLI_ASSERT (message_accumulation);

  /* Sort them.  */
  qsort (message_accumulation->elem, message_accumulation_count, sizeof (message_occurrence),
         (int ( *)(const void *, const void *))msg_compare);

  /* Output them.  */
  for (message_index = 0; message_index < message_accumulation_count; message_index ++)
    output_msg (message_accumulation->elem + message_index);
}

/* Output summary, return error count.  */

uint32
output_summary ()
{
  if (errorcount || warningcount)
    fprintf (stderr, "\nErrors %d Warnings %d\n", errorcount, warningcount);
  else
    if (message_output_count)
      fprintf (stderr, "\n");
  return errorcount;
}

/* Initialization for message processing.  */

void
message_init ()
{

  uint32 message_info_index;
  uint32 message_ptr_index;

  /* Set up message info pointers.  */

  max_message_id_nbr = 0;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].message_id_nbr > max_message_id_nbr)
        max_message_id_nbr = message_info_array[message_info_index].message_id_nbr;
    }
  
  message_info_ptr = (struct dynarray_message_info_ptr_struct *) 
    ALLOC_DYN ((max_message_id_nbr + 1), 
               sizeof (struct bli_message_info *));
  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      message_ptr_index = message_info_array[message_info_index].message_id_nbr;
      message_info_ptr->elem[message_ptr_index] =
        message_info_array + message_info_index;
    }

  /* Set up accumulated message array.  */
  
  message_accumulation = (struct dynarray_message_occurrence_struct *) 
    ALLOC_DYN (100, sizeof (message_occurrence));
  message_accumulation_count = 0; 
}

/* Control output and error detection - output lots of annoying messages.  */

void
output_pedantic ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].underscore_not_allowed)
        message_info_array[message_info_index].output_flag = 1;
      else
      if (message_info_array[message_info_index].nons)
        message_info_array[message_info_index].output_flag = 1;
    }
}

/* Control output and error detection - flag $ in names as an error.  */

void
error_underscore_in_varnames ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    if (message_info_array[message_info_index].underscore_not_allowed)
      {
        message_info_array[message_info_index].error_flag = 1;
        message_info_array[message_info_index].output_flag = 1;
      }
}

/* Control output and error detection - output "for your info" messages.  */

void
output_fyi ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].fyi)
        message_info_array[message_info_index].output_flag = 1;
    }
}

/* Control output and error detection - output efficiency messages.  */

void
output_eff ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].eff)
        message_info_array[message_info_index].output_flag = 1;
    }
}

/* Control output and error detection - output messages for use of obsolete features.  */

void
output_obs ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].obs)
        message_info_array[message_info_index].output_flag = 1;
    }
}

/* Control output and error detection - output files as they are opened.  */

void
output_filenames ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].filename)
        message_info_array[message_info_index].output_flag = 1;
    }
}

/* Control output and error detection - suppress warnings.  */

void
no_output_warning ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].warning)
        {
          message_info_array[message_info_index].output_flag = 0;
          message_info_array[message_info_index].error_flag = 0;
        }
    }
}

/* Control output and error detection - suppress messages about unsupported features.  */

void
no_output_unsupported ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].uns)
        {
          message_info_array[message_info_index].output_flag = 0;
          message_info_array[message_info_index].error_flag = 0;
        }
    }
}

/* Control output and error detection - suppress errors for unsupported features.  */

void
no_error_unsupported ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].uns)
          message_info_array[message_info_index].error_flag = 0;
    }
}

/* Be a real pain - fail the compile is any pedantic warnings occur.  */

void
error_pedantic ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].underscore_not_allowed)
        {
          message_info_array[message_info_index].output_flag = 1;
          message_info_array[message_info_index].error_flag = 1;
        }
      else
        if (message_info_array[message_info_index].nons)
          {
            message_info_array[message_info_index].output_flag = 1;
            message_info_array[message_info_index].error_flag = 1;
          }
    }
}

/* Fail the compile if any warnings are output.  */

void
error_warning ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].warning)
        {
          message_info_array[message_info_index].output_flag = 1;
          message_info_array[message_info_index].error_flag = 1;
        }
    }
}

/* Fail the compile if a FYI message appears.  */

void
error_fyi ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].fyi)
        {
          message_info_array[message_info_index].output_flag = 1;
          message_info_array[message_info_index].error_flag = 1;
        }
    }
}

/* Fail the compile if any efficiency messages are output.  */

void
error_eff ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].eff)
        {
          message_info_array[message_info_index].output_flag = 1;
          message_info_array[message_info_index].error_flag = 1;
        }
    }
}


/* Fail the compie if any obsolete features are used.  */

void
error_obs ()
{
  uint32 message_info_index;

  for (message_info_index = 0; 
       !(message_info_array[message_info_index].past_end_of_list);
       message_info_index ++)
    {
      if (message_info_array[message_info_index].obs)
        {
          message_info_array[message_info_index].output_flag = 1;
          message_info_array[message_info_index].error_flag = 1;
        }
    }
}


