
/* 
   BLISS Compiler preprocessor utilities (blipppru.c)

   -------------------------------------------------------------------------------

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Adapted to BLISS by Tim Josling Mar-Apr 1999
   
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
   
   -------------------------------------------------------------------------------.  */

#include "blir_temp_config.h"

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#include "ansidecl.h"

#define GTY(a) 
#include "bliumem.h"
#include "bliumsg.h"
#include "blidebug.h"
#define INCLUDE_PRODUCTIONLITS go richo

#include "blipre.h"

#define INCLUDE_TOKENLITS YESYESYES
#include "bliplex.h"

uint32 token_table_entry_count;
uint32 max_token_nbr;

extern int option_allow_underscore_in_varnames;

/* Each entry points to the token string for that token value.  */
uchar **token_string_ptrs;
/* Pointers etc for repasting replaced tokens.  */
extern uint32 *ignore_tokens;
static uchar *tokenchars = NULL;
static uchar *tokenchars_ptr;
static uint32 total_length;

/****************************************************************
 * Print an input file whose filename is FILENAME, contained in BUFFER
 * which has length LENGTH
 ***************************************************************  */

void
dump_file (uchar *filename, uint32 length, uchar *buffer)
{
  uint32 i;
  /* Print the input file.  */
  fprintf (stdout, "\nInput file %s:\n\n", filename);
  for (i = 0; i < length; i ++)
    {
      fprintf (stdout, "%c", buffer[i]);
    }
}

/*******************************************
 * Print a line LINE number LINE_NBR from file FILENAME after fixing
 * up various error conditions
 ******************************************  */

void
dump_line (uchar *filename, uint32 line_nbr, struct line_struct *line)
{
  uchar *char_ptr;
  /* Id.  */
  fprintf (stdout, "\n%s:%d:", filename, line_nbr + 1);
  /* Sequence #.  */
  if (option_no_seq)
    {
      fprintf (stdout, "......");
    }
  else
    {
      for (char_ptr = line->line_start_orig;
	   (uint32)char_ptr < (uint32)(line->line_end_orig) &&
	   (uint32)char_ptr <
	   (uint32)(line->line_start_orig + comment_offset); char_ptr ++)
	{
	  fprintf (stdout, "%c",  * char_ptr);
	}
    }

  if ((uint32)(line->line_start_orig + area_a_offset) >
      (uint32)(line->line_end_orig))
    {
      return;
    }

  /* That's all if it's a continuation.  */
  if (line->line_flag_continuation)
    {
      /* Already printed it as part of the continued line.  */
      fprintf (stdout, "*(continuation - see in continued line %d)",
	       line->line_continued_line_nbr + 1);
      return;
    }

  fprintf (stdout, "%c", line->line_ind_char);

  for (char_ptr = line->line_start_new;
       (uint32)char_ptr < (uint32)(line->line_end_new); char_ptr ++)
    {
      fprintf (stdout, "%c",  * char_ptr);
    }

  /* Terminate erroneous partial strings.  */
  if (line->line_flag_wrap_string)
    {
      fprintf (stdout, "%c", line->line_string_char);
    }

  /* Add space at end for non-continued lines that need one.  */
  /* Or for erroneously continued lines to ensure it doesn't
     seem to work.  */
  if (line->line_flag_needs_delimiter)
    {
      fprintf (stdout, " ");
    }

  /* Space on the end of non-continued lines.  */
  if (option_fix_eol)
    {
      if ((uint32)(line->line_start_new) !=
	  (uint32)(line->line_end_new))
	{
	  if ( * (line->line_end_new - 1) != ' ')
	    fprintf (stdout, " ");
	}
    }

  if (line->line_flag_continued && !line->line_flag_continuation)
    {
      struct lineno_struct *lineno_ptr;
      struct line_details_struct *line_details_ptr;
      uint32 line_cont_ix;

      lineno_ptr = line->line_lineno_ptr;
      BLI_ASSERT (lineno_ptr);
      fprintf (stdout, "\nContinuation information: from %d lines",
	       lineno_ptr->lineno_count);
      for (line_cont_ix = 0; line_cont_ix < lineno_ptr->lineno_count;
	   line_cont_ix ++)
	{
	  line_details_ptr = lineno_ptr->lineno_line_details->elem + line_cont_ix;
	  fprintf (stdout, "\n %s %d at charno %d from line %d char %d",
		   line_cont_ix ? "Cont #" : "Start ",
		   line_cont_ix,
		   line_details_ptr->line_details_offset,
		   line_details_ptr->line_details_lineno + 1,
		   line_details_ptr->line_details_charno);
	}
    }
  else
    {
      BLI_ASSERT (!(line->line_lineno_ptr));
    }
}

/*******************************************
 * Print the LINE_COUNT lines stored in LINES array from file FILENAME
 * after fixing up
 ******************************************  */

void
dump_lines (uchar *filename, uint32 line_count, dynarray_line *lines)
{

  uint32 line_nbr;
  struct line_struct *line;

  /* Show the output.  */
  fprintf (stdout, "\nInput file lines after pasting:\n\n");
  for (line_nbr = 0; line_nbr < line_count; line_nbr ++)
    {
      line = (lines->elem + line_nbr);
      dump_line (filename, line_nbr, line);
    }
}

/*******************************************
 * Print a preprocessor token TOKEN on file F
 ******************************************  */

void
dump_token (FILE *f, struct token_struct *token)
{
  uchar *t;
  uchar c1;
  uint32 l;

  resolve_token_string_ptrs ();

  BLI_ASSERT (token);
  BLI_ASSERT (token->token_type <= max_token_nbr);

  fprintf (f, "line %d char %d F%d (%s) %s",
	   token->token_lineno,
	   token->token_charno,
	   token->token_file->file_number -1,  /* Compatibility hack.  */
	   token_string_ptrs[token->token_type],
	   (token->token_flag_debug_line ? "dbg" : ""));
  fprintf (f, " <");
  t = token->token_chars;
  l = token->token_length;
  while (l --)
    {
      c1 =  * t;
      if (((c1 > 0) && (c1 < 128)) && (isprint (c1)))
	  fprintf (f, "%c",  * (t ++));
      else
	  fprintf (f, "0x%2.2x", (uchar) *(t ++));
    }
  fprintf (f, ">");

}

/*******************************************
 * Build the token name table 
 ******************************************  */

void
resolve_token_string_ptrs ()
{
  uint32 token_ix;
  if (!token_string_ptrs)
    {
      token_table_entry_count = sizeof (token_lit_nbrs) / sizeof (int);
      max_token_nbr = 0;
      for (token_ix = 0; token_ix < token_table_entry_count; token_ix ++)
        if (token_lit_nbrs[token_ix] > max_token_nbr)
          max_token_nbr = token_lit_nbrs[token_ix];
      BLI_ASSERT (max_token_nbr > 0);
      token_string_ptrs = BLI_ALLOC ((max_token_nbr + 1) * sizeof (uint32));
      for (token_ix =  + 0; token_ix < token_table_entry_count; token_ix ++)
	  token_string_ptrs[token_lit_nbrs[token_ix]] = (uchar *)token_lits[token_ix];
    }
}

/*******************************************
 * Print preprocessor tokens, a linked list whose first entry is
 * FIRST_TOKEN, with TOKEN_COUNT tokens in list, from file FILENAME on
 * FILE F
 ******************************************  */

void
dump_tokens (FILE *f, uchar *filename,
	     struct token_struct *first_token, uint32 token_count)
{

  struct token_struct *next_token;

  fprintf (f, "\n\nInput file preprocessor tokens for %s: \n\n", filename);

  fprintf (f, "\nToken count %d \n\n", token_count);

  next_token = first_token;
  while (next_token)
    {
      fprintf (f, "\n");
      dump_token (f, next_token);
      next_token = next_token->token_next;
    }
}

/*******************************************
 * Print copy verb details stored on COPYREPL_ARRAY,
 * COPYREPL_ARRAY_COUNT in size, on file F
 ******************************************  */

void
dump_copyrepl (FILE *f, uchar *filename ATTRIBUTE_UNUSED,
	       dynarray_copyrepl *copyrepl_array,
	       uint32 copyrepl_array_count)
{
  uint32 copyrepl_ix;
  uint32 repl_ix;
  struct copyrepl_struct *copy_repl_work;
  struct replacing_struct *repl_work;

  for (copyrepl_ix = 0; copyrepl_ix < copyrepl_array_count; copyrepl_ix ++)
    {
      copy_repl_work =  & copyrepl_array->elem[copyrepl_ix];
      BLI_ASSERT (copy_repl_work);
      fprintf (f, "%s #%d %s %s #replacing %d ptr %s\n",
	       copy_repl_work->copyrepl_is_copy_flag ? "COPY" : "REPL",
	       copyrepl_ix,
	       copy_repl_work->copyrepl_valid_flag ? "Valid" : "Invalid",
	       copy_repl_work->copyrepl_replace_off_flag ? "OFF" : "",
	       copy_repl_work->copyrepl_replacing_count,
	       copy_repl_work->copyrepl_replacing ? "Yes" : "NULL");

      dump_token_maybe_with_lit (f, (uchar *)"start token:",
				 copy_repl_work->copyrepl_copyrepl_token_ptr);
      dump_token_maybe_with_lit (f, (uchar *)"end token  :",
				 copy_repl_work->copyrepl_fullstop_token_ptr);
      dump_token_maybe_with_lit (f, (uchar *)"filename   :",
				 copy_repl_work->copyrepl_filename_token_ptr);
      dump_token_maybe_with_lit (f, (uchar *)"library    :",
				 copy_repl_work->copyrepl_library_token_ptr);

      for (repl_ix = 0; repl_ix < copy_repl_work->copyrepl_replacing_count;
	   repl_ix ++)
	{
	  struct token_struct *work_token;
	  repl_work = copy_repl_work->copyrepl_replacing->elem + repl_ix;
	  BLI_ASSERT (repl_work);

	  fprintf (f, "  replacing #%d %s %s \n",
		   repl_ix,
		   repl_work->
		   replacing_from_pseudo_text_flag ? "from pseudo-text " : "",
		   repl_work->
		   replacing_to_pseudo_text_flag ? "to pseudo-text " : "");

	  dump_token_maybe_with_lit (f, (uchar *)"   from first token:",
				     repl_work->
				     replacing_from_first_token_ptr);
	  if (repl_work->replacing_from_first_token_ptr !=
	      repl_work->replacing_from_last_token_ptr)
	    {
	      for (work_token =
		   repl_work->replacing_from_first_token_ptr->token_next;
		   work_token
		   && (work_token !=
		       repl_work->replacing_from_last_token_ptr);
		   work_token = work_token->token_next)
		{
		  dump_token_maybe_with_lit (f, (uchar *)"                   :",
					     work_token);
		}
	    }
	  dump_token_maybe_with_lit (f, (uchar *)"   from last token:",
				     repl_work->
				     replacing_from_last_token_ptr);

	  dump_token_maybe_with_lit (f, (uchar *)"   to first token:",
				     repl_work->replacing_to_first_token_ptr);
	  if (repl_work->replacing_to_first_token_ptr !=
	      repl_work->replacing_to_last_token_ptr)
	    {
	      for (work_token =
		   repl_work->replacing_to_first_token_ptr->token_next;
		   work_token
		   && (work_token != repl_work->replacing_to_last_token_ptr);
		   work_token = work_token->token_next)
		{
		  dump_token_maybe_with_lit (f, (uchar *)"                   :",
					     work_token);
		}
	    }
	  dump_token_maybe_with_lit (f, (uchar *)"   to last token:",
				     repl_work->replacing_to_last_token_ptr);
	}
    }
}


/*******************************************
 * Print a production PRODIN, at level LEVEL (for indentation) on file
 * F
 ******************************************  */

void
dump_production (FILE *f, void *prodin, uint32 level)
{
  struct production **list;
  struct token_struct *token;
  uint32 count;
  struct production *p_in;
  uint32 ID;
  uint32 ix;
  uint32 level_ix;

  BLI_ASSERT (prodin);
  p_in = (struct production *)prodin;

  ID = p_in->production_ID;
  list = p_in->production_sub_productions;
  count = p_in->production_sub_production_count;

  BLI_ASSERT ((count != 0) == (list != NULL));

  BLI_ASSERT (ID > PR_dummy_start);
  BLI_ASSERT (ID < PR_dummy_end);

  fprintf (f, "\n");
  for (level_ix = 0; level_ix < level; level_ix ++)
    {
      fprintf (f, "  ");
    }
  fprintf (f, "Production %d %s", ID,
	   production_lits[ID - PR_dummy_start - 1]);
  for (ix = 0; ix < count; ix ++)
    {
      token = (struct token_struct *)list[ix];
      if (token->token_type < PR_dummy_start)
	{
	  fprintf (f, "\n");
	  for (level_ix = 0; level_ix < level + 1; level_ix ++)
	    {
	      fprintf (f, "  ");
	    }
	  dump_token (f, (void *)list[ix]);
	}
      else
	{
	  dump_production (f, list[ix], level + 1);
	}
    }
  fprintf (f, "\n");
  for (level_ix = 0; level_ix < level; level_ix ++)
    {
      fprintf (f, "  ");
    }
  fprintf (f, "Production end");
}

/*********************************************************
 * Find the one token underneath a lot of productions in a tree
 * starting from PROD_OR_TOKEN.  The first token found is used.
 ********************************************************  */

struct token_struct *
find_single_nested_token (void *prod_or_token)
{
  struct production *work_prod;

  /* If it's a token.  */
  if ((((struct token_struct *)prod_or_token)->token_type) < PR_dummy_start)
    {
      return (struct token_struct *)prod_or_token;
    }

  /* Else it's a production.  */
  work_prod = (struct production *)prod_or_token;

  BLI_ASSERT (work_prod->production_ID > PR_dummy_start);
  BLI_ASSERT (work_prod->production_ID < PR_dummy_end);
  BLI_ASSERT (1 == work_prod->production_sub_production_count);
  BLI_ASSERT (work_prod->production_sub_productions);
  return find_single_nested_token (work_prod->production_sub_productions[0]);
}

/*********************************************************
 * Maybe find the token underneath a tree of productions starting from
 * PROD_OR_TOKEN
 ********************************************************  */

struct token_struct *
find_single_nested_token_or_null (void *prod_or_token)
{
  struct production *work_prod;

  work_prod = (struct production *)prod_or_token;

  if (!(work_prod->production_sub_production_count))
    {
      return NULL;
    }

  BLI_ASSERT (work_prod->production_ID > PR_dummy_start);
  BLI_ASSERT (work_prod->production_ID < PR_dummy_end);
  BLI_ASSERT (1 == work_prod->production_sub_production_count);
  BLI_ASSERT (work_prod->production_sub_productions);
  return find_single_nested_token (work_prod->production_sub_productions[0]);
}

/*********************************************************
 * Find the details of the replacing clause parsed into PROD and add
 * it to the copy/replace data structures COPYREPL_DETAILS
 ********************************************************  */

void
fill_out_replacing_details (struct copyrepl_struct *copyrepl_details,
			    void *prod)
{

  struct production *work_prod;
  struct production *single_prod;
  struct production *multiple_prod;
  struct production *prod_from;
  struct production *prod_to;
  uint32 from_delimited_flag = 0;
  uint32 to_delimited_flag = 0;
  struct token_struct *from_first_token;
  struct token_struct *from_last_token;
  struct token_struct *to_first_token;
  struct token_struct *to_last_token;
  struct replacing_struct *current_replacing;

  BLI_ASSERT (ignore_tokens);

  work_prod = (struct production *)prod;
  BLI_ASSERT (
	  (
	   ((work_prod->production_ID == PR_replacing_clauses_single) &&
	    (work_prod->production_sub_production_count == 1))
	   ||
	   ((work_prod->production_ID == PR_replacing_clauses_multiple) &&
	    (work_prod->production_sub_production_count == 2)))
||
(
 ((work_prod->production_ID == PR_delimited_replacing_clauses_single) &&
  (work_prod->production_sub_production_count == 1))
 ||
 ((work_prod->production_ID == PR_delimited_replacing_clauses_multiple) &&
  (work_prod->production_sub_production_count == 2))));

  if (work_prod->production_sub_production_count == 2)
    {
      BLI_ASSERT (work_prod->production_sub_productions);
      multiple_prod = work_prod->production_sub_productions[0];
      BLI_ASSERT (multiple_prod);
      BLI_ASSERT ((multiple_prod->production_ID == PR_replacing_clauses_multiple)
	      || (multiple_prod->production_ID ==
		  PR_delimited_replacing_clauses_multiple)
	      || (multiple_prod->production_ID == PR_replacing_clauses_single)
	      || (multiple_prod->production_ID ==
		  PR_delimited_replacing_clauses_single));
      fill_out_replacing_details (copyrepl_details, multiple_prod);
    }

  if (work_prod->production_sub_production_count == 1)
    {
      BLI_ASSERT (work_prod->production_sub_productions);
      single_prod = work_prod->production_sub_productions[0];
    }
  else
    {
      BLI_ASSERT (work_prod->production_sub_production_count == 2);
      BLI_ASSERT (work_prod->production_sub_productions);
      single_prod = work_prod->production_sub_productions[1];
    }
  BLI_ASSERT (single_prod);

  BLI_ASSERT ((single_prod->production_ID == PR_replacing_clause)
	  || (single_prod->production_ID == PR_delimited_replacing_clause));
  BLI_ASSERT (single_prod->production_sub_production_count == 3);
  BLI_ASSERT (single_prod->production_sub_productions);
  BLI_ASSERT (single_prod->production_sub_productions[1]->production_ID ==
	  /*TOK_.  */BY);

  prod_from = single_prod->production_sub_productions[0];
  BLI_ASSERT (prod_from);
  get_replacing_from_to_details (prod_from,  & from_delimited_flag,
				 & from_first_token,  & from_last_token);
  BLI_ASSERT (from_last_token ? from_first_token : (void *)1);

  if (from_delimited_flag)
    {
      uint32 found_substance = 0;
      struct token_struct *scan_token;

      if (from_first_token)
	{
	  for (scan_token = from_first_token;
	       scan_token != from_last_token->token_next;
	       scan_token = scan_token->token_next)
	    {
	      if (!(ignore_tokens[scan_token->token_type]))
		{
		  found_substance = 1;
		  break;
		}
	    }
	}
      if (!found_substance)
	{
          struct token_struct *first_token;
          struct token_struct *last_token;
          uint32 delimited_flag = 0;
          first_token = NULL;
          last_token = NULL;
          delimited_flag = 0;
          scan_production (prod_from,  & delimited_flag,  & first_token,
                           & last_token);

          MSG_TOKEN (MSGID (14), first_token, NULL);
	  copyrepl_details->copyrepl_valid_flag = 0;
	}
    }

  prod_to = single_prod->production_sub_productions[2];
  BLI_ASSERT (prod_to);
  get_replacing_from_to_details (prod_to,  & to_delimited_flag,
				 & to_first_token,  & to_last_token);

  BLI_ASSERT (to_last_token ? to_first_token : (void *)1);

  //  allocate_replacing_struct ();

  current_replacing = copyrepl_details->copyrepl_replacing->elem
    + copyrepl_details->copyrepl_replacing_count - 1;

  current_replacing->replacing_from_pseudo_text_flag = from_delimited_flag;
  current_replacing->replacing_to_pseudo_text_flag = to_delimited_flag;
  current_replacing->replacing_from_first_token_ptr = from_first_token;
  current_replacing->replacing_from_last_token_ptr = from_last_token;
  current_replacing->replacing_to_first_token_ptr = to_first_token;
  current_replacing->replacing_to_last_token_ptr = to_last_token;
  return;
}

/*********************************************************
 * Scan a production PROD for first token => *FIRST_TOKEN, last token
 * => *LAST_TOKEN and whether it includes delimited text =>
 * DELIMITED_FLAG.
 ********************************************************  */

void
get_replacing_from_to_details (struct production *prod, uint32 *delimited_flag,
			       struct token_struct **first_token,
			       struct token_struct **last_token)
{
  * first_token = NULL;
  * last_token = NULL;
  * delimited_flag = 0;
  scan_production (prod, delimited_flag, first_token, last_token);
  if ( * delimited_flag)
    {
      if ((( * first_token)->token_next) == ( * last_token))
	{
	  * first_token = NULL;
	  * last_token = NULL;
	}
      else
	{
	  * first_token = ( * first_token)->token_next;
	  * last_token = ( * last_token)->token_prev;
	}
    }
}

/*********************************************************
 * Scan a sub production PROD for first token => *FIRST_TOKEN, last token
 * => *LAST_TOKEN and whether it includes delimited text =>
 * DELIMITED_FLAG.  Nested routine.
 ********************************************************  */

void
scan_production (struct production *prod, uint32 *delimited_flag,
		 struct token_struct **first_token,
		 struct token_struct **last_token)
{

  uint32 sub_prod_ix;
  struct production *sub_prod;

  if (( * first_token) && ( * first_token)->token_type ==  /*TOK_.  */PSEUDOTEXTDELIM)
    {
      * delimited_flag = 1;
    }

  /* If token.  */
  if (prod->production_ID < PR_dummy_start)
    {
      if (! * first_token)
	{
	  /* First token we hit is the first.  */
	  * first_token = (struct token_struct *)prod;
	}
      /* Every token we hit is the last unless there is another one.  */
      * last_token = (struct token_struct *)prod;
      return;
    }

  BLI_ASSERT (prod->production_ID > PR_dummy_start);
  BLI_ASSERT (prod->production_ID < PR_dummy_end);

  if (prod->production_sub_production_count == 0)
    return;  /* Opt something.  */

  BLI_ASSERT (prod->production_sub_productions);

  for (sub_prod_ix = 0; sub_prod_ix < prod->production_sub_production_count;
       sub_prod_ix ++)
    {
      sub_prod = prod->production_sub_productions[sub_prod_ix];
      BLI_ASSERT (sub_prod);
      scan_production (sub_prod, delimited_flag, first_token, last_token);
    }

}

/*********************************************************
 * Print an optional token TOKEN on file F with a string LIT first
 ********************************************************  */

void
dump_token_maybe_with_lit (FILE *f, const uchar *lit, struct token_struct *token)
{
  fprintf (f, "%s ", lit);
  if (token)
    dump_token (f, token);
  else
    fprintf (f, "is NULL");
  fprintf (f, "\n");
}


/*********************************************************
 * Validate an integer TOKEN warning if invalid
 ********************************************************  */

uint32
validate_integer (struct token_struct *token)
{
  uchar *chars;
  uint32 length;
  uint32 i;

  chars = token->token_chars;
  length = token->token_length;
  BLI_ASSERT (length > 0);

  for (i = 0; i < length; i ++)
    {
      if (isdigit (chars[i]))
	continue;
      MSG_TOKEN (MSGID (15), token, NULL);
      return 1;
    }
  return 0;
}


/*********************************************************
 * Validate a number TOKEN warning if invalid
 ********************************************************  */

uint32
validate_number (struct token_struct *token)
{
  uchar *chars;
  uint32 length;
  uint32 i;
  uint32 point_count = 0;

  chars = token->token_chars;
  length = token->token_length;
  BLI_ASSERT (length > 0);

  for (i = 0; i < length; i ++)
    {
      if (isdigit (chars[i]))
	continue;
      if ((chars[i] == '.') || (chars[i] == ','))
	{
	  point_count ++;
	  continue;
	}
      if ((i != 0) || ((chars[i] != '-') && (chars[i] != '+')))
	{
          MSG_TOKEN (MSGID (16), token, NULL);
	  return 1;
	}
    }
  if (point_count > 1)
    {
      MSG_TOKEN (MSGID (17), token, NULL);
      return 1;
    }
  return 0;
}

/*********************************************************
 * Validate a data-name etc TOKEN warning if invalid
 ********************************************************  */

uint32
validate_bliss_word (struct token_struct *token)
{
  uchar *chars;
  uchar *chars_upper;
  uint32 length;
  uint32 i;
  uint32 has_underscore = 0;

  chars = token->token_chars;
  length = token->token_length;
  BLI_ASSERT (length > 0);
  ensure_token_upper (token);
  chars_upper = token->token_chars_upper;
  if (chars[0] == (uchar)'-')
    {
      MSG_TOKEN (MSGID (18), token, NULL);
      return 1;
    }
  if (chars[length - 1] == (uchar)'-')
    {
      MSG_TOKEN (MSGID (19), token, NULL);
      return 1;
    }
  for (i = 0; i < length; i ++)
    {
      if (!isalnum (chars[i]))
	{
          if (chars[i] == (uchar)'_')
	    {
              has_underscore = 1;
	    }
	  if ((chars[i] != (uchar)'-') && (chars[i] != (uchar)'_'))
	    {
              MSG_TOKEN (MSGID (21), token, NULL);
	      return 1;
	    }
	}
    }
  if (has_underscore)
    {
      MSG_TOKEN (MSGID (20), token, NULL);
    }
  if (length > MAX_BLISS_WORD_LENGTH)
    {
      MSG_TOKEN (MSGID (22), token, NULL);
      return 1;
    }
  return 0;
}

/*********************************************************
 * Validate a data-name etc TOKEN warning if invalid or has no alpha
 * character
 ********************************************************  */

uint32
validate_bliss_word_strict (struct token_struct *token)
{
  uchar *chars;
  uint32 length;
  uint32 i;

  if (validate_bliss_word (token))
    return 1;

  chars = token->token_chars;
  length = token->token_length;
  BLI_ASSERT (length > 0);

  for (i = 0; i < length; i ++)
    {
      if (isalpha (chars[i]))
	return 0;
    }
  MSG_TOKEN (MSGID (23), token, NULL);
  return 1;
}

/*********************************************************
 * Ensure we have upper case version of the characters in token TOKEN
 ********************************************************  */

void
ensure_token_upper (struct token_struct *token)
{
  uint32 i;
  if (token->token_chars_upper)
    return;
  token->token_chars_upper = BLI_ALLOC (token->token_length);
  for (i = 0; i < token->token_length; i ++)
      token->token_chars_upper[i] = toupper (token->token_chars[i]);
}


/*************************************************************
 * Convert the copy verb identifier tokens in linked list FIRST_TOKEN
 * to token IDs that the parser will recognize
 ************************************************************  */

void
preprocess_copy_identifiers (struct token_struct *first_token,
			     struct token_struct *last_token ATTRIBUTE_UNUSED)
{
  struct token_struct *current_token;

  current_token = first_token;

  while (scan_for_type ( /*TOK_.  */COPY, 0, NULL,  & current_token))
    {
      fix_copy_statement ( & current_token);
    }
}


/*************************************************************
 * Scan forward from CURRENT_TOKEN_PTR for token type TYPE_TO_FIND.
 * updates *CURRENT_TOKEN_PTR.  ignores tokens within == ==
 * range. Stop looking when an entry in the TYPE_SHOW_STOPPER_ARRAY
 * array of size SHOW_STOPPER_COUNT is found.
 ************************************************************  */

uint32
scan_for_type (uint32 type_to_find, uint32 show_stopper_count,
	       uint32 *type_show_stopper_array,
	       struct token_struct **current_token_ptr)
{
  uint32 show_stopper_ix;
  uint32 in_pseudo_text = 0;
  while ( * current_token_ptr)
    {
      if (( **current_token_ptr).token_type ==  /*TOK_.  */PSEUDOTEXTDELIM)
	{
	  in_pseudo_text = 1 - in_pseudo_text;
	}
      if (!in_pseudo_text)
	{
	  if (( **current_token_ptr).token_type == type_to_find)
	    return 1;
	  for (show_stopper_ix = 0; show_stopper_ix < show_stopper_count;
	       show_stopper_ix ++)
	    {
	      if (( **current_token_ptr).token_type ==
		  type_show_stopper_array[show_stopper_ix])return 0;
	    }
	}
      * current_token_ptr = ( **current_token_ptr).token_next;
    }
  return 0;
}


/*************************************************************
 * Having found a copy verb, convert the identifier-names to
 * specialised types to handle the various structure that can form an
 * identifier.  Updates *CURRENT_TOKEN_PTR which points to the current
 * position in the linked list.  When looking at this code, be aware
 * that identifiers can be contained within other identifiers. See
 * doco for grok_identifier_type for details.
 ************************************************************  */


void
fix_copy_statement (struct token_struct **current_token_ptr)
{

  static uint32 show_stoppers[] = { /*TOK_.  */PERIOD_SPACE,  /*TOK_.  */PERIOD_NOSPACE};

  if (scan_for_type ( /*TOK_.  */REPLACING, 2, show_stoppers, current_token_ptr))
    {
      * current_token_ptr = ( **current_token_ptr).token_next;
    }
  else
    {
      return;
    }

  while (scan_for_type ( /*TOK_.  */NUMBERORNAMEORPIC,
			2, show_stoppers, current_token_ptr))
    {
      grok_identifier_type (current_token_ptr);
      * current_token_ptr = ( **current_token_ptr).token_next;
    }

}

/*************************************************************
 * Hack:
 *
 * (lexer feedback hack) 
 *
 * having found an identifier (NUMBERORNAMEORPIC)
 * work out if it has a reference modification right away
 * or one after an array reference
 * or a ref mod after a function call
 *
 * A (EXPRESSION:EXPRESSION) -> NUMBERORNAMEORPIC_REF_MOD_ONLY
 * A (EXPRESSION ...) ([EXPRESSION]:[EXPRESSION]) 
 * -> NUMBERORNAMEORPIC_ARRAY_REF_MOD
 * A  -> NUMBERORNAMEORPIC
 * A (EXPRESSION ...)  -> NUMBERORNAMEORPIC
 * number -> NUMBER
 * 
 * doesn't update *current_token_ptr due to nested identifiers.
 ************************************************************  */

void
grok_identifier_type (struct token_struct **current_token_ptr)
{

  struct token_struct *work_token;
  struct token_struct *tk1;
  uint32 paren_level = 0;
  uint32 got_colon = 0;
  uint32 got_array = 0;
  uint32 state = 0;  /* 1=After of/in.  */
  uchar *tkchars;
  uint32 charno;

  BLI_ASSERT (ignore_tokens);

  /* Check if it is a number.  */
  tk1 =  * current_token_ptr;
  tkchars = tk1->token_chars;

  tk1->token_type =  /*TOK_.  */NUMBER;
  for (charno = 0; charno < tk1->token_length; charno ++)
    {
      uchar ch;
      ch = tkchars[charno];
      if (ch == '.')
	{
	  continue;
	}
      if (isdigit (ch))
	{
	  continue;
	}
      if (ch == '-')
	{
	  if (charno == 0)
	    continue;
	}
      if (ch == '+')
	{
	  continue;
	}
      tk1->token_type =  /*TOK_.  */NUMBERORNAMEORPIC_IDENTIFIER_BASE;
      break;
    }

  if (tk1->token_type ==  /*TOK_.  */NUMBER)
    {
      return;
    }

  /* Kludgy scan follows.  */

  work_token = ( * current_token_ptr)->token_next;

  while (work_token)
    {
      switch (work_token->token_type)
	{
	case /*TOK_.  */LPAREN :
	  if (state != 0)
	    goto done;
	  paren_level ++;
	  work_token = work_token->token_next;
	  break;
	case /*TOK_.  */RPAREN :
	  if (state != 0)
	    goto done;
	  paren_level --;
	  if (!paren_level)
	    {
	      if (got_array)
		goto done;
	      got_array ++;
	    }
	  work_token = work_token->token_next;
	  break;
	case /*TOK_.  */COLON :
	  if (state != 0)
	    goto done;
	  if (paren_level == 1)
	    {
	      got_colon = 1;
	      goto done;
	    }
	  if (paren_level < 1)
	    goto done;
	  work_token = work_token->token_next;
	  break;
	case /*TOK_.  */PERIOD_SPACE :
	case /*TOK_.  */PERIOD_NOSPACE :
	  goto done;
	case /*TOK_.  */OF :
	case /*TOK_.  */IN :
	  if ((state) && (!paren_level))
	    goto done;
	  if (!state && !paren_level)
	    {
	      state = 1;
	    }
	  work_token = work_token->token_next;
	  break;
	case /*TOK_.  */NUMBERORNAMEORPIC :
	  if (state == 2)
	    {
	      state = 0;
	      work_token = work_token->token_next;
	      break;
	    }
	  if ((state != 1) && (!paren_level))
	    goto done;
	  state = 0;
	  work_token = work_token->token_next;
	  break;
	default :
	  if (!(ignore_tokens[work_token->token_type]))
	    {
	      if (!paren_level)
		goto done;
	    }
	  work_token = work_token->token_next;
	  break;
	}
    }
done :
  if (got_colon)
    {
      if (got_array)
	{
	  ( **current_token_ptr).token_type =  /*TOK_.  */NUMBERORNAMEORPIC_ARRAY_REF_MOD;
	}
      else
	{
	  ( **current_token_ptr).token_type =  /*TOK_.  */NUMBERORNAMEORPIC_REF_MOD_ONLY;
	}
    }
}

/******************************************************* 
                                                         
Do the replace of tokens according to the rules.  The replace is
detailed in COPYREPL_PTR, the scope is from START_TOKEN to
END_TOKEN. If SKIP_LEADING_WHITESPACE_FLAG is on, ignore (keep)
leading whitespace as required for COPY/REPLACING.
                                                         
******************************************************  */

void
process_replaces (struct copyrepl_struct *copyrepl_ptr,
		  struct token_struct *start_token,
		  struct token_struct *end_token,
		  uint32 skip_leading_whitespace_flag)
{
  struct token_struct *current_token;
  struct token_struct *current_check_replacing_token;
  struct token_struct *current_check_file_token;
  struct token_struct *count_token;
  struct replacing_struct *current_replacing;
  uint32 current_replacing_ix;
  uint32 nbr_replacing_tokens;
  struct token_struct *cleanup_token;

  BLI_ASSERT (start_token);
  BLI_ASSERT (end_token);
  BLI_ASSERT (copyrepl_ptr);

  /* Reset the special token IDs used to fake the parse.  */

  for (cleanup_token = copyrepl_ptr->copyrepl_copyrepl_token_ptr;
       cleanup_token &&
       (cleanup_token != copyrepl_ptr->copyrepl_fullstop_token_ptr);
       cleanup_token =
       (cleanup_token !=
	copyrepl_ptr->copyrepl_fullstop_token_ptr ? cleanup_token->
	token_next : NULL))
    {
      if ((cleanup_token->token_type ==  /*TOK_.  */NUMBERORNAMEORPIC_IDENTIFIER_BASE)
	  || (cleanup_token->token_type ==  /*TOK_.  */NUMBERORNAMEORPIC_REF_MOD_ONLY)
	  || (cleanup_token->token_type ==
	      /*TOK_.  */NUMBERORNAMEORPIC_ARRAY_REF_MOD)
	  || (cleanup_token->token_type ==  /*TOK_.  */NUMBER))
	{
	  cleanup_token->token_type =  /*TOK_.  */NUMBERORNAMEORPIC;
	}
    }

  if (!(copyrepl_ptr->copyrepl_replacing_count))
    return;

  BLI_ASSERT (ignore_tokens);

  current_token = start_token;

  if (skip_leading_whitespace_flag)
    {

      /* 
         Keep leading whitespace & "," & ";"  unconditionally 
         That's what it says to do for copy statement with replace.  */

      for (;
	   current_token;
	   current_token =
	   ((current_token
	     == end_token ? NULL : (current_token->token_next))))
	{
	  if (!ignore_tokens[current_token->token_type])
	    break;
	}
      /* We have moved the pointer forward and thereby retained the tokens.  */
    }

  /* Copy/match for the rest of the scope.  */

  for (;
       current_token;
       current_token =
       ((current_token == end_token) ? NULL : (current_token->token_next)))
    {

      uint32 match_flag = 0;

      for (current_replacing_ix = 0;
	   (current_replacing_ix < copyrepl_ptr->copyrepl_replacing_count)
	   && (!match_flag); current_replacing_ix ++)
	{

	  struct token_struct *first_non_whitespace_token = NULL;

	  current_replacing =
	    copyrepl_ptr->copyrepl_replacing->elem + current_replacing_ix;
	  BLI_ASSERT (current_replacing);

	  /* Check for match here to this 'from'.  */

	  current_check_replacing_token =
	    current_replacing->replacing_from_first_token_ptr;
	  current_check_file_token = current_token;

	  /* Loop - check tokens for match.  */
	  while (1)
	    {

	      /* Did we run out of tokens to check.  */
	      if ((!current_check_file_token)
		  || (current_check_file_token == end_token))break;

	      /* Skip whitespace in from replacing.  */

	      while (current_check_replacing_token &&
		     ignore_tokens[current_check_replacing_token->token_type])
		{
		  if (current_check_replacing_token !=
		      current_replacing->replacing_from_last_token_ptr)
		    {
		      current_check_replacing_token =
			current_check_replacing_token->token_next;
		    }
		  else
		    {
		      current_check_replacing_token = NULL;
		    }
		}

	      if (!current_check_replacing_token)
		{
		  if (first_non_whitespace_token)
		    {
		      match_flag = 1;  /* Everything (at least one thing) matched.  */
		    }
		  break;
		}

	      /* Skip whitespace in file.  */

	      while (current_check_file_token &&
		     ignore_tokens[current_check_file_token->token_type])
		{
		  if (current_check_file_token != end_token)
		    {
		      current_check_file_token =
			current_check_file_token->token_next;
		    }
		  else
		    {
		      current_check_file_token = NULL;
		    }
		}

	      if (!current_check_file_token)
		break;  /* End of file, but not end of match list.  Try next 'from'.  */

	      /* If the two tokens differ, try next match set.  */

	      if (current_check_replacing_token->token_type !=
		  current_check_file_token->token_type)break;

	      if (current_check_replacing_token->token_length !=
		  current_check_file_token->token_length)break;

	      ensure_token_upper (current_check_replacing_token);
	      ensure_token_upper (current_check_file_token);

	      if (memcmp (current_check_replacing_token->token_chars_upper,
			  current_check_file_token->token_chars_upper,
			  current_check_replacing_token->token_length))break;

	      /* This token is used to set the debug of the copied text.  */
	      if (!first_non_whitespace_token)
		{
		  first_non_whitespace_token = current_check_file_token;
		}

	      /* If we are at the end, set found flag and exit.  */

	      if (current_check_replacing_token ==
		  current_replacing->replacing_from_last_token_ptr)
		{
		  match_flag = 1;
		  break;
		}

	      /* They are the same but we are not at the end.  skip to next and test again.  */

	      if (!current_check_file_token ||
		  current_check_file_token == end_token)
		{
		  break;
		}

	      current_check_file_token = current_check_file_token->token_next;

	      current_check_replacing_token =
		current_check_replacing_token->token_next;

	    }

	  if (match_flag)
	    {
	      /* Remove the matching text.  */
	      if (first_non_whitespace_token->token_prev)
		{
		  first_non_whitespace_token->token_prev->token_next =
		    current_check_file_token->token_next;
		}
	      if (current_check_file_token->token_next)
		{
		  current_check_file_token->token_next->token_prev =
		    first_non_whitespace_token->token_prev;
		}

	      /* Count tokens to replace the old text.  */
	      nbr_replacing_tokens = 0;
	      for (count_token =
		   current_replacing->replacing_to_first_token_ptr;
		   count_token;
		   count_token =
		   (count_token ==
		    current_replacing->
		    replacing_to_last_token_ptr) ? NULL : count_token->
		   token_next)
		{
		  nbr_replacing_tokens ++;
		}

	      /* Do the replace.  */

	      if (nbr_replacing_tokens)
		{
		  struct token_struct *new_tokens;
		  struct token_struct *copy_token_from;
		  struct token_struct *copy_token_to;
		  uint32 token_nbr;
		  new_tokens = BLI_ALLOC (nbr_replacing_tokens * sizeof (struct token_struct));
		  token_nbr = 0;
		  copy_token_from =
		    current_replacing->replacing_to_first_token_ptr;
		  while (token_nbr < nbr_replacing_tokens)
		    {

		      copy_token_to = new_tokens + token_nbr;
		      memcpy (copy_token_to, copy_token_from,
			      sizeof (struct token_struct));

		      /* Link to prev/next; first last case fixed up later.  */
		      copy_token_to->token_prev = new_tokens + token_nbr - 1;
		      copy_token_to->token_next = new_tokens + token_nbr + 1;

		      BLI_ASSERT (first_non_whitespace_token);

		      if (copyrepl_ptr->copyrepl_is_copy_flag)
			{
			  copy_token_to->token_flag_debug_line |=
			    (first_non_whitespace_token->
			     token_flag_debug_line) | (copyrepl_ptr->
						       copyrepl_copyrepl_token_ptr->
						       token_flag_debug_line);
			}
		      else
			{
			  copy_token_to->token_flag_debug_line =
			    first_non_whitespace_token->token_flag_debug_line;
			}
		      copy_token_to->token_flag_replacement = 1;
		      copy_token_to->token_flag_recent_replacement = 1;
		      /* 
		         Fix up the text location 
		         for text in pseudo-text-2, the location in the pseudo text is used
		         otherwise the location of the first replaced token is used.
		         When a token is split across lines and is involved in a replace 
		         I ignore the split as there is no reasonable way to handle it.  */
		      copy_token_to->token_lineno =
			first_non_whitespace_token->token_lineno;
		      copy_token_to->token_file =
			first_non_whitespace_token->token_file;

		      if (current_replacing->replacing_to_pseudo_text_flag)
			{
			  copy_token_to->token_flag_from_pseudo_text = 1;
			}
		      else
			{
			  copy_token_to->token_flag_from_pseudo_text = 0;
			  copy_token_to->token_charno =
			    first_non_whitespace_token->token_charno;
			}
		      copy_token_from = copy_token_from->token_next;
		      token_nbr ++;
		    }

		  /* Link them in.  */

		  new_tokens[token_nbr - 1].token_next =
		    current_check_file_token->token_next;
		  new_tokens[0].token_prev =
		    first_non_whitespace_token->token_prev;
		  current_check_file_token->token_next->token_prev =
		    new_tokens + token_nbr - 1;
		  if (first_non_whitespace_token->token_prev)
		    {
		      first_non_whitespace_token->token_prev->token_next =
			new_tokens;
		    }
		  current_token = current_check_file_token;
		} /* New text inserted.  */
	      goto get_next_token;
	    } /* Found a match.  */
	} /* Next possible replacing.  */
    get_next_token :;
    } /* Next token.  */
}


/*******************************************************
 * Repaste the replaced tokens in FILE_PTR if needed
 ******************************************************  */

void
redo_tokenising (struct file_struct *file_ptr)
{

  struct token_struct *tk1;
  struct token_struct *tk2;
  struct token_struct *tk_first;
  struct token_struct *tk_last;
  struct token_struct *tk_scan;
  uint32 work_token_count = 0;
  uint32 tokenchars_length = 0;
  uint32 token_count;
  uint32 this_token;
  uint32 diff_count;
  uint32 current_offset;
  struct mini_token_struct
  {
    uint32 tok_type;
    uint32 tok_length;
    uint32 tok_offset;
  };
  typedef struct mini_token_struct mini_token;
  
  struct GTY(()) dynarray_mini_token_struct
    {
      struct dynarray_struct dynarray_details;
      mini_token * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
    };
  
  typedef struct dynarray_mini_token_struct dynarray_mini_token;

  dynarray_mini_token *token_detail_array = NULL;
  struct token_struct *new_tokens;
  struct token_struct *new_first_token;
  struct token_struct *new_last_token;
  uint32 new_token_ix;
  struct token_struct *tk;
  struct token_struct *new_token = NULL;

  /* Scan file looking for replaced tokens.  */
  for (tk1 = file_ptr->file_first_token;
       tk1; tk1 = tk1 == file_ptr->file_last_token ? tk1->token_next : NULL)
    {
      uint32 have_replaced_token;
      if ((ignore_tokens[tk1->token_type])
	  && (ignore_tokens[tk1->token_type] != IGNORE_TOKENS_NOISE))
	continue;

      have_replaced_token = tk1->token_flag_recent_replacement;
      tk2 = tk1;
      total_length = 0;
      token_count = 0;

      /* Scan for end of contiguous tokens.  */
      while (tk2->token_next &&
	     (tk2->token_next->token_type !=  /*TOK_.  */EOF_symbol) &&  /* ???  */
	     ((!ignore_tokens[tk2->token_next->token_type])
	      || (ignore_tokens[tk1->token_type] == IGNORE_TOKENS_NOISE)))
	{
	  tk2 = tk2->token_next;
	  have_replaced_token |= tk2->token_flag_recent_replacement;
	  tk2->token_flag_recent_replacement = 0;
	  token_count ++;
	  total_length += tk2->token_length;
	}

      tk_first = tk1;
      tk_last = tk2;
      tk1 = tk_last;  /* For loop iteration, skip these ones.  */

      if (token_count < 2)
	continue;
      if (!have_replaced_token)
	continue;

      tokenchars_ptr = tokenchars;

      if (total_length > tokenchars_length)
	{
	  /* Produce a string including all the token chars.  */
	 tokenchars = BLI_ALLOC (total_length);
	}

      for (tk_scan = tk_first;
	   tk_scan != tk_last->token_next; tk_scan = tk_scan->token_next)
	{
	  memcpy (tokenchars_ptr, tk_scan->token_chars,
		  tk_scan->token_length);
	  tokenchars_ptr += tk_scan->token_length;
	}

      tokenchars_ptr = tokenchars;

      /* Find the tokens - types and lengths.  */

      lexinit (LEXINIT_REDO_TOKENS, file_ptr);
      tk_scan = tk_first;
      diff_count = 0;
      work_token_count = 0;

      if (!token_detail_array)
	  token_detail_array = 
            (dynarray_mini_token *)
            ALLOC_DYN (6, sizeof (struct mini_token_struct));

      current_offset = 0;

      while (0 != (this_token = yylex ()))
	{

	  work_token_count ++;

	  if (tk_scan->token_length != (uint32)yyleng)
	    diff_count ++;
	  else
	    {
	      if (tk_scan->token_type != this_token)
		diff_count ++;
	    }

	  GROW_DYN (&token_detail_array->dynarray_details, work_token_count);

	  token_detail_array->elem[work_token_count - 1].tok_type = this_token;
	  token_detail_array->elem[work_token_count - 1].tok_length = yyleng;
	  token_detail_array->elem[work_token_count - 1].tok_offset =
	    current_offset;
	  current_offset += yyleng;
	}

      if (!diff_count)
	continue;  /* Usually nothing to do ...  */

      /* Fix up period followed by space.  */
      if ((this_token ==  /*TOK_.  */PERIOD_NOSPACE)
	  && (tk_last->token_next)
	  && (tk_last->token_next->token_length > 0)
	  && (tk_last->token_next->token_chars[0] == ' '))
	{
	  token_detail_array->elem[work_token_count - 1].tok_type =
	    /*TOK_.  */PERIOD_SPACE;
	  if (tk_last->token_type ==  /*TOK_.  */PERIOD_SPACE)
	    {
	      diff_count --;
	    }
	}

      if (!diff_count)
	continue;  /* May be nothing to do now.  */

      /* 
         Turn the old tokens and replace them with the new ones take
         line no and char no and atributes from the token the new one
         starts in.  */

      new_tokens = BLI_ALLOC (sizeof (struct token_struct) *work_token_count);
      new_first_token = new_tokens;
      new_last_token = new_tokens + work_token_count - 1;

      for (new_token_ix = 0; new_token_ix < work_token_count; new_token_ix ++)
	{
	  struct token_struct *find_token;
	  uint32 find_token_offset = 0;

	  tk = new_tokens + new_token_ix;

	  tk->token_type = token_detail_array->elem[new_token_ix].tok_type;
	  tk->token_length = token_detail_array->elem[new_token_ix].tok_length;
	  tk->token_chars =
	    tokenchars + token_detail_array->elem[new_token_ix].tok_offset;
	  tk->token_next = new_tokens + new_token_ix + 1;
	  tk->token_prev = new_tokens + new_token_ix - 1;;

	  /* 
	     Find the first token that this one comes from.  */

	  for (find_token = tk_first;
	       find_token;
	       find_token =
	       (find_token == tk_last) ? NULL : find_token->token_next)
	    {

	      if (find_token_offset + find_token->token_length >
		  token_detail_array->elem[new_token_ix].tok_offset)
		{

		  /*
		     Fill in the fields from the first token.  */

		  new_token->token_lineno = find_token->token_lineno;
		  new_token->token_charno = find_token->token_charno;
		  new_token->token_file = find_token->token_file;
		  new_token->token_flag_debug_line =
		    find_token->token_flag_debug_line;
		  new_token->token_flag_replacement =
		    find_token->token_flag_replacement;
		  new_token->token_flag_from_pseudo_text =
		    find_token->token_flag_from_pseudo_text;
		  break;
		}

	      find_token_offset += find_token->token_length;
	    }

	  BLI_ASSERT (find_token);

	}

      new_first_token->token_prev = tk_first->token_prev;
      new_last_token->token_next = tk_last->token_next;
      new_first_token->token_prev->token_next = new_first_token;
      new_last_token->token_next->token_prev = new_last_token;

      tokenchars_length = 0;  /* If tokens used.  */
    }
}

/*******************************************************

 Provide the character to re-lex - used when reparsing pasted tokens

 ******************************************************  */

extern uint32
getinput_repaste (uchar *buf, uint32 max_size)
{
  uint32 size_to_return;

  size_to_return = (total_length - (tokenchars_ptr - tokenchars));
  if (max_size <= size_to_return)
    {
      size_to_return = max_size;
    }
  if (size_to_return)
    {
      memcpy (buf, tokenchars_ptr, size_to_return);
      tokenchars_ptr += size_to_return;
    }
  return size_to_return;
}


/*******************************************************
 * Check token TK is in area b
 ******************************************************  */

void
check_area_b (struct token_struct *tk)
{
  if ((tk->token_charno + 6 * !!option_no_seq) < 12)
    {
      MSG_TOKEN (MSGID (24), tk, NULL);
    }
}

/*******************************************************
 * Print file copy hierarchy of FILE_PTR on PRBLI_INTFILE
 ******************************************************  */

void
print_file_hierarchy (struct file_struct *file_ptr, FILE *printfile)
{

  struct token_struct *tk1;
  struct token_struct *parent_token;

  tk1 = file_ptr->file_copy_token_ptr;

  if (!tk1)
    return;

  for (parent_token = tk1->token_file->file_copy_token_ptr;
       parent_token;
       parent_token = parent_token->token_file->file_copy_token_ptr)
    {
      fprintf (printfile, "\n    copied from %s:%i",
	       parent_token->token_file->file_name,
	       parent_token->token_lineno);
    }
}

void *ggc_alloc (size_t size1);

/* Dummy routine.  */
void *ggc_alloc (size_t size1 ATTRIBUTE_UNUSED)
{
  abort ();
}

