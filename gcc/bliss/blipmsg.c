/* 
 
   BLISS Compiler message handling (blipmsg.c)

   ----------------------------------------------------------------------

   Changes Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
   
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
   
   ----------------------------------------------------------------------.  */

#include "blir_temp_config.h"
#include "blidebug.h"
#include <stdio.h>
#include <string.h>
#include "ansidecl.h"
#define GTY(a) 
#include "bliumem.h"
#include "bliumsg.h"
#include "blipre.h"

/* COPY file hierarchy.  */
extern dynarray_file_ptr *file_ptrs;

/* The messages output by the preprocessor - note the caller can override the text.  */
/* 1-100 Reserved for preprocessor, 100-200 for main compiler.  */
struct bli_message_info message_info_array[] =
{
  {1, (uchar *)"Input file is empty.",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {2, (uchar *)"Invalid indicator character - line assumed to be comment and thus ignored",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {3, (uchar *)"Unrecognised input character - ignored",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {4, (uchar *)"Unrecognised input character - expecting \".\" - ignored",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {5, (uchar *)"(std) Comment not allowed in comment-entry - ignored",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {6, (uchar *)"(std) Replace verb is not preceded by fullstop or start of program, after COPY verbs have been processed",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {7, (uchar *)"Replace verb is on a debug line - undefined situation -  replace will be done",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {8, (uchar *)"(std) Use of (non-numeric) literal for library name is non-standard",
   0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
  {9, (uchar *)"(std) Fullstop is not followed by a space. Assumed space present", 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
  {10, (uchar *)"(std) Use of non-numeric literal for library member name is non-standard", 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
  {11, (uchar *)"(std) Non-numeric literal delimited by \' is non-standard", 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
  {12, (uchar *)"Unterminated non-numeric literal (missing \" or \' at end)", 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {13, (uchar *)"Parse Error", 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {14, (uchar *)"Replacing 'from' contains only spaces, comments, commas, semicolons or nothing", 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {15, (uchar *)"Integer invalid not 99...9 ",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {16, (uchar *)"Number invalid not [+-]999[.]999 ",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {17, (uchar *)"Number invalid - two decimal points/commas",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {18, (uchar *)"Bliss word starts with \"-\"",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {19, (uchar *)"Bliss word ends with \"-\" ",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {20, (uchar *)"(std) Bliss word contains _",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {21, (uchar *)"Bliss word does not consist of a..zA..Z0..9 and \'-\'",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {22, (uchar *)"(std) Bliss word longer than 30 characters - using full length",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {23, (uchar *)"Bliss word has no alpha character",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {24, (uchar *)"(std) Token should be in area B",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {25, (uchar *)"(option error - this text should be replaced by specific msg)",
   0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
  {26, (uchar *)"(opening copy file - this text should be replaced by specific msg)",
   0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0}, 
  {27, (uchar *)"(excessive nested copy files- this should be replaced by specific msg)",
   0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1}, 
  {28, (uchar *)"(std) Nested copy is non standard",
   0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 
  {29, (uchar *)"(std) Invalid continuation - in area A - used anyway",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {30, (uchar *)"Continuation line is blank",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {31, (uchar *)"Line continued by this line does not expect continuation "
   "as it ends with a terminated non-numeric literal"
   , 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {32, (uchar *)"Continuation line not preceded "
   "(before comments and blanks if present) by line requiring continuation"
   , 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {33, (uchar *)"Non-continued line has partial non numeric literal at the end"
   , 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {34, (uchar *)"Non-continued line not ended by properly terminated item ie one of \" ():\" or \"==\" or space",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {35, (uchar *)"Invalid continuation - non-numeric literal continued with differing delimiter",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {36, (uchar *)"Invalid continuation - non-numeric literal continued but not as non-numeric literal",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {37, (uchar *)"Non-standard continuation of = with = to make ==",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {38, (uchar *)"Incomplete non-numeric literal - added termination",
   0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {39, (uchar *)"Non standard (hex) literal",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
 
  {0, (uchar *)"Past End of List", 
   1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}
};


/* Get the details (FILE_NBR, LINE_NBR, BLI_CHAR_NBR) for a token
   VOID_STAR_TOKEN.  A similar routine is planned for the preprocessor which
   has a different notion of a token. Prototype in blicmsg.h.  */

void
bli_get_token_details (void_star_token, file_nbr, line_nbr, char_nbr, token_text)
     void *void_star_token;
     uint32 *file_nbr;
     uint32 *line_nbr;
     uint32 *char_nbr;
     uchar **token_text;
{
  struct token_struct *token1;

  token1 = void_star_token;

  *file_nbr = token1->token_file->file_number;
  *line_nbr = token1->token_lineno;
  *char_nbr = token1->token_charno;
  *token_text = BLI_ALLOC (token1->token_length + 1);
  memcpy (*token_text, token1->token_chars, token1->token_length);
}


/* Get the owning file and line number of a file number FILE_NBR and
   return FILE_NBR and line number LINE_NBR.  ie the file and line
   number the original file FILE_NBR if any that it was copied from.  */

void
get_owning_details (uint32 *file_nbr,
                    uint32 *line_nbr)
{
  struct file_struct *file_ptr;
  struct token_struct *token_ptr;

  if (! *file_nbr)
    {
      *line_nbr = 0;  /* Unknown.  */
      return;
    }
  file_ptr = file_ptrs->elem[*file_nbr];
  BLI_ASSERT (file_ptr);
  token_ptr = file_ptr->file_copy_token_ptr;

  if (token_ptr)
    {
      *file_nbr = token_ptr->token_file->file_number;
      *line_nbr = token_ptr->token_lineno;
    }
  else
    {
      *file_nbr = 0;
      *line_nbr = 0;
    }
  return;
}


