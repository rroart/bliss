/* 
   
   BLISS Compiler preprocessor phase 1 (blipre1)
   - basic fixup of one file and breakup into (kind of) tokens

   -------------------------------------------------------------------------

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Adapted to BLISS by Tim Josling Mar 1999
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the1
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  
   
   -------------------------------------------------------------------------.  */


#include "blir_temp_config.h"
#include "ansidecl.h"
#include "blidebug.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define GTY(a) 

#include "bliumem.h"
#include "bliumsg.h"
#include "blipre.h"
#include "bliplex.h"


/* 
 *  These are file global because 
 *  getinput called by lex has a defined interface 
 *  and I need to get this info to it somehow.  */

/* Where getinput is up to.  */
static uint32 getinput_lineno = 0;
static uchar *getinput_char = NULL;
static uint32 getinput_after_nl_flag = 0;
/* Lines details.  */
static dynarray_line *lines;
static uint32 linecount;  /* Number of lines.  */

/* File global variables.  */
static uchar *cbuffer;
static uint32 charno;
static uint32 line_nbr;
static dynarray_uint32 *continued_lines;
static uint32 continued_line_count;
static uint32 continued_line_ix;
static struct line_struct *line;
static struct line_struct *linethread;
static struct line_struct *linethreadprev;
static uint32 indicator_char;
static uint32 pending_continued_line;
static uint32 pending_continued_flag;
static uint32 blank_flag;
static uint32 non_blank_char_nbr = 0;
static uchar end_char;
static int line_length;
static uint32 required_total_continued_line_size;
static uint32 size_of_this_continuation;
static uint32 pasted_continued_line_size;
static uint32 continuation_found_nonblank;
static uint32 line_ends_within_string;
static uint32 string_delimiter_char;
static uint32 line_ends_with_terminated_string;
static dynarray_uchar *new_line = NULL;
static uint32 new_line_size = 0;

static void *buffer;
static uint32 length;
static uchar *filename;
static uint32 file_number;
static struct file_struct *saved_file_ptr;

static void count_lines (void);
static void setup_line_pointers (void);
static void set_line_start_end (void);
static void process_indicator_and_blank (void);
static void note_string_termination (void);
static void mark_continued_lines (void);
static void check_non_continued_lines (void);
static void paste_continuations (void);

extern uint32 which_lex;  /* Which lex are we doing - file or repaste tokens.  */

/* 
   Break a file FILE_PTR into pseudo tokens for preprocessing.  */

extern void
blipre1_tokenize_file (struct file_struct *file_ptr)
{

  /* Work vars.  */
  uint32 this_token;
  uchar *t;
  uint32 l;

  dynarray_token *tokens = NULL;
  uint32 token_count = 0;
  uint32 token_ix;
  struct line_struct *line;

  saved_file_ptr = file_ptr;

  lines = file_ptr->file_lines;
  linecount = file_ptr->file_line_count;

  tokens = (dynarray_token *) ALLOC_DYN (file_ptr->file_line_count * 5, sizeof (struct token_struct));

  getinput_lineno = 0;
  getinput_char = lines->elem[getinput_lineno].line_start_new;
  getinput_after_nl_flag = 2;  /* So we output the id character (*,D).  */
  token_count = 0;

  lexinit (LEXINIT_FILE, file_ptr);

  while (0 != (this_token = yylex ()))
    {

      t = (uchar *)yytext;
      l = yyleng;

      GROW_DYN (&tokens->dynarray_details, token_count);

      tokens->elem[token_count].token_type = this_token;
      tokens->elem[token_count].token_file = file_ptr;

      line = lines->elem + blipre_lineno - 1;

      /* Tokens continued across lines get recorded on starting line only.  */
      tokens->elem[token_count].token_lineno = blipre_lineno;
      tokens->elem[token_count].token_charno = blipre_charno + comment_offset;

      if (line->line_flag_continued && (!line->line_flag_continuation))
	{
	  dynarray_line_details *line_details_ptr;
	  struct lineno_struct *lineno_ptr;
	  uint32 lineno_count_copy;
	  uint32 lineno_ix;
	  uint32 low_lineno_ix;

	  /* Need to check which line this token started on.  */
	  BLI_ASSERT (line->line_lineno_ptr);
	  lineno_ptr = line->line_lineno_ptr;
	  lineno_count_copy = lineno_ptr->lineno_count;
	  BLI_ASSERT (lineno_count_copy);
	  low_lineno_ix = lineno_count_copy - 1;
	  for (lineno_ix = 1; lineno_ix < lineno_count_copy; lineno_ix ++)
	    {
	      line_details_ptr = lineno_ptr->lineno_line_details;
	      BLI_ASSERT (line_details_ptr);
	      if (blipre_charno < line_details_ptr->elem[lineno_ix].line_details_offset)
		{
		  low_lineno_ix = lineno_ix - 1;
		  break;
		}
	    }
	  line_details_ptr = lineno_ptr->lineno_line_details;
	  tokens->elem[token_count].token_lineno =
	    line_details_ptr->elem[low_lineno_ix].line_details_lineno + 1;
	  tokens->elem[token_count].token_charno =
	    blipre_charno - line_details_ptr->elem[low_lineno_ix].line_details_offset
	    + line_details_ptr->elem[low_lineno_ix].line_details_charno + comment_offset;
	}

      /* 
         Another kludge...
         we replaced the \n by a space to honour fix_eol
         so now we have to fake the newlines.  */

      if (this_token == NEWLINE)
	{
	  tokens->elem[token_count].token_chars = (uchar *)"\n";
	  /* 
	     Nested kludge
	     if a line is split, the token details of the 
	     newlines can get out of sync
	     - fix it.  */
	  if (token_count > 0)
	    {
	      if (
		  (tokens->elem[token_count].token_lineno <
		   tokens->elem[token_count - 1].token_lineno)
		  ||
		  (
		   (tokens->elem[token_count].token_lineno ==
		    tokens->elem[token_count - 1].token_lineno)
		   &&
		   (tokens->elem[token_count].token_charno <
		    tokens->elem[token_count - 1].token_charno)))
		{
		  tokens->elem[token_count].token_charno =
		    tokens->elem[token_count - 1].token_charno + 1;
		  tokens->elem[token_count].token_lineno =
		    tokens->elem[token_count - 1].token_lineno;
		}
	    }
	}
      else
	{
	  if (blipre_charno > 1)
            tokens->elem[token_count].token_chars = line->line_start_new +
              blipre_charno - 2;
	  else
            tokens->elem[token_count].token_chars = line->line_start_orig +
              blipre_charno - 1 + comment_offset;
	}

      /* Debug always comes from the first line even if continued.  */
      tokens->elem[token_count].token_flag_debug_line = blipre_debug_line;

      tokens->elem[token_count].token_length = yyleng;

      token_count ++;
    }

  for (token_ix = 0; token_ix < (token_count - 1); token_ix ++)
    {
      tokens->elem[token_ix].token_next = tokens->elem + token_ix + 1;
    }

  for (token_ix = 1; token_ix < (token_count); token_ix ++)
    {
      tokens->elem[token_ix].token_prev = tokens->elem + token_ix - 1;
    }

  file_ptr->file_token_count = token_count;
  BLI_ASSERT (tokens);
  file_ptr->file_first_token = tokens->elem;
  file_ptr->file_last_token = tokens->elem + token_count - 1;
}

/* Static char *char_ptr;.  */

/* 
   Handle the continuation lines etc for file stored in buffer
   PARM_BUFFER of length PARM_LENGTH named PARM_FILENAME, file number
   PARM_FILE_NUMBER, lines array in LINES_PTR, line count LINE_COUNT,
   from file FILE_PTR.  */

void
blipre1_fix_lines (void *parm_buffer, 
                   uint32 parm_length,
                   uchar *parm_filename,
                   uint32 parm_file_number,
                   dynarray_line **lines_ptr,
                   uint32 *line_count_ptr,
                   struct file_struct *file_ptr)
{


  saved_file_ptr = file_ptr;
  buffer = parm_buffer;
  length = parm_length;
  filename = parm_filename;
  file_number = parm_file_number;

  cbuffer = buffer;

  count_lines ();
  setup_line_pointers ();
  set_line_start_end ();
  process_indicator_and_blank ();
  note_string_termination ();
  mark_continued_lines ();
  check_non_continued_lines ();
  paste_continuations ();

  *lines_ptr = lines;
  *line_count_ptr = linecount;

}


/* Count the lines in file and allocate storage for pointers to each
   line plus a dummy one to avoid EOF tests all the time - characters
   are in global variable CBUFFER and lenngth is in LENGTH.  */

static void
count_lines (void)
{
  /* 
     1. Count the lines and allocate line descriptors.  */

  linecount = 0;
  for (charno = 0; charno < length; charno ++)
    {
      if (cbuffer[charno] != '\n')
	continue;
      linecount ++;
    }

  if (cbuffer[length - 1] != '\n')
    linecount ++;

  /* Allocate one extra to save 'is this last one' test all the time.  */

  lines = (dynarray_line *) ALLOC_DYN (linecount + 1, sizeof (struct line_struct));
  continued_lines = (dynarray_uint32 *) ALLOC_DYN (linecount + 1, sizeof (uint32 *));
}

/* Set each entry in the LINES array to point to its line.  Ensure end
   of line at EOF.  */

static void
setup_line_pointers (void)
{

  uint32 no_newline_at_end = 0;

  /* 
     2. Set up the pointers to each line.  */

  line_nbr = 0;
  line = lines->elem;
  line->line_start_orig = cbuffer;

  if (cbuffer[length - 1] != '\n')
    no_newline_at_end = 1;

  for (charno = 0; charno < length; charno ++)
    {
      if (cbuffer[charno] == '\n')
	{
	  cbuffer[charno] = ' ';  /* For later, in case we have fix_eol.  */
	  line->line_end_orig = cbuffer + charno;
	  line_nbr ++;
	  line = (lines->elem + line_nbr);
	  line->line_start_orig = cbuffer + charno + 1;
	}
    }

  /* 
     Fake an end of line at the end if needed.  */

  if (no_newline_at_end)
    {
      line = lines->elem + linecount - 1;
      line->line_end_orig = cbuffer + length + 1;
      cbuffer[length] = ' ';
      length ++;
    }

  /* Read_file allows one extra byte so I can set the extra char to ' ' 
     later on 
     and thus honour the fix_eol flag.  */
  cbuffer[length] = ' ';
}


/* Set start andend of line for lines in LINES array.  */

static void
set_line_start_end (void)
{

  /* 
     3. Set 
     - new line start to after the comment
     - new line end to true last char + 1.  */

  for (line_nbr = 0; line_nbr < linecount; line_nbr ++)
    {
      line = (lines->elem + line_nbr);
      line->line_start_new = line->line_start_orig + area_a_offset;
      line->line_end_new = line->line_end_orig;
    }

}

/* Set line flags based on the indicator character (std column 7) and
   test for blank lines.  */

static void
process_indicator_and_blank (void)
{
  uchar *char_ptr; 
  /* 
     4. 
     If line is too short for indicator field
     - set new line start to last char of line + 1 (start==end)
     - set blank line flag
     check comment field
     - set comment flag if '*' or '/'
     - set indicator flag if '*' or '/' or 'd/D'
     - set continuation flag if '-'
     set blank line flag if whole line is blank (ignoring sequence #).  */

  for (line_nbr = 0; line_nbr < linecount; line_nbr ++)
    {
      line = (lines->elem + line_nbr);
      line->line_ind_char = ' ';
      if ((uint32)(line->line_start_orig + comment_offset) >=
	  (uint32)(line->line_end_orig))
	{
	  line->line_flag_blank = 1;
	}
      else
	{
	  indicator_char = line->line_start_orig[comment_offset];
	  if (indicator_char == ' ')
	    {
	      /* Check if whole line blank.  */
	      line->line_flag_blank = 1;

	      for (char_ptr = line->line_start_new;
		   char_ptr != line->line_end_new; char_ptr ++)
		{
		  if ( * char_ptr == ' ')
		    continue;
		  line->line_flag_blank = 0;
		  break;
		}
	    }
	  else
	    {
	      if ((indicator_char == '*') || (indicator_char == '/')
		  || ((toupper (indicator_char) == 'D')))
		{
		  line->line_flag_indicator = 1;
		  line->line_ind_char = indicator_char;
		  if ((indicator_char == '*') || (indicator_char == '/'))
		    {
		      line->line_flag_comment = 1;
		    }
		}
	      else
		{
		  if (indicator_char == '-')
		    {
                      uchar *char_ptr; 
		      line->line_flag_continuation = 1;
		      line->line_flag_ignore = 1;  /* Just send cr to tokeniser.  */
		      /* Check that continuation line is not empty.  */
		      line->line_flag_indicator = 1;
		      line->line_ind_char = indicator_char;
		      blank_flag = 1;
		      for (char_ptr = line->line_start_new;
			   char_ptr != line->line_end_new; char_ptr ++)
			{
			  if ( * char_ptr == ' ')
			    continue;
			  blank_flag = 0;
			  non_blank_char_nbr =
			    char_ptr - line->line_start_new;
			  if (non_blank_char_nbr < 4)
			    {
                              MSG (MSGID (29), file_number, line_nbr + 1, 0, NULL, NULL);
			    }
			  break;
			}
		      if (blank_flag)
			{
                          MSG (MSGID (30), file_number, line_nbr + 1, 0, NULL, NULL);
                          line->line_flag_continuation = 0;  /* Treat as non-continuation.  */
			}
		    }
		  else
		    {
		      /* Invalid comment field  - treat as comment.  */
                      MSG (MSGID (2), file_number, line_nbr + 1, 0, NULL, NULL);
		      line->line_ind_char = indicator_char;
		      line->line_flag_ignore = 1;  /* Just send cr to tokeniser.  */
		    }
		}
	    }
	}
    }
}

/* Find unterminated and terminated strings ending lines.  */

static void
note_string_termination (void)
{
  uchar *char_ptr; 
  /* 
     4A. 
     Mark lines with status to indicate if they end in an unterminated 
     character string, which can start " or '
     and also note if they ended with a terminated string.  */

  for (line_nbr = linecount - 1; line_nbr < linecount; line_nbr --)
    {
      line_ends_within_string = 0;
      string_delimiter_char = 0;
      line_ends_with_terminated_string = 0;
      line = (lines->elem + line_nbr);

      for (char_ptr = line->line_start_new;
	   (uint32)char_ptr < (uint32)(line->line_end_new); char_ptr ++)
	{
	  if (( * char_ptr == '\"') || ( * char_ptr == '\''))
	    {
	      if (line_ends_within_string == 0)
		{
		  string_delimiter_char =  * char_ptr;
		  line_ends_within_string = 1;
		}
	      else
		{
		  if (string_delimiter_char ==  * char_ptr)
		    {
		      line_ends_within_string = 0;  /* Flip/flop 0 <-> 1.  */
		      line_ends_with_terminated_string = 1;
		    }
		}
	    }
	  else
	    {
	      if (line_ends_within_string == 0)
		{
		  if ( * char_ptr != ' ')
		    line_ends_with_terminated_string = 0;
		}
	    }
	}
      line->line_flag_partstring = line_ends_within_string;
      line->line_flag_termstring = line_ends_with_terminated_string;
      if (line_ends_within_string)
	{
	  line->line_string_char = string_delimiter_char;
	}
    }
}

/* Find continued lines and note where they are continued.  Check
   continuation is appropriate for token at end eg not terminated string.  */

static void
mark_continued_lines (void)
{

  /* 
     5. 
     Mark continued lines with the lines that continue them
     Check they do not end with a terminated string.  */

  continued_line_count = 0;
  pending_continued_line = 0;
  pending_continued_flag = 0;
  for (line_nbr = linecount - 1; line_nbr < linecount; line_nbr --)
    {
      line = (lines->elem + line_nbr);
      if (pending_continued_flag)
	{
	  if ((!line->line_flag_comment) && (!line->line_flag_blank))
	    {
	      pending_continued_flag = 0;
	      if (line->line_flag_termstring)
		{
                  MSG (MSGID (31), file_number, line_nbr + 1, 0, NULL, NULL);
		}
	      line->line_continuation_line_nbr = pending_continued_line;
	      lines->elem[pending_continued_line].line_continued_line_nbr =
		line - lines->elem;
	      line->line_flag_continued = 1;
	      continued_lines->elem[continued_line_count] = line - lines->elem;
	      continued_line_count ++;
	    }
	}
      if (line->line_flag_continuation)
	{
	  pending_continued_flag = 1;
	  pending_continued_line = line_nbr;
	}
    }

  if (pending_continued_flag)
    {
      MSG (MSGID (32), file_number, pending_continued_line + 1, 0, NULL, NULL);
    }
}

/* Check non continued lines are terminated OK ie do not require
   continuation.  */

static void
check_non_continued_lines (void)
{

  /* 
     6. 
     Check non-continued lines have a valid delimiter at the end
     (unless we were asked to fix them up automatically)
     and do not have a partial string at the end.  */

  for (line_nbr = 0; line_nbr < linecount; line_nbr ++)
    {
      line = (lines->elem + line_nbr);
      if (line->line_flag_comment)
	continue;
      if (!line->line_flag_continued)
	{
	  if (line->line_flag_partstring)
	    {
              MSG (MSGID (33), file_number, line_nbr + 1, 0, NULL, NULL);
	      line->line_flag_wrap_string = 1;
	      continue;
	    }
	  else
	    {
	      if (!option_fix_eol)
		{
		  line_length = line->line_end_new - line->line_start_new;
		  if (line_length > 0)
		    {
		      end_char =  * (line->line_end_new - 1);
		      if (memchr (" ():", end_char, strlen (" ():")))
			{
			  /* Correctly terminated  with space, (, ), or :.  */
			}
		      else
			{
			  uint32 blank_to_end = 1;
			  uchar *testchar;
			  for (testchar = line->line_start_new;
			       (uint32)testchar <
			       (uint32)(line->line_end_new - 1);
			       testchar ++)
			    {
			      if (( * testchar) != ' ')
				{
				  blank_to_end = 0;
				  break;
				}
			    }
			  if ((blank_to_end)
			      && (end_char == '=')
			      && (line->line_flag_continuation))
			    {
			      /* Correctly terminated with =  continued.  */
			    }
			  else
			    {
			      if ((line_length >= 2)
				  &&
				  (!memcmp (line->line_end_new - 2, "==", 2)))
				{
				  /* Correctly terminated with ==.  */
				}
			      else
				{
                                  MSG (MSGID (34), file_number, line_nbr + 1, 0, NULL, NULL);
				  line->line_flag_needs_delimiter = 1;
				}
			    }
			}
		    }
		}
	    }
	}
    }
}

/* Fold continued lines with the lin that continues them.  */

static void
paste_continuations (void)
{

  uint32 line_nbr; 
  dynarray_line_details *line_details_ptr; 
  struct lineno_struct *lineno_ptr; 
  uchar *char_ptr; 

  /* 
     7. 
     Paste the continuations onto the continued line.  */

  /* Find continued lines.  */
  for (continued_line_ix = 0;
       continued_line_ix < continued_line_count; continued_line_ix ++)
    {
      line = lines->elem + continued_lines->elem[continued_line_ix];

      /* Skip if already done.  */
      if (line->line_flag_continuation)
	continue;

      new_line_size = 100;
      new_line = (dynarray_uchar *) ALLOC_DYN (new_line_size, sizeof(uchar));

      pasted_continued_line_size = 0;
      linethread = line;
      BLI_ASSERT (linethread->line_flag_continued);
      pasted_continued_line_size =
	(linethread->line_end_new - linethread->line_start_new);
      required_total_continued_line_size = pasted_continued_line_size;
      /* Ensure buffer big enough.  */
      GROW_DYN (&new_line->dynarray_details, required_total_continued_line_size);

      line->line_lineno_ptr = BLI_ALLOC (sizeof (struct lineno_struct));
      lineno_ptr = line->line_lineno_ptr;

      /* Allow 1 continuation line at first.  */

      lineno_ptr->lineno_line_details = 
        (dynarray_line_details *) ALLOC_DYN (2, sizeof (struct line_details_struct));
      line_nbr = line - lines->elem;
      line_details_ptr = lineno_ptr->lineno_line_details;
      line_details_ptr->elem[0].line_details_lineno = line_nbr;
      line_details_ptr->elem[0].line_details_charno = area_a_offset + 1;
      line_details_ptr->elem[0].line_details_offset = area_a_offset + 1;
      lineno_ptr->lineno_count = 1;

      /* Copy first line in.  */
      memcpy (new_line->elem, line->line_start_new, pasted_continued_line_size); /* xxx; */

      line->line_start_new = new_line->elem;
      line->line_end_new = new_line->elem + pasted_continued_line_size;
      /* Truncate trailing blanks later, in the loop.  */

      /* Copy remaining lines in.  */
      while (linethread->line_continuation_line_nbr)
	{
	  linethreadprev = linethread;
	  linethread = lines->elem + linethread->line_continuation_line_nbr;
	  continuation_found_nonblank = 0;
	  /* Scan for nonblank to continue at.  */
	  for (char_ptr = linethread->line_start_new;
	       char_ptr != linethread->line_end_new; char_ptr ++)
            if ( * char_ptr != ' ')
              {
                continuation_found_nonblank = 1;
                break;
              }
	  BLI_ASSERT (continuation_found_nonblank);

	  /* Check valid partial string completion.  */
	  if (( * char_ptr == '\"') || ( * char_ptr == '\''))
	    {
	      if (!(linethreadprev->line_flag_partstring))
		{
		  /* We already picked this up as an invalid continuation.  */
		  /* Avoid consequent errors - do not skip quote character.  */
		}
	      else if ( * char_ptr != linethreadprev->line_string_char)
		{
                  MSG (MSGID (35), file_number, line_nbr + 1, 0, NULL, NULL);
		  /* Flag to close off the partial string in the continued line.  */
		  linethreadprev->line_flag_wrap_string = 1;
		}
	      else
                char_ptr ++;  /* Skip the quote start.  */
	    }
	  else
	    {
	      /* Not a string continuation.  */
	      if (linethreadprev->line_flag_partstring)
		{
		  /* We were expecting a string continuation.  */
		  /* Just assume it's continued from the first non-blank.  */
                  MSG (MSGID (36), file_number, line_nbr + 1, 0, NULL, NULL);
		}
	    }
	  size_of_this_continuation = linethread->line_end_new - char_ptr;
	  required_total_continued_line_size =
	    pasted_continued_line_size + size_of_this_continuation + 1;
	  /* Ensure buffer big enough.  */
	  GROW_DYN (&new_line->dynarray_details, required_total_continued_line_size);
	  line->line_start_new = new_line->elem;
	  if (linethreadprev->line_flag_wrap_string)
	    {
	      new_line->elem[pasted_continued_line_size++] = 
		linethreadprev->line_string_char;
	      linethreadprev->line_flag_wrap_string = 0;
	    }
	  /* Trim the spaces at the end of the previous line 
	     - continuation is from the last non-blank unless you are
	     continuing a string.  */
	  if (!linethreadprev->line_flag_partstring)
            while (' ' ==  *(new_line->elem + pasted_continued_line_size - 1))
              pasted_continued_line_size --;
	  /* Check that they are not trying to continue = with = to make ==.  */
	  if (!(linethreadprev->line_flag_partstring))
            if (*char_ptr == '=')
              if (*(new_line->elem + pasted_continued_line_size - 1) == '=')
                {
                  MSG (MSGID (37), file_number, line_nbr + 1, 0, NULL, NULL);
                }

	  /* Remember where we came from.  */
	  /* Check for room.  */
	  lineno_ptr->lineno_count ++;
	  GROW_DYN (&line_details_ptr->dynarray_details, lineno_ptr->lineno_count);
	  lineno_ptr->lineno_line_details = line_details_ptr;
	  /* Record details.  */
	  line_details_ptr->elem[lineno_ptr->lineno_count - 1].line_details_charno =
	    char_ptr - linethread->line_start_new + area_a_offset + 1;
	  line_details_ptr->elem[lineno_ptr->lineno_count - 1].line_details_lineno =
	    linethread - lines->elem;
	  line_details_ptr->elem[lineno_ptr->lineno_count - 1].line_details_offset =
	    pasted_continued_line_size + area_a_offset + 1;

	  /* Copy continuation in.  */
	  memcpy (new_line->elem + pasted_continued_line_size,
		  char_ptr, size_of_this_continuation);
	  pasted_continued_line_size += size_of_this_continuation;
	  line->line_end_new = new_line->elem + pasted_continued_line_size;
	  /* In case of fixeol.  */
	  line->line_end_new[0] = ' ';
	  continue;
	}
    }
}


/* Get input for yylex and place in BUF up to SIZE characters, return
   # chars inserted or 0 for EOF.  */

extern native_int
getinput (char *buf, int max_size)
{

  uint32 current_size = 0;
  struct line_struct *line;

  if (which_lex == LEXINIT_REDO_TOKENS)
    {
      return (int)getinput_repaste ((uchar *)buf, (uint32)max_size);
    }
  BLI_ASSERT (which_lex == LEXINIT_FILE);

  while (current_size < (uint32)max_size)
    {
      uint32 bytes_to_copy;
      if (getinput_lineno >= linecount)
	{
	  return current_size;
	}
      if (getinput_after_nl_flag)
	{
	  if (getinput_after_nl_flag == 1)
	    {
	      buf[current_size] = '\n';
	      current_size ++;
	      getinput_lineno ++;
	      getinput_after_nl_flag = 2;
	      continue;
	    }
	  else
	    { /* Nl_flag == 2.  */
	      if (lines->elem[getinput_lineno].line_flag_ignore)
		{
		  getinput_after_nl_flag = 1;
		  continue;
		}
	      else
		{
		  if (
		      ((uint32)
		       (lines->elem[getinput_lineno].line_start_orig +
			comment_offset)) <
		      ((uint32)(lines->elem[getinput_lineno].line_end_orig)))
		    {
		      buf[current_size] =
			lines->elem[getinput_lineno].line_ind_char;
		      current_size ++;
		    }
		}
	      getinput_char = lines->elem[getinput_lineno].line_start_new;
	      if ((uint32)(getinput_char) >
		  (uint32)(lines->elem[getinput_lineno].line_end_new))
		{
		  getinput_char = lines->elem[getinput_lineno].line_end_new;
		}
	      getinput_after_nl_flag = 0;
	      continue;
	    }
	}
      if (getinput_char == lines->elem[getinput_lineno].line_end_new)
	{
	  if (option_fix_eol)
	    {
	      line = lines->elem + getinput_lineno;
	      if (line->line_start_new != line->line_end_new)
		{
		  if ( * (line->line_end_new - 1) != ' ')
		    {
		      buf[current_size] = ' ';
		      current_size ++;
		      getinput_char ++;
		    }
		}
	    }
	  getinput_after_nl_flag = 1;
	  continue;
	}
      bytes_to_copy = lines->elem[getinput_lineno].line_end_new - getinput_char;
      if (bytes_to_copy > (max_size - current_size))
	{
	  bytes_to_copy = max_size - current_size;
	}
      memcpy (buf + current_size, getinput_char, bytes_to_copy);
      current_size += bytes_to_copy;
      getinput_char += bytes_to_copy;
    }
  return current_size;
}


/* Truncate or fill lines in file *FILE_PTR exactly to the r-margin.  */

void
blipre1_R_margin (struct file_struct *file_ptr)
{

  uint32 linecount;
  uint32 charno;
  uchar *cbuffer;
  uint32 length;
  uint32 new_length;
  dynarray_uchar *new_cbuffer;
  uchar *new_cbuffer_end;
  uchar *new_cbuffer_work;
  uchar *line_base;
  uint32 linecharno;
  uchar c;

  cbuffer = file_ptr->file_buffer;
  length = file_ptr->file_length;

  /* 
     1. Count the lines.  */

  linecount = 0;
  for (charno = 0; charno < length; charno ++)
    {
      if (cbuffer[charno] != '\n')
	continue;
      linecount ++;
    }

  if (cbuffer[length - 1] != '\n')
    linecount ++;

  /* 
     Allocate memory for new buffer and \n at end of each line.  */

  new_length = (linecount + 2) * (option_R_margin + 1);

  new_cbuffer = (dynarray_uchar *) ALLOC_DYN (linecount + 2, option_R_margin + 1);

  /* Create the blank lines.  */

  memset (new_cbuffer->elem, ' ', new_length);

  new_cbuffer_end = new_cbuffer->elem + new_length + option_R_margin;

  for (new_cbuffer_work = new_cbuffer->elem + option_R_margin;
       new_cbuffer_work != new_cbuffer_end; 
       new_cbuffer_work += option_R_margin + 1)
    *new_cbuffer_work = '\n';

  /*
     Fill in the chars up to the R margin.  */

  linecount = 0;
  linecharno = 0;
  line_base = new_cbuffer->elem;
  for (charno = 0; charno < length; charno ++)
    {
      c = cbuffer[charno];
      if (c == '\n')
	{
	  linecount ++;
	  linecharno = 0;
	  line_base += option_R_margin + 1;
	}
      else
	{
	  if (linecharno < option_R_margin)
	    {
	      line_base[linecharno] = c;
	      linecharno ++;
	    }
	}
    }

  file_ptr->file_buffer = new_cbuffer->elem;
  file_ptr->file_length = new_length;

}
