/*

   BLISS Compiler code generation back end interface (blicbei.c)
   Called by blicc1.c

   This code is complex and hard to follow.  I suggest you read "Using
   and Porting GCC" at the FSF web page and Joachim Nadler and Tim
   Josling's extra chapter "Interfacing to the GCC back end' at
   http://blissforgcc.sourceforge.net before venturing further.

   This should be the only module exposed to GCC's tree.h and its
   friends. All this module should do is interface to the tree.h
   code. That's enough for any one module to do, as you will see...

   The debugging code generation is now more or less OK.
   
   ---------------------------------------------------------------------

   Copyright (C) 1988, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
   2001, 2002 Free Software Foundation, Inc.

   Some of this is derived from the 'toy' front end by Richard Kenner
   Some of the changes (since replaced and therefore no longer
   present) to this were made by a person known only as 'Jon'. Other
   code comes from other parts of GCC, mainly c-decl.c and similar
   modules.
   
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
   
   ---------------------------------------------------------------------
   
   Assumption: garbage collection is never called implicitly. It will
   not be called 'at any time' when short of memory. It will only be
   called explicitly at the end of each program. This removes the need
   for a lot of bother to ensure everything is in the mark trees at
   all times.  */

#include "blir_temp_config.h"

/* Standard/OS headers.  */

#include <stdlib.h>
#include <unistd.h>
#include "safe-ctype.h"
#include <errno.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

#include <fcntl.h>
#include <getopt.h>

/* GCC headers.  */

#include "ansidecl.h"
#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "tree.h"
#include "flags.h"
#include "output.h"
#include <stdio.h>
/* #include "c-lex.h" */
#include "c-tree.h"
#include "rtl.h"
#include "tm_p.h"
#include "ggc.h"
#include "toplev.h"
#include "target.h"
#include "expr.h"
#include "convert.h"
#include "langhooks-def.h"
#include "langhooks.h"

#define MAX_FILL_GENERATE 200

/* BLI headers.  */

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
#include "blir_globals.h"

/* Tell bliumem.c that there is garbage collection.  */
native_int ggc_p = 1;

/* Things for gengtype.c to find.  */

/* Move All function trees.  */
tree moveall_fn_decl = NULL_TREE;
tree moveall_fn_type = NULL_TREE;

/* Convert alpha to numeric.  */
tree reverse_conversion_fn_decls[4] = {NULL, NULL, NULL, NULL};
tree reverse_conversion_fn_types[4] = {NULL, NULL, NULL, NULL};

/* Convert numeric to alpha.  */
tree conversion_fn_decls[4] = {NULL, NULL, NULL, NULL};
tree conversion_fn_types[4] = {NULL, NULL, NULL, NULL};

/* Compare with fill function trees.  */
tree cmpfill_fn_type = NULL_TREE;
tree cmpfill_fn_decl = NULL_TREE;

/* Compare all function trees.  */
tree cmpall_fn_type = NULL_TREE;
tree cmpall_fn_decl = NULL_TREE;

uchar * label_name_string = NULL;
uint32 label_name_string_length = 0;

/* Display function trees.  */
tree bli_display_fn_decl = NULL_TREE;
tree bli_display_fn_type = NULL_TREE;

/* Stop function trees.  */
tree bli_stop_fn_decl = NULL_TREE;
tree bli_stop_fn_type = NULL_TREE;

/* Exponentiation functions trees.  */
static tree expon_fndecls[16] = {NULL, NULL, NULL, NULL, 
                                 NULL, NULL, NULL, NULL, 
                                 NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL, NULL};
static tree expon_fntypes[16] = {NULL, NULL, NULL, NULL, 
                                 NULL, NULL, NULL, NULL, 
                                 NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL, NULL};

/* String for generating names for non-unique names.  */
uchar * nonunique_name = NULL;
uint32 nonunique_name_length = 0;

/* Work string.  */
uchar * work_string = NULL;
uint32 work_string_length = 0;

/* Work string.  */
uchar * fixed_id = NULL;
uint32 fixed_id_length = 0;

/* Trees for numbers 9 99 999 etc.  */
tree limits_trees_mod[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
uint32 limits_trees_sizes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

/* Local prototypes.  */

/*-----------------------.  */

struct exp_details;

static void output_program_code (struct bli_tree_struct_parse_tree_top *parse_tree_top, 
                         struct bli_tree_struct_program_top *current_program);
static void output_program_prolog (struct bli_tree_struct_parse_tree_top *parse_tree_top, 
                           struct bli_tree_struct_program_top *current_program);
static void output_program_epilog (struct bli_tree_struct_parse_tree_top *parse_tree_top, 
                                  struct bli_tree_struct_program_top *current_program);
static void call_void_function_no_args (const uchar *name, struct bli_token_struct *ref_token);
static void output_elementary_data_item (struct bli_tree_struct_data_item *current_data_item, 
                                    void **caller_link_header); 
static void output_group_item (struct bli_tree_struct_data_item *current_data_item, 
                                      void **caller_link_header); 
static tree get_fixed_hierarchy_identifier (struct bli_tree_struct_data_item *data_item, uchar *suffix);
static tree get_fixed_identifier (struct bli_token_struct *name_token, uchar *suffix,
                                 void *unique_address, uint32 dupflag);
static uchar *get_fixed_name (struct bli_token_struct *name_token, uchar *suffix, 
                                  uint32 unique_number, 
                                  uint32 dup_flag);
static uchar *get_fixed_string (uchar *string, uint32 length, uchar *suffix);
static void output_display_verb (struct bli_tree_cons *current_verb);

static void output_set_verb_address (struct bli_tree_cons *current_verb);
static void output_set_verb_condition (struct bli_tree_cons *current_verb);
static void output_stop_verb (struct bli_tree_cons *current_verb);
static tree type_for_fixed_length_type (int32 usage, int32 unsigned_flag);
static tree access_variable (struct bli_tree_struct_data_item *var, 
                            struct bli_tree_struct_expression *occurs,
                            tree *indexes, uint32 index_count);
static void check_redefines_length (struct bli_tree_struct_data_item *current_item);
static tree get_item_type (struct bli_tree_struct_data_item *current_data_item);
static uint32 get_uint32_value (tree t, uint32 shift);
static tree get_dynamic_length (struct bli_tree_struct_data_item *item, uint32 *fixed_length_ptr);
static tree build_int_binary_expression (uint32 op, tree op1, tree op2);
static void build_labels (struct bli_tree_struct_procedure_division *current_procedure_division);
static void build_end_of_sentence_labels 
(struct bli_tree_struct_procedure_division *current_procedure_division);
static void output_goto_verb (struct bli_tree_cons *current_verb);
static void output_if_verb (struct bli_tree_cons *current_verb);
static void output_compute_verb (struct bli_tree_cons *current_verb);
static void output_move_verb (struct bli_tree_cons *current_verb);
static void output_exit_verb (struct bli_tree_cons *current_verb);
static void output_move (struct exp_details *from, struct exp_details *to, struct bli_token_struct *ref_tok);
static tree build_int_binary_expression_exponentiation (tree op1, tree op2,
                                                       struct bli_token_struct *ref_token);
static void
build_conversion_numeric_to_alpha (struct exp_details *from,
                                  struct exp_details *to,
                                  struct bli_token_struct *ref_token);
static void
build_conversion_alpha_to_numeric (struct exp_details *from,
                                  struct exp_details *to,
                                  struct bli_token_struct *ref_token);
enum zero_enum
{
  zero_is_not_numeric = 0,
  zero_is_numeric = 1
};
static void get_exp_details (struct exp_details *ed, void *exp, tree *indexes, uint32 index_count,
                            enum zero_enum zero_numeric_p);
static uint32 get_enum_type (uint32 token_type, uint32 signedp);
static void procedure_init_part1 (struct bli_tree_struct_procedure_division *current_procedure_division);
static void procedure_init_part2 (struct bli_tree_struct_procedure_division *current_procedure_division);
static void one_deferred_init (struct bli_tree_struct_data_item *di);
static void link_deferred_init (struct bli_tree_struct_data_item *current_data_item);
static void check_value_size (struct bli_tree_struct_data_item *current_data_item);
static void check_level88_list (struct bli_tree_struct_data_item *current_data_item);
static tree truncate_to_digits (tree exp, uint32 digits);
static void check_compare_string_values (struct bli_tree_struct_data_item *current_data_item,
                                        struct bli_tree_struct_value_item *vi1,
                                        struct bli_tree_struct_value_item *vi2);
static void check_compare_numeric_values (struct bli_tree_struct_data_item *current_data_item,
                                         struct bli_tree_struct_value_item *vi1,
                                         struct bli_tree_struct_value_item *vi2);
static void output_verb (struct bli_tree_cons *current_verb);
static tree conditional_test (void *exp);
static tree basic_conditional_test (void *passed_exp);
static tree generate_level_88_test (void *passed_exp);
static tree generate_compare (uint32 operator, struct exp_details *left_ed,
                            struct exp_details *right_ed);
static void set_to_numeric_zero (struct exp_details* ed);
static uint32 swap_operator (uint32 operator);
static tree generate_character_compare (tree left, tree right, tree length);
static tree generate_character_compare_fill 
(tree address_exp, tree offset_exp, tree length_exp, 
 uint32 len,
 native_char char1);
static tree generate_all_compare (tree void_address1_exp,
                                  tree void_address2_exp,
                                  tree length1_exp,
                                  tree length2_exp);
static tree generate_dynamic_character_compare (tree left, tree right,
                                                tree length1, tree length2);

static tree convert_two_compares_to_result
(uint32 backend_operator1, uint32 backend_operator2,
 tree compare1_exp, tree compare2_exp);
static tree generate_level_88_single_test (struct exp_details *ed, struct bli_tree_struct_value_item *value);
static tree generate_level_88_comparison (uint32 test_type,
                                          struct exp_details *ed, 
                                          struct bli_tree_struct_value_item *value);
static void
output_next_sentence_verb (struct bli_tree_cons *current_verb);
static void
output_perform_verb (struct bli_tree_cons *current_verb, uint32 lineno, 
                     native_char *filename);
static void
output_call_verb (struct bli_tree_cons *current_verb);
static void
output_perform_times (struct bli_tree_struct_verb_perform * perform_verb,
                      uint32 lineno, native_char *filename);
static void
output_perform_until (struct bli_tree_struct_verb_perform * perform_verb,
                      uint32 lineno, native_char *filename);
static void
output_perform_varying (struct bli_tree_struct_verb_perform * perform_verb,
                        struct bli_tree_struct_item_list *clause_ptr,
                        uint32 lineno, native_char *filename);
static void
output_perform_transfers (struct bli_tree_struct_verb_perform * perform_verb, uint32 lineno, native_char *filename);
static tree
get_simple_array (tree provisional_item_type, uint32 min_occurs,
                  uint32 max_occurs,
                  tree identifier);
static tree output_memset (tree to, tree char1, tree length);
static void output_previous_procedures_exit (struct bli_tree_struct_label *new_label);
static void output_previous_procedure_exit (struct bli_tree_struct_label *procedure_to_exit);
static void expand_variable_goto (tree exp);
static tree bliss_sizeof (tree type);

/*-----------------------.  */

/* Details of an expression returned by get_exp_details.  */

struct exp_details
{
  void *source_exp;  /* The original expression in whatever form.  */
  enum blir_type enum_type;  /* Type - constant.  */
  uint32 token_type;  /* Type - token type for usage or type for the literal.  */
  struct bli_token_struct *token; /* The token if it's a literal else a reference token.  */
  uint32 signedp;  /* 1 If signed item.  */
  uint32 size;  /* Size in bytes (chars in literal if numeric constant).  */
  uint32 num_size; /* Size of number in bytes.  */
  tree length;  /* Tree expression for length.  */
  tree address;  /* Tree expression for address of the expression if addressable.  */
  tree exp;  /* Tree expression for the expression.  */
  struct bli_tree_struct_data_item *di;  /* The data item if data item
                                           or array reference.  */
  /* The data item if data item or array reference if level 88 item.  */
  struct bli_tree_struct_data_item *level_88_di; 
  uchar first_char; /* First character for symbolic constant.  */
  unsigned all_flag;  /* 1 If all ...  */
  unsigned constant_flag;  /* 1 If constant.  */
};

/* Details of item to be inited in procedure division.  */

struct deferred_init_details
{
  struct deferred_init_details *next;
  struct bli_tree_struct_data_item *di;
};

/* First/last static inits.  */

struct deferred_init_details *static_init_first = NULL;
struct deferred_init_details *static_init_last = NULL;

/* First/last dynamic inits.  */

struct deferred_init_details *dynamic_init_first = NULL;
struct deferred_init_details *dynamic_init_last = NULL;

/* Current storage (work/local/linkage) being processed.  */
static struct bli_tree_struct_storage *current_storage = NULL;

/* Utility types.  These are just pointers to existing immortal types
   - no need to protect from gc.  */

tree void_pointer_type_node = NULL;
tree char_pointer_type_node = NULL;
tree integer8_type_node = NULL;
tree integer8_unsigned_type_node = NULL;
tree integer16_type_node = NULL;
tree integer16_unsigned_type_node = NULL;
tree integer32_type_node = NULL;
tree integer32_unsigned_type_node = NULL;
tree integer64_type_node = NULL;
tree integer64_unsigned_type_node = NULL;
tree space_exp = NULL;

/* Current section and paragraph for making exit labels.  */

static struct bli_tree_struct_label *save_current_section = NULL;
static struct bli_tree_struct_label *save_current_paragraph = NULL;

/* An int large enough to hold a pointer, for doing pointer arithmetic.  */
static tree integer_pointer_type_node = NULL; 

/* Decls for builtin functions.  */

static tree memset_decl;
static tree memcpy_decl;
static tree memcmp_decl;

/* Data items for controlling performs data item inits.  */

static tree first_time_item_decl; /* First time flag 1 until function init done once.  */
tree saved_call_count_decl = NULL; /* Count of calls to this function.  */
/* Array one per possible perform return. Each entry is used to
control one perform exit. If the entry matches the call count of this
function, then the perform return should happen.  */
tree saved_perform_sequ_array_decl = NULL; 
/* Array one per possible perform return. Each entry has the label to
   return to from a perform.  */
tree saved_perform_ptr_array_decl = NULL;

static tree resultdecl;  /* Result declaration for current function.  */

/* The front end language hooks (addresses of code for this front
   end).  Mostly just use the C routines.  */

#undef LANG_HOOKS_TRUTHVALUE_CONVERSION
#define LANG_HOOKS_TRUTHVALUE_CONVERSION c_common_truthvalue_conversion
#undef LANG_HOOKS_MARK_ADDRESSABLE
#define LANG_HOOKS_MARK_ADDRESSABLE c_mark_addressable
#undef LANG_HOOKS_SIGNED_TYPE
#define LANG_HOOKS_SIGNED_TYPE c_common_signed_type
#undef LANG_HOOKS_UNSIGNED_TYPE
#define LANG_HOOKS_UNSIGNED_TYPE c_common_unsigned_type
#undef LANG_HOOKS_SIGNED_OR_UNSIGNED_TYPE
#define LANG_HOOKS_SIGNED_OR_UNSIGNED_TYPE c_common_signed_or_unsigned_type
#undef LANG_HOOKS_TYPE_FOR_MODE
#define LANG_HOOKS_TYPE_FOR_MODE c_common_type_for_mode
#undef LANG_HOOKS_TYPE_FOR_SIZE
#define LANG_HOOKS_TYPE_FOR_SIZE c_common_type_for_size
#undef LANG_HOOKS_PARSE_FILE
#define LANG_HOOKS_PARSE_FILE bliss_parse_file

/* Hook routines and data unique to bliss.  */

#undef LANG_HOOKS_INIT
#define LANG_HOOKS_INIT bliss_init
#undef LANG_HOOKS_NAME
#define LANG_HOOKS_NAME	"GNU BLISS for GCC"
#undef LANG_HOOKS_DECODE_OPTION	
#define LANG_HOOKS_DECODE_OPTION bliss_decode_option
const struct lang_hooks lang_hooks = LANG_HOOKS_INITIALIZER;

/* Tree code type/name/code tables.  */

#if 0
#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) TYPE,

/*const*/ char tree_code_type[] = {
#include "tree.def"
  'x'
};
#undef DEFTREECODE

#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) LENGTH,

/*const unsigned char */ int tree_code_length[] = {
#include "tree.def"
  0
};
#undef DEFTREECODE

#define DEFTREECODE(SYM, NAME, TYPE, LEN) NAME,

const char * tree_code_name[] = {
#include "tree.def"
  "@@dummy"
};
#undef DEFTREECODE
#endif

/* Code generator - called once with parse_tree_top pointing to the
   parse tree.  */

void
blic_bei (parse_tree_top)
     struct bli_tree_struct_parse_tree_top *parse_tree_top; 
{
  
  struct bli_tree_struct_program_top *current_program;

  BLI_ASSERT (parse_tree_top);
    
  current_program = (struct bli_tree_struct_program_top *)parse_tree_top->branch.child;
  BLI_ASSERT (current_program);  /* Should be at least one at this stage lll;.  */

  // while (0 && current_program)
  while(0)
    {
      /* Lll; should run garbage collection after parse and after each function.  */
      //      output_program_code (parse_tree_top, current_program);
      //    current_program = (struct bli_tree_struct_program_top *)(current_program->cons.next);
    }

}

/* Output the code for program current_program which may be one of the
   programs in a source file.  Overall parse tree in PARSE_TREE_TOP.  */

static tree fndecl = NULL;  /* Function declaration.  */

static void
output_program_code (struct bli_tree_struct_parse_tree_top *parse_tree_top, 
                    struct bli_tree_struct_program_top *current_program)
{
  fprintf(stderr,"output_program_code\n");
}

#if 0
static void
output_program_code (struct bli_tree_struct_parse_tree_top *parse_tree_top, 
                    struct bli_tree_struct_program_top *current_program)
{
  
  static_init_first = NULL;
  static_init_last = NULL;
  
  dynamic_init_first = NULL;
  dynamic_init_last = NULL;
  
  /* Scan the program and output the code.  */
  
  /* Output the external data items in the working storage.  */
  {
    struct bli_tree_struct_data_division *current_data_division = NULL;
    struct bli_tree_struct_data_item *current_data_item;
      
    current_data_division = current_program->data_division;
      
    if (current_data_division)
      {
        for (current_storage = (struct bli_tree_struct_storage *)current_data_division->branch.child;
             current_storage; 
             current_storage = (struct bli_tree_struct_storage *)current_storage->branch.next)
          {
            struct bli_tree_struct_item_list *il;
              
            if (current_storage->reference_token->pfx.type.tok_type != WORKING_STORAGE)
              continue;
            
            for (il = current_storage->first_external;
                 il;
                 il = (struct bli_tree_struct_item_list *)il->branch.next)
              {
                current_data_item = (void *) il->branch.child;
                BLI_ASSERT (current_data_item->external_details);
                  
                if ((current_data_item->group_flag) || (current_data_item->redefine_master_flag))
                  output_group_item (current_data_item, NULL);
                else
                  output_elementary_data_item (current_data_item, NULL);
                  
                check_redefines_length ((struct bli_tree_struct_data_item *)current_data_item);
              }
            if (current_storage->first_external)
              get_pending_sizes ();
          }
      }
  }
    
  /* Output program prolog.  */
  output_program_prolog (parse_tree_top, current_program);
    
  {
    /* Output the data items in the storage areas (working storage,
       linkage, local-storage) other than externals.  */
    {
      struct bli_tree_struct_data_division *current_data_division = NULL;
      struct bli_tree_struct_data_item *current_data_item;
        
      current_data_division = current_program->data_division;
        
      if (current_data_division)
        {
          for (current_storage = (struct bli_tree_struct_storage *)current_data_division->branch.child;
               current_storage; 
               current_storage = (struct bli_tree_struct_storage *)current_storage->branch.next)
            {
              for (current_data_item = (struct bli_tree_struct_data_item *)current_storage->branch.child;
                   current_data_item; 
                   current_data_item = (struct bli_tree_struct_data_item *)current_data_item->branch.next)
                {
                  if (current_data_item->external_details)
                    continue;
                    
                  if ((current_data_item->group_flag) || (current_data_item->redefine_master_flag))
                    output_group_item (current_data_item, NULL);
                  else
                    output_elementary_data_item (current_data_item, NULL);
                    
                  check_redefines_length ((struct bli_tree_struct_data_item *)current_data_item);
                }
            }
        }
    }
      
    /* Output the procedure division now.  */
      
    {
      struct bli_tree_struct_procedure_division *current_procedure_division;
      struct bli_tree_cons *current_verb;
      
      current_procedure_division = current_program->procedure_division;
      
      if (!current_procedure_division)
        abort(); /* Should have been picked up earlier.  */
      else
        {
          /* Process the sections and paragraphs.  */

          procedure_init_part1 (current_procedure_division);
          procedure_init_part2 (current_procedure_division);
          
          build_labels (current_procedure_division);
          build_end_of_sentence_labels (current_procedure_division);
          
          save_current_section = NULL;
          save_current_paragraph = NULL;

          for (current_verb = (current_procedure_division
                               ? (struct bli_tree_cons *)current_procedure_division->branch.child : NULL); 
               current_verb; 
               current_verb = (struct bli_tree_cons *)current_verb->next)
            output_verb (current_verb);

          output_previous_procedures_exit (NULL);
        }
    }
    
  }
  
  output_program_epilog (parse_tree_top, current_program);

  /* Free the storage.  */
  
  static_init_first = NULL;
  static_init_last = NULL;

  dynamic_init_first = NULL;
  dynamic_init_last = NULL;
  
  return;
}

/* Output code for the verb or pseudo-verb CURRENT_VERB.  */

static void
output_verb (struct bli_tree_cons *current_verb)
{
  struct statement_prefix *pfx1;
  struct bli_token_struct *ref_token;
  int32 lineno;
  int32 fileno;
  native_char *filename;
  struct bli_tree_struct_verb_set *verb_set;
  
  BLI_ASSERT (current_verb->pfx.cat == type_production);
  
  pfx1 = (struct statement_prefix *)current_verb;
  
  ref_token = pfx1->reference_token;
  lineno = ref_token->bli_token_lineno;
  fileno = ref_token->bli_token_fileno;
  filename = (native_char *)(file_names->elem[fileno]);
  
  emit_line_note (filename, lineno);
  
  switch (current_verb->pfx.type.tok_type)
    {
    case BLI_PROD_TYPE_VERB_DISPLAY :
      output_display_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_VERB_STOP :
      output_stop_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_VERB_SET :
      verb_set = (struct bli_tree_struct_verb_set *)current_verb;
      switch (verb_set->set_type)
        {
        case set_type_address :
          output_set_verb_address (current_verb);
          break;
          
        case set_type_condition :
          output_set_verb_condition (current_verb);
          break;
          
        default :
          abort ();
        }
      
      break;
      
    case BLI_PROD_TYPE_VERB_GOTO :
      output_goto_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_VERB_IF :
      output_if_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_SECTION :
      {
        struct bli_tree_struct_label *current_label;

        current_label = (struct bli_tree_struct_label *) current_verb;
        output_previous_procedures_exit (current_label);
        save_current_section = (struct bli_tree_struct_label *) current_verb;
        save_current_paragraph = NULL;
        TREE_USED ((tree)current_label->save_decl) = 1;
        expand_label (current_label->save_decl);
        /* Label will only appear in the asm if it is used.  See label_nuses().  */
      }
      break;
      
    case BLI_PROD_TYPE_PARAGRAPH :
      {
        struct bli_tree_struct_label *current_label;

        current_label = (struct bli_tree_struct_label *) current_verb;
        output_previous_procedures_exit (current_label);
        save_current_paragraph = (struct bli_tree_struct_label *) current_verb;
        TREE_USED ((tree)current_label->save_decl) = 1;
        expand_label (current_label->save_decl);
        /* Label will only appear in the asm if it is used.  See label_nuses().  */
      }
      break;
      
    case BLI_PROD_TYPE_VERB_COMPUTE :
      output_compute_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_VERB_MOVE :
      output_move_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_VERB_CONTINUE :
      /* No op but needed for checking.  */
      break;
      
    case BLI_PROD_TYPE_VERB_EXIT :
      output_exit_verb (current_verb);
      break;
      
    case BLI_PROD_TYPE_VERB_FULLSTOP :
      /* Output the label if needed.  */
      {
        struct bli_tree_struct_verb_fullstop *current_fullstop;
        current_fullstop = (struct bli_tree_struct_verb_fullstop *)
          current_verb;
        if (current_fullstop->used)
          {
            TREE_USED ((tree)current_fullstop->save_decl) = 1;
            expand_label (current_fullstop->save_decl);
          }
      }
      break;
      
    case BLI_PROD_TYPE_VERB_NEXT_SENTENCE:
      /* Go to end of current sentence.  */
      output_next_sentence_verb (current_verb);
      break;

    case BLI_PROD_TYPE_VERB_PERFORM:
      output_perform_verb (current_verb, lineno, filename);
      break;

    case BLI_PROD_TYPE_VERB_CALL:
      output_call_verb (current_verb);
      break;

    default :
      BLI_ASSERT (0 && "bad verb type value");
      break;
    }
}

/* Output call verb code for CURRENT_VERB.  */
  
static void
output_call_verb (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_call *call_verb;
  struct bli_token_struct *callee;
  uchar *fixed_name;
  
  call_verb = (struct bli_tree_struct_verb_call *)current_verb;

  BLI_ASSERT (!call_verb->using_list); /* Lll;.  */

  callee = (void *) call_verb->what_to_call;

 /* Lll;.  dynamic not supported yet.  */
  BLI_ASSERT (callee->pfx.cat == type_token);
  BLI_ASSERT (callee->pfx.type.tok_type == GENERIC_STRING);

  if (call_verb->language_option)
    fixed_name = (uchar *) callee->string_details->string;
  else
    fixed_name = get_fixed_string 
      ((uchar *) callee->string_details->string_upper->string,
       callee->string_details->length, (uchar *)"");

  /* Lll; using list.  */
  call_void_function_no_args 
    (fixed_name, call_verb->pfx.reference_token);
}

/* Output perform verb code for CURRENT_VERB.  */
  
static void
output_perform_verb (struct bli_tree_cons *current_verb, uint32 lineno, native_char *filename)
{
  struct bli_tree_struct_verb_perform *perform_verb;

  perform_verb = (struct bli_tree_struct_verb_perform *)current_verb;

    switch (perform_verb->variant)
    {
    case PERFORM:
      output_perform_transfers(perform_verb, lineno, filename);
      break;

    case TIMES:
      output_perform_times (perform_verb, lineno, filename);
      break;
      
    case UNTIL:
      output_perform_until (perform_verb, lineno, filename);
      break;
      
    case VARYING:
      output_perform_varying (perform_verb, (void *) perform_verb->action ,lineno, filename);
      break;

    default:
      abort();
    }

}

/* Output perform return code for current procedures (paragraph and
   section, if present).  The new procedure is NEW_LABEL which is set
   to NULL at the end of procedure division.  */

static void 
output_previous_procedures_exit (struct bli_tree_struct_label *new_label) 
{
  output_previous_procedure_exit (save_current_paragraph);
  
  if (!new_label || (new_label->pfx.branch.pfx.type.prod_type == BLI_PROD_TYPE_SECTION))
    output_previous_procedure_exit (save_current_section);
}

/* Output perform return code for current procedure.  */

static void 
output_previous_procedure_exit (struct bli_tree_struct_label *procedure_to_exit) 
{
  tree eq_exp;
  tree array_exp;
  tree return_address;
  tree return_exp;
  tree exp;
  
  if (!procedure_to_exit)
    return;

  /* 1. Quit if the current procedure does not need an exit (never at
     the end of a perform).  */
  if (procedure_to_exit->label_exit_number < 0)
    return;

  /* 2. if call_count == call_count_when_this_last_perform_exit_was_needed) ... */
     
  array_exp = build (ARRAY_REF, TREE_TYPE (TREE_TYPE ((saved_perform_sequ_array_decl))),
                          saved_perform_sequ_array_decl, 
                     build_int_2 (procedure_to_exit->label_exit_number, 0));

  eq_exp = build (EQ_EXPR, TREE_TYPE (array_exp), array_exp, 
               build1 (CONVERT_EXPR, TREE_TYPE (array_exp), 
                       saved_call_count_decl));
  TREE_USED (eq_exp) = 1;

  expand_start_cond (eq_exp, 0);
  
  /* 3.1 Set call count to 0 for this return point. */
  
  exp = build (MODIFY_EXPR, TREE_TYPE (array_exp), array_exp, 
               build1 (CONVERT_EXPR, TREE_TYPE (array_exp), 
                       integer_zero_node));
  TREE_USED (exp) = 1;
  TREE_SIDE_EFFECTS (exp) = 1;
  expand_expr_stmt (exp);

  /* 3.2 return. */
  
  return_address = build (ARRAY_REF, TREE_TYPE (TREE_TYPE ((saved_perform_ptr_array_decl))),
                          saved_perform_ptr_array_decl, 
                          build_int_2 (procedure_to_exit->label_exit_number, 0));

  return_exp = build1 (CONVERT_EXPR, ptr_type_node, return_address);
  expand_variable_goto (return_exp);

  expand_end_cond ();
}

/* Expand a goto to a label expression EXP.  This is a variable goto
   like goto *x in C.  */ 

static void
expand_variable_goto (tree exp) 
{
  expand_computed_goto (exp);
}

/* Output the code for perform TIMES for the perform CURRENT_VERB.  */
  
static void
output_perform_times (struct bli_tree_struct_verb_perform * perform_verb,
                      uint32 lineno, native_char *filename)
{
  tree block;
  tree exit_cond;
  tree assign_expr;
  tree decr_expr;
  struct nesting *nest;
  tree loopvar_decl;
  tree loopvar_type;
  struct bli_tree_noncons *count;
  struct exp_details count_ed;
  static uint32 loop_variable_count = 0;
  char loop_variable_name[20];

  /* Push scope level.  */

  /* (*lang_hooks.decls.pushlevel) */ pushlevel (0);
  expand_start_bindings (0);

  /* Define loop variable.  */

  sprintf(loop_variable_name, "times_loop_count_variable_%d", loop_variable_count++);

  count = (void *) perform_verb->action;

  get_exp_details (& count_ed, count, NULL, 0, zero_is_numeric);
  
  loopvar_type = /* (*lang_hooks.types.type_for_size) */ type_for_size (count_ed.size * BLI_BITS_PER_BYTE,
                                !count_ed.signedp);
  
  loopvar_decl = build_decl (VAR_DECL, get_identifier (loop_variable_name), loopvar_type);
  layout_decl (loopvar_decl, 0);
  DECL_CONTEXT (loopvar_decl) = current_function_decl;
  
  DECL_INITIAL (loopvar_decl) = NULL_TREE;
  
  TREE_STATIC (loopvar_decl) = 1;
  TREE_USED (loopvar_decl) = 1;
  
  rest_of_decl_compilation (loopvar_decl, 0, 0, 0);
  pushdecl (copy_node (loopvar_decl));
      
  /* Set initial value.  */

  assign_expr = build (MODIFY_EXPR, loopvar_type, loopvar_decl, 
                       count_ed.exp);
  TREE_SIDE_EFFECTS (assign_expr) = 1;
  TREE_USED (assign_expr) = 1;
  expand_expr_stmt (assign_expr);

  /* Loop resume point.  */

  nest = expand_start_loop (0);

  /* Test if done.  */

  exit_cond = build (NE_EXPR, loopvar_type,
                     loopvar_decl,
                     build1 (CONVERT_EXPR,
                             loopvar_type,
                             integer_zero_node));
                              
  TREE_SIDE_EFFECTS (exit_cond) = 1;

  expand_exit_loop_if_false(nest, exit_cond);

  /* Do the body of the perform.  */

  output_perform_transfers (perform_verb, lineno, filename);

  /* Decrement the counter.  */

  decr_expr = build (PREDECREMENT_EXPR, loopvar_type, loopvar_decl, 
                     build1 (CONVERT_EXPR,
                             loopvar_type,
                             integer_one_node));
  TREE_SIDE_EFFECTS (decr_expr) = 1;
  TREE_USED (decr_expr) = 1;
  expand_expr_stmt (decr_expr);

  /* End loop.  
   */

  expand_continue_loop(nest);

  expand_end_loop ();

  /* Pop scope level.  */
  
  block = /* (*lang_hooks.decls.poplevel) */ poplevel  (1, 0, 0);
  expand_end_bindings (block, 1, 1);
}

/* Output the code for perform UNTIL for the perform CURRENT_VERB.  */
  
static void
output_perform_until (struct bli_tree_struct_verb_perform * perform_verb,
                      uint32 lineno, native_char *filename)
{
  tree block;
  tree exit_cond;
  tree cond_test;
  struct nesting *nest;
  struct bli_tree_noncons *test_exp;

  /* Push scope level.  */

  /* (*lang_hooks.decls.pushlevel) */ pushlevel (0);
  expand_start_bindings (0);

  /* Loop resume point.  */

  nest = expand_start_loop (0);

  /* Test if done.  */

  test_exp = (void *) perform_verb->action;
  cond_test = conditional_test (test_exp);
  exit_cond = build1 (TRUTH_NOT_EXPR,
                     TREE_TYPE (cond_test),
                     cond_test);
                              
  if (perform_verb->when_to_test 
      && (perform_verb->when_to_test->pfx.type.tok_type == AFTER))
    {
      output_perform_transfers (perform_verb, lineno, filename);
      expand_exit_loop_if_false(nest, exit_cond);
    }
  else
    {
      expand_exit_loop_if_false(nest, exit_cond);
      output_perform_transfers (perform_verb, lineno, filename);
    }

  /* End loop.  
   */

  expand_continue_loop(nest);

  expand_end_loop ();

  /* Pop scope level.  */
  
  block = /* (*lang_hooks.decls.poplevel) */ poplevel  (1, 0, 0);
  expand_end_bindings (block, 1, 1);
}

/* Output the code for perform VARYING for the perform CURRENT_VERB.
   This may be nested.  CLAUSE_PTR points to the chain element
   pointing to the current VARYING/AFTER clause.  */
  
static void
output_perform_varying (struct bli_tree_struct_verb_perform *perform_verb,
                        struct bli_tree_struct_item_list *clause_ptr,
                        uint32 lineno, native_char *filename)
{
  tree exit_cond;
  tree cond_test;
  tree assign_expr;
  void *test_exp;
  struct nesting *nest;
  struct bli_tree_struct_item_list* pp1;
  struct bli_tree_struct_perform_varying * pv1;

  /* Set up test.  */

  pv1 = (void *) clause_ptr->branch.child;
  test_exp = pv1->until;
  cond_test = conditional_test (test_exp);
  exit_cond = build1 (TRUTH_NOT_EXPR,
                     TREE_TYPE (cond_test),
                     cond_test);
                              
  /* At top level, init the variables for this and lower levels.  */

  if (clause_ptr == (void *) perform_verb->action)
    {
      struct exp_details var_ed;
      struct exp_details from_ed;
      struct exp_details by_ed;
      
      for (pp1 = clause_ptr;
           pp1;
           pp1 = (void *) pp1->branch.next)
        {
          VERIFY_TYPE(pp1, type_production, BLI_PROD_TYPE_ITEM_LIST, 0);
          pv1 = (void *) pp1->branch.child;
          VERIFY_TYPE(pv1, type_production, BLI_PROD_TYPE_PERFORM_VARYING, 0);
          get_exp_details (&var_ed, pv1->variable, NULL, 0, zero_is_numeric);
          get_exp_details (&from_ed, pv1->from, NULL, 0, zero_is_numeric);
          get_exp_details (&by_ed, pv1->by, NULL, 0, zero_is_numeric);
          
          pv1->var_expr = var_ed.exp;
          pv1->from_expr = from_ed.exp;
          pv1->by_expr = by_ed.exp;

          assign_expr = build (MODIFY_EXPR, TREE_TYPE ((tree)pv1->var_expr), pv1->var_expr, 
                               build1 (CONVERT_EXPR, TREE_TYPE ((tree)pv1->var_expr), pv1->from_expr));
          TREE_SIDE_EFFECTS (assign_expr) = 1;
          TREE_USED (assign_expr) = 1;
          expand_expr_stmt (assign_expr);
        }
    }

  /* Loop resume point.  */

  nest = expand_start_loop (0);

  /* Body of loop and test.  */

  if (perform_verb->when_to_test 
      && (perform_verb->when_to_test->pfx.type.tok_type == AFTER))
    {
      if (clause_ptr->branch.next)
        output_perform_varying (perform_verb,
                                (void *) clause_ptr->branch.next,
                                lineno, filename);
      else
        output_perform_transfers (perform_verb, lineno, filename);
      expand_exit_loop_if_false(nest, exit_cond);
    }
  else
    {
      expand_exit_loop_if_false(nest, exit_cond);
      if (clause_ptr->branch.next)
        output_perform_varying (perform_verb,
                                (void *) clause_ptr->branch.next,
                                lineno, filename);
      else
      output_perform_transfers (perform_verb, lineno, filename);
    }
  
  /* Increment current level index.  */

  /* Lll; this just happens to work with binary numbers but with
     packed decimal etc special care is needed if the loop variable is
     unsigned and it is being incremented by a negative value.  */

  pv1 = (void *) clause_ptr->branch.child;
  assign_expr = build (MODIFY_EXPR, TREE_TYPE ((tree)pv1->var_expr), pv1->var_expr, 
                       build (PLUS_EXPR, TREE_TYPE ((tree)pv1->var_expr),
                              pv1->var_expr,
                              build1 (CONVERT_EXPR, TREE_TYPE ((tree)pv1->var_expr), pv1->by_expr)));
  TREE_SIDE_EFFECTS (assign_expr) = 1;
  TREE_USED (assign_expr) = 1;
  expand_expr_stmt (assign_expr);

  /* Set all lower levels.  */

  for (pp1 = (void *) clause_ptr->branch.next;
       pp1;
       pp1 = (void *) pp1->branch.next)
    {
      pv1 = (void *) pp1->branch.child;

      assign_expr = build (MODIFY_EXPR, TREE_TYPE ((tree)pv1->var_expr), pv1->var_expr, 
                           build1 (CONVERT_EXPR, TREE_TYPE ((tree)pv1->var_expr), pv1->from_expr));
      TREE_SIDE_EFFECTS (assign_expr) = 1;
      TREE_USED (assign_expr) = 1;
      expand_expr_stmt (assign_expr);
    }

  /* End loop.  
   */

  expand_continue_loop(nest);

  expand_end_loop ();

}

/* Output the actual transfers going to and set up the address to
   return from for the perform CURRENT_VERB.  */
  
static void
output_perform_transfers (struct bli_tree_struct_verb_perform * perform_verb,
                          uint32 lineno, native_char *filename)
{
  tree return_label;
  tree return_address;
  native_char label_name[20];
  tree id;
  tree exp;
  tree to_exp;
  struct bli_tree_struct_labels *labels;
  static uint32 perform_label_count = 0;
  struct bli_tree_struct_procedure_ref *current_proc;
  struct bli_tree_struct_label *end_label;
  uint32 inline_flag = 0;
  
  /* Arrange to go to the start of the perform and to return back.  */

  if (!perform_verb->what_to_perform)
    return;
  
  if (((struct bli_tree_noncons*)(perform_verb->what_to_perform))->pfx.type.tok_type != BLI_PROD_TYPE_LABELS)
    inline_flag = 1;

  if (inline_flag)
    {
      struct statement_prefix *current_verb;
      for (current_verb = (void *) perform_verb->what_to_perform;
           current_verb; 
           current_verb = (void *) current_verb->branch.next)
        {
          BLI_ASSERT (current_verb->branch.pfx.cat == type_production);
          output_verb ((struct bli_tree_cons *)current_verb);
        }
    }
  else
    {
      /* 1. Create the return label decl.  */
      
      sprintf(label_name, "perform_return_label_%d", perform_label_count++);
      id = get_identifier (label_name);
      return_label = build_decl (LABEL_DECL, id, void_type_node);
      DECL_CONTEXT (return_label) =  fndecl;
      DECL_MODE (return_label) = VOIDmode;
      DECL_SOURCE_LINE (return_label) = lineno;
      DECL_SOURCE_FILE (return_label) =  filename;
      TREE_USED (return_label) = 1;
      
      /* 2. Go set the return address.  */
      
      /* 2.1. Get the label number to set the return address from.  */
      
      labels = (void *) perform_verb->what_to_perform;
      
      current_proc = labels->from;
      if (labels->thru)
        current_proc = labels->thru;
      else 
        current_proc = labels->from;
      
      end_label = (void *) current_proc->label;
      
      /* 2.2 Return address.  */
      
      return_address = build1 (ADDR_EXPR, ptr_type_node, return_label);
      TREE_CONSTANT (return_address) = 1;
      
      to_exp = build (ARRAY_REF, TREE_TYPE (TREE_TYPE ((saved_perform_ptr_array_decl))),
                      saved_perform_ptr_array_decl, build_int_2 (end_label->label_exit_number, 0));
      
      exp = build (MODIFY_EXPR, TREE_TYPE (to_exp), to_exp, 
                   build1 (CONVERT_EXPR, TREE_TYPE (to_exp), 
                           return_address));
      TREE_USED (exp) = 1;
      TREE_SIDE_EFFECTS (exp) = 1;
      expand_expr_stmt (exp);
      
      /* 2.3 Set call count (for check return is current).  */
      
      to_exp = build (ARRAY_REF, TREE_TYPE (TREE_TYPE ((saved_perform_sequ_array_decl))),
                      saved_perform_sequ_array_decl, build_int_2 (end_label->label_exit_number, 0));
      
      exp = build (MODIFY_EXPR, TREE_TYPE (to_exp), to_exp, 
                   build1 (CONVERT_EXPR, TREE_TYPE (to_exp), 
                           saved_call_count_decl));
      TREE_USED (exp) = 1;
      TREE_SIDE_EFFECTS (exp) = 1;
      expand_expr_stmt (exp);
      
      /* 3. Go to the performed procedure.  */
      
      expand_goto (((struct bli_tree_struct_label *)(labels->from->label))->save_decl);
      
      /* 4. Output the label to return to.  */
      
      expand_label (return_label);
    }
}

/* Output code for NEXT SENTENCE, details in CURRENT_VERB.  */

static void
output_next_sentence_verb (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_next_sentence *current_next_sentence;
  struct bli_tree_struct_verb_fullstop *current_fullstop;

  current_next_sentence = (struct bli_tree_struct_verb_next_sentence *)
    current_verb;
  current_fullstop = (struct bli_tree_struct_verb_fullstop *)
    current_next_sentence->end_of_sentence;

  BLI_ASSERT (current_fullstop);
  BLI_ASSERT (current_fullstop->pfx.branch.pfx.cat == type_production);
  BLI_ASSERT (current_fullstop->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_VERB_FULLSTOP);
  
  BLI_ASSERT (current_fullstop->save_decl);
  expand_goto (current_fullstop->save_decl);
}

/* Output compute verb code for current_verb.  */
  
static void
output_compute_verb (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_compute *compute_verb;
  struct bli_tree_branch *compute_to_item;
  struct bli_tree_struct_expression *to_item_exp;
  struct exp_details from_ed;
  struct exp_details to_ed;
  tree from_exp;
  tree from_exp2;
  tree to_exp2;
  tree to_item_tree;

  compute_verb = (struct bli_tree_struct_verb_compute *)current_verb;
  BLI_ASSERT (compute_verb->pfx.branch.pfx.cat == type_production);
  BLI_ASSERT (compute_verb->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_VERB_COMPUTE);
  
  /* Get the from expression.  */
  /* Everything was already validated so nothing can go wrong.  */
  
  get_exp_details (& from_ed, compute_verb->compute_expression, NULL, 0, zero_is_numeric);
  
  from_exp = save_expr (from_ed.exp);

  /* Compute all the 'to' expressions to it.  */
  
  BLI_ASSERT (compute_verb->compute_to);

  for (compute_to_item = (struct bli_tree_branch *)compute_verb->compute_to;
       compute_to_item;
       compute_to_item = (struct bli_tree_branch *)compute_to_item->next)
    {
      to_item_exp = (struct bli_tree_struct_expression *)compute_to_item;
      BLI_ASSERT (to_item_exp->cons.pfx.cat == type_production);
      BLI_ASSERT (to_item_exp->cons.pfx.type.tok_type == BLI_PROD_TYPE_EXPRESSION);

      get_exp_details (& to_ed, to_item_exp, NULL, 0, zero_is_numeric);
      to_item_tree = to_ed.exp;
      
      from_exp2 = build1 (CONVERT_EXPR, TREE_TYPE (to_item_tree), from_exp);

      to_exp2 = build (MODIFY_EXPR, TREE_TYPE (to_item_tree), to_item_tree, from_exp2);
      TREE_USED (to_exp2) = 1;
      TREE_SIDE_EFFECTS (to_exp2) = 1;
      
      expand_expr_stmt (to_exp2);
    }
}

/* Output exit verb code for CURRENT_VERB.  */
  
static void
output_exit_verb (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_exit *verb;

  verb = (struct bli_tree_struct_verb_exit *)current_verb;
  if (!verb->program)
    return;

  /* Exit Program.   Return to caller.  */
  {
    tree assign_expr;
    /* Build tree for "result = 0.  */
    assign_expr = build (MODIFY_EXPR, 
                         integer32_type_node, 
                         resultdecl, integer_zero_node);
    TREE_SIDE_EFFECTS (assign_expr) = 1;
    TREE_USED (assign_expr) = 1;
    expand_expr_stmt (assign_expr);
    expand_return (resultdecl);
  }
}

/* Output move verb code for CURRENT_VERB.  */
  
static void
output_move_verb (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_move *verb;
  struct bli_tree_struct_double_entry_list *corr_pair;
  struct bli_tree_struct_item_list *to_list;
  struct bli_tree_noncons *to;
  struct exp_details from_ed;
  struct exp_details to_ed;

  verb = (struct bli_tree_struct_verb_move *)current_verb;
  if (verb->corr_token)
    {
      for (corr_pair = (void *) verb->move_corr_list;
           corr_pair;
           corr_pair = (void *) corr_pair->branch.next)
        if (corr_pair->branch.child)
          {
            get_exp_details (& to_ed, corr_pair->child2, NULL, 0, zero_is_not_numeric);
            get_exp_details (& from_ed, corr_pair->branch.child, NULL, 0,
                            to_ed.di->numeric_flag ? zero_is_numeric : zero_is_not_numeric);
            output_move (& from_ed,  & to_ed, verb->pfx.reference_token);
          }
    }
  else
    {
      
      for (to_list = (struct bli_tree_struct_item_list *)verb->move_to; 
           to_list; 
           to_list = (struct bli_tree_struct_item_list *)to_list->branch.next)
        {
          to = (void *) to_list->branch.child;
          get_exp_details (& to_ed, to, NULL, 0, zero_is_not_numeric);
          get_exp_details (& from_ed, verb->move_from, NULL, 0,
                            to_ed.di->numeric_flag ? zero_is_numeric : zero_is_not_numeric);
          /* Ensure all the froms are the same, even if array indexes
             etc change in the meantime.  */
          from_ed.exp = save_expr (from_ed.exp);
          output_move (& from_ed,  & to_ed, verb->pfx.reference_token);
        }
    }
}

/* 
   Output move verb code for FROM to TO.  Everything has been
   validated, FROM and TO all filled in.
   
   Size error and overflow are not supported. lll;
   Aligned left not supported. lll;.  
   
   Yes this is a horrible long module.  Sorry. 

*/
  
static void
output_move (struct exp_details *from, struct exp_details *to, struct bli_token_struct *ref_tok)
{
  /* 
     The rules are
     
     1.  For numeric to numeric
     - if receiving item is unsigned, strip sign
     - expand or truncate as needed
     > all this is done with generated code for binary types

     2. For alphanumeric to alphanumeric
     - move and truncate or space fill on the right (lll; justified)
     > done inline with memcpy/memset builtin functions
     
     3. Otherwise where one of the items is a group item.  
     - treat as alphanumeric move - no conversion, space fill on the
     right (lll; justified), or truncate. That's what the standard says.  
     - when the from area is a numeric literal we assume the number of
     digits is the number of digits in the literal with leading sign
     chopped (ie looks like the literal). The standard is vague on
     this point.  
     > done inline with memcpy/memset builtin functions
     
     4. For numeric to alphanumeric elementary.
     - strip the sign
     - move the digits, then space fill or truncate as needed on the right
     - the 2002 standard does not define the number of digits for
     binary-*. I use the number of digits needed to hold a number
     of that size eg binary-char=>3 (128/256), binary-long=>10(2g/4g).
     > done as subroutine
     
     5. For alphanumeric to numeric elementary 
     - assume sign is +
     - convert the field, which should be all numeric, to a number
     - store the number in the receiving item
     > done as subroutine

     6. Move from a fractional number to and from alphanumeric is not
     allowed, so this routine will never be asked to do that.

     7. lll; alphabetic, numeric with pic, numeric edited, floating
     point, figurative constants all later.  */

  uint32 which = 0;  /* Matches item numbers in list above.  */
  tree from_exp;
  tree exp;
  tree call_expr;
  tree length1_exp;
  tree length2_exp;
  tree void_address_exp;
  tree void_address_from_exp;
  tree void_address_to_exp;
  tree memcpy_decl_ptr;
  tree ptr_as_int;
  tree interim_exp;
  tree args;
  uint32 from_size;  /* After chopping sign if needed.  */
  uchar from_char1;

  /* Determine type of move, and also the first character if from
     constant for later use to check sign.  */
  switch (from->token_type)
    {
    case NULL_word :
      exp = build (MODIFY_EXPR, TREE_TYPE (to->exp), to->exp, 
                build1 (CONVERT_EXPR, TREE_TYPE (to->exp), null_pointer_node));
      TREE_USED (exp) = 1;
      TREE_SIDE_EFFECTS (exp) = 1;
      expand_expr_stmt (exp);
      return;

    case BINARY_CHAR :
    case BINARY_SHORT :
    case BINARY_LONG :
    case BINARY_DOUBLE :
      switch (to->token_type)
        {
        case BINARY_CHAR :
        case BINARY_SHORT :
        case BINARY_LONG :
        case BINARY_DOUBLE :
          which = 1;
          break;
          
        case DISPLAY :
          if (to->di->numeric_flag)
            abort (); /* LLL; numeric display not supported.  */
          else
            which = 4;
          break;
          
        default :
          abort ();
        }
      break;
      
    case ZERO :
    case ZEROS :
    case ZEROES :
      if (to->token_type == DISPLAY)
        {
          if (to->di->numeric_flag)
            abort (); /* LLL; numeric display not supported.  */
          else
            goto common2;
        }
      set_to_numeric_zero (from);
      /* Drop through.  */
    case GENERIC_NUMBER_INTEGER :
      switch (to->token_type)
        {
        case BINARY_CHAR :
        case BINARY_SHORT :
        case BINARY_LONG :
        case BINARY_DOUBLE :
          BLI_ASSERT (to->di);
          if (to->di->digits)
            {
              if (from->token_type == GENERIC_NUMBER_INTEGER)
                check_number_of_digits
                  (to->di->digits, from->token->string_details->length, 
                   from->token->string_details->string, from->token);
            }
          which = 1;
          break;
          
        case DISPLAY :
          if (to->di->numeric_flag)
            abort (); /* LLL; numeric display not supported.  */
          else
            which = 3;
          break;
          
        default :
          abort ();
        }
      break;
      
    case DISPLAY :
      /* LLL; numeric display not supported.  */
      if (from->di && from->di->numeric_flag)
        abort (); 

    /* Else drop through.  */

    case QUOTE :
    case QUOTES :
    case SPACE :
    case SPACES :
    case LOW_VALUE :
    case LOW_VALUES :
    case HIGH_VALUE :
    case HIGH_VALUES :
    common2 :
        
    case GENERIC_STRING :
      switch (to->token_type)
          {
          case DISPLAY :
            if (to->di && to->di->numeric_flag)
              abort (); /* LLL; numeric display not supported.  */
            else
              which = 2;
            break;
            
          case BINARY_CHAR :
          case BINARY_SHORT :
          case BINARY_LONG :
          case BINARY_DOUBLE :
            if (from->all_flag)
              which = 1;
            else
              which = 5;
            break;
          
          default :
            abort ();
          }
      break;
      
    default :
      abort ();
    }
  
  /* Group moves are special cases.  */
  if ((to->di && to->di->group_flag) || (from->di && from->di->group_flag))
    which = 3;

  /* Generate the move.  */
  switch (which)
    {
    case 1 :  /* Numeric to numeric or all literal to numeric.  */
      {
        tree mode1 = NULL_TREE;
        uint32 from_size;
        uint32 to_size;

        if (from->all_flag)
          {
              {
                uchar *from_str;
                uchar *work_string;
                uchar *hacked_string;
                uchar *hacked_string_end;
                uint32 slop;
                uint64 value;
                uint32 lo;
                uint32 hi;
                uint32 use_digits;
                
                switch (from->token_type)
                  {
                  case ZERO :
                  case ZEROS :
                  case ZEROES :
                    from_str = (uchar *)"0";
                    from_size = 1;
                    break;
                    
                  case GENERIC_STRING :
                    if (!to->di->digits)
                      {
                        /* No digits - ignore ALL - see assignment of from_size below.  */
                        MSG_TOKEN (MSGID (183), ref_tok, NULL);
                      }
                    from_str = (void *) from->token->string_details->string;
                    from_size = from->token->string_details->length;
                    break;
                  
                  default :
                    abort ();
                  }
                
                use_digits = to->di->digits ? to->di->digits : from_size;

                hacked_string = BLI_ALLOC (use_digits +1);
                
                if (from_size != 1 && to->di->digits)
                  {
                    MSG_TOKEN (MSGID (185), ref_tok, NULL);
                  }
                slop = use_digits % from_size;
                hacked_string_end = hacked_string + use_digits - slop;
                for (work_string = hacked_string; work_string != hacked_string_end; work_string += from_size)
                  memcpy (work_string, from_str, from_size);
                
                if (slop)
                  memcpy (work_string, from_str, slop);
                
                /* Now we have the value, generate the move.  */
                
                for (work_string = hacked_string, value = 0;  * work_string; work_string ++)
                  {
                    /* Lll; check numeric.  */
                    value = value * 10 + (* work_string - (uchar)'0');
                  }
                lo = value & BLI_INT32_MASK;
                hi = (value >> 32) & BLI_INT32_MASK;
                
                from_exp = build_int_2 (lo, hi);
                
                exp = build (MODIFY_EXPR, TREE_TYPE (to->exp), to->exp, 
                          build1 (CONVERT_EXPR, TREE_TYPE (to->exp), from_exp));
                
                TREE_USED (exp) = 1;
                TREE_SIDE_EFFECTS (exp) = 1;
                
                expand_expr_stmt (exp);
                
              }
            break;
          }

        from_exp = from->exp;

        from_size = from->size;
        to_size = to->size;
        
        if (from->constant_flag)
          {
            if (TREE_CODE (from->exp) == SAVE_EXPR)
              mode1 = TREE_TYPE (unsave_expr (from->exp));
            else
              mode1 = TREE_TYPE (from->exp);
            
            if (mode1 == integer32_type_node)
              from_size = BLI_INT32_WIDTH;
            else if (mode1 == integer32_unsigned_type_node)
              from_size = BLI_INT32_WIDTH;
            else
              if (mode1 == integer64_type_node)
                from_size = BLI_INT64_WIDTH;
              else
                if (mode1 == integer64_unsigned_type_node)
                  from_size = BLI_INT64_WIDTH;
                else
                  abort ();  /* If it were unsigned there would be no need for abs.  */
          }

        if (from->signedp && !to->signedp)
          {
            if (!from->constant_flag)
              {
                switch (from->size)
                  {
                  case 1 :
                    mode1 = integer8_type_node;
                    break;
                    
                  case 2 :
                    mode1 = integer16_type_node;
                    break;
                    
                  case 4 :
                    mode1 = integer32_type_node;
                    break;
                    
                  case 8 :
                    mode1 = integer64_type_node;
                    break;
                    
                  default :
                    abort ();
                  }
              }
            /* Move signed to unsigned gets absolute value.  This
               is not always possible eg with 2s complement
               maximum negative value.  */
            from_exp = build1 (ABS_EXPR, mode1, from->exp);
          }

        if (to->di->digits)
          from_exp = truncate_to_digits (from_exp, to->di->digits);
        
        /* Convert to the right type.  */
        if ((from_size != to_size) || (from->signedp != to->signedp)) 
          from_exp = build1 (CONVERT_EXPR, TREE_TYPE (to->exp), from_exp);
        
        exp = build (MODIFY_EXPR, TREE_TYPE (to->exp), to->exp, from_exp);
        
        TREE_USED (exp) = 1;
        TREE_SIDE_EFFECTS (exp) = 1;
        
        expand_expr_stmt (exp);
      }
      break;

    case 2 :  /* Alphanumeric to alphanumeric;.  */
    case 3 :  /* Alphanumeric move due to one item is a group, or
               literal to alphanumeric.  */
      /* Move from x to group.  */
      /* If we are moving a numeric literal to an elementary
         alphanumeric item, the sign needs to be stripped off.  */
      if ((from->token_type == GENERIC_NUMBER_INTEGER)
          && to->di && !to->di->group_flag
          && from->signedp)
        {
          tree temp1a, temp1b, temp2, temp3;
          from->size --;
          temp1a = build1 (CONVERT_EXPR, integer_pointer_type_node, from->address);
          temp1b = build1 (CONVERT_EXPR, integer_pointer_type_node, integer_one_node);
          temp2 = build (PLUS_EXPR, integer_pointer_type_node, temp1a, temp1b);
          temp3 = build1 (CONVERT_EXPR, void_pointer_type_node, temp2);
          from->address = temp3;
          from->length = build (MINUS_EXPR, TREE_TYPE (from->length), from->length,
                                build1 (CONVERT_EXPR,
                                        TREE_TYPE (from->length),
                                        integer_one_node));
        }
      
      /* Move from numeric literal to group is not defined in the
       standard.  As a literal it does not have a binary
       representation, so cannot be moved as is. I assume it is
       described as pic [+]9(n) usage display so I can move the
       literal as is (without the sign).  */

        from_size = from->size;
        
        /* 1. Copy as much as you can from the sending area.  */
        void_address_from_exp = build1 (NOP_EXPR, ptr_type_node, from->address);
        void_address_to_exp = build1 (NOP_EXPR, ptr_type_node, to->address);

        length1_exp = build (MIN_EXPR, integer_type_node, 
                          build1 (CONVERT_EXPR, integer_type_node, from->length), 
                          build1 (CONVERT_EXPR, integer_type_node, to->length));
        
        /* Strip sign from text if numeric literal to group item.  */
        if ((from->token_type == GENERIC_NUMBER_INTEGER)
            && to->di && to->di->group_flag)
          {
            from_char1 = from->token->string_details->string[0];
            
            if ((from_char1 == (uchar)'+') || (from_char1 == (uchar)'='))
              {
                length1_exp = build (MINUS_EXPR, integer_type_node, length1_exp, integer_one_node);
                void_address_from_exp = build1 (NOP_EXPR, ptr_type_node, 
                                             build (PLUS_EXPR,
                                                   integer_pointer_type_node,
                                                   build1 (CONVERT_EXPR,
                                                          integer_pointer_type_node,
                                                          from->address),
                                                   build1 (CONVERT_EXPR,
                                                          integer_pointer_type_node,
                                                          integer_one_node)
                                                   ));
                from_size --;
              }
          }
        
      if (from->constant_flag && from->all_flag)
        {
          if (from->size == (1 + from->signedp))
            {
              tree char_exp;

              switch (from->token_type)
                {
                case QUOTE :
                case QUOTES :
                case SPACE :
                case SPACES :
                case LOW_VALUE :
                case LOW_VALUES :
                case HIGH_VALUE :
                case HIGH_VALUES :
                case ZERO :
                case ZEROES :
                case ZEROS :
                  break;
                  
                case GENERIC_STRING :
                    from->first_char = from->token->string_details->string[from->signedp];
                    break;
                    
                default :
                  abort ();
                }
              
              /* Build the call to memset.  */
              output_memset (void_address_to_exp, 
                             char_exp = build_int_2 ((unsigned char)from->first_char, 0),  /* Assumes ascii.  */
                             build1 (CONVERT_EXPR, integer_type_node, to->length)); 

                             
              
            }
          else
            {
              tree moveall_identifier;
              tree param_type_list;
              tree decl1;
              tree moveall_decl_ptr;
              int32 lineno;
              int32 fileno;

              if (!moveall_fn_decl)
                {
                  moveall_identifier = get_identifier ("blir_moveall"); 
                  
                  /* Types are consed in reverse order.  */
                  param_type_list = tree_cons
                    (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
                  
                  param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
                  param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
                  param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
                  param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
                  
                  moveall_fn_type = build_function_type (void_type_node, param_type_list);
                  
                   (decl1 = build_decl (PARM_DECL, get_identifier ("length_from"), 
                                   integer32_type_node));
                   (decl1 = build_decl (PARM_DECL, get_identifier ("address_from"), 
                                   void_pointer_type_node));
                   (decl1 = build_decl (PARM_DECL, get_identifier ("length_to"), 
                                   integer32_type_node));
                   (decl1 = build_decl (PARM_DECL, get_identifier ("address_to"), 
                                   void_pointer_type_node));

                  lineno = ref_tok->bli_token_lineno;
                  fileno = ref_tok->bli_token_fileno;
                  input_filename = (native_char *)(file_names->elem[fileno]);
                  
                  moveall_fn_decl = build_decl
                    (FUNCTION_DECL, moveall_identifier, moveall_fn_type);
                  
                  TREE_TYPE (moveall_fn_decl) = moveall_fn_type;
                  DECL_EXTERNAL (moveall_fn_decl) = 1;
                  TREE_PUBLIC (moveall_fn_decl) = 1;
                  TREE_STATIC (moveall_fn_decl) = 0;
                  DECL_ARGUMENTS (moveall_fn_decl) = getdecls ();
                  DECL_RESULT (moveall_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, void_type_node);
                  DECL_CONTEXT (DECL_RESULT (moveall_fn_decl)) = NULL_TREE;
                  TREE_USED (moveall_fn_decl) = 1;
                  
                  rest_of_decl_compilation (moveall_fn_decl, NULL, 1, 0);
                  make_decl_rtl (moveall_fn_decl, NULL);
                }
              
              length1_exp = build1 (CONVERT_EXPR, integer_type_node, from->length);
              length2_exp = build1 (CONVERT_EXPR, integer_type_node, to->length);

              args = tree_cons (NULL_TREE, length1_exp, NULL_TREE); 
              args = chainon (tree_cons (NULL_TREE, void_address_from_exp, NULL_TREE), args);
              args = chainon (tree_cons (NULL_TREE, length2_exp, NULL_TREE), args);
              args = chainon (tree_cons (NULL_TREE, void_address_to_exp, NULL_TREE), args);
              
              moveall_decl_ptr = build1
              (ADDR_EXPR, build_pointer_type (TREE_TYPE (moveall_fn_decl)), moveall_fn_decl);
            
              call_expr = build (CALL_EXPR, void_type_node, moveall_decl_ptr, args, NULL_TREE);
              TREE_USED (call_expr) = 1;
              TREE_SIDE_EFFECTS (call_expr) = 1;
              
              expand_expr_stmt (call_expr);
            }
          break;
        }

        /* Invoke memcpy.  */
        args = tree_cons (NULL_TREE, length1_exp, NULL_TREE); 
        args = chainon (tree_cons (NULL_TREE, void_address_from_exp, NULL_TREE), args);
        args = chainon (tree_cons (NULL_TREE, void_address_to_exp, NULL_TREE), args);
                      
        memcpy_decl_ptr = build1
          (ADDR_EXPR, build_pointer_type (TREE_TYPE (memcpy_decl)), memcpy_decl);

        call_expr = build (CALL_EXPR, void_type_node, memcpy_decl_ptr, args, NULL_TREE);
        TREE_USED (call_expr) = 1;
        TREE_SIDE_EFFECTS (call_expr) = 1;
        expand_expr_stmt (call_expr);

        /* 2. Fill the rest with spaces using memset.  */

        /* Set to pointer to point after the copied area.  */
        if (!from->size || !to->size || (from_size < to->size))
          {
            ptr_as_int = build1 (CONVERT_EXPR, integer_pointer_type_node,
                              void_address_to_exp);
            interim_exp = build (PLUS_EXPR, integer_pointer_type_node,
                              build1 (CONVERT_EXPR, integer_pointer_type_node, length1_exp),
                              ptr_as_int);
            void_address_exp = build1 (CONVERT_EXPR, ptr_type_node, interim_exp);

            /* Length is whatever is left over, never less than 0.  */
            length2_exp = build (MINUS_EXPR, integer_type_node, to->length, length1_exp);
            length2_exp = build (MAX_EXPR, integer_type_node, integer_zero_node, length2_exp);
            
            output_memset (void_address_exp, space_exp, length2_exp);
          }
      break;

    case 4 :  /* Numeric to alphanumeric (note: numeric literal to
               alphanumeric is in case 3).  */
      build_conversion_numeric_to_alpha (from, to, ref_tok); 
      break;

    case 5 :  /* Alphanumeric to numeric.  */
      build_conversion_alpha_to_numeric (from, to, ref_tok); 
      break;

    default :
      abort ();
    }
}

/* Output move expression from->to.  We are moving a number to a
   alphanumeric item.  */

static void
build_conversion_numeric_to_alpha (struct exp_details *from,
                                  struct exp_details *to,
                                  struct bli_token_struct *ref_token)
{
  tree newtype1 = NULL_TREE;
  tree call_expr;
  tree void_address_exp;
  uint32 digitsn;
  tree digits;

  uint32 ix;
  int32 fileno = 0;
  uint32 size_from_work;
  uint32 sign_from_work;
  tree exp_from_work;

  /* Uplift to min 32 bits.  */
  size_from_work = from->size;
  sign_from_work = from->signedp;
  exp_from_work = from->exp;
  
  if (size_from_work < BLI_INT32_WIDTH)
    {
      size_from_work = BLI_INT32_WIDTH;
      sign_from_work = 1;
      newtype1 = integer32_type_node;
    }
  else
    newtype1 = TREE_TYPE (from->exp);

  if (newtype1 != TREE_TYPE (from->exp))
    exp_from_work = build1 (CONVERT_EXPR, newtype1, exp_from_work);

  /* Set up the index to the function to call.  */

  ix = ((size_from_work == BLI_INT64_WIDTH) << 1) | sign_from_work;
  
  /* Build the function decl if not done yet.  */

  if (conversion_fn_decls[ix] == NULL)
    {
      static const uchar *const fnnames[16] =
      {
        (uchar *)"blir_convert_32_n_X",
        (uchar *)"blir_convert_32_y_X",
        (uchar *)"blir_convert_64_n_X",
        (uchar *)"blir_convert_64_y_X"
      };
      tree fn_fn_type;
      tree fn_fn_decl;
      tree fn_identifier;
      tree param_type_list;
      tree decl1;
      
      fn_identifier = get_identifier ((native_char *)fnnames[ix]); 

      /* Types are consed in reverse order.  */

      param_type_list = tree_cons
        (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
      
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, newtype1, param_type_list);

      fn_fn_type = build_function_type (void_type_node, param_type_list);

       (decl1 = build_decl (PARM_DECL, get_identifier ("length"), 
                       integer32_type_node));
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("address"), 
                       void_pointer_type_node));

       (decl1 = build_decl (PARM_DECL, get_identifier ("digits"), 
                       integer32_type_node));

       (decl1 = build_decl (PARM_DECL, get_identifier ("number"), 
                       newtype1));

      lineno = ref_token->bli_token_lineno;
      fileno = ref_token->bli_token_fileno;
      input_filename = (native_char *)file_names->elem[fileno];

      fn_fn_decl = build_decl
        (FUNCTION_DECL, fn_identifier, fn_fn_type);
                    
      TREE_TYPE (fn_fn_decl) = fn_fn_type;
      DECL_EXTERNAL (fn_fn_decl) = 1;
      TREE_PUBLIC (fn_fn_decl) = 1;
      TREE_STATIC (fn_fn_decl) = 0;
      DECL_ARGUMENTS (fn_fn_decl) = getdecls ();
      DECL_RESULT (fn_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, newtype1);
      DECL_CONTEXT (DECL_RESULT (fn_fn_decl)) = NULL_TREE;
      TREE_USED (fn_fn_decl) = 1;

      rest_of_decl_compilation (fn_fn_decl, NULL, 1, 0);
      make_decl_rtl (fn_fn_decl, NULL);
      
      conversion_fn_decls[ix] = fn_fn_decl;
      conversion_fn_types[ix] = fn_fn_type;
    }

  {
    tree args;
    tree fn_decl_ptr;
    
    fn_decl_ptr = build1 (ADDR_EXPR, build_pointer_type (conversion_fn_types[ix]), conversion_fn_decls[ix]);

    digitsn = 0;
    if (from->di)
      digitsn = from->di->digits;
    if (!digitsn)
      switch (size_from_work)
        {
        case 1 :
          digitsn = 3;
          break;
          
        case 2 :
          digitsn = 5;
          break;
          
        case 4 :
          digitsn = 10;
          break;
          
        case 8 :
          if (sign_from_work)
            digitsn = 19;
          else
            digitsn = 20;
          break;
          
        default :
          abort ();
        }
    
    digits = build_int_2 (digitsn, 0);
    
    void_address_exp = build1 (NOP_EXPR, ptr_type_node, to->address);
    
    args = tree_cons (NULL_TREE, to->length, NULL_TREE); 
    args = chainon (tree_cons (NULL_TREE, void_address_exp, NULL_TREE), args);
    args = chainon (tree_cons (NULL_TREE, digits, NULL_TREE), args);
    args = chainon (tree_cons (NULL_TREE, exp_from_work, NULL_TREE), args);
    
    call_expr = build (CALL_EXPR, void_type_node, fn_decl_ptr, args, NULL_TREE);
    
  }

  TREE_USED (call_expr) = 1;
  TREE_SIDE_EFFECTS (call_expr) = 1;
  
  expand_expr_stmt (call_expr);
}

/* Output move expression from->to.  We are moving an alphanumeric item
   to a number.  */

static void
build_conversion_alpha_to_numeric (struct exp_details *from,
                                  struct exp_details *to,
                                  struct bli_token_struct *ref_token)
{
  tree newtype2 = NULL_TREE;
  tree call_expr;
  tree result_exp;
  tree to_exp2;
  tree void_address_exp;

  uint32 ix;
  int32 fileno = 0;

  /* Uplift to min 32 bits.  */
  if (to->size < BLI_INT32_WIDTH)
    {
      to->size = BLI_INT32_WIDTH;
      to->signedp = 1;
      newtype2 = integer32_type_node;
    }
  else
    newtype2 = TREE_TYPE (to->exp);

  /* Set up the index to the function to call.  */

  ix = ((to->size == BLI_INT64_WIDTH) << 1) | to->signedp;
  
  /* Build the decl if not done yet.  */

  if (reverse_conversion_fn_decls[ix] == NULL)
    {
      static const uchar *const fnnames[16] =
      {
        (uchar *)"blir_convert_X_32_n",
        (uchar *)"blir_convert_X_32_y",
        (uchar *)"blir_convert_X_64_n",
        (uchar *)"blir_convert_X_64_y"
      };
      tree fn_fn_type;
      tree fn_fn_decl;
      tree fn_identifier;
      tree param_type_list;
      tree decl1;
      
      fn_identifier = get_identifier ((native_char *)fnnames[ix]); 

      /* Types are consed in reverse order.  */

      param_type_list = tree_cons
        (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
      
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);

      fn_fn_type = build_function_type (newtype2, param_type_list);

       (decl1 = build_decl (PARM_DECL, get_identifier ("digits"), 
                       integer32_type_node));
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("length"), 
                       integer32_type_node));
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("address"), 
                       void_pointer_type_node));

      lineno = ref_token->bli_token_lineno;
      fileno = ref_token->bli_token_fileno;
      input_filename = (native_char *)file_names->elem[fileno];

      fn_fn_decl = build_decl
        (FUNCTION_DECL, fn_identifier, fn_fn_type);
                    
      TREE_TYPE (fn_fn_decl) = fn_fn_type;
      DECL_EXTERNAL (fn_fn_decl) = 1;
      TREE_PUBLIC (fn_fn_decl) = 1;
      TREE_STATIC (fn_fn_decl) = 0;
      DECL_ARGUMENTS (fn_fn_decl) = getdecls ();
      DECL_RESULT (fn_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, newtype2);
      DECL_CONTEXT (DECL_RESULT (fn_fn_decl)) = NULL_TREE;
      TREE_USED (fn_fn_decl) = 1;

      rest_of_decl_compilation (fn_fn_decl, NULL, 1, 0);
      make_decl_rtl (fn_fn_decl, NULL);
      
      reverse_conversion_fn_decls[ix] = fn_fn_decl;
      reverse_conversion_fn_types[ix] = fn_fn_type;
    }

  {
    tree args;
    tree fn_decl_ptr;
    tree digits_exp;
    
    fn_decl_ptr = build1
      (ADDR_EXPR, build_pointer_type (reverse_conversion_fn_types[ix]), reverse_conversion_fn_decls[ix]);
    
    void_address_exp = build1 (NOP_EXPR, ptr_type_node, from->address);
    digits_exp = build_int_2 (to->di->digits, 0);
    
    args = tree_cons (NULL_TREE, digits_exp, NULL_TREE); 
    args = chainon (tree_cons (NULL_TREE, from->length, NULL_TREE), args);
    args = chainon (tree_cons (NULL_TREE, void_address_exp, NULL_TREE), args);
    
    call_expr = build (CALL_EXPR, newtype2, fn_decl_ptr, args, NULL_TREE);

    if (newtype2 != TREE_TYPE (to->exp))
      result_exp = build1 (CONVERT_EXPR, TREE_TYPE (to->exp), call_expr);
    else
      result_exp = call_expr;
  }
    
  to_exp2 = build (MODIFY_EXPR, TREE_TYPE (to->exp), to->exp, result_exp);
  TREE_USED (to_exp2) = 1;
  TREE_SIDE_EFFECTS (to_exp2) = 1;
      
  expand_expr_stmt (to_exp2);
}

/* 
   Walk an expression or variable reference EXP, returning expressions
   for the various attributes of the expression into ED.  

   Depending on the nature of the expression not all the attributes are
   set.  Attributes not set are set to NULLs/zeros.
   
   Item source_exp: always filled in.
   
   Item enum_type: filled in if not true expression (used for display, so
   for numeric literals it is text).
   
   Item token_type: the token type if a literal else it is the usage token
   type, but if it is a true expression it is left empty.
   
   Item literal_token: the token if it is or contains a literal.

   Item signedp: 1 if a signed numeric literal or signed data item.

   Item address (tree): filled in if addressable item ie not true expression and
   not address of (see exp).  If numeric constant it is the address of
   a string representation of the numeric literal.
   
   Item exp (tree): always filled.  For non numeric literals it is the
   address expression. Note for 'address of' expressions the value of
   address of x goes into exp, not into address.
   
   Item di: filled in if it is a data item or array reference or address of
   but not if token or true expression.  

   Item level_88_di: filled in if it is a level 88 data item or array reference.  

   Item size: set except for true expressions and occurs depending on.
   
   Item length (tree) set except for true expressions.  
   
   INDEXES if not NULL is an array of expressions for the array
   indexes.  INDEX_COUNT is the number of indexes.  

   If ZERO_NUMERIC_P, treat zero as a number, else as string.
   
*/

static void
get_exp_details (struct exp_details *ed, void *exp, 
                      tree *indexes, uint32 index_count, 
                      enum zero_enum zero_numeric_p)
{
  struct bli_tree_noncons *check_type;
  struct exp_details item1_ed;
  struct exp_details item2_ed;
  struct bli_tree_struct_expression *occurs_item;
  struct bli_tree_struct_expression *data_exp;
  void *occurs_header;
  uchar *work_string;
  void * save_di;

  BLI_ASSERT (exp);
  BLI_ASSERT (ed);

  BLI_ASSERT (!!indexes == !!index_count);

  memset (ed, 0, sizeof (struct exp_details));
  check_type = exp;
  ed->source_exp = exp;

  if (check_type->pfx.cat == type_production)
    {
      BLI_ASSERT ((check_type->pfx.type.tok_type == BLI_PROD_TYPE_DATA_ITEM) || (!indexes));

      switch (check_type->pfx.type.tok_type)
        {
        case BLI_PROD_TYPE_DATA_ITEM :
          ed->di = (struct bli_tree_struct_data_item *)check_type;
          if (ed->di->lvl == 88)
            {
              save_di = ed->di;
              get_exp_details (ed, ed->di->owner, indexes, index_count, zero_numeric_p);
              ed->level_88_di = save_di;
              break;
            }
          ed->exp = access_variable (ed->di, NULL, indexes, index_count);
          ed->address = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (ed->exp)), ed->exp);
          ed->token_type = ed->di->usage_type;
          ed->token = ed->di->reference_token;
          ed->enum_type = get_enum_type (ed->di->usage_type, ed->di->signed_flag);
          ed->size = ed->di->data_length_bytes; 
          
          ed->signedp = ed->di->signed_flag;
          if (ed->di->contains_occurs_depending_on_flag)
            {
              ed->size = 0;
              ed->length = get_dynamic_length (ed->di, NULL);
            }
          else
            {
              ed->size = ed->di->data_length_bytes; 
              ed->length = build_int_2 (ed->di->data_length_bytes, 0); 
              ed->num_size = ed->di->data_length_bytes;
            }
          break;
          
        case BLI_PROD_TYPE_EXPRESSION :
          data_exp = exp;
          ed->token = data_exp->reference_token;
          switch (data_exp->operator)
            {
            case 0:  /* Just the first operand.  */
            case LEVEL_88:
            case SAVE_MAIN:
              BLI_ASSERT (data_exp->operand1);
              get_exp_details (ed, data_exp->operand1, NULL, 0, zero_numeric_p);
              return;
              
            case ARRAY_REFERENCE:  
              ed->di = (void *) data_exp->operand1;
              if (ed->di->lvl == 88)
                {
                  save_di = ed->di;
                  ed->di = (void *) ed->di->owner;
                }
              else
                save_di = NULL;
              ed->signedp = ed->di->signed_flag;
              ed->token_type = ed->di->usage_type;
              ed->enum_type = get_enum_type (ed->di->usage_type, ed->di->signed_flag);
              occurs_item = (void *) data_exp->operand2;
              /* Place the occurs list in reverse order so the elements can be
                 chopped off from bottom to top.  */
              occurs_header = occurs_item;
              reverse_list (& occurs_header);
              ed->exp = access_variable (ed->di, occurs_header, NULL, 0);
              ed->address = build1 (ADDR_EXPR, void_pointer_type_node, ed->exp);
              if (ed->di->contains_occurs_depending_on_flag)
                {
                  ed->size = 0;
                  ed->length = get_dynamic_length (ed->di, NULL);
                }
              else
                {
                  ed->size = ed->di->data_length_bytes; 
                  ed->length = build_int_2 (ed->di->data_length_bytes, 0); 
                  ed->num_size = ed->di->data_length_bytes;
                }
              ed->level_88_di = save_di;
              break;
              
            case ADDRESS_kw :
              /* 
                 Address of data item. 
                 
                 This excludes 'address of' as a receiving item in
                 the set verb, which is not an expression.  That form
                 in handled in the set verb generation.

                 It can be an array reference expression though.  */
              
              BLI_ASSERT (data_exp->operand1);
              ed->di = (struct bli_tree_struct_data_item *)data_exp->operand1;
              ed->signedp = ed->di->signed_flag;
              BLI_ASSERT (ed->di->branch.pfx.cat == type_production);
              if (ed->di->branch.pfx.type.tok_type == BLI_PROD_TYPE_EXPRESSION)
                {
                  get_exp_details (ed, data_exp->operand1, NULL, 0, zero_is_not_numeric);
                  ed->exp = build1 (ADDR_EXPR, void_pointer_type_node, ed->exp);
                  return;
                }

              BLI_ASSERT (ed->di->branch.pfx.type.tok_type == BLI_PROD_TYPE_DATA_ITEM);
              ed->token_type = ed->di->usage_type;
              ed->enum_type = get_enum_type (ed->di->usage_type, ed->di->signed_flag);
              get_exp_details (ed, data_exp->operand1, NULL, 0, zero_is_not_numeric);
              ed->exp = build1 (ADDR_EXPR, void_pointer_type_node, ed->exp);
              return;
                
            case PLUS_sym :
              BLI_ASSERT (data_exp->operand1);
              get_exp_details (& item1_ed, data_exp->operand1, NULL, 0, zero_is_numeric);
              if (data_exp->operand2)
                {
                  get_exp_details (& item2_ed, data_exp->operand2, NULL, 0, zero_is_numeric);
                  ed->exp =
                    build_int_binary_expression (PLUS_EXPR,
                                                item1_ed.exp, 
                                                item2_ed.exp);
                }
              else
                /* Unary +.  */
                ed->exp = item1_ed.exp;
              return;
              
            case MINUS_sym :
              BLI_ASSERT (data_exp->operand1);
              get_exp_details (& item1_ed, data_exp->operand1, NULL, 0, zero_is_numeric);
              if (data_exp->operand2)
                {
                  get_exp_details (& item2_ed, data_exp->operand2, NULL, 0, zero_is_numeric);
                  ed->exp =
                    build_int_binary_expression (MINUS_EXPR, item1_ed.exp, item2_ed.exp);
                }
              else
                {
                  /* Unary -.  */
                  ed->exp = build1 (NEGATE_EXPR, TREE_TYPE (item1_ed.exp), item1_ed.exp);
                }
              return;
              
            case ASTERISK_sym :
              BLI_ASSERT (data_exp->operand1);
              get_exp_details (& item1_ed, data_exp->operand1, NULL, 0, zero_is_numeric);
              BLI_ASSERT (data_exp->operand2);
              get_exp_details (& item2_ed, data_exp->operand2, NULL, 0, zero_is_numeric);
              ed->exp =
                build_int_binary_expression (MULT_EXPR, item1_ed.exp, item2_ed.exp);
              return;
              
            case ASTERISK_ASTERISK_sym :
              BLI_ASSERT (data_exp->operand1);
              get_exp_details (& item1_ed, data_exp->operand1, NULL, 0, zero_is_numeric);
              BLI_ASSERT (data_exp->operand2);
              get_exp_details (& item2_ed, data_exp->operand2, NULL, 0, zero_is_numeric);
              ed->exp =
                build_int_binary_expression_exponentiation (item1_ed.exp, 
                                                           item2_ed.exp,
                                                           data_exp->reference_token);
              return;
              
            case SLASH_sym :
              BLI_ASSERT (data_exp->operand1);
              get_exp_details (& item1_ed, data_exp->operand1, NULL, 0, zero_is_numeric);
              BLI_ASSERT (data_exp->operand2);
              get_exp_details (& item2_ed, data_exp->operand2, NULL, 0, zero_is_numeric);
              ed->exp =
                build_int_binary_expression (TRUNC_DIV_EXPR, item1_ed.exp, item2_ed.exp);
              return;
              
            case ALL :
              BLI_ASSERT (data_exp->operand1);
              BLI_ASSERT (!data_exp->operand2);
              get_exp_details (ed, data_exp->operand1, NULL, 0, zero_numeric_p);
              ed->all_flag = 1;
              return;

            default :
              abort ();
            }
          break;
          
        default :
          abort ();
          
        }
    }
  else /* Token.  */
    {
      struct bli_token_struct *tok;
      tree string_item;
      /* Token as expression ie literal.  */
      BLI_ASSERT (!indexes);
      BLI_ASSERT (check_type->pfx.cat == type_token);
      ed->constant_flag = 1;
      ed->token_type = check_type->pfx.type.tok_type;
      tok = (struct bli_token_struct *)check_type;
      ed->token = tok;
      switch (check_type->pfx.type.tok_type)
        {
        case NULL_word :
          ed->exp = null_pointer_node;
          ed->enum_type = blir_typePointer;
          ed->length = build_int_2 (BLI_POINTER_SIZE / BLI_CHAR_SIZE, 0);
          ed->address = build1 (ADDR_EXPR, void_pointer_type_node, null_pointer_node);
          ed->size = BLI_POINTER_SIZE / BLI_CHAR_SIZE;
          ed->length = build_int_2 (ed->size, 0);
          ed->enum_type = blir_typePointer;
          return;
          
        case SPACE :
        case SPACES :
          work_string = (uchar *)" ";
          goto common_named_literal;
          
        case QUOTE :
        case QUOTES :
          work_string = (uchar *)"\"";
          goto common_named_literal;
          
        case LOW_VALUE :
        case LOW_VALUES :
          work_string = (uchar *)"\x00";
          goto common_named_literal;
          
        case HIGH_VALUE :
        case HIGH_VALUES :
          work_string = (uchar *)"\xff";
          goto common_named_literal;
          
        common_named_literal :
          ed->all_flag = 1;
          ed->length = integer_one_node;
          string_item = build_string (1, (native_char *)work_string);
          TREE_TYPE (string_item) = char_array_type_node;
          ed->enum_type = blir_typeText;
          ed->address = build1 (ADDR_EXPR, char_pointer_type_node, string_item);
          ed->size = 1;
          ed->exp = ed->address;
          ed->first_char = work_string[0];
          return; 

        case GENERIC_STRING :
          ed->length = build_int_2 (tok->string_details->length, 0); 
          string_item = build_string (tok->string_details->length,
                                   (native_char *)tok->string_details->string);
          TREE_TYPE (string_item) = char_array_type_node;
          ed->enum_type = blir_typeText;
          ed->address = build1 (ADDR_EXPR, char_pointer_type_node, string_item);
          ed->size = tok->string_details->length;
          ed->exp = ed->address;
          ed->first_char = tok->string_details->string[0];
          return; 

          
        case ZERO :
        case ZEROS :
        case ZEROES :
          if (!zero_numeric_p)
            {
              work_string = (uchar *)"0";
              goto common_named_literal;
            }
          else
            {
              set_to_numeric_zero (ed);
            }
          break;

        case GENERIC_NUMBER_INTEGER :
          {
            uchar *str;
            uint32 len;
            uint32 error_flag;
            uint64 my_uint64;
            int64  my_int64;
            uint32 lo;
            uint32 hi;
            
            str = (uchar *) tok->string_details->string;
            len = tok->string_details->length;
            ed->enum_type = blir_typeText;
            if ((str[0] == '+') || (str[0] == '-'))
              ed->signedp = 1;
            if (str[0] == '-')
              {
                my_int64 = get_string_signed_long_long_value (len, (uchar *) str,  & error_flag);
                lo = my_int64 & BLI_INT32_MASK;
                hi = (my_int64 >> 32) & BLI_INT32_MASK;
                ed->exp = build_int_2 (lo, hi);
                if ((hi) && (hi != (uint32)-1))
                 {
                   TREE_TYPE (ed->exp) = integer64_type_node;
                   ed->num_size = 8;
                 }
                else
                  {
                    TREE_TYPE (ed->exp) = integer32_type_node;
                    ed->num_size = 4;
                  }
              }
            else
              {
                my_uint64 = get_string_unsigned_long_long_value (len, (uchar *) str,  & error_flag);
                lo = my_uint64 & BLI_INT32_MASK;
                hi = (my_uint64 >> 32) & BLI_INT32_MASK;
                ed->exp = build_int_2 (lo, hi);
                if (hi)
                  {
                    TREE_TYPE (ed->exp) = integer64_unsigned_type_node;
                    ed->num_size = 8;
                  }
                else
                  {
                    TREE_TYPE (ed->exp) = integer32_unsigned_type_node;
                    ed->num_size = 4;
                  }
             }
            BLI_ASSERT (!error_flag);
            string_item = build_string (len, (native_char *)str);
            TREE_TYPE (string_item) = char_array_type_node;
            ed->address = build1 (ADDR_EXPR, char_pointer_type_node, string_item);
            ed->size = len;
            ed->length = build_int_2 (ed->size, 0);
          }
          break;
          
        default :
          abort ();
        }
    }
}

/* Set ED to zero numeric constant.  */
static void
set_to_numeric_zero (struct exp_details* ed)
{
  tree string_item;
  ed->exp = integer_zero_node;
  TREE_TYPE (ed->exp) = integer32_type_node;
  string_item = build_string (1, (native_char *)"0");
  TREE_TYPE (string_item) = char_array_type_node;
  ed->address = build1 (ADDR_EXPR, char_pointer_type_node, string_item);
  ed->size = 1;
  ed->length = integer_one_node;
}

/* Given a usage token type token_type and signed attribute signedp,
   return the member of blir_type which matches it.  */

static uint32 
get_enum_type (uint32 token_type, uint32 signedp)
{
  switch (token_type)
        {
        case BINARY_CHAR :
        case BINARY_SHORT :
        case BINARY_LONG :
        case BINARY_DOUBLE :
          return signedp ? blir_typeBinary : blir_typeUnsigned;
          
        case DISPLAY :
          return blir_typeText;
          
        case POINTER :
          return blir_typePointer;
          
        default :
          abort ();
          break;
        }
  return 0;
}


/* Build the decls for the end of sentences in the
   current_procedure_division.  They will be output later, when we
   come across them in the pass through the procedure division.  They
   have to be done now, because the NEXT SENTENCE constructs need to
   refer to them in advance.  */

static void
build_end_of_sentence_labels 
(struct bli_tree_struct_procedure_division *current_procedure_division)
{
  struct bli_tree_struct_verb_fullstop *current_fullstop;
  static uint32 eos_count = 0;
  for (current_fullstop = current_procedure_division->end_of_sentence_first;
       current_fullstop;
       current_fullstop = current_fullstop->next_to_make_decls)
    {
      uchar eos_name[20];

      /* 1. Build the name.  */

      eos_count++;
      sprintf((native_char*)eos_name, "eos%d", eos_count);

      {
        /* 2. Create the decl.  */
        
        tree id;
        id = get_identifier ((native_char *)eos_name);
        
        current_fullstop->save_decl = build_decl (LABEL_DECL, id, void_type_node);
        
        DECL_CONTEXT ((tree)current_fullstop->save_decl) = fndecl;
        DECL_MODE ((tree)current_fullstop->save_decl) = VOIDmode;
        DECL_SOURCE_LINE ((tree)current_fullstop->save_decl) = 
          current_fullstop->pfx.reference_token->bli_token_lineno; 
        DECL_SOURCE_FILE ((tree)current_fullstop->save_decl) = (native_char *)
          file_names->elem[current_fullstop->pfx.reference_token->bli_token_fileno];
      }
    }
}

/* Build the decls for the sections and paragraphs in the
   current_procedure_division.  They will be output later, when we
   come across them in the pass through the procedure division.  */

static void
build_labels (struct bli_tree_struct_procedure_division *current_procedure_division)
{
  struct bli_tree_struct_label *current_label;
  struct bli_tree_struct_label *current_section;
  struct bli_tree_struct_label *current_section_symtab;
  
  struct bli_tree_struct_label *current_paragraph;

  if (label_name_string_length == 0)
    {
      label_name_string_length = 100;
      label_name_string = BLI_ALLOC (label_name_string_length);
    }
  
  current_section = NULL;
  current_section_symtab = NULL;

  for (current_label = current_procedure_division->label_first;
       current_label;
       current_label = (struct bli_tree_struct_label *)current_label->label_next)
    {
      BLI_ASSERT (current_label->pfx.branch.pfx.cat == type_production);

      switch (current_label->pfx.branch.pfx.type.tok_type)
        {
        case BLI_PROD_TYPE_SECTION :
          current_section = current_label;
          current_paragraph = NULL;
          break;
          
        case BLI_PROD_TYPE_PARAGRAPH :
          current_paragraph = current_label;
          break;
          
        default :
          abort ();
        }
      /* 1. Build the name 'pPARAGRAPHsSECTION' where PARAGRAPH is the
         name of the paragraph (if it's a paragraph) and SECTION is the name of the section
         (if present).  */
      
      {
        uint32 total_length = 1;
        uint32 current_offset = 0;
        
        if (current_paragraph)
          total_length += current_paragraph->pfx.reference_token->string_details->length +1;
        
        if (current_section)
          total_length += current_section->pfx.reference_token->string_details->length +1;
        
        if (total_length > label_name_string_length)
          {
            label_name_string_length = 100 +2 * label_name_string_length;
            label_name_string = BLI_ALLOC (label_name_string_length);
          }
        
        if (current_paragraph)
          {
            label_name_string[current_offset] = 'p';
            current_offset ++;
            memcpy (label_name_string + current_offset, 
                   current_paragraph->pfx.reference_token->string_details->string_upper->string,
                   current_paragraph->pfx.reference_token->string_details->length);
            current_offset += current_paragraph->pfx.reference_token->string_details->length;
          }
        
        if (current_section)
          {
            label_name_string[current_offset] = 's';
            current_offset ++;
            memcpy (label_name_string + current_offset, 
                   current_section->pfx.reference_token->string_details->string_upper->string,
                   current_section->pfx.reference_token->string_details->length);
            current_offset += current_section->pfx.reference_token->string_details->length;
          }
        label_name_string[current_offset] = 0;
      }
      
      {
        /* 2. Create the decl.  */
        
        tree id;
        id = get_identifier ((native_char *)label_name_string);

        current_label->save_decl = build_decl (LABEL_DECL, id, void_type_node);
        
        DECL_CONTEXT ((tree)current_label->save_decl) = fndecl;
        DECL_MODE ((tree)current_label->save_decl) = VOIDmode;
        DECL_SOURCE_LINE ((tree)current_label->save_decl) = 
          current_label->pfx.reference_token->bli_token_lineno; 
        DECL_SOURCE_FILE ((tree)current_label->save_decl) = (native_char *)
          file_names->elem[current_label->pfx.reference_token->bli_token_fileno];
      }
    }
      
}

/* Output function prolog for function current_program within parse
   tree parse_tee_top, creating decl fndecl.  */
  
static void
output_program_prolog (struct bli_tree_struct_parse_tree_top *parse_tree_top ATTRIBUTE_UNUSED,
 struct bli_tree_struct_program_top *current_program)
{
  
  tree arglist;  /* Argument list.  */
  tree functype;  /* Type decl for function.  */
  tree funcid;  /* Function name.  */

  struct bli_token_struct *ref_token;
  int32 fileno = 0;
  struct bli_token_struct *name_token;

  /* Build & initialize declaration node for function.  */
  /* Type is (void) -> int.  */
      
  /* Create function type.  */
      
  /* The void type at the end of the parm list is to show it's not varargs, i think?  */
      
  arglist = tree_cons (NULL_TREE, void_type_node, NULL_TREE);
      
  functype = build_function_type (integer32_type_node, arglist);
      
  /* Create function declaration.  */
      
  if (current_program->identification_division->program_name->as_value)
    {
      name_token = (struct bli_token_struct *)
        current_program->identification_division->program_name->as_value;
      BLI_ASSERT (name_token);
      BLI_ASSERT (name_token->pfx.cat == type_token);
      BLI_ASSERT (name_token->pfx.type.tok_type == GENERIC_STRING);
      funcid = get_identifier ((native_char *)(name_token->string_details->string)); /* \0 is appended.  */
    }
  else
    {
      name_token = current_program->identification_division->program_name->id;
      funcid = get_fixed_identifier (name_token, (uchar *)"", NULL, 0);
    }
  
  ref_token = current_program->reference_token;
  lineno = ref_token->bli_token_lineno;
  fileno = ref_token->bli_token_fileno;
  input_filename = (native_char *)(file_names->elem[fileno]);
  
  fndecl = build_decl (FUNCTION_DECL, funcid, functype);
  
  /* Set current scope to this function.  */
  
  current_function_decl =  fndecl;
  
  TREE_TYPE (fndecl) = functype;
  
  SET_DECL_ASSEMBLER_NAME (fndecl, funcid);
  
  /* Set function definition flag.  */
  
  TREE_STATIC (fndecl) = 1;
  
  /* Set not solely inlineable.  */
  
  TREE_ADDRESSABLE (fndecl) = 1;

  /* Set visible externally.  */
  
  TREE_PUBLIC (fndecl) = 1;
  
  /* Stop -o3 from deleting function.  */
  TREE_USED (fndecl) = 1;
  
  /* Display function name.  */
  announce_function (fndecl);
  
  /* Place in scope list; otherwise it forgets to output the code if
     it was a candidate for inlining.  */
  pushdecl (copy_node (fndecl));
  
  /* Build result declaration.  */
  
  resultdecl = build_decl (RESULT_DECL, NULL_TREE, integer32_type_node);
  TREE_TYPE (resultdecl) = integer32_type_node;
  
  /* Set result scope to the function.  */
  
  DECL_CONTEXT (resultdecl) =  fndecl;
  
  /* Tell the function who his result is.  */
  
  DECL_RESULT (fndecl) = resultdecl;
  
  /* Work out the size ??? is this needed?  */
  
  layout_decl (resultdecl, 0);
  
  /* Set no args.  */
  
  DECL_ARGUMENTS (fndecl) = NULL_TREE;
      
  /* Create rtl for function declaration.  */
  
  rest_of_decl_compilation (fndecl, NULL, 1, 0);
  
  /* Create rtl for function definition.  */
  
  make_decl_rtl (fndecl, NULL);
  
  /* Prepare creation of rtl for a new function.  */
  
  init_function_start (fndecl, (native_char *)input_filename, lineno);  /* Use filename/lineno from above.  */
  
  /* Create rtl for startup code of function, such as saving registers.  */
  
  expand_function_start (fndecl, 0);
  
  /* Function.c requires a push at the start of the function.  that
     looks like a bug to me but let's make it happy ...  */
  
  /* (*lang_hooks.decls.pushlevel) */ pushlevel (0);
  
  /* Create rtl for the start of a new scope.  */
  
  expand_start_bindings (2);
  
  /* Add a new level to the debugger symbol table.  */
  
  /* (*lang_hooks.decls.pushlevel) */ pushlevel (0);
  
  /* Create rtl for the start of a new scope.  */
  
  expand_start_bindings (0);
  
}

/* Output the trailing code for a program current_program within parse
   tree parse_tree_top the function decl is fndecl.  */

static void
output_program_epilog (struct bli_tree_struct_parse_tree_top *parse_tree_top ATTRIBUTE_UNUSED, 
                      struct bli_tree_struct_program_top *current_program)
{
  
  tree block;  /* Block tree returned from poplevel.  */
  tree resultdecl;  /* Result declaration.  */
  tree assign_expr;

  struct bli_token_struct *ref_token;
  int32 fileno = 0;

  resultdecl = DECL_RESULT (fndecl);

  /* Complain if you fall past the end of the code 
     except in main where you return 0;.  */
  
  if (current_program->program_is_main)
    {
      /* Build tree for "result = 0.  */
      assign_expr = build (MODIFY_EXPR, 
                           integer32_type_node, 
                           resultdecl, integer_zero_node);
      TREE_SIDE_EFFECTS (assign_expr) = 1;
      TREE_USED (assign_expr) = 1;
      expand_expr_stmt (assign_expr);
      /* Emit rtl for return from this function, this is very
         important, because mere assignments to the 'result' variable
         will be optimized away by data flow analysis.  */
      expand_return (resultdecl);
    }
  else
    call_void_function_no_args 
      ((uchar *)"blir_error_drop_off_end",
       current_program->reference_token);
  
  /* Pop inner level.  */
  
  block = /* (*lang_hooks.decls.poplevel) */ poplevel  (1, 1, 0);
  
  /* Emit rtl for end of scope.  */
  
  expand_end_bindings (block, 1, 1);
  
  /* Cleanup tail.  */
  /* Get completely built level from debugger symbol table.  */
  
  block = /* (*lang_hooks.decls.poplevel) */ poplevel  (1, 0, 1);
  
  /* And attach it to the function.  */
  
  DECL_INITIAL (fndecl) = block;
  
  /* Emit rtl for end of scope.  */
  
  expand_end_bindings (block, 0, 1);
  
  /* Emit rtl for end of function.  */
  
  ref_token = current_program->reference_token;
  lineno = ref_token->bli_token_lineno;
  fileno = ref_token->bli_token_fileno;
  input_filename = (native_char *)file_names->elem[fileno];
  
  expand_function_end ((native_char *)input_filename, lineno, 0);
  
  /* Call optimization and convert optimized rtl to assembly code.  */
  
  rest_of_compilation (fndecl);
  
  /* We are not inside of any scope now.  */
  
  current_function_decl = NULL_TREE;

  /* Output the blir_main_address variable.  */

  if (current_program->program_is_main)
    {
      tree function_type;
      tree item_type;
      tree item_decl;
      tree item_id;
      tree fn_decl_ptr;

      function_type = build_function_type (integer_type_node, NULL_TREE);

      item_type = build_pointer_type (function_type);
      
      item_id = get_identifier ("blir_main_address");
      
      item_decl = build_decl (VAR_DECL, item_id, item_type);
      
      layout_decl (item_decl, 0);
      
      fn_decl_ptr = build1
        (ADDR_EXPR, build_pointer_type (TREE_TYPE (fndecl)), fndecl);

      DECL_INITIAL (item_decl) = fn_decl_ptr;
      
      DECL_CONTEXT (item_decl) = NULL_TREE;
        
      TREE_STATIC (item_decl) = 1; 
      TREE_PUBLIC(item_decl) = 1; 
      
      TREE_USED (item_decl) = 1; 

      DECL_SOURCE_LINE (item_decl) = lineno;
      DECL_SOURCE_FILE (item_decl) = input_filename;

      rest_of_decl_compilation (item_decl, 0, 1, 0);  
      get_pending_sizes ();
      /* pushdecl (copy_node (item_decl));  */
    }
}

/* 
   Output code for elementary data item current_data_item.  */
  
static void
output_elementary_data_item (struct bli_tree_struct_data_item *current_data_item, 
                        void **caller_link_header)
{

  tree item_id;
  tree item_type;
  tree item_type_decl;
  tree item_decl;
  tree base_type;
  tree base_id;
  tree type_for_this_size;
  tree type_for_length;
  void *old_first;
  struct bli_token_struct *ref;
  int32 top_level = 0;
  int32 definition = 0;
  
  ref = current_data_item->reference_token;
  VERIFY_TYPE (ref, type_token, 0, 0);

  lineno = ref->bli_token_lineno;
  input_filename = (native_char *)file_names->elem[ref->bli_token_fileno];

  if (caller_link_header)
    { /* Group member.  */
      current_data_item->includes_non_display = (current_data_item->usage_type != DISPLAY);
      
      BLI_ASSERT (caller_link_header);

      item_type = get_item_type (current_data_item);
  
      item_id = get_fixed_identifier (current_data_item->data_name_token, (uchar *)"",
                                      & current_data_item, current_data_item->duplicate_notfirst_flag);
      
      item_type_decl = build_decl (FIELD_DECL, item_id, item_type);

      layout_decl (item_type_decl, 0);

      if (current_data_item->occurs_details)
        type_for_length = TREE_TYPE (item_type);
      else
        type_for_length = item_type;
      
      current_data_item->data_length_bytes
        = get_uint32_value (TYPE_SIZE (type_for_length), LOG_BLI_BITS_PER_BYTE);
      BLI_ASSERT (current_data_item->data_length_bytes);

      current_data_item->save_decl = item_type_decl;
      
      /* Insert in reverse order into parent.  */
      old_first = * caller_link_header;
      * caller_link_header = current_data_item->save_decl;
      TREE_CHAIN ((tree)(current_data_item->save_decl)) = old_first;
      
    }
  else
    { /* Self standing item.  */
        
      current_data_item->includes_non_display = (current_data_item->usage_type != DISPLAY);

      /* Build the type.  */
      
      BLI_ASSERT (!caller_link_header);

      item_type = get_item_type (current_data_item);
      
      if (current_data_item->external_details &&
          current_data_item->external_details->alt_data_name_token)
        item_id = get_identifier
          ((native_char *) current_data_item->external_details->alt_data_name_token->string_details->string);
        else
          item_id = get_fixed_identifier (current_data_item->data_name_token, (uchar *)
                                          ((current_data_item->based_flag) ? "deref" : ""),
                                          & current_data_item, current_data_item->duplicate_notfirst_flag); 
      
      if (current_data_item->based_flag)
        {
          base_type = item_type;
          base_id = item_id;
          item_type = build_pointer_type (base_type);
          item_id = get_fixed_identifier (current_data_item->data_name_token, (uchar *)"",
                                       & current_data_item, current_data_item->duplicate_notfirst_flag); 
          TYPE_NAME (item_type) = item_id;
          TYPE_STUB_DECL (item_type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, item_type));
          layout_type (item_type);
          type_for_this_size = base_type;
          rest_of_type_compilation (item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);
        }
      else
        type_for_this_size = item_type;

      item_decl = build_decl (VAR_DECL, item_id, item_type);
      current_data_item->save_decl = item_decl;
      
      layout_decl (item_decl, 0);
      
      current_data_item->data_length_bytes = get_uint32_value (TYPE_SIZE (type_for_this_size),
                                                            LOG_BLI_BITS_PER_BYTE);
      BLI_ASSERT (current_data_item->data_length_bytes);

      DECL_CONTEXT (item_decl) = current_function_decl;
      
      TREE_STATIC (item_decl) = !current_data_item->in_dynamic_storage;
      
      /* If used ...  lll;.  */
      TREE_USED (item_decl) = 1;
      
      if (current_data_item->external_details)
        {
          struct bli_tree_struct_external *ext;
          struct bli_token_struct *lo;
          
          ext = current_data_item->external_details;
          lo = ext->language_option;
          
          top_level = 1;
          DECL_CONTEXT (item_decl) = NULL_TREE;

          if (!lo)
            {
              /* BLISS type.  */
              TREE_PUBLIC(item_decl) = 1; 
              DECL_COMMON(item_decl) = 1;
            }
          else 
            {
              /* C type.  */
              struct interned_token_string *st;
              st = lo->string_details->string_upper;
              if ((st->length = 12) && !memcmp (st->string, "C-DEFINITION", 12))
                {
                  definition = 1;
                  TREE_PUBLIC(item_decl) = 1; 
                  TREE_STATIC (item_decl) = 1;
                }
              else 
                if ((st->length = 11) && !memcmp (st->string, "C-REFERENCE", 11))
                  {
                    DECL_EXTERNAL(item_decl) = 1;
                  }
                else
                  abort (); /* Type not known - parser should have stopped this.  */
            }
        }
      
      DECL_SOURCE_LINE (item_decl) = current_data_item->reference_token->bli_token_lineno;
      DECL_SOURCE_FILE (item_decl) = (native_char *)
        (file_names->elem[current_data_item->reference_token->bli_token_fileno]);

      if (definition)
        {
          /* Initialize it with anything just so it is initialized.  */
	  DECL_INITIAL (item_decl) = build (CONSTRUCTOR, type_for_this_size, NULL_TREE, NULL_TREE);
          TREE_TYPE (DECL_INITIAL (item_decl)) = item_type;
        }

      /* Expand declaration and initial value if any.  */
      
      if (TREE_STATIC (item_decl) || current_data_item->external_details)
        rest_of_decl_compilation (item_decl, 0, top_level, 0);
      else
        expand_decl (item_decl);
      
      /* If (TREE_STATIC(item_decl)).  */
      if (!current_data_item->external_details)
        pushdecl (copy_node (item_decl));
      
      current_data_item->save_decl = item_decl;
    }

  if (current_data_item->value_list)
    link_deferred_init (current_data_item);
  
  check_level88_list (current_data_item);
  
}

/* Create the type for elementary data item current_data_item.  */

static tree
get_item_type (struct bli_tree_struct_data_item *current_data_item)
{
  tree item_max_value;
  tree item_index_type;
  tree item_type;
  tree group_item_type = NULL_TREE;
  tree group_item_type_decl;
  tree field_decl;
  tree field_id;
  tree provisional_item_type = NULL_TREE;
      
  switch (current_data_item->usage_type)
    {
    case BINARY_CHAR :
    case BINARY_SHORT :
    case BINARY_LONG :
    case BINARY_DOUBLE :
      provisional_item_type = type_for_fixed_length_type (current_data_item->usage_type,
                                           !current_data_item->signed_flag);
      break;
              
    case DISPLAY :

      /* Display is done as a struct containing an array of chars.  */

      /* Create the type for the array.  */
      item_max_value = build_int_2 (current_data_item->data_length_bytes, 0);
      item_index_type = build_index_2_type (integer_one_node, item_max_value);
      item_type = build_array_type (char_type_node, item_index_type);
      TREE_TYPE (item_type) = char_type_node; 

      /* Create the field.  */
      field_id = get_fixed_identifier (current_data_item->data_name_token, (uchar *)"char",
                                    & current_data_item, current_data_item->duplicate_notfirst_flag);
      field_decl = build_decl (FIELD_DECL, field_id, item_type);
      DECL_FIELD_CONTEXT (field_decl) = group_item_type;
      layout_decl (field_decl, 0);  /* Not needed ???  */
      BLI_ASSERT (get_uint32_value (TYPE_SIZE (TREE_TYPE (field_decl)), LOG_BLI_BITS_PER_BYTE) != 0);
      DECL_NAME (field_decl) = get_fixed_identifier
        (current_data_item->data_name_token, 
         (uchar *)(current_data_item->occurs_details ? "" : "item"),
         & current_data_item, current_data_item->duplicate_notfirst_flag);

      /* Create the group type.  */
      group_item_type = make_node (RECORD_TYPE); 
      TYPE_FIELDS (group_item_type) = field_decl; 
      DECL_FIELD_CONTEXT (field_decl) = group_item_type; /* Field is in context of record.  */
      TYPE_NAME (group_item_type) = get_fixed_identifier
        (current_data_item->data_name_token,
         (uchar *)(current_data_item->occurs_details ? "" : "item"),
         & current_data_item, current_data_item->duplicate_notfirst_flag);
  
      group_item_type_decl = make_node (TYPE_DECL);

      TREE_TYPE (group_item_type_decl) = group_item_type;
      
      /* TYPE_STUB_DECL(group_item_type)=group_item_type_decl;.  */
      TYPE_STUB_DECL (group_item_type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, group_item_type));

      /* Complete the type.  */
      
      layout_type (group_item_type);
      BLI_ASSERT (get_uint32_value (TYPE_SIZE (group_item_type), LOG_BLI_BITS_PER_BYTE) != 0);
      rest_of_type_compilation (group_item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);

      provisional_item_type = group_item_type;
      break;
      
    case POINTER :
      provisional_item_type = void_pointer_type_node;
      break;

    default : 
      BLI_ASSERT (0 && "invalid usage type");
    }

  if (current_data_item->occurs_details)
    return get_simple_array (provisional_item_type, 
                             1,
                             current_data_item->occurs_details->occurs_to_int,
                             get_fixed_identifier
                             (current_data_item->data_name_token, (uchar *)"array",
                              & current_data_item, current_data_item->duplicate_notfirst_flag));

  /* Else ...  but gcc complains if the return is conditional.  */
  return provisional_item_type;
}
          

/* 
   Output code for group data item current_data_item,  */
  
static void
output_group_item (struct bli_tree_struct_data_item *current_data_item, 
                         void **caller_link_header)
{
  struct bli_tree_struct_data_item *member_data_item;
  tree item_type = NULL;
  tree array_item_type = NULL;
  tree item_id = NULL;
  tree item_decl = NULL;
  tree item_type_decl = NULL;
  tree true_item_type_decl = NULL;
  tree item_max_value = NULL;
  tree item_index_type = NULL;
  tree array_item_type_decl = NULL;
  tree true_item_type = NULL;
  tree base_type;
  tree base_id;
  tree type_for_length;
  void *old_first;
  int32 top_level = 0;
  int32 definition = 0;
  
  BLI_ASSERT ((current_data_item->group_flag) || (current_data_item->redefine_master_flag));
  
  lineno = current_data_item->reference_token->bli_token_lineno;
  input_filename = (native_char *)file_names->elem[current_data_item->reference_token->bli_token_fileno];

  if (caller_link_header) /* Included within something else.  */
    {
      item_type = make_node ((current_data_item->group_flag) ? RECORD_TYPE : UNION_TYPE);
      TREE_TYPE (item_type) = item_type;  /* ???  */
        
      item_id = get_fixed_hierarchy_identifier
        (current_data_item, (uchar *)"item");
      TYPE_NAME (item_type) = item_id;
      
      item_type_decl = build_decl (FIELD_DECL, item_id, item_type);

      DECL_CONTEXT (item_type_decl) = current_function_decl;
      
      TREE_TYPE (item_type_decl) = item_type;
      
      TYPE_FIELDS (item_type) = NULL; 

      for (member_data_item = (struct bli_tree_struct_data_item *)current_data_item->branch.child;
           member_data_item;
           member_data_item = (struct bli_tree_struct_data_item *)member_data_item->branch.next
           )
        {
          if ((member_data_item->group_flag) || (member_data_item->redefine_master_flag))
            output_group_item (member_data_item, (void **) & TYPE_FIELDS (item_type));
          else
            output_elementary_data_item (member_data_item, (void **) & TYPE_FIELDS (item_type));
          
          current_data_item->includes_non_display |= member_data_item->includes_non_display;
          DECL_FIELD_CONTEXT ((tree)member_data_item->save_decl) = item_type_decl;
        }
      
      /* Reverse the field type list so it is now in the correct order.  */
      TYPE_FIELDS (item_type) = nreverse (TYPE_FIELDS (item_type));

      /* Place a copy of the type declaration into symbol table.  */
      
      TREE_TYPE (item_type) = item_type;  /* Dbx needs these just so.  */
      TYPE_NAME (item_type) = item_id;
  
      /* TYPE_STUB_DECL (item_type) = item_type_decl; xxx; */
      TYPE_STUB_DECL (item_type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, item_type));

      /* Complete the type.  */

      layout_type (item_type);
      rest_of_type_compilation (item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);

      true_item_type_decl = item_type_decl;
      true_item_type = item_type;
      type_for_length = true_item_type;

      if (current_data_item->occurs_details)
        {
          item_max_value = build_int_2 (current_data_item->occurs_details->occurs_to_int, 0);
          item_index_type = build_index_2_type (integer_one_node, item_max_value);
          array_item_type = build_array_type (item_type, item_index_type);
          TYPE_NAME (array_item_type) = get_fixed_identifier
            (current_data_item->data_name_token, (uchar *)"array",
             & current_data_item, current_data_item->duplicate_notfirst_flag);
          
          array_item_type_decl = build_decl
            (FIELD_DECL, 
             get_fixed_hierarchy_identifier (current_data_item, (uchar *)"item_decl"), 
             array_item_type);
          
          TREE_TYPE (array_item_type_decl) = array_item_type;
      
          TREE_TYPE (array_item_type_decl) = array_item_type;
          TYPE_STUB_DECL (array_item_type) = array_item_type_decl;

          layout_type (array_item_type);
          BLI_ASSERT (get_uint32_value (TYPE_SIZE (array_item_type), LOG_BLI_BITS_PER_BYTE) != 0);
          rest_of_type_compilation (array_item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);
          true_item_type_decl = array_item_type_decl;
          true_item_type = array_item_type;
        }

      current_data_item->data_length_bytes = get_uint32_value (TYPE_SIZE (type_for_length),
                                                            LOG_BLI_BITS_PER_BYTE);
      BLI_ASSERT (current_data_item->data_length_bytes);

      /* Insert in reverse order into parent's list.  */
      old_first = * caller_link_header;
      * caller_link_header = true_item_type_decl;
      TREE_CHAIN (true_item_type_decl) = old_first;

      current_data_item->save_decl = true_item_type_decl;
    }
  else /* No caller link ie top level.  */
    {

      BLI_ASSERT (!caller_link_header);

      item_type = make_node ((current_data_item->group_flag) ? RECORD_TYPE : UNION_TYPE);

      item_type_decl = make_node (TYPE_DECL);
      TREE_TYPE (item_type) = item_type;  /* ???.  */
      TREE_TYPE (item_type_decl) = item_type;

      if (current_data_item->external_details &&
          current_data_item->external_details->alt_data_name_token)
        item_id = get_identifier
          ((native_char *) current_data_item->external_details->alt_data_name_token->string_details->string);
      else
        item_id = get_fixed_identifier (current_data_item->data_name_token, (uchar *)
                                        ((current_data_item->based_flag) ? "deref" : ""),
                                        & current_data_item, current_data_item->duplicate_notfirst_flag);

      TYPE_NAME (item_type) = item_id;

      TYPE_FIELDS (item_type) = NULL;
  
      /* Pass 1 - build the field chain.  */
      for (member_data_item = (struct bli_tree_struct_data_item *)current_data_item->branch.child;
           member_data_item;
           member_data_item = (struct bli_tree_struct_data_item *)member_data_item->branch.next
           )
        {
          if ((member_data_item->group_flag) || (member_data_item->redefine_master_flag))
            output_group_item (member_data_item, (void **) & TYPE_FIELDS (item_type));
          else
            output_elementary_data_item (member_data_item, (void **) & TYPE_FIELDS (item_type));

          current_data_item->includes_non_display |= member_data_item->includes_non_display;
          DECL_FIELD_CONTEXT ((tree)member_data_item->save_decl) = item_type_decl;
        }

      /* Reverse the field type list so it is now in the correct order and
         save in type_fields.  */
      {
        void *temp;
        temp = nreverse (TYPE_FIELDS (item_type));
        TYPE_FIELDS (item_type) = temp;
      }
  
      true_item_type = item_type;

      /* Build the declaration of the variable.  */
  
      /* Set up for debug.  */
      
      TYPE_STUB_DECL (true_item_type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, true_item_type));

      /* Complete the type.  */
      layout_type (true_item_type);
      rest_of_type_compilation (true_item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);
      type_for_length = true_item_type;

      if (current_data_item->based_flag)
        {
          base_type = true_item_type;
          base_id = item_id;
          true_item_type = build_pointer_type (base_type);
          item_id = get_fixed_identifier (current_data_item->data_name_token, (uchar *)"",
                                       & current_data_item, current_data_item->duplicate_notfirst_flag); 
          TYPE_NAME (true_item_type) = item_id;
          TYPE_STUB_DECL (true_item_type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, true_item_type));
          layout_type (true_item_type);
          BLI_ASSERT (get_uint32_value (TYPE_SIZE (true_item_type), LOG_BLI_BITS_PER_BYTE) != 0);
          rest_of_type_compilation (true_item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);
        }
      
      current_data_item->data_length_bytes = get_uint32_value (TYPE_SIZE (type_for_length), 
                                                            LOG_BLI_BITS_PER_BYTE);
      BLI_ASSERT (current_data_item->data_length_bytes);

      item_decl = build_decl (VAR_DECL, item_id, true_item_type);
      current_data_item->save_decl = item_decl;

      DECL_CONTEXT (item_decl) = current_function_decl;
      
      TREE_STATIC (item_decl) = !current_data_item->in_dynamic_storage;
      
      if (current_data_item->external_details)
        {
          struct bli_tree_struct_external *ext;
          struct bli_token_struct *lo;
          
          ext = current_data_item->external_details;
          lo = ext->language_option;
          
          top_level = 1;
          DECL_CONTEXT (item_decl) = NULL_TREE;

          if (!lo)
            {
              /* BLISS type.  */
              TREE_PUBLIC(item_decl) = 1; 
              DECL_COMMON(item_decl) = 1;
            }
          else 
            {
              /* C type.  */
              struct interned_token_string *st;
              st = lo->string_details->string_upper;
              if ((st->length = 12) && !memcmp (st->string, "C-DEFINITION", 12))
                {
                  definition = 1;
                  TREE_PUBLIC(item_decl) = 1; 
                  TREE_STATIC (item_decl) = 1;
                }
              else 
                if ((st->length = 11) && !memcmp (st->string, "C-REFERENCE", 11))
                  {
                    DECL_EXTERNAL(item_decl) = 1;
                  }
                else
                  abort (); /* Type not known - parser should have stopped this.  */
            }
        }
      
      DECL_SOURCE_LINE (item_decl) = current_data_item->reference_token->bli_token_lineno;
      DECL_SOURCE_FILE (item_decl) = (native_char *)
        (file_names->elem[current_data_item->reference_token->bli_token_fileno]);

      TREE_TYPE (item_decl) = true_item_type;
  
      /* If used ...  lll;.  */
      TREE_USED (item_decl) = 1;
  
      /* Lll; multiple external c-references to an external in one
         file may produce an error.  */
      if (definition)
        {
          /* Initialize it with anything just so it is initialized.  */
	  DECL_INITIAL (item_decl) = build (CONSTRUCTOR, true_item_type, NULL_TREE, NULL_TREE);
          TREE_TYPE (DECL_INITIAL (item_decl)) = item_type;
        }

      /* Expand declaration.  */
  
      if (TREE_STATIC (item_decl) || current_data_item->external_details)
        rest_of_decl_compilation (item_decl, 0, top_level, 0);
      else
        expand_decl (item_decl);
      
      /* If (TREE_STATIC(item_decl)).  */
      if (!current_data_item->external_details)
        pushdecl (copy_node (item_decl));
      
      /* Current_data_item->save_decl=item_decl;.  */
      
    }
  
  if (current_data_item->value_list)
    {
      check_value_size (current_data_item);
      link_deferred_init (current_data_item);
      check_level88_list (current_data_item);
    }

  return;
}
  
/* Check that the value ranges for CURRENT_DATA_ITEM are valid ie that
   the low range is less than or equal to the high range.  The standard
   says that it must be less than but we only check for less than or
   equal to.  */

static void
check_level88_list (struct bli_tree_struct_data_item *current_data_item)
{
  struct bli_tree_struct_value_item *vi1;
  struct bli_tree_struct_value_item *vi2;
  struct bli_tree_struct_data_item *di1;
  for (di1 = (void *) current_data_item->level88; di1; di1 = (void *) di1->branch.next)
    {
      for (vi1 = (void *) di1->value_list; vi1; vi1 = (void *) vi1->cons.next)
        {
          vi2 = vi1->second_value;
          if (!vi2)
            continue;
          
          switch (vi1->value_type)
            {
            case VALUE_TYPE_STRING :
              BLI_ASSERT (vi1->value_type == vi2->value_type);
              check_compare_string_values (current_data_item, vi1, vi2);
              break;
                  
            case VALUE_TYPE_SIGNED :
            case VALUE_TYPE_LONG_LONG_SIGNED :
            case VALUE_TYPE_UNSIGNED :
            case VALUE_TYPE_LONG_LONG_UNSIGNED :
              check_compare_numeric_values (current_data_item, vi1, vi2);
              break;
                  
            case VALUE_TYPE_POINTER :
              BLI_ASSERT (vi1->value_type == vi2->value_type);
              /* Nothing can go wrong.  */
              break;
                  
            default :
              abort ();
            }
        }
    }
}

/* Check that the value ranges VI1 and VI2 for CURRENT_DATA_ITEM are valid ie that
   the low range is less than or equal to the high range.  The standard
   says that it must be less than but we only check for less than or
   equal to. The values are strings.  */

static void
check_compare_string_values (struct bli_tree_struct_data_item *current_data_item,
                            struct bli_tree_struct_value_item *vi1,
                            struct bli_tree_struct_value_item *vi2)
{
  uint32 char_ix;
  /* Using max length of data item ie as required for initial values.  */
  for (char_ix = 0; char_ix < current_data_item->data_length_bytes; char_ix ++)
    {
      uchar c1;
      uchar c2;
      c1 = vi1->string[char_ix % vi1->length];
      c2 = vi2->string[char_ix % vi2->length];
      if (c2 < c1)
        MSG_TOKEN (MSGID (194), current_data_item->reference_token, NULL);
    }
}


/* Check that the value ranges VI1 and VI2 for CURRENT_DATA_ITEM are valid ie that
   the low range is less than or equal to the high range.  The standard
   says that it must be less than but we only check for less than or
   equal to. The values are numbers.  */

static void
check_compare_numeric_values (struct bli_tree_struct_data_item *current_data_item ATTRIBUTE_UNUSED,
                             struct bli_tree_struct_value_item *vi1,
                             struct bli_tree_struct_value_item *vi2)
{

  /* Fix up for ALL value.  ALL is only allowed with unsigned values.  */
  BLI_ASSERT (!vi1->value_all);
  BLI_ASSERT (!vi2->value_all);
      
  switch (vi1->value_type)
    {
    case VALUE_TYPE_SIGNED :
    case VALUE_TYPE_LONG_LONG_SIGNED :
      
      if ((vi2->value_type == VALUE_TYPE_UNSIGNED)
         || (vi2->value_type == VALUE_TYPE_LONG_LONG_UNSIGNED))
        {
          if (vi1->value_number.value_signed > 0 &&
              ((uint64)vi1->value_number.value_signed) > vi2->value_number.value_unsigned)
            {
              MSG_TOKEN (MSGID (194), vi2->value_tok, NULL);
            }
        }
      else
        {
          if (vi1->value_number.value_signed > vi2->value_number.value_signed)
            {
              MSG_TOKEN (MSGID (194), vi2->value_tok, NULL);
            }
        }
      break;
      
    case VALUE_TYPE_UNSIGNED :
    case VALUE_TYPE_LONG_LONG_UNSIGNED :
      if ((vi2->value_type == VALUE_TYPE_SIGNED)
         || (vi2->value_type == VALUE_TYPE_LONG_LONG_SIGNED))
        {
          if (vi2->value_number.value_signed > 0 &&
              ((uint64)vi2->value_number.value_signed) < vi1->value_number.value_unsigned)
            {
              MSG_TOKEN (MSGID (194), vi2->value_tok, NULL);
            }
        }
      else
        {
          if (vi1->value_number.value_unsigned > vi2->value_number.value_unsigned)
            {
              MSG_TOKEN (MSGID (194), vi2->value_tok, NULL);
            }
        }
      break;

    default :
      abort ();
    }
}

/* Link data item CURRENT_DATA_ITEM into the right deferred init
   queue.  These queues exist because of the difficulty in coaxing gcc
   to do various types if init eg at group level, Also to support init
   of large arrays and large items without large amounts of
   storage being needed eg 01 var1 pic x(1000000) value all "*".  */

static void
link_deferred_init (struct bli_tree_struct_data_item *current_data_item)
{
  struct deferred_init_details *ptr;

  /* These are allocated without protection from gc as there is no gc
     while they are in use.  */
  ptr = BLI_ALLOC (sizeof (struct deferred_init_details));
  ptr->di = current_data_item;
  if (current_data_item->in_dynamic_storage)
    {
      if (dynamic_init_first)
        {
          dynamic_init_last->next = ptr;
          dynamic_init_last = ptr;
        }
      else
        {
          dynamic_init_first = ptr;
          dynamic_init_last = ptr;
        }
    }
  else
    {
      if (static_init_first)
        {
          static_init_last->next = ptr;
          static_init_last = ptr;
        }
      else
        {
          static_init_first = ptr;
          static_init_last = ptr;
        }
    }
}

/* Check the size of the value given for a data item
   CURRENT_DATA_ITEM which is a group.  */

static void
check_value_size (struct bli_tree_struct_data_item *current_data_item)
{
  struct bli_tree_struct_value_item *val;
  BLI_ASSERT (current_data_item->data_length_bytes);

  val = current_data_item->value_list;
  BLI_ASSERT (val);
  if (val->length > current_data_item->data_length_bytes)
    {
      MSG_TOKEN (MSGID (125), val->value_tok, NULL);
    }
}

/* Output variables for the deferred init queues and other work variables for this pgm part1.  */

static void
procedure_init_part1 (struct bli_tree_struct_procedure_division *current_procedure_division)
{
  tree item_value;
  tree item_decl;
  tree sequ_array_type;
  tree ptr_array_type;

  /* Create the first call flag.  */
  
  first_time_item_decl = build_decl (VAR_DECL, get_identifier ("first_time_flag"), integer32_type_node);
  layout_decl (first_time_item_decl, 0);
  DECL_CONTEXT (first_time_item_decl) = current_function_decl;
  
  item_value = integer_one_node;
  TREE_TYPE ((tree)item_value) = integer32_type_node;
  DECL_INITIAL (first_time_item_decl) = item_value;
  
  TREE_STATIC (first_time_item_decl) = 1;
  TREE_USED (first_time_item_decl) = 1;
  
  /* Expand declaration and initial value if any.  */
  
  rest_of_decl_compilation (first_time_item_decl, 0, 0, 0);
  pushdecl (copy_node (first_time_item_decl));
  
  if (current_procedure_division->exit_label_count)
    {
      /* Create the counter.  */
      
      item_decl = build_decl (VAR_DECL, get_identifier ("call_count"), integer32_type_node);
      layout_decl (item_decl, 0);
      DECL_CONTEXT (item_decl) = current_function_decl;
      
      item_value = build_int_2 (0, 0);
      TREE_TYPE ((tree)item_value) = integer32_type_node;
      DECL_INITIAL (item_decl) = item_value;
      
      TREE_STATIC (item_decl) = 1;
      TREE_USED (item_decl) = 1;
      
      /* Expand declaration and initial value if any.  */
      
      rest_of_decl_compilation (item_decl, 0, 0, 0);
      pushdecl (copy_node (item_decl));
      
      saved_call_count_decl = item_decl;
      
      /* Create array of sequences, one for each exit location, and a
         return address storage for each.  */
      
      sequ_array_type = get_simple_array (integer32_type_node,
                                          0,
                                          current_procedure_division->exit_label_count - 1,
                                          get_identifier ("perform_sequ_array_type"));
      
      item_decl = build_decl (VAR_DECL, get_identifier ("perform_sequ_array"), sequ_array_type);
      layout_decl (item_decl, 0);
      DECL_CONTEXT (item_decl) = current_function_decl;
      TREE_STATIC (item_decl) = 1;
      TREE_USED (item_decl) = 1;
      rest_of_decl_compilation (item_decl, 0, 0, 0);
      pushdecl (copy_node (item_decl));
      
      saved_perform_sequ_array_decl = item_decl;
      
      ptr_array_type = get_simple_array (void_pointer_type_node, 0,
                                         current_procedure_division->exit_label_count - 1,
                                         get_identifier ("perform_ptr_array_type"));
      item_decl = build_decl (VAR_DECL,  get_identifier ("perform_ptr_array"), ptr_array_type);
      layout_decl (item_decl, 0);
      DECL_CONTEXT (item_decl) = current_function_decl;
      TREE_STATIC (item_decl) = 1;
      TREE_USED (item_decl) = 1;
      rest_of_decl_compilation (item_decl, 0, 0, 0);
      pushdecl (copy_node (item_decl));
      saved_perform_ptr_array_decl = item_decl;
    }
}

/* Output init code for the deferred init queues and perform data for this pgm part 2.  */

static void
procedure_init_part2 (struct bli_tree_struct_procedure_division *current_procedure_division)
{
  
  struct deferred_init_details *current_init_details;
  tree condition;

  /* Static inits.  */
  
  if (current_procedure_division->exit_label_count)
    {
      /* Expand if (call count == 0).  */
      condition = build (EQ_EXPR, integer32_type_node, 
                         build (POSTINCREMENT_EXPR, integer32_type_node, 
                                saved_call_count_decl, 
                                build1 (CONVERT_EXPR, integer32_type_node,
                                        integer_one_node)),
                         build1 (CONVERT_EXPR, integer32_type_node,
                                 integer_zero_node));
      expand_start_cond (condition, 0);
    }
  
  /* Expand if (first_time) - call count could have rolled over.  */
  
  condition = build (NE_EXPR, integer32_type_node, first_time_item_decl, integer_zero_node);
  expand_start_cond (condition, 0);
  
  /* Output the inits.  */
  
  for (current_init_details = static_init_first;
       current_init_details;
       current_init_details = current_init_details->next)
    one_deferred_init (current_init_details->di);
  
  /* Set first_time_flag to 0.  */
  
  {
    tree stmt;
    stmt = build (MODIFY_EXPR, TREE_TYPE (first_time_item_decl), first_time_item_decl,
                  build1 (CONVERT_EXPR, TREE_TYPE (first_time_item_decl), integer_zero_node));
    TREE_USED (stmt) = 1;
    TREE_SIDE_EFFECTS (stmt) = 1;
    expand_expr_stmt (stmt);
  }
  
  /* Expand end-if.  */
  expand_end_cond ();
  
  /* Zero the perform return arrays if wraparound.  */
  
  if (current_procedure_division->exit_label_count)
    {
      /* Init the perform arrays.  */
      output_memset (build1 (ADDR_EXPR, 
                             void_pointer_type_node,
                             saved_perform_sequ_array_decl),
                     integer_zero_node, 
                     bliss_sizeof(TREE_TYPE (saved_perform_sequ_array_decl)));

      output_memset (build1 (ADDR_EXPR, 
                             void_pointer_type_node,
                             saved_perform_ptr_array_decl),
                     integer_zero_node, 
                     bliss_sizeof(TREE_TYPE (saved_perform_sequ_array_decl)));
      
      /* Expand end-if.  */
      expand_end_cond ();
    }

  /* Local storage inits execute always.  */
  for (current_init_details = dynamic_init_first;
       current_init_details;
       current_init_details = current_init_details->next)
    {
      one_deferred_init (current_init_details->di);
    }
}

/* Return size in bytes of TYPE.  */

static tree 
bliss_sizeof (tree type)
{
  tree size;
  size = size_binop (CEIL_DIV_EXPR, TYPE_SIZE_UNIT (type),
                     size_int (TYPE_PRECISION (char_type_node)
                               / BITS_PER_UNIT));
  return fold (build1 (NOP_EXPR, integer_type_node, size));
}


/* Create an array based on another type PROVISIONAL_ITEM_TYPE, with
   size MAX_OCCURS named IDENTIFIER.  */

static tree
get_simple_array (tree provisional_item_type,
                  uint32 min_occurs,
                  uint32 max_occurs,
                  tree identifier)
{
  tree item_max_value;
  tree item_min_value;
  tree item_index_type;
  tree array_item_type;

  item_max_value = build_int_2 (max_occurs, 0);
  item_min_value = build_int_2 (min_occurs, 0);
  item_index_type = build_index_2_type (item_min_value, item_max_value);
  array_item_type = build_array_type (provisional_item_type, item_index_type);
  TYPE_NAME (array_item_type) = identifier;
  TYPE_STUB_DECL (array_item_type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, array_item_type));
  layout_type (array_item_type);
  BLI_ASSERT (get_uint32_value (TYPE_SIZE (array_item_type), LOG_BLI_BITS_PER_BYTE) != 0);
  rest_of_type_compilation (array_item_type, 0 /* Lll; toplev ==1 for file scope vars.  */);
  return array_item_type;
}

/* Output init code for data item DI.  */

static void

one_deferred_init (struct bli_tree_struct_data_item *di)
{
  struct exp_details from;
  struct exp_details to;
  uint32 count_arrays;
  struct bli_token_struct *lit;
  struct bli_tree_struct_data_item *owner_ptr;
  uint32 lineno;
  uint32 fileno;
  native_char *filename;
  tree sizes[LEVELS_ARRAY_SIZE];
  
  /* Just like a move except for tables which require a loop or nested loops.  */

  /* Get line number info for debug.  */

  BLI_ASSERT (di->value_list);
  BLI_ASSERT (di->value_list->value_ptr);
  BLI_ASSERT (di->value_list->value_tok);
  lit = (struct bli_token_struct *)di->value_list->value_tok;
  lineno = lit->bli_token_lineno;
  fileno = lit->bli_token_fileno;
  filename = (native_char *)(file_names->elem[fileno]);
  emit_line_note (filename, lineno);
  
  /* 1. Count the number of occurs levels if any.  */
  for (count_arrays = 0, owner_ptr = (void *) di; owner_ptr; owner_ptr = (void *) owner_ptr->owner)
    {
      if (owner_ptr->occurs_details)
        {
          sizes[count_arrays] = build_int_2 (owner_ptr->occurs_details->occurs_to_int, 0);
          count_arrays ++;
        }
    }
  BLI_ASSERT (count_arrays <= LEVELS_ARRAY_SIZE);
  
  if (count_arrays)
    {
      tree decls[LEVELS_ARRAY_SIZE];
      struct nesting *loops[LEVELS_ARRAY_SIZE];
      tree block;
      tree item_decl;
      uint32 array_ix;
      tree condition;

      /* Push a level so we can define some loop variables.  */
      /* (*lang_hooks.decls.pushlevel) */ pushlevel (0);
      expand_start_bindings (0);

      /* 1. Create the loop vars.  */
      for (array_ix = 0; array_ix < count_arrays; array_ix ++)
        {
          uchar name[20];
          sprintf ((native_char *)name, "init_ix_%d", array_ix);
          item_decl = build_decl (VAR_DECL, get_identifier ((native_char *)name), integer_type_node);
          layout_decl (item_decl, 0);
          DECL_CONTEXT (item_decl) = current_function_decl;
          DECL_INITIAL (item_decl) = NULL_TREE;
          TREE_STATIC (item_decl) = 0;
          TREE_USED (item_decl) = 1;
          
          /* Expand declaration and initial value if any.  */
          
          expand_decl (item_decl);
          if (DECL_INITIAL (item_decl))
            expand_decl_init (item_decl);
          /*Rest_of_decl_compilation(item_decl, 0, 0, 0);.  */
          pushdecl (copy_node (item_decl));
      
          decls[array_ix] = item_decl;
        }
      
      /* 2. Init the loop vars, start each loop.  bliss indexes start with 1.  */
      for (array_ix = 0; array_ix < count_arrays; array_ix ++)
        {
          tree stmt;
          stmt = build (MODIFY_EXPR, integer_type_node, decls[array_ix], integer_one_node);
          TREE_USED (stmt) = 1;
          TREE_SIDE_EFFECTS (stmt) = 1;
          expand_expr_stmt (stmt);
          loops[array_ix] = expand_start_loop (1);
        }

      /* 3. Output the init assignment.  */
      
      get_exp_details (& to, di, decls, count_arrays, zero_is_not_numeric);
      get_exp_details (& from, lit, NULL, 0,
                       to.di->numeric_flag ? zero_is_numeric : zero_is_not_numeric);
      from.all_flag = di->value_list->value_all;
      
      output_move (& from,  & to, lit);
      
      /* 4. Increment the loop vars, test them and terminate each loop.  */
      for (array_ix = 0; array_ix < count_arrays; array_ix ++)
        {
          tree stmt;
          stmt = build (PREINCREMENT_EXPR, integer_type_node, decls[array_ix], integer_one_node);
          TREE_USED (stmt) = 1;
          TREE_SIDE_EFFECTS (stmt) = 1;
          expand_expr_stmt (stmt);
          condition = build (GT_EXPR, integer_type_node, decls[array_ix], sizes[array_ix]);
          expand_start_cond (condition, 0);
          if (!expand_exit_loop (loops[array_ix]))
            abort ();  /* Not in loop?  */
          expand_end_cond ();
          expand_continue_loop (loops[array_ix]);
          expand_end_loop ();
        }

      /* Pop inner level.  */
      block = /* (*lang_hooks.decls.poplevel) */ poplevel  (1, 0, 0);
      expand_end_bindings (block, 1, 1);
    }
  else
    {
      get_exp_details (& to, di, NULL, 0, zero_is_not_numeric);
      get_exp_details (& from, lit, NULL, 0, 
                       to.di->numeric_flag ? zero_is_numeric : zero_is_not_numeric);
      BLI_ASSERT (di->value_list);
      from.all_flag = di->value_list->value_all;
      output_move (& from,  & to, di->reference_token);
    }
}

/* Given an integer constant T return its value as an unsigned 32 bit
   int.  The constant is expressed in terms of units of size 2**SHIFT
   so shift by that number of bits.  */

static uint32
get_uint32_value (tree t, uint32 shift)
{
  uint32 lo;
  uint32 hi;
  uint32 ret1;
  uint32 ret2;
  uint32 ret3;
  
  BLI_ASSERT (TREE_CODE (t) == INTEGER_CST);
  lo = TREE_INT_CST_LOW (t);
  hi = TREE_INT_CST_HIGH (t);
  BLI_ASSERT (!(hi >> shift));
  ret1 = lo >> shift;
  ret2 = hi << (BLI_BITS_PER_WORD - shift);
  ret3 = ret1 | ret2;
  return ret3;
}

/* Issue an error message if a non-01 level data item CURRENT_ITEM
   redefines something that is shorter than itself.  */

static void
check_redefines_length (struct bli_tree_struct_data_item *current_item)
{
  
  BLI_ASSERT (current_item->data_length_bytes > 0);
  
  if (current_item->redefine_member_flag)
    /* Check if not 01 and larger than original.  */
    if (current_item->data_length_bytes
        > current_item->redefines_details->redefines_master->data_length_bytes)
      if (current_item->lvl != 1) /* Lll; or if set to global.  */
        {
          MSG_TOKEN (MSGID (146), current_item->reference_token, NULL);
        }
}

/* Output if verb code for CURRENT_VERB.  */
  
static void
output_if_verb (struct bli_tree_cons *passed_if_verb)
{
  struct bli_tree_struct_verb_if *if_verb;
  struct statement_prefix *current_verb;
  tree condition;

  if_verb = (struct bli_tree_struct_verb_if *)passed_if_verb;

  condition = conditional_test (if_verb->if_condition);
  expand_start_cond (condition, 0);

  for (current_verb = (void *) if_verb->if_true;
       current_verb; 
       current_verb = (void *) current_verb->branch.next)
    {
      BLI_ASSERT (current_verb->branch.pfx.cat == type_production);
      output_verb ((struct bli_tree_cons *)current_verb);
    }

  expand_start_else ();

  for (current_verb = (void *) if_verb->if_false;
       current_verb; 
       current_verb = (void *) current_verb->branch.next)
    {
      BLI_ASSERT (current_verb->branch.pfx.cat == type_production);
      output_verb ((struct bli_tree_cons *)current_verb);
    }

  expand_end_cond ();
}

/* Generate the code for a conditional test for expression PASSED_EXP,
   returning the tree for the test.  */

static tree
conditional_test (void *passed_exp)
{
  struct bli_tree_noncons *test_type;
  struct bli_tree_struct_expression *exp;
  struct bli_tree_struct_data_item *di;

  test_type = passed_exp;
  switch (test_type->pfx.cat)
    {
    case type_production:
      switch (test_type->pfx.type.tok_type)
        {
        case BLI_PROD_TYPE_DATA_ITEM:
          di = passed_exp;
          BLI_ASSERT (di->lvl == 88);
          return generate_level_88_test (passed_exp);
          
        case BLI_PROD_TYPE_EXPRESSION:
          exp = passed_exp;
          switch (exp->operator)
            {
            case 0:
            case LEVEL_88:
              return conditional_test (exp->operand1);
              
            case NOT_magic_for_abbreviated_relation_operator:
              abort (); /* Should be converted to NOT_kw earlier.  */

            case NOT_kw:
              return build1 (TRUTH_NOT_EXPR, integer32_type_node, 
                             conditional_test (exp->operand1)); 

            case OR:
              return build (TRUTH_ORIF_EXPR, integer32_type_node,
                            conditional_test (exp->operand1), 
                            conditional_test (exp->operand2)); 

            case AND_kw:
              return build (TRUTH_ANDIF_EXPR, integer32_type_node,
                            conditional_test (exp->operand1), 
                            conditional_test (exp->operand2)); 

            case LESS_THAN_sym:
            case GREATER_THAN_sym:
            case EQUALS_sym:
            case GREATER_EQUALS_sym:
            case LESS_EQUALS_sym:
            case NOT_EQUALS_sym:
              return basic_conditional_test (exp); 

            case ABBREVIATED_GREATER_THAN_sym:
            case ABBREVIATED_LESS_THAN_sym:
            case ABBREVIATED_EQUAL_sym:
            case ABBREVIATED_NOT_EQUAL_sym:
            case ABBREVIATED_GREATER_EQUAL_sym:
            case ABBREVIATED_LESS_EQUAL_sym:
              return basic_conditional_test (exp); 

            case ABBREVIATED_INVERTED_GREATER_THAN_sym:
            case ABBREVIATED_INVERTED_LESS_THAN_sym:
            case ABBREVIATED_INVERTED_EQUAL_sym:
            case ABBREVIATED_INVERTED_NOT_EQUAL_sym:
            case ABBREVIATED_INVERTED_GREATER_EQUAL_sym:
            case ABBREVIATED_INVERTED_LESS_EQUAL_sym:
              return build1 (TRUTH_NOT_EXPR, integer32_type_node,
                             basic_conditional_test (exp)); 

            case ARRAY_REFERENCE:
              return generate_level_88_test (passed_exp);
              
            default:
              abort ();
            }
          
        default:
          abort ();
        }

        /* Should not be here; should have called get_item_type.  */
    case type_token: 
    default:
      abort ();
      
    }
}

/* Generate code for a comparison being PASSED_EXP.  */

static tree
basic_conditional_test (void *passed_exp)
{
  struct bli_tree_struct_expression *exp;
  struct bli_tree_struct_expression *op1_exp;
  struct bli_tree_struct_expression *op1_exp_use;
  struct exp_details left_ed;
  struct exp_details right_ed;
  uint32 save_needed_flag = 0;
  tree ret;
  
  /* 
     1. If there is a save_details, use that for the trees.
     2. Generate the code. 
     3. If save_expr_needed then set up the save details structures
     and fill them in with the trees.
  */

  exp = passed_exp;
  
  BLI_ASSERT (exp->operand1);
  BLI_ASSERT (exp->operand2);

  op1_exp_use = op1_exp = (void *) exp->operand1;
  if (op1_exp->cons.pfx.cat == type_production && op1_exp->cons.pfx.type.tok_type == SAVE_MAIN)
    op1_exp_use = (void *) op1_exp->operand1;

  /* Get the details for the LHS.  We fix up the zero to numeric later
     on in generate_compare, if this is a numeric expression.  */
  get_exp_details (& left_ed, op1_exp_use, NULL, 0,
                   zero_is_not_numeric);

  if (op1_exp->cons.pfx.cat == type_production && op1_exp->cons.pfx.type.tok_type == SAVE_MAIN)
    {
      struct bli_tree_struct_save_details *sd;
      sd = (void *) op1_exp->operand2;
      left_ed.length = sd->save_length;
      left_ed.address = sd->save_address;
      left_ed.exp = sd->save_value;
    }
  
  /* Get the details for the RHS.  */
  get_exp_details (& right_ed, exp->operand2, NULL, 0,
                   zero_is_not_numeric);

  switch (exp->operator)
    {
    case LESS_THAN_sym:
    case GREATER_THAN_sym:
    case EQUALS_sym:
    case GREATER_EQUALS_sym:
    case LESS_EQUALS_sym:
    case NOT_EQUALS_sym:
      save_needed_flag = check_save (exp->operand1);
    }

  switch (exp->operator)
    {
    case LESS_THAN_sym:
    case ABBREVIATED_LESS_THAN_sym:
    case ABBREVIATED_INVERTED_GREATER_EQUAL_sym:
      ret = generate_compare (LESS_THAN_sym, & left_ed, & right_ed);
      break;

    case GREATER_THAN_sym:
    case ABBREVIATED_GREATER_THAN_sym:
    case ABBREVIATED_INVERTED_LESS_EQUAL_sym:
      ret = generate_compare (GREATER_THAN_sym, & left_ed, & right_ed);
      break;

    case EQUALS_sym:
    case ABBREVIATED_EQUAL_sym:
    case ABBREVIATED_INVERTED_NOT_EQUAL_sym:
      ret = generate_compare (EQUALS_sym, & left_ed, & right_ed);
      break;

    case GREATER_EQUALS_sym:
    case ABBREVIATED_GREATER_EQUAL_sym:
    case ABBREVIATED_INVERTED_LESS_THAN_sym:
      ret = generate_compare (GREATER_EQUALS_sym, & left_ed, & right_ed);
      break;

    case LESS_EQUALS_sym:
    case ABBREVIATED_LESS_EQUAL_sym:
    case ABBREVIATED_INVERTED_GREATER_THAN_sym:
      ret = generate_compare (LESS_EQUALS_sym, & left_ed, & right_ed);
      break;

    case NOT_EQUALS_sym:
    case ABBREVIATED_NOT_EQUAL_sym:
    case ABBREVIATED_INVERTED_EQUAL_sym:
      ret = generate_compare (NOT_EQUALS_sym, & left_ed, & right_ed);
      break;
      
    default:
      abort ();
    }

  if (save_needed_flag)
    {
      /* Save the expression details for later by adding some structs
         to the tree.  */
      struct bli_tree_struct_expression *save_master;
      struct bli_tree_struct_save_details *save_details;
      save_master = BLI_ALLOC (sizeof (struct bli_tree_struct_expression));
      save_details = BLI_ALLOC (sizeof (struct bli_tree_struct_save_details));

      save_details->noncons.pfx.cat = type_production;
      save_details->noncons.pfx.type.tok_type = BLI_PROD_TYPE_EXPRESSION;
      save_master->operator = SAVE_MAIN;
      
      if (op1_exp->cons.pfx.cat == type_token)
        save_master->reference_token = (struct bli_token_struct*)op1_exp;
      else
        {
          if (op1_exp->cons.pfx.type.tok_type == BLI_PROD_TYPE_EXPRESSION)
            {
              struct bli_tree_struct_expression *ex;
              ex =  (struct bli_tree_struct_expression *) op1_exp;
              save_master->reference_token = ex->reference_token;
            }
          else
            {
              struct bli_tree_struct_data_item *di;
              BLI_ASSERT (op1_exp->cons.pfx.type.tok_type == BLI_PROD_TYPE_DATA_ITEM);
              di = (struct bli_tree_struct_data_item *) op1_exp;
              save_master->reference_token = di->reference_token;
            }
        }
      
      save_master->operand1 = passed_exp;
      save_master->operand2 = (void *) save_details;

      save_details->noncons.pfx.cat = type_production;
      save_details->noncons.pfx.type.tok_type = BLI_PROD_TYPE_SAVE_DETAILS;
      save_details->save_length = left_ed.length;
      save_details->save_address = left_ed.address;
      save_details->save_value = left_ed.exp;
    }
  return ret; 
}


/* Generate the compare code for compare of type TYPE_OF_COMPARE,
   comparing LEFT_ED and RIGHT_ED.  */

static tree
generate_compare (uint32 type_of_compare,   
                struct exp_details *left_ed,
                struct exp_details *right_ed)
{
  tree ret = NULL_TREE;
  uint32 backend_operator = 0;
  tree common_type;
  uint32 common_size;
  tree signed_item;
  tree unsigned_item;
  tree slz; /* Signed item < 0.  */
  /* Comparison type - matches item numbers in list above.  */
  uint32 which = 0;  
  tree void_address_left_exp;
  tree void_address_right_exp;
  tree length1_exp;
  tree length2_exp;
  tree compare1_exp;
  tree compare2_exp;
  tree dummy_string;
  tree item1;
  tree item2;
  uint32 modified_backend_operator;
  uint32 len = 0;

  /* 
     Supported comparisons:
     1.  Numeric to numeric - binary only.
     2.  Alphanumeric to alphanumeric.
     3.  Pointer to pointer.
  */

  /* Convert the comparison type to the backend codes.  */
  switch (type_of_compare)
    {
    case LESS_THAN_sym:
      backend_operator = LT_EXPR;
      break;

    case GREATER_THAN_sym:
      backend_operator = GT_EXPR;
      break;

    case EQUALS_sym:
      backend_operator = EQ_EXPR;
      break;

    case GREATER_EQUALS_sym:
      backend_operator = GE_EXPR;
      break;

    case LESS_EQUALS_sym:
      backend_operator = LE_EXPR;
      break;

    case NOT_EQUALS_sym:
      backend_operator = NE_EXPR;
      break;
      
    default:
      abort ();
    }

  /* Determine type of compare.  We already checked that the compare
     is compatible, so that limits the cases.  */

  switch (left_ed->token_type)
    {
    case NULL_word :
      which = 3;
      break;

    case GENERIC_NUMBER_INTEGER :
    case BINARY_CHAR :
    case BINARY_SHORT :
    case BINARY_LONG :
    case BINARY_DOUBLE :
      which = 1;
      break;
      
    case ZERO :
    case ZEROS :
    case ZEROES :
      switch (right_ed->token_type)
        {
        case DISPLAY:
          if (right_ed->di->numeric_flag)
            abort (); /* LLL; numeric display not supported.  */
          else
            left_ed->all_flag = 1; /* Implicit.  */
          goto common2;
          
        case ZERO :
        case ZEROS :
        case ZEROES :
          set_to_numeric_zero (right_ed);
        }
      set_to_numeric_zero (left_ed);
      which = 1;
      break;

    case DISPLAY :
      if (right_ed->di && right_ed->di->numeric_flag)
        abort (); /* LLL; numeric display not supported.  */
      else
        goto common2;
      
    case QUOTE :
    case QUOTES :
    case SPACE :
    case SPACES :
    case LOW_VALUE :
    case LOW_VALUES :
    case HIGH_VALUE :
    case HIGH_VALUES :
      left_ed->all_flag = 1; /* Implicit.  */
    case GENERIC_STRING :
    common2:
      which = 2;
    break;
          
    default :
      abort ();
    }
  
  switch (right_ed->token_type)
    {
      /* ZERO: dual personality literal.  If alphanumeric, it's always
         'all'.  */
    case ZERO :
    case ZEROS :
    case ZEROES :
      if (which != 2)
        {
          set_to_numeric_zero (right_ed);
          break; 
        }
    case QUOTE :
    case QUOTES :
    case SPACE :
    case SPACES :
    case LOW_VALUE :
    case LOW_VALUES :
    case HIGH_VALUE :
    case HIGH_VALUES :
      right_ed->all_flag = 1;
    }


  switch (which)
    {
    case 1 :  /* Numeric to numeric.  */
      {
        common_size = left_ed->num_size;
        if (right_ed->num_size > common_size)
          common_size = right_ed->num_size;
        
        common_type = NULL_TREE;
        if ((left_ed->num_size == right_ed->num_size) && (left_ed->signedp == right_ed->signedp))
          common_type = TREE_TYPE (left_ed->exp);
        else
          if ((right_ed->num_size < 4) && (left_ed->num_size < 4))
            common_type = integer32_type_node;
          else
            if (right_ed->num_size < 8 && left_ed->num_size < 8)
              common_type = integer64_type_node;
            else
              if ((left_ed->num_size == 8)
                  && ((left_ed->signedp == right_ed->signedp)
                      || (left_ed->signedp && right_ed->num_size < 8)))
                common_type = TREE_TYPE (left_ed->exp);
              else
                if ((right_ed->num_size == 8)
                    && ((left_ed->signedp == right_ed->signedp)
                        || (right_ed->signedp && left_ed->num_size < 8)))
                  common_type = TREE_TYPE (right_ed->exp);
        
        if (common_type)
          return build (backend_operator,
                        common_type,
                        build1 (CONVERT_EXPR, common_type, left_ed->exp),
                        build1 (CONVERT_EXPR, common_type, right_ed->exp)
                        );
 
        /* We are comparing a signed of any size and an unsigned which
           is a 64 bit integer.  If the signed one is negative, return
           the comparison of it to zero, else cast the signed one to
           unsigned and compare.  */

        if (left_ed->signedp)
          {
            signed_item = left_ed->exp;
            unsigned_item = right_ed->exp;
            modified_backend_operator = backend_operator;
          }
        else
          {
            signed_item = right_ed->exp;
            unsigned_item = left_ed->exp;
            modified_backend_operator = swap_operator (backend_operator);
          }
        slz = build (LT_EXPR, 
                     integer32_type_node, 
                     signed_item,
                     build1 (CONVERT_EXPR, TREE_TYPE(signed_item), integer_zero_node));
        item1 = build (modified_backend_operator, 
                       integer32_type_node, 
                       signed_item,
                       build1 (CONVERT_EXPR, TREE_TYPE(signed_item), integer_zero_node));
        item2 = build (modified_backend_operator, 
                       integer32_type_node, 
                       build1 (CONVERT_EXPR, TREE_TYPE(unsigned_item), signed_item),
                       unsigned_item);
        ret = build (COND_EXPR, integer32_type_node, slz, item1, item2);
        return ret;
      }
      
    case 2 :  /* Alphanumeric to alphanumeric.  */
      /* 
         1. Both of the operands are 'all' something.  
         
         - In this case we ignore the 'all', and do a normal
         compare as per case 3.

         2. One operand is 'all' something. 

         2.1. The length of the other operand < MAX_FILL_GENERATE.

         - Generate a temp comparison operand and do the compare directly.

         2.2 Otherwise. 

         - Generate a function call to do the compare. 

         3. Neither of the operands are 'all' something. 

         - First compare the common length using memcmp.
         - If the common length was the same, and one is longer,
         compare the longer one to all spaces.
      */
      
      /* Cases 1 & 3 - normal compare.  */
      if (left_ed->all_flag == right_ed->all_flag)
        {
          void_address_left_exp = build1 (NOP_EXPR, ptr_type_node, left_ed->address);
          void_address_right_exp = build1 (NOP_EXPR, ptr_type_node, right_ed->address);
          
          length1_exp = build (MIN_EXPR, integer_type_node, 
                               build1 (CONVERT_EXPR, integer_type_node, left_ed->length), 
                               build1 (CONVERT_EXPR, integer_type_node, right_ed->length));
          
          compare1_exp = generate_character_compare 
            (void_address_left_exp, 
             void_address_right_exp, length1_exp);

          modified_backend_operator = backend_operator;

          if (left_ed->size && right_ed->size && (left_ed->size == right_ed->size))
            compare2_exp = NULL_TREE;
          else
            {
              struct exp_details *op1_ed;
              struct exp_details *op2_ed;
              tree void_address1_exp;
              
              /* Fixed length.  */
              
              if (left_ed->size && right_ed->size)
                {
                  if (left_ed->size > right_ed->size)
                    {
                      op1_ed = left_ed;
                      op2_ed = right_ed;
                    }
                  else
                    {
                      BLI_ASSERT (left_ed->size < right_ed->size);
                      op2_ed = left_ed;
                      op1_ed = right_ed;
                      modified_backend_operator = swap_operator (backend_operator);
                    }
                  void_address1_exp = build1 (NOP_EXPR, ptr_type_node, op1_ed->address);
                  length1_exp = build1 (CONVERT_EXPR, integer_type_node, op2_ed->length);
                  
                  /* Length is whatever is left over, never less than 0.  */
                  len = op1_ed->size - op2_ed->size;
                  length2_exp = build_int_2 (len, 0);
                  
                  compare2_exp = generate_character_compare_fill 
                    (void_address1_exp, length1_exp, length2_exp, len, ' ');
                }
              else
                {
                  /* One or both has dynamic length.  Need to generate dynamic compare.  */
                  void_address_left_exp = build1 (NOP_EXPR, ptr_type_node, left_ed->address);
                  void_address_right_exp = build1 (NOP_EXPR, ptr_type_node, right_ed->address);
                  
                  length1_exp = build1 (CONVERT_EXPR, integer_type_node, left_ed->length);
                  length2_exp = build1 (CONVERT_EXPR, integer_type_node, right_ed->length);
                  
                  compare1_exp = generate_dynamic_character_compare 
                    (void_address_left_exp, 
                     void_address_right_exp, length1_exp, length2_exp);
                  compare2_exp = NULL_TREE;
                  modified_backend_operator = 0;
                }
            }
          return convert_two_compares_to_result (backend_operator, modified_backend_operator,
                                                 compare1_exp, compare2_exp);
        }
      else
        /* Case 2 - one operand only is all.  */
        {
          struct exp_details *all_ed;
          struct exp_details *not_all_ed;
          
          if (left_ed->all_flag)
            {
              all_ed = left_ed;
              not_all_ed = right_ed;
              modified_backend_operator = swap_operator (backend_operator);
            }
          else
            {
              not_all_ed = left_ed;
              all_ed = right_ed;
              modified_backend_operator = backend_operator;
            }

          if (not_all_ed->size && not_all_ed->size < MAX_FILL_GENERATE)
            {
              char tempchars[MAX_FILL_GENERATE];
              uint32 charno;

              if (all_ed->token_type == GENERIC_STRING)
                {
                  for (charno = 0; charno < not_all_ed->size; charno++)
                    {
                      /* The 'all' one must be a literal so we do know the
                         contents and length.  */
                      tempchars[charno] = all_ed->token->string_details->string
                        [charno%all_ed->size];
                    }
                }
              else
                {
                  /* Must be a figurative constant.  */
                  memset (tempchars, all_ed->first_char, not_all_ed->size);
                }
              dummy_string = build_string (not_all_ed->size, (native_char *)tempchars);
              TREE_TYPE (dummy_string) = char_array_type_node;
              void_address_left_exp = build1 (NOP_EXPR, ptr_type_node, not_all_ed->address);
              void_address_right_exp = 
                build1 (CONVERT_EXPR, ptr_type_node,
                        build1 (ADDR_EXPR, char_pointer_type_node, dummy_string));
              length1_exp = build1 (CONVERT_EXPR, integer_type_node, not_all_ed->length);
              length2_exp = build1 (CONVERT_EXPR, integer_type_node, all_ed->length);

              compare1_exp = generate_dynamic_character_compare (void_address_left_exp,
                                                                 void_address_right_exp,
                                                                 length1_exp,
                                                                 length2_exp);
              return convert_two_compares_to_result (modified_backend_operator, 0,
                                              compare1_exp, NULL_TREE);
            }
          else
            {
              /* Too long to generate a literal to compare with, or
                 variable length.  Generate a function call.  */
              void_address_left_exp = build1 (NOP_EXPR, ptr_type_node, left_ed->address);
              void_address_right_exp = build1 (NOP_EXPR, ptr_type_node, right_ed->address);
              
              length1_exp = build1 (CONVERT_EXPR, integer_type_node, left_ed->length);
              length2_exp = build1 (CONVERT_EXPR, integer_type_node, right_ed->length);
              compare1_exp = generate_all_compare (void_address_left_exp,
                                                   void_address_right_exp,
                                                   length1_exp,
                                                   length2_exp);
              return convert_two_compares_to_result (modified_backend_operator, 0,
                                              compare1_exp, NULL_TREE);
            } 
        } 
      break;
      
    case 3 :  /* Pointer to pointer.  */
      common_type = integer_pointer_type_node;
      return build (backend_operator,
                    integer32_type_node,
                    build1 (NOP_EXPR, common_type, left_ed->exp),
                    build1 (NOP_EXPR, common_type, right_ed->exp)
                    );

    default:
      abort ();
    }
  abort ();
}

/* COMPARE1_EXP and COMPARE2_EXP are two expressions with values
   <,=,>0 indicating the results of two comparisons.  If COMPARE1_EXP
   is non-zero or if COMPARE2_EXP is NULL, then return the result of
   apply BACKEND_OPERATOR1 on COMPARE1_EXP, else return the result of
   apply BACKEND_OPERATOR2 on COMPARE2_EXP.  This is used in string
   comparisons where the first comparison relates to the common length
   and the second part relates to the implies fill at the end.  */
   
static tree 
convert_two_compares_to_result
(uint32 backend_operator1, uint32 backend_operator2,
 tree compare1_exp, tree compare2_exp)
{
  tree res1;
  tree res2;
  tree cond_exp;

  res1 = build (backend_operator1, 
                integer32_type_node,
                compare1_exp,
                build1 (NOP_EXPR, integer32_type_node, integer_zero_node));
  BLI_ASSERT (res1);
  if (compare2_exp == NULL_TREE)
    return res1;
  res2 = build (backend_operator2, 
                integer32_type_node,
                compare2_exp,
                build1 (NOP_EXPR, integer32_type_node, integer_zero_node));
  BLI_ASSERT (res2);
  cond_exp = build (COND_EXPR,
                    integer32_type_node,
                    compare1_exp,
                    res1,
                    res2);
  BLI_ASSERT (cond_exp);
  return cond_exp;
}

/* Modify the comparison operator OPERATOR to allow for the operands
   being swapped and return the result.  */

static uint32 
swap_operator (uint32 operator)
{
  switch (operator)
    {
    case LT_EXPR:
      return GE_EXPR;
      
    case GT_EXPR:
      return LE_EXPR;

    case EQ_EXPR:
      return EQ_EXPR;

    case GE_EXPR:
      return LT_EXPR;

    case LE_EXPR:
      return GT_EXPR;

    case NE_EXPR:
      return NE_EXPR;

    default:
      abort ();
    }
}

/* Generate code to compare LEFT/LENGTH1 and RIGHT/LENGTH2 using
BACKEND_OPERATOR as the (tree.def style) comparison operator.  Return
<0, 0 , >0 if LEFT is less than, equal or greater than RIGHT.
LENGTH1/2 are the lengths of the strings with address LEFT and RIGHT
respectively.  */

static tree 
generate_dynamic_character_compare (tree left, tree right,
                                    tree length1, tree length2)
{
  tree common_length;
  tree fill_length;
  tree compare1_exp;
  tree compare2_exp;
  tree left_less;
  tree which_string;
  tree ret;
  
  common_length = build (MIN_EXPR, integer32_type_node, length1, length2);
  compare1_exp = generate_character_compare (left, right, common_length);
  
  fill_length = build (MINUS_EXPR, integer32_type_node, 
                       build (MAX_EXPR, integer32_type_node,
                              length1, length2),
                       common_length);
  left_less = build (LT_EXPR, integer32_type_node, length1, length2);
  which_string = build (COND_EXPR,
                        TREE_TYPE (left),
                        left_less,
                        right, 
                        left);
  compare2_exp = generate_character_compare_fill 
    (which_string, common_length, fill_length, 0, ' ');
  BLI_ASSERT (compare1_exp);
  BLI_ASSERT (compare2_exp);
  ret = build (COND_EXPR, 
         integer32_type_node,
         build (NE_EXPR, integer32_type_node, 
                compare1_exp, 
                build1 (NOP_EXPR, integer32_type_node, integer_zero_node)),
         compare1_exp,
         compare2_exp);
  return ret;
}

/* Generate code to compare LEFT and RIGHT.  Return <0, 0 , >0 if LEFT
is less than, equal or greater than RIGHT.  LENGTH is the length of
both strings.  */
static tree 
generate_character_compare (tree left, tree right, tree length)
{
  tree args;
  tree memcmp_decl_ptr;
  tree call_expr;

  /* Invoke memcmp.  */

  args = tree_cons (NULL_TREE, length, NULL_TREE); 
  args = chainon (tree_cons (NULL_TREE, right, NULL_TREE), args);
  args = chainon (tree_cons (NULL_TREE, left, NULL_TREE), args);
  
  memcmp_decl_ptr = build1
    (ADDR_EXPR, build_pointer_type (TREE_TYPE (memcmp_decl)), memcmp_decl);
  
  call_expr = build (CALL_EXPR, integer_type_node, memcmp_decl_ptr, args, NULL_TREE);
  BLI_ASSERT (call_expr);

  return call_expr;
}

/* Generate code to compare ADDRESS_EXP offset OFFSET_EXP of length
   LENGTH_EXP with an implied string of the same length filled with
   character FILL_CHAR_EXP using BACKEND_OPERATOR as the (tree.def
   style) comparison operator.  Return <0, 0 , >0 if less than, equal
   or greater than.  LEN is the constant value of LENGTH_EXP or zero
   if not constant.  CHAR1 is the actual fill character.  */

static tree 
generate_character_compare_fill (tree address_exp, tree offset_exp,
                                 tree length_exp, 
                                 uint32 len,
                                 native_char char1)
{
  tree dummy_string;
  tree void_address_right_exp;
  tree compare1_exp;
  tree cmpfill_identifier;
  tree param_type_list;
  tree cmpfill_decl_ptr;
  tree decl1;
  tree args;   
  tree call_expr;
  tree ptr_as_int;
  tree interim_exp;
  tree void_new_address_exp;
  tree fill_char_exp;

  fill_char_exp = build_int_2 (char1, 0);

  ptr_as_int = build1 (CONVERT_EXPR, integer_pointer_type_node,
                       address_exp);
  interim_exp = build (PLUS_EXPR, integer_pointer_type_node,
                       build1 (CONVERT_EXPR, integer_pointer_type_node, offset_exp),
                       ptr_as_int);
  void_new_address_exp = build1 (CONVERT_EXPR, ptr_type_node, interim_exp);
  
  /* If it's a constant length and less than MAX_FILL_GENERATE, generate a direct compare.  */
  
  if (len && (len < MAX_FILL_GENERATE)) 
    {
      char tempchars[MAX_FILL_GENERATE];
      
      memset (tempchars, char1, len);

      dummy_string = build_string (len, (native_char *)tempchars);
      TREE_TYPE (dummy_string) = char_array_type_node;
      void_address_right_exp = 
        build1 (CONVERT_EXPR, ptr_type_node,
                build1 (ADDR_EXPR, char_pointer_type_node, dummy_string));
      compare1_exp = generate_character_compare (void_new_address_exp,
                                                 void_address_right_exp,
                                                 length_exp);
      return compare1_exp;
    }
  else
    {
      if (!cmpfill_fn_decl)
        {
          
          cmpfill_identifier = get_identifier ("blir_cmpfill"); 
          
          /* Types are consed in reverse order.  */
          param_type_list = tree_cons
            (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
          param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
          param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
          param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
          
          cmpfill_fn_type = build_function_type (integer32_type_node, param_type_list);
          
           (decl1 = build_decl (PARM_DECL, get_identifier ("fill_char"), 
                              integer32_type_node));
          
           (decl1 = build_decl (PARM_DECL, get_identifier ("length"), 
                              void_pointer_type_node));
          
           (decl1 = build_decl (PARM_DECL, get_identifier ("address"), 
                              integer32_type_node));
          
          cmpfill_fn_decl = build_decl
            (FUNCTION_DECL, cmpfill_identifier, cmpfill_fn_type);
          
          TREE_TYPE (cmpfill_fn_decl) = cmpfill_fn_type;
          DECL_EXTERNAL (cmpfill_fn_decl) = 1;
          TREE_PUBLIC (cmpfill_fn_decl) = 1;
          TREE_STATIC (cmpfill_fn_decl) = 0;
          DECL_ARGUMENTS (cmpfill_fn_decl) = getdecls ();
          DECL_RESULT (cmpfill_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, void_type_node);
          DECL_CONTEXT (DECL_RESULT (cmpfill_fn_decl)) = NULL_TREE;
          TREE_USED (cmpfill_fn_decl) = 1;
          
          rest_of_decl_compilation (cmpfill_fn_decl, NULL, 1, 0);
          make_decl_rtl (cmpfill_fn_decl, NULL);
        }
      args = tree_cons (NULL_TREE, 
                        build1 (NOP_EXPR, integer32_type_node, fill_char_exp), 
                        NULL_TREE); 
      args = chainon (tree_cons (NULL_TREE, length_exp, NULL_TREE), args);
      args = chainon (tree_cons (NULL_TREE, address_exp, NULL_TREE), args);
      
      cmpfill_decl_ptr = build1
        (ADDR_EXPR, build_pointer_type (TREE_TYPE (cmpfill_fn_decl)), cmpfill_fn_decl);
      
      call_expr = build (CALL_EXPR, void_type_node, cmpfill_decl_ptr, args, NULL_TREE);
      
      return call_expr;
    }
}

/* Generate a function call to compare LEFT of length LEFT_LENGTH_EXP
with RIGHT of length RIGHT_LENGTH_EXP.  The compare should
conceptually repeat the second item until the first item is all
compared, if the first item is longer than the second item.  Return
<0, 0 , >0 if LEFT is less than, equal or greater than RIGHT.  */

static tree 
generate_all_compare (tree left,
                      tree right,
                      tree left_length_exp,
                      tree right_length_exp)
{
  
  tree cmpall_identifier;
  tree param_type_list;
  tree cmpall_decl_ptr;
  tree decl1;
  tree args;   
  tree call_expr;

  if (!cmpall_fn_decl)
    {
      
      cmpall_identifier = get_identifier ("blir_cmpall"); 
      
      /* Types are consed in reverse order.  */
      param_type_list = tree_cons
        (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
      
      cmpall_fn_type = build_function_type (integer32_type_node, param_type_list);
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("length2"), 
                          void_pointer_type_node));
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("address2"), 
                          integer32_type_node));
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("length1"), 
                          void_pointer_type_node));
      
       (decl1 = build_decl (PARM_DECL, get_identifier ("address1"), 
                          integer32_type_node));

      cmpall_fn_decl = build_decl
        (FUNCTION_DECL, cmpall_identifier, cmpall_fn_type);
      
      TREE_TYPE (cmpall_fn_decl) = cmpall_fn_type;
      DECL_EXTERNAL (cmpall_fn_decl) = 1;
      TREE_PUBLIC (cmpall_fn_decl) = 1;
      TREE_STATIC (cmpall_fn_decl) = 0;
      DECL_ARGUMENTS (cmpall_fn_decl) = getdecls ();
      DECL_RESULT (cmpall_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, void_type_node);
      DECL_CONTEXT (DECL_RESULT (cmpall_fn_decl)) = NULL_TREE;
      TREE_USED (cmpall_fn_decl) = 1;
      
      rest_of_decl_compilation (cmpall_fn_decl, NULL, 1, 0);
      make_decl_rtl (cmpall_fn_decl, NULL);
    }

  args = tree_cons (NULL_TREE, right_length_exp, NULL_TREE);
  args = chainon (tree_cons (NULL_TREE, right, NULL_TREE), args);
  args = chainon (tree_cons (NULL_TREE, left_length_exp, NULL_TREE), args);
  args = chainon (tree_cons (NULL_TREE, left, NULL_TREE), args);
  
  cmpall_decl_ptr = build1
    (ADDR_EXPR, build_pointer_type (TREE_TYPE (cmpall_fn_decl)), cmpall_fn_decl);
  
  call_expr = build (CALL_EXPR, void_type_node, cmpall_decl_ptr, args, NULL_TREE);
  
  return call_expr;
}


/* Generate code for a level 88 type test, viz PASSED_EXP.  */

static tree
generate_level_88_test (void *passed_exp)
{
  struct exp_details ed;
  struct bli_tree_struct_value_item *value;
  tree exp = NULL;
  struct bli_tree_struct_expression *passed_exp_struct;
  
  passed_exp_struct = passed_exp;

  get_exp_details (&ed, passed_exp, NULL, 0, zero_is_not_numeric);
  
  for (value = ed.level_88_di->value_list;
       value;
       value = (void *) value->cons.next)
    {
      tree one_test;
      one_test = generate_level_88_single_test (&ed, value);
      if (exp)
        exp = build (TRUTH_OR_EXPR, integer32_type_node, exp, one_test);
      else
        exp = one_test;
    }

  return exp;
}

/* Output code for one VALUE of a level 88 test: VALUE with the
   overall expression in ED. */

static tree
generate_level_88_single_test (struct exp_details *ed, struct bli_tree_struct_value_item *value)
{
  if (value->second_value)
    {
      tree part1;
      tree part2;
      
      part1 = generate_level_88_comparison (GREATER_EQUALS_sym, ed, value);
      part2 = generate_level_88_comparison (LESS_EQUALS_sym, ed, value->second_value);

      return build (TRUTH_AND_EXPR,
                    integer32_type_node,
                    part1,
                    part2);
    }
  else
    {
      return generate_level_88_comparison (EQUALS_sym, ed, value);
    }
}

/* Output code for one comparison of a level 88 test: compare value
   with the VALUE using test of type TEST_TYPE (see tree.def for
   types; not all are supported), with surrounding expression
   including the main data item detailed in ED. */

static tree
generate_level_88_comparison (uint32 test_type,
                           struct exp_details *ed, 
                           struct bli_tree_struct_value_item *value)
{
  struct exp_details value_ed;
  struct bli_tree_struct_expression value_exp;
  enum zero_enum zero_numeric_flag;
  
  /* Convert the value item to an expresion.  */
  memset (&value_exp, 0, sizeof (value_exp));
  value_exp.cons.pfx.cat=type_production;
  value_exp.cons.pfx.type.tok_type=BLI_PROD_TYPE_EXPRESSION;
  if (value->value_all)
    value_exp.operator = ALL;
  else
    value_exp.operator = 0;
  value_exp.reference_token = value->value_tok;
  value_exp.operand1 = (void *) value->value_tok;

  /* Get the tree stuff for the expression.  */
  if (ed->di->numeric_flag)
    zero_numeric_flag = zero_is_numeric;
  else
    zero_numeric_flag = zero_is_not_numeric;
  get_exp_details (&value_ed, &value_exp, NULL, 0, 
                   zero_numeric_flag);

  return generate_compare (test_type, ed, &value_ed);
}

/* Output goto verb code for CURRENT_VERB.  */
  
static void
output_goto_verb (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_goto *goto_verb;
  struct bli_tree_struct_procedure_ref *current_proc;
  struct bli_tree_struct_label *label;
  struct exp_details ed;

  goto_verb = (struct bli_tree_struct_verb_goto *)current_verb;
  current_proc = (void *) goto_verb->proc;
  BLI_ASSERT (current_proc);
  BLI_ASSERT (current_proc->pfx.branch.pfx.cat == type_production);
  if (current_proc->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_PROCEDURE_REF)
    {
      /* Plain old go to.  */
      label = (void *) current_proc->label;
      BLI_ASSERT (label);
      BLI_ASSERT (label->pfx.branch.pfx.cat == type_production);
      BLI_ASSERT ((label->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_PARAGRAPH) ||
             (label->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_SECTION));
      expand_goto (label->save_decl);
    }
  else
    {
      /* Go to depending on.  */
      uint32 count;
      tree type = NULL_TREE;
      tree value;
      tree duplicate;
      struct bli_tree_branch *ptr;
      struct bli_tree_struct_procedure_ref *ref;
      int32 pushres;
      tree branch_label;

      get_exp_details (& ed, goto_verb->expression, NULL, 0, zero_is_numeric);
      
      if (ed.exp)
        type = TREE_TYPE (ed.exp);

      expand_start_case (0, ed.exp, type, "GO TO DEPENDING ON");
      
      for (ptr = (struct bli_tree_branch *)current_proc, count = 1;
           ptr;
           ptr = (void *) ptr->next, count++)
        {
          BLI_ASSERT (ptr->pfx.cat == type_production);
          BLI_ASSERT (ptr->pfx.type.tok_type == BLI_PROD_TYPE_ITEM_LIST);
          ref = (struct bli_tree_struct_procedure_ref *)ptr->child;
          BLI_ASSERT (ref);
          BLI_ASSERT (ref->pfx.branch.pfx.cat == type_production);
          BLI_ASSERT (ref->pfx.branch.pfx.type.prod_type == BLI_PROD_TYPE_PROCEDURE_REF);
          value = build_int_2 (count, 0);
          branch_label = build_decl (LABEL_DECL, NULL_TREE, NULL_TREE);
          pushres = pushcase (value, convert, branch_label,  & duplicate);
          BLI_ASSERT (pushres == 0);  /* Should be noncontiguous.  */
          expand_goto (((struct bli_tree_struct_label *)ref->label)->save_decl);
        }
      expand_end_case (ed.exp);
    }
}

/* Output set verb code for CURRENT_VERB which is a SET ADDRESS or SET
   POINTER type.  */
  
static void
output_set_verb_address (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_set *set_verb;
  struct bli_tree_branch *set_to_item;
  struct bli_tree_struct_expression *to_item_exp;
  struct exp_details from_ed;
  struct exp_details to_ed;
  tree from_exp;
  tree to_exp1;
  tree to_exp2;
  tree to_item_tree;

  set_verb = (struct bli_tree_struct_verb_set *)current_verb;
  BLI_ASSERT (set_verb->pfx.branch.pfx.cat == type_production);
  BLI_ASSERT (set_verb->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_VERB_SET);
  
  /* Get the from expression.  */
  /* Everything was already validated so nothing can go wrong.  */
  
  get_exp_details (& from_ed, set_verb->set_expression, NULL, 0, zero_is_not_numeric);
  BLI_ASSERT (from_ed.exp);
  from_exp = save_expr (from_ed.exp);

  /* Set all the 'to' expressions to it.  */
  
  BLI_ASSERT (set_verb->set_to);

  for (set_to_item = (struct bli_tree_branch *)set_verb->set_to;
       set_to_item;
       set_to_item = (struct bli_tree_branch *)set_to_item->next)
    {
      to_item_exp = (struct bli_tree_struct_expression *)set_to_item;
      BLI_ASSERT (to_item_exp->cons.pfx.cat == type_production);
      BLI_ASSERT (to_item_exp->cons.pfx.type.tok_type == BLI_PROD_TYPE_EXPRESSION);

      if (to_item_exp->operator == ADDRESS_kw)
        {
          to_item_tree = ((struct bli_tree_struct_data_item *)to_item_exp->operand1)->save_decl;
        }
      else
        {
          get_exp_details (& to_ed, to_item_exp, NULL, 0, zero_is_not_numeric);
          to_item_tree = to_ed.exp;
        }

      if (set_verb->up_down_true_token)
        {
          tree from_as_int;
          tree ptr_as_int;
          tree interim_exp;
          
          from_as_int = from_exp;
          ptr_as_int = build1 (CONVERT_EXPR, integer_pointer_type_node, to_item_tree);
          to_exp1 = build_int_binary_expression
            ((set_verb->up_down_true_token->pfx.type.tok_type == UP) ? PLUS_EXPR : MINUS_EXPR,
             ptr_as_int, from_as_int);
          if ((TREE_TYPE (to_exp1) == TREE_TYPE (to_item_tree)))
            interim_exp = to_exp1;
          else
            interim_exp = build1 (CONVERT_EXPR, TREE_TYPE (to_item_tree), to_exp1);
          
          to_exp2 = build (MODIFY_EXPR, TREE_TYPE (to_item_tree), to_item_tree, interim_exp);
        }
      else
        {
          to_exp1 = from_exp;
          to_exp2 = build (MODIFY_EXPR, TREE_TYPE (to_item_tree), to_item_tree, to_exp1);
        }
      TREE_USED (to_exp2) = 1;
      TREE_SIDE_EFFECTS (to_exp2) = 1;
      
      expand_expr_stmt (to_exp2);
    }
}

/* Output set verb code for CURRENT_VERB which is a SET CONDITION.  */
  
static void
output_set_verb_condition (struct bli_tree_cons *current_verb)
{
  struct bli_tree_struct_verb_set *set_verb;
  struct bli_tree_branch *set_to_item;
  struct bli_tree_struct_expression *to_item_exp;

  set_verb = (struct bli_tree_struct_verb_set *)current_verb;
  BLI_ASSERT (set_verb->pfx.branch.pfx.cat == type_production);
  BLI_ASSERT (set_verb->pfx.branch.pfx.type.tok_type == BLI_PROD_TYPE_VERB_SET);
  
  /* Set all the 'to' expressions to true.  */
  
  BLI_ASSERT (set_verb->set_to);

  for (set_to_item = (struct bli_tree_branch *)set_verb->set_to;
       set_to_item;
       set_to_item = (struct bli_tree_branch *)set_to_item->next)
    {
      struct exp_details from_ed;
      struct exp_details to_ed;
      struct bli_tree_struct_expression *delve_exp;
      struct bli_tree_struct_data_item *delve_data_item;
      struct bli_tree_struct_value_item *delve_value_item;
      struct bli_tree_struct_expression delve_fake_expression;
      struct bli_tree_struct_expression *delve_use_expression = NULL;

      to_item_exp = (struct bli_tree_struct_expression *)set_to_item;
      BLI_ASSERT (to_item_exp->cons.pfx.cat == type_production);
      BLI_ASSERT (to_item_exp->cons.pfx.type.tok_type == BLI_PROD_TYPE_EXPRESSION);

      /* Get the underlying receiving variable.  */
      get_exp_details (& to_ed, set_to_item, NULL, 0, zero_is_not_numeric);
      
      /* Find the value expression.  The starting point could be in an
         array expression so we need to delve to the bottom.  */
      delve_exp = to_item_exp;
      while (1)
        {
          if (delve_exp->cons.pfx.cat == type_token)
            {
              delve_use_expression = delve_exp; 
              break;
            }
          BLI_ASSERT (delve_exp->cons.pfx.cat == type_production);
          switch (delve_exp->cons.pfx.type.tok_type)
            {
            case BLI_PROD_TYPE_EXPRESSION :
              switch (delve_exp->operator)
                {
                case 0:
                case LEVEL_88:
                case ARRAY_REFERENCE: 
                  delve_exp = (void *) delve_exp->operand1;
                  break;
                  
                default :
                  abort ();
                }
              continue;
              
            case BLI_PROD_TYPE_DATA_ITEM :
              delve_data_item = (struct bli_tree_struct_data_item *)delve_exp;
              delve_value_item = delve_data_item->value_list;
              if (delve_value_item->value_all)
                {
                  memset (& delve_fake_expression, 0, sizeof (delve_fake_expression));
                  delve_fake_expression.cons.pfx.cat = type_production;
                  delve_fake_expression.cons.pfx.type.tok_type = BLI_PROD_TYPE_EXPRESSION;
                  delve_fake_expression.operator = ALL;
                  delve_fake_expression.operand1 = (void *) delve_data_item->value_list->value_tok;
                  delve_fake_expression.reference_token = delve_data_item->value_list->value_tok;
                  delve_use_expression = & delve_fake_expression;
                }
              else
                {
                  delve_use_expression = (struct bli_tree_struct_expression *)
                    delve_data_item->value_list->value_tok;
                }
              break;
              
            case BLI_PROD_TYPE_VALUE_ITEM :
              
            default :
              abort ();
            }
          break;
        }
      get_exp_details (& from_ed, delve_use_expression, NULL, 0, 
                       to_ed.di->numeric_flag ? zero_is_numeric : zero_is_not_numeric);
      
      output_move (& from_ed,  & to_ed, to_item_exp->reference_token);
    }
}


/* Output display verb code for CURRENT_VERB.  */
  
static void
output_display_verb (struct bli_tree_cons *current_verb)
{
  int32 fileno = 0;
  struct bli_token_struct *ref_token;

  struct bli_tree_struct_verb_display *display_verb;
  struct bli_tree_branch *display_list_item;
  struct exp_details from_ed;
  tree call_expr;
  tree bli_display_identifier = NULL_TREE;
  tree bli_display_fn_decl_ptr = NULL_TREE;
  tree param_type_list;
  tree decl1;

  display_verb = (struct bli_tree_struct_verb_display *)current_verb;

  lineno = display_verb->pfx.reference_token->bli_token_lineno;
  input_filename = (native_char *)file_names->elem[display_verb->pfx.reference_token->bli_token_fileno];

  if (!bli_display_fn_decl)
    {
      bli_display_identifier = get_identifier ("blir_display"); 

      /* Types are consed in reverse order.  */
      param_type_list = tree_cons
        (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */

      param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, integer32_type_node, param_type_list);
      param_type_list = tree_cons (NULL_TREE, void_pointer_type_node, param_type_list);

      bli_display_fn_type = build_function_type (void_type_node, param_type_list);

       (decl1 = build_decl (PARM_DECL, get_identifier ("pic"), 
                       void_pointer_type_node));
       (decl1 = build_decl (PARM_DECL, get_identifier ("flags"), 
                       integer32_type_node));
       (decl1 = build_decl (PARM_DECL, get_identifier ("type"), 
                       integer32_type_node));
       (decl1 = build_decl (PARM_DECL, get_identifier ("length2"), 
                       integer32_type_node));
       (decl1 = build_decl (PARM_DECL, get_identifier ("length"), 
                       integer32_type_node));
       (decl1 = build_decl (PARM_DECL, get_identifier ("address"), 
                       void_pointer_type_node));

      ref_token = display_verb->pfx.reference_token;
      lineno = ref_token->bli_token_lineno;
      fileno = ref_token->bli_token_fileno;
      input_filename = (native_char *)file_names->elem[fileno];

      bli_display_fn_decl = build_decl
        (FUNCTION_DECL, bli_display_identifier, bli_display_fn_type);
                    
      TREE_TYPE (bli_display_fn_decl) = bli_display_fn_type;
      DECL_EXTERNAL (bli_display_fn_decl) = 1;
      TREE_PUBLIC (bli_display_fn_decl) = 1;
      TREE_STATIC (bli_display_fn_decl) = 0;
      DECL_ARGUMENTS (bli_display_fn_decl) = getdecls ();
      DECL_RESULT (bli_display_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, void_type_node);
      DECL_CONTEXT (DECL_RESULT (bli_display_fn_decl)) = NULL_TREE;
      TREE_USED (bli_display_fn_decl) = 1;

      rest_of_decl_compilation (bli_display_fn_decl, NULL, 1, 0);
      make_decl_rtl (bli_display_fn_decl, NULL);
    }
  
  bli_display_fn_decl_ptr = build1
    (ADDR_EXPR, build_pointer_type (bli_display_fn_type), bli_display_fn_decl);

  /* Walk the things to display.  */

  {
    tree flag_exp;
    tree length2_exp;
    tree void_address_exp;
    tree args = NULL_TREE;
    struct bli_tree_noncons *display_details;
                  
    flag_exp = integer_zero_node; 
    length2_exp = integer_zero_node; 

    for (display_list_item = (struct bli_tree_branch *)display_verb->pfx.branch.child;
         display_list_item;
         display_list_item = (struct bli_tree_branch *)display_list_item->next)
      {
        display_details = (struct bli_tree_noncons *)(display_list_item->child);

        get_exp_details (& from_ed, display_details, NULL, 0, zero_is_not_numeric);

        void_address_exp = build1 (NOP_EXPR, ptr_type_node, from_ed.address);
        
        args = tree_cons (NULL_TREE, null_pointer_node, NULL_TREE); 
        args = chainon (tree_cons (NULL_TREE, flag_exp, NULL_TREE), args);
        args = chainon (tree_cons (NULL_TREE, build_int_2 (from_ed.enum_type, 0), NULL_TREE), args);
        args = chainon (tree_cons (NULL_TREE, length2_exp, NULL_TREE), args);
        args = chainon (tree_cons (NULL_TREE, from_ed.length, NULL_TREE), args);
        args = chainon (tree_cons (NULL_TREE, void_address_exp, NULL_TREE), args);
                      
        call_expr = build (CALL_EXPR, void_type_node, bli_display_fn_decl_ptr, args, NULL_TREE);
        TREE_USED (call_expr) = 1;
        TREE_SIDE_EFFECTS (call_expr) = 1;
                      
        expand_expr_stmt (call_expr);
                      
      }
                  
    /* Output the final crlf or whatever.  */
    call_void_function_no_args ((uchar *)"blir_display_nl", display_verb->pfx.reference_token);
    
  }
}

/* Output stop verb code for CURRENT_VERB.  */
  
static void
output_stop_verb (struct bli_tree_cons *current_verb)
{
  struct bli_token_struct *ref_token;
  static const uchar *name = (uchar *)"blir_stop_run";
  tree bli_identifier = NULL_TREE;
  tree decl1;
  tree args = NULL_TREE;
  tree call_expr;
  tree rc_exp = NULL_TREE;
  tree bli_fn_decl_ptr = NULL_TREE;
  tree param_type_list = NULL_TREE;
  
  ref_token = ((struct bli_tree_struct_verb_stop *)current_verb)->pfx.reference_token;

  lineno = ref_token->bli_token_lineno;
  input_filename = (native_char *)file_names->elem[ref_token->bli_token_fileno];

  {
    bli_identifier = get_identifier ((native_char *)name); 
    
    /* Types are consed in reverse order.  */
    param_type_list = tree_cons
      (NULL_TREE, integer32_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
    
    bli_stop_fn_type = build_function_type (void_type_node, param_type_list);
    
    bli_stop_fn_decl = build_decl
      (FUNCTION_DECL, bli_identifier, bli_stop_fn_type);
    
    TREE_TYPE (bli_stop_fn_decl) = bli_stop_fn_type;
    DECL_EXTERNAL (bli_stop_fn_decl) = 1;
    TREE_PUBLIC (bli_stop_fn_decl) = 1;
    TREE_STATIC (bli_stop_fn_decl) = 0;
    DECL_ARGUMENTS (bli_stop_fn_decl) = NULL_TREE;
    DECL_RESULT (bli_stop_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, void_type_node);
    DECL_CONTEXT (DECL_RESULT (bli_stop_fn_decl)) = NULL_TREE;
    TREE_USED (bli_stop_fn_decl) = 1;
    
     (decl1 = build_decl (PARM_DECL, get_identifier ("return_code"), 
                     void_pointer_type_node));

    DECL_ARGUMENTS (bli_stop_fn_decl) = getdecls ();

    rest_of_decl_compilation (bli_stop_fn_decl, NULL, 1, 0);
    make_decl_rtl (bli_stop_fn_decl, NULL);
  }
  
  rc_exp = integer_zero_node; 
  
  args = tree_cons (NULL_TREE, rc_exp, NULL_TREE);
  
  bli_fn_decl_ptr = build1
    (ADDR_EXPR, build_pointer_type (bli_stop_fn_type), bli_stop_fn_decl);

  call_expr = build (CALL_EXPR, void_type_node, bli_fn_decl_ptr, args, NULL_TREE);
  TREE_USED (call_expr) = 1;
  TREE_SIDE_EFFECTS (call_expr) = 1;

  expand_expr_stmt (call_expr);
}

/* 

   Recursive function to return an expression accessing a variable VAR
   which may be in a structure and may have OCCURS expressions as the
   array indexes. 
   
   The occurs expressions are in reverse order ie bottom first, top
   last.  */

static tree
access_variable (struct bli_tree_struct_data_item *var, 
                struct bli_tree_struct_expression *occurs,
                tree *indexes,
                uint32 index_count)
{
  tree higher_level_access = NULL;
  tree item_ref;
  tree array_ref;
  tree array_index_exp;
  tree pointed_ref;
  tree prelim_ret;
  
  BLI_ASSERT (occurs ? !indexes : 1);
  BLI_ASSERT (indexes ? !occurs : 1);
  BLI_ASSERT (!!index_count <= !!indexes);

  if (data_item_top_level (var))
    {
      /* Simple, just return the decl.  For linkage section and based
         variables there is a pointer behind the scenes (like C++
         reference). Top level data items cannot have occurs in
         BLISS.  */
      BLI_ASSERT (var->save_decl);
      if (var->based_flag)
        {
          tree pointer_type;
          pointer_type = TREE_TYPE ((tree)(var->save_decl));
          /* Type is the pointer type.  type of type is the type that the pointer points to.  */
          pointed_ref = build1 (INDIRECT_REF, TREE_TYPE (pointer_type), var->save_decl);
          prelim_ret = pointed_ref;
       }
      else
        {
          prelim_ret = var->save_decl;
        }
    }
  else
    {
      /* Need to access the top 01 level decl and then do component_ref
         at each level below that.  */
      if (var->occurs_details)
        {
          struct exp_details from_ed;
          tree array_index_expression_int;
          if (occurs)
            {
              higher_level_access =
                access_variable ((void *) var->owner, ((struct bli_tree_struct_expression *)occurs->cons.next),
                                 NULL, 0);
              item_ref = build (COMPONENT_REF, TREE_TYPE (((tree)var->save_decl)),
                             higher_level_access, var->save_decl);
              get_exp_details (& from_ed, occurs, NULL, 0, zero_is_numeric);
              array_index_exp = from_ed.exp;
            }
          else
            {
              BLI_ASSERT (indexes);
              higher_level_access =
                access_variable ((void *) var->owner, NULL, 
                                 indexes, index_count -1);
              item_ref = build (COMPONENT_REF, TREE_TYPE (((tree)var->save_decl)),
                             higher_level_access, var->save_decl);
              array_index_exp = indexes[index_count -1];
            }
          if (TREE_TYPE (array_index_exp) == integer32_type_node)
            array_index_expression_int = array_index_exp;
          else
            array_index_expression_int = build1 (CONVERT_EXPR, integer32_type_node, array_index_exp);
          /* Tree type of an array is the type of the elements.  */
          array_ref = build (ARRAY_REF, TREE_TYPE (TREE_TYPE (((tree)var->save_decl))),
                          item_ref, array_index_expression_int);
          item_ref = array_ref;
        }
      else
        {
          tree new_type;
          higher_level_access = access_variable ((void *) var->owner, occurs, indexes, index_count);
          new_type = TREE_TYPE (((tree)var->save_decl));
          item_ref = build (COMPONENT_REF, new_type, higher_level_access, var->save_decl);
        }
      prelim_ret = item_ref;
    }
  return prelim_ret;
}

/* Return an expression for the variable part of the length of a group
 data item ITEM whose size is governed by one or more occurs depending
 on variables, and fill in the int pointed to by FIXED_LENGTH_PTR with
 the fixed part of the length, if the pointer exists, else incorporate
 it into the main expression.  */

static tree
get_dynamic_length (struct bli_tree_struct_data_item *item, uint32 *fixed_length_ptr)
{
  tree ret_exp = NULL_TREE;
  tree partial_exp;
  uint32 tot_fixed_size = 0;
  uint32 part_fixed_size;
  struct bli_tree_struct_data_item *child;

  if ((!item->redefine_master_flag) /* Not fixed length.  */
      && ((item->contains_occurs_depending_on_flag)
         || ((item->occurs_details) && (item->occurs_details->occurs_depending))))
    {
      if (!item->branch.child)
        {
          partial_exp = NULL_TREE;
          tot_fixed_size = item->data_length_bytes;
        }
      else
        {
          for (child = (struct bli_tree_struct_data_item *)item->branch.child; 
               child; 
               child = (struct bli_tree_struct_data_item *)child->branch.next)
            {
              partial_exp = get_dynamic_length (child,  & part_fixed_size);
              tot_fixed_size += part_fixed_size;
              if (partial_exp)
                {
                  if (ret_exp)
                    {
                      ret_exp = build_int_binary_expression (PLUS_EXPR, ret_exp, partial_exp);
                    }
                  else
                    {
                      ret_exp = partial_exp;
                    }
                }
            }
        }
      if (item->occurs_details)
        {
          if (item->occurs_details->occurs_depending)
            {
              tree occurs_dep;
              tree occurs_dep_int;
              occurs_dep = access_variable (item->occurs_details->occurs_depending_item, NULL, NULL, 0);
              occurs_dep_int = occurs_dep;
              if (ret_exp)
                {
                  tree sum1;
                  sum1 = build_int_binary_expression (PLUS_EXPR, ret_exp, build_int_2 (tot_fixed_size, 0));
                  ret_exp = build_int_binary_expression (MULT_EXPR, sum1, occurs_dep_int);
                  tot_fixed_size = 0;
                }
              else
                {
                  ret_exp = build_int_binary_expression (MULT_EXPR, occurs_dep_int, 
                                                      build_int_2 (tot_fixed_size, 0));
                  tot_fixed_size = 0;
                }
            }
          else
            {
              tree occurs_to;
              occurs_to = build_int_2 (item->occurs_details->occurs_to_int, 0);
              if (ret_exp)
                {
                  tree sum1;
                  sum1 = build_int_binary_expression (PLUS_EXPR, ret_exp, 
                                                   build_int_2 (tot_fixed_size, 0));
                  ret_exp = build_int_binary_expression (MULT_EXPR, sum1, occurs_to);
                  tot_fixed_size = 0;
                }
              else
                {
                  tot_fixed_size *= item->occurs_details->occurs_to_int;
                }
              
            }
        }
    }
  else
    {
      tot_fixed_size = item->data_length_bytes;
      partial_exp = NULL;
    }

  if (fixed_length_ptr)
    * fixed_length_ptr = tot_fixed_size;
  else
    ret_exp = build_int_binary_expression (PLUS_EXPR, ret_exp, 
                                        build_int_2 (tot_fixed_size, 0));
  return ret_exp;
}

/* Build an expression combining OP1 and OP2 using operator OP, and
   doing any needed promotions to larger types.  At a minimum the terms
   are promoted to 32 bit signed int. The hierarchy from low to high
   is int32, unsigned32, int64, unsigned64.  */

static tree
build_int_binary_expression (uint32 op, tree op1, tree op2)
{
  tree type1;
  tree type2;
  tree newop1;
  tree newop2;
  tree newtype = NULL_TREE;
  uint32 size1;
  uint32 size2;
  uint32 unsign1;
  uint32 unsign2;

  type1 = TREE_TYPE (op1);
  size1 = get_uint32_value (TYPE_SIZE (type1), 0);
  type2 = TREE_TYPE (op2);
  size2 = get_uint32_value (TYPE_SIZE (type2), 0);
  
  unsign1 = (integer_zerop (TYPE_MIN_VALUE (type1)));
  unsign2 = (integer_zerop (TYPE_MIN_VALUE (type2)));

  if ((size1 < 32) && (size2 < 32))
    newtype = integer32_type_node;
  else
    if ((size1 == 32) && (size2 == 32))
      if (unsign1 == unsign2)
        newtype = type1;
      else
        newtype = integer64_type_node;
    else
      if ((size1 == 64) && (size2 == 64) && (unsign1 == unsign2))
        newtype = type1;
      else
        newtype = integer64_unsigned_type_node;
  
  if (type1 != newtype)
    newop1 = build1 (CONVERT_EXPR, newtype, op1);
  else
    newop1 = op1;

  if (type2 != newtype)
    newop2 = build1 (CONVERT_EXPR, newtype, op2);
  else
    newop2 = op2;
  
  return build (op, newtype, newop1, newop2);
}

/* Build exponentiation expression op1**op2.  Promote op1 as needed at
   a minimum op1 is promoted to 32 bit signed int. The hierarchy from
   low to high is int32, unsigned32, int64, unsigned64.  */

static tree
build_int_binary_expression_exponentiation (tree op1, tree op2,
                                           struct bli_token_struct *ref_token)
{
#define SIZE1_64 8  
#define SIGN1    4
#define SIZE2_64 2 
#define SIGN2    1 
  tree type1;
  tree type2;
  tree newop1;
  tree newop2;
  tree newtype1 = NULL_TREE;
  tree newtype2 = NULL_TREE;
  tree call_expr;
  uint32 size1;
  uint32 size2;
  uint32 sign1;
  uint32 sign2;

  uint32 ix;
  int32 fileno = 0;

  newtype1 = type1 = TREE_TYPE (op1);
  size1 = get_uint32_value (TYPE_SIZE (type1), 0);
  sign1 = (integer_zerop (TYPE_MIN_VALUE (type1)));

  newtype2 = type2 = TREE_TYPE (op2);
  size2 = get_uint32_value (TYPE_SIZE (type2), 0);
  sign2 = (integer_zerop (TYPE_MIN_VALUE (type2)));

  if (size1 < 32)
    {
      size1 = 32;
      sign1 = 1;
      newtype1 = integer32_type_node;
    }

  if (size2 < 32)
    {
      size2 = 32;
      sign2 = 1;
      newtype2 = integer32_type_node;
    }

  if (type1 != newtype1)
    newop1 = build1 (CONVERT_EXPR, newtype1, op1);
  else
    newop1 = op1;

  if (type2 != newtype2)
    newop2 = build1 (CONVERT_EXPR, newtype2, op2);
  else
    newop2 = op2;

  /* Set up the index to the function to call.  */

  ix = 0;

  if (size1 == 64)
    ix |= SIZE1_64;

  if (size2 == 64)
    ix |= SIZE2_64;
 
  if (sign1)
    ix |= SIGN1;

  if (sign2)
    ix += SIGN2;

  /* Build the decl if not done yet.  */

  if (expon_fndecls[ix] == NULL)
    {
      static const uchar *const fnnames[16] =
      {
        (uchar *)"blir_exponentiation_32_n_32_n",
        (uchar *)"blir_exponentiation_32_n_32_y",
        (uchar *)"blir_exponentiation_32_n_64_n",
        (uchar *)"blir_exponentiation_32_n_64_y",
        (uchar *)"blir_exponentiation_32_y_32_n",
        (uchar *)"blir_exponentiation_32_y_32_y",
        (uchar *)"blir_exponentiation_32_y_64_n",
        (uchar *)"blir_exponentiation_32_y_64_y",
        (uchar *)"blir_exponentiation_64_n_32_n",
        (uchar *)"blir_exponentiation_64_n_32_y",
        (uchar *)"blir_exponentiation_64_n_64_n",
        (uchar *)"blir_exponentiation_64_n_64_y",
        (uchar *)"blir_exponentiation_64_y_32_n",
        (uchar *)"blir_exponentiation_64_y_32_y",
        (uchar *)"blir_exponentiation_64_y_64_n",
        (uchar *)"blir_exponentiation_64_y_64_y"
      };
      tree fn_fn_type;
      tree fn_fn_decl;
      tree fn_identifier;
      tree param_type_list;
      tree decl1;
      
      fn_identifier = get_identifier ((native_char *)fnnames[ix]); 

      /* Types are consed in reverse order.  */

      param_type_list = tree_cons
        (NULL_TREE, void_type_node, NULL_TREE);  /* Signal normal not ...  parm list.  */
      
      param_type_list = tree_cons (NULL_TREE, newtype2, param_type_list);
      param_type_list = tree_cons (NULL_TREE, newtype1, param_type_list);

      fn_fn_type = build_function_type (newtype1, param_type_list);

       (decl1 = build_decl (PARM_DECL, get_identifier ("exp"), newtype2));

       (decl1 = build_decl (PARM_DECL, get_identifier ("data"), 
                       newtype1));

      lineno = ref_token->bli_token_lineno;
      fileno = ref_token->bli_token_fileno;
      input_filename = (native_char *)file_names->elem[fileno];

      fn_fn_decl = build_decl
        (FUNCTION_DECL, fn_identifier, fn_fn_type);
                    
      TREE_TYPE (fn_fn_decl) = fn_fn_type;
      DECL_EXTERNAL (fn_fn_decl) = 1;
      TREE_PUBLIC (fn_fn_decl) = 1;
      TREE_STATIC (fn_fn_decl) = 0;
      DECL_ARGUMENTS (fn_fn_decl) = getdecls ();
      DECL_RESULT (fn_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, newtype1);
      DECL_CONTEXT (DECL_RESULT (fn_fn_decl)) = NULL_TREE;
      TREE_USED (fn_fn_decl) = 1;

      rest_of_decl_compilation (fn_fn_decl, NULL, 1, 0);
      make_decl_rtl (fn_fn_decl, NULL);
      
      expon_fndecls[ix] = fn_fn_decl;
      expon_fntypes[ix] = fn_fn_type;
    }

  {
    tree args;
    tree fn_decl_ptr;
    
    fn_decl_ptr = build1
      (ADDR_EXPR, build_pointer_type (expon_fntypes[ix]), expon_fndecls[ix]);
    
    args = tree_cons (NULL_TREE, newop2, NULL_TREE); 
    args = chainon (tree_cons (NULL_TREE, newop1, NULL_TREE), args);
    
    call_expr = build (CALL_EXPR, newtype1, fn_decl_ptr, args, NULL_TREE);
  }
    
  return call_expr;
}

/* Return a name for a nonunique data item, with optional suffix SUFFIX.  */

static uchar nonunique_name_orig[] = "ppp9999999999ssssssssss";
static uint32 dup_suffix_length = 0;

static uchar *
get_nonunique_name (uchar *prefix, uchar *suffix, uint32 unique_number)
{
  static uint32 first_time = 1;
  uint32 suffix_length;

  BLI_ASSERT (unique_number != 0);
  
  BLI_ASSERT (strlen ((native_char *)prefix) == 3);

  if (first_time)
    {
      first_time = 0;
      nonunique_name_length = sizeof (nonunique_name_orig) / sizeof (nonunique_name_orig[0]);
      nonunique_name = BLI_ALLOC (nonunique_name_length);
      memcpy (nonunique_name, nonunique_name_orig, nonunique_name_length);
    }

  memcpy (nonunique_name, prefix, 3);
  sprintf ((native_char *)(nonunique_name +3), "%10.10"BLI_UINT32_FORMAT, unique_number);
  BLI_ASSERT (strlen ((native_char *)(nonunique_name +3)) == 10);
  suffix_length = strlen ((native_char *)suffix);
  BLI_ASSERT (suffix_length <= 10);
  memcpy (nonunique_name +13, suffix, suffix_length);
  nonunique_name[13 + suffix_length] = 0;
  return nonunique_name;
}


/* 
   
   Return an identifier resulting from taking each name in the
   hierarchy of the data item in upper case, and replacing the "-" in
   the name by "h" (hyphen), with each level separated by "of".  For
   example:

   01 www-www
     02 yyy-yyy.
       03 zzz-zzz.

   Gives ZZZhZZZofYYYhYYYofWWWhWWW for zzz-zzz.

   Redefines adds an extra reNAME where name is the redefined item's
   name (or itself if the current item is redefined).

   01 www-www
     02 yyy-yyy.
       03 zzz-zzz.
       03 aaa-aaa redefines zzz-zzz.

   Gives (void *)H(void *)reZZZhZZZofYYYhYYYofWWWhWWW for aaa-aaa.

   lll; this does not support duplicate names which BLISS does require.  */

tree
get_fixed_hierarchy_identifier (struct bli_tree_struct_data_item *data_item, uchar *suffix)
{
  static uint32 first_time = 1;
  uint32 current_length;
  struct bli_tree_struct_data_item *current_data_item;
  tree ret1;
  uint32 suffix_length;

  suffix_length = strlen ((native_char *)suffix);

  if (first_time)
    {
      first_time = 0;
      nonunique_name_length = sizeof (nonunique_name_orig) / sizeof (nonunique_name_orig[0]);
    }

  /* 1. Compute the length.  */
  current_length = suffix_length;
  current_data_item = data_item;
  for (; current_data_item; )
    {
      if (current_data_item->data_name_token)
        {
          current_length += current_data_item->data_name_token->string_details->length;
          if (current_data_item->duplicate_notfirst_flag)
            {
              if (!dup_suffix_length)
                {
                  dup_suffix_length = (uint32)(strchr ((native_char *)nonunique_name_orig, 's')
                                                 - (native_char *)nonunique_name_orig);
                  BLI_ASSERT (dup_suffix_length > 0);
                  BLI_ASSERT (dup_suffix_length < 20);
                }
              current_length += dup_suffix_length;
            }
        }
      else
        current_length += nonunique_name_length;

      current_data_item = (void *) current_data_item->owner;
    }
  
  /* 2. Allocate storage.  */
  if ((current_length + suffix_length) >= work_string_length)
    {
      work_string_length = current_length +1 + suffix_length +200;
      work_string = BLI_ALLOC (work_string_length);
    }

  /* 3. Concatenate.  */
  current_length = 0;
  current_data_item = data_item;
  for (; current_data_item; )
    {
      memcpy (work_string + current_length, (current_data_item->redefine_master_flag) ? "re" : "of", 2);
      current_length += 2;

      memcpy (work_string + current_length, 
             current_data_item->data_name_token->string_details->string_upper->string,
             current_data_item->data_name_token->string_details->length);
      current_length += current_data_item->data_name_token->string_details->length;
      
      current_data_item = (void *) current_data_item->owner;
    }
  memcpy (work_string + current_length, suffix, suffix_length);
  current_length += suffix_length;

  /* 4. Get it and return, ignoring first 'of' which is redundant.  */
  
  ret1 = get_identifier ((native_char *)get_fixed_string (work_string +2, current_length -2, (uchar *)""));

  return ret1;
  
}

/* Return a tree identifier resulting from converting to upper case
 and replacing the "-" in a token's name by "h" and adding SUFFIX on
 the end.  UNIQUE_ADDRESS is used for fillers as a random unique suffix
 which goes before the SUFFIX. DUPFLAG is true if the unique address
 is to be used in the data name as it is not unique.  */

static tree
get_fixed_identifier (struct bli_token_struct *name_token, 
                      uchar *suffix,
                     void *unique_address, uint32 dupflag)
{
  return get_identifier ((native_char *)
                         get_fixed_name (name_token, suffix,
                                         (uint32)unique_address, dupflag));
}
     
/* Return a string resulting from replacing the "-" in a token's name
 by "$", otherwise in upper case.  EG A-B-C ==> "A$B$C" and adding 
 SUFFIX on the end.  UNIQUE_NUMBER is used for fillers as a random unique suffix
 which goes before the SUFFIX. DUPFLAG is true if the unique address
 is to be used in the data name as it is not unique.  */

static uchar *
get_fixed_name (struct bli_token_struct *name_token, 
               uchar *suffix, uint32 unique_number, uint32 dupflag)
{
  if ((!name_token) || (name_token->pfx.type.tok_type == FILLER))
    return get_nonunique_name ((uchar *)"flr", (uchar *) suffix, unique_number);

  if (dupflag)
    suffix = get_nonunique_name ((uchar *)"dup", (uchar *) suffix, unique_number); 
  return get_fixed_string (name_token->string_details->string_upper->string,
                          name_token->string_details->length, suffix);
}

/* Return a new string resulting from replacing the "-" in STRING's
   name by "$" (STRING is a string of length LENGTH) , otherwise as
   is.  EG A-B-C ==> "A$B$C" and adding SUFFIX on the end.  */

static uchar *
get_fixed_string (uchar *string, uint32 length, uchar *suffix)
{

  uint32 ix;
  uint32 suffix_length;

  suffix_length = strlen ((native_char *)suffix);

  if ((length + suffix_length) >= fixed_id_length)
    {
      fixed_id = BLI_ALLOC (length + suffix_length +100);
      fixed_id_length = length + suffix_length +100;
    }

  for (ix = 0; ix < length; ix ++)
    {
      int32 c;
      c = string[ix];
      fixed_id[ix] = ((c == '-') ? '$' : c);
    }
  strcpy ((native_char *)fixed_id + length, (native_char *)suffix);

  fixed_id[length + suffix_length] = 0;
  return fixed_id;
}

/* Get the type for a fixed length type of usage USAGE; signed
   version if UNSIGNED_FLAG!=0 (ignored for types like float that are
   always signed).  */

static tree
type_for_fixed_length_type (int32 usage, int32 unsigned_flag)
{
  switch (usage)
    {
    case BINARY_CHAR :
      return /* (*lang_hooks.types.type_for_size) */ type_for_size (BLI_INT8_WIDTH * BLI_BITS_PER_BYTE, unsigned_flag);
      break;                                                                       
                                                                                   
    case BINARY_SHORT :                                                            
      return /* (*lang_hooks.types.type_for_size) */ type_for_size (BLI_INT16_WIDTH * BLI_BITS_PER_BYTE, unsigned_flag);
      break;                                                                       
                                                                                   
    case BINARY_LONG :                                                             
      return /* (*lang_hooks.types.type_for_size) */ type_for_size (BLI_INT32_WIDTH * BLI_BITS_PER_BYTE, unsigned_flag);
      break;                                                                       
                                                                                   
    case BINARY_DOUBLE :                                                           
      return /* (*lang_hooks.types.type_for_size) */ type_for_size (BLI_INT64_WIDTH * BLI_BITS_PER_BYTE, unsigned_flag);
      break;
      
    default :
      BLI_ASSERT (0 && "unknown type");
      
    }
  return NULL_TREE; /* Keep GCC happy.  */
}

/* Extend an expression EXP to truncate it to DIGITS decimal digits,
   and return the new expression.  The type of EXP must be large
   enough to hold that number of digits.  */

static tree
truncate_to_digits (tree exp, uint32 digits)
{
  tree modval;
  tree modexp;
  uint32 size_of_exp;
  uint64 nines = 1;
  uint32 lo;
  uint32 hi;
  
  BLI_ASSERT (digits < 20);
  BLI_ASSERT (digits > 0);
  
  /* Set of the constants required for this one.  */
  if (!limits_trees_mod[digits])
    {
      uint32 i;

      for (i = 1; i <= digits; i ++)
        nines = nines * 10;

      lo = BLI_INT32_MASK & nines;
      hi = BLI_INT32_MASK & (nines >> BLI_INT32_SHIFT_TO_LOW);
      
      limits_trees_mod[digits] = build_int_2 (lo, hi);
      TREE_TYPE (limits_trees_mod[digits]) = integer64_unsigned_type_node;
      
      if (nines < 0x100)
        limits_trees_sizes[digits] = 1;
      else
        if (nines < 0x10000)
        limits_trees_sizes[digits] = 2;
      else
        if (!hi)
          limits_trees_sizes[digits] = 4;
        else
          limits_trees_sizes[digits] = 8;
      
    }
  
  size_of_exp = get_uint32_value (TYPE_SIZE (TREE_TYPE (exp)), LOG_BLI_BITS_PER_BYTE);

  if (limits_trees_sizes[digits] > size_of_exp)
    /* Cannot even fit the number of digits needed to be too big, so
       cannot be too big.  */
    return exp;
  
  modval = build1 (CONVERT_EXPR, TREE_TYPE (exp), limits_trees_mod[digits]); 
  modexp = build (TRUNC_MOD_EXPR, TREE_TYPE (exp), exp, modval); 
  return modexp;
}

/* Output code to call a function called NAME with no arguments, returning nothing.  */

static void
call_void_function_no_args (const uchar *name, struct bli_token_struct *ref_token)
{

  int32 fileno = 0;
  tree call_expr;
  tree bli_identifier = NULL_TREE;
  tree bli_fn_decl = NULL_TREE;
  tree bli_fn_decl_ptr = NULL_TREE;
  tree bli_fn_type = NULL_TREE;
                
  bli_identifier = get_identifier ((native_char *)name); 
  
  bli_fn_type = build_function_type (void_type_node, NULL_TREE);
  
  lineno = ref_token->bli_token_lineno;
  fileno = ref_token->bli_token_fileno;
  input_filename = (native_char *)file_names->elem[fileno];

  bli_fn_decl = build_decl
    (FUNCTION_DECL, bli_identifier, bli_fn_type);
  
  TREE_TYPE (bli_fn_decl) = bli_fn_type;
  DECL_EXTERNAL (bli_fn_decl) = 1;
  TREE_PUBLIC (bli_fn_decl) = 1;
  TREE_STATIC (bli_fn_decl) = 0;
  DECL_ARGUMENTS (bli_fn_decl) = NULL_TREE;
  DECL_RESULT (bli_fn_decl) = build_decl (RESULT_DECL, NULL_TREE, void_type_node);
  DECL_CONTEXT (DECL_RESULT (bli_fn_decl)) = NULL_TREE;
  TREE_USED (bli_fn_decl) = 1;
  
  rest_of_decl_compilation (bli_fn_decl, NULL, 1, 0);
  make_decl_rtl (bli_fn_decl, NULL);
  
  bli_fn_decl_ptr = build1
    (ADDR_EXPR, build_pointer_type (bli_fn_type), bli_fn_decl);

  call_expr = build (CALL_EXPR, void_type_node, bli_fn_decl_ptr, NULL_TREE, NULL_TREE);
  TREE_USED (call_expr) = 1;
  TREE_SIDE_EFFECTS (call_expr) = 1;

  expand_expr_stmt (call_expr);
}


/* Output a call to memset, address is TO, fill is CHAR1 length is LENGTH.  */

static tree
output_memset (tree to, tree char1, tree length)
{
  tree args;
  tree memset_decl_ptr;
  tree call_expr;

  args = tree_cons (NULL_TREE, length, NULL_TREE); 
  args = chainon (tree_cons (NULL_TREE, char1, NULL_TREE), args);
  args = chainon (tree_cons (NULL_TREE, to, NULL_TREE), args);
  
  memset_decl_ptr = build1
    (ADDR_EXPR, build_pointer_type (TREE_TYPE (memset_decl)), memset_decl);
  
  call_expr = build (CALL_EXPR, void_type_node, memset_decl_ptr, args, NULL_TREE);
  TREE_USED (call_expr) = 1;
  TREE_SIDE_EFFECTS (call_expr) = 1;
  
  expand_expr_stmt (call_expr);
}
#endif

/* The reserved keyword table.  */
struct resword
{
  const char *word;
  ENUM_BITFIELD(rid) rid : 16;
  unsigned int disable   : 16;
};

static const struct resword reswords[] =
{
  { "_Bool",		RID_BOOL,	0 },
  { "_Complex",		RID_COMPLEX,	0 },
  { "__FUNCTION__",	RID_FUNCTION_NAME, 0 },
  { "__PRETTY_FUNCTION__", RID_PRETTY_FUNCTION_NAME, 0 },
  { "__alignof",	RID_ALIGNOF,	0 },
  { "__alignof__",	RID_ALIGNOF,	0 },
  { "__asm",		RID_ASM,	0 },
  { "__asm__",		RID_ASM,	0 },
  { "__attribute",	RID_ATTRIBUTE,	0 },
  { "__attribute__",	RID_ATTRIBUTE,	0 },
  { "__bounded",	RID_BOUNDED,	0 },
  { "__bounded__",	RID_BOUNDED,	0 },
  { "__builtin_choose_expr", RID_CHOOSE_EXPR, 0 },
  { "__builtin_types_compatible_p", RID_TYPES_COMPATIBLE_P, 0 },
  { "__builtin_va_arg",	RID_VA_ARG,	0 },
  { "__complex",	RID_COMPLEX,	0 },
  { "__complex__",	RID_COMPLEX,	0 },
  { "__const",		RID_CONST,	0 },
  { "__const__",	RID_CONST,	0 },
  { "__extension__",	RID_EXTENSION,	0 },
  { "__func__",		RID_C99_FUNCTION_NAME, 0 },
  { "__imag",		RID_IMAGPART,	0 },
  { "__imag__",		RID_IMAGPART,	0 },
  { "__inline",		RID_INLINE,	0 },
  { "__inline__",	RID_INLINE,	0 },
  { "__label__",	RID_LABEL,	0 },
  { "__ptrbase",	RID_PTRBASE,	0 },
  { "__ptrbase__",	RID_PTRBASE,	0 },
  { "__ptrextent",	RID_PTREXTENT,	0 },
  { "__ptrextent__",	RID_PTREXTENT,	0 },
  { "__ptrvalue",	RID_PTRVALUE,	0 },
  { "__ptrvalue__",	RID_PTRVALUE,	0 },
  { "__real",		RID_REALPART,	0 },
  { "__real__",		RID_REALPART,	0 },
  { "__restrict",	RID_RESTRICT,	0 },
  { "__restrict__",	RID_RESTRICT,	0 },
  { "__signed",		RID_SIGNED,	0 },
  { "__signed__",	RID_SIGNED,	0 },
  { "__typeof",		RID_TYPEOF,	0 },
  { "__typeof__",	RID_TYPEOF,	0 },
  { "__unbounded",	RID_UNBOUNDED,	0 },
  { "__unbounded__",	RID_UNBOUNDED,	0 },
  { "__volatile",	RID_VOLATILE,	0 },
  { "__volatile__",	RID_VOLATILE,	0 },
  { "asm",		RID_ASM,	0 },
  { "auto",		RID_AUTO,	0 },
  { "break",		RID_BREAK,	0 },
  { "case",		RID_CASE,	0 },
  { "char",		RID_CHAR,	0 },
  { "const",		RID_CONST,	0 },
  { "continue",		RID_CONTINUE,	0 },
  { "default",		RID_DEFAULT,	0 },
  { "do",		RID_DO,		0 },
  { "double",		RID_DOUBLE,	0 },
  { "else",		RID_ELSE,	0 },
  { "enum",		RID_ENUM,	0 },
  { "extern",		RID_EXTERN,	0 },
  { "float",		RID_FLOAT,	0 },
  { "for",		RID_FOR,	0 },
  { "goto",		RID_GOTO,	0 },
  { "if",		RID_IF,		0 },
  { "inline",		RID_INLINE,	0 },
  { "int",		RID_INT,	0 },
  { "long",		RID_LONG,	0 },
  { "register",		RID_REGISTER,	0 },
  { "restrict",		RID_RESTRICT,	0 },
  { "return",		RID_RETURN,	0 },
  { "short",		RID_SHORT,	0 },
  { "signed",		RID_SIGNED,	0 },
  { "sizeof",		RID_SIZEOF,	0 },
  { "static",		RID_STATIC,	0 },
  { "struct",		RID_STRUCT,	0 },
  { "switch",		RID_SWITCH,	0 },
  { "typedef",		RID_TYPEDEF,	0 },
  { "typeof",		RID_TYPEOF,	0 },
  { "union",		RID_UNION,	0 },
  { "unsigned",		RID_UNSIGNED,	0 },
  { "void",		RID_VOID,	0 },
  { "volatile",		RID_VOLATILE,	0 },
  { "while",		RID_WHILE,	0 },
};
#define N_reswords (sizeof reswords / sizeof (struct resword))

/* Init enough to allow the C decl code to work, then clean up
   afterwards.  */

void
bliss_init_decl_processing ()
{
  unsigned int i;
  tree id;

  ridpointers = (tree *) xcalloc ((int) RID_MAX, sizeof (tree));
  
  for (i = 0; i < N_reswords; i++)
    {
      id = get_identifier (reswords[i].word);
      C_RID_CODE (id) = reswords[i].rid;
      C_IS_RESERVED_WORD (id) = 1;
      ridpointers [(int) reswords[i].rid] = id;
    }

  c_init_decl_processing ();

  /* ix86_return_pops_args takes the type of these so need to patch
     their own type as themselves.  */

  for (i = 0; i < itk_none; i++)
    {
      if (integer_types[i])
        TREE_TYPE (integer_types [i]) = integer_types[i];
    }

  /* Probably these ones too.  */
  TREE_TYPE (float_type_node) = float_type_node;
  TREE_TYPE (double_type_node) = double_type_node;
  TREE_TYPE (long_double_type_node) = long_double_type_node;

  void_pointer_type_node = build_pointer_type (void_type_node);
  char_pointer_type_node = build_pointer_type (char_type_node);

  integer8_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (8, 0);
  integer8_unsigned_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (8, 1);
  integer16_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (16, 0);
  integer16_unsigned_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (16, 1);
  integer32_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (32, 0);
  integer32_unsigned_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (32, 1);
  integer64_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (64, 0);
  integer64_unsigned_type_node = /* (*lang_hooks.types.type_for_size) */ type_for_size (64, 1);

#if BLI_POINTER_SIZE == 32 
  integer_pointer_type_node = integer32_type_node;
#elif BLI_POINTER_SIZE == 64
  integer_pointer_type_node = integer64_type_node;
#else
#error unsupported pointer size not 32 or 64 bits 
#endif

  /* Make an int out of space.  */
  space_exp = build_int_2 ((unsigned char)' ', 0);  /* Assumes ascii.  */

  memset_decl = built_in_decls[BUILT_IN_MEMSET]; 
  memcpy_decl = built_in_decls[BUILT_IN_MEMCPY]; 
  memcmp_decl = built_in_decls[BUILT_IN_MEMCMP]; 
  BLI_ASSERT (memset_decl && memcpy_decl && memcmp_decl);
}

/* Following  from c-lang.c.  */

/* Tell the c code we are not objective C.  */

int
maybe_objc_comptypes (tree lhs ATTRIBUTE_UNUSED,
                      tree rhs ATTRIBUTE_UNUSED,
                      int reflexive ATTRIBUTE_UNUSED)
{
  return -1;
}

/* Used by c-typeck.c (build_external_ref), but only for objc.  */

tree
lookup_objc_ivar (tree id ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Dummy routines called from c code. Save copying c-decl.c, c-common.c etc.  */

void
check_function_format (int *status ATTRIBUTE_UNUSED,
                       tree attrs ATTRIBUTE_UNUSED,
                       tree params ATTRIBUTE_UNUSED)
{
  return;
}

/* Tell the c code we are not objective C.  */

tree
maybe_building_objc_message_expr ()
{
  return 0;
}

/* Should not be called for bliss.   */

tree
build_stmt VPARAMS ((enum tree_code code  ATTRIBUTE_UNUSED, ...))
{
  //  abort ();
  tree t;
  int length;
  int i;

  VA_OPEN (p, code);
  VA_FIXEDARG (p, enum tree_code, code);

  t = make_node (code);
  length = TREE_CODE_LENGTH (code);
  STMT_LINENO (t) = lineno;

  for (i = 0; i < length; i++)
    TREE_OPERAND (t, i) = va_arg (p, tree);

  VA_CLOSE (p);
  return t;
}

/* Should not be called for bliss.   */

tree
add_stmt (tree t ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

tree
build_return_stmt (tree expr ATTRIBUTE_UNUSED)
{
  abort ();
}

/* C warning, ignore.  */

void
pedwarn_c99 VPARAMS ((const char *msgid ATTRIBUTE_UNUSED, ...))
{
  return;
}

/* Should not be called for bliss.   */

tree
build_case_label (tree low_value ATTRIBUTE_UNUSED,
                  tree high_value ATTRIBUTE_UNUSED,
                  tree label_decl ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
emit_local_var (tree decl ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
expand_stmt (tree t ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

cpp_reader *
cpp_create_reader (enum c_lang lang ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
cpp_post_options (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
cpp_preprocess_file (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

const char *
init_c_lex (const char *filename ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void init_pragma (void);

void
init_pragma ()
{
  abort ();
}

/* Should not be called for bliss.   */

void
cpp_finish (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

unsigned int
cpp_errors (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

tree
handle_format_attribute (tree *node ATTRIBUTE_UNUSED,
                         tree name ATTRIBUTE_UNUSED,
                         tree args ATTRIBUTE_UNUSED,
                         int flags ATTRIBUTE_UNUSED,
                         bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  //  abort ();
}

/* Should not be called for bliss.   */

tree
handle_format_arg_attribute (tree *node ATTRIBUTE_UNUSED,
     tree name ATTRIBUTE_UNUSED,
     tree args ATTRIBUTE_UNUSED,
     int flags ATTRIBUTE_UNUSED,
     bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

int
cpp_handle_option (cpp_reader *pfile ATTRIBUTE_UNUSED,
     int argc ATTRIBUTE_UNUSED,
     char **argv ATTRIBUTE_UNUSED, int i)
{
  abort ();
}

/* Should not be called for bliss.   */

void 
cpp_assert (cpp_reader * cr ATTRIBUTE_UNUSED, 
            const char *s ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
set_Wformat (int setting ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
maybe_objc_check_decl (tree decl ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
gen_aux_info_record (tree fndecl ATTRIBUTE_UNUSED,
                     int is_definition ATTRIBUTE_UNUSED,
                     int is_implicit ATTRIBUTE_UNUSED,
                     int is_prototyped ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss, but it is.   */

void
c_parse_init ()
{
  fprintf(stderr,"in c_parse_init\n");
  return;
}

/* Should not be called for bliss.   */

void maybe_apply_pragma_weak (tree decl);

void
maybe_apply_pragma_weak (tree decl ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
add_decl_stmt (tree decl ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

tree
maybe_apply_renaming_pragma (tree decl, tree asmname);

/* Should not be called for bliss.   */

tree
maybe_apply_renaming_pragma (tree decl ATTRIBUTE_UNUSED, tree asmname ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void
begin_stmt_tree (tree *t ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */
extern struct function *cfun;
void
finish_stmt_tree (t)
     tree *t;
{
  tree stmt;
  
  /* Remove the fake extra statement added in begin_stmt_tree.  */
  stmt = TREE_CHAIN (*t);
  *t = stmt;
  last_tree = NULL_TREE;

  if (cfun && stmt)
    {
      /* The line-number recorded in the outermost statement in a function
         is the line number of the end of the function.  */
      STMT_LINENO (stmt) = lineno;
      STMT_LINENO_FOR_FN_P (stmt) = 1;
    }
}

/* Should not be called for bliss.   */

int
defer_fn (tree fn ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

cpp_options 
*cpp_get_options (cpp_reader * cr ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Should not be called for bliss.   */

void 
cpp_define (cpp_reader * cr ATTRIBUTE_UNUSED, const char * c ATTRIBUTE_UNUSED)
{
  abort ();  
}

/* Should not be called for bliss.   */

cpp_callbacks *
cpp_get_callbacks (cpp_reader * cr ATTRIBUTE_UNUSED)
{
  abort ();
}

/* Flags etc required by c code.  */

int warn_format = 0;
int warn_format_y2k = 0;
int warn_format_extra_args = 0;
int warn_format_nonliteral = 0;
int warn_format_security = 0;
int warn_format_zero_length = 0;

/* Save typing debug_tree all the time. Dump a tree T pretty and
   concise.  */

void dt (tree t);

void
dt (tree t)
{
  debug_tree (t);
}
