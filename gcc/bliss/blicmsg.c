/* 
 
   BLISS Compiler messages for the main compiler (blicmsg.c)

   ---------------------------------------------------------------------

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
   
   ---------------------------------------------------------------------.  */
typedef void *tree;
#include <stdio.h>
#include <string.h>
#include "ansidecl.h"
#define GTY(a) 
#include "blir_temp_config.h"

#include "blidebug.h"
#include "blicsymn.h"
#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliclex.h"
#include "blicc1.h"
#include "blicsyt.h"
#include "blicpru.h"

/* 1-100 Reserved for preprocessor, 100+ for main compiler.  */
struct bli_message_info message_info_array[] =
{
  {101, (uchar *)"Input File is Empty",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {113, (uchar *)"Parse Error",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {114, (uchar *)"Non standard Keyword",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {115, (uchar *)"Underscore in variable name",
   0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {116, (uchar *)"Incomplete non-numeric literal - added termination",
   0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {117, (uchar *)"Non standard (hex) literal",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {118, (uchar *)"Period not followed by space",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {119, (uchar *)"Unrecognized character in input",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {120, (uchar *)"Continuation character in indicator - run program through preprocessor first ",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {121, (uchar *)"Unrecognized character in identifier ",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {122, (uchar *)"Parser trace cannot be enabled - blicprs.tpl not compiled with YYDEBUG set ",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1}, 
  {123, (uchar *)"Display usage but no PIC",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {124, (uchar *)"Non-numeric literal but usage not display ",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {125, (uchar *)"Non-numeric literal too long for data item ",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {126, (uchar *)"Literal not consistent with usage ",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {127, NULL,  /* Feature not currently supported.  */
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {128, NULL,  /* Feature not supported with options selected.  */
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {129, (uchar *)"Invalid numeric literal - too large",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {130, (uchar *)"Invalid numeric literal - invalid sign",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {131, (uchar *)"Invalid hex literal - odd number of digits",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {132, (uchar *)"Invalid hex literal - invalid hex digit",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {133, (uchar *)"Main routine specified but not found",
   0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {134, (uchar *)"Invalid level number",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {135, (uchar *)"Level number hierarchy error - not 01/77 level but no prior 01 level",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {136, (uchar *)"Level number hierarchy error - not 01/77 level but cannot fit into current structure",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {137, (uchar *)"Data item is under item which is not group item",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {138, (uchar *)"Lowest level data item is a group (no pic/usage)",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {139, (uchar *)"Too many levels of 'of' or 'in' in identifier; used first 50 levels only",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {140, (uchar *)"Ambiguous name - cannot be referenced",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {141, (uchar *)"Ambiguous reference",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {142, (uchar *)"Item referenced, and with correct type, was not found",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {143, (uchar *)"Data item length from PIC is zero",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {144, (uchar *)"Pic present but is not consistent with usage",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {145, (uchar *)"Invalid redefines - redefine name does not match "
   "previous at same level that is not redefined",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {146, (uchar *)"Redefine of non 01 level area is larger than original area",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {147, (uchar *)"Low range of occurs depending not less than high range",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {148, (uchar *)"Occurs depending on item not last within 01/77 level",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {149, (uchar *)"Occurs not allowed on level 01 77 66 or 88",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {150, (uchar *)"Fixed number of occurs must be for more than 0 entries",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {151, (uchar *)"Occurs depending on may not be under another occurs",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {152, (uchar *)"Linkage section initial values ignored except in INITIALIZE statement",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {153, (uchar *)"Value will be truncated",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {154, (uchar *)"Empty table index list",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {155, (uchar *)"Non integral variable controls occurs depending on",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {156, (uchar *)"Number of table indexes does not match number of dimensions in table reference",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {157, (uchar *)"Invalid (negative or too large) value used within table occurrence expression",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {158, (uchar *)"Section out of order",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {159, (uchar *)"Section repeated",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {160, (uchar *)"Expecting pointer",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {161, (uchar *)"Setting address of item which is not linkage section 01/77 level and not redefine",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {162, (uchar *)"Invalid redefines - redefine of filler",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {163, (uchar *)"Language specified not C, ignored",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {164, (uchar *)"Statement not preceded by paragraph",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {165, (uchar *)"User defined name used as more than one type of user defined name",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {166, (uchar *)"Ambiguous section or paragraph, may not be referenced",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {167, (uchar *)"Non integral expression but integral expression required",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {168, (uchar *)"Invalid source for move",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {169, (uchar *)"Invalid destination for move",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {170, (uchar *)"Incompatible source and destination in move",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {171, (uchar *)"Value cannot override value at higher level",
   0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {172, (uchar *)"Table reference without indexes",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {173, (uchar *)"Redefining item may not have initial value",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {174, (uchar *)"Multiple values and value range can only be specified in a level 88 item",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {175, (uchar *)"Non display usage within group item with initial value",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {176, (uchar *)"Exit without 'program' must be on its own in a paragraph",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {177, (uchar *)"Numeric exceeds size of 18 digits signed or 19 unsigned",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {178, (uchar *)"Non standard numeric size > 18 digits",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {179, (uchar *)"Numeric value too large",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {180, (uchar *)"PIC is not in a  supported format",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {181, (uchar *)"Signed numic literal but unsigned value required",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {182, (uchar *)"ALL is not allowed for NULL or for signed integer, ignored",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {183, (uchar *)"MOVE ALL iteral to numeric item without pic: ALL ignored",
   0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {184, (uchar *)"GO TO considered harmful by some (this warning once per compilation only)",
   0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, 
  {185, (uchar *)"MOVE ALL literal length > 1 to numeric with pic: obsolete, strange results",
   0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {186, (uchar *)"Level 88 without data item to refer to",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {187, (uchar *)"Level 88 must have a name (which is not 'filler')",
   0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {188, (uchar *)"Level 88 must not have PICTURE",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {189, (uchar *)"Level 88 must not have USAGE",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {190, (uchar *)"Level 88 must not have REDEFINES",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {191, (uchar *)"Level 88 must not have OCCURS",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {192, (uchar *)"Level 88 must have VALUE",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {193, (uchar *)"Level 88 VALUE range first not less than higher",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {194, (uchar *)"First in value range is greater than second",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {195, (uchar *)"NEXT SENTENCE not allowed in IF/END-IF",
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 
  {196, (uchar *)"Impermissible comparison",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {197, (uchar *)"Object of abbreviated relational comaprison out of place",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {198, (uchar *)"'All' number not allowed in arithmetic expression",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {199, (uchar *)"'All' on zero length non numeric literal not allowed",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {200, (uchar *)"EXTERNAL only allowed in WORKING-STORAGE",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {201, (uchar *)"EXTERNAL only allowed for 01 level items",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {202, (uchar *)"END name does not match PROGRAM-ID name",
   0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
  {203, (uchar *)"EXTERNAL not allowed for redefine items.",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {204, (uchar *)"External item may not have initial value",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {205, (uchar *)"EXIT PROGRAM not last in sequence of statements",
   0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {206, (uchar *)"EXIT PROGRAM in main program is ignored",
   0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {207, (uchar *)"External reference type not C-REFERENCE or C-DEFINITION, ignored",
   0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
 
   {0, (uchar *)"Past End of Error Messages List", 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


/* Get the details (FILE_NBR, LINE_NBR, CHAR_NBR) for a token
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
  struct bli_token_struct *token;

  if (!void_star_token) /* Empty file scenario.  */
    {
      * file_nbr = 1;
      * line_nbr = 1;
      * char_nbr = 1;
      * token_text = (uchar *)"";
      return;
    }
  token = void_star_token;

  * file_nbr = token->bli_token_fileno;
  * line_nbr = token->bli_token_lineno;
  * char_nbr = token->bli_token_charno;
  * token_text = BLI_ALLOC (token->string_details->length +1);
  memcpy ( * token_text, token->string_details->string, token->string_details->length);
}

/* Get the owning file and line number (FILE_NBR, LINE_NBR) of a file
   and line number ie the file and line number if any it was copied
   from.  In the main compiler this is a dummy routine, as we fill in
   the hierarchy from the #line records right away.  */

void
get_owning_details (file_nbr, line_nbr)
     uint32 *file_nbr;
     uint32 *line_nbr;
{
  * file_nbr = 0;
  * line_nbr = 0;
  return;
}

/* Output an error message to the effect that a feature that occurred
   at token TOKEN is not supported; the text of the message can be
   overridden by TEXT.  */

void
rsn_error (struct bli_token_struct *token, const uchar *text)
{
  BLI_ASSERT (token->pfx.cat == type_token);
  MSG_TOKEN (MSGID (127), token, (uchar *)text);
}

