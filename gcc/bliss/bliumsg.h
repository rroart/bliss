/* 
 
   BLISS Compiler utility message handling header (bliumsg.h)

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
   what you give them.   Help stamp out software-hoarding!  */

/* This is used in the source to flag a message number eg
   RECORD_MSG(MSGID(77), fileno, etc) so you can scan the source for a message number.  */

#define MSGID(x) x

/* A message, accumulated for later output.  */

struct message_occurrence_struct GTY(())
{
  uint32 message_id_nbr;
  uint32 file_nbr;
  uint32 line_nbr;
  uint32 char_nbr; 
  const uchar *text;
  const uchar *reading_text;
  uint32 * GTY((length ("file_count * 2"))) full_sort_key;
};

extern uint32 file_count;

typedef struct message_occurrence_struct message_occurrence;

/* Save an error message for later output.  */

#define MSG( MESSAGE_NBR, FILE_NBR, LINE_NBR, BLI_CHAR_NBR, TEXT, READING_TEXT)\
 record_msg( MESSAGE_NBR, FILE_NBR, LINE_NBR, BLI_CHAR_NBR, TEXT, READING_TEXT)

/* Save an error message for a token for later output.  */

#define MSG_TOKEN(MESSAGE_NBR, TOKEN, TEXT) record_msg_token (MESSAGE_NBR, TOKEN, TEXT)

/* Output accumulated messages.  */

#define BLI_OUTPUT_MSGS()  output_msgs()

/* All about a message type, by number.  */

struct bli_message_info
{
  uint32 message_id_nbr;  /* Number used to identify message.  */
  const uchar *message_text;  /* Default text.  */

  /* Last entry in table + 1 - no data in this entry.  */

  uint32 past_end_of_list : 1;

  /* The following three fields are defaults and can be amended using
     the subsequent information and the options.  */

  uint32 fatal_flag : 1;  /* Fatal error if found - immediate termination.  */
  uint32 error_flag : 1;  /* Count as error (compile returns FAIL), no output file.  */
  uint32 output_flag : 1;  /* Output the message (some messages are
                             suppressed depending on compiler options).  */


  uint32 underscore_not_allowed : 1;  /* this is an "underscore not allowed" msg.  */
  uint32 fatal : 1;  /* Immediate fatal.  */
  uint32 error : 1;  /* Error.  */

  uint32 warning : 1;  /* Warning.  */
  uint32 fyi : 1;  /* For your info.  */
  uint32 eff : 1;  /* Efficiency.  */

  uint32 obs : 1;  /* Obsolete.  */
  uint32 uns : 1;  /* Unsupported.  */
  uint32 nons : 1;  /* Non standard.  */

  uint32 filename : 1;  /* This is the output filename message.  */
  uint32 immediate : 1;  /* Immediate output message do not wait for sort and print at the end.  */
  
};

/* COPY Files details.  */

extern GTY((length ("file_count"))) dynarray_uchar_ptr *file_names;
extern GTY((length ("file_count"))) dynarray_uchar_ptr *file_names_copied;
extern GTY((length ("file_count"))) dynarray_uchar_ptr *file_names_copied_original;

extern GTY((length ("file_count"))) dynarray_uint32 *file_names_lengths;
extern GTY((length ("file_count"))) dynarray_uint32 *file_names_copied_lengths;
extern GTY((length ("file_count"))) dynarray_uint32 *file_names_copied_original_lengths;

extern uint32 file_count;

#define BLI_GET_TOKEN_DETAILS( token, file_nbr,  line_nbr,  char_nbr, token_text) \
bli_get_token_details (token,  &file_nbr,  &line_nbr,  &char_nbr, &token_text);

void record_msg (uint32 message_id_nbr, 
                        uint32 file_nbr, 
                        uint32 line_nbr, uint32 char_nbr, 
                        const uchar *text, const uchar *reading_text);

void get_owning_details (uint32 *file_nbr, uint32 *line_nbr);

void record_msg_token (uint32 message_nbr, void *token, uchar *text);

void output_msgs (void);

uint32 output_summary (void);

void message_init (void);

void output_pedantic (void);

void output_fyi (void);

void output_eff (void);

void output_obs (void);

void output_filenames (void);

void error_pedantic (void);

void error_warning (void);

void error_fyi (void);

void error_eff (void);

void error_obs (void);

void error_underscore_in_varnames (void);

void no_output_warning (void);

void no_error_unsupported (void);

void no_output_unsupported (void);

void bli_get_token_details (void *token, uint32 *file_nbr, 
                            uint32 *line_nbr, uint32 *char_nbr, 
                            uchar **token_text);

void get_owning_details (uint32 *file_nbr, uint32 *line_nbr);

