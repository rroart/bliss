  /* 
     BLISS Compiler parser utilities (blicpru.c) 

     Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
     
     -----------------------------------------------------------------------------
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License as
     published by the Free Software Foundation; either version 2, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
     USA.

     In other words, you are welcome to use, share and improve this
     program.  You are forbidden to forbid anyone else to use, share
     and improve what you give them.  Help stamp out
     software-hoarding!

     -----------------------------------------------------------------------------.  */

#include "blir_temp_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "safe-ctype.h"

/* Avoid having to pull in the whole GCC tree thing, which we regard
   as a black box in this module.  */
typedef void *tree;

#include "ansidecl.h"

#define GTY(a) 
#include "blidebug.h"
#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliclex.h"
#include "blicc1.h"
#include "blicsymn.h"
#include "blicsyt.h"
#include "blicpru.h"

extern int32 option_debug;

/* Local prototypes.  */

static void update_class_condition_prefix (struct bli_token_struct *tk);
static void update_sign_condition_prefix (struct bli_token_struct *tk);
static void update_size_prefix (struct bli_token_struct *tk);
static void update_overflow_prefix (struct bli_token_struct *tk);
static void update_for_prefix (struct bli_token_struct *tk);
static enum symtab_entry_type_enum get_name_symtab_type (struct bli_token_struct *tk);
static void update_for_relational (struct bli_token_struct *tk);

/* Set up the magic tokens.  The parser blictok.def has the details of
   why they are all needed (find 'problem'), when they are created
   etc.
   
   All these are in the procedure division. Parsing the other
   divisions is a piece of cake.  We re-scan the procedure division,
   when we hit it in the parse.
  
  START is the token to start from.  */

#if 0
void
set_up_magic_tokens (struct bli_token_struct *start)
{

  struct bli_token_struct *tk;
  struct bli_token_struct *tok_new;
  struct bli_token_struct *tok_scan;
  struct bli_token_struct *tok_function_scan;
  struct bli_token_struct *tok_refmod_scan;
  enum symtab_entry_type_enum symtab_type;
  void *symtab_entry;
  int32 refmod_level;
  int32 current_verb = 0;
  uint32 get_name_status;

  for (tk = start; tk; tk = (tk ? tk->bli_token_next : NULL))
    {
      BLI_ASSERT (tk->bli_token_prev);

#if 0
      /* This is not the solution.  Fix up later.  */
      if (!option_debug && tk->bli_token_debug)
        continue;  /* Debug only if in debug mode.  */
#endif

      switch (tk->pfx.type.tok_type)
	{


	case FULLSTOP_sym:
	  current_verb = 0;
	  break;

	case FOR:
	  if (current_verb == INSPECT)
            update_for_prefix (tk);
	  break;

	case END:
	  tok_scan = tk->bli_token_next;
	  if (!tok_scan)
	    break;
	  if ((tok_scan->pfx.type.tok_type == PROGRAM) ||
	      (tok_scan->pfx.type.tok_type == FUNCTION))
            tk = NULL;
	  break;

	case FUNCTION:
	  tok_function_scan = tk->bli_token_next;
	  if (!tok_function_scan)
	    break;
	  switch (tok_function_scan->pfx.type.tok_type)
	    {
	    case LENGTH:
	    case RANDOM:
	    case SIZE:
	    case SUM:
	    case GENERIC_NAME:
              /* Otherwise the GENERIC_NAME below messes it up.  */
	      tok_function_scan->pfx.type.tok_type = FUNCTION_NAME; 
	      symtab_entry =
		find_symtab_entry (tok_function_scan->string_details->string_upper->string,
				   tok_function_scan->string_details->length);
	      if (symtab_entry)
		{
		  symtab_type = get_symtab_entry_type (symtab_entry);
		}
	      while (symtab_entry
		     && symtab_type != symtab_entry_type_function_name)
		{
		  symtab_entry =
		    find_next_symtab_entry (symtab_entry,
					    tok_function_scan->
					    string_details->string_upper->string,
					    tok_function_scan->
					    string_details->length);
		  if (symtab_entry)
		    {
		      symtab_type = get_symtab_entry_type (symtab_entry);
		    }
		}
	      if (symtab_entry)
		{
		  if (symtab_type == symtab_entry_type_function_name)
		    {
		      if (get_symtab_entry_attribute_flags
			  (symtab_entry, (uint32)SYMBOL_TABLE_FLAG_OCCURS))
			{
			  tok_function_scan->pfx.type.tok_type = 
			    GENERIC_NAME_magic_that_can_have_arguments;
			}
		    }
		}
	      else
		{
                  /* Assume it can have arguments if not found.  */
		  tok_function_scan->pfx.type.tok_type = GENERIC_NAME_magic_that_can_have_arguments; 
		}
	      break;
	    default:
	      break;
	    }
	  break;

	case PROGRAM:
	  if (tk->bli_token_prev && (tk->bli_token_prev->pfx.type.tok_type == END))
	    {
	      if (!tk->bli_token_prev->bli_token_prev)
                return;
	      if (tk->bli_token_prev->bli_token_prev->pfx.type.tok_type == FULLSTOP_sym)
                return;
	    }
	  break;

	case COLON_sym:
	  /* Scan back for left parenthesis and fix it.  */
	  refmod_level = 1;
	  for (tok_refmod_scan = tk->bli_token_prev; tok_refmod_scan;
	       tok_refmod_scan =
	       tok_refmod_scan ? tok_refmod_scan->bli_token_prev : NULL)
	    {
	      switch (tok_refmod_scan->pfx.type.tok_type)
		{
		case FULLSTOP_sym:
		  tok_refmod_scan = NULL;
		  break;
		case LEFT_PARENTHESIS_sym:
		  if (refmod_level == 1)
		    {
		      tok_refmod_scan->pfx.type.tok_type = 
			LEFT_PARENTHESIS_magic_that_starts_refmod;
		      tok_refmod_scan = NULL;
		    }
		  refmod_level --;
		  break;
		case RIGHT_PARENTHESIS_sym:
		  refmod_level ++;
		  break;
		default:
		  break;
		}
	    }
	  break;

	case SIZE:
	  update_size_prefix (tk);
	  break;

	case OVERFLOW:
	  update_overflow_prefix (tk);
	  break;

	case NUMERIC:
	case ALPHABETIC:
	case ALPHABETIC_LOWER:
	case ALPHABETIC_UPPER:
	  update_class_condition_prefix (tk);
	  break;

	case POSITIVE:
	case NEGATIVE:
	case ZERO:
	  update_sign_condition_prefix (tk);
	  break;

	case GENERIC_NAME:
          {
            uint32 of_interest = 1;
            BLI_ASSERT (tk->bli_token_prev);
            /* We are looking for the first in an a of/in b etc
               sequence.  Ignore if OF name or OR name, unless ADDRESS
               OF name.  */
            if (tk->bli_token_prev->pfx.type.tok_type == IN)
              of_interest = 0;
            else
              if (tk->bli_token_prev->pfx.type.tok_type == OF)
                {
                  if (tk->bli_token_prev->bli_token_prev)
                    {
                      if (tk->bli_token_prev->bli_token_prev->pfx.type.tok_type == ADDRESS_kw)
                        of_interest = 1;
                      else
                        of_interest = 0;
                    }
                  else
                    of_interest = 0;
                }
            
            if (of_interest)
              {
                /* First of identifier.  */
                symtab_entry = NULL;
                symtab_entry = get_name_symtab_entry (tk,  & get_name_status, 0);
                if (symtab_entry)
                  {
                    symtab_type = get_symtab_entry_type (symtab_entry);
                    switch (symtab_type)
                      {
                      case symtab_entry_type_condition:
                        if (get_symtab_entry_attribute_flags
                            (symtab_entry, (uint32)SYMBOL_TABLE_FLAG_OCCURS))
                          tk->pfx.type.tok_type = GENERIC_NAME_magic_condition_name_that_can_have_array;
                        else
                          tk->pfx.type.tok_type = GENERIC_NAME_magic_condition_name;
                        break;
                      case symtab_entry_type_data:
                        if (get_symtab_entry_attribute_flags
                            (symtab_entry, (uint32)SYMBOL_TABLE_FLAG_OCCURS))
                        tk->pfx.type.tok_type = GENERIC_NAME_magic_that_can_have_array;
                        break;
                      case symtab_entry_type_switch_name:
                        tk->pfx.type.tok_type = GENERIC_NAME_magic_switch_name;
                        break;
                      case symtab_entry_type_symbolic_character_name:
                        tk->pfx.type.tok_type = GENERIC_NAME_magic_symbolic_character_name;
                        break;
                      case symtab_entry_type_class_condition:
                        update_class_condition_prefix (tk);
                        break;
                      default:
                        break;
                      }
                  }
                if ((current_verb == IF) || (current_verb == PERFORM))
                  {
                    update_for_relational (tk);
                  }
              }
          }
	  break;
          
	case PERFORM:
	  tok_scan = tk->bli_token_next;

          /* If this perform is followed by an unknown name then we
             have an out of line perform because we assume it muct be
             a procedure name.  At this stage paragraph and section
             labels are not in the symbol table.  */
          if (token_is_generic_name (tok_scan->pfx.type.tok_type))
            {
              if (get_name_symtab_type (tok_scan))
                tk->pfx.type.tok_type =
                  PERFORM_magic_requires_end_perform;
            }
          else
            switch (tok_scan->pfx.type.tok_type)
              {
                /* A number can be PERFORM n TIMES => inline, or
                   PERFORM n ... => paragraph name.  */
              case GENERIC_NUMBER_INTEGER:
                tok_scan = tok_scan->bli_token_next;
                if (tok_scan && (tok_scan->pfx.type.tok_type == TIMES))
                  tk->pfx.type.tok_type = PERFORM_magic_requires_end_perform;
                /* Otherwise it is a numeric procedure name; old style perform.  */
                break;
                
                /* This cannot be a paragraph name so we assume an
                   inine perform.  */
              default:
                tk->pfx.type.tok_type = PERFORM_magic_requires_end_perform;
                break;
              }
          
          
	  /* Drop through.  */

	default:
          if (token_is_verb (tk->pfx.type.tok_type))
            {
              current_verb = tk->pfx.type.tok_type;  /* Just to reduce the risk of hacks in the wrong places.  */
              /* Put the end of statement token in here.  */
              tok_new = BLI_ALLOC (sizeof (struct bli_token_struct));
              memcpy (tok_new, tk, sizeof (struct bli_token_struct));
              tk->bli_token_prev = tok_new;
              tok_new->bli_token_next = tk;
              BLI_ASSERT (tok_new->bli_token_prev);
              tok_new->bli_token_prev->bli_token_next = tok_new;
              tok_new->pfx.type.tok_type = START_OF_STATEMENT_magic;
              BLI_ASSERT (tok_new->bli_token_prev);
              BLI_ASSERT (tk->bli_token_prev);
            }
	  break;

	}
    }

}
#endif

/* Return 1 if this TOK_TYPE is for a verb, else zero.  */
uint32
token_is_verb (uint32 tok_type)
{
  switch (tok_type)
    {
    case ADD:
    case CALL_kw:
    case COMPUTE:
    case DELETE:
    case DIVIDE:
    case EVALUATE:
    case IF:
    case MULTIPLY:
    case READ:
    case RECEIVE:
    case RETURN_kw:
    case REWRITE:
    case SEARCH:
    case START:
    case STRING:
    case SUBTRACT:
    case UNSTRING:
    case WRITE:
      
    case ACCEPT:
    case ALTER:
    case CANCEL:
    case CLOSE:
    case CONTINUE:
    case DISABLE:
    case DISPLAY:
    case ENABLE:
    case ENTER:
    case EXIT:
    case GENERATE:
    case GO:
    case INITIALIZE:
    case INITIATE:
    case INSPECT:
    case MERGE:
    case MOVE:
    case OPEN:
    case PURGE:
    case RELEASE:
    case SEND:
    case SET_kw:
    case SORT:
    case STOP:
    case SUPPRESS:
    case TERMINATE:
    case PERFORM:
      return 1;
      
    default:
      return 0;
    }
}

#if 0
/* Return 1 if this TOK_TYPE is for a generic name, else zero.  */
uint32
token_is_generic_name (uint32 tok_type)
{
  switch (tok_type)
    {
    case GENERIC_NAME:
    case GENERIC_NAME_magic_that_can_have_array:
    case GENERIC_NAME_magic_that_can_have_arguments:
    case GENERIC_NAME_magic_condition_name:
    case GENERIC_NAME_magic_condition_name_that_can_have_array:
    case GENERIC_NAME_magic_switch_name:
    case GENERIC_NAME_magic_symbolic_character_name:
    case GENERIC_NAME_magic_has_for_following_in_inspect:
      return 1;
      
    default:
      return 0;
    }
}
#endif

/* Place the intrinsic functions into the symbol table.  */

void
init_intrinsic_functions (void)
{
#if 0
  void *symtab_entry;
  uint32 i;

  struct function_def
  {
    const uchar *name;
    int32 has_args;
  };

  struct function_def function_names[] = {

    {(uchar *)"ACOS", 1}, 
    {(uchar *)"ANNUITY", 1}, 
    {(uchar *)"ASIN", 1}, 
    {(uchar *)"ATAN", 1}, 
    {(uchar *)"BLI_CHAR", 1},
    {(uchar *)"COS", 1},
    {(uchar *)"CURRENT-DATE", 0}, 
    {(uchar *)"DATE-OF-BLI_INTEGER", 1}, 
    {(uchar *)"DAY-OF-BLI_INTEGER", 1},
    {(uchar *)"FACTORIAL", 1},
    {(uchar *)"BLI_INTEGER", 1}, 
    {(uchar *)"BLI_INTEGER-OF-DATE", 1}, 
    {(uchar *)"BLI_INTEGER-OF-DAY", 1},
    {(uchar *)"BLI_INTEGER-PART", 1},
    {(uchar *)"LENGTH", 1}, 
    {(uchar *)"LOG", 1}, 
    {(uchar *)"LOG10", 1}, 
    {(uchar *)"LOWER-CASE", 1}, 
    {(uchar *)"MAX", 1},
    {(uchar *)"MEAN", 1}, 
    {(uchar *)"MEDIAN", 1}, 
    {(uchar *)"MIDRANGE", 1}, 
    {(uchar *)"MIN", 1}, 
    {(uchar *)"MOD", 1},
    {(uchar *)"NUMVAL", 1}, 
    {(uchar *)"NUMVAL-C", 1}, 
    {(uchar *)"ORD", 1}, 
    {(uchar *)"ORD-MAX", 1}, 
    {(uchar *)"ORD-MIN", 1},
    {(uchar *)"PRESENT-VALUE", 1}, 
    {(uchar *)"RANDOM", 1}, 
    {(uchar *)"RANGE", 1}, 
    {(uchar *)"REM", 1},
    {(uchar *)"REVERSE", 1},
    {(uchar *)"SIN", 1}, 
    {(uchar *)"SQRT", 1}, 
    {(uchar *)"STANDARD-DEVIATION", 1}, 
    {(uchar *)"SUM", 1}, 
    {(uchar *)"TAN", 1},
    {(uchar *)"UPPER-CASE", 1}, 
    {(uchar *)"VARIANCE", 1}, 
    {(uchar *)"WHEN-COMPILED", 0},
  };
#endif

  BLI_ASSERT (0 && "not ready for intrinsic functions yet");

#if 0
  
  /* Lll; intrinsic functions maybe these shouldn't be in the main symbol table ???  */
  
  for (i = 0; i < ((sizeof (function_names)) / (sizeof (function_names[0]))); i ++)
    {
      symtab_entry =
	create_symtab_entry (symtab_entry_type_function_name, NULL,
			     (uchar *)function_names[i].name,
                            strlen ((char *)function_names[i].name));
      put_symtab_entry_attribute_flags
        (symtab_entry,
         (uint32)((function_names[i].has_args ? SYMBOL_TABLE_FLAG_OCCURS : 0) | SYMBOL_TABLE_FLAG_GLOBAL),
         (uint32)(SYMBOL_TABLE_FLAG_OCCURS | SYMBOL_TABLE_FLAG_GLOBAL));
    }
#endif
  return;
  
}

/* Fix preceding is/not before token TK which is a class condition to
   reflect the fact that a class condition is coming.  */

#if 0
static void
update_class_condition_prefix (struct bli_token_struct *tk)
{

  while ((tk = tk->bli_token_prev))
    {
      if (tk->pfx.type.tok_type == IS)
	{
	  tk->pfx.type.tok_type = IS_magic_class_condition_follows;
	  continue;
	}
      if (tk->pfx.type.tok_type == NOT_kw)
	{
	  tk->pfx.type.tok_type = NOT_magic_class_condition_follows;
	  continue;
	}
      break;
    }

}
#endif

/* Fix preceding is/not before token TK which is a sign condition to
   reflect the fact that a sign condition is coming.  */

#if 0
static void
update_sign_condition_prefix (struct bli_token_struct *tk)
{

  while ((tk = tk->bli_token_prev))
    {
      if (tk->pfx.type.tok_type == IS)
	{
	  tk->pfx.type.tok_type = IS_magic_sign_condition_follows;
	  continue;
	}
      if (tk->pfx.type.tok_type == NOT_kw)
	{
	  tk->pfx.type.tok_type = NOT_magic_sign_condition_follows;
	  continue;
	}
      break;
    }

}

/* Fix preceding is/not before token TK to reflect the fact that
   'size' is coming.  */

static void
update_size_prefix (struct bli_token_struct *tk)
{

  while ((tk = tk->bli_token_prev))
    {
      if (tk->pfx.type.tok_type == ON)
	{
	  tk->pfx.type.tok_type = ON_magic_size_follows;
	  continue;
	}
      if (tk->pfx.type.tok_type == NOT_kw)
	{
	  tk->pfx.type.tok_type = NOT_magic_size_follows;
	  continue;
	}
      break;
    }

}

/* Fix preceding is/not before token TK to reflect the fact that
   'overflow' is coming.  */

static void
update_overflow_prefix (struct bli_token_struct *tk)
{

  while ((tk = tk->bli_token_prev))
    {
      if (tk->pfx.type.tok_type == ON)
	{
	  tk->pfx.type.tok_type = on_magic_has_overflow;
	  continue;
	}
      if (tk->pfx.type.tok_type == NOT_kw)
	{
	  tk->pfx.type.tok_type = NOT_magic_has_overflow;
	  continue;
	}
      break;
    }

}


/* 
   Fix the TOKEN NOT, which is within an IF or PERFORM.  Maybe convert
   it to special magic token as per problem 12 in blictok.def.
   
   "IF a > b and NOT c ...".  */

static void
update_for_relational (struct bli_token_struct *tk)
{

  struct bli_token_struct *scan;
  struct bli_token_struct *scan_prev;
  uint32 found;

  scan_prev = tk->bli_token_prev;
  BLI_ASSERT (scan_prev);
  if (scan_prev->pfx.type.tok_type != NOT_kw)
    return;

  switch (tk->pfx.type.tok_type)
    {
    case GENERIC_NAME:
    case GENERIC_NAME_magic_that_can_have_array:

      /*

        Scan forward to something other than OF/IN/GENERIC_NAME and
        anything in parentheses.  If the next thing is OR or AND or a
        verb or a full stop or some other form of natural end then
        this may be part of an abbreviated relational condition. So if
        the previous token is a NOT, set it to the magic value. (NOT
        test is above).

        You may think this is incomplete, but it is only the case
        where the NOT is just before the data name that causes a
        problem. All the others are not LALR(1) ambiguous so no need
        to meddle.  */

      for (scan = tk->bli_token_next, found = 0; scan && !found; 
           scan = scan ? scan->bli_token_next : NULL)
        {
          int32 parenthesis_level = 0;
          switch (scan->pfx.type.tok_type)
            {
            case GENERIC_NAME:
            case OF:
            case IN:
              continue;
              
            case LEFT_PARENTHESIS_sym:
            case LEFT_PARENTHESIS_magic_that_starts_refmod:
              parenthesis_level ++;
              continue;
              
            case RIGHT_PARENTHESIS_sym:
              parenthesis_level --;
              if (parenthesis_level > 0)
                continue;
              /* Drop through.  A stray right parenthesis here means we are done.  */
            case OR:
            case AND_kw:
            case FULLSTOP_sym:
            case NEXT:
            case AFTER:
            case START_OF_STATEMENT_magic:
                found = 1;
              continue;

            default:
              if (token_is_verb (scan->pfx.type.tok_type))
                found = 1;
              else
                if (parenthesis_level < 1)
                  scan = NULL;
              continue;
            }
          
        }
      if (found == 1)
        {
          BLI_ASSERT (scan_prev->pfx.type.tok_type == NOT_kw);
          scan_prev->pfx.type.tok_type = NOT_magic_for_abbreviated_relation_operator;
        }
      break;
      
    case GENERIC_NAME_magic_condition_name:
    case GENERIC_NAME_magic_condition_name_that_can_have_array:
    case GENERIC_NAME_magic_switch_name:
    case GENERIC_NAME_magic_symbolic_character_name:
      /* These are full conditions in themselves or are already
         handled and NOT does not need changing.  */
      return;
      
    default:
      abort ();
    }
      
}


/* Fix preceding identifier if followed by 'for'.  This is used to
   handle parse situations involving 'for identifier' in inspect
   verbs.  */

static void
update_for_prefix (struct bli_token_struct *tk)
{

  struct bli_token_struct *scan;
  struct bli_token_struct *scan2;

  for (scan = tk->bli_token_prev; scan;
       scan = scan ? scan->bli_token_prev : NULL)
    {
      switch (scan->pfx.type.tok_type)
	{
	case GENERIC_NAME:
	  scan2 = scan->bli_token_prev;
	  if ((!scan2)
	      || ((scan2->pfx.type.tok_type != OF)
		  && (scan2->pfx.type.tok_type != IN)))
	    {
	      scan->pfx.type.tok_type = 
		GENERIC_NAME_magic_has_for_following_in_inspect;
	    }
	  scan = scan->bli_token_prev;
	  break;

	case GENERIC_NAME_magic_that_can_have_array:
	  scan->pfx.type.tok_type = 
	    GENERIC_NAME_magic_has_for_following_in_inspect;
	  scan = NULL;
	  break;

	default:
	  scan = NULL;
	  break;
	}
    }

}
#endif

/* Get the type of a name (TK).  */

static enum symtab_entry_type_enum
get_name_symtab_type (struct bli_token_struct *tk)
{
  uint32 symtab_retcode;
  uchar *names[50];
  uint32 name_lengths[50];
  uint32 names_used;
  enum symtab_entry_type_enum symtab_type;
  void *symtab_entry;
  struct bli_token_struct *tok_name_part;

  names_used = 0;
  for (tok_name_part = tk, names_used = 0; names_used < 50; names_used ++)
    {
      struct bli_token_struct *next;
      struct bli_token_struct *next2;
      name_lengths[names_used] = tk->string_details->length;
      names[names_used] = tk->string_details->string_upper->string;
      next = tok_name_part->bli_token_next;
      if (next)
	{
	  if ((next->pfx.type.tok_type == OF) || (next->pfx.type.tok_type == IN))
	    {
	      next2 = next->bli_token_next;
	      if (next2)
		{
		  if (next2->pfx.type.tok_type == GENERIC_NAME)
		    {
		      tok_name_part = next2;
		      continue;
		    }
		}
	    }
	}
      break;
    }
  names_used ++;
  symtab_retcode =
    find_symtab_entry_hierarchy (names_used, names, name_lengths,
				 & symtab_entry, 0);
  if (symtab_retcode)
    symtab_type = get_symtab_entry_type (symtab_entry);
  else
    symtab_type = symtab_entry_type_not;

  /* Lll; there could be duplicates but here we have to go with one or the other.  */
  return symtab_type;
}

/* Get the symbol table entry of a name TK perhaps followed in linked list by 'of/in x'...  */

void *
get_name_symtab_entry (struct bli_token_struct *tk, uint32 *status, 
                       enum symtab_entry_type_enum type)
{

  uchar *names[51];
  uint32 name_lengths[51];
  uint32 names_used;
  void *symtab_entry;
  struct bli_token_struct *tok_name_part;

  for (tok_name_part = tk, names_used = 1;; names_used ++)
    {
      struct bli_token_struct *next;
      struct bli_token_struct *next2;
      if (names_used > 50) /* Too many of/in for anyone (50 allowed due to "of file/report").  */
        {
          MSG_TOKEN (MSGID (139), tk, NULL);
          break;
        }
      name_lengths[names_used -1] = tok_name_part->string_details->length;
      names[names_used -1] = tok_name_part->string_details->string_upper->string;
      next = tok_name_part->bli_token_next;
      if (next)
	{
	  if ((next->pfx.type.tok_type == OF) || (next->pfx.type.tok_type == IN))
	    {
	      next2 = next->bli_token_next;
	      if (next2)
		{
		  if (next2->pfx.type.tok_type == GENERIC_NAME)
		    {
		      tok_name_part = next2;
		      continue;
		    }
		}
	    }
	}
      break;
    }
  symtab_entry = NULL;
  * status = find_symtab_entry_hierarchy
    (names_used, names, name_lengths,  & symtab_entry, type);
  return symtab_entry;
}

/* Get the next symbol table entry of a name TK perhaps followed in linked list by 'of/in x'...  */

void *
get_next_name_symtab_entry (void *current, struct bli_token_struct *tk, uint32 *status, 
                            enum symtab_entry_type_enum type)
{

  uchar *names[51];
  uint32 name_lengths[51];
  uint32 names_used;
  void *symtab_entry;
  struct bli_token_struct *tok_name_part;

  for (tok_name_part = tk, names_used = 1;; names_used ++)
    {
      struct bli_token_struct *next;
      struct bli_token_struct *next2;
      if (names_used > 50) /* Too many of/in for anyone (50 allowed due to "of file/report").  */
        {
          MSG_TOKEN (MSGID (139), tk, NULL);
          break;
        }
      name_lengths[names_used -1] = tok_name_part->string_details->length;
      names[names_used -1] = tok_name_part->string_details->string_upper->string;
      next = tok_name_part->bli_token_next;
      if (next)
	{
	  if ((next->pfx.type.tok_type == OF) || (next->pfx.type.tok_type == IN))
	    {
	      next2 = next->bli_token_next;
	      if (next2)
		{
		  if (next2->pfx.type.tok_type == GENERIC_NAME)
		    {
		      tok_name_part = next2;
		      continue;
		    }
		}
	    }
	}
      break;
    }
  symtab_entry = current;
  * status = find_next_symtab_entry_hierarchy (names_used, names, name_lengths,  & symtab_entry, type);
  return symtab_entry;
}





