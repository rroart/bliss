/* 

    BLISS Compiler almost main (blicc1)
    Called by gcc's toplev.c

    >>> experimental only, no use to man or beast <<<

    This is the main compiler.  It handles everything except certain
    text processing functions. If a program has any of the following
    features, it must be run through the preprocessor first (blipre):

    - copy verb (blipre includes the copy text into the program) -
    replace verb (blipre processes the replace verbs) 

    - continuation lines (blipre folds these together) 

    - comment-entries (author, etc) (blipre removes these, except it
    leaves the date-compiled paragraph so the compiler can fill it in
    the date.  

    - fixed position R margin. This program assumes that the rightmost
    position of the line is the R margin. blipre has an option to
    truncate and/or fill each line to a specified fixed R margin. Note
    that even in such cases, the file out of blipre does not have
    fixed length lines. This is due to continuation lines, text
    replacement etc. However the net effect is that the input file's
    lines are treated as though they were R margin bytes long.

    - lines do not end with spaces as required. Bliss requires that
    full stops, commas etc are followed by spaces. blipre can fix this up
    for you using the fixeol option.

    By default, this program assumes that the input file has sequence
    numbers in columns 1-6. However, this can be overridden by the
    -fnoseq option. With noseq, the lines start with the indicator area
    (the usual column 7).

    ---------------------------------------------------------------------------

    Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
    Written by Paul Rubin, June 1986
    Adapted to ANSI C, Richard Stallman, Jan 1987
    Adapted to BLISS by Tim Josling Mar-Apr 1999
    Some of this is derived from the 'toy' front end by Richard Kenner
    Some of the changes to this were made by a person known only as 'Jon'.

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

int yyparse(void) {
  return bliss_parse_file();
}
#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "flags.h"
#include "toplev.h"
#include "diagnostic.h"
#include "tree.h"
#include "ggc.h"

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "blir_temp_config.h"

#include "blidebug.h"
#include "blicsymn.h"
#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliclex.h"
#include "blicc1.h"
#include "blictkt.h"
#include "blicsyt.h"
#include "blicpru.h"
#include "blicsyms.h"
#include "blicprs.h"

/* Language for usage for messages.  */

const char *const language_string = 
   "Bliss for GCC - partial bliss85 compiler - version "
   BLISSFORGCC_VERSION;

/* Local prototypes.  */

void version (void);

uint32 generate_code (void *parse_tree);

/* Real parse module in bliprs.c.  */
native_int bli_parse (void);

/* Global variables.  */
extern struct bli_tree_struct_parse_tree_top *parse_tree_top;

/* Only [1] used, for main file.  */
struct bli_file_struct **file_structs_ptrs = NULL;

uint32 current_file_nbr = 0;  /* Current file number starting from 1.  */
/* 
   Options.  */

/* 1 If input file has no sequence numbers, ie columns 1-6 are missing.  */

uint32 option_no_seq = 0;

/* These fields set based on option_no_seq.  */

/* 0 Based offset of command area.  */
uint32 comment_offset = 6;
/* 0 Based offset of BLISS area a.  */
uint32 area_a_offset = 7;
/* 0 Based offset of BLISS area b.  */
uint32 area_b_offset = 11;

/* Trace the parser.  */

uint32 option_parser_trace = 0;

/* Trace the lexical analysis.  */

uint32 option_lexer_trace = 0;

/* Only do the lexical analysis.  */

uint32 option_lexer_only = 0;
extern native_int flag_syntax_only;  /* In toplev.c - laos needs to be set if lexer-only.  */

/* Program is main so create pointer to it called blir_main_address.  */

uint32 option_main = 0;
uchar *option_main_name = NULL;
uint32 option_main_found = 0;

/* Output messages as they are found not in textual order.  */

uint32 option_immediate_msg_output = 0;

/* Compiler debug mode is debug.  */

uint32 option_debug = 0;

/* Ptrs to names of fixed token types.  */
uchar **token_lit_ptrs = NULL;

/* Highest token ID number.  */
uint32 max_token_nbr = 0;

/* For flags below, see defines at token_flag_struct.  */
struct token_flag_struct * GTY(()) token_flags = NULL;  /* Flags for each token.  */
uint32 token_mask = TOKEN_FLAG_STD;  /* Flag mask. Standard BLISS 85.  */

/* Input file buffer.  */

void *bliu_fbuffer;

/* First token from lexical analysis.  */

struct bli_token_struct *first_token;

/* Local variables.  */

static uchar *in_fname = NULL;  /* Input file name.  */
static struct bli_file_struct * GTY(()) main_file_ptr = NULL;  /* Input file details.  */
static uint32 version_done = 0;  /* 1 If we have output the version string.  */

/* 
    Almost the main routine for bliss compiler.  FLAG non-zero means
    do parser trace.  */

struct dict {
  char *d_text;
  int   d_num;
};

extern struct dict keywords[];

void
bliss_parse_file (int flag)
{

  void *parse_tree = NULL;
  uint32 token_ix;

  if (flag)
    {
      option_immediate_msg_output = 1;
      option_parser_trace = 1;
      option_lexer_trace = 1;
      output_fyi ();
      output_eff ();
      output_obs ();
    }

  /* No input file and help.  */
   
  if (!in_fname)
    return;

  /* Init message processing.  */

  message_init ();

  /* Input options and names.  */

  main_file_ptr = BLI_ALLOC (sizeof (struct bli_file_struct));

  /*******************************************
   * Constant lookup table inits
   ******************************************  */

  max_token_nbr = 0;
#if 0
  for (token_ix = 0; token_ix < sizeof (token_lits) / sizeof (token_lits[0]);
       token_ix ++)
    if (token_lit_nbrs[token_ix] > max_token_nbr)
      max_token_nbr = token_lit_nbrs[token_ix];
#endif
  max_token_nbr = REALLY_MAX;
  BLI_ASSERT (max_token_nbr > 0);

  token_lit_ptrs = BLI_ALLOC (sizeof (uchar *)*(max_token_nbr + 1));

#if 0
  for (token_ix = 0; token_ix < sizeof (token_lits) / sizeof (token_lits[0]);
       token_ix ++)
    token_lit_ptrs[token_lit_nbrs[token_ix]] = (uchar *)token_lits[token_ix];
#endif

  for (token_ix = 0; token_ix < max_token_nbr;
       token_ix ++)
    token_lit_ptrs[token_ix] = (uchar *)keywords[token_ix].d_text;

  token_flags = BLI_ALLOC (sizeof (struct token_flag_struct) * (max_token_nbr + 1));

  for (token_ix = 0; token_ix < sizeof (token_flags_data) / sizeof (struct token_flag_struct); token_ix ++)
    memcpy (token_flags + token_flags_data[token_ix].token_nbr,
            token_flags_data + token_ix,
            sizeof (struct token_flag_struct));

  /*******************************************
   * Main processing
   ******************************************  */

  main_file_ptr->bli_file_name = in_fname;
  cmp_process_file (main_file_ptr,  & parse_tree);

  output_msgs ();
  output_summary ();
}


/*******************************************
  * Process one switch - called by toplev.c
  ******************************************  */

int
bliss_decode_option (int32 num_options_left ATTRIBUTE_UNUSED,
                   char **first_option_left)
{
  
  /*
    
    Process options - bear in mind I may get options that are really
    meant for someone else (eg the main compiler) so I have to be very
    permissive.  */
  
  if (first_option_left[0][0] != '-')
    return 0; 
  
  switch (first_option_left[0][1])
    {
    case '-' :
      if (!strcmp (first_option_left[0], "--help"))
        {
          if (!version_done)
            {
              fputs (language_string, stdout);
              fputs (version_string, stdout);
              fputs ("\n", stdout);
              version_done = 1;
            }
          fprintf (stdout, "Usage: bli1 [switches] -o output input\n");
          return 1;
        }
    case 'v' :
      if (!strcmp (first_option_left[0], "-v"))
        {
          if (!version_done)
            {
              fputs (language_string, stdout);
              fputs (version_string, stdout);
              fputs ("\n", stdout);
              version_done = 1;
            }
          return 1;
        }
    case 'f' :
      if (!strcmp (first_option_left[0], "-fimmediate-msg-output"))
        {
          option_immediate_msg_output = 1;
          return 1;
        }
      if (strlen (first_option_left[0]) > 6)
        if (!memcmp (first_option_left[0], "-fname", 6))
          {
            /* Allow gcc to put name in command so I can watch the test suite in top.  */
            return 1;
          }
      if (strlen (first_option_left[0]) > 6)
        if (!memcmp (first_option_left[0], "-fmain=", 7))
          {
            option_main = 1;
            option_main_name = (uchar*) first_option_left[0]+7;
            option_main_found = 0;
            return 1;
          }
      if (!strcmp (first_option_left[0], "-fparser-trace"))
        {
          option_parser_trace = 1;
          return 1;
        }
      if (!strcmp (first_option_left[0], "-flexer-trace"))
        {
          option_lexer_trace = 1;
          return 1;
        }
      if (!strcmp (first_option_left[0], "-flexer-only"))
        {
          option_lexer_only = 1;
          flag_syntax_only = 1;
          return 1;
        }
      if (!strcmp (first_option_left[0], "-fseq"))
        {
          option_no_seq = 0;
          comment_offset = 6;
          area_a_offset = 7;
          area_b_offset = 11;
          return 1;
        }
      if (!strcmp (first_option_left[0], "-fnoseq"))
        {
          option_no_seq = 1;
          comment_offset = 0;
          area_a_offset = 1;
          area_b_offset = 5;
          return 1;
        }
      if (!strcmp (first_option_left[0], "-fcompiler-subset"))
        {
          token_mask = TOKEN_FLAG_CORE;
          return 1;
        }
      return 0;

    case 'p' :
      if (!strcmp (first_option_left[0], "-pedantic"))
        {
          output_pedantic ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-pedantic-errors"))
        {
          error_pedantic ();
          token_mask = TOKEN_FLAG_STD;
          return 1;
        }
      return 0;

    case 'w' :
      if (!strcmp (first_option_left[0], "-w"))
        {
          no_output_warning ();
          return 1;
        }
      return 0;

    case 'W' :
      if (!strcmp (first_option_left[0], "-Wall"))
        {
          output_fyi ();
          output_eff ();
          output_obs ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WFYI"))
        {
          output_fyi ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WEfficiency"))
        {
          output_eff ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WObsolete"))
        {
          output_obs ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WNonStandard"))
        {
          output_pedantic ();
          return 1;
        }

      if (!strcmp (first_option_left[0], "-WnoWarning"))
        {
          no_output_warning ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WnoUnsupported"))
        {
          no_output_unsupported ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WWarning-error"))
        {
          error_warning ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WFYI-error"))
        {
          error_fyi ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WEfficiency-error"))
        {
          error_eff ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WObsolete-error"))
        {
          error_obs ();
          return 1;
        }
      if (!strcmp (first_option_left[0], "-WNonStandard-error"))
        {
          error_pedantic ();
          return 1;
        }

      if (!strcmp (first_option_left[0], "-WUnsupported-no-error"))
        {
          no_error_unsupported ();
          return 1;
        }

      return 0;

    default :
      return 0;
    }

  return 0;

}

/*************************************************************
  *  Language dependent parser setup
  ***********************************************************  */

const char *
bliss_init (const char *filename)
{
  bliumsc_set_progname ((uchar *)"bli1");

  bliss_init_decl_processing ();

  /* This error will not happen from gcc as it will always create a
     fake input file.  So it will only happen running the compiler
     bare (blicc1 rather than gcc).  */
  if (!filename || (filename[0] == ' ') || (!filename[0]))
    {
      if (!version_done)
        bliumsc_fatal ("no input file specified, try --help for help");

      in_fname = NULL;
      return NULL;
    }
  return (char *)((in_fname = (uchar *)filename));
}

/***********************************************************************
  * Process the input file lex it parse it analyse it output code for it 
  **********************************************************************  */

void
cmp_process_file (struct bli_file_struct *file_ptr,
                  void **parse_tree_ptr ATTRIBUTE_UNUSED)
{

  struct bli_token_struct *current;
  uint32 token_count;

  if (!file_names)
    {

      file_names = (dynarray_uchar_ptr *) ALLOC_DYN (10, sizeof (uchar *));
      file_names_lengths = (dynarray_uint32 *) ALLOC_DYN (10, sizeof (uint32));
      file_names_copied_original = (dynarray_uchar_ptr *) ALLOC_DYN (10, sizeof (uchar *));
      file_names_copied = (dynarray_uchar_ptr *) ALLOC_DYN (10, sizeof (uchar *));
      file_names_copied_lengths = (dynarray_uint32 *) ALLOC_DYN (10, sizeof (uint32));
      file_names_copied_original_lengths = (dynarray_uint32 *) ALLOC_DYN (10, sizeof (uint32));
      file_structs_ptrs = BLI_ALLOC (2 * sizeof (struct bli_file_struct *));
    }

  /* Need to copy to allow it to be GCed.  */
  file_names_lengths->elem[1] = strlen ((char *)in_fname);
  {
    void *a;
    file_names->elem[1] = BLI_ALLOC (file_names_lengths->elem[1] + 1);
    a = file_names->elem[1];
    memcpy (a, in_fname, file_names_lengths->elem[1]);
  }
  file_names_copied->elem[1] = NULL;
  file_names_copied_lengths->elem[1] = 0;
  file_names_copied_original->elem[1] = NULL;
  file_names_copied_original_lengths->elem[1] = 0;
  file_structs_ptrs[1] = file_ptr;
  file_count = 1;
  current_file_nbr = 1;  /* 0 Entry unused.  */

  /*
   * Read the whole file in.  */

  file_ptr->bli_file_buffer =
    read_file (file_ptr->bli_file_name ? file_ptr->bli_file_name : (uchar *)"",
               & (file_ptr->bli_file_length));

  if (!file_ptr->bli_file_buffer)
    {
      if (!version_done)
        MSG (MSGID (101), 1, 1, 0, NULL, NULL);
      return;
    }

  file_ptr->bli_current_position = file_ptr->bli_file_buffer;
  file_ptr->bli_current_line_start = file_ptr->bli_file_buffer;
  file_ptr->bli_end_position =
    file_ptr->bli_file_buffer + file_ptr->bli_file_length;

  yylexinit (file_ptr->bli_file_name, file_ptr->bli_file_buffer, file_ptr->bli_file_length);

  //  first_token = initial_lexical_analysis ( & token_count);

  if (option_lexer_trace)
    {
      printf ("Dump of tokens from preliminary lexical analysis\n");

      for (current = first_token; current; current = current->bli_token_next)
        {
          dump_token (stdout, (uchar *)"\n ", current);
        }

    }

  token_count=1000;

  create_symbol_table (token_count);

  /*
   * Parse it.  */

  yylexinit (file_ptr->bli_file_name, file_ptr->bli_file_buffer, file_ptr->bli_file_length);

  parse_init (first_token, option_parser_trace);

  if (!option_lexer_only)
    {
      bli_parse ();
      if (option_main && !option_main_found)
        {
          MSG (MSGID (133), 1, 1, 0, NULL, NULL);
        }
    }

  if (option_lexer_trace)
    {
      printf ("Dump of tokens from lexical analysis\n");
      for (current = first_token; current; current = current->bli_token_next)
        dump_token (stdout, (uchar *)"\n ", current);
    }
  if (option_lexer_only)
    return;

  /* Free the file buffer and surplus tokens.  */

  bliu_fbuffer = NULL;

  first_token = NULL;

  if ((!errorcount) && (!flag_syntax_only))
    {
      /* Generate the code.  */
      if (parse_tree_top)
        blic_bei (parse_tree_top);
    }
  
}

/**************************************************
  *  Get input for compiler lexical analysis 
  *************************************************  */

int32
getcompinput (char *buf, int32 max_size)
{

  uint32 size_to_copy;
  uint32 size_available;

  size_available =
    main_file_ptr->bli_end_position - main_file_ptr->bli_current_position;

  size_to_copy = (size_available > (uint32)max_size) ? (uint32)max_size : size_available;

  memcpy (buf, main_file_ptr->bli_current_position, size_to_copy);

  main_file_ptr->bli_current_position += size_to_copy;

  if (option_lexer_trace)
    {
      uint32 ix;
      printf ("\nfile IO (getcompinput) returning: length %d\ncharacters:\n",
              size_to_copy);
      if (size_to_copy)
        {
          for (ix = 0; ix < size_to_copy; ix ++)
            {
              printf ("%c", buf[ix]);
            }
          printf ("\nfile IO (getcompinput) in hex:\n");
          for (ix = 0; ix < size_to_copy; ix ++)
            {
              printf (" %2.2x", buf[ix]);
            }
        }
    }

  return size_to_copy;

}

/* Output an error message to the effect that a feature that occurred
   at token TOKEN is not supported given the options they have
   selected; the text of the message can be overridden by TEXT.  */

void
blic_turned_off_error (struct bli_token_struct *token, const uchar *text)
{
  MSG_TOKEN (MSGID (128), token, (uchar *)text);
}
