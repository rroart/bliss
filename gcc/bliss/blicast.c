/* 

   BLISS Compiler abstract sytax tree manipulation (blicast.c).  The
   parser creates a parse tree (abstract syntax tree) but does minimum
   processing with it (1).  This routine does any such processing eg
   validation of cross references, merging of concatenated strings
   ("a" & "b") and BLISS specific optimisation (if any).

   Notes 

   1.  OK the parser does call a routine here to merge concatenated
   strings, but that's because it can be done in one place in the
   parse, but would have to be put in 100 different places here.

   2.  LLL;  this is a very partial implementation !!!

   --------------------------------------------------------

   Copyright (C) 1999, 2000, 2001 Free Software FOundation, Inc.

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
   what you give them.  Help stamp out software-hoarding!  
   
   --------------------------------------------------------.  */

#include <stdlib.h>
#include <unistd.h>
#include "safe-ctype.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <limits.h>

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "ggc.h"
#include "flags.h"
#include "toplev.h"

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

enum numeric_literal_errors
{
  numeric_too_long = 1,
  numeric_too_large_value = 2,
  numeric_invalid_sign = 3
};

#define ABBREVIATED_EQUAL_sym 1
#define ABBREVIATED_GREATER_EQUAL_sym 1
#define ABBREVIATED_GREATER_THAN_sym 1
#define ABBREVIATED_INVERTED_EQUAL_sym 1
#define ABBREVIATED_INVERTED_GREATER_EQUAL_sym 1
#define ABBREVIATED_INVERTED_GREATER_THAN_sym 1
#define ABBREVIATED_INVERTED_LESS_EQUAL_sym 1
#define ABBREVIATED_INVERTED_LESS_THAN_sym 1
#define ABBREVIATED_INVERTED_NOT_EQUAL_sym 1
#define ABBREVIATED_LESS_EQUAL_sym 1
#define ABBREVIATED_LESS_THAN_sym 1
#define ABBREVIATED_NOT_EQUAL_sym 1
#define ABBREVIATED_UNKNOWN_sym 1
#define ADDRESS_kw  1
#define AFTER 1
#define ALL  1
#define AND_kw 1
#define ARRAY_REFERENCE 1
#define ASTERISK_ASTERISK_sym  1
#define BEFORE 1
#define BINARY 1
#define BINARY_CHAR 1
#define BINARY_CHAR  1
#define BINARY_DOUBLE 1
#define BINARY_DOUBLE  1
#define BINARY_LONG 1
#define BINARY_LONG  1
#define BINARY_LONG_DOUBLE 1
#define BINARY_LONG_LONG_DOUBLE 1
#define BINARY_SHORT 1
#define BINARY_SHORT  1
#define BLI_PROD_TYPE_DATA_ITEM 1
#define BLI_PROD_TYPE_DATA_ITEM  1
#define BLI_PROD_TYPE_EXPRESSION 1
#define BLI_PROD_TYPE_EXPRESSION  1
#define BLI_PROD_TYPE_LABELS 1
#define BLI_PROD_TYPE_PARAGRAPH 1
#define BLI_PROD_TYPE_PARAGRAPH  1
#define BLI_PROD_TYPE_SECTION 1
#define BLI_PROD_TYPE_SECTION  1
#define BLI_PROD_TYPE_VERB_CALL 1
#define BLI_PROD_TYPE_VERB_COMPUTE 1
#define BLI_PROD_TYPE_VERB_CONTINUE 1
#define BLI_PROD_TYPE_VERB_DISPLAY 1
#define BLI_PROD_TYPE_VERB_EXIT 1
#define BLI_PROD_TYPE_VERB_FULLSTOP 1
#define BLI_PROD_TYPE_VERB_GOTO 1
#define BLI_PROD_TYPE_VERB_IF 1
#define BLI_PROD_TYPE_VERB_MOVE 1
#define BLI_PROD_TYPE_VERB_NEXT_SENTENCE 1
#define BLI_PROD_TYPE_VERB_PERFORM 1
#define BLI_PROD_TYPE_VERB_SET 1
#define BLI_PROD_TYPE_VERB_STOP 1
#define DISPLAY 1
#define DISPLAY  1
#define EQUALS_sym 1
#define GENERIC_NAME  1
#define GENERIC_NAME_magic_condition_name  1
#define GENERIC_NAME_magic_condition_name_that_can_have_array  1
#define GENERIC_NAME_magic_that_can_have_array  1
#define GENERIC_NUMBER_INTEGER 1
#define GENERIC_NUMBER_INTEGER  1
#define GENERIC_NUMBER_NON_INTEGER 1
#define GENERIC_NUMBER_NON_INTEGER  1
#define GENERIC_STRING 1
#define GENERIC_STRING  1
#define GREATER_EQUALS_sym 1
#define GREATER_THAN_sym 1
#define HIGH_VALUE  1
#define HIGH_VALUES  1
#define LESS_EQUALS_sym 1
#define LESS_THAN_sym 1
#define LEVEL_88 1
#define LINKAGE  1
#define LOCAL_STORAGE  1
#define LOW_VALUE  1
#define LOW_VALUES  1
#define MINUS_sym  1
#define NOT_EQUALS_sym 1
#define NOT_kw 1
#define NOT_magic_for_abbreviated_relation_operator 1
#define NULL_word 1
#define NULL_word  1
#define OR 1
#define PERFORM 1
#define PLUS_sym  1
#define POINTER 1
#define QUOTE  1
#define QUOTES  1
#define SLASH_sym  1
#define SPACE  1
#define SPACES  1
#define TIMES 1
#define UNTIL 1
#define VARYING 1
#define WORKING_STORAGE  1
#define ZERO  1
#define ZEROES  1
#define ZEROS  1

/* First in linked list of next sentence structs pending the fullstop.  */
static struct bli_tree_struct_verb_next_sentence *pending_fullstops = NULL;

/* Current procedure division section.  */
static struct bli_tree_struct_label *current_section = NULL;

/* Previous verb type.  */
static uint32 prev_type = 0;

/* Active procedure division.  */
static struct bli_tree_struct_procedure_division *global_current_procedure_division;

/* Names for each fixed token type indexed by number.  */
extern uchar **token_lit_ptrs;

/* Current pgm is main.  */

extern unsigned current_is_main;

/* Internal prototypes.  */

static void explore_expression (void **expression);
static void replace_token_by_data_item (void **token_ptr_ptr);
static void handle_numeric_error (uint32 error_code, struct bli_token_struct *token,
                                         uint32 toobigmsg, uint32 badsignmsg);
static void process_storage (struct bli_tree_struct_storage *current_storage);
static void check_not_empty_group (struct bli_tree_struct_data_item *data_item);
static uint32 data_item_integral (struct bli_tree_struct_data_item *item);
static void restructure_redefines (struct bli_tree_struct_data_item *data_item);
static void process_occurs (struct bli_tree_struct_data_division *current_data_division);
static void scan_occurs_dependents (struct bli_tree_struct_data_item *this_data_item);
static void check_pointer_type (struct bli_tree_struct_expression *exp, uint32 writeable);
static void validate_labels (struct bli_tree_struct_procedure_division *current_procedure_division);
static uint32 ensure_not_signed (struct bli_token_struct *tok);
static void *find_label (struct bli_tree_struct_procedure_ref *, 
                        struct bli_tree_struct_label *current_section);
static int check_integral_type (struct bli_tree_struct_expression *exp);
static void match_corresponding (void **list_header, void **last,
                                struct bli_tree_struct_data_item *move_from, 
                                struct bli_tree_struct_data_item *original_move_from, 
                                struct bli_tree_struct_data_item *move_to);
enum message_choice_enum
{
  DO_OUTPUT_MSG = 1,
  DONT_OUTPUT_MSG = 0
};
static uint32 check_move_compatible (void *from, void *to, enum message_choice_enum msg);
static void process_level_88 (struct bli_tree_struct_data_item *cdi, 
                             struct bli_tree_struct_data_item *bdi, 
                             struct bli_tree_struct_storage *cs);
static void process_value (struct bli_tree_struct_value_item *val,
                          struct bli_tree_struct_data_item *cdi, 
                          struct bli_tree_struct_data_item *bdi, 
                          struct bli_tree_struct_storage *cs);
static void process_basic_value (struct bli_tree_struct_value_item *val,
                                struct bli_tree_struct_data_item *cdi, 
                                struct bli_tree_struct_data_item *bdi, 
                                struct bli_tree_struct_storage *cs);
static void explore_if (void *current_verb);
static void explore_perform (void *current_verb);
static void explore_call (void *current_verb);
static void explore_condition (void **condition, 
                               uint32 *prev_relational_operator,
                               void **prev_relational_operand);
static void explore_non_atomic_condition (void **condition, 
                               uint32 *prev_relational_operator,
                               void **prev_relational_operand);
enum top_level_enum
{
  not_top_level = 0,
  top_level = 1
};
static void explore_statement (void *current_verb, enum top_level_enum top_level_flag);
static void check_comparable (void* operand1, uint32 operator, void* operand2);
static void mark_save (void * item);
static struct bli_tree_struct_data_item * get_data_item (void *exp);
static struct bli_tree_struct_data_item * get_literal (void *exp);

/* Validate the data division CURRENT_DATA_DIVISION and insert all the
   symbols into the symbol table.  */

static void
explore_call (void *passed_call_verb)
{
  struct bli_tree_struct_verb_call *call_verb;
  struct bli_token_struct *callee;

  call_verb = (struct bli_tree_struct_verb_call *)passed_call_verb;

  BLI_ASSERT (!call_verb->using_list);

  callee = (struct bli_token_struct *) call_verb->what_to_call;
  BLI_ASSERT (callee);

}

/* Put a list of corresponding items in the structures MOVE_FROM and
   MOVE_TO into LIST_HEADER/LAST.  ORIGINAL_MOVE_FROM is the original
   move_from item which is needed to build the search trees.  The list
   entries are struct bli_tree_struct_double_entry_list.  */

static void
match_corresponding (void **list_header, void **last, struct bli_tree_struct_data_item *move_from, 
                    struct bli_tree_struct_data_item *original_move_from, 
                    struct bli_tree_struct_data_item *move_to)
{
    uint32 count;
    uint32 number_names;
    uchar *names[LEVELS_ARRAY_SIZE];
    uint32 name_lengths[LEVELS_ARRAY_SIZE];
    struct bli_tree_struct_data_item *scan_ptr;
    struct bli_tree_struct_data_item *found_item;
    void *found_entry;
    struct bli_token_struct *name_tok;
        
    /* Check the current from item.  Ignore if ambiguous, table,
       redefine.  An ambiguous group item may result in a
       non-ambiguous name as a totality so do not discard just because
       ambiguous if it is a group.  */
    
    if (move_from->duplicate_flag)
      if (!move_from->group_flag)
        return;

  if (move_from->occurs_details)
    return;

  /* Only process the first which is the actual main item redefined by
     others.  */
  if (move_from->redefine_master_flag)
    {
      match_corresponding (list_header, last,
                          (struct bli_tree_struct_data_item *)move_from->branch.child, 
                          original_move_from,
                          move_to);
      return;
    }

  /* Recurse to subordinates.  */
  if (move_from->group_flag)
    {
      struct bli_tree_struct_data_item *child_ptr;
      for (child_ptr = (struct bli_tree_struct_data_item *)move_from->branch.child;
           child_ptr;
           child_ptr = (struct bli_tree_struct_data_item *)child_ptr->branch.next)
        match_corresponding (list_header, last, child_ptr, original_move_from, move_to);
      return;
    }
  
  /* We have a candidate.  Look for a match in the move_to hierarchy
     and if found, add it to the list.  */
  {
    /* First check it is not ambiguous in itself.  */
    for (number_names = 0, scan_ptr = move_from;
         scan_ptr;
         scan_ptr = (void *) scan_ptr->owner)
      {
        if (scan_ptr->redefine_master_flag)
          continue; /* Because this is a dummy level only.  */
        if (scan_ptr->filler_flag)
          {
            name_lengths[number_names] = 0;
            names[number_names] = NULL;
            number_names ++;
            continue;
          }
        name_tok = scan_ptr->data_name_token;
        name_lengths[number_names] = name_tok->string_details->length;
        names[number_names] = name_tok->string_details->string_upper->string;
        number_names ++;
      }
    count = find_symtab_entry_hierarchy (number_names, names, (uint32 *) & name_lengths[0],
                                       & found_entry, symtab_entry_type_data);
    BLI_ASSERT (count > 0); /* Should be found!  */
    if (count > 1) 
      return; /* Ambiguous name in context.  */
    
    for (number_names = 0, scan_ptr = move_from;
         scan_ptr;
         scan_ptr = (void *) scan_ptr->owner)
      {
        if (scan_ptr->redefine_master_flag)
          continue; /* Because this is a dummy level only.  */
        if (scan_ptr->filler_flag)
          {
            name_lengths[number_names] = 0;
            names[number_names] = NULL;
            number_names ++;
            continue;
          }
        if (scan_ptr == original_move_from)
          scan_ptr = move_to;
        name_tok = scan_ptr->data_name_token;
        name_lengths[number_names] = name_tok->string_details->length;
        names[number_names] = name_tok->string_details->string_upper->string;
        number_names ++;
      }
    count = find_symtab_entry_hierarchy (number_names, names, (uint32 *) & name_lengths[0],
                                       & found_entry, symtab_entry_type_data);
    /* If found and not ambiguous, allocate the list item and link it
       all in.  */
    if (count == 1)
      {
        struct bli_tree_struct_double_entry_list *new_list_item;
        found_item = get_symtab_entry_item (found_entry);
        BLI_ASSERT (found_item);
        new_list_item = BLI_ALLOC (sizeof (struct bli_tree_struct_double_entry_list));
        new_list_item->branch.child = (void *)move_from;
        new_list_item->child2 = (void *)found_item;
        if ( * list_header)
          ((struct bli_tree_struct_double_entry_list *)*last)->branch.next = 
            (void *)new_list_item;
        else
          * list_header = new_list_item;
        * last = new_list_item;
      }
    }
}

/* Check that a move from FROM to TO is allowable and issue any
   warnings if MSG == DO_OUTPUT_MSG (else  == DONT_OUTPUT_MSG).  Return 1
   if compatible else 0.  */

