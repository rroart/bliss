/* 
   
   BLISS Compiler parser symbol table and parse tree (blicsyt.c)

   -------------------------------------------------------------------
   
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
   
   -------------------------------------------------------------------.  */

#include "blir_temp_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "ggc.h"

#include "blidebug.h"
#include "bliumsc.h"
#include "bliumem.h"
#include "bliumsg.h"
#include "bliclex.h"
#include "blicc1.h"
#include "blicsymn.h"
#include "blicsyt.h"
#include "blicpru.h"

static uint32 program_count = 0;

#define MAGIC 000777000777u

/* Symbol table hash table.  */
static struct symtab_entry * *  GTY((length ("work_string_length"))) symbol_table_hash = NULL;

/* Hash table size.  */
static uint32 symbol_table_hash_entries = 0;

struct bli_tree_struct_program * GTY(()) top_level_program = NULL;

/* Create the symbol table.  TOKEN_COUNT is a hint how big it needs to be to avoid too many clashes.  */
void
create_symbol_table (uint32 token_count)
{
  BLI_ASSERT (!symbol_table_hash);
  symbol_table_hash_entries = (token_count < 10) ? 10 : token_count;
  symbol_table_hash = BLI_ALLOC (symbol_table_hash_entries * sizeof (void *));
 }

/* Cleanup the symbol table.  Any non global items will be chopped.  */
void
cleanup_symbol_table ()
{
  uint32 symbol_hash_ix;
  void **store_ptr_here;
  struct symtab_entry *current_item;

  /* Link the still used items; unlink the others.  ggc will free them if it needs to.  */
  for (symbol_hash_ix = 0; symbol_hash_ix < symbol_table_hash_entries; symbol_hash_ix ++)
    {
      store_ptr_here = (void **)symbol_table_hash + symbol_hash_ix;
      for (current_item = symbol_table_hash[symbol_hash_ix];
           current_item; current_item = current_item->symtab_hash_next)
          if (current_item->symtab_entry_flags & SYMBOL_TABLE_FLAG_GLOBAL)
            {
              * store_ptr_here = current_item;
              store_ptr_here = (void **) & (current_item->symtab_hash_next);
            }
    }
}

/* Tell symbol table that we have a new program, so old symbols are obsolete.  */

void symbol_table_new_pgm ()
{
  program_count ++;
  cleanup_symbol_table ();
}

/* Add entry type TYPE, with token or data_item or label ITEM , to the symbol table.  */

void *
create_symtab_entry (enum symtab_entry_type_enum type, void *item)
{

  struct symtab_entry *st; 
  uint32 index; 
  struct bli_token_struct *tk;
  struct bli_tree_struct_data_item *di;
  struct bli_tree_struct_label *lb;
  uchar *my_name;
  uint32 my_name_length;

  BLI_ASSERT (symbol_table_hash); 

  st = BLI_ALLOC (sizeof (struct symtab_entry));

  st->magic = MAGIC;
  st->symtab_entry_type = type;
  st->symtab_entry_item = item;

  BLI_ASSERT (item);

  tk = item;
  
  if (tk->pfx.cat != type_token)
    {
      BLI_ASSERT (tk->pfx.cat == type_production);
      if (tk->pfx.type.tok_type == BLI_PROD_TYPE_DATA_ITEM)
        {
          di = item;
          tk = di->data_name_token;
        }
      else
        {
          lb = item;
          BLI_ASSERT ((tk->pfx.type.tok_type == BLI_PROD_TYPE_PARAGRAPH)
                 || (tk->pfx.type.tok_type == BLI_PROD_TYPE_SECTION));
          tk = lb->pfx.reference_token;
        }
    }

  /* Do not add fillers to the hash table.  */
  if ((!tk) || (tk->pfx.type.tok_type == FILLER))
    return st;
  
  my_name = tk->string_details->string_upper->string;
  my_name_length = tk->string_details->length;

  st->symtab_entry_name = my_name;
  st->symtab_entry_name_length = my_name_length;
  
  index = bliumsc_hash_string (my_name, my_name_length) % symbol_table_hash_entries;
  
  st->symtab_hash_next = symbol_table_hash[index];
  symbol_table_hash[index] = st;

  return st;
}

/* Overwrite attributes FLAGS_MASK with new values FLAGS of symbol
   table entry ENTRY.  */

void
put_symtab_entry_attribute_flags (void *entry, uint32 flags,
				  uint32 flags_mask)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st->symtab_entry_flags &= ~flags_mask;
  st->symtab_entry_flags |= (flags & flags_mask);

}

/* Obtain the attributes FLAGS_MASK of symbol table entry ENTRY.  */

uint32
get_symtab_entry_attribute_flags (void *entry, uint32 flags_mask)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  return (st->symtab_entry_flags & flags_mask);

}

/* Get the payload from symbol table entry ENTRY.  */

void *
get_symtab_entry_item (void *entry)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);
  BLI_ASSERT (st->symtab_entry_item);
  return st->symtab_entry_item;  /* Null for builtin function.  */
}

/* Get type of sumbol table entry ENTRY.  */

enum symtab_entry_type_enum
get_symtab_entry_type (void *entry)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  return st->symtab_entry_type;

}

/* Get next symbol table entry up in hierarchy from ENTRY.  */

void *
get_symtab_entry_owner (void *entry)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  return st->symtab_entry_owner;

}

/* Set hierarchical owner of symbol table entry ENTRY to OWNER.  */

void
set_symtab_entry_owner (void *entry, void *owner)
{

  struct symtab_entry *st;
  struct symtab_entry *st2;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st2 = owner;
  BLI_ASSERT (st2);
  BLI_ASSERT (st2->magic == MAGIC);

  st->symtab_entry_owner = owner;

}

/* Set object of symbol table entry ENTRY to OBJECT.  */

void
set_symtab_entry_item (void *entry, void *item)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st->symtab_entry_item = item;

}

/* Find the (first synonym) symbol table entry for NAME of length NAME_LENGTH.  */

void *
find_symtab_entry (uchar *name, uint32 name_length)
{

  struct symtab_entry *st;
  uint32 index;

  BLI_ASSERT (symbol_table_hash);

  index = bliumsc_hash_string (name, name_length) % symbol_table_hash_entries;

  for (st = symbol_table_hash[index]; st; st = st->symtab_hash_next)
    {
      BLI_ASSERT (st->magic == MAGIC);
      if (name_length != st->symtab_entry_name_length)
	continue;
      if (memcmp (name, st->symtab_entry_name, name_length))
	continue;
      return st;
    }

  return NULL;

}

/* Find the next synonym symbol table entry for NAME of length NAME_LENGTH.  */

void *
find_next_symtab_entry (void *entry, uchar *name, uint32 name_length)
{

  struct symtab_entry *st;

  st = entry;
  BLI_ASSERT (st);
  BLI_ASSERT (st->magic == MAGIC);

  st = st->symtab_hash_next;

  for (; st; st = st->symtab_hash_next)
    {
      BLI_ASSERT (st->magic == MAGIC);
      if (name_length != st->symtab_entry_name_length)
	continue;
      if (memcmp (name, st->symtab_entry_name, name_length))
	continue;
      return st;
    }
  return NULL;

}

/* Find an entry in the symbol table which matches not just one name
   but the whole name hierarchy.  NBR_NAMES is the number of names in
   the hierarchy NAMES, and NAME_LENGTHS are the lengths of each NAMES
   entry, returns 0,1,2 depending on 0 1 or more matches and returns
   the individual entry in *ENTRY. Entry must be of type TYPE to
   match.  In the arrays the most subordinate entry is the [0] entry
   (ie the highest BLISS level number). The caller needs to make sure
   that the lowest entry is not a filler.  */

uint32 /* 0==Not found 1=found unique 2=found not unique.  */
find_symtab_entry_hierarchy (uint32 nbr_names, uchar **names, uint32 *name_lengths,
			     /* Return.  */void **entry, enum symtab_entry_type_enum type)
{
  struct symtab_entry *st;
  struct symtab_entry *st2;
  uint32 match_count = 0;
  uint32 name_ix;

  BLI_ASSERT (nbr_names > 0);
  * entry = NULL;

  BLI_ASSERT (name_lengths[0] != 0);  /* Should not be filler.  */

  /* For each possible match.  */
  for (st = find_symtab_entry (names[0], name_lengths[0]);  /* Per matching lowest entry.  */
       st; st = find_next_symtab_entry (st, names[0], name_lengths[0]))
    {
      if (type)
        if (st->symtab_entry_type != type)
          continue;
      /* For each element in the names array.  */
      for (st2 = st->symtab_entry_owner, name_ix = 1;  /* Per element in list.  */
	   name_ix < nbr_names; name_ix ++)
	{
	  uint32 success = 0;
          if (name_lengths[name_ix] == 0) /* Filler.  */
            continue;
          /* Scanning up in the data item hierarchy.  */
	  for (; st2 && (!success); st2 = st2->symtab_entry_owner)
            if (st2->symtab_entry_name_length &&  /* Case of filler.  */
                (st2->symtab_entry_name_length == name_lengths[name_ix]) &&
                (!memcmp (st2->symtab_entry_name, names[name_ix],
                          name_lengths[name_ix])))
              success = 1;
	  if (!success)
            break; 
	}
      if (name_ix == nbr_names)
	{
          if (!match_count)
            * entry = st;  /* First match found.  */
	  match_count ++;
	}
    }
  return match_count > 2 ? 2 : match_count;
}

/* Find next entry in the symbol table after ENTRY which matches not
   just one name but the whole name hierarchy.  NBR_NAMES is the number
   of names in the hierarchy NAMES, and NAME_LENGTHS are the lengths
   of each NAMES entry, returns 0 fi no match found, 1 if a match was
   found and returns the individual entry in *ENTRY. Entry must be of
   type TYPE to match if TYPE is non zero.  In the arrays the most
   subordinate entry is the [0] entry (ie the highest BLISS level
   number). The caller needs to make sure that the lowest entry is not
   a filler.  */

uint32 /* 0==Not found 1=found one or more.  */
find_next_symtab_entry_hierarchy (uint32 nbr_names, 
                                  uchar **names, uint32 *name_lengths,
                                  /* Input and return.  */void **entry, 
                                  enum symtab_entry_type_enum type)
{
  struct symtab_entry *st;
  struct symtab_entry *st2;
  uint32 name_ix;

  BLI_ASSERT (nbr_names > 0);
  * entry = NULL;

  BLI_ASSERT (name_lengths[0] != 0);  /* Should not be filler.  */

  for (st = find_next_symtab_entry (entry, names[0], name_lengths[0]);  /* Per matching lowest entry.  */
       st; st = find_next_symtab_entry (st, names[0], name_lengths[0]))
    {
      if (type)
        if (st->symtab_entry_type != type)
          continue;
      for (st2 = st->symtab_entry_owner, name_ix = 1;  /* Per element in list.  */
	   name_ix < nbr_names; name_ix ++)
	{
	  uint32 success = 0;
          if (name_lengths[name_ix] == 0) /* Filler.  */
            continue;
	  for (; st2 && (!success); st2 = st2->symtab_entry_owner)
            if (st2->symtab_entry_name_length &&  /* Case of filler.  */
                (st2->symtab_entry_name_length == name_lengths[name_ix]) &&
                (!memcmp (st2->symtab_entry_name, names[name_ix],
                          name_lengths[name_ix])))
              success = 1;
	  if (!success)
            break;
	}
      if (name_ix == nbr_names)
	{
	  * entry = st;  /* First match in file.  */
          break;
	}
    }
  return * entry != NULL;
}

/* Issue an error and return 1 if a symbol for token TOKEN is used in some
   other namespace than TYPE.  */

uint32
check_symtab_entry_namespace (struct bli_token_struct *token, enum symtab_entry_type_enum type)
{
  void *ste;
  enum symtab_entry_type_enum tab_type;
  for (ste = find_symtab_entry (token->string_details->string_upper->string, token->string_details->length);
      ste;
      ste = find_next_symtab_entry
        (ste, token->string_details->string_upper->string, token->string_details->length))
    {
      tab_type = get_symtab_entry_type (ste);
      if (tab_type != type)
        {
          /* Used in >1 name space.  */
          MSG_TOKEN (MSGID (165), token, NULL);
          return 1;
        }
    }
  return 0;
}


// borrowed stuff from ..

/* Process declarations and variables for C compiler.
   Copyright (C) 1988, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
   2001, 2002 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

/* Process declarations and symbol lookup for C front end.
   Also constructs types; the standard scalar types at initialization,
   and structure, union, array and enum types when they are declared.  */

/* ??? not all decl nodes are given the most useful possible
   line numbers.  For example, the CONST_DECLs for enum values.  */


#include "system.h"
#include "intl.h"
#include "tree.h"
#include "tree-inline.h"
#include "rtl.h"
#include "flags.h"
#include "function.h"
#include "output.h"
#include "expr.h"
#include "c-tree.h"
#include "c-lex.h"
#include "toplev.h"
#include "tm_p.h"
#include "cpplib.h"
#include "target.h"
#include "debug.h"
#include "timevar.h"
#include "c-common.h"
#include "c-pragma.h"

/* In grokdeclarator, distinguish syntactic contexts of declarators.  */
enum decl_context
{ NORMAL,			/* Ordinary declaration */
  FUNCDEF,			/* Function definition */
  PARM,				/* Declaration of parm before function body */
  FIELD,			/* Declaration inside struct or union */
  BITFIELD,			/* Likewise but with specified width */
  TYPENAME};			/* Typename (inside cast or sizeof)  */


/* Nonzero if we have seen an invalid cross reference
   to a struct, union, or enum, but not yet printed the message.  */

tree pending_invalid_xref;
/* File and line to appear in the eventual error message.  */
const char *pending_invalid_xref_file;
int pending_invalid_xref_line;

/* While defining an enum type, this is 1 plus the last enumerator
   constant value.  Note that will do not have to save this or `enum_overflow'
   around nested function definition since such a definition could only
   occur in an enum value expression and we don't use these variables in
   that case.  */

static tree enum_next_value;

/* Nonzero means that there was overflow computing enum_next_value.  */

static int enum_overflow;

/* Parsing a function declarator leaves a list of parameter names
   or a chain or parameter decls here.  */

static tree last_function_parms;

/* Parsing a function declarator leaves here a chain of structure
   and enum types declared in the parmlist.  */

static tree last_function_parm_tags;

/* After parsing the declarator that starts a function definition,
   `start_function' puts here the list of parameter names or chain of decls.
   `store_parm_decls' finds it here.  */

static tree current_function_parms;

/* Similar, for last_function_parm_tags.  */
static tree current_function_parm_tags;

/* Similar, for the file and line that the prototype came from if this is
   an old-style definition.  */
static const char *current_function_prototype_file;
static int current_function_prototype_line;

/* The current statement tree.  */

static struct stmt_tree_s c_stmt_tree;

/* The current scope statement stack.  */

static tree c_scope_stmt_stack;

/* A list (chain of TREE_LIST nodes) of all LABEL_DECLs in the function
   that have names.  Here so we can clear out their names' definitions
   at the end of the function.  */

static tree named_labels;

/* A list of LABEL_DECLs from outer contexts that are currently shadowed.  */

static tree shadowed_labels;

/* Nonzero when store_parm_decls is called indicates a varargs function.
   Value not meaningful after store_parm_decls.  */

static int c_function_varargs;

/* Set to 0 at beginning of a function definition, set to 1 if
   a return statement that specifies a return value is seen.  */

int current_function_returns_value;

/* Set to 0 at beginning of a function definition, set to 1 if
   a return statement with no argument is seen.  */

int current_function_returns_null;

/* Set to 0 at beginning of a function definition, set to 1 if
   a call to a noreturn function is seen.  */

int current_function_returns_abnormally;

/* Set to nonzero by `grokdeclarator' for a function
   whose return type is defaulted, if warnings for this are desired.  */

static int warn_about_return_type;

/* Nonzero when starting a function declared `extern inline'.  */

static int current_extern_inline;

/* For each binding contour we allocate a binding_level structure
 * which records the names defined in that contour.
 * Contours include:
 *  0) the global one
 *  1) one for each function definition,
 *     where internal declarations of the parameters appear.
 *  2) one for each compound statement,
 *     to record its declarations.
 *
 * The current meaning of a name can be found by searching the levels from
 * the current one out to the global one.
 */

/* Note that the information in the `names' component of the global contour
   is duplicated in the IDENTIFIER_GLOBAL_VALUEs of all identifiers.  */

struct binding_level
  {
    /* A chain of _DECL nodes for all variables, constants, functions,
       and typedef types.  These are in the reverse of the order supplied.
     */
    tree names;

    /* A list of structure, union and enum definitions,
     * for looking up tag names.
     * It is a chain of TREE_LIST nodes, each of whose TREE_PURPOSE is a name,
     * or NULL_TREE; and whose TREE_VALUE is a RECORD_TYPE, UNION_TYPE,
     * or ENUMERAL_TYPE node.
     */
    tree tags;

    /* For each level, a list of shadowed outer-level local definitions
       to be restored when this level is popped.
       Each link is a TREE_LIST whose TREE_PURPOSE is an identifier and
       whose TREE_VALUE is its old definition (a kind of ..._DECL node).  */
    tree shadowed;

    /* For each level (except not the global one),
       a chain of BLOCK nodes for all the levels
       that were entered and exited one level down.  */
    tree blocks;

    /* The BLOCK node for this level, if one has been preallocated.
       If 0, the BLOCK is allocated (if needed) when the level is popped.  */
    tree this_block;

    /* The binding level which this one is contained in (inherits from).  */
    struct binding_level *level_chain;

    /* Nonzero for the level that holds the parameters of a function.  */
    char parm_flag;

    /* Nonzero if this level "doesn't exist" for tags.  */
    char tag_transparent;

    /* Nonzero if sublevels of this level "don't exist" for tags.
       This is set in the parm level of a function definition
       while reading the function body, so that the outermost block
       of the function body will be tag-transparent.  */
    char subblocks_tag_transparent;

    /* Nonzero means make a BLOCK for this level regardless of all else.  */
    char keep;

    /* Nonzero means make a BLOCK if this level has any subblocks.  */
    char keep_if_subblocks;

    /* Number of decls in `names' that have incomplete
       structure or union types.  */
    int n_incomplete;

    /* A list of decls giving the (reversed) specified order of parms,
       not including any forward-decls in the parmlist.
       This is so we can put the parms in proper order for assign_parms.  */
    tree parm_order;
  };

#define NULL_BINDING_LEVEL (struct binding_level *) NULL

/* The binding level currently in effect.  */

static struct binding_level *current_binding_level;

/* A chain of binding_level structures awaiting reuse.  */

static struct binding_level *free_binding_level;

/* The outermost binding level, for names of file scope.
   This is created when the compiler is started and exists
   through the entire run.  */

static struct binding_level *global_binding_level;

/* Binding level structures are initialized by copying this one.  */

static struct binding_level clear_binding_level
  = {NULL, NULL, NULL, NULL, NULL, NULL_BINDING_LEVEL, 0, 0, 0, 0, 0, 0,
     NULL};

/* Nonzero means unconditionally make a BLOCK for the next level pushed.  */

static int keep_next_level_flag;

/* Nonzero means make a BLOCK for the next level pushed
   if it has subblocks.  */

static int keep_next_if_subblocks;

/* The chain of outer levels of label scopes.
   This uses the same data structure used for binding levels,
   but it works differently: each link in the chain records
   saved values of named_labels and shadowed_labels for
   a label binding level outside the current one.  */

static struct binding_level *label_level_chain;

/* Functions called automatically at the beginning and end of execution.  */

tree static_ctors, static_dtors;

/* Forward declarations.  */

static struct binding_level * make_binding_level	PARAMS ((void));
static void mark_binding_level		PARAMS ((void *));
static void clear_limbo_values		PARAMS ((tree));
static int duplicate_decls		PARAMS ((tree, tree, int));
static int redeclaration_error_message	PARAMS ((tree, tree));
static void storedecls			PARAMS ((tree));
static void storetags			PARAMS ((tree));
static tree lookup_tag			PARAMS ((enum tree_code, tree,
						 struct binding_level *, int));
static tree lookup_tag_reverse		PARAMS ((tree));
static tree grokdeclarator		PARAMS ((tree, tree, enum decl_context,
						 int));
static tree grokparms			PARAMS ((tree, int));
static void layout_array_type		PARAMS ((tree));
static tree c_make_fname_decl           PARAMS ((tree, int));
static void bli_expand_body               PARAMS ((tree, int, int));
static void warn_if_shadowing		PARAMS ((tree, tree));

/* C-specific option variables.  */

/* Nonzero means allow type mismatches in conditional expressions;
   just make their values `void'.  */

int flag_cond_mismatch;

/* Nonzero means don't recognize the keyword `asm'.  */

int flag_no_asm;

/* Nonzero means do some things the same way PCC does.  */

int flag_traditional;

/* Nonzero means enable C89 Amendment 1 features.  */

int flag_isoc94 = 0;

/* Nonzero means use the ISO C99 dialect of C.  */

int flag_isoc99 = 0;

/* Nonzero means that we have builtin functions, and main is an int */

int flag_hosted = 1;

/* Nonzero means add default format_arg attributes for functions not
   in ISO C.  */

int flag_noniso_default_format_attributes = 1;

/* Nonzero means to allow single precision math even if we're generally
   being traditional.  */
int flag_allow_single_precision = 0;

/* Nonzero means to treat bitfields as signed unless they say `unsigned'.  */

int flag_signed_bitfields = 1;
int explicit_flag_signed_bitfields = 0;

/* Nonzero means warn about use of implicit int.  */

int warn_implicit_int;

/* Nonzero means warn about usage of long long when `-pedantic'.  */

int warn_long_long = 1;

/* Nonzero means message about use of implicit function declarations;
 1 means warning; 2 means error.  */

int mesg_implicit_function_declaration = -1;

/* Nonzero means give string constants the type `const char *'
   to get extra warnings from them.  These warnings will be too numerous
   to be useful, except in thoroughly ANSIfied programs.  */

int flag_const_strings;

/* Nonzero means warn about pointer casts that can drop a type qualifier
   from the pointer target type.  */

int warn_cast_qual;

/* Nonzero means warn when casting a function call to a type that does
   not match the return type (e.g. (float)sqrt() or (anything*)malloc()
   when there is no previous declaration of sqrt or malloc.  */

int warn_bad_function_cast;

/* Warn about functions which might be candidates for format attributes.  */

int warn_missing_format_attribute;

/* Warn about traditional constructs whose meanings changed in ANSI C.  */

int warn_traditional;

/* Nonzero means warn about sizeof(function) or addition/subtraction
   of function pointers.  */

int warn_pointer_arith;

/* Nonzero means warn for non-prototype function decls
   or non-prototyped defs without previous prototype.  */

int warn_strict_prototypes;

/* Nonzero means warn for any global function def
   without separate previous prototype decl.  */

int warn_missing_prototypes;

/* Nonzero means warn for any global function def
   without separate previous decl.  */

int warn_missing_declarations;

/* Nonzero means warn about multiple (redundant) decls for the same single
   variable or function.  */

int warn_redundant_decls = 0;

/* Nonzero means warn about extern declarations of objects not at
   file-scope level and about *all* declarations of functions (whether
   extern or static) not at file-scope level.  Note that we exclude
   implicit function declarations.  To get warnings about those, use
   -Wimplicit.  */

int warn_nested_externs = 0;

/* Warn about a subscript that has type char.  */

int warn_char_subscripts = 0;

/* Warn if a type conversion is done that might have confusing results.  */

int warn_conversion;

/* Warn if adding () is suggested.  */

int warn_parentheses;

/* Warn if initializer is not completely bracketed.  */

int warn_missing_braces;

/* Warn if main is suspicious.  */

int warn_main;

/* Warn about #pragma directives that are not recognised.  */

int warn_unknown_pragmas = 0; /* Tri state variable.  */

/* Warn about comparison of signed and unsigned values.
   If -1, neither -Wsign-compare nor -Wno-sign-compare has been specified.  */

int warn_sign_compare = -1;

/* Warn about testing equality of floating point numbers.  */

int warn_float_equal = 0;

/* Nonzero means warn about use of multicharacter literals.  */

int warn_multichar = 1;

/* Nonzero means `$' can be in an identifier.  */

#ifndef DOLLARS_IN_IDENTIFIERS
#define DOLLARS_IN_IDENTIFIERS 1
#endif
int dollars_in_ident = DOLLARS_IN_IDENTIFIERS;

/* States indicating how grokdeclarator() should handle declspecs marked
   with __attribute__((deprecated)).  An object declared as
   __attribute__((deprecated)) suppresses warnings of uses of other
   deprecated items.  */
   
enum deprecated_states {
  DEPRECATED_NORMAL,
  DEPRECATED_SUPPRESS
};

static enum deprecated_states deprecated_state = DEPRECATED_NORMAL;

/* Hook called at end of compilation to assume 1 elt
   for a top-level tentative array defn that wasn't complete before.  */

void
finish_incomplete_decl (decl)
     tree decl;
{
  if (TREE_CODE (decl) == VAR_DECL)
    {
      tree type = TREE_TYPE (decl);
      if (type != error_mark_node
	  && TREE_CODE (type) == ARRAY_TYPE
	  && ! DECL_EXTERNAL (decl)
	  && TYPE_DOMAIN (type) == 0)
	{
	  warning_with_decl (decl, "array `%s' assumed to have one element");

	  complete_array_type (type, NULL_TREE, 1);

	  layout_decl (decl, 0);
	}
    }
}

/* Create a new `struct binding_level'.  */

static struct binding_level *
make_binding_level ()
{
  /* NOSTRICT */
  return (struct binding_level *) xmalloc (sizeof (struct binding_level));
}

/* Nonzero if we are currently in the global binding level.  */

int
global_bindings_p ()
{
  return current_binding_level == global_binding_level;
}

void
keep_next_level ()
{
  keep_next_level_flag = 1;
}

/* Nonzero if the current level needs to have a BLOCK made.  */

int
kept_level_p ()
{
  return ((current_binding_level->keep_if_subblocks
	   && current_binding_level->blocks != 0)
	  || current_binding_level->keep
	  || current_binding_level->names != 0
	  || (current_binding_level->tags != 0
	      && !current_binding_level->tag_transparent));
}

/* Identify this binding level as a level of parameters.
   DEFINITION_FLAG is 1 for a definition, 0 for a declaration.
   But it turns out there is no way to pass the right value for
   DEFINITION_FLAG, so we ignore it.  */

void
declare_parm_level (definition_flag)
     int definition_flag ATTRIBUTE_UNUSED;
{
  current_binding_level->parm_flag = 1;
}

/* Nonzero if currently making parm declarations.  */

int
in_parm_level_p ()
{
  return current_binding_level->parm_flag;
}

/* Enter a new binding level.
   If TAG_TRANSPARENT is nonzero, do so only for the name space of variables,
   not for that of tags.  */

void
pushlevel (tag_transparent)
     int tag_transparent;
{
  struct binding_level *newlevel = NULL_BINDING_LEVEL;

  /* If this is the top level of a function,
     just make sure that NAMED_LABELS is 0.  */

  if (current_binding_level == global_binding_level)
    {
      named_labels = 0;
    }

  /* Reuse or create a struct for this binding level.  */

  if (free_binding_level)
    {
      newlevel = free_binding_level;
      free_binding_level = free_binding_level->level_chain;
    }
  else
    {
      newlevel = make_binding_level ();
    }

  /* Add this level to the front of the chain (stack) of levels that
     are active.  */

  *newlevel = clear_binding_level;
  newlevel->tag_transparent
    = (tag_transparent
       || (current_binding_level
	   ? current_binding_level->subblocks_tag_transparent
	   : 0));
  newlevel->level_chain = current_binding_level;
  current_binding_level = newlevel;
  newlevel->keep = keep_next_level_flag;
  keep_next_level_flag = 0;
  newlevel->keep_if_subblocks = keep_next_if_subblocks;
  keep_next_if_subblocks = 0;
}

/* Clear the limbo values of all identifiers defined in BLOCK or a subblock.  */

static void
clear_limbo_values (block)
     tree block;
{
  tree tem;

  for (tem = BLOCK_VARS (block); tem; tem = TREE_CHAIN (tem))
    if (DECL_NAME (tem) != 0)
      IDENTIFIER_LIMBO_VALUE (DECL_NAME (tem)) = 0;

  for (tem = BLOCK_SUBBLOCKS (block); tem; tem = TREE_CHAIN (tem))
    clear_limbo_values (tem);
}

/* Exit a binding level.
   Pop the level off, and restore the state of the identifier-decl mappings
   that were in effect when this level was entered.

   If KEEP is nonzero, this level had explicit declarations, so
   and create a "block" (a BLOCK node) for the level
   to record its declarations and subblocks for symbol table output.

   If FUNCTIONBODY is nonzero, this level is the body of a function,
   so create a block as if KEEP were set and also clear out all
   label names.

   If REVERSE is nonzero, reverse the order of decls before putting
   them into the BLOCK.  */

tree
poplevel (keep, reverse, functionbody)
     int keep;
     int reverse;
     int functionbody;
{
  tree link;
  /* The chain of decls was accumulated in reverse order.
     Put it into forward order, just for cleanliness.  */
  tree decls;
  tree tags = current_binding_level->tags;
  tree subblocks = current_binding_level->blocks;
  tree block = 0;
  tree decl;
  int block_previously_created;

  keep |= current_binding_level->keep;

  /* This warning is turned off because it causes warnings for
     declarations like `extern struct foo *x'.  */
#if 0
  /* Warn about incomplete structure types in this level.  */
  for (link = tags; link; link = TREE_CHAIN (link))
    if (!COMPLETE_TYPE_P (TREE_VALUE (link)))
      {
	tree type = TREE_VALUE (link);
	tree type_name = TYPE_NAME (type);
	char *id = IDENTIFIER_POINTER (TREE_CODE (type_name) == IDENTIFIER_NODE
				       ? type_name
				       : DECL_NAME (type_name));
	switch (TREE_CODE (type))
	  {
	  case RECORD_TYPE:
	    error ("`struct %s' incomplete in scope ending here", id);
	    break;
	  case UNION_TYPE:
	    error ("`union %s' incomplete in scope ending here", id);
	    break;
	  case ENUMERAL_TYPE:
	    error ("`enum %s' incomplete in scope ending here", id);
	    break;
	  }
      }
#endif /* 0 */

  /* Get the decls in the order they were written.
     Usually current_binding_level->names is in reverse order.
     But parameter decls were previously put in forward order.  */

  if (reverse)
    current_binding_level->names
      = decls = nreverse (current_binding_level->names);
  else
    decls = current_binding_level->names;

  /* Output any nested inline functions within this block
     if they weren't already output.  */

  for (decl = decls; decl; decl = TREE_CHAIN (decl))
    if (TREE_CODE (decl) == FUNCTION_DECL
	&& ! TREE_ASM_WRITTEN (decl)
	&& DECL_INITIAL (decl) != 0
	&& TREE_ADDRESSABLE (decl))
      {
	/* If this decl was copied from a file-scope decl
	   on account of a block-scope extern decl,
	   propagate TREE_ADDRESSABLE to the file-scope decl.

	   DECL_ABSTRACT_ORIGIN can be set to itself if warn_return_type is
	   true, since then the decl goes through save_for_inline_copying.  */
	if (DECL_ABSTRACT_ORIGIN (decl) != 0
	    && DECL_ABSTRACT_ORIGIN (decl) != decl)
	  TREE_ADDRESSABLE (DECL_ABSTRACT_ORIGIN (decl)) = 1;
      }

  /* We used to warn about unused variables in expand_end_bindings,
     i.e. while generating RTL.  But in function-at-a-time mode we may
     choose to never expand a function at all (e.g. auto inlining), so
     we do this explicitly now.  */
  warn_about_unused_variables (getdecls ());

  /* If there were any declarations or structure tags in that level,
     or if this level is a function body,
     create a BLOCK to record them for the life of this function.  */

  block = 0;
  block_previously_created = (current_binding_level->this_block != 0);
  if (block_previously_created)
    block = current_binding_level->this_block;
  else if (keep || functionbody
	   || (current_binding_level->keep_if_subblocks && subblocks != 0))
    block = make_node (BLOCK);
  if (block != 0)
    {
      BLOCK_VARS (block) = decls;
      BLOCK_SUBBLOCKS (block) = subblocks;
    }

  /* In each subblock, record that this is its superior.  */

  for (link = subblocks; link; link = TREE_CHAIN (link))
    BLOCK_SUPERCONTEXT (link) = block;

  /* Clear out the meanings of the local variables of this level.  */

  for (link = decls; link; link = TREE_CHAIN (link))
    {
      if (DECL_NAME (link) != 0)
	{
	  /* If the ident. was used or addressed via a local extern decl,
	     don't forget that fact.  */
	  if (DECL_EXTERNAL (link))
	    {
	      if (TREE_USED (link))
		TREE_USED (DECL_NAME (link)) = 1;
	      if (TREE_ADDRESSABLE (link))
		TREE_ADDRESSABLE (DECL_ASSEMBLER_NAME (link)) = 1;
	    }
	  IDENTIFIER_LOCAL_VALUE (DECL_NAME (link)) = 0;
	}
    }

  /* Restore all name-meanings of the outer levels
     that were shadowed by this level.  */

  for (link = current_binding_level->shadowed; link; link = TREE_CHAIN (link))
    IDENTIFIER_LOCAL_VALUE (TREE_PURPOSE (link)) = TREE_VALUE (link);

  /* If the level being exited is the top level of a function,
     check over all the labels, and clear out the current
     (function local) meanings of their names.  */

  if (functionbody)
    {
      clear_limbo_values (block);

      /* If this is the top level block of a function,
	 the vars are the function's parameters.
	 Don't leave them in the BLOCK because they are
	 found in the FUNCTION_DECL instead.  */

      BLOCK_VARS (block) = 0;

      /* Clear out the definitions of all label names,
	 since their scopes end here,
	 and add them to BLOCK_VARS.  */

      for (link = named_labels; link; link = TREE_CHAIN (link))
	{
	  tree label = TREE_VALUE (link);

	  if (DECL_INITIAL (label) == 0)
	    {
	      error_with_decl (label, "label `%s' used but not defined");
	      /* Avoid crashing later.  */
	      define_label (input_filename, lineno,
			    DECL_NAME (label));
	    }
	  else if (warn_unused_label && !TREE_USED (label))
	    warning_with_decl (label, "label `%s' defined but not used");
	  IDENTIFIER_LABEL_VALUE (DECL_NAME (label)) = 0;

	  /* Put the labels into the "variables" of the
	     top-level block, so debugger can see them.  */
	  TREE_CHAIN (label) = BLOCK_VARS (block);
	  BLOCK_VARS (block) = label;
	}
    }

  /* Pop the current level, and free the structure for reuse.  */

  {
    struct binding_level *level = current_binding_level;
    current_binding_level = current_binding_level->level_chain;

    level->level_chain = free_binding_level;
    free_binding_level = level;
  }

  /* Dispose of the block that we just made inside some higher level.  */
  if (functionbody)
    DECL_INITIAL (current_function_decl) = block;
  else if (block)
    {
      if (!block_previously_created)
	current_binding_level->blocks
	  = chainon (current_binding_level->blocks, block);
    }
  /* If we did not make a block for the level just exited,
     any blocks made for inner levels
     (since they cannot be recorded as subblocks in that level)
     must be carried forward so they will later become subblocks
     of something else.  */
  else if (subblocks)
    current_binding_level->blocks
      = chainon (current_binding_level->blocks, subblocks);

  /* Set the TYPE_CONTEXTs for all of the tagged types belonging to this
     binding contour so that they point to the appropriate construct, i.e.
     either to the current FUNCTION_DECL node, or else to the BLOCK node
     we just constructed.

     Note that for tagged types whose scope is just the formal parameter
     list for some function type specification, we can't properly set
     their TYPE_CONTEXTs here, because we don't have a pointer to the
     appropriate FUNCTION_TYPE node readily available to us.  For those
     cases, the TYPE_CONTEXTs of the relevant tagged type nodes get set
     in `grokdeclarator' as soon as we have created the FUNCTION_TYPE
     node which will represent the "scope" for these "parameter list local"
     tagged types.  */

  if (functionbody)
    for (link = tags; link; link = TREE_CHAIN (link))
      TYPE_CONTEXT (TREE_VALUE (link)) = current_function_decl;
  else if (block)
    for (link = tags; link; link = TREE_CHAIN (link))
      TYPE_CONTEXT (TREE_VALUE (link)) = block;

  if (block)
    TREE_USED (block) = 1;

  return block;
}

/* Insert BLOCK at the end of the list of subblocks of the
   current binding level.  This is used when a BIND_EXPR is expanded,
   to handle the BLOCK node inside the BIND_EXPR.  */

void
insert_block (block)
     tree block;
{
  TREE_USED (block) = 1;
  current_binding_level->blocks
    = chainon (current_binding_level->blocks, block);
}

/* Set the BLOCK node for the innermost scope
   (the one we are currently in).  */

void
set_block (block)
     tree block;
{
  current_binding_level->this_block = block;
  current_binding_level->names = chainon (current_binding_level->names,
					  BLOCK_VARS (block));
  current_binding_level->blocks = chainon (current_binding_level->blocks,
					   BLOCK_SUBBLOCKS (block));
}

void
push_label_level ()
{
  struct binding_level *newlevel;

  /* Reuse or create a struct for this binding level.  */

  if (free_binding_level)
    {
      newlevel = free_binding_level;
      free_binding_level = free_binding_level->level_chain;
    }
  else
    {
      newlevel = make_binding_level ();
    }

  /* Add this level to the front of the chain (stack) of label levels.  */

  newlevel->level_chain = label_level_chain;
  label_level_chain = newlevel;

  newlevel->names = named_labels;
  newlevel->shadowed = shadowed_labels;
  named_labels = 0;
  shadowed_labels = 0;
}

void
pop_label_level ()
{
  struct binding_level *level = label_level_chain;
  tree link, prev;

  /* Clear out the definitions of the declared labels in this level.
     Leave in the list any ordinary, non-declared labels.  */
  for (link = named_labels, prev = 0; link;)
    {
      if (C_DECLARED_LABEL_FLAG (TREE_VALUE (link)))
	{
	  if (DECL_SOURCE_LINE (TREE_VALUE (link)) == 0)
	    {
	      error_with_decl (TREE_VALUE (link),
			       "label `%s' used but not defined");
	      /* Avoid crashing later.  */
	      define_label (input_filename, lineno,
			    DECL_NAME (TREE_VALUE (link)));
	    }
	  else if (warn_unused_label && !TREE_USED (TREE_VALUE (link)))
	    warning_with_decl (TREE_VALUE (link),
			       "label `%s' defined but not used");
	  IDENTIFIER_LABEL_VALUE (DECL_NAME (TREE_VALUE (link))) = 0;

	  /* Delete this element from the list.  */
	  link = TREE_CHAIN (link);
	  if (prev)
	    TREE_CHAIN (prev) = link;
	  else
	    named_labels = link;
	}
      else
	{
	  prev = link;
	  link = TREE_CHAIN (link);
	}
    }

  /* Bring back all the labels that were shadowed.  */
  for (link = shadowed_labels; link; link = TREE_CHAIN (link))
    if (DECL_NAME (TREE_VALUE (link)) != 0)
      IDENTIFIER_LABEL_VALUE (DECL_NAME (TREE_VALUE (link)))
	= TREE_VALUE (link);

  named_labels = chainon (named_labels, level->names);
  shadowed_labels = level->shadowed;

  /* Pop the current level, and free the structure for reuse.  */
  label_level_chain = label_level_chain->level_chain;
  level->level_chain = free_binding_level;
  free_binding_level = level;
}

/* Push a definition or a declaration of struct, union or enum tag "name".
   "type" should be the type node.
   We assume that the tag "name" is not already defined.

   Note that the definition may really be just a forward reference.
   In that case, the TYPE_SIZE will be zero.  */

void
pushtag (name, type)
     tree name, type;
{
  struct binding_level *b;

  /* Find the proper binding level for this type tag.  */

  for (b = current_binding_level; b->tag_transparent; b = b->level_chain)
    continue;

  if (name)
    {
      /* Record the identifier as the type's name if it has none.  */

      if (TYPE_NAME (type) == 0)
	TYPE_NAME (type) = name;
    }

  b->tags = tree_cons (name, type, b->tags);

  /* Create a fake NULL-named TYPE_DECL node whose TREE_TYPE will be the
     tagged type we just added to the current binding level.  This fake
     NULL-named TYPE_DECL node helps dwarfout.c to know when it needs
     to output a representation of a tagged type, and it also gives
     us a convenient place to record the "scope start" address for the
     tagged type.  */

  TYPE_STUB_DECL (type) = pushdecl (build_decl (TYPE_DECL, NULL_TREE, type));

  /* An approximation for now, so we can tell this is a function-scope tag.
     This will be updated in poplevel.  */
  TYPE_CONTEXT (type) = DECL_CONTEXT (TYPE_STUB_DECL (type));
}

/* Handle when a new declaration NEWDECL
   has the same name as an old one OLDDECL
   in the same binding contour.
   Prints an error message if appropriate.

   If safely possible, alter OLDDECL to look like NEWDECL, and return 1.
   Otherwise, return 0.

   When DIFFERENT_BINDING_LEVEL is true, NEWDECL is an external declaration,
   and OLDDECL is in an outer binding level and should thus not be changed.  */

static int
duplicate_decls (newdecl, olddecl, different_binding_level)
     tree newdecl, olddecl;
     int different_binding_level;
{
  int types_match = comptypes (TREE_TYPE (newdecl), TREE_TYPE (olddecl));
  int new_is_definition = (TREE_CODE (newdecl) == FUNCTION_DECL
			    && DECL_INITIAL (newdecl) != 0);
   tree oldtype = TREE_TYPE (olddecl);
   tree newtype = TREE_TYPE (newdecl);
   int errmsg = 0;

   if (DECL_P (olddecl))
     {
       if (TREE_CODE (newdecl) == FUNCTION_DECL
	   && TREE_CODE (olddecl) == FUNCTION_DECL
	   && (DECL_UNINLINABLE (newdecl) || DECL_UNINLINABLE (olddecl)))
	 {
	   if (DECL_DECLARED_INLINE_P (newdecl)
	       && DECL_UNINLINABLE (newdecl)
	       && lookup_attribute ("noinline", DECL_ATTRIBUTES (newdecl)))
	     /* Already warned elsewhere.  */;
	   else if (DECL_DECLARED_INLINE_P (olddecl)
		    && DECL_UNINLINABLE (olddecl)
		    && lookup_attribute ("noinline", DECL_ATTRIBUTES (olddecl)))
	     /* Already warned.  */;
	   else if (DECL_DECLARED_INLINE_P (newdecl)
		    && ! DECL_DECLARED_INLINE_P (olddecl)
		    && DECL_UNINLINABLE (olddecl)
		    && lookup_attribute ("noinline", DECL_ATTRIBUTES (olddecl)))
	     {
	       warning_with_decl (newdecl,
				  "function `%s' redeclared as inline");
	       warning_with_decl (olddecl,
				  "previous declaration of function `%s' with attribute noinline");
	     }
	   else if (DECL_DECLARED_INLINE_P (olddecl)
		    && DECL_UNINLINABLE (newdecl)
		    && lookup_attribute ("noinline", DECL_ATTRIBUTES (newdecl)))
	     {
	       warning_with_decl (newdecl,
				  "function `%s' redeclared with attribute noinline");
	       warning_with_decl (olddecl,
				  "previous declaration of function `%s' was inline");
	     }
	 }

       DECL_ATTRIBUTES (newdecl)
	 = (*targetm.merge_decl_attributes) (olddecl, newdecl);
     }

   if (TREE_CODE (newtype) == ERROR_MARK
       || TREE_CODE (oldtype) == ERROR_MARK)
     types_match = 0;

   /* New decl is completely inconsistent with the old one =>
      tell caller to replace the old one.
      This is always an error except in the case of shadowing a builtin.  */
   if (TREE_CODE (olddecl) != TREE_CODE (newdecl))
     {
       if (TREE_CODE (olddecl) == FUNCTION_DECL
	   && (DECL_BUILT_IN (olddecl)
	       || DECL_BUILT_IN_NONANSI (olddecl)))
	 {
	   /* If you declare a built-in or predefined function name as static,
	      the old definition is overridden,
	      but optionally warn this was a bad choice of name.  */
	   if (!TREE_PUBLIC (newdecl))
	     {
	       if (!warn_shadow)
		 ;
	       else if (DECL_BUILT_IN (olddecl))
		 warning_with_decl (newdecl, "shadowing built-in function `%s'");
	       else
		 warning_with_decl (newdecl, "shadowing library function `%s'");
	     }
	   /* Likewise, if the built-in is not ansi, then programs can
	      override it even globally without an error.  */
	   else if (! DECL_BUILT_IN (olddecl))
	     warning_with_decl (newdecl,
				"library function `%s' declared as non-function");

	   else if (DECL_BUILT_IN_NONANSI (olddecl))
	     warning_with_decl (newdecl,
				"built-in function `%s' declared as non-function");
	   else
	     warning_with_decl (newdecl,
				"built-in function `%s' declared as non-function");
	 }
       else
	 {
	   error_with_decl (newdecl, "`%s' redeclared as different kind of symbol");
	   error_with_decl (olddecl, "previous declaration of `%s'");
	 }

       return 0;
     }

   /* For real parm decl following a forward decl,
      return 1 so old decl will be reused.  */
   if (types_match && TREE_CODE (newdecl) == PARM_DECL
       && TREE_ASM_WRITTEN (olddecl) && ! TREE_ASM_WRITTEN (newdecl))
     return 1;

   /* The new declaration is the same kind of object as the old one.
      The declarations may partially match.  Print warnings if they don't
      match enough.  Ultimately, copy most of the information from the new
      decl to the old one, and keep using the old one.  */

   if (flag_traditional && TREE_CODE (newdecl) == FUNCTION_DECL
       && IDENTIFIER_IMPLICIT_DECL (DECL_NAME (newdecl)) == olddecl
       && DECL_INITIAL (olddecl) == 0)
     /* If -traditional, avoid error for redeclaring fcn
	after implicit decl.  */
     ;
   else if (TREE_CODE (olddecl) == FUNCTION_DECL
	    && DECL_BUILT_IN (olddecl))
     {
       /* A function declaration for a built-in function.  */
       if (!TREE_PUBLIC (newdecl))
	 {
	   /* If you declare a built-in function name as static, the
	      built-in definition is overridden,
	      but optionally warn this was a bad choice of name.  */
	   if (warn_shadow)
	     warning_with_decl (newdecl, "shadowing built-in function `%s'");
	   /* Discard the old built-in function.  */
	   return 0;
	 }
       else if (!types_match)
	 {
	   /* Accept the return type of the new declaration if same modes.  */
	   tree oldreturntype = TREE_TYPE (oldtype);
	   tree newreturntype = TREE_TYPE (newtype);

	   if (TYPE_MODE (oldreturntype) == TYPE_MODE (newreturntype))
	     {
	       /* Function types may be shared, so we can't just modify
		  the return type of olddecl's function type.  */
	       tree trytype
		 = build_function_type (newreturntype,
					TYPE_ARG_TYPES (oldtype));
	       trytype = build_type_attribute_variant (trytype,
						       TYPE_ATTRIBUTES (oldtype));

	       types_match = comptypes (newtype, trytype);
	       if (types_match)
		 oldtype = trytype;
	     }
	   /* Accept harmless mismatch in first argument type also.
	      This is for the ffs and fprintf builtins.  */
	   if (TYPE_ARG_TYPES (TREE_TYPE (newdecl)) != 0
	       && TYPE_ARG_TYPES (oldtype) != 0
	       && TREE_VALUE (TYPE_ARG_TYPES (newtype)) != 0
	       && TREE_VALUE (TYPE_ARG_TYPES (oldtype)) != 0
	       && (TYPE_MODE (TREE_VALUE (TYPE_ARG_TYPES (newtype)))
		   == TYPE_MODE (TREE_VALUE (TYPE_ARG_TYPES (oldtype)))))
	     {
	       /* Function types may be shared, so we can't just modify
		  the return type of olddecl's function type.  */
	       tree trytype
		 = build_function_type (TREE_TYPE (oldtype),
					tree_cons (NULL_TREE,
						   TREE_VALUE (TYPE_ARG_TYPES (newtype)),
						   TREE_CHAIN (TYPE_ARG_TYPES (oldtype))));
	       trytype = build_type_attribute_variant (trytype,
						       TYPE_ATTRIBUTES (oldtype));

	       types_match = comptypes (newtype, trytype);
	       if (types_match)
		 oldtype = trytype;
	     }
	   if (! different_binding_level)
	     TREE_TYPE (olddecl) = oldtype;
	 }
       else if (TYPE_ARG_TYPES (oldtype) == NULL
		&& TYPE_ARG_TYPES (newtype) != NULL)
	 {
	   /* For bcmp, bzero, fputs the builtin type has arguments not
	      specified.  Use the ones from the prototype so that type checking
	      is done for them.  */
	   tree trytype
	     = build_function_type (TREE_TYPE (oldtype),
				    TYPE_ARG_TYPES (newtype));
	   trytype = build_type_attribute_variant (trytype,
						   TYPE_ATTRIBUTES (oldtype));

	   oldtype = trytype;
	   if (! different_binding_level)
	     TREE_TYPE (olddecl) = oldtype;
	 }
       if (!types_match)
	 {
	   /* If types don't match for a built-in, throw away the built-in.  */
	   warning_with_decl (newdecl, "conflicting types for built-in function `%s'");
	   return 0;
	 }
     }
   else if (TREE_CODE (olddecl) == FUNCTION_DECL
	    && DECL_SOURCE_LINE (olddecl) == 0)
     {
       /* A function declaration for a predeclared function
	  that isn't actually built in.  */
       if (!TREE_PUBLIC (newdecl))
	 {
	   /* If you declare it as static, the
	      default definition is overridden.  */
	   return 0;
	 }
       else if (!types_match)
	 {
	   /* If the types don't match, preserve volatility indication.
	      Later on, we will discard everything else about the
	      default declaration.  */
	   TREE_THIS_VOLATILE (newdecl) |= TREE_THIS_VOLATILE (olddecl);
	 }
     }
   /* Permit char *foo () to match void *foo (...) if not pedantic,
      if one of them came from a system header file.  */
   else if (!types_match
	    && TREE_CODE (olddecl) == FUNCTION_DECL
	    && TREE_CODE (newdecl) == FUNCTION_DECL
	    && TREE_CODE (TREE_TYPE (oldtype)) == POINTER_TYPE
	    && TREE_CODE (TREE_TYPE (newtype)) == POINTER_TYPE
	    && (DECL_IN_SYSTEM_HEADER (olddecl)
		|| DECL_IN_SYSTEM_HEADER (newdecl))
	    && ((TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (newtype))) == void_type_node
		 && TYPE_ARG_TYPES (oldtype) == 0
		 && self_promoting_args_p (TYPE_ARG_TYPES (newtype))
		 && TREE_TYPE (TREE_TYPE (oldtype)) == char_type_node)
		||
		(TREE_TYPE (TREE_TYPE (newtype)) == char_type_node
		 && TYPE_ARG_TYPES (newtype) == 0
		 && self_promoting_args_p (TYPE_ARG_TYPES (oldtype))
		 && TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (oldtype))) == void_type_node)))
     {
       if (pedantic)
	 pedwarn_with_decl (newdecl, "conflicting types for `%s'");
       /* Make sure we keep void * as ret type, not char *.  */
       if (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (oldtype))) == void_type_node)
	 TREE_TYPE (newdecl) = newtype = oldtype;

       /* Set DECL_IN_SYSTEM_HEADER, so that if we see another declaration
	  we will come back here again.  */
       DECL_IN_SYSTEM_HEADER (newdecl) = 1;
     }
   else if (!types_match
	    /* Permit char *foo (int, ...); followed by char *foo ();
	       if not pedantic.  */
	    && ! (TREE_CODE (olddecl) == FUNCTION_DECL
		  && ! pedantic
		  /* Return types must still match.  */
		  && comptypes (TREE_TYPE (oldtype),
				TREE_TYPE (newtype))
		  && TYPE_ARG_TYPES (newtype) == 0))
     {
       error_with_decl (newdecl, "conflicting types for `%s'");
       /* Check for function type mismatch
	  involving an empty arglist vs a nonempty one.  */
       if (TREE_CODE (olddecl) == FUNCTION_DECL
	   && comptypes (TREE_TYPE (oldtype),
			 TREE_TYPE (newtype))
	   && ((TYPE_ARG_TYPES (oldtype) == 0
		&& DECL_INITIAL (olddecl) == 0)
	       ||
	       (TYPE_ARG_TYPES (newtype) == 0
		&& DECL_INITIAL (newdecl) == 0)))
	 {
	   /* Classify the problem further.  */
	   tree t = TYPE_ARG_TYPES (oldtype);
	   if (t == 0)
	     t = TYPE_ARG_TYPES (newtype);
	   for (; t; t = TREE_CHAIN (t))
	     {
	       tree type = TREE_VALUE (t);

	       if (TREE_CHAIN (t) == 0
		   && TYPE_MAIN_VARIANT (type) != void_type_node)
		 {
		   error ("a parameter list with an ellipsis can't match an empty parameter name list declaration");
		   break;
		 }

	       if (simple_type_promotes_to (type) != NULL_TREE)
		 {
		   error ("an argument type that has a default promotion can't match an empty parameter name list declaration");
		   break;
		 }
	     }
	 }
       error_with_decl (olddecl, "previous declaration of `%s'");
     }
   else
     {
       errmsg = redeclaration_error_message (newdecl, olddecl);
       if (errmsg)
	 {
	   switch (errmsg)
	     {
	     case 1:
	       error_with_decl (newdecl, "redefinition of `%s'");
	       break;
	     case 2:
	       error_with_decl (newdecl, "redeclaration of `%s'");
	       break;
	     case 3:
	       error_with_decl (newdecl, "conflicting declarations of `%s'");
	       break;
	     default:
	       abort ();
	     }

	   error_with_decl (olddecl,
			    ((DECL_INITIAL (olddecl)
			      && current_binding_level == global_binding_level)
			     ? "`%s' previously defined here"
			     : "`%s' previously declared here"));
	   return 0;
	 }
       else if (TREE_CODE (newdecl) == TYPE_DECL
		&& (DECL_IN_SYSTEM_HEADER (olddecl)
		    || DECL_IN_SYSTEM_HEADER (newdecl)))
	 {
	   warning_with_decl (newdecl, "redefinition of `%s'");
	   warning_with_decl
	     (olddecl,
	      ((DECL_INITIAL (olddecl)
		&& current_binding_level == global_binding_level)
	       ? "`%s' previously defined here"
	       : "`%s' previously declared here"));
	 }
       else if (TREE_CODE (olddecl) == FUNCTION_DECL
		&& DECL_INITIAL (olddecl) != 0
		&& TYPE_ARG_TYPES (oldtype) == 0
		&& TYPE_ARG_TYPES (newtype) != 0
		&& TYPE_ACTUAL_ARG_TYPES (oldtype) != 0)
	 {
	   tree type, parm;
	   int nargs;
	   /* Prototype decl follows defn w/o prototype.  */

	   for (parm = TYPE_ACTUAL_ARG_TYPES (oldtype),
		type = TYPE_ARG_TYPES (newtype),
		nargs = 1;
		;
		parm = TREE_CHAIN (parm), type = TREE_CHAIN (type), nargs++)
	     {
	       if (TYPE_MAIN_VARIANT (TREE_VALUE (parm)) == void_type_node
		   && TYPE_MAIN_VARIANT (TREE_VALUE (type)) == void_type_node)
		 {
		   warning_with_decl (newdecl, "prototype for `%s' follows");
		   warning_with_decl (olddecl, "non-prototype definition here");
		   break;
		 }
	       if (TYPE_MAIN_VARIANT (TREE_VALUE (parm)) == void_type_node
		   || TYPE_MAIN_VARIANT (TREE_VALUE (type)) == void_type_node)
		 {
		   error_with_decl (newdecl,
				    "prototype for `%s' follows and number of arguments doesn't match");
		   error_with_decl (olddecl, "non-prototype definition here");
		   errmsg = 1;
		   break;
		 }
	       /* Type for passing arg must be consistent
		  with that declared for the arg.  */
	       if (! comptypes (TREE_VALUE (parm), TREE_VALUE (type))
		   /* If -traditional, allow `unsigned int' instead of `int'
		      in the prototype.  */
		   && (! (flag_traditional
			  && TYPE_MAIN_VARIANT (TREE_VALUE (parm)) == integer_type_node
			  && TYPE_MAIN_VARIANT (TREE_VALUE (type)) == unsigned_type_node)))
		 {
		   error_with_decl (newdecl,
				    "prototype for `%s' follows and argument %d doesn't match",
				    nargs);
		   error_with_decl (olddecl, "non-prototype definition here");
		   errmsg = 1;
		   break;
		 }
	     }
	 }
       /* Warn about mismatches in various flags.  */
       else
	 {
	   /* Warn if function is now inline
	      but was previously declared not inline and has been called.  */
	   if (TREE_CODE (olddecl) == FUNCTION_DECL
	       && ! DECL_DECLARED_INLINE_P (olddecl)
	       && DECL_DECLARED_INLINE_P (newdecl)
	       && TREE_USED (olddecl))
	     warning_with_decl (newdecl,
				"`%s' declared inline after being called");
	   if (TREE_CODE (olddecl) == FUNCTION_DECL
	       && ! DECL_DECLARED_INLINE_P (olddecl)
	       && DECL_DECLARED_INLINE_P (newdecl)
	       && DECL_INITIAL (olddecl) != 0)
	     warning_with_decl (newdecl,
				"`%s' declared inline after its definition");

	   /* If pedantic, warn when static declaration follows a non-static
	      declaration.  Otherwise, do so only for functions.  */
	   if ((pedantic || TREE_CODE (olddecl) == FUNCTION_DECL)
	       && TREE_PUBLIC (olddecl)
	       && !TREE_PUBLIC (newdecl))
	     warning_with_decl (newdecl, "static declaration for `%s' follows non-static");

	   /* If warn_traditional, warn when a non-static function
	      declaration follows a static one.  */
	   if (warn_traditional && !in_system_header
	       && TREE_CODE (olddecl) == FUNCTION_DECL
	       && !TREE_PUBLIC (olddecl)
	       && TREE_PUBLIC (newdecl))
	     warning_with_decl (newdecl, "non-static declaration for `%s' follows static");

	   /* Warn when const declaration follows a non-const
	      declaration, but not for functions.  */
	   if (TREE_CODE (olddecl) != FUNCTION_DECL
	       && !TREE_READONLY (olddecl)
	       && TREE_READONLY (newdecl))
	     warning_with_decl (newdecl, "const declaration for `%s' follows non-const");
	   /* These bits are logically part of the type, for variables.
	      But not for functions
	      (where qualifiers are not valid ANSI anyway).  */
	   else if (pedantic && TREE_CODE (olddecl) != FUNCTION_DECL
	       && (TREE_READONLY (newdecl) != TREE_READONLY (olddecl)
		   || TREE_THIS_VOLATILE (newdecl) != TREE_THIS_VOLATILE (olddecl)))
	     pedwarn_with_decl (newdecl, "type qualifiers for `%s' conflict with previous decl");
	 }
     }

   /* Optionally warn about more than one declaration for the same name.  */
   if (errmsg == 0 && warn_redundant_decls && DECL_SOURCE_LINE (olddecl) != 0
       /* Don't warn about a function declaration
	  followed by a definition.  */
       && !(TREE_CODE (newdecl) == FUNCTION_DECL && DECL_INITIAL (newdecl) != 0
	    && DECL_INITIAL (olddecl) == 0)
       /* Don't warn about extern decl followed by (tentative) definition.  */
       && !(DECL_EXTERNAL (olddecl) && ! DECL_EXTERNAL (newdecl)))
     {
       warning_with_decl (newdecl, "redundant redeclaration of `%s' in same scope");
       warning_with_decl (olddecl, "previous declaration of `%s'");
     }

   /* Copy all the DECL_... slots specified in the new decl
      except for any that we copy here from the old type.

      Past this point, we don't change OLDTYPE and NEWTYPE
      even if we change the types of NEWDECL and OLDDECL.  */

   if (types_match)
     {
       /* When copying info to olddecl, we store into write_olddecl
	  instead.  This allows us to avoid modifying olddecl when
	  different_binding_level is true.  */
       tree write_olddecl = different_binding_level ? newdecl : olddecl;

       /* Merge the data types specified in the two decls.  */
       if (TREE_CODE (newdecl) != FUNCTION_DECL || !DECL_BUILT_IN (olddecl))
	 {
	   if (different_binding_level)
	     {
	       if (TYPE_ARG_TYPES (oldtype) != 0
		   && TYPE_ARG_TYPES (newtype) == 0)
		 TREE_TYPE (newdecl) = common_type (newtype, oldtype);
	       else
		 TREE_TYPE (newdecl)
		   = build_type_attribute_variant
		     (newtype,
		      merge_attributes (TYPE_ATTRIBUTES (newtype),
					TYPE_ATTRIBUTES (oldtype)));
	     }
	   else
	     TREE_TYPE (newdecl)
	       = TREE_TYPE (olddecl)
		 = common_type (newtype, oldtype);
	 }

       /* Lay the type out, unless already done.  */
       if (oldtype != TREE_TYPE (newdecl))
	 {
	   if (TREE_TYPE (newdecl) != error_mark_node)
	     layout_type (TREE_TYPE (newdecl));
	   if (TREE_CODE (newdecl) != FUNCTION_DECL
	       && TREE_CODE (newdecl) != TYPE_DECL
	       && TREE_CODE (newdecl) != CONST_DECL)
	     layout_decl (newdecl, 0);
	 }
       else
	 {
	   /* Since the type is OLDDECL's, make OLDDECL's size go with.  */
	   DECL_SIZE (newdecl) = DECL_SIZE (olddecl);
	   DECL_SIZE_UNIT (newdecl) = DECL_SIZE_UNIT (olddecl);
	   DECL_MODE (newdecl) = DECL_MODE (olddecl);
	   if (TREE_CODE (olddecl) != FUNCTION_DECL)
	     if (DECL_ALIGN (olddecl) > DECL_ALIGN (newdecl))
	       {
		 DECL_ALIGN (newdecl) = DECL_ALIGN (olddecl);
		 DECL_USER_ALIGN (newdecl) |= DECL_ALIGN (olddecl);
	       }
	 }

       /* Keep the old rtl since we can safely use it.  */
       COPY_DECL_RTL (olddecl, newdecl);

       /* Merge the type qualifiers.  */
       if (TREE_CODE (olddecl) == FUNCTION_DECL
	   && DECL_BUILT_IN_NONANSI (olddecl) && TREE_THIS_VOLATILE (olddecl)
	   && ! TREE_THIS_VOLATILE (newdecl))
	 TREE_THIS_VOLATILE (write_olddecl) = 0;

       if (TREE_READONLY (newdecl))
	 TREE_READONLY (write_olddecl) = 1;

       if (TREE_THIS_VOLATILE (newdecl))
	 {
	   TREE_THIS_VOLATILE (write_olddecl) = 1;
	   if (TREE_CODE (newdecl) == VAR_DECL
	       /* If an automatic variable is re-declared in the same
		  function scope, but the old declaration was not
		  volatile, make_var_volatile() would crash because the
		  variable would have been assigned to a pseudo, not a
		  MEM.  Since this duplicate declaration is invalid
		  anyway, we just skip the call.  */
	       && errmsg == 0)
	     make_var_volatile (newdecl);
	 }

       /* Keep source location of definition rather than declaration.  */
       /* When called with different_binding_level set, keep the old
	  information so that meaningful diagnostics can be given.  */
       if (DECL_INITIAL (newdecl) == 0 && DECL_INITIAL (olddecl) != 0
	   && ! different_binding_level)
	 {
	   DECL_SOURCE_LINE (newdecl) = DECL_SOURCE_LINE (olddecl);
	   DECL_SOURCE_FILE (newdecl) = DECL_SOURCE_FILE (olddecl);
	 }

       /* Merge the unused-warning information.  */
       if (DECL_IN_SYSTEM_HEADER (olddecl))
	 DECL_IN_SYSTEM_HEADER (newdecl) = 1;
       else if (DECL_IN_SYSTEM_HEADER (newdecl))
	 DECL_IN_SYSTEM_HEADER (write_olddecl) = 1;

       /* Merge the initialization information.  */
       /* When called with different_binding_level set, don't copy over
	  DECL_INITIAL, so that we don't accidentally change function
	  declarations into function definitions.  */
       if (DECL_INITIAL (newdecl) == 0 && ! different_binding_level)
	 DECL_INITIAL (newdecl) = DECL_INITIAL (olddecl);

       /* Merge the section attribute.
	  We want to issue an error if the sections conflict but that must be
	  done later in decl_attributes since we are called before attributes
	  are assigned.  */
       if (DECL_SECTION_NAME (newdecl) == NULL_TREE)
	 DECL_SECTION_NAME (newdecl) = DECL_SECTION_NAME (olddecl);

       /* Copy the assembler name.
	  Currently, it can only be defined in the prototype.  */
       COPY_DECL_ASSEMBLER_NAME (olddecl, newdecl);

       if (TREE_CODE (newdecl) == FUNCTION_DECL)
	 {
	   DECL_STATIC_CONSTRUCTOR(newdecl) |= DECL_STATIC_CONSTRUCTOR(olddecl);
	   DECL_STATIC_DESTRUCTOR (newdecl) |= DECL_STATIC_DESTRUCTOR (olddecl);
	   DECL_NO_LIMIT_STACK (newdecl) |= DECL_NO_LIMIT_STACK (olddecl);
	   DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT (newdecl)
	     |= DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT (olddecl);
	 }
     }
   /* If cannot merge, then use the new type and qualifiers,
      and don't preserve the old rtl.  */
   else if (! different_binding_level)
     {
       TREE_TYPE (olddecl) = TREE_TYPE (newdecl);
       TREE_READONLY (olddecl) = TREE_READONLY (newdecl);
       TREE_THIS_VOLATILE (olddecl) = TREE_THIS_VOLATILE (newdecl);
       TREE_SIDE_EFFECTS (olddecl) = TREE_SIDE_EFFECTS (newdecl);
     }

   /* Merge the storage class information.  */
   merge_weak (newdecl, olddecl);

   /* For functions, static overrides non-static.  */
   if (TREE_CODE (newdecl) == FUNCTION_DECL)
     {
       TREE_PUBLIC (newdecl) &= TREE_PUBLIC (olddecl);
       /* This is since we don't automatically
	  copy the attributes of NEWDECL into OLDDECL.  */
       /* No need to worry about different_binding_level here because
	  then TREE_PUBLIC (newdecl) was true.  */
       TREE_PUBLIC (olddecl) = TREE_PUBLIC (newdecl);
       /* If this clears `static', clear it in the identifier too.  */
       if (! TREE_PUBLIC (olddecl))
	 TREE_PUBLIC (DECL_NAME (olddecl)) = 0;
     }
   if (DECL_EXTERNAL (newdecl))
     {
       if (! different_binding_level)
	 {
	   /* Don't mess with these flags on local externs; they remain
	      external even if there's a declaration at file scope which
	      isn't.  */
	   TREE_STATIC (newdecl) = TREE_STATIC (olddecl);
	   DECL_EXTERNAL (newdecl) = DECL_EXTERNAL (olddecl);
	 }
       /* An extern decl does not override previous storage class.  */
       TREE_PUBLIC (newdecl) = TREE_PUBLIC (olddecl);
       if (! DECL_EXTERNAL (newdecl))
	 DECL_CONTEXT (newdecl) = DECL_CONTEXT (olddecl);
     }
   else
     {
       TREE_STATIC (olddecl) = TREE_STATIC (newdecl);
       TREE_PUBLIC (olddecl) = TREE_PUBLIC (newdecl);
     }

   if (TREE_CODE (newdecl) == FUNCTION_DECL)
     {
       /* If we're redefining a function previously defined as extern
	  inline, make sure we emit debug info for the inline before we
	  throw it away, in case it was inlined into a function that hasn't
	  been written out yet.  */
       if (new_is_definition && DECL_INITIAL (olddecl) && TREE_USED (olddecl))
	 {
	   (*debug_hooks->outlining_inline_function) (olddecl);

	   /* The new defn must not be inline.  */
	   DECL_INLINE (newdecl) = 0;
	   DECL_UNINLINABLE (newdecl) = 1;
	 }
       else
	 {
	   /* If either decl says `inline', this fn is inline,
	      unless its definition was passed already.  */
	   if (DECL_DECLARED_INLINE_P (newdecl)
	       || DECL_DECLARED_INLINE_P (olddecl))
	     DECL_DECLARED_INLINE_P (newdecl) = 1;

	   DECL_UNINLINABLE (newdecl) = DECL_UNINLINABLE (olddecl)
	     = (DECL_UNINLINABLE (newdecl) || DECL_UNINLINABLE (olddecl));
	 }

       if (DECL_BUILT_IN (olddecl))
	 {
	   /* Get rid of any built-in function if new arg types don't match it
	      or if we have a function definition.  */
	   if (! types_match || new_is_definition)
	     {
	       if (! different_binding_level)
		 {
		   TREE_TYPE (olddecl) = TREE_TYPE (newdecl);
		   DECL_BUILT_IN_CLASS (olddecl) = NOT_BUILT_IN;
		 }
	     }
	   else
	     {
	       /* If redeclaring a builtin function, and not a definition,
		  it stays built in.  */
	       DECL_BUILT_IN_CLASS (newdecl) = DECL_BUILT_IN_CLASS (olddecl);
	       DECL_FUNCTION_CODE (newdecl) = DECL_FUNCTION_CODE (olddecl);
	     }
	 }

       /* Also preserve various other info from the definition.  */
       if (! new_is_definition)
	 {
	   DECL_RESULT (newdecl) = DECL_RESULT (olddecl);
	   /* When called with different_binding_level set, don't copy over
	      DECL_INITIAL, so that we don't accidentally change function
	      declarations into function definitions.  */
	   if (! different_binding_level)
	     DECL_INITIAL (newdecl) = DECL_INITIAL (olddecl);
	   DECL_SAVED_INSNS (newdecl) = DECL_SAVED_INSNS (olddecl);
	   DECL_SAVED_TREE (newdecl) = DECL_SAVED_TREE (olddecl);
	   DECL_NUM_STMTS (newdecl) = DECL_NUM_STMTS (olddecl);
	   DECL_ARGUMENTS (newdecl) = DECL_ARGUMENTS (olddecl);

	   /* Set DECL_INLINE on the declaration if we've got a body
	      from which to instantiate.  */
	   if (DECL_INLINE (olddecl) && ! DECL_UNINLINABLE (newdecl))
	     {
	       DECL_INLINE (newdecl) = 1;
	       DECL_ABSTRACT_ORIGIN (newdecl)
		 = (different_binding_level
		    ? DECL_ORIGIN (olddecl)
		    : DECL_ABSTRACT_ORIGIN (olddecl));
	     }
	 }
       else
	 {
	   /* If a previous declaration said inline, mark the
	      definition as inlinable.  */
	   if (DECL_DECLARED_INLINE_P (newdecl)
	       && ! DECL_UNINLINABLE (newdecl))
	     DECL_INLINE (newdecl) = 1;
	 }
     }
   if (different_binding_level)
     return 0;

   /* Copy most of the decl-specific fields of NEWDECL into OLDDECL.
      But preserve OLDDECL's DECL_UID.  */
   {
     unsigned olddecl_uid = DECL_UID (olddecl);

     memcpy ((char *) olddecl + sizeof (struct tree_common),
	     (char *) newdecl + sizeof (struct tree_common),
	     sizeof (struct tree_decl) - sizeof (struct tree_common));
     DECL_UID (olddecl) = olddecl_uid;
   }

   /* NEWDECL contains the merged attribute lists.
      Update OLDDECL to be the same.  */
   DECL_ATTRIBUTES (olddecl) = DECL_ATTRIBUTES (newdecl);

   return 1;
 }

 /* Check whether decl-node X shadows an existing declaration.
    OLDLOCAL is the old IDENTIFIER_LOCAL_VALUE of the DECL_NAME of X,
    which might be a NULL_TREE.  */
 static void
 warn_if_shadowing (x, oldlocal)
      tree x, oldlocal;
 {
   tree name;

   if (DECL_EXTERNAL (x))
     return;

   name = DECL_NAME (x);

   /* Warn if shadowing an argument at the top level of the body.  */
   if (oldlocal != 0
       /* This warning doesn't apply to the parms of a nested fcn.  */
       && ! current_binding_level->parm_flag
       /* Check that this is one level down from the parms.  */
       && current_binding_level->level_chain->parm_flag
       /* Check that the decl being shadowed
	  comes from the parm level, one level up.  */
       && chain_member (oldlocal, current_binding_level->level_chain->names))
     {
       if (TREE_CODE (oldlocal) == PARM_DECL)
	 pedwarn ("declaration of `%s' shadows a parameter",
		  IDENTIFIER_POINTER (name));
       else
	 pedwarn ("declaration of `%s' shadows a symbol from the parameter list",
		  IDENTIFIER_POINTER (name));
     }
   /* Maybe warn if shadowing something else.  */
   else if (warn_shadow
	    /* No shadow warnings for internally generated vars.  */
	    && DECL_SOURCE_LINE (x) != 0
	    /* No shadow warnings for vars made for inlining.  */
	    && ! DECL_FROM_INLINE (x))
     {
       if (TREE_CODE (x) == PARM_DECL
	   && current_binding_level->level_chain->parm_flag)
	 /* Don't warn about the parm names in function declarator
	    within a function declarator.
	    It would be nice to avoid warning in any function
	    declarator in a declaration, as opposed to a definition,
	    but there is no way to tell it's not a definition.  */
	 ;
       else if (oldlocal)
	 {
	   if (TREE_CODE (oldlocal) == PARM_DECL)
	     shadow_warning ("a parameter", name, oldlocal);
	   else
	     shadow_warning ("a previous local", name, oldlocal);
	 }
       else if (IDENTIFIER_GLOBAL_VALUE (name) != 0
		&& IDENTIFIER_GLOBAL_VALUE (name) != error_mark_node)
	 shadow_warning ("a global declaration", name,
			 IDENTIFIER_GLOBAL_VALUE (name));
     }
 }

 /* Record a decl-node X as belonging to the current lexical scope.
    Check for errors (such as an incompatible declaration for the same
    name already seen in the same scope).

    Returns either X or an old decl for the same name.
    If an old decl is returned, it may have been smashed
    to agree with what X says.  */

 tree
 pushdecl (x)
      tree x;
 {
   tree t;
   tree name = DECL_NAME (x);
   struct binding_level *b = current_binding_level;

   /* Functions need the lang_decl data.  */
   if (TREE_CODE (x) == FUNCTION_DECL && ! DECL_LANG_SPECIFIC (x))
     DECL_LANG_SPECIFIC (x) = (struct lang_decl *)
       ggc_alloc_cleared (sizeof (struct lang_decl));

   DECL_CONTEXT (x) = current_function_decl;
   /* A local extern declaration for a function doesn't constitute nesting.
      A local auto declaration does, since it's a forward decl
      for a nested function coming later.  */
   if ((TREE_CODE (x) == FUNCTION_DECL || TREE_CODE (x) == VAR_DECL)
       && DECL_INITIAL (x) == 0 && DECL_EXTERNAL (x))
     DECL_CONTEXT (x) = 0;

   if (name)
     {
       int different_binding_level = 0;

       if (warn_nested_externs
	   && DECL_EXTERNAL (x)
	   && b != global_binding_level
	   && x != IDENTIFIER_IMPLICIT_DECL (name)
	   /* No error messages for __FUNCTION__ and __PRETTY_FUNCTION__.  */
	   && !DECL_IN_SYSTEM_HEADER (x))
	 warning ("nested extern declaration of `%s'",
		  IDENTIFIER_POINTER (name));

       t = lookup_name_current_level (name);
       /* Don't type check externs here when -traditional.  This is so that
	  code with conflicting declarations inside blocks will get warnings
	  not errors.  X11 for instance depends on this.  */
       if (! t && DECL_EXTERNAL (x) && TREE_PUBLIC (x) && ! flag_traditional)
	 {
	   t = IDENTIFIER_GLOBAL_VALUE (name);
	   /* Type decls at global scope don't conflict with externs declared
	      inside lexical blocks.  */
	   if (! t || TREE_CODE (t) == TYPE_DECL)
	     /* If there's no visible global declaration, try for an
		invisible one.  */
	     t = IDENTIFIER_LIMBO_VALUE (name);
	   different_binding_level = 1;
	 }
       if (t != 0 && t == error_mark_node)
	 /* error_mark_node is 0 for a while during initialization!  */
	 {
	   t = 0;
	   error_with_decl (x, "`%s' used prior to declaration");
	 }

       /* If this decl is `static' and an implicit decl was seen previously,
	  warn.  But don't complain if -traditional,
	  since traditional compilers don't complain.  */
       if (! flag_traditional && TREE_PUBLIC (name)
	   /* Don't test for DECL_EXTERNAL, because grokdeclarator
	      sets this for all functions.  */
	   && ! TREE_PUBLIC (x)
	   && (TREE_CODE (x) == FUNCTION_DECL || b == global_binding_level)
	   /* We used to warn also for explicit extern followed by static,
	      but sometimes you need to do it that way.  */
	   && IDENTIFIER_IMPLICIT_DECL (name) != 0)
	 {
	   pedwarn ("`%s' was declared implicitly `extern' and later `static'",
		    IDENTIFIER_POINTER (name));
	   pedwarn_with_file_and_line
	     (DECL_SOURCE_FILE (IDENTIFIER_IMPLICIT_DECL (name)),
	      DECL_SOURCE_LINE (IDENTIFIER_IMPLICIT_DECL (name)),
	      "previous declaration of `%s'",
	      IDENTIFIER_POINTER (name));
	   TREE_THIS_VOLATILE (name) = 1;
	 }

       if (t != 0 && duplicate_decls (x, t, different_binding_level))
	 {
	   if (TREE_CODE (t) == PARM_DECL)
	     {
	       /* Don't allow more than one "real" duplicate
		  of a forward parm decl.  */
	       TREE_ASM_WRITTEN (t) = TREE_ASM_WRITTEN (x);
	       return t;
	     }
	   return t;
	 }

       /* If we are processing a typedef statement, generate a whole new
	  ..._TYPE node (which will be just an variant of the existing
	  ..._TYPE node with identical properties) and then install the
	  TYPE_DECL node generated to represent the typedef name as the
	  TYPE_NAME of this brand new (duplicate) ..._TYPE node.

	  The whole point here is to end up with a situation where each
	  and every ..._TYPE node the compiler creates will be uniquely
	  associated with AT MOST one node representing a typedef name.
	  This way, even though the compiler substitutes corresponding
	  ..._TYPE nodes for TYPE_DECL (i.e. "typedef name") nodes very
	  early on, later parts of the compiler can always do the reverse
	  translation and get back the corresponding typedef name.  For
	  example, given:

		 typedef struct S MY_TYPE;
		 MY_TYPE object;

	  Later parts of the compiler might only know that `object' was of
	  type `struct S' if it were not for code just below.  With this
	  code however, later parts of the compiler see something like:

		 struct S' == struct S
		 typedef struct S' MY_TYPE;
		 struct S' object;

	  And they can then deduce (from the node for type struct S') that
	  the original object declaration was:

		 MY_TYPE object;

	  Being able to do this is important for proper support of protoize,
	  and also for generating precise symbolic debugging information
	  which takes full account of the programmer's (typedef) vocabulary.

	  Obviously, we don't want to generate a duplicate ..._TYPE node if
	  the TYPE_DECL node that we are now processing really represents a
	  standard built-in type.

	  Since all standard types are effectively declared at line zero
	  in the source file, we can easily check to see if we are working
	  on a standard type by checking the current value of lineno.  */

       if (TREE_CODE (x) == TYPE_DECL)
	 {
	   if (DECL_SOURCE_LINE (x) == 0)
	     {
	       if (TYPE_NAME (TREE_TYPE (x)) == 0)
		 TYPE_NAME (TREE_TYPE (x)) = x;
	     }
	   else if (TREE_TYPE (x) != error_mark_node
		    && DECL_ORIGINAL_TYPE (x) == NULL_TREE)
	     {
	       tree tt = TREE_TYPE (x);
	       DECL_ORIGINAL_TYPE (x) = tt;
	       tt = build_type_copy (tt);
	       TYPE_NAME (tt) = x;
	       TREE_USED (tt) = TREE_USED (x);
	       TREE_TYPE (x) = tt;
	     }
	 }

       /* Multiple external decls of the same identifier ought to match.
	  Check against both global declarations (when traditional) and out of
	  scope (limbo) block level declarations.

	  We get warnings about inline functions where they are defined.
	  Avoid duplicate warnings where they are used.  */
       if (TREE_PUBLIC (x)
	   && ! (TREE_CODE (x) == FUNCTION_DECL && DECL_INLINE (x)))
	 {
	   tree decl;

	   if (flag_traditional && IDENTIFIER_GLOBAL_VALUE (name) != 0
	       && (DECL_EXTERNAL (IDENTIFIER_GLOBAL_VALUE (name))
		   || TREE_PUBLIC (IDENTIFIER_GLOBAL_VALUE (name))))
	     decl = IDENTIFIER_GLOBAL_VALUE (name);
	   else if (IDENTIFIER_LIMBO_VALUE (name) != 0)
	     /* Decls in limbo are always extern, so no need to check that.  */
	     decl = IDENTIFIER_LIMBO_VALUE (name);
	   else
	     decl = 0;

	   if (decl && ! comptypes (TREE_TYPE (x), TREE_TYPE (decl))
	       /* If old decl is built-in, we already warned if we should.  */
	       && !DECL_BUILT_IN (decl))
	     {
	       pedwarn_with_decl (x,
				  "type mismatch with previous external decl");
	       pedwarn_with_decl (decl, "previous external decl of `%s'");
	     }
	 }

       /* If a function has had an implicit declaration, and then is defined,
	  make sure they are compatible.  */

       if (IDENTIFIER_IMPLICIT_DECL (name) != 0
	   && IDENTIFIER_GLOBAL_VALUE (name) == 0
	   && TREE_CODE (x) == FUNCTION_DECL
	   && ! comptypes (TREE_TYPE (x),
			   TREE_TYPE (IDENTIFIER_IMPLICIT_DECL (name))))
	 {
	   warning_with_decl (x, "type mismatch with previous implicit declaration");
	   warning_with_decl (IDENTIFIER_IMPLICIT_DECL (name),
			      "previous implicit declaration of `%s'");
	 }

       /* In PCC-compatibility mode, extern decls of vars with no current decl
	  take effect at top level no matter where they are.  */
       if (flag_traditional && DECL_EXTERNAL (x)
	   && lookup_name (name) == 0)
	 {
	   tree type = TREE_TYPE (x);

	   /* But don't do this if the type contains temporary nodes.  */
	   while (type)
	     {
	       if (type == error_mark_node)
		 break;
	       if (TYPE_CONTEXT (type))
		 {
		   warning_with_decl (x, "type of external `%s' is not global");
		   /* By exiting the loop early, we leave TYPE nonzero,
		      and thus prevent globalization of the decl.  */
		   break;
		 }
	       else if (TREE_CODE (type) == FUNCTION_TYPE
			&& TYPE_ARG_TYPES (type) != 0)
		 /* The types might not be truly local,
		    but the list of arg types certainly is temporary.
		    Since prototypes are nontraditional,
		    ok not to do the traditional thing.  */
		 break;
	       type = TREE_TYPE (type);
	     }

	   if (type == 0)
	     b = global_binding_level;
	 }

       /* This name is new in its binding level.
	  Install the new declaration and return it.  */
       if (b == global_binding_level)
	 {
	   /* Install a global value.  */

	   /* If the first global decl has external linkage,
	      warn if we later see static one.  */
	   if (IDENTIFIER_GLOBAL_VALUE (name) == 0 && TREE_PUBLIC (x))
	     TREE_PUBLIC (name) = 1;

	   IDENTIFIER_GLOBAL_VALUE (name) = x;

	   /* We no longer care about any previous block level declarations.  */
	   IDENTIFIER_LIMBO_VALUE (name) = 0;

	   /* Don't forget if the function was used via an implicit decl.  */
	   if (IDENTIFIER_IMPLICIT_DECL (name)
	       && TREE_USED (IDENTIFIER_IMPLICIT_DECL (name)))
	     TREE_USED (x) = 1, TREE_USED (name) = 1;

	   /* Don't forget if its address was taken in that way.  */
	   if (IDENTIFIER_IMPLICIT_DECL (name)
	       && TREE_ADDRESSABLE (IDENTIFIER_IMPLICIT_DECL (name)))
	     TREE_ADDRESSABLE (x) = 1;

	   /* Warn about mismatches against previous implicit decl.  */
	   if (IDENTIFIER_IMPLICIT_DECL (name) != 0
	       /* If this real decl matches the implicit, don't complain.  */
	       && ! (TREE_CODE (x) == FUNCTION_DECL
		     && (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (x)))
			 == integer_type_node)))
	     pedwarn ("`%s' was previously implicitly declared to return `int'",
		      IDENTIFIER_POINTER (name));

	   /* If this decl is `static' and an `extern' was seen previously,
	      that is erroneous.  */
	   if (TREE_PUBLIC (name)
	       && ! TREE_PUBLIC (x) && ! DECL_EXTERNAL (x))
	     {
	       /* Okay to redeclare an ANSI built-in as static.  */
	       if (t != 0 && DECL_BUILT_IN (t))
		 ;
	       /* Okay to declare a non-ANSI built-in as anything.  */
	       else if (t != 0 && DECL_BUILT_IN_NONANSI (t))
		 ;
	       /* Okay to have global type decl after an earlier extern
		  declaration inside a lexical block.  */
	       else if (TREE_CODE (x) == TYPE_DECL)
		 ;
	       else if (IDENTIFIER_IMPLICIT_DECL (name))
		 {
		   if (! TREE_THIS_VOLATILE (name))
		     pedwarn ("`%s' was declared implicitly `extern' and later `static'",
			      IDENTIFIER_POINTER (name));
		 }
	       else
		 pedwarn ("`%s' was declared `extern' and later `static'",
			  IDENTIFIER_POINTER (name));
	     }
	 }
       else
	 {
	   /* Here to install a non-global value.  */
	   tree oldlocal = IDENTIFIER_LOCAL_VALUE (name);
	   tree oldglobal = IDENTIFIER_GLOBAL_VALUE (name);

	   IDENTIFIER_LOCAL_VALUE (name) = x;

	   /* If this is an extern function declaration, see if we
	      have a global definition or declaration for the function.  */
	   if (oldlocal == 0
	       && oldglobal != 0
	       && TREE_CODE (x) == FUNCTION_DECL
	       && TREE_CODE (oldglobal) == FUNCTION_DECL
	       && DECL_EXTERNAL (x)
	       && ! DECL_DECLARED_INLINE_P (x))
	     {
	       /* We have one.  Their types must agree.  */
	       if (! comptypes (TREE_TYPE (x),
				TREE_TYPE (IDENTIFIER_GLOBAL_VALUE (name))))
		 pedwarn_with_decl (x, "extern declaration of `%s' doesn't match global one");
	       else
		 {
		   /* Inner extern decl is inline if global one is.
		      Copy enough to really inline it.  */
		   if (DECL_DECLARED_INLINE_P (oldglobal))
		     {
		       DECL_DECLARED_INLINE_P (x)
			 = DECL_DECLARED_INLINE_P (oldglobal);
		       DECL_INLINE (x) = DECL_INLINE (oldglobal);
		       DECL_INITIAL (x) = (current_function_decl == oldglobal
					   ? 0 : DECL_INITIAL (oldglobal));
		       DECL_SAVED_INSNS (x) = DECL_SAVED_INSNS (oldglobal);
		       DECL_NUM_STMTS (x) = DECL_NUM_STMTS (oldglobal);
		       DECL_ARGUMENTS (x) = DECL_ARGUMENTS (oldglobal);
		       DECL_RESULT (x) = DECL_RESULT (oldglobal);
		       TREE_ASM_WRITTEN (x) = TREE_ASM_WRITTEN (oldglobal);
		       DECL_ABSTRACT_ORIGIN (x)
			 = DECL_ABSTRACT_ORIGIN (oldglobal);
		     }
		   /* Inner extern decl is built-in if global one is.  */
		   if (DECL_BUILT_IN (oldglobal))
		     {
		       DECL_BUILT_IN_CLASS (x) = DECL_BUILT_IN_CLASS (oldglobal);
		       DECL_FUNCTION_CODE (x) = DECL_FUNCTION_CODE (oldglobal);
		     }
		   /* Keep the arg types from a file-scope fcn defn.  */
		   if (TYPE_ARG_TYPES (TREE_TYPE (oldglobal)) != 0
		       && DECL_INITIAL (oldglobal)
		       && TYPE_ARG_TYPES (TREE_TYPE (x)) == 0)
		     TREE_TYPE (x) = TREE_TYPE (oldglobal);
		 }
	     }

 #if 0
	   /* This case is probably sometimes the right thing to do.  */
	   /* If we have a local external declaration,
	      then any file-scope declaration should not
	      have been static.  */
	   if (oldlocal == 0 && oldglobal != 0
	       && !TREE_PUBLIC (oldglobal)
	       && DECL_EXTERNAL (x) && TREE_PUBLIC (x))
	     warning ("`%s' locally external but globally static",
		      IDENTIFIER_POINTER (name));
 #endif

	   /* If we have a local external declaration,
	      and no file-scope declaration has yet been seen,
	      then if we later have a file-scope decl it must not be static.  */
	   if (oldlocal == 0
	       && DECL_EXTERNAL (x)
	       && TREE_PUBLIC (x))
	     {
	       if (oldglobal == 0)
		 TREE_PUBLIC (name) = 1;

	       /* Save this decl, so that we can do type checking against
		  other decls after it falls out of scope.

		  Only save it once.  This prevents temporary decls created in
		  expand_inline_function from being used here, since this
		  will have been set when the inline function was parsed.
		  It also helps give slightly better warnings.  */
	       if (IDENTIFIER_LIMBO_VALUE (name) == 0)
		 IDENTIFIER_LIMBO_VALUE (name) = x;
	     }

	   warn_if_shadowing (x, oldlocal);

	   /* If storing a local value, there may already be one (inherited).
	      If so, record it for restoration when this binding level ends.  */
	   if (oldlocal != 0)
	     b->shadowed = tree_cons (name, oldlocal, b->shadowed);
	 }

       /* Keep count of variables in this level with incomplete type.
	  If the input is erroneous, we can have error_mark in the type
	  slot (e.g. "f(void a, ...)") - that doesn't count as an
	  incomplete type.  */
       if (TREE_TYPE (x) != error_mark_node
	   && !COMPLETE_TYPE_P (TREE_TYPE (x)))
	 {
	   tree element = TREE_TYPE (x);

	   while (TREE_CODE (element) == ARRAY_TYPE)
	     element = TREE_TYPE (element);
	   if (TREE_CODE (element) == RECORD_TYPE
	       || TREE_CODE (element) == UNION_TYPE)
	     ++b->n_incomplete;
	 }
     }

   /* Put decls on list in reverse order.
      We will reverse them later if necessary.  */
   TREE_CHAIN (x) = b->names;
   b->names = x;

   return x;
 }

 /* Like pushdecl, only it places X in GLOBAL_BINDING_LEVEL, if appropriate.  */

 tree
 pushdecl_top_level (x)
      tree x;
 {
   tree t;
   struct binding_level *b = current_binding_level;

   current_binding_level = global_binding_level;
   t = pushdecl (x);
   current_binding_level = b;
   return t;
 }
 
 /* Generate an implicit declaration for identifier FUNCTIONID
    as a function of type int ().  Print a warning if appropriate.  */

 tree
 implicitly_declare (functionid)
      tree functionid;
 {
   tree decl;
   int traditional_warning = 0;
   /* Only one "implicit declaration" warning per identifier.  */
   int implicit_warning;

   /* We used to reuse an old implicit decl here,
      but this loses with inline functions because it can clobber
      the saved decl chains.  */
 #if 0
   if (IDENTIFIER_IMPLICIT_DECL (functionid) != 0)
     decl = IDENTIFIER_IMPLICIT_DECL (functionid);
   else
 #endif
     decl = build_decl (FUNCTION_DECL, functionid, default_function_type);

   /* Warn of implicit decl following explicit local extern decl.
      This is probably a program designed for traditional C.  */
   if (TREE_PUBLIC (functionid) && IDENTIFIER_GLOBAL_VALUE (functionid) == 0)
     traditional_warning = 1;

   /* Warn once of an implicit declaration.  */
   implicit_warning = (IDENTIFIER_IMPLICIT_DECL (functionid) == 0);

   DECL_EXTERNAL (decl) = 1;
   TREE_PUBLIC (decl) = 1;

   /* Record that we have an implicit decl and this is it.  */
   IDENTIFIER_IMPLICIT_DECL (functionid) = decl;

   /* ANSI standard says implicit declarations are in the innermost block.
      So we record the decl in the standard fashion.
      If flag_traditional is set, pushdecl does it top-level.  */
   pushdecl (decl);

   /* This is a no-op in c-lang.c or something real in objc-actions.c.  */
   maybe_objc_check_decl (decl);

   rest_of_decl_compilation (decl, NULL, 0, 0);

   if (implicit_warning)
     implicit_decl_warning (functionid);
   else if (warn_traditional && traditional_warning)
     warning ("function `%s' was previously declared within a block",
	      IDENTIFIER_POINTER (functionid));

   /* Write a record describing this implicit function declaration to the
      prototypes file (if requested).  */

   gen_aux_info_record (decl, 0, 1, 0);

   /* Possibly apply some default attributes to this implicit declaration.  */
   decl_attributes (&decl, NULL_TREE, 0);

   return decl;
 }

 void
 implicit_decl_warning (id)
      tree id;
 {
   const char *name = IDENTIFIER_POINTER (id);
   if (mesg_implicit_function_declaration == 2)
     error ("implicit declaration of function `%s'", name);
   else if (mesg_implicit_function_declaration == 1)
     warning ("implicit declaration of function `%s'", name);
 }

 /* Return zero if the declaration NEWDECL is valid
    when the declaration OLDDECL (assumed to be for the same name)
    has already been seen.
    Otherwise return 1 if NEWDECL is a redefinition, 2 if it is a redeclaration,
    and 3 if it is a conflicting declaration.  */

 static int
 redeclaration_error_message (newdecl, olddecl)
      tree newdecl, olddecl;
 {
   if (TREE_CODE (newdecl) == TYPE_DECL)
     {
       if (flag_traditional && TREE_TYPE (newdecl) == TREE_TYPE (olddecl))
	 return 0;
       /* pushdecl creates distinct types for TYPE_DECLs by calling
	  build_type_copy, so the above comparison generally fails.  We do
	  another test against the TYPE_MAIN_VARIANT of the olddecl, which
	  is equivalent to what this code used to do before the build_type_copy
	  call.  The variant type distinction should not matter for traditional
	  code, because it doesn't have type qualifiers.  */
       if (flag_traditional
	   && TYPE_MAIN_VARIANT (TREE_TYPE (olddecl)) == TREE_TYPE (newdecl))
	 return 0;
       if (DECL_IN_SYSTEM_HEADER (olddecl) || DECL_IN_SYSTEM_HEADER (newdecl))
	 return 0;
       return 1;
     }
   else if (TREE_CODE (newdecl) == FUNCTION_DECL)
     {
       /* Declarations of functions can insist on internal linkage
	  but they can't be inconsistent with internal linkage,
	  so there can be no error on that account.
	  However defining the same name twice is no good.  */
       if (DECL_INITIAL (olddecl) != 0 && DECL_INITIAL (newdecl) != 0
	   /* However, defining once as extern inline and a second
	      time in another way is ok.  */
	   && ! (DECL_DECLARED_INLINE_P (olddecl) && DECL_EXTERNAL (olddecl)
		&& ! (DECL_DECLARED_INLINE_P (newdecl)
		      && DECL_EXTERNAL (newdecl))))
	 return 1;
       return 0;
     }
   else if (DECL_CONTEXT (newdecl) == NULL_TREE)
     {
       /* Objects declared at top level:  */
       /* If at least one is a reference, it's ok.  */
       if (DECL_EXTERNAL (newdecl) || DECL_EXTERNAL (olddecl))
	 return 0;
       /* Reject two definitions.  */
       if (DECL_INITIAL (olddecl) != 0 && DECL_INITIAL (newdecl) != 0)
	 return 1;
       /* Now we have two tentative defs, or one tentative and one real def.  */
       /* Insist that the linkage match.  */
       if (TREE_PUBLIC (olddecl) != TREE_PUBLIC (newdecl))
	 return 3;
       return 0;
     }
   else if (current_binding_level->parm_flag
	    && TREE_ASM_WRITTEN (olddecl) && !TREE_ASM_WRITTEN (newdecl))
     return 0;
   else
     {
       /* Newdecl has block scope.  If olddecl has block scope also, then
	  reject two definitions, and reject a definition together with an
	  external reference.  Otherwise, it is OK, because newdecl must
	  be an extern reference to olddecl.  */
       if (!(DECL_EXTERNAL (newdecl) && DECL_EXTERNAL (olddecl))
	   && DECL_CONTEXT (newdecl) == DECL_CONTEXT (olddecl))
	 return 2;
       return 0;
     }
 }
 
 /* Get the LABEL_DECL corresponding to identifier ID as a label.
    Create one if none exists so far for the current function.
    This function is called for both label definitions and label references.  */

 tree
 lookup_label (id)
      tree id;
 {
   tree decl = IDENTIFIER_LABEL_VALUE (id);

   if (current_function_decl == 0)
     {
       error ("label %s referenced outside of any function",
	      IDENTIFIER_POINTER (id));
       return 0;
     }

   /* Use a label already defined or ref'd with this name.  */
   if (decl != 0)
     {
       /* But not if it is inherited and wasn't declared to be inheritable.  */
       if (DECL_CONTEXT (decl) != current_function_decl
	   && ! C_DECLARED_LABEL_FLAG (decl))
	 return shadow_label (id);
       return decl;
     }

   decl = build_decl (LABEL_DECL, id, void_type_node);

   /* A label not explicitly declared must be local to where it's ref'd.  */
   DECL_CONTEXT (decl) = current_function_decl;

   DECL_MODE (decl) = VOIDmode;

   /* Say where one reference is to the label,
      for the sake of the error if it is not defined.  */
   DECL_SOURCE_LINE (decl) = lineno;
   DECL_SOURCE_FILE (decl) = input_filename;

   IDENTIFIER_LABEL_VALUE (id) = decl;

   named_labels = tree_cons (NULL_TREE, decl, named_labels);

   return decl;
 }

 /* Make a label named NAME in the current function,
    shadowing silently any that may be inherited from containing functions
    or containing scopes.

    Note that valid use, if the label being shadowed
    comes from another scope in the same function,
    requires calling declare_nonlocal_label right away.  */

 tree
 shadow_label (name)
      tree name;
 {
   tree decl = IDENTIFIER_LABEL_VALUE (name);

   if (decl != 0)
     {
       tree dup;

       /* Check to make sure that the label hasn't already been declared
	  at this label scope */
       for (dup = named_labels; dup; dup = TREE_CHAIN (dup))
	 if (TREE_VALUE (dup) == decl)
	   {
	     error ("duplicate label declaration `%s'",
		    IDENTIFIER_POINTER (name));
	     error_with_decl (TREE_VALUE (dup),
			      "this is a previous declaration");
	     /* Just use the previous declaration.  */
	     return lookup_label (name);
	   }

       shadowed_labels = tree_cons (NULL_TREE, decl, shadowed_labels);
       IDENTIFIER_LABEL_VALUE (name) = decl = 0;
     }

   return lookup_label (name);
 }

 /* Define a label, specifying the location in the source file.
    Return the LABEL_DECL node for the label, if the definition is valid.
    Otherwise return 0.  */

 tree
 define_label (filename, line, name)
      const char *filename;
      int line;
      tree name;
 {
   tree decl = lookup_label (name);

   /* If label with this name is known from an outer context, shadow it.  */
   if (decl != 0 && DECL_CONTEXT (decl) != current_function_decl)
     {
       shadowed_labels = tree_cons (NULL_TREE, decl, shadowed_labels);
       IDENTIFIER_LABEL_VALUE (name) = 0;
       decl = lookup_label (name);
     }

   if (warn_traditional && !in_system_header && lookup_name (name))
     warning_with_file_and_line (filename, line,
				 "traditional C lacks a separate namespace for labels, identifier `%s' conflicts",
				 IDENTIFIER_POINTER (name));

   if (DECL_INITIAL (decl) != 0)
     {
       error_with_file_and_line (filename, line, "duplicate label `%s'",
				 IDENTIFIER_POINTER (name));
       return 0;
     }
   else
     {
       /* Mark label as having been defined.  */
       DECL_INITIAL (decl) = error_mark_node;
       /* Say where in the source.  */
       DECL_SOURCE_FILE (decl) = filename;
       DECL_SOURCE_LINE (decl) = line;
       return decl;
     }
 }
 
 /* Return the list of declarations of the current level.
    Note that this list is in reverse order unless/until
    you nreverse it; and when you do nreverse it, you must
    store the result back using `storedecls' or you will lose.  */

 tree
 getdecls ()
 {
   return current_binding_level->names;
 }

 /* Return the list of type-tags (for structs, etc) of the current level.  */

 tree
 gettags ()
 {
   return current_binding_level->tags;
 }

 /* Store the list of declarations of the current level.
    This is done for the parameter declarations of a function being defined,
    after they are modified in the light of any missing parameters.  */

 static void
 storedecls (decls)
      tree decls;
 {
   current_binding_level->names = decls;
 }

 /* Similarly, store the list of tags of the current level.  */

 static void
 storetags (tags)
      tree tags;
 {
   current_binding_level->tags = tags;
 }
 
 /* Given NAME, an IDENTIFIER_NODE,
    return the structure (or union or enum) definition for that name.
    Searches binding levels from BINDING_LEVEL up to the global level.
    If THISLEVEL_ONLY is nonzero, searches only the specified context
    (but skips any tag-transparent contexts to find one that is
    meaningful for tags).
    CODE says which kind of type the caller wants;
    it is RECORD_TYPE or UNION_TYPE or ENUMERAL_TYPE.
    If the wrong kind of type is found, an error is reported.  */

 static tree
 lookup_tag (code, name, binding_level, thislevel_only)
      enum tree_code code;
      struct binding_level *binding_level;
      tree name;
      int thislevel_only;
 {
   struct binding_level *level;
   int thislevel = 1;

   for (level = binding_level; level; level = level->level_chain)
     {
       tree tail;
       for (tail = level->tags; tail; tail = TREE_CHAIN (tail))
	 {
	   if (TREE_PURPOSE (tail) == name)
	     {
	       if (TREE_CODE (TREE_VALUE (tail)) != code)
		 {
		   /* Definition isn't the kind we were looking for.  */
		   pending_invalid_xref = name;
		   pending_invalid_xref_file = input_filename;
		   pending_invalid_xref_line = lineno;
		   /* If in the same binding level as a declaration as a tag
		      of a different type, this must not be allowed to
		      shadow that tag, so give the error immediately.
		      (For example, "struct foo; union foo;" is invalid.)  */
		   if (thislevel)
		     pending_xref_error ();
		 }
	       return TREE_VALUE (tail);
	     }
	 }
       if (! level->tag_transparent)
	 {
	   if (thislevel_only)
	     return NULL_TREE;
	   thislevel = 0;
	 }
     }
   return NULL_TREE;
 }

 /* Print an error message now
    for a recent invalid struct, union or enum cross reference.
    We don't print them immediately because they are not invalid
    when used in the `struct foo;' construct for shadowing.  */

 void
 pending_xref_error ()
 {
   if (pending_invalid_xref != 0)
     error_with_file_and_line (pending_invalid_xref_file,
			       pending_invalid_xref_line,
			       "`%s' defined as wrong kind of tag",
			       IDENTIFIER_POINTER (pending_invalid_xref));
   pending_invalid_xref = 0;
 }

 /* Given a type, find the tag that was defined for it and return the tag name.
    Otherwise return 0.  */

 static tree
 lookup_tag_reverse (type)
      tree type;
 {
   struct binding_level *level;

   for (level = current_binding_level; level; level = level->level_chain)
     {
       tree tail;
       for (tail = level->tags; tail; tail = TREE_CHAIN (tail))
	 {
	   if (TREE_VALUE (tail) == type)
	     return TREE_PURPOSE (tail);
	 }
     }
   return NULL_TREE;
 }
 
 /* Look up NAME in the current binding level and its superiors
    in the namespace of variables, functions and typedefs.
    Return a ..._DECL node of some kind representing its definition,
    or return 0 if it is undefined.  */

 tree
 lookup_name (name)
      tree name;
 {
   tree val;

   if (current_binding_level != global_binding_level
       && IDENTIFIER_LOCAL_VALUE (name))
     val = IDENTIFIER_LOCAL_VALUE (name);
   else
     val = IDENTIFIER_GLOBAL_VALUE (name);
   return val;
 }

 /* Similar to `lookup_name' but look only at current binding level.  */

 tree
 lookup_name_current_level (name)
      tree name;
 {
   tree t;

   if (current_binding_level == global_binding_level)
     return IDENTIFIER_GLOBAL_VALUE (name);

   if (IDENTIFIER_LOCAL_VALUE (name) == 0)
     return 0;

   for (t = current_binding_level->names; t; t = TREE_CHAIN (t))
     if (DECL_NAME (t) == name)
       break;

   return t;
 }
 
 /* Mark ARG for GC.  */

 static void
 mark_binding_level (arg)
      void *arg;
 {
   struct binding_level *level = *(struct binding_level **) arg;

   for (; level != 0; level = level->level_chain)
     {
       ggc_mark_tree (level->names);
       ggc_mark_tree (level->tags);
       ggc_mark_tree (level->shadowed);
       ggc_mark_tree (level->blocks);
       ggc_mark_tree (level->this_block);
       ggc_mark_tree (level->parm_order);
     }
 }

 /* Create the predefined scalar types of C,
    and some nodes representing standard constants (0, 1, (void *) 0).
    Initialize the global binding level.
    Make definitions for built-in primitive functions.  */

 void
 c_init_decl_processing ()
 {
   tree endlink;
   tree ptr_ftype_void, ptr_ftype_ptr;

   /* Adds some ggc roots, and reserved words for c-parse.in.  */
   c_parse_init ();

   current_function_decl = NULL;
   named_labels = NULL;
   current_binding_level = NULL_BINDING_LEVEL;
   free_binding_level = NULL_BINDING_LEVEL;

   /* Make the binding_level structure for global names.  */
   pushlevel (0);
   global_binding_level = current_binding_level;

   build_common_tree_nodes (flag_signed_char);

   c_common_nodes_and_builtins ();

   boolean_type_node = integer_type_node;
   boolean_true_node = integer_one_node;
   boolean_false_node = integer_zero_node;

   c_bool_type_node = make_unsigned_type (BOOL_TYPE_SIZE);
   TREE_SET_CODE (c_bool_type_node, BOOLEAN_TYPE);
   TYPE_MAX_VALUE (c_bool_type_node) = build_int_2 (1, 0);
   TREE_TYPE (TYPE_MAX_VALUE (c_bool_type_node)) = c_bool_type_node;
   TYPE_PRECISION (c_bool_type_node) = 1;
   pushdecl (build_decl (TYPE_DECL, get_identifier ("_Bool"),
			 c_bool_type_node));
   c_bool_false_node = build_int_2 (0, 0);
   TREE_TYPE (c_bool_false_node) = c_bool_type_node;
   c_bool_true_node = build_int_2 (1, 0);
   TREE_TYPE (c_bool_true_node) = c_bool_type_node;

   endlink = void_list_node;
   ptr_ftype_void = build_function_type (ptr_type_node, endlink);
   ptr_ftype_ptr
     = build_function_type (ptr_type_node,
			    tree_cons (NULL_TREE, ptr_type_node, endlink));

   /* Types which are common to the fortran compiler and libf2c.  When
      changing these, you also need to be concerned with f/com.h.  */

   if (TYPE_PRECISION (float_type_node)
       == TYPE_PRECISION (long_integer_type_node))
     {
       g77_integer_type_node = long_integer_type_node;
       g77_uinteger_type_node = long_unsigned_type_node;
     }
   else if (TYPE_PRECISION (float_type_node)
	    == TYPE_PRECISION (integer_type_node))
     {
       g77_integer_type_node = integer_type_node;
       g77_uinteger_type_node = unsigned_type_node;
     }
   else
     g77_integer_type_node = g77_uinteger_type_node = NULL_TREE;

   if (g77_integer_type_node != NULL_TREE)
     {
       pushdecl (build_decl (TYPE_DECL, get_identifier ("__g77_integer"),
			     g77_integer_type_node));
       pushdecl (build_decl (TYPE_DECL, get_identifier ("__g77_uinteger"),
			     g77_uinteger_type_node));
     }

   if (TYPE_PRECISION (float_type_node) * 2
       == TYPE_PRECISION (long_integer_type_node))
     {
       g77_longint_type_node = long_integer_type_node;
       g77_ulongint_type_node = long_unsigned_type_node;
     }
   else if (TYPE_PRECISION (float_type_node) * 2
	    == TYPE_PRECISION (long_long_integer_type_node))
     {
       g77_longint_type_node = long_long_integer_type_node;
       g77_ulongint_type_node = long_long_unsigned_type_node;
     }
   else
     g77_longint_type_node = g77_ulongint_type_node = NULL_TREE;

   if (g77_longint_type_node != NULL_TREE)
     {
       pushdecl (build_decl (TYPE_DECL, get_identifier ("__g77_longint"),
			     g77_longint_type_node));
       pushdecl (build_decl (TYPE_DECL, get_identifier ("__g77_ulongint"),
			     g77_ulongint_type_node));
     }

   pedantic_lvalues = pedantic;

   make_fname_decl = c_make_fname_decl;
   start_fname_decls ();

   incomplete_decl_finalize_hook = finish_incomplete_decl;

   /* Record our roots.  */

   ggc_add_tree_root (c_global_trees, CTI_MAX);
   ggc_add_root (&c_stmt_tree, 1, sizeof c_stmt_tree, mark_stmt_tree);
   ggc_add_tree_root (&c_scope_stmt_stack, 1);
   ggc_add_tree_root (&named_labels, 1);
   ggc_add_tree_root (&shadowed_labels, 1);
   ggc_add_root (&current_binding_level, 1, sizeof current_binding_level,
		 mark_binding_level);
   ggc_add_root (&label_level_chain, 1, sizeof label_level_chain,
		 mark_binding_level);
   ggc_add_tree_root (&static_ctors, 1);
   ggc_add_tree_root (&static_dtors, 1);
 }

 /* Create the VAR_DECL for __FUNCTION__ etc. ID is the name to give the
    decl, NAME is the initialization string and TYPE_DEP indicates whether
    NAME depended on the type of the function.  As we don't yet implement
    delayed emission of static data, we mark the decl as emitted
    so it is not placed in the output.  Anything using it must therefore pull
    out the STRING_CST initializer directly.  This does mean that these names
    are string merging candidates, which is wrong for C99's __func__.  FIXME.  */

 static tree
 c_make_fname_decl (id, type_dep)
      tree id;
      int type_dep;
 {
   const char *name = fname_as_string (type_dep);
   tree decl, type, init;
   size_t length = strlen (name);

   type =  build_array_type
	   (build_qualified_type (char_type_node, TYPE_QUAL_CONST),
	    build_index_type (size_int (length)));

   decl = build_decl (VAR_DECL, id, type);
   /* We don't push the decl, so have to set its context here.  */
   DECL_CONTEXT (decl) = current_function_decl;

   TREE_STATIC (decl) = 1;
   TREE_READONLY (decl) = 1;
   DECL_ARTIFICIAL (decl) = 1;

   init = build_string (length + 1, name);
   TREE_TYPE (init) = type;
   DECL_INITIAL (decl) = init;

   TREE_USED (decl) = 1;

   finish_decl (decl, init, NULL_TREE);

   return decl;
 }

 /* Return a definition for a builtin function named NAME and whose data type
    is TYPE.  TYPE should be a function type with argument types.
    FUNCTION_CODE tells later passes how to compile calls to this function.
    See tree.h for its possible values.

    If LIBRARY_NAME is nonzero, use that for DECL_ASSEMBLER_NAME,
    the name to be called if we can't opencode the function.  */

 tree
 builtin_function (name, type, function_code, class, library_name)
      const char *name;
      tree type;
      int function_code;
      enum built_in_class class;
      const char *library_name;
 {
   tree decl = build_decl (FUNCTION_DECL, get_identifier (name), type);
   DECL_EXTERNAL (decl) = 1;
   TREE_PUBLIC (decl) = 1;
   /* If -traditional, permit redefining a builtin function any way you like.
      (Though really, if the program redefines these functions,
      it probably won't work right unless compiled with -fno-builtin.)  */
   if (flag_traditional && name[0] != '_')
     DECL_BUILT_IN_NONANSI (decl) = 1;
   if (library_name)
     SET_DECL_ASSEMBLER_NAME (decl, get_identifier (library_name));
   make_decl_rtl (decl, NULL);
   pushdecl (decl);
   DECL_BUILT_IN_CLASS (decl) = class;
   DECL_FUNCTION_CODE (decl) = function_code;

   /* The return builtins leave the current function.  */
   if (function_code == BUILT_IN_RETURN || function_code == BUILT_IN_EH_RETURN)
     TREE_THIS_VOLATILE (decl) = 1;

   /* Warn if a function in the namespace for users
      is used without an occasion to consider it declared.  */
   if (name[0] != '_' || name[1] != '_')
     C_DECL_ANTICIPATED (decl) = 1;

   /* Possibly apply some default attributes to this built-in function.  */
   decl_attributes (&decl, NULL_TREE, 0);

   return decl;
 }

 /* Apply default attributes to a function, if a system function with default
    attributes.  */

 void
 insert_default_attributes (decl)
      tree decl;
 {
 }
 
 /* Called when a declaration is seen that contains no names to declare.
    If its type is a reference to a structure, union or enum inherited
    from a containing scope, shadow that tag name for the current scope
    with a forward reference.
    If its type defines a new named structure or union
    or defines an enum, it is valid but we need not do anything here.
    Otherwise, it is an error.  */

 void
 shadow_tag (declspecs)
      tree declspecs;
 {
   shadow_tag_warned (declspecs, 0);
 }

 void
 shadow_tag_warned (declspecs, warned)
      tree declspecs;
      int warned;
      /* 1 => we have done a pedwarn.  2 => we have done a warning, but
	 no pedwarn.  */
 {
   int found_tag = 0;
   tree link;
   tree specs, attrs;

   pending_invalid_xref = 0;

   /* Remove the attributes from declspecs, since they will confuse the
      following code.  */
   split_specs_attrs (declspecs, &specs, &attrs);

   for (link = specs; link; link = TREE_CHAIN (link))
     {
       tree value = TREE_VALUE (link);
       enum tree_code code = TREE_CODE (value);

       if (code == RECORD_TYPE || code == UNION_TYPE || code == ENUMERAL_TYPE)
	 /* Used to test also that TYPE_SIZE (value) != 0.
	    That caused warning for `struct foo;' at top level in the file.  */
	 {
	   tree name = lookup_tag_reverse (value);
	   tree t;

	   found_tag++;

	   if (name == 0)
	     {
	       if (warned != 1 && code != ENUMERAL_TYPE)
		 /* Empty unnamed enum OK */
		 {
		   pedwarn ("unnamed struct/union that defines no instances");
		   warned = 1;
		 }
	     }
	   else
	     {
	       t = lookup_tag (code, name, current_binding_level, 1);

	       if (t == 0)
		 {
		   t = make_node (code);
		   pushtag (name, t);
		 }
	     }
	 }
       else
	 {
	   if (!warned && ! in_system_header)
	     {
	       warning ("useless keyword or type name in empty declaration");
	       warned = 2;
	     }
	 }
     }

   if (found_tag > 1)
     error ("two types specified in one empty declaration");

   if (warned != 1)
     {
       if (found_tag == 0)
	 pedwarn ("empty declaration");
     }
 }
 
 /* Construct an array declarator.  EXPR is the expression inside [], or
    NULL_TREE.  QUALS are the type qualifiers inside the [] (to be applied
    to the pointer to which a parameter array is converted).  STATIC_P is
    non-zero if "static" is inside the [], zero otherwise.  VLA_UNSPEC_P
    is non-zero is the array is [*], a VLA of unspecified length which is
    nevertheless a complete type (not currently implemented by GCC),
    zero otherwise.  The declarator is constructed as an ARRAY_REF
    (to be decoded by grokdeclarator), whose operand 0 is what's on the
    left of the [] (filled by in set_array_declarator_type) and operand 1
    is the expression inside; whose TREE_TYPE is the type qualifiers and
    which has TREE_STATIC set if "static" is used.  */

 tree
 build_array_declarator (expr, quals, static_p, vla_unspec_p)
      tree expr;
      tree quals;
      int static_p;
      int vla_unspec_p;
 {
   tree decl;
   decl = build_nt (ARRAY_REF, NULL_TREE, expr);
   TREE_TYPE (decl) = quals;
   TREE_STATIC (decl) = (static_p ? 1 : 0);
   if (pedantic && !flag_isoc99)
     {
       if (static_p || quals != NULL_TREE)
	 pedwarn ("ISO C89 does not support `static' or type qualifiers in parameter array declarators");
       if (vla_unspec_p)
	 pedwarn ("ISO C89 does not support `[*]' array declarators");
     }
   if (vla_unspec_p)
     warning ("GCC does not yet properly implement `[*]' array declarators");
   return decl;
 }

 /* Set the type of an array declarator.  DECL is the declarator, as
    constructed by build_array_declarator; TYPE is what appears on the left
    of the [] and goes in operand 0.  ABSTRACT_P is non-zero if it is an
    abstract declarator, zero otherwise; this is used to reject static and
    type qualifiers in abstract declarators, where they are not in the
    C99 grammar.  */

 tree
 set_array_declarator_type (decl, type, abstract_p)
      tree decl;
      tree type;
      int abstract_p;
 {
   TREE_OPERAND (decl, 0) = type;
   if (abstract_p && (TREE_TYPE (decl) != NULL_TREE || TREE_STATIC (decl)))
     error ("static or type qualifiers in abstract declarator");
   return decl;
 }
 
 /* Decode a "typename", such as "int **", returning a ..._TYPE node.  */

 tree
 groktypename (typename)
      tree typename;
 {
   tree specs, attrs;

   if (TREE_CODE (typename) != TREE_LIST)
     return typename;

   split_specs_attrs (TREE_PURPOSE (typename), &specs, &attrs);

   typename = grokdeclarator (TREE_VALUE (typename), specs, TYPENAME, 0);

   /* Apply attributes.  */
   decl_attributes (&typename, attrs, 0);

   return typename;
 }

 /* Return a PARM_DECL node for a given pair of specs and declarator.  */

 tree
 groktypename_in_parm_context (typename)
      tree typename;
 {
   if (TREE_CODE (typename) != TREE_LIST)
     return typename;
   return grokdeclarator (TREE_VALUE (typename),
			  TREE_PURPOSE (typename),
			  PARM, 0);
 }

 /* Decode a declarator in an ordinary declaration or data definition.
    This is called as soon as the type information and variable name
    have been parsed, before parsing the initializer if any.
    Here we create the ..._DECL node, fill in its type,
    and put it on the list of decls for the current context.
    The ..._DECL node is returned as the value.

    Exception: for arrays where the length is not specified,
    the type is left null, to be filled in by `finish_decl'.

    Function definitions do not come here; they go to start_function
    instead.  However, external and forward declarations of functions
    do go through here.  Structure field declarations are done by
    grokfield and not through here.  */

 tree
 start_decl (declarator, declspecs, initialized, attributes)
      tree declarator, declspecs;
      int initialized;
      tree attributes;
 {
   tree decl;
   tree tem;

   /* An object declared as __attribute__((deprecated)) suppresses
      warnings of uses of other deprecated items.  */
   if (lookup_attribute ("deprecated", attributes))
     deprecated_state = DEPRECATED_SUPPRESS;

   decl = grokdeclarator (declarator, declspecs,
			  NORMAL, initialized);

   deprecated_state = DEPRECATED_NORMAL;

   if (warn_main > 0 && TREE_CODE (decl) != FUNCTION_DECL
       && MAIN_NAME_P (DECL_NAME (decl)))
     warning_with_decl (decl, "`%s' is usually a function");

   if (initialized)
     /* Is it valid for this decl to have an initializer at all?
	If not, set INITIALIZED to zero, which will indirectly
	tell `finish_decl' to ignore the initializer once it is parsed.  */
     switch (TREE_CODE (decl))
       {
       case TYPE_DECL:
	 /* typedef foo = bar  means give foo the same type as bar.
	    We haven't parsed bar yet, so `finish_decl' will fix that up.
	    Any other case of an initialization in a TYPE_DECL is an error.  */
	 if (pedantic || list_length (declspecs) > 1)
	   {
	     error ("typedef `%s' is initialized",
		    IDENTIFIER_POINTER (DECL_NAME (decl)));
	     initialized = 0;
	   }
	 break;

       case FUNCTION_DECL:
	 error ("function `%s' is initialized like a variable",
		IDENTIFIER_POINTER (DECL_NAME (decl)));
	 initialized = 0;
	 break;

       case PARM_DECL:
	 /* DECL_INITIAL in a PARM_DECL is really DECL_ARG_TYPE.  */
	 error ("parameter `%s' is initialized",
		IDENTIFIER_POINTER (DECL_NAME (decl)));
	 initialized = 0;
	 break;

       default:
	 /* Don't allow initializations for incomplete types
	    except for arrays which might be completed by the initialization.  */

	 /* This can happen if the array size is an undefined macro.  We already
	    gave a warning, so we don't need another one.  */
	 if (TREE_TYPE (decl) == error_mark_node)
	   initialized = 0;
	 else if (COMPLETE_TYPE_P (TREE_TYPE (decl)))
	   {
	     /* A complete type is ok if size is fixed.  */

	     if (TREE_CODE (TYPE_SIZE (TREE_TYPE (decl))) != INTEGER_CST
		 || C_DECL_VARIABLE_SIZE (decl))
	       {
		 error ("variable-sized object may not be initialized");
		 initialized = 0;
	       }
	   }
	 else if (TREE_CODE (TREE_TYPE (decl)) != ARRAY_TYPE)
	   {
	     error ("variable `%s' has initializer but incomplete type",
		    IDENTIFIER_POINTER (DECL_NAME (decl)));
	     initialized = 0;
	   }
	 else if (!COMPLETE_TYPE_P (TREE_TYPE (TREE_TYPE (decl))))
	   {
	     error ("elements of array `%s' have incomplete type",
		    IDENTIFIER_POINTER (DECL_NAME (decl)));
	     initialized = 0;
	   }
       }

   if (initialized)
     {
 #if 0
       /* Seems redundant with grokdeclarator.  */
       if (current_binding_level != global_binding_level
	   && DECL_EXTERNAL (decl)
	   && TREE_CODE (decl) != FUNCTION_DECL)
	 warning ("declaration of `%s' has `extern' and is initialized",
		  IDENTIFIER_POINTER (DECL_NAME (decl)));
 #endif
       DECL_EXTERNAL (decl) = 0;
       if (current_binding_level == global_binding_level)
	 TREE_STATIC (decl) = 1;

       /* Tell `pushdecl' this is an initialized decl
	  even though we don't yet have the initializer expression.
	  Also tell `finish_decl' it may store the real initializer.  */
       DECL_INITIAL (decl) = error_mark_node;
     }

   /* If this is a function declaration, write a record describing it to the
      prototypes file (if requested).  */

   if (TREE_CODE (decl) == FUNCTION_DECL)
     gen_aux_info_record (decl, 0, 0, TYPE_ARG_TYPES (TREE_TYPE (decl)) != 0);

   /* ANSI specifies that a tentative definition which is not merged with
      a non-tentative definition behaves exactly like a definition with an
      initializer equal to zero.  (Section 3.7.2)
      -fno-common gives strict ANSI behavior.  Usually you don't want it.
      This matters only for variables with external linkage.  */
   if (! flag_no_common || ! TREE_PUBLIC (decl))
     DECL_COMMON (decl) = 1;

   /* Set attributes here so if duplicate decl, will have proper attributes.  */
   decl_attributes (&decl, attributes, 0);

   /* If #pragma weak was used, mark the decl weak now.  */
   if (current_binding_level == global_binding_level)
     maybe_apply_pragma_weak (decl);

   if (TREE_CODE (decl) == FUNCTION_DECL
       && DECL_DECLARED_INLINE_P (decl)
       && DECL_UNINLINABLE (decl)
       && lookup_attribute ("noinline", DECL_ATTRIBUTES (decl)))
     warning_with_decl (decl,
			"inline function `%s' given attribute noinline");

   /* Add this decl to the current binding level.
      TEM may equal DECL or it may be a previous decl of the same name.  */
   tem = pushdecl (decl);

   /* For a local variable, define the RTL now.  */
   if (current_binding_level != global_binding_level
       /* But not if this is a duplicate decl
	  and we preserved the rtl from the previous one
	  (which may or may not happen).  */
       && !DECL_RTL_SET_P (tem)
       && !DECL_CONTEXT (tem))
     {
       if (TREE_TYPE (tem) != error_mark_node
	   && COMPLETE_TYPE_P (TREE_TYPE (tem)))
	 expand_decl (tem);
       else if (TREE_CODE (TREE_TYPE (tem)) == ARRAY_TYPE
		&& DECL_INITIAL (tem) != 0)
	 expand_decl (tem);
     }

   return tem;
 }

 /* Finish processing of a declaration;
    install its initial value.
    If the length of an array type is not known before,
    it must be determined now, from the initial value, or it is an error.  */

 void
 finish_decl (decl, init, asmspec_tree)
      tree decl, init;
      tree asmspec_tree;
 {
   tree type = TREE_TYPE (decl);
   int was_incomplete = (DECL_SIZE (decl) == 0);
   const char *asmspec = 0;

   /* If a name was specified, get the string.  */
   if (current_binding_level == global_binding_level)
     asmspec_tree = maybe_apply_renaming_pragma (decl, asmspec_tree);
   if (asmspec_tree)
     asmspec = TREE_STRING_POINTER (asmspec_tree);

   /* If `start_decl' didn't like having an initialization, ignore it now.  */
   if (init != 0 && DECL_INITIAL (decl) == 0)
     init = 0;

   /* Don't crash if parm is initialized.  */
   if (TREE_CODE (decl) == PARM_DECL)
     init = 0;

   if (init)
     {
       if (TREE_CODE (decl) != TYPE_DECL)
	 store_init_value (decl, init);
       else
	 {
	   /* typedef foo = bar; store the type of bar as the type of foo.  */
	   TREE_TYPE (decl) = TREE_TYPE (init);
	   DECL_INITIAL (decl) = init = 0;
	 }
     }

   /* Deduce size of array from initialization, if not already known */
   if (TREE_CODE (type) == ARRAY_TYPE
       && TYPE_DOMAIN (type) == 0
       && TREE_CODE (decl) != TYPE_DECL)
     {
       int do_default
	 = (TREE_STATIC (decl)
	    /* Even if pedantic, an external linkage array
	       may have incomplete type at first.  */
	    ? pedantic && !TREE_PUBLIC (decl)
	    : !DECL_EXTERNAL (decl));
       int failure
	 = complete_array_type (type, DECL_INITIAL (decl), do_default);

       /* Get the completed type made by complete_array_type.  */
       type = TREE_TYPE (decl);

       if (failure == 1)
	 error_with_decl (decl, "initializer fails to determine size of `%s'");

       else if (failure == 2)
	 {
	   if (do_default)
	     error_with_decl (decl, "array size missing in `%s'");
	   /* If a `static' var's size isn't known,
	      make it extern as well as static, so it does not get
	      allocated.
	      If it is not `static', then do not mark extern;
	      finish_incomplete_decl will give it a default size
	      and it will get allocated.  */
	   else if (!pedantic && TREE_STATIC (decl) && ! TREE_PUBLIC (decl))
	     DECL_EXTERNAL (decl) = 1;
	 }

       /* TYPE_MAX_VALUE is always one less than the number of elements
	  in the array, because we start counting at zero.  Therefore,
	  warn only if the value is less than zero.  */
       else if (pedantic && TYPE_DOMAIN (type) != 0
	       && tree_int_cst_sgn (TYPE_MAX_VALUE (TYPE_DOMAIN (type))) < 0)
	 error_with_decl (decl, "zero or negative size array `%s'");

       layout_decl (decl, 0);
     }

   if (TREE_CODE (decl) == VAR_DECL)
     {
       if (DECL_SIZE (decl) == 0 && TREE_TYPE (decl) != error_mark_node
	   && COMPLETE_TYPE_P (TREE_TYPE (decl)))
	 layout_decl (decl, 0);

       if (DECL_SIZE (decl) == 0
	   /* Don't give an error if we already gave one earlier.  */
	   && TREE_TYPE (decl) != error_mark_node
	   && (TREE_STATIC (decl)
	       ?
		 /* A static variable with an incomplete type
		    is an error if it is initialized.
		    Also if it is not file scope.
		    Otherwise, let it through, but if it is not `extern'
		    then it may cause an error message later.  */
		 (DECL_INITIAL (decl) != 0
		  || DECL_CONTEXT (decl) != 0)
	       :
		 /* An automatic variable with an incomplete type
		    is an error.  */
		 !DECL_EXTERNAL (decl)))
	 {
	   error_with_decl (decl, "storage size of `%s' isn't known");
	   TREE_TYPE (decl) = error_mark_node;
	 }

       if ((DECL_EXTERNAL (decl) || TREE_STATIC (decl))
	   && DECL_SIZE (decl) != 0)
	 {
	   if (TREE_CODE (DECL_SIZE (decl)) == INTEGER_CST)
	     constant_expression_warning (DECL_SIZE (decl));
	   else
	     error_with_decl (decl, "storage size of `%s' isn't constant");
	 }

       if (TREE_USED (type))
	 TREE_USED (decl) = 1;
     }

   /* If this is a function and an assembler name is specified, it isn't
      builtin any more.  Also reset DECL_RTL so we can give it its new
      name.  */
   if (TREE_CODE (decl) == FUNCTION_DECL && asmspec)
     {
       DECL_BUILT_IN_CLASS (decl) = NOT_BUILT_IN;
       SET_DECL_RTL (decl, NULL_RTX);
       SET_DECL_ASSEMBLER_NAME (decl, get_identifier (asmspec));
     }

   /* Output the assembler code and/or RTL code for variables and functions,
      unless the type is an undefined structure or union.
      If not, it will get done when the type is completed.  */

   if (TREE_CODE (decl) == VAR_DECL || TREE_CODE (decl) == FUNCTION_DECL)
     {
       /* This is a no-op in c-lang.c or something real in objc-actions.c.  */
       maybe_objc_check_decl (decl);

       if (!DECL_CONTEXT (decl))
	 {
	   if (DECL_INITIAL (decl) == NULL_TREE
	       || DECL_INITIAL (decl) == error_mark_node)
	     /* Don't output anything
		when a tentative file-scope definition is seen.
		But at end of compilation, do output code for them.  */
	     DECL_DEFER_OUTPUT (decl) = 1;
	   rest_of_decl_compilation (decl, asmspec,
				     (DECL_CONTEXT (decl) == 0
				      || TREE_ASM_WRITTEN (decl)), 0);
	 }
       else
	 {
	   /* This is a local variable.  If there is an ASMSPEC, the
	      user has requested that we handle it specially.  */
	   if (asmspec)
	     {
	       /* In conjunction with an ASMSPEC, the `register'
		  keyword indicates that we should place the variable
		  in a particular register.  */
	       if (DECL_REGISTER (decl))
		 DECL_C_HARD_REGISTER (decl) = 1;

	       /* If this is not a static variable, issue a warning.
		  It doesn't make any sense to give an ASMSPEC for an
		  ordinary, non-register local variable.  Historically,
		  GCC has accepted -- but ignored -- the ASMSPEC in
		  this case.  */
	       if (TREE_CODE (decl) == VAR_DECL 
		   && !DECL_REGISTER (decl)
		   && !TREE_STATIC (decl))
		 warning_with_decl (decl,
				    "ignoring asm-specifier for non-static local variable `%s'");
	       else
		 SET_DECL_ASSEMBLER_NAME (decl, get_identifier (asmspec));
	     }

	   if (TREE_CODE (decl) != FUNCTION_DECL)
	     add_decl_stmt (decl);
	 }

       if (DECL_CONTEXT (decl) != 0)
	 {
	   /* Recompute the RTL of a local array now
	      if it used to be an incomplete type.  */
	   if (was_incomplete
	       && ! TREE_STATIC (decl) && ! DECL_EXTERNAL (decl))
	     {
	       /* If we used it already as memory, it must stay in memory.  */
	       TREE_ADDRESSABLE (decl) = TREE_USED (decl);
	       /* If it's still incomplete now, no init will save it.  */
	       if (DECL_SIZE (decl) == 0)
		 DECL_INITIAL (decl) = 0;
	     }
	 }
     }

   if (TREE_CODE (decl) == TYPE_DECL)
     {
       /* This is a no-op in c-lang.c or something real in objc-actions.c.  */
       maybe_objc_check_decl (decl);
       rest_of_decl_compilation (decl, NULL, DECL_CONTEXT (decl) == 0, 0);
     }

   /* At the end of a declaration, throw away any variable type sizes
      of types defined inside that declaration.  There is no use
      computing them in the following function definition.  */
   if (current_binding_level == global_binding_level)
     get_pending_sizes ();
 }

 /* If DECL has a cleanup, build and return that cleanup here.
    This is a callback called by expand_expr.  */

 tree
 maybe_build_cleanup (decl)
      tree decl ATTRIBUTE_UNUSED;
 {
   /* There are no cleanups in C.  */
   return NULL_TREE;
 }

 /* Given a parsed parameter declaration,
    decode it into a PARM_DECL and push that on the current binding level.
    Also, for the sake of forward parm decls,
    record the given order of parms in `parm_order'.  */

 void
 push_parm_decl (parm)
      tree parm;
 {
   tree decl;
   int old_immediate_size_expand = immediate_size_expand;
   /* Don't try computing parm sizes now -- wait till fn is called.  */
   immediate_size_expand = 0;

   decl = grokdeclarator (TREE_VALUE (TREE_PURPOSE (parm)),
			  TREE_PURPOSE (TREE_PURPOSE (parm)), PARM, 0);
   decl_attributes (&decl, TREE_VALUE (parm), 0);

 #if 0
   if (DECL_NAME (decl))
     {
       tree olddecl;
       olddecl = lookup_name (DECL_NAME (decl));
       if (pedantic && olddecl != 0 && TREE_CODE (olddecl) == TYPE_DECL)
	 pedwarn_with_decl (decl,
			    "ISO C forbids parameter `%s' shadowing typedef");
     }
 #endif

   decl = pushdecl (decl);

   immediate_size_expand = old_immediate_size_expand;

   current_binding_level->parm_order
     = tree_cons (NULL_TREE, decl, current_binding_level->parm_order);

   /* Add this decl to the current binding level.  */
   finish_decl (decl, NULL_TREE, NULL_TREE);
 }

 /* Clear the given order of parms in `parm_order'.
    Used at start of parm list,
    and also at semicolon terminating forward decls.  */

 void
 clear_parm_order ()
 {
   current_binding_level->parm_order = NULL_TREE;
 }
 
 /* Build a COMPOUND_LITERAL_EXPR.  TYPE is the type given in the compound
    literal, which may be an incomplete array type completed by the
    initializer; INIT is a CONSTRUCTOR that initializes the compound
    literal.  */

 tree
 build_compound_literal (type, init)
      tree type;
      tree init;
 {
   /* We do not use start_decl here because we have a type, not a declarator;
      and do not use finish_decl because the decl should be stored inside
      the COMPOUND_LITERAL_EXPR rather than added elsewhere as a DECL_STMT.  */
   tree decl = build_decl (VAR_DECL, NULL_TREE, type);
   tree complit;
   tree stmt;
   DECL_EXTERNAL (decl) = 0;
   TREE_PUBLIC (decl) = 0;
   TREE_STATIC (decl) = (current_binding_level == global_binding_level);
   DECL_CONTEXT (decl) = current_function_decl;
   TREE_USED (decl) = 1;
   TREE_TYPE (decl) = type;
   store_init_value (decl, init);

   if (TREE_CODE (type) == ARRAY_TYPE && !COMPLETE_TYPE_P (type))
     {
       int failure = complete_array_type (type, DECL_INITIAL (decl), 1);
       if (failure)
	 abort ();
     }

   type = TREE_TYPE (decl);
   if (type == error_mark_node || !COMPLETE_TYPE_P (type))
     return error_mark_node;

   stmt = build_stmt (DECL_STMT, decl);
   complit = build1 (COMPOUND_LITERAL_EXPR, TREE_TYPE (decl), stmt);
   TREE_SIDE_EFFECTS (complit) = 1;

   layout_decl (decl, 0);

   if (TREE_STATIC (decl))
     {
       /* This decl needs a name for the assembler output.  We also need
	  a unique suffix to be added to the name, for which DECL_CONTEXT
	  must be set.  */
       DECL_NAME (decl) = get_identifier ("__compound_literal");
       DECL_CONTEXT (decl) = complit;
       rest_of_decl_compilation (decl, NULL, 1, 0);
       DECL_CONTEXT (decl) = NULL_TREE;
     }

   return complit;
 }
 
 /* Make TYPE a complete type based on INITIAL_VALUE.
    Return 0 if successful, 1 if INITIAL_VALUE can't be deciphered,
    2 if there was no information (in which case assume 1 if DO_DEFAULT).  */

 int
 complete_array_type (type, initial_value, do_default)
      tree type;
      tree initial_value;
      int do_default;
 {
   tree maxindex = NULL_TREE;
   int value = 0;

   if (initial_value)
     {
       /* Note MAXINDEX  is really the maximum index,
	  one less than the size.  */
       if (TREE_CODE (initial_value) == STRING_CST)
	 {
	   int eltsize
	     = int_size_in_bytes (TREE_TYPE (TREE_TYPE (initial_value)));
	   maxindex = build_int_2 ((TREE_STRING_LENGTH (initial_value)
				    / eltsize) - 1, 0);
	 }
       else if (TREE_CODE (initial_value) == CONSTRUCTOR)
	 {
	   tree elts = CONSTRUCTOR_ELTS (initial_value);
	   maxindex = build_int_2 (-1, -1);
	   for (; elts; elts = TREE_CHAIN (elts))
	     {
	       if (TREE_PURPOSE (elts))
		 maxindex = TREE_PURPOSE (elts);
	       else
		 maxindex = fold (build (PLUS_EXPR, integer_type_node,
					 maxindex, integer_one_node));
	     }
	   maxindex = copy_node (maxindex);
	 }
       else
	 {
	   /* Make an error message unless that happened already.  */
	   if (initial_value != error_mark_node)
	     value = 1;

	   /* Prevent further error messages.  */
	   maxindex = build_int_2 (0, 0);
	 }
     }

   if (!maxindex)
     {
       if (do_default)
	 maxindex = build_int_2 (0, 0);
       value = 2;
     }

   if (maxindex)
     {
       TYPE_DOMAIN (type) = build_index_type (maxindex);
       if (!TREE_TYPE (maxindex))
	 TREE_TYPE (maxindex) = TYPE_DOMAIN (type);
     }

   /* Lay out the type now that we can get the real answer.  */

   layout_type (type);

   return value;
 }
 
 /* Given declspecs and a declarator,
    determine the name and type of the object declared
    and construct a ..._DECL node for it.
    (In one case we can return a ..._TYPE node instead.
     For invalid input we sometimes return 0.)

    DECLSPECS is a chain of tree_list nodes whose value fields
     are the storage classes and type specifiers.

    DECL_CONTEXT says which syntactic context this declaration is in:
      NORMAL for most contexts.  Make a VAR_DECL or FUNCTION_DECL or TYPE_DECL.
      FUNCDEF for a function definition.  Like NORMAL but a few different
       error messages in each case.  Return value may be zero meaning
       this definition is too screwy to try to parse.
      PARM for a parameter declaration (either within a function prototype
       or before a function body).  Make a PARM_DECL, or return void_type_node.
      TYPENAME if for a typename (in a cast or sizeof).
       Don't make a DECL node; just return the ..._TYPE node.
      FIELD for a struct or union field; make a FIELD_DECL.
      BITFIELD for a field with specified width.
    INITIALIZED is 1 if the decl has an initializer.

    In the TYPENAME case, DECLARATOR is really an absolute declarator.
    It may also be so in the PARM case, for a prototype where the
    argument type is specified but not the name.

    This function is where the complicated C meanings of `static'
    and `extern' are interpreted.  */

 static tree
 grokdeclarator (declarator, declspecs, decl_context, initialized)
      tree declspecs;
      tree declarator;
      enum decl_context decl_context;
      int initialized;
 {
   int specbits = 0;
   tree spec;
   tree type = NULL_TREE;
   int longlong = 0;
   int constp;
   int restrictp;
   int volatilep;
   int type_quals = TYPE_UNQUALIFIED;
   int inlinep;
   int explicit_int = 0;
   int explicit_char = 0;
   int defaulted_int = 0;
   tree typedef_decl = 0;
   const char *name;
   tree typedef_type = 0;
   int funcdef_flag = 0;
   enum tree_code innermost_code = ERROR_MARK;
   int bitfield = 0;
   int size_varies = 0;
   tree decl_attr = NULL_TREE;
   tree array_ptr_quals = NULL_TREE;
   int array_parm_static = 0;
   tree returned_attrs = NULL_TREE;

   if (decl_context == BITFIELD)
     bitfield = 1, decl_context = FIELD;

   if (decl_context == FUNCDEF)
     funcdef_flag = 1, decl_context = NORMAL;

   /* Look inside a declarator for the name being declared
      and get it as a string, for an error message.  */
   {
     tree decl = declarator;
     name = 0;

     while (decl)
       switch (TREE_CODE (decl))
	 {
	 case ARRAY_REF:
	 case INDIRECT_REF:
	 case CALL_EXPR:
	   innermost_code = TREE_CODE (decl);
	   decl = TREE_OPERAND (decl, 0);
	   break;

	 case TREE_LIST:
	   decl = TREE_VALUE (decl);
	   break;

	 case STRING_CST:
	   name = TREE_STRING_POINTER (decl);
	   decl = 0;
	   break;

	 case IDENTIFIER_NODE:
	   name = IDENTIFIER_POINTER (decl);
	   decl = 0;
	   break;

	 default:
	   abort ();
	 }
     if (name == 0)
       name = "type name";
   }

   /* A function definition's declarator must have the form of
      a function declarator.  */

   if (funcdef_flag && innermost_code != CALL_EXPR)
     return 0;

   /* Anything declared one level down from the top level
      must be one of the parameters of a function
      (because the body is at least two levels down).  */

   /* If this looks like a function definition, make it one,
      even if it occurs where parms are expected.
      Then store_parm_decls will reject it and not use it as a parm.  */
   if (decl_context == NORMAL && !funcdef_flag
       && current_binding_level->parm_flag)
     decl_context = PARM;

   /* Look through the decl specs and record which ones appear.
      Some typespecs are defined as built-in typenames.
      Others, the ones that are modifiers of other types,
      are represented by bits in SPECBITS: set the bits for
      the modifiers that appear.  Storage class keywords are also in SPECBITS.

      If there is a typedef name or a type, store the type in TYPE.
      This includes builtin typedefs such as `int'.

      Set EXPLICIT_INT or EXPLICIT_CHAR if the type is `int' or `char'
      and did not come from a user typedef.

      Set LONGLONG if `long' is mentioned twice.  */

   for (spec = declspecs; spec; spec = TREE_CHAIN (spec))
     {
       tree id = TREE_VALUE (spec);

       /* If the entire declaration is itself tagged as deprecated then
	  suppress reports of deprecated items.  */
       if (id && TREE_DEPRECATED (id))
	 {
	   if (deprecated_state != DEPRECATED_SUPPRESS)
	     warn_deprecated_use (id);
	 }

       if (id == ridpointers[(int) RID_INT])
	 explicit_int = 1;
       if (id == ridpointers[(int) RID_CHAR])
	 explicit_char = 1;

       if (TREE_CODE (id) == IDENTIFIER_NODE && C_IS_RESERVED_WORD (id))
	 {
	   enum rid i = C_RID_CODE (id);
	   if ((int) i <= (int) RID_LAST_MODIFIER)
	     {
	       if (i == RID_LONG && (specbits & (1 << (int) i)))
		 {
		   if (longlong)
		     error ("`long long long' is too long for GCC");
		   else
		     {
		       if (pedantic && !flag_isoc99 && ! in_system_header
			   && warn_long_long)
			 pedwarn ("ISO C89 does not support `long long'");
		       longlong = 1;
		     }
		 }
	       else if (specbits & (1 << (int) i))
		 pedwarn ("duplicate `%s'", IDENTIFIER_POINTER (id));
	       specbits |= 1 << (int) i;
	       goto found;
	     }
	 }
       if (type)
	 error ("two or more data types in declaration of `%s'", name);
       /* Actual typedefs come to us as TYPE_DECL nodes.  */
       else if (TREE_CODE (id) == TYPE_DECL)
	 {
	   if (TREE_TYPE (id) == error_mark_node)
	     ; /* Allow the type to default to int to avoid cascading errors.  */
	   else
	     {
	       type = TREE_TYPE (id);
	       decl_attr = DECL_ATTRIBUTES (id);
	       typedef_decl = id;
	     }
	 }
       /* Built-in types come as identifiers.  */
       else if (TREE_CODE (id) == IDENTIFIER_NODE)
	 {
	   tree t = lookup_name (id);
	   if (TREE_TYPE (t) == error_mark_node)
	     ;
	   else if (!t || TREE_CODE (t) != TYPE_DECL)
	     error ("`%s' fails to be a typedef or built in type",
		    IDENTIFIER_POINTER (id));
	   else
	     {
	       type = TREE_TYPE (t);
	       typedef_decl = t;
	     }
	 }
       else if (TREE_CODE (id) != ERROR_MARK)
	 type = id;

     found:
       ;
     }

   typedef_type = type;
   if (type)
     size_varies = C_TYPE_VARIABLE_SIZE (type);

   /* No type at all: default to `int', and set DEFAULTED_INT
      because it was not a user-defined typedef.  */

   if (type == 0)
     {
       if ((! (specbits & ((1 << (int) RID_LONG) | (1 << (int) RID_SHORT)
			   | (1 << (int) RID_SIGNED)
			   | (1 << (int) RID_UNSIGNED)
			   | (1 << (int) RID_COMPLEX))))
	   /* Don't warn about typedef foo = bar.  */
	   && ! (specbits & (1 << (int) RID_TYPEDEF) && initialized)
	   && ! in_system_header)
	 {
	   /* Issue a warning if this is an ISO C 99 program or if -Wreturn-type
	      and this is a function, or if -Wimplicit; prefer the former
	      warning since it is more explicit.  */
	   if ((warn_implicit_int || warn_return_type || flag_isoc99)
	       && funcdef_flag)
	     warn_about_return_type = 1;
	   else if (warn_implicit_int || flag_isoc99)
	     pedwarn_c99 ("type defaults to `int' in declaration of `%s'",
			  name);
	 }

       defaulted_int = 1;
       type = integer_type_node;
     }

   /* Now process the modifiers that were specified
      and check for invalid combinations.  */

   /* Long double is a special combination.  */

   if ((specbits & 1 << (int) RID_LONG) && ! longlong
       && TYPE_MAIN_VARIANT (type) == double_type_node)
     {
       specbits &= ~(1 << (int) RID_LONG);
       type = long_double_type_node;
     }

   /* Check all other uses of type modifiers.  */

   if (specbits & ((1 << (int) RID_LONG) | (1 << (int) RID_SHORT)
		   | (1 << (int) RID_UNSIGNED) | (1 << (int) RID_SIGNED)))
     {
       int ok = 0;

       if ((specbits & 1 << (int) RID_LONG)
	   && (specbits & 1 << (int) RID_SHORT))
	 error ("both long and short specified for `%s'", name);
       else if (((specbits & 1 << (int) RID_LONG)
		 || (specbits & 1 << (int) RID_SHORT))
		&& explicit_char)
	 error ("long or short specified with char for `%s'", name);
       else if (((specbits & 1 << (int) RID_LONG)
		 || (specbits & 1 << (int) RID_SHORT))
		&& TREE_CODE (type) == REAL_TYPE)
	 {
	   static int already = 0;

	   error ("long or short specified with floating type for `%s'", name);
	   if (! already && ! pedantic)
	     {
	       error ("the only valid combination is `long double'");
	       already = 1;
	     }
	 }
       else if ((specbits & 1 << (int) RID_SIGNED)
		&& (specbits & 1 << (int) RID_UNSIGNED))
	 error ("both signed and unsigned specified for `%s'", name);
       else if (TREE_CODE (type) != INTEGER_TYPE)
	 error ("long, short, signed or unsigned invalid for `%s'", name);
       else
	 {
	   ok = 1;
	   if (!explicit_int && !defaulted_int && !explicit_char && pedantic)
	     {
	       pedwarn ("long, short, signed or unsigned used invalidly for `%s'",
			name);
	       if (flag_pedantic_errors)
		 ok = 0;
	     }
	 }

       /* Discard the type modifiers if they are invalid.  */
       if (! ok)
	 {
	   specbits &= ~((1 << (int) RID_LONG) | (1 << (int) RID_SHORT)
			 | (1 << (int) RID_UNSIGNED) | (1 << (int) RID_SIGNED));
	   longlong = 0;
	 }
     }

   if ((specbits & (1 << (int) RID_COMPLEX))
       && TREE_CODE (type) != INTEGER_TYPE && TREE_CODE (type) != REAL_TYPE)
     {
       error ("complex invalid for `%s'", name);
       specbits &= ~(1 << (int) RID_COMPLEX);
     }

   /* Decide whether an integer type is signed or not.
      Optionally treat bitfields as signed by default.  */
   if (specbits & 1 << (int) RID_UNSIGNED
       /* Traditionally, all bitfields are unsigned.  */
       || (bitfield && flag_traditional
	   && (! explicit_flag_signed_bitfields || !flag_signed_bitfields))
       || (bitfield && ! flag_signed_bitfields
	   && (explicit_int || defaulted_int || explicit_char
	       /* A typedef for plain `int' without `signed'
		  can be controlled just like plain `int'.  */
	       || ! (typedef_decl != 0
		     && C_TYPEDEF_EXPLICITLY_SIGNED (typedef_decl)))
	   && TREE_CODE (type) != ENUMERAL_TYPE
	   && !(specbits & 1 << (int) RID_SIGNED)))
     {
       if (longlong)
	 type = long_long_unsigned_type_node;
       else if (specbits & 1 << (int) RID_LONG)
	 type = long_unsigned_type_node;
       else if (specbits & 1 << (int) RID_SHORT)
	 type = short_unsigned_type_node;
       else if (type == char_type_node)
	 type = unsigned_char_type_node;
       else if (typedef_decl)
	 type = unsigned_type (type);
       else
	 type = unsigned_type_node;
     }
   else if ((specbits & 1 << (int) RID_SIGNED)
	    && type == char_type_node)
     type = signed_char_type_node;
   else if (longlong)
     type = long_long_integer_type_node;
   else if (specbits & 1 << (int) RID_LONG)
     type = long_integer_type_node;
   else if (specbits & 1 << (int) RID_SHORT)
     type = short_integer_type_node;

   if (specbits & 1 << (int) RID_COMPLEX)
     {
       if (pedantic && !flag_isoc99)
	 pedwarn ("ISO C89 does not support complex types");
       /* If we just have "complex", it is equivalent to
	  "complex double", but if any modifiers at all are specified it is
	  the complex form of TYPE.  E.g, "complex short" is
	  "complex short int".  */

       if (defaulted_int && ! longlong
	   && ! (specbits & ((1 << (int) RID_LONG) | (1 << (int) RID_SHORT)
			     | (1 << (int) RID_SIGNED)
			     | (1 << (int) RID_UNSIGNED))))
	 {
	   if (pedantic)
	     pedwarn ("ISO C does not support plain `complex' meaning `double complex'");
	   type = complex_double_type_node;
	 }
       else if (type == integer_type_node)
	 {
	   if (pedantic)
	     pedwarn ("ISO C does not support complex integer types");
	   type = complex_integer_type_node;
	 }
       else if (type == float_type_node)
	 type = complex_float_type_node;
       else if (type == double_type_node)
	 type = complex_double_type_node;
       else if (type == long_double_type_node)
	 type = complex_long_double_type_node;
       else
	 {
	   if (pedantic)
	     pedwarn ("ISO C does not support complex integer types");
	   type = build_complex_type (type);
	 }
     }

   /* Figure out the type qualifiers for the declaration.  There are
      two ways a declaration can become qualified.  One is something
      like `const int i' where the `const' is explicit.  Another is
      something like `typedef const int CI; CI i' where the type of the
      declaration contains the `const'.  */
   constp = !! (specbits & 1 << (int) RID_CONST) + TYPE_READONLY (type);
   restrictp = !! (specbits & 1 << (int) RID_RESTRICT) + TYPE_RESTRICT (type);
   volatilep = !! (specbits & 1 << (int) RID_VOLATILE) + TYPE_VOLATILE (type);
   inlinep = !! (specbits & (1 << (int) RID_INLINE));
   if (constp > 1 && ! flag_isoc99)
     pedwarn ("duplicate `const'");
   if (restrictp > 1 && ! flag_isoc99)
     pedwarn ("duplicate `restrict'");
   if (volatilep > 1 && ! flag_isoc99)
     pedwarn ("duplicate `volatile'");
   if (! flag_gen_aux_info && (TYPE_QUALS (type)))
     type = TYPE_MAIN_VARIANT (type);
   type_quals = ((constp ? TYPE_QUAL_CONST : 0)
		 | (restrictp ? TYPE_QUAL_RESTRICT : 0)
		 | (volatilep ? TYPE_QUAL_VOLATILE : 0));

   /* Warn if two storage classes are given. Default to `auto'.  */

   {
     int nclasses = 0;

     if (specbits & 1 << (int) RID_AUTO) nclasses++;
     if (specbits & 1 << (int) RID_STATIC) nclasses++;
     if (specbits & 1 << (int) RID_EXTERN) nclasses++;
     if (specbits & 1 << (int) RID_REGISTER) nclasses++;
     if (specbits & 1 << (int) RID_TYPEDEF) nclasses++;

     /* Warn about storage classes that are invalid for certain
	kinds of declarations (parameters, typenames, etc.).  */

     if (nclasses > 1)
       error ("multiple storage classes in declaration of `%s'", name);
     else if (funcdef_flag
	      && (specbits
		  & ((1 << (int) RID_REGISTER)
		     | (1 << (int) RID_AUTO)
		     | (1 << (int) RID_TYPEDEF))))
       {
	 if (specbits & 1 << (int) RID_AUTO
	     && (pedantic || current_binding_level == global_binding_level))
	   pedwarn ("function definition declared `auto'");
	 if (specbits & 1 << (int) RID_REGISTER)
	   error ("function definition declared `register'");
	 if (specbits & 1 << (int) RID_TYPEDEF)
	   error ("function definition declared `typedef'");
	 specbits &= ~((1 << (int) RID_TYPEDEF) | (1 << (int) RID_REGISTER)
		       | (1 << (int) RID_AUTO));
       }
     else if (decl_context != NORMAL && nclasses > 0)
       {
	 if (decl_context == PARM && specbits & 1 << (int) RID_REGISTER)
	   ;
	 else
	   {
	     switch (decl_context)
	       {
	       case FIELD:
		 error ("storage class specified for structure field `%s'",
			name);
		 break;
	       case PARM:
		 error ("storage class specified for parameter `%s'", name);
		 break;
	       default:
		 error ("storage class specified for typename");
		 break;
	       }
	     specbits &= ~((1 << (int) RID_TYPEDEF) | (1 << (int) RID_REGISTER)
			   | (1 << (int) RID_AUTO) | (1 << (int) RID_STATIC)
			   | (1 << (int) RID_EXTERN));
	   }
       }
     else if (specbits & 1 << (int) RID_EXTERN && initialized && ! funcdef_flag)
       {
	 /* `extern' with initialization is invalid if not at top level.  */
	 if (current_binding_level == global_binding_level)
	   warning ("`%s' initialized and declared `extern'", name);
	 else
	   error ("`%s' has both `extern' and initializer", name);
       }
     else if (specbits & 1 << (int) RID_EXTERN && funcdef_flag
	      && current_binding_level != global_binding_level)
       error ("nested function `%s' declared `extern'", name);
     else if (current_binding_level == global_binding_level
	      && specbits & (1 << (int) RID_AUTO))
       error ("top-level declaration of `%s' specifies `auto'", name);
   }

   /* Now figure out the structure of the declarator proper.
      Descend through it, creating more complex types, until we reach
      the declared identifier (or NULL_TREE, in an absolute declarator).  */

   while (declarator && TREE_CODE (declarator) != IDENTIFIER_NODE)
     {
       if (type == error_mark_node)
	 {
	   declarator = TREE_OPERAND (declarator, 0);
	   continue;
	 }

       /* Each level of DECLARATOR is either an ARRAY_REF (for ...[..]),
	  an INDIRECT_REF (for *...),
	  a CALL_EXPR (for ...(...)),
	  a TREE_LIST (for nested attributes),
	  an identifier (for the name being declared)
	  or a null pointer (for the place in an absolute declarator
	  where the name was omitted).
	  For the last two cases, we have just exited the loop.

	  At this point, TYPE is the type of elements of an array,
	  or for a function to return, or for a pointer to point to.
	  After this sequence of ifs, TYPE is the type of the
	  array or function or pointer, and DECLARATOR has had its
	  outermost layer removed.  */

       if (array_ptr_quals != NULL_TREE || array_parm_static)
	 {
	   /* Only the innermost declarator (making a parameter be of
	      array type which is converted to pointer type)
	      may have static or type qualifiers.  */
	   error ("static or type qualifiers in non-parameter array declarator");
	   array_ptr_quals = NULL_TREE;
	   array_parm_static = 0;
	 }

       if (TREE_CODE (declarator) == TREE_LIST)
	 {
	   /* We encode a declarator with embedded attributes using
	      a TREE_LIST.  */
	   tree attrs = TREE_PURPOSE (declarator);
	   tree inner_decl;
	   int attr_flags = 0;
	   declarator = TREE_VALUE (declarator);
	   inner_decl = declarator;
	   while (inner_decl != NULL_TREE
		  && TREE_CODE (inner_decl) == TREE_LIST)
	     inner_decl = TREE_VALUE (inner_decl);
	   if (inner_decl == NULL_TREE
	       || TREE_CODE (inner_decl) == IDENTIFIER_NODE)
	     attr_flags |= (int) ATTR_FLAG_DECL_NEXT;
	   else if (TREE_CODE (inner_decl) == CALL_EXPR)
	     attr_flags |= (int) ATTR_FLAG_FUNCTION_NEXT;
	   else if (TREE_CODE (inner_decl) == ARRAY_REF)
	     attr_flags |= (int) ATTR_FLAG_ARRAY_NEXT;
	   returned_attrs = decl_attributes (&type,
					     chainon (returned_attrs, attrs),
					     attr_flags);
	 }
       else if (TREE_CODE (declarator) == ARRAY_REF)
	 {
	   tree itype = NULL_TREE;
	   tree size = TREE_OPERAND (declarator, 1);
	   /* The index is a signed object `sizetype' bits wide.  */
	   tree index_type = signed_type (sizetype);

	   array_ptr_quals = TREE_TYPE (declarator);
	   array_parm_static = TREE_STATIC (declarator);

	   declarator = TREE_OPERAND (declarator, 0);

	   /* Check for some types that there cannot be arrays of.  */

	   if (VOID_TYPE_P (type))
	     {
	       error ("declaration of `%s' as array of voids", name);
	       type = error_mark_node;
	     }

	   if (TREE_CODE (type) == FUNCTION_TYPE)
	     {
	       error ("declaration of `%s' as array of functions", name);
	       type = error_mark_node;
	     }

	   if (size == error_mark_node)
	     type = error_mark_node;

	   if (type == error_mark_node)
	     continue;

	   /* If size was specified, set ITYPE to a range-type for that size.
	      Otherwise, ITYPE remains null.  finish_decl may figure it out
	      from an initial value.  */

	   if (size)
	     {
	       /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
	       STRIP_TYPE_NOPS (size);

	       if (! INTEGRAL_TYPE_P (TREE_TYPE (size)))
		 {
		   error ("size of array `%s' has non-integer type", name);
		   size = integer_one_node;
		 }

	       if (pedantic && integer_zerop (size))
		 pedwarn ("ISO C forbids zero-size array `%s'", name);

	       if (TREE_CODE (size) == INTEGER_CST)
		 {
		   constant_expression_warning (size);
		   if (tree_int_cst_sgn (size) < 0)
		     {
		       error ("size of array `%s' is negative", name);
		       size = integer_one_node;
		     }
		 }
	       else
		 {
		   /* Make sure the array size remains visibly nonconstant
		      even if it is (eg) a const variable with known value.  */
		   size_varies = 1;

		   if (pedantic)
		     {
		       if (TREE_CONSTANT (size))
			 pedwarn ("ISO C89 forbids array `%s' whose size can't be evaluated",
				  name);
		       else
			 pedwarn ("ISO C89 forbids variable-size array `%s'",
				  name);
		     }
		 }

	       if (integer_zerop (size))
		 {
		   /* A zero-length array cannot be represented with an
		      unsigned index type, which is what we'll get with
		      build_index_type.  Create an open-ended range instead.  */
		   itype = build_range_type (sizetype, size, NULL_TREE);
		 }
	       else
		 {
		   /* Compute the maximum valid index, that is, size - 1.
		      Do the calculation in index_type, so that if it is
		      a variable the computations will be done in the
		      proper mode.  */
		   itype = fold (build (MINUS_EXPR, index_type,
					convert (index_type, size),
					convert (index_type, size_one_node)));

		   /* If that overflowed, the array is too big.
		      ??? While a size of INT_MAX+1 technically shouldn't
		      cause an overflow (because we subtract 1), the overflow
		      is recorded during the conversion to index_type, before
		      the subtraction.  Handling this case seems like an
		      unnecessary complication.  */
		   if (TREE_OVERFLOW (itype))
		     {
		       error ("size of array `%s' is too large", name);
		       type = error_mark_node;
		       continue;
		     }

		   if (size_varies)
		     itype = variable_size (itype);
		   itype = build_index_type (itype);
		 }
	     }
	   else if (decl_context == FIELD)
	     {
	       if (pedantic && !flag_isoc99 && !in_system_header)
		 pedwarn ("ISO C89 does not support flexible array members");

	       /* ISO C99 Flexible array members are effectively identical
		  to GCC's zero-length array extension.  */
	       itype = build_range_type (sizetype, size_zero_node, NULL_TREE);
	     }

	   /* If pedantic, complain about arrays of incomplete types.  */

	   if (pedantic && !COMPLETE_TYPE_P (type))
	     pedwarn ("array type has incomplete element type");

 #if 0
	   /* We shouldn't have a function type here at all!
	      Functions aren't allowed as array elements.  */
	   if (pedantic && TREE_CODE (type) == FUNCTION_TYPE
	       && (constp || volatilep))
	     pedwarn ("ISO C forbids const or volatile function types");
 #endif

	   /* Build the array type itself, then merge any constancy or
	      volatility into the target type.  We must do it in this order
	      to ensure that the TYPE_MAIN_VARIANT field of the array type
	      is set correctly.  */

	   type = build_array_type (type, itype);
	   if (type_quals)
	     type = c_build_qualified_type (type, type_quals);

	   if (size_varies)
	     C_TYPE_VARIABLE_SIZE (type) = 1;

	   /* The GCC extension for zero-length arrays differs from
	      ISO flexible array members in that sizeof yields zero.  */
	   if (size && integer_zerop (size))
	     {
	       layout_type (type);
	       TYPE_SIZE (type) = bitsize_zero_node;
	       TYPE_SIZE_UNIT (type) = size_zero_node;
	     }
	   if (decl_context != PARM
	       && (array_ptr_quals != NULL_TREE || array_parm_static))
	     {
	       error ("static or type qualifiers in non-parameter array declarator");
	       array_ptr_quals = NULL_TREE;
	       array_parm_static = 0;
	     }
	 }
       else if (TREE_CODE (declarator) == CALL_EXPR)
	 {
	   tree arg_types;

	   /* Declaring a function type.
	      Make sure we have a valid type for the function to return.  */
	   if (type == error_mark_node)
	     continue;

	   size_varies = 0;

	   /* Warn about some types functions can't return.  */

	   if (TREE_CODE (type) == FUNCTION_TYPE)
	     {
	       error ("`%s' declared as function returning a function", name);
	       type = integer_type_node;
	     }
	   if (TREE_CODE (type) == ARRAY_TYPE)
	     {
	       error ("`%s' declared as function returning an array", name);
	       type = integer_type_node;
	     }

 #ifndef TRADITIONAL_RETURN_FLOAT
	   /* Traditionally, declaring return type float means double.  */

	   if (flag_traditional && TYPE_MAIN_VARIANT (type) == float_type_node)
	     type = double_type_node;
 #endif /* TRADITIONAL_RETURN_FLOAT */

	   /* Construct the function type and go to the next
	      inner layer of declarator.  */

	   arg_types = grokparms (TREE_OPERAND (declarator, 1),
				  funcdef_flag
				  /* Say it's a definition
				     only for the CALL_EXPR
				     closest to the identifier.  */
				  && TREE_CODE (TREE_OPERAND (declarator, 0)) == IDENTIFIER_NODE);
	   /* Type qualifiers before the return type of the function
	      qualify the return type, not the function type.  */
	   if (type_quals)
	     {
	       /* Type qualifiers on a function return type are normally
		  permitted by the standard but have no effect, so give a
		  warning at -W.  Qualifiers on a void return type have
		  meaning as a GNU extension, and are banned on function
		  definitions in ISO C.  FIXME: strictly we shouldn't
		  pedwarn for qualified void return types except on function
		  definitions, but not doing so could lead to the undesirable
		  state of a "volatile void" function return type not being
		  warned about, and a use of the function being compiled
		  with GNU semantics, with no diagnostics under -pedantic.  */
	       if (VOID_TYPE_P (type) && pedantic && !in_system_header)
		 pedwarn ("ISO C forbids qualified void function return type");
	       else if (extra_warnings
			&& !(VOID_TYPE_P (type)
			     && type_quals == TYPE_QUAL_VOLATILE))
		 warning ("type qualifiers ignored on function return type");

	       type = c_build_qualified_type (type, type_quals);
	     }
	   type_quals = TYPE_UNQUALIFIED;

	   type = build_function_type (type, arg_types);
	   declarator = TREE_OPERAND (declarator, 0);

	   /* Set the TYPE_CONTEXTs for each tagged type which is local to
	      the formal parameter list of this FUNCTION_TYPE to point to
	      the FUNCTION_TYPE node itself.  */

	   {
	     tree link;

	     for (link = last_function_parm_tags;
		  link;
		  link = TREE_CHAIN (link))
	       TYPE_CONTEXT (TREE_VALUE (link)) = type;
	   }
	 }
       else if (TREE_CODE (declarator) == INDIRECT_REF)
	 {
	   /* Merge any constancy or volatility into the target type
	      for the pointer.  */

	   if (pedantic && TREE_CODE (type) == FUNCTION_TYPE
	       && type_quals)
	     pedwarn ("ISO C forbids qualified function types");
	   if (type_quals)
	     type = c_build_qualified_type (type, type_quals);
	   type_quals = TYPE_UNQUALIFIED;
	   size_varies = 0;

	   type = build_pointer_type (type);

	   /* Process a list of type modifier keywords
	      (such as const or volatile) that were given inside the `*'.  */

	   if (TREE_TYPE (declarator))
	     {
	       tree typemodlist;
	       int erred = 0;

	       constp = 0;
	       volatilep = 0;
	       restrictp = 0;
	       for (typemodlist = TREE_TYPE (declarator); typemodlist;
		    typemodlist = TREE_CHAIN (typemodlist))
		 {
		   tree qualifier = TREE_VALUE (typemodlist);

		   if (C_IS_RESERVED_WORD (qualifier))
		     {
		       if (C_RID_CODE (qualifier) == RID_CONST)
			 constp++;
		       else if (C_RID_CODE (qualifier) == RID_VOLATILE)
			 volatilep++;
		       else if (C_RID_CODE (qualifier) == RID_RESTRICT)
			 restrictp++;
		       else
			 erred++;
		     }
		   else
		     erred++;
		 }

	       if (erred)
		 error ("invalid type modifier within pointer declarator");
	       if (constp > 1 && ! flag_isoc99)
		 pedwarn ("duplicate `const'");
	       if (volatilep > 1 && ! flag_isoc99)
		 pedwarn ("duplicate `volatile'");
	       if (restrictp > 1 && ! flag_isoc99)
		 pedwarn ("duplicate `restrict'");

	       type_quals = ((constp ? TYPE_QUAL_CONST : 0)
			     | (restrictp ? TYPE_QUAL_RESTRICT : 0)
			     | (volatilep ? TYPE_QUAL_VOLATILE : 0));
	     }

	   declarator = TREE_OPERAND (declarator, 0);
	 }
       else
	 abort ();

     }

   /* Now TYPE has the actual type.  */

   /* Did array size calculations overflow?  */

   if (TREE_CODE (type) == ARRAY_TYPE
       && COMPLETE_TYPE_P (type)
       && TREE_OVERFLOW (TYPE_SIZE (type)))
     {
       error ("size of array `%s' is too large", name);
       /* If we proceed with the array type as it is, we'll eventually
	  crash in tree_low_cst().  */
       type = error_mark_node;
     }

   /* If this is declaring a typedef name, return a TYPE_DECL.  */

   if (specbits & (1 << (int) RID_TYPEDEF))
     {
       tree decl;
       /* Note that the grammar rejects storage classes
	  in typenames, fields or parameters */
       if (pedantic && TREE_CODE (type) == FUNCTION_TYPE
	   && type_quals)
	 pedwarn ("ISO C forbids qualified function types");
       if (type_quals)
	 type = c_build_qualified_type (type, type_quals);
       decl = build_decl (TYPE_DECL, declarator, type);
       if ((specbits & (1 << (int) RID_SIGNED))
	   || (typedef_decl && C_TYPEDEF_EXPLICITLY_SIGNED (typedef_decl)))
	 C_TYPEDEF_EXPLICITLY_SIGNED (decl) = 1;
       decl_attributes (&decl, returned_attrs, 0);
       return decl;
     }

   /* Detect the case of an array type of unspecified size
      which came, as such, direct from a typedef name.
      We must copy the type, so that each identifier gets
      a distinct type, so that each identifier's size can be
      controlled separately by its own initializer.  */

   if (type != 0 && typedef_type != 0
       && TREE_CODE (type) == ARRAY_TYPE && TYPE_DOMAIN (type) == 0
       && TYPE_MAIN_VARIANT (type) == TYPE_MAIN_VARIANT (typedef_type))
     {
       type = build_array_type (TREE_TYPE (type), 0);
       if (size_varies)
	 C_TYPE_VARIABLE_SIZE (type) = 1;
     }

   /* If this is a type name (such as, in a cast or sizeof),
      compute the type and return it now.  */

   if (decl_context == TYPENAME)
     {
       /* Note that the grammar rejects storage classes
	  in typenames, fields or parameters */
       if (pedantic && TREE_CODE (type) == FUNCTION_TYPE
	   && type_quals)
	 pedwarn ("ISO C forbids const or volatile function types");
       if (type_quals)
	 type = c_build_qualified_type (type, type_quals);
       decl_attributes (&type, returned_attrs, 0);
       return type;
     }

   /* Aside from typedefs and type names (handle above),
      `void' at top level (not within pointer)
      is allowed only in public variables.
      We don't complain about parms either, but that is because
      a better error message can be made later.  */

   if (VOID_TYPE_P (type) && decl_context != PARM
       && ! ((decl_context != FIELD && TREE_CODE (type) != FUNCTION_TYPE)
	     && ((specbits & (1 << (int) RID_EXTERN))
		 || (current_binding_level == global_binding_level
		     && !(specbits
			  & ((1 << (int) RID_STATIC) | (1 << (int) RID_REGISTER)))))))
     {
       error ("variable or field `%s' declared void", name);
       type = integer_type_node;
     }

   /* Now create the decl, which may be a VAR_DECL, a PARM_DECL
      or a FUNCTION_DECL, depending on DECL_CONTEXT and TYPE.  */

   {
     tree decl;

     if (decl_context == PARM)
       {
	 tree type_as_written;
	 tree promoted_type;

	 /* A parameter declared as an array of T is really a pointer to T.
	    One declared as a function is really a pointer to a function.  */

	 if (TREE_CODE (type) == ARRAY_TYPE)
	   {
	     /* Transfer const-ness of array into that of type pointed to.  */
	     type = TREE_TYPE (type);
	     if (type_quals)
	       type = c_build_qualified_type (type, type_quals);
	     type = build_pointer_type (type);
	     type_quals = TYPE_UNQUALIFIED;
	     if (array_ptr_quals)
	       {
		 tree new_ptr_quals, new_ptr_attrs;
		 int erred = 0;
		 split_specs_attrs (array_ptr_quals, &new_ptr_quals, &new_ptr_attrs);
		 /* We don't yet implement attributes in this context.  */
		 if (new_ptr_attrs != NULL_TREE)
		   warning ("attributes in parameter array declarator ignored");

		 constp = 0;
		 volatilep = 0;
		 restrictp = 0;
		 for (; new_ptr_quals; new_ptr_quals = TREE_CHAIN (new_ptr_quals))
		   {
		     tree qualifier = TREE_VALUE (new_ptr_quals);

		     if (C_IS_RESERVED_WORD (qualifier))
		       {
			 if (C_RID_CODE (qualifier) == RID_CONST)
			   constp++;
			 else if (C_RID_CODE (qualifier) == RID_VOLATILE)
			   volatilep++;
			 else if (C_RID_CODE (qualifier) == RID_RESTRICT)
			   restrictp++;
			 else
			   erred++;
		       }
		     else
		       erred++;
		   }

		 if (erred)
		   error ("invalid type modifier within array declarator");

		 type_quals = ((constp ? TYPE_QUAL_CONST : 0)
			       | (restrictp ? TYPE_QUAL_RESTRICT : 0)
			       | (volatilep ? TYPE_QUAL_VOLATILE : 0));
	       }
	     size_varies = 0;
	   }
	 else if (TREE_CODE (type) == FUNCTION_TYPE)
	   {
	     if (pedantic && type_quals)
	       pedwarn ("ISO C forbids qualified function types");
	     if (type_quals)
	       type = c_build_qualified_type (type, type_quals);
	     type = build_pointer_type (type);
	     type_quals = TYPE_UNQUALIFIED;
	   }
	 else if (type_quals)
	   type = c_build_qualified_type (type, type_quals);

	 type_as_written = type;

	 decl = build_decl (PARM_DECL, declarator, type);
	 if (size_varies)
	   C_DECL_VARIABLE_SIZE (decl) = 1;

	 /* Compute the type actually passed in the parmlist,
	    for the case where there is no prototype.
	    (For example, shorts and chars are passed as ints.)
	    When there is a prototype, this is overridden later.  */

	 if (type == error_mark_node)
	   promoted_type = type;
	 else
	   {
	     promoted_type = simple_type_promotes_to (type);
	     if (! promoted_type)
	       promoted_type = type;
	   }

	 DECL_ARG_TYPE (decl) = promoted_type;
	 DECL_ARG_TYPE_AS_WRITTEN (decl) = type_as_written;
       }
     else if (decl_context == FIELD)
       {
	 /* Structure field.  It may not be a function.  */

	 if (TREE_CODE (type) == FUNCTION_TYPE)
	   {
	     error ("field `%s' declared as a function", name);
	     type = build_pointer_type (type);
	   }
	 else if (TREE_CODE (type) != ERROR_MARK
		  && !COMPLETE_OR_UNBOUND_ARRAY_TYPE_P (type))
	   {
	     error ("field `%s' has incomplete type", name);
	     type = error_mark_node;
	   }
	 /* Move type qualifiers down to element of an array.  */
	 if (TREE_CODE (type) == ARRAY_TYPE && type_quals)
	   {
	     type = build_array_type (c_build_qualified_type (TREE_TYPE (type),
							      type_quals),
				      TYPE_DOMAIN (type));
 #if 0
	     /* Leave the field const or volatile as well.  */
	     type_quals = TYPE_UNQUALIFIED;
 #endif
	   }
	 decl = build_decl (FIELD_DECL, declarator, type);
	 DECL_NONADDRESSABLE_P (decl) = bitfield;

	 if (size_varies)
	   C_DECL_VARIABLE_SIZE (decl) = 1;
       }
     else if (TREE_CODE (type) == FUNCTION_TYPE)
       {
	 /* Every function declaration is "external"
	    except for those which are inside a function body
	    in which `auto' is used.
	    That is a case not specified by ANSI C,
	    and we use it for forward declarations for nested functions.  */
	 int extern_ref = (!(specbits & (1 << (int) RID_AUTO))
			   || current_binding_level == global_binding_level);

	 if (specbits & (1 << (int) RID_AUTO)
	     && (pedantic || current_binding_level == global_binding_level))
	   pedwarn ("invalid storage class for function `%s'", name);
	 if (specbits & (1 << (int) RID_REGISTER))
	   error ("invalid storage class for function `%s'", name);
	 /* Function declaration not at top level.
	    Storage classes other than `extern' are not allowed
	    and `extern' makes no difference.  */
	 if (current_binding_level != global_binding_level
	     && (specbits & ((1 << (int) RID_STATIC) | (1 << (int) RID_INLINE)))
	     && pedantic)
	   pedwarn ("invalid storage class for function `%s'", name);

	 decl = build_decl (FUNCTION_DECL, declarator, type);
	 decl = build_decl_attribute_variant (decl, decl_attr);

	 DECL_LANG_SPECIFIC (decl) = (struct lang_decl *)
	   ggc_alloc_cleared (sizeof (struct lang_decl));

	 if (pedantic && type_quals && ! DECL_IN_SYSTEM_HEADER (decl))
	   pedwarn ("ISO C forbids qualified function types");

	 /* GNU C interprets a `volatile void' return type to indicate
	    that the function does not return.  */
	 if ((type_quals & TYPE_QUAL_VOLATILE)
	     && !VOID_TYPE_P (TREE_TYPE (TREE_TYPE (decl))))
	   warning ("`noreturn' function returns non-void value");

	 if (extern_ref)
	   DECL_EXTERNAL (decl) = 1;
	 /* Record absence of global scope for `static' or `auto'.  */
	 TREE_PUBLIC (decl)
	   = !(specbits & ((1 << (int) RID_STATIC) | (1 << (int) RID_AUTO)));

	 if (defaulted_int)
	   C_FUNCTION_IMPLICIT_INT (decl) = 1;

	 /* Record presence of `inline', if it is reasonable.  */
	 if (MAIN_NAME_P (declarator))
	   {
	     if (inlinep)
	       warning ("cannot inline function `main'");
	   }
	 else if (inlinep)
	   {
	     /* Assume that otherwise the function can be inlined.  */
	     DECL_DECLARED_INLINE_P (decl) = 1;

	     /* Do not mark bare declarations as DECL_INLINE.  Doing so
		in the presence of multiple declarations can result in
		the abstract origin pointing between the declarations,
		which will confuse dwarf2out.  */
	     if (initialized)
	       {
		 DECL_INLINE (decl) = 1;
		 if (specbits & (1 << (int) RID_EXTERN))
		   current_extern_inline = 1;
	       }
	   }
	 /* If -finline-functions, assume it can be inlined.  This does
	    two things: let the function be deferred until it is actually
	    needed, and let dwarf2 know that the function is inlinable.  */
	 else if (flag_inline_trees == 2 && initialized)
	   {
	     DECL_INLINE (decl) = 1;
	     DECL_DECLARED_INLINE_P (decl) = 0;
	   }
       }
     else
       {
	 /* It's a variable.  */
	 /* An uninitialized decl with `extern' is a reference.  */
	 int extern_ref = !initialized && (specbits & (1 << (int) RID_EXTERN));

	 /* Move type qualifiers down to element of an array.  */
	 if (TREE_CODE (type) == ARRAY_TYPE && type_quals)
	   {
	     int saved_align = TYPE_ALIGN(type);
	     type = build_array_type (c_build_qualified_type (TREE_TYPE (type),
							      type_quals),
				      TYPE_DOMAIN (type));
	     TYPE_ALIGN (type) = saved_align;
 #if 0 /* Leave the variable const or volatile as well.  */
	     type_quals = TYPE_UNQUALIFIED;
 #endif
	   }
	 else if (type_quals)
	   type = c_build_qualified_type (type, type_quals);

	 decl = build_decl (VAR_DECL, declarator, type);
	 if (size_varies)
	   C_DECL_VARIABLE_SIZE (decl) = 1;

	 if (inlinep)
	   pedwarn_with_decl (decl, "variable `%s' declared `inline'");

	 DECL_EXTERNAL (decl) = extern_ref;
	 /* At top level, the presence of a `static' or `register' storage
	    class specifier, or the absence of all storage class specifiers
	    makes this declaration a definition (perhaps tentative).  Also,
	    the absence of both `static' and `register' makes it public.  */
	 if (current_binding_level == global_binding_level)
	   {
	     TREE_PUBLIC (decl)
	       = !(specbits
		   & ((1 << (int) RID_STATIC) | (1 << (int) RID_REGISTER)));
	     TREE_STATIC (decl) = ! DECL_EXTERNAL (decl);
	   }
	 /* Not at top level, only `static' makes a static definition.  */
	 else
	   {
	     TREE_STATIC (decl) = (specbits & (1 << (int) RID_STATIC)) != 0;
	     TREE_PUBLIC (decl) = DECL_EXTERNAL (decl);
	   }
       }

     /* Record `register' declaration for warnings on &
	and in case doing stupid register allocation.  */

     if (specbits & (1 << (int) RID_REGISTER))
       DECL_REGISTER (decl) = 1;

     /* Record constancy and volatility.  */
     c_apply_type_quals_to_decl (type_quals, decl);

     /* If a type has volatile components, it should be stored in memory.
	Otherwise, the fact that those components are volatile
	will be ignored, and would even crash the compiler.  */
     if (C_TYPE_FIELDS_VOLATILE (TREE_TYPE (decl)))
       mark_addressable (decl);

     decl_attributes (&decl, returned_attrs, 0);

     return decl;
   }
 }
 
 /* Decode the parameter-list info for a function type or function definition.
    The argument is the value returned by `get_parm_info' (or made in parse.y
    if there is an identifier list instead of a parameter decl list).
    These two functions are separate because when a function returns
    or receives functions then each is called multiple times but the order
    of calls is different.  The last call to `grokparms' is always the one
    that contains the formal parameter names of a function definition.

    Store in `last_function_parms' a chain of the decls of parms.
    Also store in `last_function_parm_tags' a chain of the struct, union,
    and enum tags declared among the parms.

    Return a list of arg types to use in the FUNCTION_TYPE for this function.

    FUNCDEF_FLAG is nonzero for a function definition, 0 for
    a mere declaration.  A nonempty identifier-list gets an error message
    when FUNCDEF_FLAG is zero.  */

 static tree
 grokparms (parms_info, funcdef_flag)
      tree parms_info;
      int funcdef_flag;
 {
   tree first_parm = TREE_CHAIN (parms_info);

   last_function_parms = TREE_PURPOSE (parms_info);
   last_function_parm_tags = TREE_VALUE (parms_info);

   if (warn_strict_prototypes && first_parm == 0 && !funcdef_flag
       && !in_system_header)
     warning ("function declaration isn't a prototype");

   if (first_parm != 0
       && TREE_CODE (TREE_VALUE (first_parm)) == IDENTIFIER_NODE)
     {
       if (! funcdef_flag)
	 pedwarn ("parameter names (without types) in function declaration");

       last_function_parms = first_parm;
       return 0;
     }
   else
     {
       tree parm;
       tree typelt;
       /* We no longer test FUNCDEF_FLAG.
	  If the arg types are incomplete in a declaration,
	  they must include undefined tags.
	  These tags can never be defined in the scope of the declaration,
	  so the types can never be completed,
	  and no call can be compiled successfully.  */
 #if 0
       /* In a fcn definition, arg types must be complete.  */
       if (funcdef_flag)
 #endif
	 for (parm = last_function_parms, typelt = first_parm;
	      parm;
	      parm = TREE_CHAIN (parm))
	   /* Skip over any enumeration constants declared here.  */
	   if (TREE_CODE (parm) == PARM_DECL)
	     {
	       /* Barf if the parameter itself has an incomplete type.  */
	       tree type = TREE_VALUE (typelt);
	       if (type == error_mark_node)
		 continue;
	       if (!COMPLETE_TYPE_P (type))
		 {
		   if (funcdef_flag && DECL_NAME (parm) != 0)
		     error ("parameter `%s' has incomplete type",
			    IDENTIFIER_POINTER (DECL_NAME (parm)));
		   else
		     warning ("parameter has incomplete type");
		   if (funcdef_flag)
		     {
		       TREE_VALUE (typelt) = error_mark_node;
		       TREE_TYPE (parm) = error_mark_node;
		     }
		 }
 #if 0
	       /* This has been replaced by parm_tags_warning, which
		  uses a more accurate criterion for what to warn
		  about.  */
	       else
		 {
		   /* Now warn if is a pointer to an incomplete type.  */
		   while (TREE_CODE (type) == POINTER_TYPE
			  || TREE_CODE (type) == REFERENCE_TYPE)
		     type = TREE_TYPE (type);
		   type = TYPE_MAIN_VARIANT (type);
		   if (!COMPLETE_TYPE_P (type))
		     {
		       if (DECL_NAME (parm) != 0)
			 warning ("parameter `%s' points to incomplete type",
				  IDENTIFIER_POINTER (DECL_NAME (parm)));
		       else
			 warning ("parameter points to incomplete type");
		     }
		 }
 #endif
	       typelt = TREE_CHAIN (typelt);
	     }

       return first_parm;
     }
 }

 /* Return a tree_list node with info on a parameter list just parsed.
    The TREE_PURPOSE is a chain of decls of those parms.
    The TREE_VALUE is a list of structure, union and enum tags defined.
    The TREE_CHAIN is a list of argument types to go in the FUNCTION_TYPE.
    This tree_list node is later fed to `grokparms'.

    VOID_AT_END nonzero means append `void' to the end of the type-list.
    Zero means the parmlist ended with an ellipsis so don't append `void'.  */

 tree
 get_parm_info (void_at_end)
      int void_at_end;
 {
   tree decl, t;
   tree types = 0;
   int erred = 0;
   tree tags = gettags ();
   tree parms = getdecls ();
   tree new_parms = 0;
   tree order = current_binding_level->parm_order;

   /* Just `void' (and no ellipsis) is special.  There are really no parms.
      But if the `void' is qualified (by `const' or `volatile') or has a
      storage class specifier (`register'), then the behavior is undefined;
      by not counting it as the special case of `void' we will cause an
      error later.  Typedefs for `void' are OK (see DR#157).  */
   if (void_at_end && parms != 0
       && TREE_CHAIN (parms) == 0
       && VOID_TYPE_P (TREE_TYPE (parms))
       && ! TREE_THIS_VOLATILE (parms)
       && ! TREE_READONLY (parms)
       && ! DECL_REGISTER (parms)
       && DECL_NAME (parms) == 0)
     {
       parms = NULL_TREE;
       storedecls (NULL_TREE);
       return tree_cons (NULL_TREE, NULL_TREE,
			 tree_cons (NULL_TREE, void_type_node, NULL_TREE));
     }

   /* Extract enumerator values and other non-parms declared with the parms.
      Likewise any forward parm decls that didn't have real parm decls.  */
   for (decl = parms; decl;)
     {
       tree next = TREE_CHAIN (decl);

       if (TREE_CODE (decl) != PARM_DECL)
	 {
	   TREE_CHAIN (decl) = new_parms;
	   new_parms = decl;
	 }
       else if (TREE_ASM_WRITTEN (decl))
	 {
	   error_with_decl (decl,
			    "parameter `%s' has just a forward declaration");
	   TREE_CHAIN (decl) = new_parms;
	   new_parms = decl;
	 }
       decl = next;
     }

   /* Put the parm decls back in the order they were in in the parm list.  */
   for (t = order; t; t = TREE_CHAIN (t))
     {
       if (TREE_CHAIN (t))
	 TREE_CHAIN (TREE_VALUE (t)) = TREE_VALUE (TREE_CHAIN (t));
       else
	 TREE_CHAIN (TREE_VALUE (t)) = 0;
     }

   new_parms = chainon (order ? nreverse (TREE_VALUE (order)) : 0,
			new_parms);

   /* Store the parmlist in the binding level since the old one
      is no longer a valid list.  (We have changed the chain pointers.)  */
   storedecls (new_parms);

   for (decl = new_parms; decl; decl = TREE_CHAIN (decl))
     /* There may also be declarations for enumerators if an enumeration
	type is declared among the parms.  Ignore them here.  */
     if (TREE_CODE (decl) == PARM_DECL)
       {
	 /* Since there is a prototype,
	    args are passed in their declared types.  */
	 tree type = TREE_TYPE (decl);
	 DECL_ARG_TYPE (decl) = type;
	 if (PROMOTE_PROTOTYPES
	     && INTEGRAL_TYPE_P (type)
	     && TYPE_PRECISION (type) < TYPE_PRECISION (integer_type_node))
	   DECL_ARG_TYPE (decl) = integer_type_node;

	 types = tree_cons (NULL_TREE, TREE_TYPE (decl), types);
	 if (VOID_TYPE_P (TREE_VALUE (types)) && ! erred
	     && DECL_NAME (decl) == 0)
	   {
	     error ("`void' in parameter list must be the entire list");
	     erred = 1;
	   }
       }

   if (void_at_end)
     return tree_cons (new_parms, tags,
		       nreverse (tree_cons (NULL_TREE, void_type_node, types)));

   return tree_cons (new_parms, tags, nreverse (types));
 }

 /* At end of parameter list, warn about any struct, union or enum tags
    defined within.  Do so because these types cannot ever become complete.  */

 void
 parmlist_tags_warning ()
 {
   tree elt;
   static int already;

   for (elt = current_binding_level->tags; elt; elt = TREE_CHAIN (elt))
     {
       enum tree_code code = TREE_CODE (TREE_VALUE (elt));
       /* An anonymous union parm type is meaningful as a GNU extension.
	  So don't warn for that.  */
       if (code == UNION_TYPE && TREE_PURPOSE (elt) == 0 && !pedantic)
	 continue;
       if (TREE_PURPOSE (elt) != 0)
	 {
	   if (code == RECORD_TYPE)
	     warning ("`struct %s' declared inside parameter list",
		      IDENTIFIER_POINTER (TREE_PURPOSE (elt)));
	   else if (code == UNION_TYPE)
	     warning ("`union %s' declared inside parameter list",
		      IDENTIFIER_POINTER (TREE_PURPOSE (elt)));
	   else
	     warning ("`enum %s' declared inside parameter list",
		      IDENTIFIER_POINTER (TREE_PURPOSE (elt)));
	 }
       else
	 {
	   /* For translation these need to be separate warnings */
	   if (code == RECORD_TYPE)
	     warning ("anonymous struct declared inside parameter list");
	   else if (code == UNION_TYPE)
	     warning ("anonymous union declared inside parameter list");
	   else
	     warning ("anonymous enum declared inside parameter list");
	 }
       if (! already)
	 {
	   warning ("its scope is only this definition or declaration, which is probably not what you want");
	   already = 1;
	 }
     }
 }
 
 /* Get the struct, enum or union (CODE says which) with tag NAME.
    Define the tag as a forward-reference if it is not defined.  */

 tree
 xref_tag (code, name)
      enum tree_code code;
      tree name;
 {
   /* If a cross reference is requested, look up the type
      already defined for this tag and return it.  */

   tree ref = lookup_tag (code, name, current_binding_level, 0);
   /* If this is the right type of tag, return what we found.
      (This reference will be shadowed by shadow_tag later if appropriate.)
      If this is the wrong type of tag, do not return it.  If it was the
      wrong type in the same binding level, we will have had an error
      message already; if in a different binding level and declaring
      a name, pending_xref_error will give an error message; but if in a
      different binding level and not declaring a name, this tag should
      shadow the previous declaration of a different type of tag, and
      this would not work properly if we return the reference found.
      (For example, with "struct foo" in an outer scope, "union foo;"
      must shadow that tag with a new one of union type.)  */
   if (ref && TREE_CODE (ref) == code)
     return ref;

   /* If no such tag is yet defined, create a forward-reference node
      and record it as the "definition".
      When a real declaration of this type is found,
      the forward-reference will be altered into a real type.  */

   ref = make_node (code);
   if (code == ENUMERAL_TYPE)
     {
       /* Give the type a default layout like unsigned int
	  to avoid crashing if it does not get defined.  */
       TYPE_MODE (ref) = TYPE_MODE (unsigned_type_node);
       TYPE_ALIGN (ref) = TYPE_ALIGN (unsigned_type_node);
       TYPE_USER_ALIGN (ref) = 0;
       TREE_UNSIGNED (ref) = 1;
       TYPE_PRECISION (ref) = TYPE_PRECISION (unsigned_type_node);
       TYPE_MIN_VALUE (ref) = TYPE_MIN_VALUE (unsigned_type_node);
       TYPE_MAX_VALUE (ref) = TYPE_MAX_VALUE (unsigned_type_node);
     }

   pushtag (name, ref);

   return ref;
 }
 
 /* Make sure that the tag NAME is defined *in the current binding level*
    at least as a forward reference.
    CODE says which kind of tag NAME ought to be.  */

 tree
 start_struct (code, name)
      enum tree_code code;
      tree name;
 {
   /* If there is already a tag defined at this binding level
      (as a forward reference), just return it.  */

   tree ref = 0;

   if (name != 0)
     ref = lookup_tag (code, name, current_binding_level, 1);
   if (ref && TREE_CODE (ref) == code)
     {
       C_TYPE_BEING_DEFINED (ref) = 1;
       TYPE_PACKED (ref) = flag_pack_struct;
       if (TYPE_FIELDS (ref))
	 {
	   if (code == UNION_TYPE)
	     error ("redefinition of `union %s'",
		    IDENTIFIER_POINTER (name));
	   else
	     error ("redefinition of `struct %s'",
		    IDENTIFIER_POINTER (name));
	 }  

       return ref;
     }

   /* Otherwise create a forward-reference just so the tag is in scope.  */

   ref = make_node (code);
   pushtag (name, ref);
   C_TYPE_BEING_DEFINED (ref) = 1;
   TYPE_PACKED (ref) = flag_pack_struct;
   return ref;
 }

 /* Process the specs, declarator (NULL if omitted) and width (NULL if omitted)
    of a structure component, returning a FIELD_DECL node.
    WIDTH is non-NULL for bit fields only, and is an INTEGER_CST node.

    This is done during the parsing of the struct declaration.
    The FIELD_DECL nodes are chained together and the lot of them
    are ultimately passed to `build_struct' to make the RECORD_TYPE node.  */

 tree
 grokfield (filename, line, declarator, declspecs, width)
      const char *filename ATTRIBUTE_UNUSED;
      int line ATTRIBUTE_UNUSED;
      tree declarator, declspecs, width;
 {
   tree value;

   if (declarator == NULL_TREE && width == NULL_TREE)
     {
       /* This is an unnamed decl.  We only support unnamed
	  structs/unions, so check for other things and refuse them.  */
       if (TREE_CODE (TREE_VALUE (declspecs)) != RECORD_TYPE
	   && TREE_CODE (TREE_VALUE (declspecs)) != UNION_TYPE)
	 {
	   error ("unnamed fields of type other than struct or union are not allowed");
	   return NULL_TREE;
	 }
     }

   value = grokdeclarator (declarator, declspecs, width ? BITFIELD : FIELD, 0);

   finish_decl (value, NULL_TREE, NULL_TREE);
   DECL_INITIAL (value) = width;

   maybe_objc_check_decl (value);
   return value;
 }
 
 /* Fill in the fields of a RECORD_TYPE or UNION_TYPE node, T.
    FIELDLIST is a chain of FIELD_DECL nodes for the fields.
    ATTRIBUTES are attributes to be applied to the structure.  */

 tree
 finish_struct (t, fieldlist, attributes)
      tree t;
      tree fieldlist;
      tree attributes;
 {
   tree x;
   int toplevel = global_binding_level == current_binding_level;
   int saw_named_field;

   /* If this type was previously laid out as a forward reference,
      make sure we lay it out again.  */

   TYPE_SIZE (t) = 0;

   decl_attributes (&t, attributes, (int) ATTR_FLAG_TYPE_IN_PLACE);

   /* Nameless union parm types are useful as GCC extension.  */
   if (! (TREE_CODE (t) == UNION_TYPE && TYPE_NAME (t) == 0) && !pedantic)
     /* Otherwise, warn about any struct or union def. in parmlist.  */
     if (in_parm_level_p ())
       {
	 if (pedantic)
	   pedwarn ("%s defined inside parms",
		    TREE_CODE (t) == UNION_TYPE ? _("union") : _("structure"));
	 else if (! flag_traditional)
	   warning ("%s defined inside parms",
		    TREE_CODE (t) == UNION_TYPE ? _("union") : _("structure"));
       }

   if (pedantic)
     {
       for (x = fieldlist; x; x = TREE_CHAIN (x))
	 if (DECL_NAME (x) != 0)
	   break;

       if (x == 0)
	 pedwarn ("%s has no %s",
		  TREE_CODE (t) == UNION_TYPE ? _("union") : _("struct"),
		  fieldlist ? _("named members") : _("members"));
     }

   /* Install struct as DECL_CONTEXT of each field decl.
      Also process specified field sizes,m which is found in the DECL_INITIAL.
      Store 0 there, except for ": 0" fields (so we can find them
      and delete them, below).  */

   saw_named_field = 0;
   for (x = fieldlist; x; x = TREE_CHAIN (x))
     {
       DECL_CONTEXT (x) = t;
       DECL_PACKED (x) |= TYPE_PACKED (t);

       /* If any field is const, the structure type is pseudo-const.  */
       if (TREE_READONLY (x))
	 C_TYPE_FIELDS_READONLY (t) = 1;
       else
	 {
	   /* A field that is pseudo-const makes the structure likewise.  */
	   tree t1 = TREE_TYPE (x);
	   while (TREE_CODE (t1) == ARRAY_TYPE)
	     t1 = TREE_TYPE (t1);
	   if ((TREE_CODE (t1) == RECORD_TYPE || TREE_CODE (t1) == UNION_TYPE)
	       && C_TYPE_FIELDS_READONLY (t1))
	     C_TYPE_FIELDS_READONLY (t) = 1;
	 }

       /* Any field that is volatile means variables of this type must be
	  treated in some ways as volatile.  */
       if (TREE_THIS_VOLATILE (x))
	 C_TYPE_FIELDS_VOLATILE (t) = 1;

       /* Any field of nominal variable size implies structure is too.  */
       if (C_DECL_VARIABLE_SIZE (x))
	 C_TYPE_VARIABLE_SIZE (t) = 1;

       /* Detect invalid nested redefinition.  */
       if (TREE_TYPE (x) == t)
	 error ("nested redefinition of `%s'",
		IDENTIFIER_POINTER (TYPE_NAME (t)));

       /* Detect invalid bit-field size.  */
       if (DECL_INITIAL (x))
	 STRIP_NOPS (DECL_INITIAL (x));
       if (DECL_INITIAL (x))
	 {
	   if (TREE_CODE (DECL_INITIAL (x)) == INTEGER_CST)
	     constant_expression_warning (DECL_INITIAL (x));
	   else
	     {
	       error_with_decl (x,
				"bit-field `%s' width not an integer constant");
	       DECL_INITIAL (x) = NULL;
	     }
	 }

       /* Detect invalid bit-field type.  */
       if (DECL_INITIAL (x)
	   && TREE_CODE (TREE_TYPE (x)) != INTEGER_TYPE
	   && TREE_CODE (TREE_TYPE (x)) != BOOLEAN_TYPE
	   && TREE_CODE (TREE_TYPE (x)) != ENUMERAL_TYPE)
	 {
	   error_with_decl (x, "bit-field `%s' has invalid type");
	   DECL_INITIAL (x) = NULL;
	 }

       if (DECL_INITIAL (x) && pedantic
	   && TYPE_MAIN_VARIANT (TREE_TYPE (x)) != integer_type_node
	   && TYPE_MAIN_VARIANT (TREE_TYPE (x)) != unsigned_type_node
	   && TYPE_MAIN_VARIANT (TREE_TYPE (x)) != c_bool_type_node
	   /* Accept an enum that's equivalent to int or unsigned int.  */
	   && !(TREE_CODE (TREE_TYPE (x)) == ENUMERAL_TYPE
		&& (TYPE_PRECISION (TREE_TYPE (x))
		    == TYPE_PRECISION (integer_type_node))))
	 pedwarn_with_decl (x, "bit-field `%s' type invalid in ISO C");

       /* Detect and ignore out of range field width and process valid
	  field widths.  */
       if (DECL_INITIAL (x))
	 {
	   int max_width
	     = (TYPE_MAIN_VARIANT (TREE_TYPE (x)) == c_bool_type_node
		? CHAR_TYPE_SIZE : TYPE_PRECISION (TREE_TYPE (x)));

	   if (tree_int_cst_sgn (DECL_INITIAL (x)) < 0)
	     error_with_decl (x, "negative width in bit-field `%s'");
	   else if (0 < compare_tree_int (DECL_INITIAL (x), max_width))
	     pedwarn_with_decl (x, "width of `%s' exceeds its type");
	   else if (integer_zerop (DECL_INITIAL (x)) && DECL_NAME (x) != 0)
	     error_with_decl (x, "zero width for bit-field `%s'");
	   else
	     {
	       /* The test above has assured us that TREE_INT_CST_HIGH is 0.  */
	       unsigned HOST_WIDE_INT width
		 = tree_low_cst (DECL_INITIAL (x), 1);

	       if (TREE_CODE (TREE_TYPE (x)) == ENUMERAL_TYPE
		   && (width < min_precision (TYPE_MIN_VALUE (TREE_TYPE (x)),
					      TREE_UNSIGNED (TREE_TYPE (x)))
		       || (width
			   < min_precision (TYPE_MAX_VALUE (TREE_TYPE (x)),
					    TREE_UNSIGNED (TREE_TYPE (x))))))
		 warning_with_decl (x,
				    "`%s' is narrower than values of its type");

	       DECL_SIZE (x) = bitsize_int (width);
	       DECL_BIT_FIELD (x) = 1;
	       SET_DECL_C_BIT_FIELD (x);

	       if (width == 0
		   && ! (* targetm.ms_bitfield_layout_p) (t))
		 {
		   /* field size 0 => force desired amount of alignment.  */
 #ifdef EMPTY_FIELD_BOUNDARY
		   DECL_ALIGN (x) = MAX (DECL_ALIGN (x), EMPTY_FIELD_BOUNDARY);
 #endif
 #ifdef PCC_BITFIELD_TYPE_MATTERS
		   if (PCC_BITFIELD_TYPE_MATTERS)
		     {
		       DECL_ALIGN (x) = MAX (DECL_ALIGN (x),
					     TYPE_ALIGN (TREE_TYPE (x)));
		       DECL_USER_ALIGN (x) |= TYPE_USER_ALIGN (TREE_TYPE (x));
		     }
 #endif
		 }
	     }
	 }

       else if (TREE_TYPE (x) != error_mark_node)
	 {
	   unsigned int min_align = (DECL_PACKED (x) ? BITS_PER_UNIT
				     : TYPE_ALIGN (TREE_TYPE (x)));

	   /* Non-bit-fields are aligned for their type, except packed
	      fields which require only BITS_PER_UNIT alignment.  */
	   DECL_ALIGN (x) = MAX (DECL_ALIGN (x), min_align);
	   if (! DECL_PACKED (x))
	     DECL_USER_ALIGN (x) |= TYPE_USER_ALIGN (TREE_TYPE (x));
	 }

       DECL_INITIAL (x) = 0;

       /* Detect flexible array member in an invalid context.  */
       if (TREE_CODE (TREE_TYPE (x)) == ARRAY_TYPE
	   && TYPE_SIZE (TREE_TYPE (x)) == NULL_TREE
	   && TYPE_DOMAIN (TREE_TYPE (x)) != NULL_TREE
	   && TYPE_MAX_VALUE (TYPE_DOMAIN (TREE_TYPE (x))) == NULL_TREE)
	 {
	   if (TREE_CODE (t) == UNION_TYPE)
	     error_with_decl (x, "flexible array member in union");
	   else if (TREE_CHAIN (x) != NULL_TREE)
	     error_with_decl (x, "flexible array member not at end of struct");
	   else if (! saw_named_field)
	     error_with_decl (x, "flexible array member in otherwise empty struct");
	 }
       if (DECL_NAME (x))
	 saw_named_field = 1;
     }

   /* Delete all duplicate fields from the fieldlist */
   for (x = fieldlist; x && TREE_CHAIN (x);)
     /* Anonymous fields aren't duplicates.  */
     if (DECL_NAME (TREE_CHAIN (x)) == 0)
       x = TREE_CHAIN (x);
     else
       {
	 tree y = fieldlist;

	 while (1)
	   {
	     if (DECL_NAME (y) == DECL_NAME (TREE_CHAIN (x)))
	       break;
	     if (y == x)
	       break;
	     y = TREE_CHAIN (y);
	   }
	 if (DECL_NAME (y) == DECL_NAME (TREE_CHAIN (x)))
	   {
	     error_with_decl (TREE_CHAIN (x), "duplicate member `%s'");
	     TREE_CHAIN (x) = TREE_CHAIN (TREE_CHAIN (x));
	   }
	 else
	   x = TREE_CHAIN (x);
       }

   /* Now we have the nearly final fieldlist.  Record it,
      then lay out the structure or union (including the fields).  */

   TYPE_FIELDS (t) = fieldlist;

   layout_type (t);

   /* Delete all zero-width bit-fields from the fieldlist */
   {
     tree *fieldlistp = &fieldlist;
     while (*fieldlistp)
       if (TREE_CODE (*fieldlistp) == FIELD_DECL && DECL_INITIAL (*fieldlistp))
	 *fieldlistp = TREE_CHAIN (*fieldlistp);
       else
	 fieldlistp = &TREE_CHAIN (*fieldlistp);
   }

   /* Now we have the truly final field list.
      Store it in this type and in the variants.  */

   TYPE_FIELDS (t) = fieldlist;

   for (x = TYPE_MAIN_VARIANT (t); x; x = TYPE_NEXT_VARIANT (x))
     {
       TYPE_FIELDS (x) = TYPE_FIELDS (t);
       TYPE_LANG_SPECIFIC (x) = TYPE_LANG_SPECIFIC (t);
       TYPE_ALIGN (x) = TYPE_ALIGN (t);
       TYPE_USER_ALIGN (x) = TYPE_USER_ALIGN (t);
     }

   /* If this was supposed to be a transparent union, but we can't
      make it one, warn and turn off the flag.  */
   if (TREE_CODE (t) == UNION_TYPE
       && TYPE_TRANSPARENT_UNION (t)
       && TYPE_MODE (t) != DECL_MODE (TYPE_FIELDS (t)))
     {
       TYPE_TRANSPARENT_UNION (t) = 0;
       warning ("union cannot be made transparent");
     }

   /* If this structure or union completes the type of any previous
      variable declaration, lay it out and output its rtl.  */

   if (current_binding_level->n_incomplete != 0)
     {
       tree decl;
       for (decl = current_binding_level->names; decl; decl = TREE_CHAIN (decl))
	 {
	   if (TYPE_MAIN_VARIANT (TREE_TYPE (decl)) == TYPE_MAIN_VARIANT (t)
	       && TREE_CODE (decl) != TYPE_DECL)
	     {
	       layout_decl (decl, 0);
	       /* This is a no-op in c-lang.c or something real in objc-actions.c.  */
	       maybe_objc_check_decl (decl);
	       rest_of_decl_compilation (decl, NULL, toplevel, 0);
	       if (! toplevel)
		 expand_decl (decl);
	       if (--current_binding_level->n_incomplete == 0)
		 break;
	     }
	   else if (!COMPLETE_TYPE_P (TREE_TYPE (decl))
		    && TREE_CODE (TREE_TYPE (decl)) == ARRAY_TYPE)
	     {
	       tree element = TREE_TYPE (decl);
	       while (TREE_CODE (element) == ARRAY_TYPE)
		 element = TREE_TYPE (element);
	       if (element == t)
		 {
		   layout_array_type (TREE_TYPE (decl));
		   if (TREE_CODE (decl) != TYPE_DECL)
		     {
		       layout_decl (decl, 0);
		       maybe_objc_check_decl (decl);
		       rest_of_decl_compilation (decl, NULL, toplevel, 0);
		       if (! toplevel)
			 expand_decl (decl);
		     }
		   if (--current_binding_level->n_incomplete == 0)
		     break;
		 }
	     }
	 }
     }

   /* Finish debugging output for this type.  */
   rest_of_type_compilation (t, toplevel);

   return t;
 }

 /* Lay out the type T, and its element type, and so on.  */

 static void
 layout_array_type (t)
      tree t;
 {
   if (TREE_CODE (TREE_TYPE (t)) == ARRAY_TYPE)
     layout_array_type (TREE_TYPE (t));
   layout_type (t);
 }
 
 /* Begin compiling the definition of an enumeration type.
    NAME is its name (or null if anonymous).
    Returns the type object, as yet incomplete.
    Also records info about it so that build_enumerator
    may be used to declare the individual values as they are read.  */

 tree
 start_enum (name)
      tree name;
 {
   tree enumtype = 0;

   /* If this is the real definition for a previous forward reference,
      fill in the contents in the same object that used to be the
      forward reference.  */

   if (name != 0)
     enumtype = lookup_tag (ENUMERAL_TYPE, name, current_binding_level, 1);

   if (enumtype == 0 || TREE_CODE (enumtype) != ENUMERAL_TYPE)
     {
       enumtype = make_node (ENUMERAL_TYPE);
       pushtag (name, enumtype);
     }

   C_TYPE_BEING_DEFINED (enumtype) = 1;

   if (TYPE_VALUES (enumtype) != 0)
     {
       /* This enum is a named one that has been declared already.  */
       error ("redeclaration of `enum %s'", IDENTIFIER_POINTER (name));

       /* Completely replace its old definition.
	  The old enumerators remain defined, however.  */
       TYPE_VALUES (enumtype) = 0;
     }

   enum_next_value = integer_zero_node;
   enum_overflow = 0;

   if (flag_short_enums)
     TYPE_PACKED (enumtype) = 1;

   return enumtype;
 }

 /* After processing and defining all the values of an enumeration type,
    install their decls in the enumeration type and finish it off.
    ENUMTYPE is the type object, VALUES a list of decl-value pairs,
    and ATTRIBUTES are the specified attributes.
    Returns ENUMTYPE.  */

 tree
 finish_enum (enumtype, values, attributes)
      tree enumtype;
      tree values;
      tree attributes;
 {
   tree pair, tem;
   tree minnode = 0, maxnode = 0, enum_value_type;
   int precision, unsign;
   int toplevel = (global_binding_level == current_binding_level);

   if (in_parm_level_p ())
     warning ("enum defined inside parms");

   decl_attributes (&enumtype, attributes, (int) ATTR_FLAG_TYPE_IN_PLACE);

   /* Calculate the maximum value of any enumerator in this type.  */

   if (values == error_mark_node)
     minnode = maxnode = integer_zero_node;
   else
     {
       minnode = maxnode = TREE_VALUE (values);
       for (pair = TREE_CHAIN (values); pair; pair = TREE_CHAIN (pair))
	 {
	   tree value = TREE_VALUE (pair);
	   if (tree_int_cst_lt (maxnode, value))
	     maxnode = value;
	   if (tree_int_cst_lt (value, minnode))
	     minnode = value;
	 }
     }

   /* Construct the final type of this enumeration.  It is the same
      as one of the integral types - the narrowest one that fits, except
      that normally we only go as narrow as int - and signed iff any of
      the values are negative.  */
   unsign = (tree_int_cst_sgn (minnode) >= 0);
   precision = MAX (min_precision (minnode, unsign),
		    min_precision (maxnode, unsign));
   if (TYPE_PACKED (enumtype) || precision > TYPE_PRECISION (integer_type_node))
     {
       tree narrowest = type_for_size (precision, unsign);
       if (narrowest == 0)
	 {
	   warning ("enumeration values exceed range of largest integer");
	   narrowest = long_long_integer_type_node;
	 }

       precision = TYPE_PRECISION (narrowest);
     }
   else
     precision = TYPE_PRECISION (integer_type_node);

   if (precision == TYPE_PRECISION (integer_type_node))
     enum_value_type = type_for_size (precision, 0);
   else
     enum_value_type = enumtype;

   TYPE_MIN_VALUE (enumtype) = minnode;
   TYPE_MAX_VALUE (enumtype) = maxnode;
   TYPE_PRECISION (enumtype) = precision;
   TREE_UNSIGNED (enumtype) = unsign;
   TYPE_SIZE (enumtype) = 0;
   layout_type (enumtype);

   if (values != error_mark_node)
     {
       /* Change the type of the enumerators to be the enum type.  We
	  need to do this irrespective of the size of the enum, for
	  proper type checking.  Replace the DECL_INITIALs of the
	  enumerators, and the value slots of the list, with copies
	  that have the enum type; they cannot be modified in place
	  because they may be shared (e.g.  integer_zero_node) Finally,
	  change the purpose slots to point to the names of the decls.  */
       for (pair = values; pair; pair = TREE_CHAIN (pair))
	 {
	   tree enu = TREE_PURPOSE (pair);

	   TREE_TYPE (enu) = enumtype;
	   DECL_SIZE (enu) = TYPE_SIZE (enumtype);
	   DECL_SIZE_UNIT (enu) = TYPE_SIZE_UNIT (enumtype);
	   DECL_ALIGN (enu) = TYPE_ALIGN (enumtype);
	   DECL_USER_ALIGN (enu) = TYPE_USER_ALIGN (enumtype);
	   DECL_MODE (enu) = TYPE_MODE (enumtype);

	   /* The ISO C Standard mandates enumerators to have type int,
	      even though the underlying type of an enum type is
	      unspecified.  Here we convert any enumerators that fit in
	      an int to type int, to avoid promotions to unsigned types
	      when comparing integers with enumerators that fit in the
	      int range.  When -pedantic is given, build_enumerator()
	      would have already taken care of those that don't fit.  */
	   if (int_fits_type_p (DECL_INITIAL (enu), enum_value_type))
	     DECL_INITIAL (enu) = convert (enum_value_type, DECL_INITIAL (enu));
	   else
	     DECL_INITIAL (enu) = convert (enumtype, DECL_INITIAL (enu));

	   TREE_PURPOSE (pair) = DECL_NAME (enu);
	   TREE_VALUE (pair) = DECL_INITIAL (enu);
	 }

       TYPE_VALUES (enumtype) = values;
     }

   /* Fix up all variant types of this enum type.  */
   for (tem = TYPE_MAIN_VARIANT (enumtype); tem; tem = TYPE_NEXT_VARIANT (tem))
     {
       if (tem == enumtype)
	 continue;
       TYPE_VALUES (tem) = TYPE_VALUES (enumtype);
       TYPE_MIN_VALUE (tem) = TYPE_MIN_VALUE (enumtype);
       TYPE_MAX_VALUE (tem) = TYPE_MAX_VALUE (enumtype);
       TYPE_SIZE (tem) = TYPE_SIZE (enumtype);
       TYPE_SIZE_UNIT (tem) = TYPE_SIZE_UNIT (enumtype);
       TYPE_MODE (tem) = TYPE_MODE (enumtype);
       TYPE_PRECISION (tem) = TYPE_PRECISION (enumtype);
       TYPE_ALIGN (tem) = TYPE_ALIGN (enumtype);
       TYPE_USER_ALIGN (tem) = TYPE_USER_ALIGN (enumtype);
       TREE_UNSIGNED (tem) = TREE_UNSIGNED (enumtype);
     }

   /* Finish debugging output for this type.  */
   rest_of_type_compilation (enumtype, toplevel);

   return enumtype;
 }

 /* Build and install a CONST_DECL for one value of the
    current enumeration type (one that was begun with start_enum).
    Return a tree-list containing the CONST_DECL and its value.
    Assignment of sequential values by default is handled here.  */

 tree
 build_enumerator (name, value)
      tree name, value;
 {
   tree decl, type;

   /* Validate and default VALUE.  */

   /* Remove no-op casts from the value.  */
   if (value)
     STRIP_TYPE_NOPS (value);

   if (value != 0)
     {
       if (TREE_CODE (value) == INTEGER_CST)
	 {
	   value = default_conversion (value);
	   constant_expression_warning (value);
	 }
       else
	 {
	   error ("enumerator value for `%s' not integer constant",
		  IDENTIFIER_POINTER (name));
	   value = 0;
	 }
     }

   /* Default based on previous value.  */
   /* It should no longer be possible to have NON_LVALUE_EXPR
      in the default.  */
   if (value == 0)
     {
       value = enum_next_value;
       if (enum_overflow)
	 error ("overflow in enumeration values");
     }

   if (pedantic && ! int_fits_type_p (value, integer_type_node))
     {
       pedwarn ("ISO C restricts enumerator values to range of `int'");
       value = convert (integer_type_node, value);
     }

   /* Set basis for default for next value.  */
   enum_next_value = build_binary_op (PLUS_EXPR, value, integer_one_node, 0);
   enum_overflow = tree_int_cst_lt (enum_next_value, value);

   /* Now create a declaration for the enum value name.  */

   type = TREE_TYPE (value);
   type = type_for_size (MAX (TYPE_PRECISION (type),
			      TYPE_PRECISION (integer_type_node)),
			 ((flag_traditional
			   || TYPE_PRECISION (type) >= TYPE_PRECISION (integer_type_node))
			  && TREE_UNSIGNED (type)));

   decl = build_decl (CONST_DECL, name, type);
   DECL_INITIAL (decl) = convert (type, value);
   pushdecl (decl);

   return tree_cons (decl, value, NULL_TREE);
 }

 
 /* Create the FUNCTION_DECL for a function definition.
    DECLSPECS, DECLARATOR and ATTRIBUTES are the parts of
    the declaration; they describe the function's name and the type it returns,
    but twisted together in a fashion that parallels the syntax of C.

    This function creates a binding context for the function body
    as well as setting up the FUNCTION_DECL in current_function_decl.

    Returns 1 on success.  If the DECLARATOR is not suitable for a function
    (it defines a datum instead), we return 0, which tells
    yyparse to report a parse error.  */

 int
 bli_start_function (declspecs, declarator, attributes)
      tree declarator, declspecs, attributes;
 {
   tree decl1, old_decl;
   tree restype;
   int old_immediate_size_expand = immediate_size_expand;

   current_function_returns_value = 0;  /* Assume, until we see it does.  */
   current_function_returns_null = 0;
   current_function_returns_abnormally = 0;
   warn_about_return_type = 0;
   current_extern_inline = 0;
   c_function_varargs = 0;
   named_labels = 0;
   shadowed_labels = 0;

   /* Don't expand any sizes in the return type of the function.  */
   immediate_size_expand = 0;

   decl1 = grokdeclarator (declarator, declspecs, FUNCDEF, 1);

   /* If the declarator is not suitable for a function definition,
      cause a syntax error.  */
   if (decl1 == 0)
     {
       immediate_size_expand = old_immediate_size_expand;
       return 0;
     }

   decl_attributes (&decl1, attributes, 0);

   /* If #pragma weak was used, mark the decl weak now.  */
   if (current_binding_level == global_binding_level)
     maybe_apply_pragma_weak (decl1);

   if (DECL_DECLARED_INLINE_P (decl1)
       && DECL_UNINLINABLE (decl1)
       && lookup_attribute ("noinline", DECL_ATTRIBUTES (decl1)))
     warning_with_decl (decl1,
			"inline function `%s' given attribute noinline");

   announce_function (decl1);

   if (!COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (TREE_TYPE (decl1))))
     {
       error ("return type is an incomplete type");
       /* Make it return void instead.  */
       TREE_TYPE (decl1)
	 = build_function_type (void_type_node,
				TYPE_ARG_TYPES (TREE_TYPE (decl1)));
     }

   if (warn_about_return_type)
     pedwarn_c99 ("return type defaults to `int'");

   /* Save the parm names or decls from this function's declarator
      where store_parm_decls will find them.  */
   current_function_parms = last_function_parms;
   current_function_parm_tags = last_function_parm_tags;

   /* Make the init_value nonzero so pushdecl knows this is not tentative.
      error_mark_node is replaced below (in poplevel) with the BLOCK.  */
   DECL_INITIAL (decl1) = error_mark_node;

   /* If this definition isn't a prototype and we had a prototype declaration
      before, copy the arg type info from that prototype.
      But not if what we had before was a builtin function.  */
   old_decl = lookup_name_current_level (DECL_NAME (decl1));
   if (old_decl != 0 && TREE_CODE (TREE_TYPE (old_decl)) == FUNCTION_TYPE
       && !DECL_BUILT_IN (old_decl)
       && (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (decl1)))
	   == TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (old_decl))))
       && TYPE_ARG_TYPES (TREE_TYPE (decl1)) == 0)
     {
       TREE_TYPE (decl1) = TREE_TYPE (old_decl);
       current_function_prototype_file = DECL_SOURCE_FILE (old_decl);
       current_function_prototype_line = DECL_SOURCE_LINE (old_decl);
     }

   /* If there is no explicit declaration, look for any out-of-scope implicit
      declarations.  */
   if (old_decl == 0)
     old_decl = IDENTIFIER_IMPLICIT_DECL (DECL_NAME (decl1));

   /* Optionally warn of old-fashioned def with no previous prototype.  */
   if (warn_strict_prototypes
       && TYPE_ARG_TYPES (TREE_TYPE (decl1)) == 0
       && !(old_decl != 0
	    && (TYPE_ARG_TYPES (TREE_TYPE (old_decl)) != 0
		|| (DECL_BUILT_IN (old_decl)
		    && ! C_DECL_ANTICIPATED (old_decl)))))
     warning ("function declaration isn't a prototype");
   /* Optionally warn of any global def with no previous prototype.  */
   else if (warn_missing_prototypes
	    && TREE_PUBLIC (decl1)
	    && !(old_decl != 0
		 && (TYPE_ARG_TYPES (TREE_TYPE (old_decl)) != 0
		     || (DECL_BUILT_IN (old_decl)
			 && ! C_DECL_ANTICIPATED (old_decl))))
	    && ! MAIN_NAME_P (DECL_NAME (decl1)))
     warning_with_decl (decl1, "no previous prototype for `%s'");
   /* Optionally warn of any def with no previous prototype
      if the function has already been used.  */
   else if (warn_missing_prototypes
	    && old_decl != 0 && TREE_USED (old_decl)
	    && TYPE_ARG_TYPES (TREE_TYPE (old_decl)) == 0)
     warning_with_decl (decl1,
			"`%s' was used with no prototype before its definition");
   /* Optionally warn of any global def with no previous declaration.  */
   else if (warn_missing_declarations
	    && TREE_PUBLIC (decl1)
	    && old_decl == 0
	    && ! MAIN_NAME_P (DECL_NAME (decl1)))
     warning_with_decl (decl1, "no previous declaration for `%s'");
   /* Optionally warn of any def with no previous declaration
      if the function has already been used.  */
   else if (warn_missing_declarations
	    && old_decl != 0 && TREE_USED (old_decl)
	    && old_decl == IDENTIFIER_IMPLICIT_DECL (DECL_NAME (decl1)))
     warning_with_decl (decl1,
			"`%s' was used with no declaration before its definition");

   /* This is a definition, not a reference.
      So normally clear DECL_EXTERNAL.
      However, `extern inline' acts like a declaration
      except for defining how to inline.  So set DECL_EXTERNAL in that case.  */
   DECL_EXTERNAL (decl1) = current_extern_inline;

   /* This function exists in static storage.
      (This does not mean `static' in the C sense!)  */
   TREE_STATIC (decl1) = 1;

   /* A nested function is not global.  */
   if (current_function_decl != 0)
     TREE_PUBLIC (decl1) = 0;

   /* Warn for unlikely, improbable, or stupid declarations of `main'.  */
   if (warn_main > 0 && MAIN_NAME_P (DECL_NAME (decl1)))
     {
       tree args;
       int argct = 0;

       if (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (decl1)))
	   != integer_type_node)
	 pedwarn_with_decl (decl1, "return type of `%s' is not `int'");

       for (args = TYPE_ARG_TYPES (TREE_TYPE (decl1)); args;
	    args = TREE_CHAIN (args))
	 {
	   tree type = args ? TREE_VALUE (args) : 0;

	   if (type == void_type_node)
	     break;

	   ++argct;
	   switch (argct)
	     {
	     case 1:
	       if (TYPE_MAIN_VARIANT (type) != integer_type_node)
		 pedwarn_with_decl (decl1,
				    "first argument of `%s' should be `int'");
	       break;

	     case 2:
	       if (TREE_CODE (type) != POINTER_TYPE
		   || TREE_CODE (TREE_TYPE (type)) != POINTER_TYPE
		   || (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (type)))
		       != char_type_node))
		 pedwarn_with_decl (decl1,
				    "second argument of `%s' should be `char **'");
	       break;

	     case 3:
	       if (TREE_CODE (type) != POINTER_TYPE
		   || TREE_CODE (TREE_TYPE (type)) != POINTER_TYPE
		   || (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (type)))
		       != char_type_node))
		 pedwarn_with_decl (decl1,
				    "third argument of `%s' should probably be `char **'");
	       break;
	     }
	 }

       /* It is intentional that this message does not mention the third
	  argument because it's only mentioned in an appendix of the
	  standard.  */
       if (argct > 0 && (argct < 2 || argct > 3))
	 pedwarn_with_decl (decl1, "`%s' takes only zero or two arguments");

       if (! TREE_PUBLIC (decl1))
	 pedwarn_with_decl (decl1, "`%s' is normally a non-static function");
     }

   /* Record the decl so that the function name is defined.
      If we already have a decl for this name, and it is a FUNCTION_DECL,
      use the old decl.  */

   current_function_decl = pushdecl (decl1);

   pushlevel (0);
   declare_parm_level (1);
   current_binding_level->subblocks_tag_transparent = 1;

   make_decl_rtl (current_function_decl, NULL);

   restype = TREE_TYPE (TREE_TYPE (current_function_decl));
   /* Promote the value to int before returning it.  */
   if (c_promoting_integer_type_p (restype))
     {
       /* It retains unsignedness if traditional
	  or if not really getting wider.  */
       if (TREE_UNSIGNED (restype)
	   && (flag_traditional
	       || (TYPE_PRECISION (restype)
		   == TYPE_PRECISION (integer_type_node))))
	 restype = unsigned_type_node;
       else
	 restype = integer_type_node;
     }
   DECL_RESULT (current_function_decl)
     = build_decl (RESULT_DECL, NULL_TREE, restype);

   /* If this fcn was already referenced via a block-scope `extern' decl
      (or an implicit decl), propagate certain information about the usage.  */
   if (TREE_ADDRESSABLE (DECL_ASSEMBLER_NAME (current_function_decl)))
     TREE_ADDRESSABLE (current_function_decl) = 1;

   immediate_size_expand = old_immediate_size_expand;

   start_fname_decls ();

   return 1;
 }

 /* Record that this function is going to be a varargs function.
    This is called before store_parm_decls, which is too early
    to call mark_varargs directly.  */

 void
 c_mark_varargs ()
 {
   c_function_varargs = 1;
 }
 
 /* Store the parameter declarations into the current function declaration.
    This is called after parsing the parameter declarations, before
    digesting the body of the function.

    For an old-style definition, modify the function's type
    to specify at least the number of arguments.  */

 void
 store_parm_decls ()
 {
   tree fndecl = current_function_decl;
   tree parm;

   /* This is either a chain of PARM_DECLs (if a prototype was used)
      or a list of IDENTIFIER_NODEs (for an old-fashioned C definition).  */
   tree specparms = current_function_parms;

   /* This is a list of types declared among parms in a prototype.  */
   tree parmtags = current_function_parm_tags;

   /* This is a chain of PARM_DECLs from old-style parm declarations.  */
   tree parmdecls = getdecls ();

   /* This is a chain of any other decls that came in among the parm
      declarations.  If a parm is declared with  enum {foo, bar} x;
      then CONST_DECLs for foo and bar are put here.  */
   tree nonparms = 0;

   /* The function containing FNDECL, if any.  */
   tree context = decl_function_context (fndecl);

   /* Nonzero if this definition is written with a prototype.  */
   int prototype = 0;

   int saved_warn_shadow = warn_shadow;

   /* Don't re-emit shadow warnings.  */
   warn_shadow = 0;

   if (specparms != 0 && TREE_CODE (specparms) != TREE_LIST)
     {
       /* This case is when the function was defined with an ANSI prototype.
	  The parms already have decls, so we need not do anything here
	  except record them as in effect
	  and complain if any redundant old-style parm decls were written.  */

       tree next;
       tree others = 0;

       prototype = 1;

       if (parmdecls != 0)
	 {
	   tree decl, link;

	   error_with_decl (fndecl,
			    "parm types given both in parmlist and separately");
	   /* Get rid of the erroneous decls; don't keep them on
	      the list of parms, since they might not be PARM_DECLs.  */
	   for (decl = current_binding_level->names;
		decl; decl = TREE_CHAIN (decl))
	     if (DECL_NAME (decl))
	       IDENTIFIER_LOCAL_VALUE (DECL_NAME (decl)) = 0;
	   for (link = current_binding_level->shadowed;
		link; link = TREE_CHAIN (link))
	     IDENTIFIER_LOCAL_VALUE (TREE_PURPOSE (link)) = TREE_VALUE (link);
	   current_binding_level->names = 0;
	   current_binding_level->shadowed = 0;
	 }

       specparms = nreverse (specparms);
       for (parm = specparms; parm; parm = next)
	 {
	   next = TREE_CHAIN (parm);
	   if (TREE_CODE (parm) == PARM_DECL)
	     {
	       if (DECL_NAME (parm) == 0)
		 error_with_decl (parm, "parameter name omitted");
	       else if (TREE_CODE (TREE_TYPE (parm)) != ERROR_MARK
			&& VOID_TYPE_P (TREE_TYPE (parm)))
		 {
		   error_with_decl (parm, "parameter `%s' declared void");
		   /* Change the type to error_mark_node so this parameter
		      will be ignored by assign_parms.  */
		   TREE_TYPE (parm) = error_mark_node;
		 }
	       pushdecl (parm);
	     }
	   else
	     {
	       /* If we find an enum constant or a type tag,
		  put it aside for the moment.  */
	       TREE_CHAIN (parm) = 0;
	       others = chainon (others, parm);
	     }
	 }

       /* Get the decls in their original chain order
	  and record in the function.  */
       DECL_ARGUMENTS (fndecl) = getdecls ();

 #if 0
       /* If this function takes a variable number of arguments,
	  add a phony parameter to the end of the parm list,
	  to represent the position of the first unnamed argument.  */
       if (TREE_VALUE (tree_last (TYPE_ARG_TYPES (TREE_TYPE (fndecl))))
	   != void_type_node)
	 {
	   tree dummy = build_decl (PARM_DECL, NULL_TREE, void_type_node);
	   /* Let's hope the address of the unnamed parm
	      won't depend on its type.  */
	   TREE_TYPE (dummy) = integer_type_node;
	   DECL_ARG_TYPE (dummy) = integer_type_node;
	   DECL_ARGUMENTS (fndecl) = chainon (DECL_ARGUMENTS (fndecl), dummy);
	 }
 #endif

       /* Now pushdecl the enum constants.  */
       for (parm = others; parm; parm = next)
	 {
	   next = TREE_CHAIN (parm);
	   if (DECL_NAME (parm) == 0)
	     ;
	   else if (TYPE_MAIN_VARIANT (TREE_TYPE (parm)) == void_type_node)
	     ;
	   else if (TREE_CODE (parm) != PARM_DECL)
	     pushdecl (parm);
	 }

       storetags (chainon (parmtags, gettags ()));
     }
   else
     {
       /* SPECPARMS is an identifier list--a chain of TREE_LIST nodes
	  each with a parm name as the TREE_VALUE.

	  PARMDECLS is a chain of declarations for parameters.
	  Warning! It can also contain CONST_DECLs which are not parameters
	  but are names of enumerators of any enum types
	  declared among the parameters.

	  First match each formal parameter name with its declaration.
	  Associate decls with the names and store the decls
	  into the TREE_PURPOSE slots.  */

       /* We use DECL_WEAK as a flag to show which parameters have been
	  seen already since it is not used on PARM_DECL or CONST_DECL.  */
       for (parm = parmdecls; parm; parm = TREE_CHAIN (parm))
	 DECL_WEAK (parm) = 0;

       for (parm = specparms; parm; parm = TREE_CHAIN (parm))
	 {
	   tree tail, found = NULL;

	   if (TREE_VALUE (parm) == 0)
	     {
	       error_with_decl (fndecl,
				"parameter name missing from parameter list");
	       TREE_PURPOSE (parm) = 0;
	       continue;
	     }

	   /* See if any of the parmdecls specifies this parm by name.
	      Ignore any enumerator decls.  */
	   for (tail = parmdecls; tail; tail = TREE_CHAIN (tail))
	     if (DECL_NAME (tail) == TREE_VALUE (parm)
		 && TREE_CODE (tail) == PARM_DECL)
	       {
		 found = tail;
		 break;
	       }

	   /* If declaration already marked, we have a duplicate name.
	      Complain, and don't use this decl twice.  */
	   if (found && DECL_WEAK (found))
	     {
	       error_with_decl (found, "multiple parameters named `%s'");
	       found = 0;
	     }

	   /* If the declaration says "void", complain and ignore it.  */
	   if (found && VOID_TYPE_P (TREE_TYPE (found)))
	     {
	       error_with_decl (found, "parameter `%s' declared void");
	       TREE_TYPE (found) = integer_type_node;
	       DECL_ARG_TYPE (found) = integer_type_node;
	       layout_decl (found, 0);
	     }

	   /* Traditionally, a parm declared float is actually a double.  */
	   if (found && flag_traditional
	       && TYPE_MAIN_VARIANT (TREE_TYPE (found)) == float_type_node)
	     {
	       TREE_TYPE (found) = double_type_node;
	       DECL_ARG_TYPE (found) = double_type_node;
	       layout_decl (found, 0);
	     }

	   /* If no declaration found, default to int.  */
	   if (!found)
	     {
	       found = build_decl (PARM_DECL, TREE_VALUE (parm),
				   integer_type_node);
	       DECL_ARG_TYPE (found) = TREE_TYPE (found);
	       DECL_SOURCE_LINE (found) = DECL_SOURCE_LINE (fndecl);
	       DECL_SOURCE_FILE (found) = DECL_SOURCE_FILE (fndecl);
	       if (flag_isoc99)
		 pedwarn_with_decl (found, "type of `%s' defaults to `int'");
	       else if (extra_warnings)
		 warning_with_decl (found, "type of `%s' defaults to `int'");
	       pushdecl (found);
	     }

	   TREE_PURPOSE (parm) = found;

	   /* Mark this decl as "already found".  */
	   DECL_WEAK (found) = 1;
	 }

       /* Put anything which is on the parmdecls chain and which is
	  not a PARM_DECL onto the list NONPARMS.  (The types of
	  non-parm things which might appear on the list include
	  enumerators and NULL-named TYPE_DECL nodes.) Complain about
	  any actual PARM_DECLs not matched with any names.  */

       nonparms = 0;
       for (parm = parmdecls; parm;)
	 {
	   tree next = TREE_CHAIN (parm);
	   TREE_CHAIN (parm) = 0;

	   if (TREE_CODE (parm) != PARM_DECL)
	     nonparms = chainon (nonparms, parm);
	   else
	     {
	       /* Complain about args with incomplete types.  */
	       if (!COMPLETE_TYPE_P (TREE_TYPE (parm)))
		 {
		   error_with_decl (parm, "parameter `%s' has incomplete type");
		   TREE_TYPE (parm) = error_mark_node;
		 }

	       if (! DECL_WEAK (parm))
		 {
		   error_with_decl (parm,
				    "declaration for parameter `%s' but no such parameter");
		   /* Pretend the parameter was not missing.
		      This gets us to a standard state and minimizes
		      further error messages.  */
		   specparms
		     = chainon (specparms,
				tree_cons (parm, NULL_TREE, NULL_TREE));
		 }
	     }

	   parm = next;
	 }

       /* Chain the declarations together in the order of the list of
	  names.  Store that chain in the function decl, replacing the
	  list of names.  */
       parm = specparms;
       DECL_ARGUMENTS (fndecl) = 0;
       {
	 tree last;
	 for (last = 0; parm; parm = TREE_CHAIN (parm))
	   if (TREE_PURPOSE (parm))
	     {
	       if (last == 0)
		 DECL_ARGUMENTS (fndecl) = TREE_PURPOSE (parm);
	       else
		 TREE_CHAIN (last) = TREE_PURPOSE (parm);
	       last = TREE_PURPOSE (parm);
	       TREE_CHAIN (last) = 0;
	     }
       }

       /* If there was a previous prototype,
	  set the DECL_ARG_TYPE of each argument according to
	  the type previously specified, and report any mismatches.  */

       if (TYPE_ARG_TYPES (TREE_TYPE (fndecl)))
	 {
	   tree type;
	   for (parm = DECL_ARGUMENTS (fndecl),
		type = TYPE_ARG_TYPES (TREE_TYPE (fndecl));
		parm || (type && (TYPE_MAIN_VARIANT (TREE_VALUE (type))
				  != void_type_node));
		parm = TREE_CHAIN (parm), type = TREE_CHAIN (type))
	     {
	       if (parm == 0 || type == 0
		   || TYPE_MAIN_VARIANT (TREE_VALUE (type)) == void_type_node)
		 {
		   error ("number of arguments doesn't match prototype");
		   error_with_file_and_line (current_function_prototype_file,
					     current_function_prototype_line,
					     "prototype declaration");
		   break;
		 }
	       /* Type for passing arg must be consistent with that
		  declared for the arg.  ISO C says we take the unqualified
		  type for parameters declared with qualified type.  */
	       if (! comptypes (TYPE_MAIN_VARIANT (DECL_ARG_TYPE (parm)),
				TYPE_MAIN_VARIANT (TREE_VALUE (type))))
		 {
		   if (TYPE_MAIN_VARIANT (TREE_TYPE (parm))
		       == TYPE_MAIN_VARIANT (TREE_VALUE (type)))
		     {
		       /* Adjust argument to match prototype.  E.g. a previous
			  `int foo(float);' prototype causes
			  `int foo(x) float x; {...}' to be treated like
			  `int foo(float x) {...}'.  This is particularly
			  useful for argument types like uid_t.  */
		       DECL_ARG_TYPE (parm) = TREE_TYPE (parm);

		       if (PROMOTE_PROTOTYPES
			   && INTEGRAL_TYPE_P (TREE_TYPE (parm))
			   && TYPE_PRECISION (TREE_TYPE (parm))
			   < TYPE_PRECISION (integer_type_node))
			 DECL_ARG_TYPE (parm) = integer_type_node;

		       if (pedantic)
			 {
			   pedwarn ("promoted argument `%s' doesn't match prototype",
				    IDENTIFIER_POINTER (DECL_NAME (parm)));
			   warning_with_file_and_line
			     (current_function_prototype_file,
			      current_function_prototype_line,
			      "prototype declaration");
			 }
		     }
		   /* If -traditional, allow `int' argument to match
		      `unsigned' prototype.  */
		   else if (! (flag_traditional
			       && TYPE_MAIN_VARIANT (TREE_TYPE (parm)) == integer_type_node
			       && TYPE_MAIN_VARIANT (TREE_VALUE (type)) == unsigned_type_node))
		     {
		       error ("argument `%s' doesn't match prototype",
			      IDENTIFIER_POINTER (DECL_NAME (parm)));
		       error_with_file_and_line (current_function_prototype_file,
						 current_function_prototype_line,
						 "prototype declaration");
		     }
		 }
	     }
	   TYPE_ACTUAL_ARG_TYPES (TREE_TYPE (fndecl)) = 0;
	 }

       /* Otherwise, create a prototype that would match.  */

       else
	 {
	   tree actual = 0, last = 0, type;

	   for (parm = DECL_ARGUMENTS (fndecl); parm; parm = TREE_CHAIN (parm))
	     {
	       type = tree_cons (NULL_TREE, DECL_ARG_TYPE (parm), NULL_TREE);
	       if (last)
		 TREE_CHAIN (last) = type;
	       else
		 actual = type;
	       last = type;
	     }
	   type = tree_cons (NULL_TREE, void_type_node, NULL_TREE);
	   if (last)
	     TREE_CHAIN (last) = type;
	   else
	     actual = type;

	   /* We are going to assign a new value for the TYPE_ACTUAL_ARG_TYPES
	      of the type of this function, but we need to avoid having this
	      affect the types of other similarly-typed functions, so we must
	      first force the generation of an identical (but separate) type
	      node for the relevant function type.  The new node we create
	      will be a variant of the main variant of the original function
	      type.  */

	   TREE_TYPE (fndecl) = build_type_copy (TREE_TYPE (fndecl));

	   TYPE_ACTUAL_ARG_TYPES (TREE_TYPE (fndecl)) = actual;
	 }

       /* Now store the final chain of decls for the arguments
	  as the decl-chain of the current lexical scope.
	  Put the enumerators in as well, at the front so that
	  DECL_ARGUMENTS is not modified.  */

       storedecls (chainon (nonparms, DECL_ARGUMENTS (fndecl)));
     }

   /* Make sure the binding level for the top of the function body
      gets a BLOCK if there are any in the function.
      Otherwise, the dbx output is wrong.  */

   keep_next_if_subblocks = 1;

   /* ??? This might be an improvement,
      but needs to be thought about some more.  */
 #if 0
   keep_next_level_flag = 1;
 #endif

   /* Write a record describing this function definition to the prototypes
      file (if requested).  */

   gen_aux_info_record (fndecl, 1, 0, prototype);

   /* Initialize the RTL code for the function.  */
   init_function_start (fndecl, input_filename, lineno);

   /* Begin the statement tree for this function.  */
   begin_stmt_tree (&DECL_SAVED_TREE (current_function_decl));

   /* If this is a nested function, save away the sizes of any
      variable-size types so that we can expand them when generating
      RTL.  */
   if (context)
     {
       tree t;

       DECL_LANG_SPECIFIC (fndecl)->pending_sizes 
	 = nreverse (get_pending_sizes ());
       for (t = DECL_LANG_SPECIFIC (fndecl)->pending_sizes;
	    t;
	    t = TREE_CHAIN (t))
	 SAVE_EXPR_CONTEXT (TREE_VALUE (t)) = context;
     }

   /* This function is being processed in whole-function mode.  */
   cfun->x_whole_function_mode_p = 1;

   /* Even though we're inside a function body, we still don't want to
      call expand_expr to calculate the size of a variable-sized array.
      We haven't necessarily assigned RTL to all variables yet, so it's
      not safe to try to expand expressions involving them.  */
   immediate_size_expand = 0;
   cfun->x_dont_save_pending_sizes_p = 1;

   warn_shadow = saved_warn_shadow;
 }
 
 /* Finish up a function declaration and compile that function
    all the way to assembler language output.  The free the storage
    for the function definition.

    This is called after parsing the body of the function definition.

    NESTED is nonzero if the function being finished is nested in another.
    CAN_DEFER_P is nonzero if the function may be deferred.  */

 void
 bli_finish_function (nested, can_defer_p)
      int nested;
      int can_defer_p;
 {
   tree fndecl = current_function_decl;

 #if 0
   /* This caused &foo to be of type ptr-to-const-function which then
      got a warning when stored in a ptr-to-function variable.  */
   TREE_READONLY (fndecl) = 1;
 #endif

   poplevel (1, 0, 1);
   BLOCK_SUPERCONTEXT (DECL_INITIAL (fndecl)) = fndecl;

   /* Must mark the RESULT_DECL as being in this function.  */

   DECL_CONTEXT (DECL_RESULT (fndecl)) = fndecl;

   /* Obey `register' declarations if `setjmp' is called in this fn.  */
   if (flag_traditional && current_function_calls_setjmp)
     {
       setjmp_protect (DECL_INITIAL (fndecl));
       setjmp_protect_args ();
     }

   if (MAIN_NAME_P (DECL_NAME (fndecl)) && flag_hosted)
     {
       if (TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (fndecl)))
	   != integer_type_node)
	 {
	   /* If warn_main is 1 (-Wmain) or 2 (-Wall), we have already warned.
	      If warn_main is -1 (-Wno-main) we don't want to be warned.  */
	   if (! warn_main)
	     pedwarn_with_decl (fndecl, "return type of `%s' is not `int'");
	 }
       else
	 {
 #ifdef DEFAULT_MAIN_RETURN
	   /* Make it so that `main' always returns success by default.  */
	   DEFAULT_MAIN_RETURN;
 #else
	   if (flag_isoc99)
	     c_expand_return (integer_zero_node);
 #endif
	 }
     }

   finish_fname_decls ();

   /* Tie off the statement tree for this function.  */
   finish_stmt_tree (&DECL_SAVED_TREE (fndecl));

   /* Complain if there's just no return statement.  */
   if (warn_return_type
       && TREE_CODE (TREE_TYPE (TREE_TYPE (fndecl))) != VOID_TYPE
       && !current_function_returns_value && !current_function_returns_null
       /* Don't complain if we abort.  */
       && !current_function_returns_abnormally
       /* Don't warn for main().  */
       && !MAIN_NAME_P (DECL_NAME (fndecl))
       /* Or if they didn't actually specify a return type.  */
       && !C_FUNCTION_IMPLICIT_INT (fndecl)
       /* Normally, with -Wreturn-type, flow will complain.  Unless we're an
	  inline function, as we might never be compiled separately.  */
       && DECL_INLINE (fndecl))
     warning ("no return statement in function returning non-void");

   /* Clear out memory we no longer need.  */
   free_after_parsing (cfun);
   /* Since we never call rest_of_compilation, we never clear
      CFUN.  Do so explicitly.  */
   free_after_compilation (cfun);
   cfun = NULL;

   if (! nested)
     {
       /* Generate RTL for the body of this function.  */
       bli_expand_body (fndecl, nested, can_defer_p);

       /* Let the error reporting routines know that we're outside a
	  function.  For a nested function, this value is used in
	  pop_c_function_context and then reset via pop_function_context.  */
       current_function_decl = NULL;
     }
 }

 /* Generate the RTL for a deferred function FNDECL.  */

 void
 c_expand_deferred_function (fndecl)
      tree fndecl;
 {
   /* DECL_INLINE or DECL_RESULT might got cleared after the inline
      function was deferred, e.g. in duplicate_decls.  */
   if (DECL_INLINE (fndecl) && DECL_RESULT (fndecl))
     {
       bli_expand_body (fndecl, 0, 0);
       current_function_decl = NULL;
     }
 }

 /* Generate the RTL for the body of FNDECL.  If NESTED_P is non-zero,
    then we are already in the process of generating RTL for another
    function.  If can_defer_p is zero, we won't attempt to defer the
    generation of RTL.  */

 static void
 bli_expand_body (fndecl, nested_p, can_defer_p)
      tree fndecl;
      int nested_p, can_defer_p;
 {
   int uninlinable = 1;

   /* There's no reason to do any of the work here if we're only doing
      semantic analysis; this code just generates RTL.  */
   if (flag_syntax_only)
     return;

   if (flag_inline_trees)
     {
       /* First, cache whether the current function is inlinable.  Some
	  predicates depend on cfun and current_function_decl to
	  function completely.  */
       timevar_push (TV_INTEGRATION);
       uninlinable = ! tree_inlinable_function_p (fndecl);

       if (! uninlinable && can_defer_p
	   /* Save function tree for inlining.  Should return 0 if the
	      language does not support function deferring or the
	      function could not be deferred.  */
	   && defer_fn (fndecl))
	 {
	   /* Let the back-end know that this function exists.  */
	   (*debug_hooks->deferred_inline_function) (fndecl);
	   timevar_pop (TV_INTEGRATION);
	   return;
	 }

       /* Then, inline any functions called in it.  */
       optimize_inline_calls (fndecl);
       timevar_pop (TV_INTEGRATION);
     }

   timevar_push (TV_EXPAND);

   if (nested_p)
     {
       /* Make sure that we will evaluate variable-sized types involved
	  in our function's type.  */
       expand_pending_sizes (DECL_LANG_SPECIFIC (fndecl)->pending_sizes);
       /* Squirrel away our current state.  */
       push_function_context ();
     }

   /* Initialize the RTL code for the function.  */
   current_function_decl = fndecl;
   input_filename = DECL_SOURCE_FILE (fndecl);
   init_function_start (fndecl, input_filename, DECL_SOURCE_LINE (fndecl));

   /* This function is being processed in whole-function mode.  */
   cfun->x_whole_function_mode_p = 1;

   /* Even though we're inside a function body, we still don't want to
      call expand_expr to calculate the size of a variable-sized array.
      We haven't necessarily assigned RTL to all variables yet, so it's
      not safe to try to expand expressions involving them.  */
   immediate_size_expand = 0;
   cfun->x_dont_save_pending_sizes_p = 1;

   /* If this is a varargs function, inform function.c.  */
   if (c_function_varargs)
     mark_varargs ();

   /* Set up parameters and prepare for return, for the function.  */
   expand_function_start (fndecl, 0);

   /* If this function is `main', emit a call to `__main'
      to run global initializers, etc.  */
   if (DECL_NAME (fndecl)
       && MAIN_NAME_P (DECL_NAME (fndecl))
       && DECL_CONTEXT (fndecl) == NULL_TREE)
     expand_main_function ();

   /* Generate the RTL for this function.  */
   bli_expand_stmt (DECL_SAVED_TREE (fndecl));
   if (uninlinable)
     {
       /* Allow the body of the function to be garbage collected.  */
       DECL_SAVED_TREE (fndecl) = NULL_TREE;
     }

   /* We hard-wired immediate_size_expand to zero above.
      expand_function_end will decrement this variable.  So, we set the
      variable to one here, so that after the decrement it will remain
      zero.  */
   immediate_size_expand = 1;

   /* Allow language dialects to perform special processing.  */
   if (lang_expand_function_end)
     (*lang_expand_function_end) ();

   /* Generate rtl for function exit.  */
   expand_function_end (input_filename, lineno, 0);

   /* If this is a nested function, protect the local variables in the stack
      above us from being collected while we're compiling this function.  */
   if (nested_p)
     ggc_push_context ();

   /* Run the optimizers and output the assembler code for this function.  */
   rest_of_compilation (fndecl);

   /* Undo the GC context switch.  */
   if (nested_p)
     ggc_pop_context ();

   /* With just -W, complain only if function returns both with
      and without a value.  */
   if (extra_warnings
       && current_function_returns_value
       && current_function_returns_null)
     warning ("this function may return with or without a value");

   /* If requested, warn about function definitions where the function will
      return a value (usually of some struct or union type) which itself will
      take up a lot of stack space.  */

   if (warn_larger_than && !DECL_EXTERNAL (fndecl) && TREE_TYPE (fndecl))
     {
       tree ret_type = TREE_TYPE (TREE_TYPE (fndecl));

       if (ret_type && TYPE_SIZE_UNIT (ret_type)
	   && TREE_CODE (TYPE_SIZE_UNIT (ret_type)) == INTEGER_CST
	   && 0 < compare_tree_int (TYPE_SIZE_UNIT (ret_type),
				    larger_than_size))
	 {
	   unsigned int size_as_int
	     = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (ret_type));

	   if (compare_tree_int (TYPE_SIZE_UNIT (ret_type), size_as_int) == 0)
	     warning_with_decl (fndecl,
				"size of return value of `%s' is %u bytes",
				size_as_int);
	   else
	     warning_with_decl (fndecl,
				"size of return value of `%s' is larger than %d bytes",
				larger_than_size);
	 }
     }

   if (DECL_SAVED_INSNS (fndecl) == 0 && ! nested_p
       && ! flag_inline_trees)
     {
       /* Stop pointing to the local nodes about to be freed.
	  But DECL_INITIAL must remain nonzero so we know this
	  was an actual function definition.
	  For a nested function, this is done in pop_c_function_context.
	  If rest_of_compilation set this to 0, leave it 0.  */
       if (DECL_INITIAL (fndecl) != 0)
	 DECL_INITIAL (fndecl) = error_mark_node;

       DECL_ARGUMENTS (fndecl) = 0;
     }

   if (DECL_STATIC_CONSTRUCTOR (fndecl))
     {
       if (targetm.have_ctors_dtors)
	 (* targetm.asm_out.constructor) (XEXP (DECL_RTL (fndecl), 0),
					  DEFAULT_INIT_PRIORITY);
       else
	 static_ctors = tree_cons (NULL_TREE, fndecl, static_ctors);
     }

   if (DECL_STATIC_DESTRUCTOR (fndecl))
     {
       if (targetm.have_ctors_dtors)
	 (* targetm.asm_out.destructor) (XEXP (DECL_RTL (fndecl), 0),
					 DEFAULT_INIT_PRIORITY);
       else
	 static_dtors = tree_cons (NULL_TREE, fndecl, static_dtors);
     }

   if (nested_p)
     /* Return to the enclosing function.  */
     pop_function_context ();
   timevar_pop (TV_EXPAND);
 }
 
 /* Check the declarations given in a for-loop for satisfying the C99
    constraints.  */
 void
 check_for_loop_decls ()
 {
   tree t;

   if (!flag_isoc99)
     {
       /* If we get here, declarations have been used in a for loop without
	  the C99 for loop scope.  This doesn't make much sense, so don't
	  allow it.  */
       error ("`for' loop initial declaration used outside C99 mode");
       return;
     }
   /* C99 subclause 6.8.5 paragraph 3:

	[#3]  The  declaration  part  of  a for statement shall only
	declare identifiers for objects having storage class auto or
	register.

      It isn't clear whether, in this sentence, "identifiers" binds to
      "shall only declare" or to "objects" - that is, whether all identifiers
      declared must be identifiers for objects, or whether the restriction
      only applies to those that are.  (A question on this in comp.std.c
      in November 2000 received no answer.)  We implement the strictest
      interpretation, to avoid creating an extension which later causes
      problems.  */

   for (t = gettags (); t; t = TREE_CHAIN (t))
     {
       if (TREE_PURPOSE (t) != 0)
	 {
	   enum tree_code code = TREE_CODE (TREE_VALUE (t));

	   if (code == RECORD_TYPE)
	     error ("`struct %s' declared in `for' loop initial declaration",
		    IDENTIFIER_POINTER (TREE_PURPOSE (t)));
	   else if (code == UNION_TYPE)
	     error ("`union %s' declared in `for' loop initial declaration",
		    IDENTIFIER_POINTER (TREE_PURPOSE (t)));
	   else
	     error ("`enum %s' declared in `for' loop initial declaration",
		    IDENTIFIER_POINTER (TREE_PURPOSE (t)));
	 }
     }

   for (t = getdecls (); t; t = TREE_CHAIN (t))
     {
       if (TREE_CODE (t) != VAR_DECL && DECL_NAME (t))
	 error_with_decl (t, "declaration of non-variable `%s' in `for' loop initial declaration");
       else if (TREE_STATIC (t))
	 error_with_decl (t, "declaration of static variable `%s' in `for' loop initial declaration");
       else if (DECL_EXTERNAL (t))
	 error_with_decl (t, "declaration of `extern' variable `%s' in `for' loop initial declaration");
     }
 }
 
 /* Save and restore the variables in this file and elsewhere
    that keep track of the progress of compilation of the current function.
    Used for nested functions.  */

 struct c_language_function
 {
   struct language_function base;
   tree named_labels;
   tree shadowed_labels;
   int returns_value;
   int returns_null;
   int returns_abnormally;
   int warn_about_return_type;
   int extern_inline;
   struct binding_level *binding_level;
 };

 /* Save and reinitialize the variables
    used during compilation of a C function.  */

 void
 push_c_function_context (f)
      struct function *f;
 {
   struct c_language_function *p;
   p = ((struct c_language_function *)
	xmalloc (sizeof (struct c_language_function)));
   f->language = (struct language_function *) p;

   p->base.x_stmt_tree = c_stmt_tree;
   p->base.x_scope_stmt_stack = c_scope_stmt_stack;
   p->named_labels = named_labels;
   p->shadowed_labels = shadowed_labels;
   p->returns_value = current_function_returns_value;
   p->returns_null = current_function_returns_null;
   p->returns_abnormally = current_function_returns_abnormally;
   p->warn_about_return_type = warn_about_return_type;
   p->extern_inline = current_extern_inline;
   p->binding_level = current_binding_level;
 }

 /* Restore the variables used during compilation of a C function.  */

 void
 pop_c_function_context (f)
      struct function *f;
 {
   struct c_language_function *p
     = (struct c_language_function *) f->language;
   tree link;

   /* Bring back all the labels that were shadowed.  */
   for (link = shadowed_labels; link; link = TREE_CHAIN (link))
     if (DECL_NAME (TREE_VALUE (link)) != 0)
       IDENTIFIER_LABEL_VALUE (DECL_NAME (TREE_VALUE (link)))
	 = TREE_VALUE (link);

   if (DECL_SAVED_INSNS (current_function_decl) == 0
       && DECL_SAVED_TREE (current_function_decl) == NULL_TREE)
     {
       /* Stop pointing to the local nodes about to be freed.  */
       /* But DECL_INITIAL must remain nonzero so we know this
	  was an actual function definition.  */
       DECL_INITIAL (current_function_decl) = error_mark_node;
       DECL_ARGUMENTS (current_function_decl) = 0;
     }

   c_stmt_tree = p->base.x_stmt_tree;
   c_scope_stmt_stack = p->base.x_scope_stmt_stack;
   named_labels = p->named_labels;
   shadowed_labels = p->shadowed_labels;
   current_function_returns_value = p->returns_value;
   current_function_returns_null = p->returns_null;
   current_function_returns_abnormally = p->returns_abnormally;
   warn_about_return_type = p->warn_about_return_type;
   current_extern_inline = p->extern_inline;
   current_binding_level = p->binding_level;

   free (p);
   f->language = 0;
 }

 /* Mark the language specific parts of F for GC.  */

 void
 mark_c_function_context (f)
      struct function *f;
 {
   struct c_language_function *p
     = (struct c_language_function *) f->language;

   if (p == 0)
     return;

   mark_c_language_function (&p->base);
   ggc_mark_tree (p->shadowed_labels);
   ggc_mark_tree (p->named_labels);
   mark_binding_level (&p->binding_level);
 }

 /* Copy the DECL_LANG_SPECIFIC data associated with NODE.  */

 void
 copy_lang_decl (decl)
      tree decl;
 {
   struct lang_decl *ld;

   if (!DECL_LANG_SPECIFIC (decl))
     return;

   ld = (struct lang_decl *) ggc_alloc (sizeof (struct lang_decl));
   memcpy ((char *) ld, (char *) DECL_LANG_SPECIFIC (decl),
	   sizeof (struct lang_decl));
   DECL_LANG_SPECIFIC (decl) = ld;
 }

 /* Mark the language specific bits in T for GC.  */

 void
 lang_mark_tree (t)
      tree t;
 {
   if (TREE_CODE (t) == IDENTIFIER_NODE)
     {
       struct lang_identifier *i = (struct lang_identifier *) t;
       ggc_mark_tree (i->global_value);
       ggc_mark_tree (i->local_value);
       ggc_mark_tree (i->label_value);
       ggc_mark_tree (i->implicit_decl);
       ggc_mark_tree (i->error_locus);
       ggc_mark_tree (i->limbo_value);
     }
   else if (TYPE_P (t) && TYPE_LANG_SPECIFIC (t))
     ggc_mark (TYPE_LANG_SPECIFIC (t));
   else if (DECL_P (t) && DECL_LANG_SPECIFIC (t))
     {
       ggc_mark (DECL_LANG_SPECIFIC (t));
       c_mark_lang_decl (&DECL_LANG_SPECIFIC (t)->base);
       ggc_mark_tree (DECL_LANG_SPECIFIC (t)->pending_sizes);
     }
 }

 /* The functions below are required for functionality of doing
    function at once processing in the C front end. Currently these
    functions are not called from anywhere in the C front end, but as
    these changes continue, that will change.  */

 /* Returns non-zero if the current statement is a full expression,
    i.e. temporaries created during that statement should be destroyed
    at the end of the statement.  */

 int
 stmts_are_full_exprs_p ()
 {
   return 0;
 }

 /* Returns the stmt_tree (if any) to which statements are currently
    being added.  If there is no active statement-tree, NULL is
    returned.  */

 stmt_tree
 current_stmt_tree ()
 {
   return &c_stmt_tree;
 }

 /* Returns the stack of SCOPE_STMTs for the current function.  */

 tree *
 current_scope_stmt_stack ()
 {
   return &c_scope_stmt_stack;
 }

 /* Nonzero if TYPE is an anonymous union or struct type.  Always 0 in
    C.  */

 int
 anon_aggr_type_p (node)
      tree node ATTRIBUTE_UNUSED;
 {
   return 0;
 }

 /* Dummy function in place of callback used by C++.  */

 void
 extract_interface_info ()
 {
 }

 /* Return a new COMPOUND_STMT, after adding it to the current
    statement tree.  */

 tree
 bli_begin_compound_stmt () /* was stmt */
 {
   tree stmt;

   /* Create the COMPOUND_STMT.  */
   stmt = bli_add_stmt (build_stmt (COMPOUND_STMT, NULL_TREE));

   return stmt;
 }

 /* Expand T (a DECL_STMT) if it declares an entity not handled by the
    common code.  */

 void
 c_expand_decl_stmt (t)
      tree t;
 {
   tree decl = DECL_STMT_DECL (t);

   /* Expand nested functions.  */
   if (TREE_CODE (decl) == FUNCTION_DECL
       && DECL_CONTEXT (decl) == current_function_decl
       && DECL_SAVED_TREE (decl))
     bli_expand_body (decl, /*nested_p=*/1, /*can_defer_p=*/0);
 }

 /* Return the IDENTIFIER_GLOBAL_VALUE of T, for use in common code, since
    the definition of IDENTIFIER_GLOBAL_VALUE is different for C and C++.  */

 tree
 identifier_global_value	(t)
      tree t;
 {
   return IDENTIFIER_GLOBAL_VALUE (t);
 }

 /* Record a builtin type for C.  If NAME is non-NULL, it is the name used;
    otherwise the name is found in ridpointers from RID_INDEX.  */

 void
 record_builtin_type (rid_index, name, type)
      enum rid rid_index;
      const char *name;
      tree type;
 {
   tree id;
   if (name == 0)
     id = ridpointers[(int) rid_index];
   else
     id = get_identifier (name);
   pushdecl (build_decl (TYPE_DECL, id, type));
 }

 /* Build the void_list_node (void_type_node having been created).  */
 tree
 build_void_list_node ()
 {
   tree t = build_tree_list (NULL_TREE, void_type_node);
   return t;
 }


/* Build expressions with type checking for C compiler.
   Copyright (C) 1987, 1988, 1991, 1992, 1993, 1994, 1995, 1996, 1997,
   1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */


/* This file is part of the C front end.
   It contains routines to build C expressions given their operands,
   including computing the types of the result, C-specific error checks,
   and some optimization.

   There are also routines to build RETURN_STMT nodes and CASE_STMT nodes,
   and to process initializations in declarations (since they work
   like a strange sort of assignment).  */

#include "system.h"
#include "rtl.h"
#include "c-tree.h"
#include "tm_p.h"
#include "flags.h"
#include "output.h"
#include "toplev.h"
#include "intl.h"

/* Nonzero if we've already printed a "missing braces around initializer"
   message within this initializer.  */
static int missing_braces_mentioned;

/* 1 if we explained undeclared var errors.  */
static int undeclared_variable_notice;

static tree qualify_type		PARAMS ((tree, tree));
static int comp_target_types		PARAMS ((tree, tree));
static int function_types_compatible_p	PARAMS ((tree, tree));
static int type_lists_compatible_p	PARAMS ((tree, tree));
static tree decl_constant_value_for_broken_optimization PARAMS ((tree));
static tree default_function_array_conversion	PARAMS ((tree));
static tree lookup_field		PARAMS ((tree, tree));
static tree convert_arguments		PARAMS ((tree, tree, tree, tree));
static tree pointer_diff		PARAMS ((tree, tree));
static tree unary_complex_lvalue	PARAMS ((enum tree_code, tree, int));
static void pedantic_lvalue_warning	PARAMS ((enum tree_code));
static tree internal_build_compound_expr PARAMS ((tree, int));
static tree convert_for_assignment	PARAMS ((tree, tree, const char *,
						 tree, tree, int));
static void warn_for_assignment		PARAMS ((const char *, const char *,
						 tree, int));
static tree valid_compound_expr_initializer PARAMS ((tree, tree));
static void push_string			PARAMS ((const char *));
static void push_member_name		PARAMS ((tree));
static void push_array_bounds		PARAMS ((int));
static int spelling_length		PARAMS ((void));
static char *print_spelling		PARAMS ((char *));
static void warning_init		PARAMS ((const char *));
static tree digest_init			PARAMS ((tree, tree, int, int));
static void output_init_element		PARAMS ((tree, tree, tree, int));
static void output_pending_init_elements PARAMS ((int));
static int set_designator		PARAMS ((int));
static void push_range_stack		PARAMS ((tree));
static void add_pending_init		PARAMS ((tree, tree));
static void set_nonincremental_init	PARAMS ((void));
static void set_nonincremental_init_from_string	PARAMS ((tree));
static tree find_init_member		PARAMS ((tree));

/* Do `exp = require_complete_type (exp);' to make sure exp
   does not have an incomplete type.  (That includes void types.)  */

tree
require_complete_type (value)
     tree value;
{
  tree type = TREE_TYPE (value);

  if (value == error_mark_node || type == error_mark_node)
    return error_mark_node;

  /* First, detect a valid value with a complete type.  */
  if (COMPLETE_TYPE_P (type))
    return value;

  incomplete_type_error (value, type);
  return error_mark_node;
}

/* Print an error message for invalid use of an incomplete type.
   VALUE is the expression that was used (or 0 if that isn't known)
   and TYPE is the type that was invalid.  */

void
incomplete_type_error (value, type)
     tree value;
     tree type;
{
  const char *type_code_string;

  /* Avoid duplicate error message.  */
  if (TREE_CODE (type) == ERROR_MARK)
    return;

  if (value != 0 && (TREE_CODE (value) == VAR_DECL
		     || TREE_CODE (value) == PARM_DECL))
    error ("`%s' has an incomplete type",
	   IDENTIFIER_POINTER (DECL_NAME (value)));
  else
    {
    retry:
      /* We must print an error message.  Be clever about what it says.  */

      switch (TREE_CODE (type))
	{
	case RECORD_TYPE:
	  type_code_string = "struct";
	  break;

	case UNION_TYPE:
	  type_code_string = "union";
	  break;

	case ENUMERAL_TYPE:
	  type_code_string = "enum";
	  break;

	case VOID_TYPE:
	  error ("invalid use of void expression");
	  return;

	case ARRAY_TYPE:
	  if (TYPE_DOMAIN (type))
	    {
	      if (TYPE_MAX_VALUE (TYPE_DOMAIN (type)) == NULL)
		{
		  error ("invalid use of flexible array member");
		  return;
		}
	      type = TREE_TYPE (type);
	      goto retry;
	    }
	  error ("invalid use of array with unspecified bounds");
	  return;

	default:
	  abort ();
	}

      if (TREE_CODE (TYPE_NAME (type)) == IDENTIFIER_NODE)
	error ("invalid use of undefined type `%s %s'",
	       type_code_string, IDENTIFIER_POINTER (TYPE_NAME (type)));
      else
	/* If this type has a typedef-name, the TYPE_NAME is a TYPE_DECL.  */
	error ("invalid use of incomplete typedef `%s'",
	       IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (type))));
    }
}

/* Return a variant of TYPE which has all the type qualifiers of LIKE
   as well as those of TYPE.  */

static tree
qualify_type (type, like)
     tree type, like;
{
  return c_build_qualified_type (type, 
				 TYPE_QUALS (type) | TYPE_QUALS (like));
}

/* Return the common type of two types.
   We assume that comptypes has already been done and returned 1;
   if that isn't so, this may crash.  In particular, we assume that qualifiers
   match.

   This is the type for the result of most arithmetic operations
   if the operands have the given two types.  */

tree
common_type (t1, t2)
     tree t1, t2;
{
  enum tree_code code1;
  enum tree_code code2;
  tree attributes;

  /* Save time if the two types are the same.  */

  if (t1 == t2) return t1;

  /* If one type is nonsense, use the other.  */
  if (t1 == error_mark_node)
    return t2;
  if (t2 == error_mark_node)
    return t1;

  /* Merge the attributes.  */
  attributes = (*targetm.merge_type_attributes) (t1, t2);

  /* Treat an enum type as the unsigned integer type of the same width.  */

  if (TREE_CODE (t1) == ENUMERAL_TYPE)
    t1 = type_for_size (TYPE_PRECISION (t1), 1);
  if (TREE_CODE (t2) == ENUMERAL_TYPE)
    t2 = type_for_size (TYPE_PRECISION (t2), 1);

  code1 = TREE_CODE (t1);
  code2 = TREE_CODE (t2);

  /* If one type is complex, form the common type of the non-complex
     components, then make that complex.  Use T1 or T2 if it is the
     required type.  */
  if (code1 == COMPLEX_TYPE || code2 == COMPLEX_TYPE)
    {
      tree subtype1 = code1 == COMPLEX_TYPE ? TREE_TYPE (t1) : t1;
      tree subtype2 = code2 == COMPLEX_TYPE ? TREE_TYPE (t2) : t2;
      tree subtype = common_type (subtype1, subtype2);

      if (code1 == COMPLEX_TYPE && TREE_TYPE (t1) == subtype)
	return build_type_attribute_variant (t1, attributes);
      else if (code2 == COMPLEX_TYPE && TREE_TYPE (t2) == subtype)
	return build_type_attribute_variant (t2, attributes);
      else
	return build_type_attribute_variant (build_complex_type (subtype),
					     attributes);
    }

  switch (code1)
    {
    case INTEGER_TYPE:
    case REAL_TYPE:
      /* If only one is real, use it as the result.  */

      if (code1 == REAL_TYPE && code2 != REAL_TYPE)
	return build_type_attribute_variant (t1, attributes);

      if (code2 == REAL_TYPE && code1 != REAL_TYPE)
	return build_type_attribute_variant (t2, attributes);

      /* Both real or both integers; use the one with greater precision.  */

      if (TYPE_PRECISION (t1) > TYPE_PRECISION (t2))
	return build_type_attribute_variant (t1, attributes);
      else if (TYPE_PRECISION (t2) > TYPE_PRECISION (t1))
	return build_type_attribute_variant (t2, attributes);

      /* Same precision.  Prefer longs to ints even when same size.  */

      if (TYPE_MAIN_VARIANT (t1) == long_unsigned_type_node
	  || TYPE_MAIN_VARIANT (t2) == long_unsigned_type_node)
	return build_type_attribute_variant (long_unsigned_type_node,
					     attributes);

      if (TYPE_MAIN_VARIANT (t1) == long_integer_type_node
	  || TYPE_MAIN_VARIANT (t2) == long_integer_type_node)
	{
	  /* But preserve unsignedness from the other type,
	     since long cannot hold all the values of an unsigned int.  */
	  if (TREE_UNSIGNED (t1) || TREE_UNSIGNED (t2))
	     t1 = long_unsigned_type_node;
	  else
	     t1 = long_integer_type_node;
	  return build_type_attribute_variant (t1, attributes);
	}

      /* Likewise, prefer long double to double even if same size.  */
      if (TYPE_MAIN_VARIANT (t1) == long_double_type_node
	  || TYPE_MAIN_VARIANT (t2) == long_double_type_node)
	return build_type_attribute_variant (long_double_type_node,
					     attributes);

      /* Otherwise prefer the unsigned one.  */

      if (TREE_UNSIGNED (t1))
	return build_type_attribute_variant (t1, attributes);
      else
	return build_type_attribute_variant (t2, attributes);

    case POINTER_TYPE:
      /* For two pointers, do this recursively on the target type,
	 and combine the qualifiers of the two types' targets.  */
      /* This code was turned off; I don't know why.
	 But ANSI C specifies doing this with the qualifiers.
	 So I turned it on again.  */
      {
	tree pointed_to_1 = TREE_TYPE (t1);
	tree pointed_to_2 = TREE_TYPE (t2);
	tree target = common_type (TYPE_MAIN_VARIANT (pointed_to_1),
				   TYPE_MAIN_VARIANT (pointed_to_2));
	t1 = build_pointer_type (c_build_qualified_type 
				 (target, 
				  TYPE_QUALS (pointed_to_1) | 
				  TYPE_QUALS (pointed_to_2)));
	return build_type_attribute_variant (t1, attributes);
      }
#if 0
      t1 = build_pointer_type (common_type (TREE_TYPE (t1), TREE_TYPE (t2)));
      return build_type_attribute_variant (t1, attributes);
#endif

    case ARRAY_TYPE:
      {
	tree elt = common_type (TREE_TYPE (t1), TREE_TYPE (t2));
	/* Save space: see if the result is identical to one of the args.  */
	if (elt == TREE_TYPE (t1) && TYPE_DOMAIN (t1))
	  return build_type_attribute_variant (t1, attributes);
	if (elt == TREE_TYPE (t2) && TYPE_DOMAIN (t2))
	  return build_type_attribute_variant (t2, attributes);
	/* Merge the element types, and have a size if either arg has one.  */
	t1 = build_array_type (elt, TYPE_DOMAIN (TYPE_DOMAIN (t1) ? t1 : t2));
	return build_type_attribute_variant (t1, attributes);
      }

    case FUNCTION_TYPE:
      /* Function types: prefer the one that specified arg types.
	 If both do, merge the arg types.  Also merge the return types.  */
      {
	tree valtype = common_type (TREE_TYPE (t1), TREE_TYPE (t2));
	tree p1 = TYPE_ARG_TYPES (t1);
	tree p2 = TYPE_ARG_TYPES (t2);
	int len;
	tree newargs, n;
	int i;

	/* Save space: see if the result is identical to one of the args.  */
	if (valtype == TREE_TYPE (t1) && ! TYPE_ARG_TYPES (t2))
	  return build_type_attribute_variant (t1, attributes);
	if (valtype == TREE_TYPE (t2) && ! TYPE_ARG_TYPES (t1))
	  return build_type_attribute_variant (t2, attributes);

	/* Simple way if one arg fails to specify argument types.  */
	if (TYPE_ARG_TYPES (t1) == 0)
	 {
	   t1 = build_function_type (valtype, TYPE_ARG_TYPES (t2));
	   return build_type_attribute_variant (t1, attributes);
	 }
	if (TYPE_ARG_TYPES (t2) == 0)
	 {
	   t1 = build_function_type (valtype, TYPE_ARG_TYPES (t1));
	   return build_type_attribute_variant (t1, attributes);
	 }

	/* If both args specify argument types, we must merge the two
	   lists, argument by argument.  */

	pushlevel (0);
	declare_parm_level (1);

	len = list_length (p1);
	newargs = 0;

	for (i = 0; i < len; i++)
	  newargs = tree_cons (NULL_TREE, NULL_TREE, newargs);

	n = newargs;

	for (; p1;
	     p1 = TREE_CHAIN (p1), p2 = TREE_CHAIN (p2), n = TREE_CHAIN (n))
	  {
	    /* A null type means arg type is not specified.
	       Take whatever the other function type has.  */
	    if (TREE_VALUE (p1) == 0)
	      {
		TREE_VALUE (n) = TREE_VALUE (p2);
		goto parm_done;
	      }
	    if (TREE_VALUE (p2) == 0)
	      {
		TREE_VALUE (n) = TREE_VALUE (p1);
		goto parm_done;
	      }
	      
	    /* Given  wait (union {union wait *u; int *i} *)
	       and  wait (union wait *),
	       prefer  union wait *  as type of parm.  */
	    if (TREE_CODE (TREE_VALUE (p1)) == UNION_TYPE
		&& TREE_VALUE (p1) != TREE_VALUE (p2))
	      {
		tree memb;
		for (memb = TYPE_FIELDS (TREE_VALUE (p1));
		     memb; memb = TREE_CHAIN (memb))
		  if (comptypes (TREE_TYPE (memb), TREE_VALUE (p2)))
		    {
		      TREE_VALUE (n) = TREE_VALUE (p2);
		      if (pedantic)
			pedwarn ("function types not truly compatible in ISO C");
		      goto parm_done;
		    }
	      }
	    if (TREE_CODE (TREE_VALUE (p2)) == UNION_TYPE
		&& TREE_VALUE (p2) != TREE_VALUE (p1))
	      {
		tree memb;
		for (memb = TYPE_FIELDS (TREE_VALUE (p2));
		     memb; memb = TREE_CHAIN (memb))
		  if (comptypes (TREE_TYPE (memb), TREE_VALUE (p1)))
		    {
		      TREE_VALUE (n) = TREE_VALUE (p1);
		      if (pedantic)
			pedwarn ("function types not truly compatible in ISO C");
		      goto parm_done;
		    }
	      }
	    TREE_VALUE (n) = common_type (TREE_VALUE (p1), TREE_VALUE (p2));
	  parm_done: ;
	  }

	poplevel (0, 0, 0);

	t1 = build_function_type (valtype, newargs);
	/* ... falls through ...  */
      }

    default:
      return build_type_attribute_variant (t1, attributes);
    }

}

/* Return 1 if TYPE1 and TYPE2 are compatible types for assignment
   or various other operations.  Return 2 if they are compatible
   but a warning may be needed if you use them together.  */

int
comptypes (type1, type2)
     tree type1, type2;
{
  tree t1 = type1;
  tree t2 = type2;
  int attrval, val;

  /* Suppress errors caused by previously reported errors.  */

  if (t1 == t2 || !t1 || !t2
      || TREE_CODE (t1) == ERROR_MARK || TREE_CODE (t2) == ERROR_MARK)
    return 1;

  /* If either type is the internal version of sizetype, return the
     language version.  */
  if (TREE_CODE (t1) == INTEGER_TYPE && TYPE_IS_SIZETYPE (t1)
      && TYPE_DOMAIN (t1) != 0)
    t1 = TYPE_DOMAIN (t1);

  if (TREE_CODE (t2) == INTEGER_TYPE && TYPE_IS_SIZETYPE (t2)
      && TYPE_DOMAIN (t2) != 0)
    t2 = TYPE_DOMAIN (t2);

  /* Treat an enum type as the integer type of the same width and 
     signedness.  */

  if (TREE_CODE (t1) == ENUMERAL_TYPE)
    t1 = type_for_size (TYPE_PRECISION (t1), TREE_UNSIGNED (t1));
  if (TREE_CODE (t2) == ENUMERAL_TYPE)
    t2 = type_for_size (TYPE_PRECISION (t2), TREE_UNSIGNED (t2));

  if (t1 == t2)
    return 1;

  /* Different classes of types can't be compatible.  */

  if (TREE_CODE (t1) != TREE_CODE (t2)) return 0;

  /* Qualifiers must match.  */

  if (TYPE_QUALS (t1) != TYPE_QUALS (t2))
    return 0;

  /* Allow for two different type nodes which have essentially the same
     definition.  Note that we already checked for equality of the type
     qualifiers (just above).  */

  if (TYPE_MAIN_VARIANT (t1) == TYPE_MAIN_VARIANT (t2))
    return 1;

  /* 1 if no need for warning yet, 2 if warning cause has been seen.  */
  if (! (attrval = (*targetm.comp_type_attributes) (t1, t2)))
     return 0;

  /* 1 if no need for warning yet, 2 if warning cause has been seen.  */
  val = 0;

  switch (TREE_CODE (t1))
    {
    case POINTER_TYPE:
      val = (TREE_TYPE (t1) == TREE_TYPE (t2)
	      ? 1 : comptypes (TREE_TYPE (t1), TREE_TYPE (t2)));
      break;

    case FUNCTION_TYPE:
      val = function_types_compatible_p (t1, t2);
      break;

    case ARRAY_TYPE:
      {
	tree d1 = TYPE_DOMAIN (t1);
	tree d2 = TYPE_DOMAIN (t2);
	bool d1_variable, d2_variable;
	bool d1_zero, d2_zero;
	val = 1;

	/* Target types must match incl. qualifiers.  */
	if (TREE_TYPE (t1) != TREE_TYPE (t2)
	    && 0 == (val = comptypes (TREE_TYPE (t1), TREE_TYPE (t2))))
	  return 0;

	/* Sizes must match unless one is missing or variable.  */
	if (d1 == 0 || d2 == 0 || d1 == d2)
	  break;

	d1_zero = ! TYPE_MAX_VALUE (d1);
	d2_zero = ! TYPE_MAX_VALUE (d2);

	d1_variable = (! d1_zero
		       && (TREE_CODE (TYPE_MIN_VALUE (d1)) != INTEGER_CST
			   || TREE_CODE (TYPE_MAX_VALUE (d1)) != INTEGER_CST));
	d2_variable = (! d2_zero
		       && (TREE_CODE (TYPE_MIN_VALUE (d2)) != INTEGER_CST
			   || TREE_CODE (TYPE_MAX_VALUE (d2)) != INTEGER_CST));

	if (d1_variable || d2_variable)
	  break;
	if (d1_zero && d2_zero)
	  break;
	if (d1_zero || d2_zero
	    || ! tree_int_cst_equal (TYPE_MIN_VALUE (d1), TYPE_MIN_VALUE (d2))
	    || ! tree_int_cst_equal (TYPE_MAX_VALUE (d1), TYPE_MAX_VALUE (d2)))
	  val = 0;

        break;
      }

    case RECORD_TYPE:
      if (maybe_objc_comptypes (t1, t2, 0) == 1)
	val = 1;
      break;

    default:
      break;
    }
  return attrval == 2 && val == 1 ? 2 : val;
}

/* Return 1 if TTL and TTR are pointers to types that are equivalent,
   ignoring their qualifiers.  */

static int
comp_target_types (ttl, ttr)
     tree ttl, ttr;
{
  int val;

  /* Give maybe_objc_comptypes a crack at letting these types through.  */
  if ((val = maybe_objc_comptypes (ttl, ttr, 1)) >= 0)
    return val;

  val = comptypes (TYPE_MAIN_VARIANT (TREE_TYPE (ttl)),
		   TYPE_MAIN_VARIANT (TREE_TYPE (ttr)));

  if (val == 2 && pedantic)
    pedwarn ("types are not quite compatible");
  return val;
}

/* Subroutines of `comptypes'.  */

/* Return 1 if two function types F1 and F2 are compatible.
   If either type specifies no argument types,
   the other must specify a fixed number of self-promoting arg types.
   Otherwise, if one type specifies only the number of arguments, 
   the other must specify that number of self-promoting arg types.
   Otherwise, the argument types must match.  */

static int
function_types_compatible_p (f1, f2)
     tree f1, f2;
{
  tree args1, args2;
  /* 1 if no need for warning yet, 2 if warning cause has been seen.  */
  int val = 1;
  int val1;

  if (!(TREE_TYPE (f1) == TREE_TYPE (f2)
	|| (val = comptypes (TREE_TYPE (f1), TREE_TYPE (f2)))))
    return 0;

  args1 = TYPE_ARG_TYPES (f1);
  args2 = TYPE_ARG_TYPES (f2);

  /* An unspecified parmlist matches any specified parmlist
     whose argument types don't need default promotions.  */

  if (args1 == 0)
    {
      if (!self_promoting_args_p (args2))
	return 0;
      /* If one of these types comes from a non-prototype fn definition,
	 compare that with the other type's arglist.
	 If they don't match, ask for a warning (but no error).  */
      if (TYPE_ACTUAL_ARG_TYPES (f1)
	  && 1 != type_lists_compatible_p (args2, TYPE_ACTUAL_ARG_TYPES (f1)))
	val = 2;
      return val;
    }
  if (args2 == 0)
    {
      if (!self_promoting_args_p (args1))
	return 0;
      if (TYPE_ACTUAL_ARG_TYPES (f2)
	  && 1 != type_lists_compatible_p (args1, TYPE_ACTUAL_ARG_TYPES (f2)))
	val = 2;
      return val;
    }

  /* Both types have argument lists: compare them and propagate results.  */
  val1 = type_lists_compatible_p (args1, args2);
  return val1 != 1 ? val1 : val;
}

/* Check two lists of types for compatibility,
   returning 0 for incompatible, 1 for compatible,
   or 2 for compatible with warning.  */

static int
type_lists_compatible_p (args1, args2)
     tree args1, args2;
{
  /* 1 if no need for warning yet, 2 if warning cause has been seen.  */
  int val = 1;
  int newval = 0;

  while (1)
    {
      if (args1 == 0 && args2 == 0)
	return val;
      /* If one list is shorter than the other,
	 they fail to match.  */
      if (args1 == 0 || args2 == 0)
	return 0;
      /* A null pointer instead of a type
	 means there is supposed to be an argument
	 but nothing is specified about what type it has.
	 So match anything that self-promotes.  */
      if (TREE_VALUE (args1) == 0)
	{
	  if (simple_type_promotes_to (TREE_VALUE (args2)) != NULL_TREE)
	    return 0;
	}
      else if (TREE_VALUE (args2) == 0)
	{
	  if (simple_type_promotes_to (TREE_VALUE (args1)) != NULL_TREE)
	    return 0;
	}
      else if (! (newval = comptypes (TYPE_MAIN_VARIANT (TREE_VALUE (args1)), 
				      TYPE_MAIN_VARIANT (TREE_VALUE (args2)))))
	{
	  /* Allow  wait (union {union wait *u; int *i} *)
	     and  wait (union wait *)  to be compatible.  */
	  if (TREE_CODE (TREE_VALUE (args1)) == UNION_TYPE
	      && (TYPE_NAME (TREE_VALUE (args1)) == 0
		  || TYPE_TRANSPARENT_UNION (TREE_VALUE (args1)))
	      && TREE_CODE (TYPE_SIZE (TREE_VALUE (args1))) == INTEGER_CST
	      && tree_int_cst_equal (TYPE_SIZE (TREE_VALUE (args1)),
				     TYPE_SIZE (TREE_VALUE (args2))))
	    {
	      tree memb;
	      for (memb = TYPE_FIELDS (TREE_VALUE (args1));
		   memb; memb = TREE_CHAIN (memb))
		if (comptypes (TREE_TYPE (memb), TREE_VALUE (args2)))
		  break;
	      if (memb == 0)
		return 0;
	    }
	  else if (TREE_CODE (TREE_VALUE (args2)) == UNION_TYPE
		   && (TYPE_NAME (TREE_VALUE (args2)) == 0
		       || TYPE_TRANSPARENT_UNION (TREE_VALUE (args2)))
		   && TREE_CODE (TYPE_SIZE (TREE_VALUE (args2))) == INTEGER_CST
		   && tree_int_cst_equal (TYPE_SIZE (TREE_VALUE (args2)),
					  TYPE_SIZE (TREE_VALUE (args1))))
	    {
	      tree memb;
	      for (memb = TYPE_FIELDS (TREE_VALUE (args2));
		   memb; memb = TREE_CHAIN (memb))
		if (comptypes (TREE_TYPE (memb), TREE_VALUE (args1)))
		  break;
	      if (memb == 0)
		return 0;
	    }
	  else
	    return 0;
	}

      /* comptypes said ok, but record if it said to warn.  */
      if (newval > val)
	val = newval;

      args1 = TREE_CHAIN (args1);
      args2 = TREE_CHAIN (args2);
    }
}

/* Compute the value of the `sizeof' operator.  */

tree
c_sizeof (type)
     tree type;
{
  enum tree_code code = TREE_CODE (type);
  tree size;

  if (code == FUNCTION_TYPE)
    {
      if (pedantic || warn_pointer_arith)
	pedwarn ("sizeof applied to a function type");
      size = size_one_node;
    }
  else if (code == VOID_TYPE)
    {
      if (pedantic || warn_pointer_arith)
	pedwarn ("sizeof applied to a void type");
      size = size_one_node;
    }
  else if (code == ERROR_MARK)
    size = size_one_node;
  else if (!COMPLETE_TYPE_P (type))
    {
      error ("sizeof applied to an incomplete type");
      size = size_zero_node;
    }
  else
    /* Convert in case a char is more than one unit.  */
    size = size_binop (CEIL_DIV_EXPR, TYPE_SIZE_UNIT (type),
		       size_int (TYPE_PRECISION (char_type_node)
			         / BITS_PER_UNIT));

  /* SIZE will have an integer type with TYPE_IS_SIZETYPE set.
     TYPE_IS_SIZETYPE means that certain things (like overflow) will
     never happen.  However, this node should really have type
     `size_t', which is just a typedef for an ordinary integer type.  */
  return fold (build1 (NOP_EXPR, c_size_type_node, size));
}

tree
c_sizeof_nowarn (type)
     tree type;
{
  enum tree_code code = TREE_CODE (type);
  tree size;

  if (code == FUNCTION_TYPE || code == VOID_TYPE || code == ERROR_MARK)
    size = size_one_node;
  else if (!COMPLETE_TYPE_P (type))
    size = size_zero_node;
  else
    /* Convert in case a char is more than one unit.  */
    size = size_binop (CEIL_DIV_EXPR, TYPE_SIZE_UNIT (type),
		       size_int (TYPE_PRECISION (char_type_node)
			         / BITS_PER_UNIT));

  /* SIZE will have an integer type with TYPE_IS_SIZETYPE set.
     TYPE_IS_SIZETYPE means that certain things (like overflow) will
     never happen.  However, this node should really have type
     `size_t', which is just a typedef for an ordinary integer type.  */
  return fold (build1 (NOP_EXPR, c_size_type_node, size));
}

/* Compute the size to increment a pointer by.  */

tree
c_size_in_bytes (type)
     tree type;
{
  enum tree_code code = TREE_CODE (type);

  if (code == FUNCTION_TYPE || code == VOID_TYPE || code == ERROR_MARK)
    return size_one_node;

  if (!COMPLETE_OR_VOID_TYPE_P (type))
    {
      error ("arithmetic on pointer to an incomplete type");
      return size_one_node;
    }

  /* Convert in case a char is more than one unit.  */
  return size_binop (CEIL_DIV_EXPR, TYPE_SIZE_UNIT (type),
		     size_int (TYPE_PRECISION (char_type_node)
			       / BITS_PER_UNIT));
}

/* Return either DECL or its known constant value (if it has one).  */

tree
decl_constant_value (decl)
     tree decl;
{
  if (/* Don't change a variable array bound or initial value to a constant
	 in a place where a variable is invalid.  */
      current_function_decl != 0
      && ! TREE_THIS_VOLATILE (decl)
      && TREE_READONLY (decl)
      && DECL_INITIAL (decl) != 0
      && TREE_CODE (DECL_INITIAL (decl)) != ERROR_MARK
      /* This is invalid if initial value is not constant.
	 If it has either a function call, a memory reference,
	 or a variable, then re-evaluating it could give different results.  */
      && TREE_CONSTANT (DECL_INITIAL (decl))
      /* Check for cases where this is sub-optimal, even though valid.  */
      && TREE_CODE (DECL_INITIAL (decl)) != CONSTRUCTOR)
    return DECL_INITIAL (decl);
  return decl;
}

/* Return either DECL or its known constant value (if it has one), but
   return DECL if pedantic or DECL has mode BLKmode.  This is for
   bug-compatibility with the old behavior of decl_constant_value
   (before GCC 3.0); every use of this function is a bug and it should
   be removed before GCC 3.1.  It is not appropriate to use pedantic
   in a way that affects optimization, and BLKmode is probably not the
   right test for avoiding misoptimizations either.  */

static tree
decl_constant_value_for_broken_optimization (decl)
     tree decl;
{
  if (pedantic || DECL_MODE (decl) == BLKmode)
    return decl;
  else
    return decl_constant_value (decl);
}


/* Perform the default conversion of arrays and functions to pointers.
   Return the result of converting EXP.  For any other expression, just
   return EXP.  */

static tree
default_function_array_conversion (exp)
     tree exp;
{
  tree orig_exp;
  tree type = TREE_TYPE (exp);
  enum tree_code code = TREE_CODE (type);
  int not_lvalue = 0;

  /* Strip NON_LVALUE_EXPRs and no-op conversions, since we aren't using as
     an lvalue. 

     Do not use STRIP_NOPS here!  It will remove conversions from pointer
     to integer and cause infinite recursion.  */
  orig_exp = exp;
  while (TREE_CODE (exp) == NON_LVALUE_EXPR
	 || (TREE_CODE (exp) == NOP_EXPR
	     && TREE_TYPE (TREE_OPERAND (exp, 0)) == TREE_TYPE (exp)))
    {
      if (TREE_CODE (exp) == NON_LVALUE_EXPR)
	not_lvalue = 1;
      exp = TREE_OPERAND (exp, 0);
    }

  /* Preserve the original expression code.  */
  if (IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (TREE_CODE (exp))))
    C_SET_EXP_ORIGINAL_CODE (exp, C_EXP_ORIGINAL_CODE (orig_exp));

  if (code == FUNCTION_TYPE)
    {
      return build_unary_op (ADDR_EXPR, exp, 0);
    }
  if (code == ARRAY_TYPE)
    {
      tree adr;
      tree restype = TREE_TYPE (type);
      tree ptrtype;
      int constp = 0;
      int volatilep = 0;
      int lvalue_array_p;

      if (TREE_CODE_CLASS (TREE_CODE (exp)) == 'r' || DECL_P (exp))
	{
	  constp = TREE_READONLY (exp);
	  volatilep = TREE_THIS_VOLATILE (exp);
	}

      if (TYPE_QUALS (type) || constp || volatilep)
	restype 
	  = c_build_qualified_type (restype,
				    TYPE_QUALS (type) 
				    | (constp * TYPE_QUAL_CONST)
				    | (volatilep * TYPE_QUAL_VOLATILE));

      if (TREE_CODE (exp) == INDIRECT_REF)
	return convert (TYPE_POINTER_TO (restype),
			TREE_OPERAND (exp, 0));

      if (TREE_CODE (exp) == COMPOUND_EXPR)
	{
	  tree op1 = default_conversion (TREE_OPERAND (exp, 1));
	  return build (COMPOUND_EXPR, TREE_TYPE (op1),
			TREE_OPERAND (exp, 0), op1);
	}

      lvalue_array_p = !not_lvalue && lvalue_p (exp);
      if (!flag_isoc99 && !lvalue_array_p)
	{
	  /* Before C99, non-lvalue arrays do not decay to pointers.
	     Normally, using such an array would be invalid; but it can
	     be used correctly inside sizeof or as a statement expression.
	     Thus, do not give an error here; an error will result later.  */
	  return exp;
	}

      ptrtype = build_pointer_type (restype);

      if (TREE_CODE (exp) == VAR_DECL)
	{
	  /* ??? This is not really quite correct
	     in that the type of the operand of ADDR_EXPR
	     is not the target type of the type of the ADDR_EXPR itself.
	     Question is, can this lossage be avoided?  */
	  adr = build1 (ADDR_EXPR, ptrtype, exp);
	  if (mark_addressable (exp) == 0)
	    return error_mark_node;
	  TREE_CONSTANT (adr) = staticp (exp);
	  TREE_SIDE_EFFECTS (adr) = 0;   /* Default would be, same as EXP.  */
	  return adr;
	}
      /* This way is better for a COMPONENT_REF since it can
	 simplify the offset for a component.  */
      adr = build_unary_op (ADDR_EXPR, exp, 1);
      return convert (ptrtype, adr);
    }
  return exp;
}

/* Perform default promotions for C data used in expressions.
   Arrays and functions are converted to pointers;
   enumeral types or short or char, to int.
   In addition, manifest constants symbols are replaced by their values.  */

tree
default_conversion (exp)
     tree exp;
{
  tree orig_exp;
  tree type = TREE_TYPE (exp);
  enum tree_code code = TREE_CODE (type);

  if (code == FUNCTION_TYPE || code == ARRAY_TYPE)
    return default_function_array_conversion (exp);

  /* Constants can be used directly unless they're not loadable.  */
  if (TREE_CODE (exp) == CONST_DECL)
    exp = DECL_INITIAL (exp);

  /* Replace a nonvolatile const static variable with its value unless
     it is an array, in which case we must be sure that taking the
     address of the array produces consistent results.  */
  else if (optimize && TREE_CODE (exp) == VAR_DECL && code != ARRAY_TYPE)
    {
      exp = decl_constant_value_for_broken_optimization (exp);
      type = TREE_TYPE (exp);
    }

  /* Strip NON_LVALUE_EXPRs and no-op conversions, since we aren't using as
     an lvalue. 

     Do not use STRIP_NOPS here!  It will remove conversions from pointer
     to integer and cause infinite recursion.  */
  orig_exp = exp;
  while (TREE_CODE (exp) == NON_LVALUE_EXPR
	 || (TREE_CODE (exp) == NOP_EXPR
	     && TREE_TYPE (TREE_OPERAND (exp, 0)) == TREE_TYPE (exp)))
    exp = TREE_OPERAND (exp, 0);

  /* Preserve the original expression code.  */
  if (IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (TREE_CODE (exp))))
    C_SET_EXP_ORIGINAL_CODE (exp, C_EXP_ORIGINAL_CODE (orig_exp));

  /* Normally convert enums to int,
     but convert wide enums to something wider.  */
  if (code == ENUMERAL_TYPE)
    {
      type = type_for_size (MAX (TYPE_PRECISION (type),
				 TYPE_PRECISION (integer_type_node)),
			    ((flag_traditional
			      || (TYPE_PRECISION (type)
				  >= TYPE_PRECISION (integer_type_node)))
			     && TREE_UNSIGNED (type)));

      return convert (type, exp);
    }

  if (TREE_CODE (exp) == COMPONENT_REF
      && DECL_C_BIT_FIELD (TREE_OPERAND (exp, 1))
      /* If it's thinner than an int, promote it like a
	 c_promoting_integer_type_p, otherwise leave it alone.  */
      && 0 > compare_tree_int (DECL_SIZE (TREE_OPERAND (exp, 1)),
			       TYPE_PRECISION (integer_type_node)))
    return convert (flag_traditional && TREE_UNSIGNED (type)
		    ? unsigned_type_node : integer_type_node,
		    exp);

  if (c_promoting_integer_type_p (type))
    {
      /* Traditionally, unsignedness is preserved in default promotions.
         Also preserve unsignedness if not really getting any wider.  */
      if (TREE_UNSIGNED (type)
	  && (flag_traditional
	      || TYPE_PRECISION (type) == TYPE_PRECISION (integer_type_node)))
	return convert (unsigned_type_node, exp);

      return convert (integer_type_node, exp);
    }

  if (flag_traditional && !flag_allow_single_precision
      && TYPE_MAIN_VARIANT (type) == float_type_node)
    return convert (double_type_node, exp);

  if (code == VOID_TYPE)
    {
      error ("void value not ignored as it ought to be");
      return error_mark_node;
    }
  return exp;
}

/* Look up COMPONENT in a structure or union DECL.

   If the component name is not found, returns NULL_TREE.  Otherwise,
   the return value is a TREE_LIST, with each TREE_VALUE a FIELD_DECL
   stepping down the chain to the component, which is in the last
   TREE_VALUE of the list.  Normally the list is of length one, but if
   the component is embedded within (nested) anonymous structures or
   unions, the list steps down the chain to the component.  */
     
static tree
lookup_field (decl, component)
     tree decl, component;
{
  tree type = TREE_TYPE (decl);
  tree field;

  /* If TYPE_LANG_SPECIFIC is set, then it is a sorted array of pointers
     to the field elements.  Use a binary search on this array to quickly
     find the element.  Otherwise, do a linear search.  TYPE_LANG_SPECIFIC
     will always be set for structures which have many elements.  */

  if (TYPE_LANG_SPECIFIC (type))
    {
      int bot, top, half;
      tree *field_array = &TYPE_LANG_SPECIFIC (type)->elts[0];

      field = TYPE_FIELDS (type);
      bot = 0;
      top = TYPE_LANG_SPECIFIC (type)->len;
      while (top - bot > 1)
	{
	  half = (top - bot + 1) >> 1;
	  field = field_array[bot+half];

	  if (DECL_NAME (field) == NULL_TREE)
	    {
	      /* Step through all anon unions in linear fashion.  */
	      while (DECL_NAME (field_array[bot]) == NULL_TREE)
		{
		  field = field_array[bot++];
		  if (TREE_CODE (TREE_TYPE (field)) == RECORD_TYPE
		      || TREE_CODE (TREE_TYPE (field)) == UNION_TYPE)
		    {
		      tree anon = lookup_field (field, component);

		      if (anon)
			return tree_cons (NULL_TREE, field, anon);
		    } 
		}

	      /* Entire record is only anon unions.  */
	      if (bot > top)
		return NULL_TREE;

	      /* Restart the binary search, with new lower bound.  */
	      continue;
	    }

	  if (DECL_NAME (field) == component)
	    break;
	  if (DECL_NAME (field) < component)
	    bot += half;
	  else
	    top = bot + half;
	}

      if (DECL_NAME (field_array[bot]) == component)
	field = field_array[bot];
      else if (DECL_NAME (field) != component)
	return NULL_TREE;
    }
  else
    {
      for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field))
	{
	  if (DECL_NAME (field) == NULL_TREE
	      && (TREE_CODE (TREE_TYPE (field)) == RECORD_TYPE
		  || TREE_CODE (TREE_TYPE (field)) == UNION_TYPE))
	    {
	      tree anon = lookup_field (field, component);

	      if (anon)
		return tree_cons (NULL_TREE, field, anon);
	    }

	  if (DECL_NAME (field) == component)
	    break;
	}

      if (field == NULL_TREE)
	return NULL_TREE;
    }

  return tree_cons (NULL_TREE, field, NULL_TREE);
}

/* Make an expression to refer to the COMPONENT field of
   structure or union value DATUM.  COMPONENT is an IDENTIFIER_NODE.  */

tree
build_component_ref (datum, component)
     tree datum, component;
{
  tree type = TREE_TYPE (datum);
  enum tree_code code = TREE_CODE (type);
  tree field = NULL;
  tree ref;

  /* If DATUM is a COMPOUND_EXPR, move our reference inside it.
     If pedantic ensure that the arguments are not lvalues; otherwise,
     if the component is an array, it would wrongly decay to a pointer in
     C89 mode.
     We cannot do this with a COND_EXPR, because in a conditional expression
     the default promotions are applied to both sides, and this would yield
     the wrong type of the result; for example, if the components have
     type "char".  */
  switch (TREE_CODE (datum))
    {
    case COMPOUND_EXPR:
      {
	tree value = build_component_ref (TREE_OPERAND (datum, 1), component);
	return build (COMPOUND_EXPR, TREE_TYPE (value),
		      TREE_OPERAND (datum, 0), pedantic_non_lvalue (value));
      }
    default:
      break;
    }

  /* See if there is a field or component with name COMPONENT.  */

  if (code == RECORD_TYPE || code == UNION_TYPE)
    {
      if (!COMPLETE_TYPE_P (type))
	{
	  incomplete_type_error (NULL_TREE, type);
	  return error_mark_node;
	}

      field = lookup_field (datum, component);

      if (!field)
	{
	  error ("%s has no member named `%s'",
		 code == RECORD_TYPE ? "structure" : "union",
		 IDENTIFIER_POINTER (component));
	  return error_mark_node;
	}

      /* Chain the COMPONENT_REFs if necessary down to the FIELD.
	 This might be better solved in future the way the C++ front
	 end does it - by giving the anonymous entities each a
	 separate name and type, and then have build_component_ref
	 recursively call itself.  We can't do that here.  */
      for (; field; field = TREE_CHAIN (field))
	{
	  tree subdatum = TREE_VALUE (field);

	  if (TREE_TYPE (subdatum) == error_mark_node)
	    return error_mark_node;

	  ref = build (COMPONENT_REF, TREE_TYPE (subdatum), datum, subdatum);
	  if (TREE_READONLY (datum) || TREE_READONLY (subdatum))
	    TREE_READONLY (ref) = 1;
	  if (TREE_THIS_VOLATILE (datum) || TREE_THIS_VOLATILE (subdatum))
	    TREE_THIS_VOLATILE (ref) = 1;

	  if (TREE_DEPRECATED (subdatum))
	    warn_deprecated_use (subdatum);

	  datum = ref;
	}

      return ref;
    }
  else if (code != ERROR_MARK)
    error ("request for member `%s' in something not a structure or union",
	    IDENTIFIER_POINTER (component));

  return error_mark_node;
}

/* Given an expression PTR for a pointer, return an expression
   for the value pointed to.
   ERRORSTRING is the name of the operator to appear in error messages.  */

tree
build_indirect_ref (ptr, errorstring)
     tree ptr;
     const char *errorstring;
{
  tree pointer = default_conversion (ptr);
  tree type = TREE_TYPE (pointer);

  if (TREE_CODE (type) == POINTER_TYPE)
    {
      if (TREE_CODE (pointer) == ADDR_EXPR
	  && !flag_volatile
	  && (TREE_TYPE (TREE_OPERAND (pointer, 0))
	      == TREE_TYPE (type)))
	return TREE_OPERAND (pointer, 0);
      else
	{
	  tree t = TREE_TYPE (type);
	  tree ref = build1 (INDIRECT_REF, TYPE_MAIN_VARIANT (t), pointer);

	  if (!COMPLETE_OR_VOID_TYPE_P (t) && TREE_CODE (t) != ARRAY_TYPE)
	    {
	      error ("dereferencing pointer to incomplete type");
	      return error_mark_node;
	    }
	  if (VOID_TYPE_P (t) && skip_evaluation == 0)
	    warning ("dereferencing `void *' pointer");

	  /* We *must* set TREE_READONLY when dereferencing a pointer to const,
	     so that we get the proper error message if the result is used
	     to assign to.  Also, &* is supposed to be a no-op.
	     And ANSI C seems to specify that the type of the result
	     should be the const type.  */
	  /* A de-reference of a pointer to const is not a const.  It is valid
	     to change it via some other pointer.  */
	  TREE_READONLY (ref) = TYPE_READONLY (t);
	  TREE_SIDE_EFFECTS (ref)
	    = TYPE_VOLATILE (t) || TREE_SIDE_EFFECTS (pointer) || flag_volatile;
	  TREE_THIS_VOLATILE (ref) = TYPE_VOLATILE (t);
	  return ref;
	}
    }
  else if (TREE_CODE (pointer) != ERROR_MARK)
    error ("invalid type argument of `%s'", errorstring);
  return error_mark_node;
}

/* This handles expressions of the form "a[i]", which denotes
   an array reference.

   This is logically equivalent in C to *(a+i), but we may do it differently.
   If A is a variable or a member, we generate a primitive ARRAY_REF.
   This avoids forcing the array out of registers, and can work on
   arrays that are not lvalues (for example, members of structures returned
   by functions).  */

tree
build_array_ref (array, index)
     tree array, index;
{
  if (index == 0)
    {
      error ("subscript missing in array reference");
      return error_mark_node;
    }

  if (TREE_TYPE (array) == error_mark_node
      || TREE_TYPE (index) == error_mark_node)
    return error_mark_node;

  if (TREE_CODE (TREE_TYPE (array)) == ARRAY_TYPE
      && TREE_CODE (array) != INDIRECT_REF)
    {
      tree rval, type;

      /* Subscripting with type char is likely to lose
	 on a machine where chars are signed.
	 So warn on any machine, but optionally.
	 Don't warn for unsigned char since that type is safe.
	 Don't warn for signed char because anyone who uses that
	 must have done so deliberately.  */
      if (warn_char_subscripts
	  && TYPE_MAIN_VARIANT (TREE_TYPE (index)) == char_type_node)
	warning ("array subscript has type `char'");

      /* Apply default promotions *after* noticing character types.  */
      index = default_conversion (index);

      /* Require integer *after* promotion, for sake of enums.  */
      if (TREE_CODE (TREE_TYPE (index)) != INTEGER_TYPE)
	{
	  error ("array subscript is not an integer");
	  return error_mark_node;
	}

      /* An array that is indexed by a non-constant
	 cannot be stored in a register; we must be able to do
	 address arithmetic on its address.
	 Likewise an array of elements of variable size.  */
      if (TREE_CODE (index) != INTEGER_CST
	  || (COMPLETE_TYPE_P (TREE_TYPE (TREE_TYPE (array)))
	      && TREE_CODE (TYPE_SIZE (TREE_TYPE (TREE_TYPE (array)))) != INTEGER_CST))
	{
	  if (mark_addressable (array) == 0)
	    return error_mark_node;
	}
      /* An array that is indexed by a constant value which is not within
	 the array bounds cannot be stored in a register either; because we
	 would get a crash in store_bit_field/extract_bit_field when trying
	 to access a non-existent part of the register.  */
      if (TREE_CODE (index) == INTEGER_CST
	  && TYPE_VALUES (TREE_TYPE (array))
	  && ! int_fits_type_p (index, TYPE_VALUES (TREE_TYPE (array))))
	{
	  if (mark_addressable (array) == 0)
	    return error_mark_node;
	}

      if (pedantic)
	{
	  tree foo = array;
	  while (TREE_CODE (foo) == COMPONENT_REF)
	    foo = TREE_OPERAND (foo, 0);
	  if (TREE_CODE (foo) == VAR_DECL && DECL_REGISTER (foo))
	    pedwarn ("ISO C forbids subscripting `register' array");
	  else if (! flag_isoc99 && ! lvalue_p (foo))
	    pedwarn ("ISO C89 forbids subscripting non-lvalue array");
	}

      type = TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (array)));
      rval = build (ARRAY_REF, type, array, index);
      /* Array ref is const/volatile if the array elements are
         or if the array is.  */
      TREE_READONLY (rval)
	|= (TYPE_READONLY (TREE_TYPE (TREE_TYPE (array)))
	    | TREE_READONLY (array));
      TREE_SIDE_EFFECTS (rval)
	|= (TYPE_VOLATILE (TREE_TYPE (TREE_TYPE (array)))
	    | TREE_SIDE_EFFECTS (array));
      TREE_THIS_VOLATILE (rval)
	|= (TYPE_VOLATILE (TREE_TYPE (TREE_TYPE (array)))
	    /* This was added by rms on 16 Nov 91.
	       It fixes  vol struct foo *a;  a->elts[1] 
	       in an inline function.
	       Hope it doesn't break something else.  */
	    | TREE_THIS_VOLATILE (array));
      return require_complete_type (fold (rval));
    }

  {
    tree ar = default_conversion (array);
    tree ind = default_conversion (index);

    /* Do the same warning check as above, but only on the part that's
       syntactically the index and only if it is also semantically
       the index.  */
    if (warn_char_subscripts
	&& TREE_CODE (TREE_TYPE (index)) == INTEGER_TYPE
	&& TYPE_MAIN_VARIANT (TREE_TYPE (index)) == char_type_node)
      warning ("subscript has type `char'");

    /* Put the integer in IND to simplify error checking.  */
    if (TREE_CODE (TREE_TYPE (ar)) == INTEGER_TYPE)
      {
	tree temp = ar;
	ar = ind;
	ind = temp;
      }

    if (ar == error_mark_node)
      return ar;

    if (TREE_CODE (TREE_TYPE (ar)) != POINTER_TYPE
	|| TREE_CODE (TREE_TYPE (TREE_TYPE (ar))) == FUNCTION_TYPE)
      {
	error ("subscripted value is neither array nor pointer");
	return error_mark_node;
      }
    if (TREE_CODE (TREE_TYPE (ind)) != INTEGER_TYPE)
      {
	error ("array subscript is not an integer");
	return error_mark_node;
      }

    return build_indirect_ref (build_binary_op (PLUS_EXPR, ar, ind, 0),
			       "array indexing");
  }
}

/* Build an external reference to identifier ID.  FUN indicates
   whether this will be used for a function call.  */
tree
build_external_ref (id, fun)
     tree id;
     int fun;
{
  tree ref;
  tree decl = lookup_name (id);
  tree objc_ivar = lookup_objc_ivar (id);

  if (decl && TREE_DEPRECATED (decl))
    warn_deprecated_use (decl);

  if (!decl || decl == error_mark_node || C_DECL_ANTICIPATED (decl))
    {
      if (objc_ivar)
	ref = objc_ivar;
      else if (fun)
	{
	  if (!decl || decl == error_mark_node)
	    /* Ordinary implicit function declaration.  */
	    ref = implicitly_declare (id);
	  else
	    {
	      /* Implicit declaration of built-in function.  Don't
		 change the built-in declaration, but don't let this
		 go by silently, either.  */
	      implicit_decl_warning (id);

	      /* only issue this warning once */
	      C_DECL_ANTICIPATED (decl) = 0;
	      ref = decl;
	    }
	}
      else
	{
	  /* Reference to undeclared variable, including reference to
	     builtin outside of function-call context.  */
	  if (current_function_decl == 0)
	    error ("`%s' undeclared here (not in a function)",
		   IDENTIFIER_POINTER (id));
	  else
	    {
	      if (IDENTIFIER_GLOBAL_VALUE (id) != error_mark_node
		  || IDENTIFIER_ERROR_LOCUS (id) != current_function_decl)
		{
		  error ("`%s' undeclared (first use in this function)",
			 IDENTIFIER_POINTER (id));

		  if (! undeclared_variable_notice)
		    {
		      error ("(Each undeclared identifier is reported only once");
		      error ("for each function it appears in.)");
		      undeclared_variable_notice = 1;
		    }
		}
	      IDENTIFIER_GLOBAL_VALUE (id) = error_mark_node;
	      IDENTIFIER_ERROR_LOCUS (id) = current_function_decl;
	    }
	  return error_mark_node;
	}
    }
  else
    {
      /* Properly declared variable or function reference.  */
      if (!objc_ivar)
	ref = decl;
      else if (decl != objc_ivar && IDENTIFIER_LOCAL_VALUE (id))
	{
	  warning ("local declaration of `%s' hides instance variable",
		   IDENTIFIER_POINTER (id));
	  ref = decl;
	}
      else
	ref = objc_ivar;
    }

  if (TREE_TYPE (ref) == error_mark_node)
    return error_mark_node;

  assemble_external (ref);
  TREE_USED (ref) = 1;

  if (TREE_CODE (ref) == CONST_DECL)
    {
      ref = DECL_INITIAL (ref);
      TREE_CONSTANT (ref) = 1;
    }

  return ref;
}

/* Build a function call to function FUNCTION with parameters PARAMS.
   PARAMS is a list--a chain of TREE_LIST nodes--in which the
   TREE_VALUE of each node is a parameter-expression.
   FUNCTION's data type may be a function type or a pointer-to-function.  */

tree
build_function_call (function, params)
     tree function, params;
{
  tree fntype, fundecl = 0;
  tree coerced_params;
  tree name = NULL_TREE, assembler_name = NULL_TREE, result;

  /* Strip NON_LVALUE_EXPRs, etc., since we aren't using as an lvalue.  */
  STRIP_TYPE_NOPS (function);

  /* Convert anything with function type to a pointer-to-function.  */
  if (TREE_CODE (function) == FUNCTION_DECL)
    {
      name = DECL_NAME (function);
      assembler_name = DECL_ASSEMBLER_NAME (function);

      /* Differs from default_conversion by not setting TREE_ADDRESSABLE
	 (because calling an inline function does not mean the function
	 needs to be separately compiled).  */
      fntype = build_type_variant (TREE_TYPE (function),
				   TREE_READONLY (function),
				   TREE_THIS_VOLATILE (function));
      fundecl = function;
      function = build1 (ADDR_EXPR, build_pointer_type (fntype), function);
    }
  else
    function = default_conversion (function);

  fntype = TREE_TYPE (function);

  if (TREE_CODE (fntype) == ERROR_MARK)
    return error_mark_node;

  if (!(TREE_CODE (fntype) == POINTER_TYPE
	&& TREE_CODE (TREE_TYPE (fntype)) == FUNCTION_TYPE))
    {
      error ("called object is not a function");
      return error_mark_node;
    }

  if (fundecl && TREE_THIS_VOLATILE (fundecl))
    current_function_returns_abnormally = 1;

  /* fntype now gets the type of function pointed to.  */
  fntype = TREE_TYPE (fntype);

  /* Convert the parameters to the types declared in the
     function prototype, or apply default promotions.  */

  coerced_params
    = convert_arguments (TYPE_ARG_TYPES (fntype), params, name, fundecl);

  /* Check for errors in format strings.  */

  if (warn_format)
    check_function_format (NULL, TYPE_ATTRIBUTES (fntype), coerced_params);

  /* Recognize certain built-in functions so we can make tree-codes
     other than CALL_EXPR.  We do this when it enables fold-const.c
     to do something useful.  */

  if (TREE_CODE (function) == ADDR_EXPR
      && TREE_CODE (TREE_OPERAND (function, 0)) == FUNCTION_DECL
      && DECL_BUILT_IN (TREE_OPERAND (function, 0)))
    {
      result = expand_tree_builtin (TREE_OPERAND (function, 0),
				    params, coerced_params);
      if (result)
	return result;
    }

  result = build (CALL_EXPR, TREE_TYPE (fntype),
		  function, coerced_params, NULL_TREE);
  TREE_SIDE_EFFECTS (result) = 1;
  result = fold (result);

  if (VOID_TYPE_P (TREE_TYPE (result)))
    return result;
  return require_complete_type (result);
}

/* Convert the argument expressions in the list VALUES
   to the types in the list TYPELIST.  The result is a list of converted
   argument expressions.

   If TYPELIST is exhausted, or when an element has NULL as its type,
   perform the default conversions.

   PARMLIST is the chain of parm decls for the function being called.
   It may be 0, if that info is not available.
   It is used only for generating error messages.

   NAME is an IDENTIFIER_NODE or 0.  It is used only for error messages.

   This is also where warnings about wrong number of args are generated.

   Both VALUES and the returned value are chains of TREE_LIST nodes
   with the elements of the list in the TREE_VALUE slots of those nodes.  */

static tree
convert_arguments (typelist, values, name, fundecl)
     tree typelist, values, name, fundecl;
{
  tree typetail, valtail;
  tree result = NULL;
  int parmnum;

  /* Scan the given expressions and types, producing individual
     converted arguments and pushing them on RESULT in reverse order.  */

  for (valtail = values, typetail = typelist, parmnum = 0;
       valtail;
       valtail = TREE_CHAIN (valtail), parmnum++)
    {
      tree type = typetail ? TREE_VALUE (typetail) : 0;
      tree val = TREE_VALUE (valtail);

      if (type == void_type_node)
	{
	  if (name)
	    error ("too many arguments to function `%s'",
		   IDENTIFIER_POINTER (name));
	  else
	    error ("too many arguments to function");
	  break;
	}

      /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
      /* Do not use STRIP_NOPS here!  We do not want an enumerator with value 0
	 to convert automatically to a pointer.  */
      if (TREE_CODE (val) == NON_LVALUE_EXPR)
	val = TREE_OPERAND (val, 0);

      val = default_function_array_conversion (val);

      val = require_complete_type (val);

      if (type != 0)
	{
	  /* Formal parm type is specified by a function prototype.  */
	  tree parmval;

	  if (!COMPLETE_TYPE_P (type))
	    {
	      error ("type of formal parameter %d is incomplete", parmnum + 1);
	      parmval = val;
	    }
	  else
	    {
	      /* Optionally warn about conversions that
		 differ from the default conversions.  */
	      if (warn_conversion || warn_traditional)
		{
		  int formal_prec = TYPE_PRECISION (type);

		  if (INTEGRAL_TYPE_P (type)
		      && TREE_CODE (TREE_TYPE (val)) == REAL_TYPE)
		    warn_for_assignment ("%s as integer rather than floating due to prototype", (char *) 0, name, parmnum + 1);
		  if (INTEGRAL_TYPE_P (type)
		      && TREE_CODE (TREE_TYPE (val)) == COMPLEX_TYPE)
		    warn_for_assignment ("%s as integer rather than complex due to prototype", (char *) 0, name, parmnum + 1);
		  else if (TREE_CODE (type) == COMPLEX_TYPE
			   && TREE_CODE (TREE_TYPE (val)) == REAL_TYPE)
		    warn_for_assignment ("%s as complex rather than floating due to prototype", (char *) 0, name, parmnum + 1);
		  else if (TREE_CODE (type) == REAL_TYPE
			   && INTEGRAL_TYPE_P (TREE_TYPE (val)))
		    warn_for_assignment ("%s as floating rather than integer due to prototype", (char *) 0, name, parmnum + 1);
		  else if (TREE_CODE (type) == COMPLEX_TYPE
			   && INTEGRAL_TYPE_P (TREE_TYPE (val)))
		    warn_for_assignment ("%s as complex rather than integer due to prototype", (char *) 0, name, parmnum + 1);
		  else if (TREE_CODE (type) == REAL_TYPE
			   && TREE_CODE (TREE_TYPE (val)) == COMPLEX_TYPE)
		    warn_for_assignment ("%s as floating rather than complex due to prototype", (char *) 0, name, parmnum + 1);
		  /* ??? At some point, messages should be written about
		     conversions between complex types, but that's too messy
		     to do now.  */
		  else if (TREE_CODE (type) == REAL_TYPE
			   && TREE_CODE (TREE_TYPE (val)) == REAL_TYPE)
		    {
		      /* Warn if any argument is passed as `float',
			 since without a prototype it would be `double'.  */
		      if (formal_prec == TYPE_PRECISION (float_type_node))
			warn_for_assignment ("%s as `float' rather than `double' due to prototype", (char *) 0, name, parmnum + 1);
		    }
		  /* Detect integer changing in width or signedness.
		     These warnings are only activated with
		     -Wconversion, not with -Wtraditional.  */
		  else if (warn_conversion && INTEGRAL_TYPE_P (type)
			   && INTEGRAL_TYPE_P (TREE_TYPE (val)))
		    {
		      tree would_have_been = default_conversion (val);
		      tree type1 = TREE_TYPE (would_have_been);

		      if (TREE_CODE (type) == ENUMERAL_TYPE
			  && (TYPE_MAIN_VARIANT (type)
			      == TYPE_MAIN_VARIANT (TREE_TYPE (val))))
			/* No warning if function asks for enum
			   and the actual arg is that enum type.  */
			;
		      else if (formal_prec != TYPE_PRECISION (type1))
			warn_for_assignment ("%s with different width due to prototype", (char *) 0, name, parmnum + 1);
		      else if (TREE_UNSIGNED (type) == TREE_UNSIGNED (type1))
			;
		      /* Don't complain if the formal parameter type
			 is an enum, because we can't tell now whether
			 the value was an enum--even the same enum.  */
		      else if (TREE_CODE (type) == ENUMERAL_TYPE)
			;
		      else if (TREE_CODE (val) == INTEGER_CST
			       && int_fits_type_p (val, type))
			/* Change in signedness doesn't matter
			   if a constant value is unaffected.  */
			;
		      /* Likewise for a constant in a NOP_EXPR.  */
		      else if (TREE_CODE (val) == NOP_EXPR
			       && TREE_CODE (TREE_OPERAND (val, 0)) == INTEGER_CST
			       && int_fits_type_p (TREE_OPERAND (val, 0), type))
			;
#if 0 /* We never get such tree structure here.  */
		      else if (TREE_CODE (TREE_TYPE (val)) == ENUMERAL_TYPE
			       && int_fits_type_p (TYPE_MIN_VALUE (TREE_TYPE (val)), type)
			       && int_fits_type_p (TYPE_MAX_VALUE (TREE_TYPE (val)), type))
			/* Change in signedness doesn't matter
			   if an enum value is unaffected.  */
			;
#endif
		      /* If the value is extended from a narrower
			 unsigned type, it doesn't matter whether we
			 pass it as signed or unsigned; the value
			 certainly is the same either way.  */
		      else if (TYPE_PRECISION (TREE_TYPE (val)) < TYPE_PRECISION (type)
			       && TREE_UNSIGNED (TREE_TYPE (val)))
			;
		      else if (TREE_UNSIGNED (type))
			warn_for_assignment ("%s as unsigned due to prototype", (char *) 0, name, parmnum + 1);
		      else
			warn_for_assignment ("%s as signed due to prototype", (char *) 0, name, parmnum + 1);
		    }
		}

	      parmval = convert_for_assignment (type, val, 
					        (char *) 0, /* arg passing  */
						fundecl, name, parmnum + 1);
	      
	      if (PROMOTE_PROTOTYPES
		  && INTEGRAL_TYPE_P (type)
		  && (TYPE_PRECISION (type) < TYPE_PRECISION (integer_type_node)))
		parmval = default_conversion (parmval);
	    }
	  result = tree_cons (NULL_TREE, parmval, result);
	}
      else if (TREE_CODE (TREE_TYPE (val)) == REAL_TYPE
               && (TYPE_PRECISION (TREE_TYPE (val))
	           < TYPE_PRECISION (double_type_node)))
	/* Convert `float' to `double'.  */
	result = tree_cons (NULL_TREE, convert (double_type_node, val), result);
      else
	/* Convert `short' and `char' to full-size `int'.  */
	result = tree_cons (NULL_TREE, default_conversion (val), result);

      if (typetail)
	typetail = TREE_CHAIN (typetail);
    }

  if (typetail != 0 && TREE_VALUE (typetail) != void_type_node)
    {
      if (name)
	error ("too few arguments to function `%s'",
	       IDENTIFIER_POINTER (name));
      else
	error ("too few arguments to function");
    }

  return nreverse (result);
}

/* This is the entry point used by the parser
   for binary operators in the input.
   In addition to constructing the expression,
   we check for operands that were written with other binary operators
   in a way that is likely to confuse the user.  */

tree
parser_build_binary_op (code, arg1, arg2)
     enum tree_code code;
     tree arg1, arg2;
{
  tree result = build_binary_op (code, arg1, arg2, 1);

  char class;
  char class1 = TREE_CODE_CLASS (TREE_CODE (arg1));
  char class2 = TREE_CODE_CLASS (TREE_CODE (arg2));
  enum tree_code code1 = ERROR_MARK;
  enum tree_code code2 = ERROR_MARK;

  if (TREE_CODE (result) == ERROR_MARK)
    return error_mark_node;

  if (IS_EXPR_CODE_CLASS (class1))
    code1 = C_EXP_ORIGINAL_CODE (arg1);
  if (IS_EXPR_CODE_CLASS (class2))
    code2 = C_EXP_ORIGINAL_CODE (arg2);

  /* Check for cases such as x+y<<z which users are likely
     to misinterpret.  If parens are used, C_EXP_ORIGINAL_CODE
     is cleared to prevent these warnings.  */
  if (warn_parentheses)
    {
      if (code == LSHIFT_EXPR || code == RSHIFT_EXPR)
	{
	  if (code1 == PLUS_EXPR || code1 == MINUS_EXPR
	      || code2 == PLUS_EXPR || code2 == MINUS_EXPR)
	    warning ("suggest parentheses around + or - inside shift");
	}

      if (code == TRUTH_ORIF_EXPR)
	{
	  if (code1 == TRUTH_ANDIF_EXPR
	      || code2 == TRUTH_ANDIF_EXPR)
	    warning ("suggest parentheses around && within ||");
	}

      if (code == BIT_IOR_EXPR)
	{
	  if (code1 == BIT_AND_EXPR || code1 == BIT_XOR_EXPR
	      || code1 == PLUS_EXPR || code1 == MINUS_EXPR
	      || code2 == BIT_AND_EXPR || code2 == BIT_XOR_EXPR
	      || code2 == PLUS_EXPR || code2 == MINUS_EXPR)
	    warning ("suggest parentheses around arithmetic in operand of |");
	  /* Check cases like x|y==z */
	  if (TREE_CODE_CLASS (code1) == '<' || TREE_CODE_CLASS (code2) == '<')
	    warning ("suggest parentheses around comparison in operand of |");
	}

      if (code == BIT_XOR_EXPR)
	{
	  if (code1 == BIT_AND_EXPR
	      || code1 == PLUS_EXPR || code1 == MINUS_EXPR
	      || code2 == BIT_AND_EXPR
	      || code2 == PLUS_EXPR || code2 == MINUS_EXPR)
	    warning ("suggest parentheses around arithmetic in operand of ^");
	  /* Check cases like x^y==z */
	  if (TREE_CODE_CLASS (code1) == '<' || TREE_CODE_CLASS (code2) == '<')
	    warning ("suggest parentheses around comparison in operand of ^");
	}

      if (code == BIT_AND_EXPR)
	{
	  if (code1 == PLUS_EXPR || code1 == MINUS_EXPR
	      || code2 == PLUS_EXPR || code2 == MINUS_EXPR)
	    warning ("suggest parentheses around + or - in operand of &");
	  /* Check cases like x&y==z */
	  if (TREE_CODE_CLASS (code1) == '<' || TREE_CODE_CLASS (code2) == '<')
	    warning ("suggest parentheses around comparison in operand of &");
	}
    }

  /* Similarly, check for cases like 1<=i<=10 that are probably errors.  */
  if (TREE_CODE_CLASS (code) == '<' && extra_warnings
      && (TREE_CODE_CLASS (code1) == '<' || TREE_CODE_CLASS (code2) == '<'))
    warning ("comparisons like X<=Y<=Z do not have their mathematical meaning");

  unsigned_conversion_warning (result, arg1);
  unsigned_conversion_warning (result, arg2);
  overflow_warning (result);

  class = TREE_CODE_CLASS (TREE_CODE (result));

  /* Record the code that was specified in the source,
     for the sake of warnings about confusing nesting.  */
  if (IS_EXPR_CODE_CLASS (class))
    C_SET_EXP_ORIGINAL_CODE (result, code);
  else
    {
      int flag = TREE_CONSTANT (result);
      /* We used to use NOP_EXPR rather than NON_LVALUE_EXPR
	 so that convert_for_assignment wouldn't strip it.
	 That way, we got warnings for things like p = (1 - 1).
	 But it turns out we should not get those warnings.  */
      result = build1 (NON_LVALUE_EXPR, TREE_TYPE (result), result);
      C_SET_EXP_ORIGINAL_CODE (result, code);
      TREE_CONSTANT (result) = flag;
    }

  return result;
}

/* Build a binary-operation expression without default conversions.
   CODE is the kind of expression to build.
   This function differs from `build' in several ways:
   the data type of the result is computed and recorded in it,
   warnings are generated if arg data types are invalid,
   special handling for addition and subtraction of pointers is known,
   and some optimization is done (operations on narrow ints
   are done in the narrower type when that gives the same result).
   Constant folding is also done before the result is returned.

   Note that the operands will never have enumeral types, or function
   or array types, because either they will have the default conversions
   performed or they have both just been converted to some other type in which
   the arithmetic is to be done.  */

tree
build_binary_op (code, orig_op0, orig_op1, convert_p)
     enum tree_code code;
     tree orig_op0, orig_op1;
     int convert_p;
{
  tree type0, type1;
  enum tree_code code0, code1;
  tree op0, op1;

  /* Expression code to give to the expression when it is built.
     Normally this is CODE, which is what the caller asked for,
     but in some special cases we change it.  */
  enum tree_code resultcode = code;

  /* Data type in which the computation is to be performed.
     In the simplest cases this is the common type of the arguments.  */
  tree result_type = NULL;

  /* Nonzero means operands have already been type-converted
     in whatever way is necessary.
     Zero means they need to be converted to RESULT_TYPE.  */
  int converted = 0;

  /* Nonzero means create the expression with this type, rather than
     RESULT_TYPE.  */
  tree build_type = 0;

  /* Nonzero means after finally constructing the expression
     convert it to this type.  */
  tree final_type = 0;

  /* Nonzero if this is an operation like MIN or MAX which can
     safely be computed in short if both args are promoted shorts.
     Also implies COMMON.
     -1 indicates a bitwise operation; this makes a difference
     in the exact conditions for when it is safe to do the operation
     in a narrower mode.  */
  int shorten = 0;

  /* Nonzero if this is a comparison operation;
     if both args are promoted shorts, compare the original shorts.
     Also implies COMMON.  */
  int short_compare = 0;

  /* Nonzero if this is a right-shift operation, which can be computed on the
     original short and then promoted if the operand is a promoted short.  */
  int short_shift = 0;

  /* Nonzero means set RESULT_TYPE to the common type of the args.  */
  int common = 0;

  if (convert_p)
    {
      op0 = default_conversion (orig_op0);
      op1 = default_conversion (orig_op1);
    }
  else
    {
      op0 = orig_op0;
      op1 = orig_op1;
    }

  type0 = TREE_TYPE (op0);
  type1 = TREE_TYPE (op1);

  /* The expression codes of the data types of the arguments tell us
     whether the arguments are integers, floating, pointers, etc.  */
  code0 = TREE_CODE (type0);
  code1 = TREE_CODE (type1);

  /* Strip NON_LVALUE_EXPRs, etc., since we aren't using as an lvalue.  */
  STRIP_TYPE_NOPS (op0);
  STRIP_TYPE_NOPS (op1);

  /* If an error was already reported for one of the arguments,
     avoid reporting another error.  */

  if (code0 == ERROR_MARK || code1 == ERROR_MARK)
    return error_mark_node;

  switch (code)
    {
    case PLUS_EXPR:
      /* Handle the pointer + int case.  */
      if (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
	return pointer_int_sum (PLUS_EXPR, op0, op1);
      else if (code1 == POINTER_TYPE && code0 == INTEGER_TYPE)
	return pointer_int_sum (PLUS_EXPR, op1, op0);
      else
	common = 1;
      break;

    case MINUS_EXPR:
      /* Subtraction of two similar pointers.
	 We must subtract them as integers, then divide by object size.  */
      if (code0 == POINTER_TYPE && code1 == POINTER_TYPE
	  && comp_target_types (type0, type1))
	return pointer_diff (op0, op1);
      /* Handle pointer minus int.  Just like pointer plus int.  */
      else if (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
	return pointer_int_sum (MINUS_EXPR, op0, op1);
      else
	common = 1;
      break;

    case MULT_EXPR:
      common = 1;
      break;

    case TRUNC_DIV_EXPR:
    case CEIL_DIV_EXPR:
    case FLOOR_DIV_EXPR:
    case ROUND_DIV_EXPR:
    case EXACT_DIV_EXPR:
      /* Floating point division by zero is a legitimate way to obtain
	 infinities and NaNs.  */
      if (warn_div_by_zero && skip_evaluation == 0 && integer_zerop (op1))
	warning ("division by zero");

      if ((code0 == INTEGER_TYPE || code0 == REAL_TYPE
	   || code0 == COMPLEX_TYPE)
	  && (code1 == INTEGER_TYPE || code1 == REAL_TYPE
	      || code1 == COMPLEX_TYPE))
	{
	  if (!(code0 == INTEGER_TYPE && code1 == INTEGER_TYPE))
	    resultcode = RDIV_EXPR;
	  else
	    /* Although it would be tempting to shorten always here, that
	       loses on some targets, since the modulo instruction is
	       undefined if the quotient can't be represented in the
	       computation mode.  We shorten only if unsigned or if
	       dividing by something we know != -1.  */
	    shorten = (TREE_UNSIGNED (TREE_TYPE (orig_op0))
		       || (TREE_CODE (op1) == INTEGER_CST
			   && ! integer_all_onesp (op1)));
	  common = 1;
	}
      break;

    case BIT_AND_EXPR:
    case BIT_ANDTC_EXPR:
    case BIT_IOR_EXPR:
    case BIT_XOR_EXPR:
      if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
	shorten = -1;
      break;

    case TRUNC_MOD_EXPR:
    case FLOOR_MOD_EXPR:
      if (warn_div_by_zero && skip_evaluation == 0 && integer_zerop (op1))
	warning ("division by zero");

      if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
	{
	  /* Although it would be tempting to shorten always here, that loses
	     on some targets, since the modulo instruction is undefined if the
	     quotient can't be represented in the computation mode.  We shorten
	     only if unsigned or if dividing by something we know != -1.  */
	  shorten = (TREE_UNSIGNED (TREE_TYPE (orig_op0))
		     || (TREE_CODE (op1) == INTEGER_CST
			 && ! integer_all_onesp (op1)));
	  common = 1;
	}
      break;

    case TRUTH_ANDIF_EXPR:
    case TRUTH_ORIF_EXPR:
    case TRUTH_AND_EXPR:
    case TRUTH_OR_EXPR:
    case TRUTH_XOR_EXPR:
      if ((code0 == INTEGER_TYPE || code0 == POINTER_TYPE
	   || code0 == REAL_TYPE || code0 == COMPLEX_TYPE)
	  && (code1 == INTEGER_TYPE || code1 == POINTER_TYPE
	      || code1 == REAL_TYPE || code1 == COMPLEX_TYPE))
	{
	  /* Result of these operations is always an int,
	     but that does not mean the operands should be
	     converted to ints!  */
	  result_type = integer_type_node;
	  op0 = truthvalue_conversion (op0);
	  op1 = truthvalue_conversion (op1);
	  converted = 1;
	}
      break;

      /* Shift operations: result has same type as first operand;
	 always convert second operand to int.
	 Also set SHORT_SHIFT if shifting rightward.  */

    case RSHIFT_EXPR:
      if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
	{
	  if (TREE_CODE (op1) == INTEGER_CST && skip_evaluation == 0)
	    {
	      if (tree_int_cst_sgn (op1) < 0)
		warning ("right shift count is negative");
	      else
		{
		  if (! integer_zerop (op1))
		    short_shift = 1;

		  if (compare_tree_int (op1, TYPE_PRECISION (type0)) >= 0)
		    warning ("right shift count >= width of type");
		}
	    }

	  /* Use the type of the value to be shifted.
	     This is what most traditional C compilers do.  */
	  result_type = type0;
	  /* Unless traditional, convert the shift-count to an integer,
	     regardless of size of value being shifted.  */
	  if (! flag_traditional)
	    {
	      if (TYPE_MAIN_VARIANT (TREE_TYPE (op1)) != integer_type_node)
		op1 = convert (integer_type_node, op1);
	      /* Avoid converting op1 to result_type later.  */
	      converted = 1;
	    }
	}
      break;

    case LSHIFT_EXPR:
      if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
	{
	  if (TREE_CODE (op1) == INTEGER_CST && skip_evaluation == 0)
	    {
	      if (tree_int_cst_sgn (op1) < 0)
		warning ("left shift count is negative");

	      else if (compare_tree_int (op1, TYPE_PRECISION (type0)) >= 0)
		warning ("left shift count >= width of type");
	    }

	  /* Use the type of the value to be shifted.
	     This is what most traditional C compilers do.  */
	  result_type = type0;
	  /* Unless traditional, convert the shift-count to an integer,
	     regardless of size of value being shifted.  */
	  if (! flag_traditional)
	    {
	      if (TYPE_MAIN_VARIANT (TREE_TYPE (op1)) != integer_type_node)
		op1 = convert (integer_type_node, op1);
	      /* Avoid converting op1 to result_type later.  */
	      converted = 1;
	    }
	}
      break;

    case RROTATE_EXPR:
    case LROTATE_EXPR:
      if (code0 == INTEGER_TYPE && code1 == INTEGER_TYPE)
	{
	  if (TREE_CODE (op1) == INTEGER_CST && skip_evaluation == 0)
	    {
	      if (tree_int_cst_sgn (op1) < 0)
		warning ("shift count is negative");
	      else if (compare_tree_int (op1, TYPE_PRECISION (type0)) >= 0)
		warning ("shift count >= width of type");
	    }

	  /* Use the type of the value to be shifted.
	     This is what most traditional C compilers do.  */
	  result_type = type0;
	  /* Unless traditional, convert the shift-count to an integer,
	     regardless of size of value being shifted.  */
	  if (! flag_traditional)
	    {
	      if (TYPE_MAIN_VARIANT (TREE_TYPE (op1)) != integer_type_node)
		op1 = convert (integer_type_node, op1);
	      /* Avoid converting op1 to result_type later.  */
	      converted = 1;
	    }
	}
      break;

    case EQ_EXPR:
    case NE_EXPR:
      if (warn_float_equal && (code0 == REAL_TYPE || code1 == REAL_TYPE))
	warning ("comparing floating point with == or != is unsafe");
      /* Result of comparison is always int,
	 but don't convert the args to int!  */
      build_type = integer_type_node;
      if ((code0 == INTEGER_TYPE || code0 == REAL_TYPE
	   || code0 == COMPLEX_TYPE)
	  && (code1 == INTEGER_TYPE || code1 == REAL_TYPE
	      || code1 == COMPLEX_TYPE))
	short_compare = 1;
      else if (code0 == POINTER_TYPE && code1 == POINTER_TYPE)
	{
	  tree tt0 = TREE_TYPE (type0);
	  tree tt1 = TREE_TYPE (type1);
	  /* Anything compares with void *.  void * compares with anything.
	     Otherwise, the targets must be compatible
	     and both must be object or both incomplete.  */
	  if (comp_target_types (type0, type1))
	    result_type = common_type (type0, type1);
	  else if (VOID_TYPE_P (tt0))
	    {
	      /* op0 != orig_op0 detects the case of something
		 whose value is 0 but which isn't a valid null ptr const.  */
	      if (pedantic && (!integer_zerop (op0) || op0 != orig_op0)
		  && TREE_CODE (tt1) == FUNCTION_TYPE)
		pedwarn ("ISO C forbids comparison of `void *' with function pointer");
	    }
	  else if (VOID_TYPE_P (tt1))
	    {
	      if (pedantic && (!integer_zerop (op1) || op1 != orig_op1)
		  && TREE_CODE (tt0) == FUNCTION_TYPE)
		pedwarn ("ISO C forbids comparison of `void *' with function pointer");
	    }
	  else
	    pedwarn ("comparison of distinct pointer types lacks a cast");

	  if (result_type == NULL_TREE)
	    result_type = ptr_type_node;
	}
      else if (code0 == POINTER_TYPE && TREE_CODE (op1) == INTEGER_CST
	       && integer_zerop (op1))
	result_type = type0;
      else if (code1 == POINTER_TYPE && TREE_CODE (op0) == INTEGER_CST
	       && integer_zerop (op0))
	result_type = type1;
      else if (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
	{
	  result_type = type0;
	  if (! flag_traditional)
	    pedwarn ("comparison between pointer and integer");
	}
      else if (code0 == INTEGER_TYPE && code1 == POINTER_TYPE)
	{
	  result_type = type1;
	  if (! flag_traditional)
	    pedwarn ("comparison between pointer and integer");
	}
      break;

    case MAX_EXPR:
    case MIN_EXPR:
      if ((code0 == INTEGER_TYPE || code0 == REAL_TYPE)
	  && (code1 == INTEGER_TYPE || code1 == REAL_TYPE))
	shorten = 1;
      else if (code0 == POINTER_TYPE && code1 == POINTER_TYPE)
	{
	  if (comp_target_types (type0, type1))
	    {
	      result_type = common_type (type0, type1);
	      if (pedantic 
		  && TREE_CODE (TREE_TYPE (type0)) == FUNCTION_TYPE)
		pedwarn ("ISO C forbids ordered comparisons of pointers to functions");
	    }
	  else
	    {
	      result_type = ptr_type_node;
	      pedwarn ("comparison of distinct pointer types lacks a cast");
	    }
	}
      break;

    case LE_EXPR:
    case GE_EXPR:
    case LT_EXPR:
    case GT_EXPR:
      build_type = integer_type_node;
      if ((code0 == INTEGER_TYPE || code0 == REAL_TYPE)
	  && (code1 == INTEGER_TYPE || code1 == REAL_TYPE))
	short_compare = 1;
      else if (code0 == POINTER_TYPE && code1 == POINTER_TYPE)
	{
	  if (comp_target_types (type0, type1))
	    {
	      result_type = common_type (type0, type1);
	      if (!COMPLETE_TYPE_P (TREE_TYPE (type0))
		  != !COMPLETE_TYPE_P (TREE_TYPE (type1)))
		pedwarn ("comparison of complete and incomplete pointers");
	      else if (pedantic 
		       && TREE_CODE (TREE_TYPE (type0)) == FUNCTION_TYPE)
		pedwarn ("ISO C forbids ordered comparisons of pointers to functions");
	    }
	  else
	    {
	      result_type = ptr_type_node;
	      pedwarn ("comparison of distinct pointer types lacks a cast");
	    }
	}
      else if (code0 == POINTER_TYPE && TREE_CODE (op1) == INTEGER_CST
	       && integer_zerop (op1))
	{
	  result_type = type0;
	  if (pedantic || extra_warnings)
	    pedwarn ("ordered comparison of pointer with integer zero");
	}
      else if (code1 == POINTER_TYPE && TREE_CODE (op0) == INTEGER_CST
	       && integer_zerop (op0))
	{
	  result_type = type1;
	  if (pedantic)
	    pedwarn ("ordered comparison of pointer with integer zero");
	}
      else if (code0 == POINTER_TYPE && code1 == INTEGER_TYPE)
	{
	  result_type = type0;
	  if (! flag_traditional)
	    pedwarn ("comparison between pointer and integer");
	}
      else if (code0 == INTEGER_TYPE && code1 == POINTER_TYPE)
	{
	  result_type = type1;
	  if (! flag_traditional)
	    pedwarn ("comparison between pointer and integer");
	}
      break;

    case UNORDERED_EXPR:
    case ORDERED_EXPR:
    case UNLT_EXPR:
    case UNLE_EXPR:
    case UNGT_EXPR:
    case UNGE_EXPR:
    case UNEQ_EXPR:
      build_type = integer_type_node;
      if (code0 != REAL_TYPE || code1 != REAL_TYPE)
	{
	  error ("unordered comparison on non-floating point argument");
	  return error_mark_node;
	}
      common = 1;
      break;

    default:
      break;
    }

  if ((code0 == INTEGER_TYPE || code0 == REAL_TYPE || code0 == COMPLEX_TYPE)
      &&
      (code1 == INTEGER_TYPE || code1 == REAL_TYPE || code1 == COMPLEX_TYPE))
    {
      int none_complex = (code0 != COMPLEX_TYPE && code1 != COMPLEX_TYPE);

      if (shorten || common || short_compare)
	result_type = common_type (type0, type1);

      /* For certain operations (which identify themselves by shorten != 0)
	 if both args were extended from the same smaller type,
	 do the arithmetic in that type and then extend.

	 shorten !=0 and !=1 indicates a bitwise operation.
	 For them, this optimization is safe only if
	 both args are zero-extended or both are sign-extended.
	 Otherwise, we might change the result.
	 Eg, (short)-1 | (unsigned short)-1 is (int)-1
	 but calculated in (unsigned short) it would be (unsigned short)-1.  */

      if (shorten && none_complex)
	{
	  int unsigned0, unsigned1;
	  tree arg0 = get_narrower (op0, &unsigned0);
	  tree arg1 = get_narrower (op1, &unsigned1);
	  /* UNS is 1 if the operation to be done is an unsigned one.  */
	  int uns = TREE_UNSIGNED (result_type);
	  tree type;

	  final_type = result_type;

	  /* Handle the case that OP0 (or OP1) does not *contain* a conversion
	     but it *requires* conversion to FINAL_TYPE.  */

	  if ((TYPE_PRECISION (TREE_TYPE (op0))
	       == TYPE_PRECISION (TREE_TYPE (arg0)))
	      && TREE_TYPE (op0) != final_type)
	    unsigned0 = TREE_UNSIGNED (TREE_TYPE (op0));
	  if ((TYPE_PRECISION (TREE_TYPE (op1))
	       == TYPE_PRECISION (TREE_TYPE (arg1)))
	      && TREE_TYPE (op1) != final_type)
	    unsigned1 = TREE_UNSIGNED (TREE_TYPE (op1));

	  /* Now UNSIGNED0 is 1 if ARG0 zero-extends to FINAL_TYPE.  */

	  /* For bitwise operations, signedness of nominal type
	     does not matter.  Consider only how operands were extended.  */
	  if (shorten == -1)
	    uns = unsigned0;

	  /* Note that in all three cases below we refrain from optimizing
	     an unsigned operation on sign-extended args.
	     That would not be valid.  */

	  /* Both args variable: if both extended in same way
	     from same width, do it in that width.
	     Do it unsigned if args were zero-extended.  */
	  if ((TYPE_PRECISION (TREE_TYPE (arg0))
	       < TYPE_PRECISION (result_type))
	      && (TYPE_PRECISION (TREE_TYPE (arg1))
		  == TYPE_PRECISION (TREE_TYPE (arg0)))
	      && unsigned0 == unsigned1
	      && (unsigned0 || !uns))
	    result_type
	      = signed_or_unsigned_type (unsigned0,
					 common_type (TREE_TYPE (arg0), TREE_TYPE (arg1)));
	  else if (TREE_CODE (arg0) == INTEGER_CST
		   && (unsigned1 || !uns)
		   && (TYPE_PRECISION (TREE_TYPE (arg1))
		       < TYPE_PRECISION (result_type))
		   && (type = signed_or_unsigned_type (unsigned1,
						       TREE_TYPE (arg1)),
		       int_fits_type_p (arg0, type)))
	    result_type = type;
	  else if (TREE_CODE (arg1) == INTEGER_CST
		   && (unsigned0 || !uns)
		   && (TYPE_PRECISION (TREE_TYPE (arg0))
		       < TYPE_PRECISION (result_type))
		   && (type = signed_or_unsigned_type (unsigned0,
						       TREE_TYPE (arg0)),
		       int_fits_type_p (arg1, type)))
	    result_type = type;
	}

      /* Shifts can be shortened if shifting right.  */

      if (short_shift)
	{
	  int unsigned_arg;
	  tree arg0 = get_narrower (op0, &unsigned_arg);

	  final_type = result_type;

	  if (arg0 == op0 && final_type == TREE_TYPE (op0))
	    unsigned_arg = TREE_UNSIGNED (TREE_TYPE (op0));

	  if (TYPE_PRECISION (TREE_TYPE (arg0)) < TYPE_PRECISION (result_type)
	      /* We can shorten only if the shift count is less than the
		 number of bits in the smaller type size.  */
	      && compare_tree_int (op1, TYPE_PRECISION (TREE_TYPE (arg0))) < 0
	      /* We cannot drop an unsigned shift after sign-extension.  */
	      && (!TREE_UNSIGNED (final_type) || unsigned_arg))
	    {
	      /* Do an unsigned shift if the operand was zero-extended.  */
	      result_type
		= signed_or_unsigned_type (unsigned_arg, TREE_TYPE (arg0));
	      /* Convert value-to-be-shifted to that type.  */
	      if (TREE_TYPE (op0) != result_type)
		op0 = convert (result_type, op0);
	      converted = 1;
	    }
	}

      /* Comparison operations are shortened too but differently.
	 They identify themselves by setting short_compare = 1.  */

      if (short_compare)
	{
	  /* Don't write &op0, etc., because that would prevent op0
	     from being kept in a register.
	     Instead, make copies of the our local variables and
	     pass the copies by reference, then copy them back afterward.  */
	  tree xop0 = op0, xop1 = op1, xresult_type = result_type;
	  enum tree_code xresultcode = resultcode;
	  tree val 
	    = shorten_compare (&xop0, &xop1, &xresult_type, &xresultcode);

	  if (val != 0)
	    return val;

	  op0 = xop0, op1 = xop1;
	  converted = 1;
	  resultcode = xresultcode;

	  if ((warn_sign_compare < 0 ? extra_warnings : warn_sign_compare != 0)
	      && skip_evaluation == 0)
	    {
	      int op0_signed = ! TREE_UNSIGNED (TREE_TYPE (orig_op0));
	      int op1_signed = ! TREE_UNSIGNED (TREE_TYPE (orig_op1));
	      int unsignedp0, unsignedp1;
	      tree primop0 = get_narrower (op0, &unsignedp0);
	      tree primop1 = get_narrower (op1, &unsignedp1);

	      xop0 = orig_op0;
	      xop1 = orig_op1;
	      STRIP_TYPE_NOPS (xop0);
	      STRIP_TYPE_NOPS (xop1);

	      /* Give warnings for comparisons between signed and unsigned
		 quantities that may fail. 

		 Do the checking based on the original operand trees, so that
		 casts will be considered, but default promotions won't be.

		 Do not warn if the comparison is being done in a signed type,
		 since the signed type will only be chosen if it can represent
		 all the values of the unsigned type.  */
	      if (! TREE_UNSIGNED (result_type))
		/* OK */;
              /* Do not warn if both operands are the same signedness.  */
              else if (op0_signed == op1_signed)
                /* OK */;
	      else
		{
		  tree sop, uop;

		  if (op0_signed)
		    sop = xop0, uop = xop1;
		  else
		    sop = xop1, uop = xop0;

		  /* Do not warn if the signed quantity is an
		     unsuffixed integer literal (or some static
		     constant expression involving such literals or a
		     conditional expression involving such literals)
		     and it is non-negative.  */
		  if (tree_expr_nonnegative_p (sop))
		    /* OK */;
		  /* Do not warn if the comparison is an equality operation,
		     the unsigned quantity is an integral constant, and it
		     would fit in the result if the result were signed.  */
		  else if (TREE_CODE (uop) == INTEGER_CST
			   && (resultcode == EQ_EXPR || resultcode == NE_EXPR)
			   && int_fits_type_p (uop, signed_type (result_type)))
		    /* OK */;
		  /* Do not warn if the unsigned quantity is an enumeration
		     constant and its maximum value would fit in the result
		     if the result were signed.  */
		  else if (TREE_CODE (uop) == INTEGER_CST
			   && TREE_CODE (TREE_TYPE (uop)) == ENUMERAL_TYPE
			   && int_fits_type_p (TYPE_MAX_VALUE (TREE_TYPE(uop)),
					       signed_type (result_type)))
		    /* OK */;
		  else
		    warning ("comparison between signed and unsigned");
		}

	      /* Warn if two unsigned values are being compared in a size
		 larger than their original size, and one (and only one) is the
		 result of a `~' operator.  This comparison will always fail.

		 Also warn if one operand is a constant, and the constant
		 does not have all bits set that are set in the ~ operand
		 when it is extended.  */

	      if ((TREE_CODE (primop0) == BIT_NOT_EXPR)
		  != (TREE_CODE (primop1) == BIT_NOT_EXPR))
		{
		  if (TREE_CODE (primop0) == BIT_NOT_EXPR)
		    primop0 = get_narrower (TREE_OPERAND (primop0, 0),
					    &unsignedp0);
		  else
		    primop1 = get_narrower (TREE_OPERAND (primop1, 0),
					    &unsignedp1);
	      
		  if (host_integerp (primop0, 0) || host_integerp (primop1, 0))
		    {
		      tree primop;
		      HOST_WIDE_INT constant, mask;
		      int unsignedp, bits;

		      if (host_integerp (primop0, 0))
			{
			  primop = primop1;
			  unsignedp = unsignedp1;
			  constant = tree_low_cst (primop0, 0);
			}
		      else
			{
			  primop = primop0;
			  unsignedp = unsignedp0;
			  constant = tree_low_cst (primop1, 0);
			}

		      bits = TYPE_PRECISION (TREE_TYPE (primop));
		      if (bits < TYPE_PRECISION (result_type)
			  && bits < HOST_BITS_PER_WIDE_INT && unsignedp)
			{
			  mask = (~ (HOST_WIDE_INT) 0) << bits;
			  if ((mask & constant) != mask)
			    warning ("comparison of promoted ~unsigned with constant");
			}
		    }
		  else if (unsignedp0 && unsignedp1
			   && (TYPE_PRECISION (TREE_TYPE (primop0))
			       < TYPE_PRECISION (result_type))
			   && (TYPE_PRECISION (TREE_TYPE (primop1))
			       < TYPE_PRECISION (result_type)))
		    warning ("comparison of promoted ~unsigned with unsigned");
		}
	    }
	}
    }

  /* At this point, RESULT_TYPE must be nonzero to avoid an error message.
     If CONVERTED is zero, both args will be converted to type RESULT_TYPE.
     Then the expression will be built.
     It will be given type FINAL_TYPE if that is nonzero;
     otherwise, it will be given type RESULT_TYPE.  */

  if (!result_type)
    {
      binary_op_error (code);
      return error_mark_node;
    }

  if (! converted)
    {
      if (TREE_TYPE (op0) != result_type)
	op0 = convert (result_type, op0); 
      if (TREE_TYPE (op1) != result_type)
	op1 = convert (result_type, op1); 
    }

  if (build_type == NULL_TREE)
    build_type = result_type;

  {
    tree result = build (resultcode, build_type, op0, op1);
    tree folded;

    folded = fold (result);
    if (folded == result)
      TREE_CONSTANT (folded) = TREE_CONSTANT (op0) & TREE_CONSTANT (op1);
    if (final_type != 0)
      return convert (final_type, folded);
    return folded;
  }
}

/* Return a tree for the difference of pointers OP0 and OP1.
   The resulting tree has type int.  */

static tree
pointer_diff (op0, op1)
     tree op0, op1;
{
  tree result, folded;
  tree restype = ptrdiff_type_node;

  tree target_type = TREE_TYPE (TREE_TYPE (op0));
  tree con0, con1, lit0, lit1;
  tree orig_op1 = op1;

  if (pedantic || warn_pointer_arith)
    {
      if (TREE_CODE (target_type) == VOID_TYPE)
	pedwarn ("pointer of type `void *' used in subtraction");
      if (TREE_CODE (target_type) == FUNCTION_TYPE)
	pedwarn ("pointer to a function used in subtraction");
    }

  /* If the conversion to ptrdiff_type does anything like widening or
     converting a partial to an integral mode, we get a convert_expression
     that is in the way to do any simplifications.
     (fold-const.c doesn't know that the extra bits won't be needed.
     split_tree uses STRIP_SIGN_NOPS, which leaves conversions to a
     different mode in place.)
     So first try to find a common term here 'by hand'; we want to cover
     at least the cases that occur in legal static initializers.  */
  con0 = TREE_CODE (op0) == NOP_EXPR ? TREE_OPERAND (op0, 0) : op0;
  con1 = TREE_CODE (op1) == NOP_EXPR ? TREE_OPERAND (op1, 0) : op1;

  if (TREE_CODE (con0) == PLUS_EXPR)
    {
      lit0 = TREE_OPERAND (con0, 1);
      con0 = TREE_OPERAND (con0, 0);
    }
  else
    lit0 = integer_zero_node;

  if (TREE_CODE (con1) == PLUS_EXPR)
    {
      lit1 = TREE_OPERAND (con1, 1);
      con1 = TREE_OPERAND (con1, 0);
    }
  else
    lit1 = integer_zero_node;

  if (operand_equal_p (con0, con1, 0))
    {
      op0 = lit0;
      op1 = lit1;
    }


  /* First do the subtraction as integers;
     then drop through to build the divide operator.
     Do not do default conversions on the minus operator
     in case restype is a short type.  */

  op0 = build_binary_op (MINUS_EXPR, convert (restype, op0),
			 convert (restype, op1), 0);
  /* This generates an error if op1 is pointer to incomplete type.  */
  if (!COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (TREE_TYPE (orig_op1))))
    error ("arithmetic on pointer to an incomplete type");

  /* This generates an error if op0 is pointer to incomplete type.  */
  op1 = c_size_in_bytes (target_type);

  /* Divide by the size, in easiest possible way.  */

  result = build (EXACT_DIV_EXPR, restype, op0, convert (restype, op1));

  folded = fold (result);
  if (folded == result)
    TREE_CONSTANT (folded) = TREE_CONSTANT (op0) & TREE_CONSTANT (op1);
  return folded;
}

/* Construct and perhaps optimize a tree representation
   for a unary operation.  CODE, a tree_code, specifies the operation
   and XARG is the operand.
   For any CODE other than ADDR_EXPR, FLAG nonzero suppresses
   the default promotions (such as from short to int).
   For ADDR_EXPR, the default promotions are not applied; FLAG nonzero
   allows non-lvalues; this is only used to handle conversion of non-lvalue
   arrays to pointers in C99.  */

tree
build_unary_op (code, xarg, flag)
     enum tree_code code;
     tree xarg;
     int flag;
{
  /* No default_conversion here.  It causes trouble for ADDR_EXPR.  */
  tree arg = xarg;
  tree argtype = 0;
  enum tree_code typecode = TREE_CODE (TREE_TYPE (arg));
  tree val;
  int noconvert = flag;

  if (typecode == ERROR_MARK)
    return error_mark_node;
  if (typecode == ENUMERAL_TYPE || typecode == BOOLEAN_TYPE)
    typecode = INTEGER_TYPE;

  switch (code)
    {
    case CONVERT_EXPR:
      /* This is used for unary plus, because a CONVERT_EXPR
	 is enough to prevent anybody from looking inside for
	 associativity, but won't generate any code.  */
      if (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE
	    || typecode == COMPLEX_TYPE))
	{
	  error ("wrong type argument to unary plus");
	  return error_mark_node;
	}
      else if (!noconvert)
	arg = default_conversion (arg);
      break;

    case NEGATE_EXPR:
      if (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE
	    || typecode == COMPLEX_TYPE))
	{
	  error ("wrong type argument to unary minus");
	  return error_mark_node;
	}
      else if (!noconvert)
	arg = default_conversion (arg);
      break;

    case BIT_NOT_EXPR:
      if (typecode == COMPLEX_TYPE)
	{
	  code = CONJ_EXPR;
	  if (pedantic)
	    pedwarn ("ISO C does not support `~' for complex conjugation");
	  if (!noconvert)
	    arg = default_conversion (arg);
	}
      else if (typecode != INTEGER_TYPE)
	{
	  error ("wrong type argument to bit-complement");
	  return error_mark_node;
	}
      else if (!noconvert)
	arg = default_conversion (arg);
      break;

    case ABS_EXPR:
      if (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE
	    || typecode == COMPLEX_TYPE))
	{
	  error ("wrong type argument to abs");
	  return error_mark_node;
	}
      else if (!noconvert)
	arg = default_conversion (arg);
      break;

    case CONJ_EXPR:
      /* Conjugating a real value is a no-op, but allow it anyway.  */
      if (!(typecode == INTEGER_TYPE || typecode == REAL_TYPE
	    || typecode == COMPLEX_TYPE))
	{
	  error ("wrong type argument to conjugation");
	  return error_mark_node;
	}
      else if (!noconvert)
	arg = default_conversion (arg);
      break;

    case TRUTH_NOT_EXPR:
      if (typecode != INTEGER_TYPE
	  && typecode != REAL_TYPE && typecode != POINTER_TYPE
	  && typecode != COMPLEX_TYPE
	  /* These will convert to a pointer.  */
	  && typecode != ARRAY_TYPE && typecode != FUNCTION_TYPE)
	{
	  error ("wrong type argument to unary exclamation mark");
	  return error_mark_node;
	}
      arg = truthvalue_conversion (arg);
      return invert_truthvalue (arg);

    case NOP_EXPR:
      break;

    case REALPART_EXPR:
      if (TREE_CODE (arg) == COMPLEX_CST)
	return TREE_REALPART (arg);
      else if (TREE_CODE (TREE_TYPE (arg)) == COMPLEX_TYPE)
	return fold (build1 (REALPART_EXPR, TREE_TYPE (TREE_TYPE (arg)), arg));
      else
	return arg;

    case IMAGPART_EXPR:
      if (TREE_CODE (arg) == COMPLEX_CST)
	return TREE_IMAGPART (arg);
      else if (TREE_CODE (TREE_TYPE (arg)) == COMPLEX_TYPE)
	return fold (build1 (IMAGPART_EXPR, TREE_TYPE (TREE_TYPE (arg)), arg));
      else
	return convert (TREE_TYPE (arg), integer_zero_node);
      
    case PREINCREMENT_EXPR:
    case POSTINCREMENT_EXPR:
    case PREDECREMENT_EXPR:
    case POSTDECREMENT_EXPR:
      /* Handle complex lvalues (when permitted)
	 by reduction to simpler cases.  */

      val = unary_complex_lvalue (code, arg, 0);
      if (val != 0)
	return val;

      /* Increment or decrement the real part of the value,
	 and don't change the imaginary part.  */
      if (typecode == COMPLEX_TYPE)
	{
	  tree real, imag;

	  if (pedantic)
	    pedwarn ("ISO C does not support `++' and `--' on complex types");

	  arg = stabilize_reference (arg);
	  real = build_unary_op (REALPART_EXPR, arg, 1);
	  imag = build_unary_op (IMAGPART_EXPR, arg, 1);
	  return build (COMPLEX_EXPR, TREE_TYPE (arg),
			build_unary_op (code, real, 1), imag);
	}

      /* Report invalid types.  */

      if (typecode != POINTER_TYPE
	  && typecode != INTEGER_TYPE && typecode != REAL_TYPE)
	{
	  if (code == PREINCREMENT_EXPR || code == POSTINCREMENT_EXPR)
            error ("wrong type argument to increment");
          else
            error ("wrong type argument to decrement");

	  return error_mark_node;
	}

      {
	tree inc;
	tree result_type = TREE_TYPE (arg);

	arg = get_unwidened (arg, 0);
	argtype = TREE_TYPE (arg);

	/* Compute the increment.  */

	if (typecode == POINTER_TYPE)
	  {
	    /* If pointer target is an undefined struct,
	       we just cannot know how to do the arithmetic.  */
	    if (!COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (result_type)))
	      {
		if (code == PREINCREMENT_EXPR || code == POSTINCREMENT_EXPR)
		  error ("increment of pointer to unknown structure");
		else
		  error ("decrement of pointer to unknown structure");
	      }
	    else if ((pedantic || warn_pointer_arith)
		     && (TREE_CODE (TREE_TYPE (result_type)) == FUNCTION_TYPE
			 || TREE_CODE (TREE_TYPE (result_type)) == VOID_TYPE))
              {
		if (code == PREINCREMENT_EXPR || code == POSTINCREMENT_EXPR)
		  pedwarn ("wrong type argument to increment");
		else
		  pedwarn ("wrong type argument to decrement");
	      }

	    inc = c_size_in_bytes (TREE_TYPE (result_type));
	  }
	else
	  inc = integer_one_node;

	inc = convert (argtype, inc);

	/* Handle incrementing a cast-expression.  */

	while (1)
	  switch (TREE_CODE (arg))
	    {
	    case NOP_EXPR:
	    case CONVERT_EXPR:
	    case FLOAT_EXPR:
	    case FIX_TRUNC_EXPR:
	    case FIX_FLOOR_EXPR:
	    case FIX_ROUND_EXPR:
	    case FIX_CEIL_EXPR:
	      pedantic_lvalue_warning (CONVERT_EXPR);
	      /* If the real type has the same machine representation
		 as the type it is cast to, we can make better output
		 by adding directly to the inside of the cast.  */
	      if ((TREE_CODE (TREE_TYPE (arg))
		   == TREE_CODE (TREE_TYPE (TREE_OPERAND (arg, 0))))
		  && (TYPE_MODE (TREE_TYPE (arg))
		      == TYPE_MODE (TREE_TYPE (TREE_OPERAND (arg, 0)))))
		arg = TREE_OPERAND (arg, 0);
	      else
		{
		  tree incremented, modify, value;
		  if (TREE_CODE (TREE_TYPE (arg)) == BOOLEAN_TYPE)
		    value = boolean_increment (code, arg);
		  else
		    {
		      arg = stabilize_reference (arg);
		      if (code == PREINCREMENT_EXPR || code == PREDECREMENT_EXPR)
			value = arg;
		      else
			value = save_expr (arg);
		      incremented = build (((code == PREINCREMENT_EXPR
					     || code == POSTINCREMENT_EXPR)
					    ? PLUS_EXPR : MINUS_EXPR),
					   argtype, value, inc);
		      TREE_SIDE_EFFECTS (incremented) = 1;
		      modify = build_modify_expr (arg, NOP_EXPR, incremented);
		      value = build (COMPOUND_EXPR, TREE_TYPE (arg), modify, value);
		    }
		  TREE_USED (value) = 1;
		  return value;
		}
	      break;

	    default:
	      goto give_up;
	    }
      give_up:

	/* Complain about anything else that is not a true lvalue.  */
	if (!lvalue_or_else (arg, ((code == PREINCREMENT_EXPR
				    || code == POSTINCREMENT_EXPR)
				   ? "invalid lvalue in increment"
				   : "invalid lvalue in decrement")))
	  return error_mark_node;

	/* Report a read-only lvalue.  */
	if (TREE_READONLY (arg))
	  readonly_warning (arg, 
			    ((code == PREINCREMENT_EXPR
			      || code == POSTINCREMENT_EXPR)
			     ? "increment" : "decrement"));

	if (TREE_CODE (TREE_TYPE (arg)) == BOOLEAN_TYPE)
	  val = boolean_increment (code, arg);
	else
	  val = build (code, TREE_TYPE (arg), arg, inc);
	TREE_SIDE_EFFECTS (val) = 1;
	val = convert (result_type, val);
	if (TREE_CODE (val) != code)
	  TREE_NO_UNUSED_WARNING (val) = 1;
	return val;
      }

    case ADDR_EXPR:
      /* Note that this operation never does default_conversion.  */

      /* Let &* cancel out to simplify resulting code.  */
      if (TREE_CODE (arg) == INDIRECT_REF)
	{
	  /* Don't let this be an lvalue.  */
	  if (lvalue_p (TREE_OPERAND (arg, 0)))
	    return non_lvalue (TREE_OPERAND (arg, 0));
	  return TREE_OPERAND (arg, 0);
	}

      /* For &x[y], return x+y */
      if (TREE_CODE (arg) == ARRAY_REF)
	{
	  if (mark_addressable (TREE_OPERAND (arg, 0)) == 0)
	    return error_mark_node;
	  return build_binary_op (PLUS_EXPR, TREE_OPERAND (arg, 0),
				  TREE_OPERAND (arg, 1), 1);
	}

      /* Handle complex lvalues (when permitted)
	 by reduction to simpler cases.  */
      val = unary_complex_lvalue (code, arg, flag);
      if (val != 0)
	return val;

#if 0 /* Turned off because inconsistent;
	 float f; *&(int)f = 3.4 stores in int format
	 whereas (int)f = 3.4 stores in float format.  */
      /* Address of a cast is just a cast of the address
	 of the operand of the cast.  */
      switch (TREE_CODE (arg))
	{
	case NOP_EXPR:
	case CONVERT_EXPR:
	case FLOAT_EXPR:
	case FIX_TRUNC_EXPR:
	case FIX_FLOOR_EXPR:
	case FIX_ROUND_EXPR:
	case FIX_CEIL_EXPR:
	  if (pedantic)
	    pedwarn ("ISO C forbids the address of a cast expression");
	  return convert (build_pointer_type (TREE_TYPE (arg)),
			  build_unary_op (ADDR_EXPR, TREE_OPERAND (arg, 0),
					  0));
	}
#endif

      /* Anything not already handled and not a true memory reference
	 or a non-lvalue array is an error.  */
      else if (typecode != FUNCTION_TYPE && !flag
	       && !lvalue_or_else (arg, "invalid lvalue in unary `&'"))
	return error_mark_node;

      /* Ordinary case; arg is a COMPONENT_REF or a decl.  */
      argtype = TREE_TYPE (arg);

      /* If the lvalue is const or volatile, merge that into the type
         to which the address will point.  Note that you can't get a
	 restricted pointer by taking the address of something, so we
	 only have to deal with `const' and `volatile' here.  */
      if ((DECL_P (arg) || TREE_CODE_CLASS (TREE_CODE (arg)) == 'r')
	  && (TREE_READONLY (arg) || TREE_THIS_VOLATILE (arg)))
	  argtype = c_build_type_variant (argtype,
					  TREE_READONLY (arg),
					  TREE_THIS_VOLATILE (arg));

      argtype = build_pointer_type (argtype);

      if (mark_addressable (arg) == 0)
	return error_mark_node;

      {
	tree addr;

	if (TREE_CODE (arg) == COMPONENT_REF)
	  {
	    tree field = TREE_OPERAND (arg, 1);

	    addr = build_unary_op (ADDR_EXPR, TREE_OPERAND (arg, 0), flag);

	    if (DECL_C_BIT_FIELD (field))
	      {
		error ("attempt to take address of bit-field structure member `%s'",
		       IDENTIFIER_POINTER (DECL_NAME (field)));
		return error_mark_node;
	      }

	    addr = fold (build (PLUS_EXPR, argtype,
				convert (argtype, addr),
				convert (argtype, byte_position (field))));
	  }
	else
	  addr = build1 (code, argtype, arg);

	/* Address of a static or external variable or
	   file-scope function counts as a constant.  */
	if (staticp (arg)
	    && ! (TREE_CODE (arg) == FUNCTION_DECL
		  && DECL_CONTEXT (arg) != 0))
	  TREE_CONSTANT (addr) = 1;
	return addr;
      }

    default:
      break;
    }

  if (argtype == 0)
    argtype = TREE_TYPE (arg);
  return fold (build1 (code, argtype, arg));
}

#if 0
/* If CONVERSIONS is a conversion expression or a nested sequence of such,
   convert ARG with the same conversions in the same order
   and return the result.  */

static tree
convert_sequence (conversions, arg)
     tree conversions;
     tree arg;
{
  switch (TREE_CODE (conversions))
    {
    case NOP_EXPR:
    case CONVERT_EXPR:
    case FLOAT_EXPR:
    case FIX_TRUNC_EXPR:
    case FIX_FLOOR_EXPR:
    case FIX_ROUND_EXPR:
    case FIX_CEIL_EXPR:
      return convert (TREE_TYPE (conversions),
		      convert_sequence (TREE_OPERAND (conversions, 0),
					arg));

    default:
      return arg;
    }
}
#endif /* 0 */

/* Return nonzero if REF is an lvalue valid for this language.
   Lvalues can be assigned, unless their type has TYPE_READONLY.
   Lvalues can have their address taken, unless they have DECL_REGISTER.  */

int
lvalue_p (ref)
     tree ref;
{
  enum tree_code code = TREE_CODE (ref);

  switch (code)
    {
    case REALPART_EXPR:
    case IMAGPART_EXPR:
    case COMPONENT_REF:
      return lvalue_p (TREE_OPERAND (ref, 0));

    case COMPOUND_LITERAL_EXPR:
    case STRING_CST:
      return 1;

    case INDIRECT_REF:
    case ARRAY_REF:
    case VAR_DECL:
    case PARM_DECL:
    case RESULT_DECL:
    case ERROR_MARK:
      return (TREE_CODE (TREE_TYPE (ref)) != FUNCTION_TYPE
	      && TREE_CODE (TREE_TYPE (ref)) != METHOD_TYPE);

    case BIND_EXPR:
    case RTL_EXPR:
      return TREE_CODE (TREE_TYPE (ref)) == ARRAY_TYPE;

    default:
      return 0;
    }
}

/* Return nonzero if REF is an lvalue valid for this language;
   otherwise, print an error message and return zero.  */

int
lvalue_or_else (ref, msgid)
     tree ref;
     const char *msgid;
{
  int win = lvalue_p (ref);

  if (! win)
    error ("%s", msgid);

  return win;
}

/* Apply unary lvalue-demanding operator CODE to the expression ARG
   for certain kinds of expressions which are not really lvalues
   but which we can accept as lvalues.  If FLAG is nonzero, then
   non-lvalues are OK since we may be converting a non-lvalue array to
   a pointer in C99.

   If ARG is not a kind of expression we can handle, return zero.  */
   
static tree
unary_complex_lvalue (code, arg, flag)
     enum tree_code code;
     tree arg;
     int flag;
{
  /* Handle (a, b) used as an "lvalue".  */
  if (TREE_CODE (arg) == COMPOUND_EXPR)
    {
      tree real_result = build_unary_op (code, TREE_OPERAND (arg, 1), 0);

      /* If this returns a function type, it isn't really being used as
	 an lvalue, so don't issue a warning about it.  */
      if (TREE_CODE (TREE_TYPE (arg)) != FUNCTION_TYPE && !flag)
	pedantic_lvalue_warning (COMPOUND_EXPR);

      return build (COMPOUND_EXPR, TREE_TYPE (real_result),
		    TREE_OPERAND (arg, 0), real_result);
    }

  /* Handle (a ? b : c) used as an "lvalue".  */
  if (TREE_CODE (arg) == COND_EXPR)
    {
      if (!flag)
	pedantic_lvalue_warning (COND_EXPR);
      if (TREE_CODE (TREE_TYPE (arg)) != FUNCTION_TYPE && !flag)
	pedantic_lvalue_warning (COMPOUND_EXPR);

      return (build_conditional_expr
	      (TREE_OPERAND (arg, 0),
	       build_unary_op (code, TREE_OPERAND (arg, 1), flag),
	       build_unary_op (code, TREE_OPERAND (arg, 2), flag)));
    }

  return 0;
}

/* If pedantic, warn about improper lvalue.   CODE is either COND_EXPR
   COMPOUND_EXPR, or CONVERT_EXPR (for casts).  */

static void
pedantic_lvalue_warning (code)
     enum tree_code code;
{
  if (pedantic)
    switch (code)
      {
      case COND_EXPR:
	pedwarn ("ISO C forbids use of conditional expressions as lvalues");
	break;
      case COMPOUND_EXPR:
	pedwarn ("ISO C forbids use of compound expressions as lvalues");
	break;
      default:
	pedwarn ("ISO C forbids use of cast expressions as lvalues");
	break;
      }
}

/* Warn about storing in something that is `const'.  */

void
readonly_warning (arg, msgid)
     tree arg;
     const char *msgid;
{
  if (TREE_CODE (arg) == COMPONENT_REF)
    {
      if (TYPE_READONLY (TREE_TYPE (TREE_OPERAND (arg, 0))))
	readonly_warning (TREE_OPERAND (arg, 0), msgid);
      else
	pedwarn ("%s of read-only member `%s'", _(msgid),
		 IDENTIFIER_POINTER (DECL_NAME (TREE_OPERAND (arg, 1))));
    }
  else if (TREE_CODE (arg) == VAR_DECL)
    pedwarn ("%s of read-only variable `%s'", _(msgid),
	     IDENTIFIER_POINTER (DECL_NAME (arg)));
  else
    pedwarn ("%s of read-only location", _(msgid));
}

/* Mark EXP saying that we need to be able to take the
   address of it; it should not be allocated in a register.
   Value is 1 if successful.  */

int
mark_addressable (exp)
     tree exp;
{
  tree x = exp;
  while (1)
    switch (TREE_CODE (x))
      {
      case COMPONENT_REF:
	if (DECL_C_BIT_FIELD (TREE_OPERAND (x, 1)))
	  {
	    error ("cannot take address of bit-field `%s'",
		   IDENTIFIER_POINTER (DECL_NAME (TREE_OPERAND (x, 1))));
	    return 0;
	  }

	/* ... fall through ...  */

      case ADDR_EXPR:
      case ARRAY_REF:
      case REALPART_EXPR:
      case IMAGPART_EXPR:
	x = TREE_OPERAND (x, 0);
	break;

      case COMPOUND_LITERAL_EXPR:
      case CONSTRUCTOR:
	TREE_ADDRESSABLE (x) = 1;
	return 1;

      case VAR_DECL:
      case CONST_DECL:
      case PARM_DECL:
      case RESULT_DECL:
	if (DECL_REGISTER (x) && !TREE_ADDRESSABLE (x)
	    && DECL_NONLOCAL (x))
	  {
	    if (TREE_PUBLIC (x))
	      {
		error ("global register variable `%s' used in nested function",
		       IDENTIFIER_POINTER (DECL_NAME (x)));
		return 0;
	      }
	    pedwarn ("register variable `%s' used in nested function",
		     IDENTIFIER_POINTER (DECL_NAME (x)));
	  }
	else if (DECL_REGISTER (x) && !TREE_ADDRESSABLE (x))
	  {
	    if (TREE_PUBLIC (x))
	      {
		error ("address of global register variable `%s' requested",
		       IDENTIFIER_POINTER (DECL_NAME (x)));
		return 0;
	      }

	    /* If we are making this addressable due to its having
	       volatile components, give a different error message.  Also
	       handle the case of an unnamed parameter by not trying
	       to give the name.  */

	    else if (C_TYPE_FIELDS_VOLATILE (TREE_TYPE (x)))
	      {
		error ("cannot put object with volatile field into register");
		return 0;
	      }

	    pedwarn ("address of register variable `%s' requested",
		     IDENTIFIER_POINTER (DECL_NAME (x)));
	  }
	put_var_into_stack (x);

	/* drops in */
      case FUNCTION_DECL:
	TREE_ADDRESSABLE (x) = 1;
#if 0  /* poplevel deals with this now.  */
	if (DECL_CONTEXT (x) == 0)
	  TREE_ADDRESSABLE (DECL_ASSEMBLER_NAME (x)) = 1;
#endif

      default:
	return 1;
    }
}

/* Build and return a conditional expression IFEXP ? OP1 : OP2.  */

tree
build_conditional_expr (ifexp, op1, op2)
     tree ifexp, op1, op2;
{
  tree type1;
  tree type2;
  enum tree_code code1;
  enum tree_code code2;
  tree result_type = NULL;
  tree orig_op1 = op1, orig_op2 = op2;

  ifexp = truthvalue_conversion (default_conversion (ifexp));

#if 0 /* Produces wrong result if within sizeof.  */
  /* Don't promote the operands separately if they promote
     the same way.  Return the unpromoted type and let the combined
     value get promoted if necessary.  */

  if (TREE_TYPE (op1) == TREE_TYPE (op2)
      && TREE_CODE (TREE_TYPE (op1)) != ARRAY_TYPE
      && TREE_CODE (TREE_TYPE (op1)) != ENUMERAL_TYPE
      && TREE_CODE (TREE_TYPE (op1)) != FUNCTION_TYPE)
    {
      if (TREE_CODE (ifexp) == INTEGER_CST)
	return pedantic_non_lvalue (integer_zerop (ifexp) ? op2 : op1);

      return fold (build (COND_EXPR, TREE_TYPE (op1), ifexp, op1, op2));
    }
#endif

  /* Promote both alternatives.  */

  if (TREE_CODE (TREE_TYPE (op1)) != VOID_TYPE)
    op1 = default_conversion (op1);
  if (TREE_CODE (TREE_TYPE (op2)) != VOID_TYPE)
    op2 = default_conversion (op2);

  if (TREE_CODE (ifexp) == ERROR_MARK
      || TREE_CODE (TREE_TYPE (op1)) == ERROR_MARK
      || TREE_CODE (TREE_TYPE (op2)) == ERROR_MARK)
    return error_mark_node;

  type1 = TREE_TYPE (op1);
  code1 = TREE_CODE (type1);
  type2 = TREE_TYPE (op2);
  code2 = TREE_CODE (type2);
      
  /* Quickly detect the usual case where op1 and op2 have the same type
     after promotion.  */
  if (TYPE_MAIN_VARIANT (type1) == TYPE_MAIN_VARIANT (type2))
    {
      if (type1 == type2)
	result_type = type1;
      else
	result_type = TYPE_MAIN_VARIANT (type1);
    }
  else if ((code1 == INTEGER_TYPE || code1 == REAL_TYPE
            || code1 == COMPLEX_TYPE)
           && (code2 == INTEGER_TYPE || code2 == REAL_TYPE
               || code2 == COMPLEX_TYPE))
    {
      result_type = common_type (type1, type2);

      /* If -Wsign-compare, warn here if type1 and type2 have
	 different signedness.  We'll promote the signed to unsigned
	 and later code won't know it used to be different.
	 Do this check on the original types, so that explicit casts
	 will be considered, but default promotions won't.  */
      if ((warn_sign_compare < 0 ? extra_warnings : warn_sign_compare)
	  && !skip_evaluation)
	{
	  int unsigned_op1 = TREE_UNSIGNED (TREE_TYPE (orig_op1));
	  int unsigned_op2 = TREE_UNSIGNED (TREE_TYPE (orig_op2));

	  if (unsigned_op1 ^ unsigned_op2)
	    {
	      /* Do not warn if the result type is signed, since the
		 signed type will only be chosen if it can represent
		 all the values of the unsigned type.  */
	      if (! TREE_UNSIGNED (result_type))
		/* OK */;
	      /* Do not warn if the signed quantity is an unsuffixed
		 integer literal (or some static constant expression
		 involving such literals) and it is non-negative.  */
	      else if ((unsigned_op2 && tree_expr_nonnegative_p (op1))
		       || (unsigned_op1 && tree_expr_nonnegative_p (op2)))
		/* OK */;
	      else
		warning ("signed and unsigned type in conditional expression");
	    }
	}
    }
  else if (code1 == VOID_TYPE || code2 == VOID_TYPE)
    {
      if (pedantic && (code1 != VOID_TYPE || code2 != VOID_TYPE))
	pedwarn ("ISO C forbids conditional expr with only one void side");
      result_type = void_type_node;
    }
  else if (code1 == POINTER_TYPE && code2 == POINTER_TYPE)
    {
      if (comp_target_types (type1, type2))
	result_type = common_type (type1, type2);
      else if (integer_zerop (op1) && TREE_TYPE (type1) == void_type_node
	       && TREE_CODE (orig_op1) != NOP_EXPR)
	result_type = qualify_type (type2, type1);
      else if (integer_zerop (op2) && TREE_TYPE (type2) == void_type_node
	       && TREE_CODE (orig_op2) != NOP_EXPR)
	result_type = qualify_type (type1, type2);
      else if (VOID_TYPE_P (TREE_TYPE (type1)))
	{
	  if (pedantic && TREE_CODE (TREE_TYPE (type2)) == FUNCTION_TYPE)
	    pedwarn ("ISO C forbids conditional expr between `void *' and function pointer");
	  result_type = build_pointer_type (qualify_type (TREE_TYPE (type1),
							  TREE_TYPE (type2)));
	}
      else if (VOID_TYPE_P (TREE_TYPE (type2)))
	{
	  if (pedantic && TREE_CODE (TREE_TYPE (type1)) == FUNCTION_TYPE)
	    pedwarn ("ISO C forbids conditional expr between `void *' and function pointer");
	  result_type = build_pointer_type (qualify_type (TREE_TYPE (type2),
							  TREE_TYPE (type1)));
	}
      else
	{
	  pedwarn ("pointer type mismatch in conditional expression");
	  result_type = build_pointer_type (void_type_node);
	}
    }
  else if (code1 == POINTER_TYPE && code2 == INTEGER_TYPE)
    {
      if (! integer_zerop (op2))
	pedwarn ("pointer/integer type mismatch in conditional expression");
      else
	{
	  op2 = null_pointer_node;
	}
      result_type = type1;
    }
  else if (code2 == POINTER_TYPE && code1 == INTEGER_TYPE)
    {
      if (!integer_zerop (op1))
	pedwarn ("pointer/integer type mismatch in conditional expression");
      else
	{
	  op1 = null_pointer_node;
	}
      result_type = type2;
    }

  if (!result_type)
    {
      if (flag_cond_mismatch)
	result_type = void_type_node;
      else
	{
	  error ("type mismatch in conditional expression");
	  return error_mark_node;
	}
    }

  /* Merge const and volatile flags of the incoming types.  */
  result_type
    = build_type_variant (result_type,
			  TREE_READONLY (op1) || TREE_READONLY (op2),
			  TREE_THIS_VOLATILE (op1) || TREE_THIS_VOLATILE (op2));

  if (result_type != TREE_TYPE (op1))
    op1 = convert_and_check (result_type, op1);
  if (result_type != TREE_TYPE (op2))
    op2 = convert_and_check (result_type, op2);
    
  if (TREE_CODE (ifexp) == INTEGER_CST)
    return pedantic_non_lvalue (integer_zerop (ifexp) ? op2 : op1);

  return fold (build (COND_EXPR, result_type, ifexp, op1, op2));
}

/* Given a list of expressions, return a compound expression
   that performs them all and returns the value of the last of them.  */

tree
build_compound_expr (list)
     tree list;
{
  return internal_build_compound_expr (list, TRUE);
}

static tree
internal_build_compound_expr (list, first_p)
     tree list;
     int first_p;
{
  tree rest;

  if (TREE_CHAIN (list) == 0)
    {
      /* Convert arrays and functions to pointers when there
	 really is a comma operator.  */
      if (!first_p)
	TREE_VALUE (list)
	  = default_function_array_conversion (TREE_VALUE (list));

#if 0 /* If something inside inhibited lvalueness, we should not override.  */
      /* Consider (x, y+0), which is not an lvalue since y+0 is not.  */

      /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
      if (TREE_CODE (list) == NON_LVALUE_EXPR)
	list = TREE_OPERAND (list, 0);
#endif

      /* Don't let (0, 0) be null pointer constant.  */
      if (!first_p && integer_zerop (TREE_VALUE (list)))
	return non_lvalue (TREE_VALUE (list));
      return TREE_VALUE (list);
    }

  rest = internal_build_compound_expr (TREE_CHAIN (list), FALSE);

  if (! TREE_SIDE_EFFECTS (TREE_VALUE (list)))
    {
      /* The left-hand operand of a comma expression is like an expression
         statement: with -W or -Wunused, we should warn if it doesn't have
	 any side-effects, unless it was explicitly cast to (void).  */
      if ((extra_warnings || warn_unused_value)
           && ! (TREE_CODE (TREE_VALUE (list)) == CONVERT_EXPR
                && VOID_TYPE_P (TREE_TYPE (TREE_VALUE (list)))))
        warning ("left-hand operand of comma expression has no effect");

      /* When pedantic, a compound expression can be neither an lvalue
         nor an integer constant expression.  */
      if (! pedantic)
        return rest;
    }

  /* With -Wunused, we should also warn if the left-hand operand does have
     side-effects, but computes a value which is not used.  For example, in
     `foo() + bar(), baz()' the result of the `+' operator is not used,
     so we should issue a warning.  */
  else if (warn_unused_value)
    warn_if_unused_value (TREE_VALUE (list));

  return build (COMPOUND_EXPR, TREE_TYPE (rest), TREE_VALUE (list), rest);
}

/* Build an expression representing a cast to type TYPE of expression EXPR.  */

tree
build_c_cast (type, expr)
     tree type;
     tree expr;
{
  tree value = expr;
  
  if (type == error_mark_node || expr == error_mark_node)
    return error_mark_node;
  type = TYPE_MAIN_VARIANT (type);

#if 0
  /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
  if (TREE_CODE (value) == NON_LVALUE_EXPR)
    value = TREE_OPERAND (value, 0);
#endif

  if (TREE_CODE (type) == ARRAY_TYPE)
    {
      error ("cast specifies array type");
      return error_mark_node;
    }

  if (TREE_CODE (type) == FUNCTION_TYPE)
    {
      error ("cast specifies function type");
      return error_mark_node;
    }

  if (type == TYPE_MAIN_VARIANT (TREE_TYPE (value)))
    {
      if (pedantic)
	{
	  if (TREE_CODE (type) == RECORD_TYPE
	      || TREE_CODE (type) == UNION_TYPE)
	    pedwarn ("ISO C forbids casting nonscalar to the same type");
	}
    }
  else if (TREE_CODE (type) == UNION_TYPE)
    {
      tree field;
      value = default_function_array_conversion (value);

      for (field = TYPE_FIELDS (type); field; field = TREE_CHAIN (field))
	if (comptypes (TYPE_MAIN_VARIANT (TREE_TYPE (field)),
		       TYPE_MAIN_VARIANT (TREE_TYPE (value))))
	  break;

      if (field)
	{
	  const char *name;
	  tree t;

	  if (pedantic)
	    pedwarn ("ISO C forbids casts to union type");
	  if (TYPE_NAME (type) != 0)
	    {
	      if (TREE_CODE (TYPE_NAME (type)) == IDENTIFIER_NODE)
		name = IDENTIFIER_POINTER (TYPE_NAME (type));
	      else
		name = IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (type)));
	    }
	  else
	    name = "";
	  t = digest_init (type, build (CONSTRUCTOR, type, NULL_TREE,
					build_tree_list (field, value)),
			   0, 0);
	  TREE_CONSTANT (t) = TREE_CONSTANT (value);
	  return t;
	}
      error ("cast to union type from type not present in union");
      return error_mark_node;
    }
  else
    {
      tree otype, ovalue;

      /* If casting to void, avoid the error that would come
	 from default_conversion in the case of a non-lvalue array.  */
      if (type == void_type_node)
	return build1 (CONVERT_EXPR, type, value);

      /* Convert functions and arrays to pointers,
	 but don't convert any other types.  */
      value = default_function_array_conversion (value);
      otype = TREE_TYPE (value);

      /* Optionally warn about potentially worrisome casts.  */

      if (warn_cast_qual
	  && TREE_CODE (type) == POINTER_TYPE
	  && TREE_CODE (otype) == POINTER_TYPE)
	{
	  tree in_type = type;
	  tree in_otype = otype;
	  int added = 0;
	  int discarded = 0;

	  /* Check that the qualifiers on IN_TYPE are a superset of
	     the qualifiers of IN_OTYPE.  The outermost level of
	     POINTER_TYPE nodes is uninteresting and we stop as soon
	     as we hit a non-POINTER_TYPE node on either type.  */
	  do
	    {
	      in_otype = TREE_TYPE (in_otype);
	      in_type = TREE_TYPE (in_type);

	      /* GNU C allows cv-qualified function types.  'const'
		 means the function is very pure, 'volatile' means it
		 can't return.  We need to warn when such qualifiers
		 are added, not when they're taken away.  */
	      if (TREE_CODE (in_otype) == FUNCTION_TYPE
		  && TREE_CODE (in_type) == FUNCTION_TYPE)
		added |= (TYPE_QUALS (in_type) & ~TYPE_QUALS (in_otype));
	      else
		discarded |= (TYPE_QUALS (in_otype) & ~TYPE_QUALS (in_type));
	    }
	  while (TREE_CODE (in_type) == POINTER_TYPE
		 && TREE_CODE (in_otype) == POINTER_TYPE);

	  if (added)
	    warning ("cast adds new qualifiers to function type");

	  if (discarded)
	    /* There are qualifiers present in IN_OTYPE that are not
	       present in IN_TYPE.  */
	    warning ("cast discards qualifiers from pointer target type");
	}

      /* Warn about possible alignment problems.  */
      if (STRICT_ALIGNMENT && warn_cast_align
	  && TREE_CODE (type) == POINTER_TYPE
	  && TREE_CODE (otype) == POINTER_TYPE
	  && TREE_CODE (TREE_TYPE (otype)) != VOID_TYPE
	  && TREE_CODE (TREE_TYPE (otype)) != FUNCTION_TYPE
	  /* Don't warn about opaque types, where the actual alignment
	     restriction is unknown.  */
	  && !((TREE_CODE (TREE_TYPE (otype)) == UNION_TYPE
		|| TREE_CODE (TREE_TYPE (otype)) == RECORD_TYPE)
	       && TYPE_MODE (TREE_TYPE (otype)) == VOIDmode)
	  && TYPE_ALIGN (TREE_TYPE (type)) > TYPE_ALIGN (TREE_TYPE (otype)))
	warning ("cast increases required alignment of target type");

      if (TREE_CODE (type) == INTEGER_TYPE
	  && TREE_CODE (otype) == POINTER_TYPE
	  && TYPE_PRECISION (type) != TYPE_PRECISION (otype)
	  && !TREE_CONSTANT (value))
	warning ("cast from pointer to integer of different size");

      if (warn_bad_function_cast
	  && TREE_CODE (value) == CALL_EXPR
	  && TREE_CODE (type) != TREE_CODE (otype))
	warning ("cast does not match function type");

      if (TREE_CODE (type) == POINTER_TYPE
	  && TREE_CODE (otype) == INTEGER_TYPE
	  && TYPE_PRECISION (type) != TYPE_PRECISION (otype)
	  /* Don't warn about converting any constant.  */
	  && !TREE_CONSTANT (value))
	warning ("cast to pointer from integer of different size");

      ovalue = value;
      value = convert (type, value);

      /* Ignore any integer overflow caused by the cast.  */
      if (TREE_CODE (value) == INTEGER_CST)
	{
	  TREE_OVERFLOW (value) = TREE_OVERFLOW (ovalue);
	  TREE_CONSTANT_OVERFLOW (value) = TREE_CONSTANT_OVERFLOW (ovalue);
	}
    }

  /* Pedantically, don't let (void *) (FOO *) 0 be a null pointer constant.  */
  if (pedantic && TREE_CODE (value) == INTEGER_CST
      && TREE_CODE (expr) == INTEGER_CST
      && TREE_CODE (TREE_TYPE (expr)) != INTEGER_TYPE)
    value = non_lvalue (value);

  /* If pedantic, don't let a cast be an lvalue.  */
  if (value == expr && pedantic)
    value = non_lvalue (value);

  return value;
}

/* Interpret a cast of expression EXPR to type TYPE.  */
tree
c_cast_expr (type, expr)
     tree type, expr;
{
  int saved_wsp = warn_strict_prototypes;

  /* This avoids warnings about unprototyped casts on
     integers.  E.g. "#define SIG_DFL (void(*)())0".  */
  if (TREE_CODE (expr) == INTEGER_CST)
    warn_strict_prototypes = 0;
  type = groktypename (type);
  warn_strict_prototypes = saved_wsp;

  return build_c_cast (type, expr);
}


/* Build an assignment expression of lvalue LHS from value RHS.
   MODIFYCODE is the code for a binary operator that we use
   to combine the old value of LHS with RHS to get the new value.
   Or else MODIFYCODE is NOP_EXPR meaning do a simple assignment.  */

tree
build_modify_expr (lhs, modifycode, rhs)
     tree lhs, rhs;
     enum tree_code modifycode;
{
  tree result;
  tree newrhs;
  tree lhstype = TREE_TYPE (lhs);
  tree olhstype = lhstype;

  /* Types that aren't fully specified cannot be used in assignments.  */
  lhs = require_complete_type (lhs);

  /* Avoid duplicate error messages from operands that had errors.  */
  if (TREE_CODE (lhs) == ERROR_MARK || TREE_CODE (rhs) == ERROR_MARK)
    return error_mark_node;

  /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
  /* Do not use STRIP_NOPS here.  We do not want an enumerator
     whose value is 0 to count as a null pointer constant.  */
  if (TREE_CODE (rhs) == NON_LVALUE_EXPR)
    rhs = TREE_OPERAND (rhs, 0);

  newrhs = rhs;

  /* Handle control structure constructs used as "lvalues".  */

  switch (TREE_CODE (lhs))
    {
      /* Handle (a, b) used as an "lvalue".  */
    case COMPOUND_EXPR:
      pedantic_lvalue_warning (COMPOUND_EXPR);
      newrhs = build_modify_expr (TREE_OPERAND (lhs, 1), modifycode, rhs);
      if (TREE_CODE (newrhs) == ERROR_MARK)
	return error_mark_node;
      return build (COMPOUND_EXPR, lhstype,
		    TREE_OPERAND (lhs, 0), newrhs);
 
      /* Handle (a ? b : c) used as an "lvalue".  */
    case COND_EXPR:
      pedantic_lvalue_warning (COND_EXPR);
      rhs = save_expr (rhs);
      {
	/* Produce (a ? (b = rhs) : (c = rhs))
	   except that the RHS goes through a save-expr
	   so the code to compute it is only emitted once.  */
	tree cond
	  = build_conditional_expr (TREE_OPERAND (lhs, 0),
				    build_modify_expr (TREE_OPERAND (lhs, 1),
						       modifycode, rhs),
				    build_modify_expr (TREE_OPERAND (lhs, 2),
						       modifycode, rhs));
	if (TREE_CODE (cond) == ERROR_MARK)
	  return cond;
	/* Make sure the code to compute the rhs comes out
	   before the split.  */
	return build (COMPOUND_EXPR, TREE_TYPE (lhs),
		      /* But cast it to void to avoid an "unused" error.  */
		      convert (void_type_node, rhs), cond);
      }
    default:
      break;
    }

  /* If a binary op has been requested, combine the old LHS value with the RHS
     producing the value we should actually store into the LHS.  */

  if (modifycode != NOP_EXPR)
    {
      lhs = stabilize_reference (lhs);
      newrhs = build_binary_op (modifycode, lhs, rhs, 1);
    }

  /* Handle a cast used as an "lvalue".
     We have already performed any binary operator using the value as cast.
     Now convert the result to the cast type of the lhs,
     and then true type of the lhs and store it there;
     then convert result back to the cast type to be the value
     of the assignment.  */

  switch (TREE_CODE (lhs))
    {
    case NOP_EXPR:
    case CONVERT_EXPR:
    case FLOAT_EXPR:
    case FIX_TRUNC_EXPR:
    case FIX_FLOOR_EXPR:
    case FIX_ROUND_EXPR:
    case FIX_CEIL_EXPR:
      newrhs = default_function_array_conversion (newrhs);
      {
	tree inner_lhs = TREE_OPERAND (lhs, 0);
	tree result;
	result = build_modify_expr (inner_lhs, NOP_EXPR,
				    convert (TREE_TYPE (inner_lhs),
					     convert (lhstype, newrhs)));
	if (TREE_CODE (result) == ERROR_MARK)
	  return result;
	pedantic_lvalue_warning (CONVERT_EXPR);
	return convert (TREE_TYPE (lhs), result);
      }
      
    default:
      break;
    }

  /* Now we have handled acceptable kinds of LHS that are not truly lvalues.
     Reject anything strange now.  */

  if (!lvalue_or_else (lhs, "invalid lvalue in assignment"))
    return error_mark_node;

  /* Warn about storing in something that is `const'.  */

  if (TREE_READONLY (lhs) || TYPE_READONLY (lhstype)
      || ((TREE_CODE (lhstype) == RECORD_TYPE
	   || TREE_CODE (lhstype) == UNION_TYPE)
	  && C_TYPE_FIELDS_READONLY (lhstype)))
    readonly_warning (lhs, "assignment");

  /* If storing into a structure or union member,
     it has probably been given type `int'.
     Compute the type that would go with
     the actual amount of storage the member occupies.  */

  if (TREE_CODE (lhs) == COMPONENT_REF
      && (TREE_CODE (lhstype) == INTEGER_TYPE
	  || TREE_CODE (lhstype) == BOOLEAN_TYPE
	  || TREE_CODE (lhstype) == REAL_TYPE
	  || TREE_CODE (lhstype) == ENUMERAL_TYPE))
    lhstype = TREE_TYPE (get_unwidened (lhs, 0));

  /* If storing in a field that is in actuality a short or narrower than one,
     we must store in the field in its actual type.  */

  if (lhstype != TREE_TYPE (lhs))
    {
      lhs = copy_node (lhs);
      TREE_TYPE (lhs) = lhstype;
    }

  /* Convert new value to destination type.  */

  newrhs = convert_for_assignment (lhstype, newrhs, _("assignment"),
				   NULL_TREE, NULL_TREE, 0);
  if (TREE_CODE (newrhs) == ERROR_MARK)
    return error_mark_node;

  /* Scan operands */

  result = build (MODIFY_EXPR, lhstype, lhs, newrhs);
  TREE_SIDE_EFFECTS (result) = 1;

  /* If we got the LHS in a different type for storing in,
     convert the result back to the nominal type of LHS
     so that the value we return always has the same type
     as the LHS argument.  */

  if (olhstype == TREE_TYPE (result))
    return result;
  return convert_for_assignment (olhstype, result, _("assignment"),
				 NULL_TREE, NULL_TREE, 0);
}

/* Convert value RHS to type TYPE as preparation for an assignment
   to an lvalue of type TYPE.
   The real work of conversion is done by `convert'.
   The purpose of this function is to generate error messages
   for assignments that are not allowed in C.
   ERRTYPE is a string to use in error messages:
   "assignment", "return", etc.  If it is null, this is parameter passing
   for a function call (and different error messages are output).

   FUNNAME is the name of the function being called,
   as an IDENTIFIER_NODE, or null.
   PARMNUM is the number of the argument, for printing in error messages.  */

static tree
convert_for_assignment (type, rhs, errtype, fundecl, funname, parmnum)
     tree type, rhs;
     const char *errtype;
     tree fundecl, funname;
     int parmnum;
{
  enum tree_code codel = TREE_CODE (type);
  tree rhstype;
  enum tree_code coder;

  /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
  /* Do not use STRIP_NOPS here.  We do not want an enumerator
     whose value is 0 to count as a null pointer constant.  */
  if (TREE_CODE (rhs) == NON_LVALUE_EXPR)
    rhs = TREE_OPERAND (rhs, 0);

  if (TREE_CODE (TREE_TYPE (rhs)) == ARRAY_TYPE
      || TREE_CODE (TREE_TYPE (rhs)) == FUNCTION_TYPE)
    rhs = default_conversion (rhs);
  else if (optimize && TREE_CODE (rhs) == VAR_DECL)
    rhs = decl_constant_value_for_broken_optimization (rhs);

  rhstype = TREE_TYPE (rhs);
  coder = TREE_CODE (rhstype);

  if (coder == ERROR_MARK)
    return error_mark_node;

  if (TYPE_MAIN_VARIANT (type) == TYPE_MAIN_VARIANT (rhstype))
    {
      overflow_warning (rhs);
      /* Check for Objective-C protocols.  This will issue a warning if
	 there are protocol violations.  No need to use the return value.  */
      maybe_objc_comptypes (type, rhstype, 0);
      return rhs;
    }

  if (coder == VOID_TYPE)
    {
      error ("void value not ignored as it ought to be");
      return error_mark_node;
    }
  /* A type converts to a reference to it.  
     This code doesn't fully support references, it's just for the
     special case of va_start and va_copy.  */
  if (codel == REFERENCE_TYPE
      && comptypes (TREE_TYPE (type), TREE_TYPE (rhs)) == 1)
    {
      if (mark_addressable (rhs) == 0)
	return error_mark_node;
      rhs = build1 (ADDR_EXPR, build_pointer_type (TREE_TYPE (rhs)), rhs);

      /* We already know that these two types are compatible, but they
	 may not be exactly identical.  In fact, `TREE_TYPE (type)' is
	 likely to be __builtin_va_list and `TREE_TYPE (rhs)' is
	 likely to be va_list, a typedef to __builtin_va_list, which
	 is different enough that it will cause problems later.  */
      if (TREE_TYPE (TREE_TYPE (rhs)) != TREE_TYPE (type))
	rhs = build1 (NOP_EXPR, build_pointer_type (TREE_TYPE (type)), rhs);

      rhs = build1 (NOP_EXPR, type, rhs);
      return rhs;
    }
  /* Arithmetic types all interconvert, and enum is treated like int.  */
  else if ((codel == INTEGER_TYPE || codel == REAL_TYPE 
	    || codel == ENUMERAL_TYPE || codel == COMPLEX_TYPE
	    || codel == BOOLEAN_TYPE)
	   && (coder == INTEGER_TYPE || coder == REAL_TYPE 
	       || coder == ENUMERAL_TYPE || coder == COMPLEX_TYPE
	       || coder == BOOLEAN_TYPE))
    return convert_and_check (type, rhs);

  /* Conversion to a transparent union from its member types.
     This applies only to function arguments.  */
  else if (codel == UNION_TYPE && TYPE_TRANSPARENT_UNION (type) && ! errtype)
    {
      tree memb_types;
      tree marginal_memb_type = 0;

      for (memb_types = TYPE_FIELDS (type); memb_types;
	   memb_types = TREE_CHAIN (memb_types))
	{
	  tree memb_type = TREE_TYPE (memb_types);

	  if (comptypes (TYPE_MAIN_VARIANT (memb_type),
			 TYPE_MAIN_VARIANT (rhstype)))
	    break;

	  if (TREE_CODE (memb_type) != POINTER_TYPE)
	    continue;

	  if (coder == POINTER_TYPE)
	    {
	      tree ttl = TREE_TYPE (memb_type);
	      tree ttr = TREE_TYPE (rhstype);

	      /* Any non-function converts to a [const][volatile] void *
		 and vice versa; otherwise, targets must be the same.
		 Meanwhile, the lhs target must have all the qualifiers of
		 the rhs.  */
	      if (VOID_TYPE_P (ttl) || VOID_TYPE_P (ttr)
		  || comp_target_types (memb_type, rhstype))
		{
		  /* If this type won't generate any warnings, use it.  */
		  if (TYPE_QUALS (ttl) == TYPE_QUALS (ttr)
		      || ((TREE_CODE (ttr) == FUNCTION_TYPE
			   && TREE_CODE (ttl) == FUNCTION_TYPE)
			  ? ((TYPE_QUALS (ttl) | TYPE_QUALS (ttr))
			     == TYPE_QUALS (ttr))
			  : ((TYPE_QUALS (ttl) | TYPE_QUALS (ttr))
			     == TYPE_QUALS (ttl))))
		    break;

		  /* Keep looking for a better type, but remember this one.  */
		  if (! marginal_memb_type)
		    marginal_memb_type = memb_type;
		}
	    }

	  /* Can convert integer zero to any pointer type.  */
	  if (integer_zerop (rhs)
	      || (TREE_CODE (rhs) == NOP_EXPR
		  && integer_zerop (TREE_OPERAND (rhs, 0))))
	    {
	      rhs = null_pointer_node;
	      break;
	    }
	}

      if (memb_types || marginal_memb_type)
	{
	  if (! memb_types)
	    {
	      /* We have only a marginally acceptable member type;
		 it needs a warning.  */
	      tree ttl = TREE_TYPE (marginal_memb_type);
	      tree ttr = TREE_TYPE (rhstype);

	      /* Const and volatile mean something different for function
		 types, so the usual warnings are not appropriate.  */
	      if (TREE_CODE (ttr) == FUNCTION_TYPE
		  && TREE_CODE (ttl) == FUNCTION_TYPE)
		{
		  /* Because const and volatile on functions are
		     restrictions that say the function will not do
		     certain things, it is okay to use a const or volatile
		     function where an ordinary one is wanted, but not
		     vice-versa.  */
		  if (TYPE_QUALS (ttl) & ~TYPE_QUALS (ttr))
		    warn_for_assignment ("%s makes qualified function pointer from unqualified",
					 errtype, funname, parmnum);
		}
	      else if (TYPE_QUALS (ttr) & ~TYPE_QUALS (ttl))
		warn_for_assignment ("%s discards qualifiers from pointer target type",
				     errtype, funname,
				     parmnum);
	    }
	  
	  if (pedantic && ! DECL_IN_SYSTEM_HEADER (fundecl))
	    pedwarn ("ISO C prohibits argument conversion to union type");

	  return build1 (NOP_EXPR, type, rhs);
	}
    }

  /* Conversions among pointers */
  else if ((codel == POINTER_TYPE || codel == REFERENCE_TYPE)
	   && (coder == POINTER_TYPE || coder == REFERENCE_TYPE))
    {
      tree ttl = TREE_TYPE (type);
      tree ttr = TREE_TYPE (rhstype);

      /* Any non-function converts to a [const][volatile] void *
	 and vice versa; otherwise, targets must be the same.
	 Meanwhile, the lhs target must have all the qualifiers of the rhs.  */
      if (VOID_TYPE_P (ttl) || VOID_TYPE_P (ttr)
	  || comp_target_types (type, rhstype)
	  || (unsigned_type (TYPE_MAIN_VARIANT (ttl))
	      == unsigned_type (TYPE_MAIN_VARIANT (ttr))))
	{
	  if (pedantic
	      && ((VOID_TYPE_P (ttl) && TREE_CODE (ttr) == FUNCTION_TYPE)
		  ||
		  (VOID_TYPE_P (ttr)
		   /* Check TREE_CODE to catch cases like (void *) (char *) 0
		      which are not ANSI null ptr constants.  */
		   && (!integer_zerop (rhs) || TREE_CODE (rhs) == NOP_EXPR)
		   && TREE_CODE (ttl) == FUNCTION_TYPE)))
	    warn_for_assignment ("ISO C forbids %s between function pointer and `void *'",
				 errtype, funname, parmnum);
	  /* Const and volatile mean something different for function types,
	     so the usual warnings are not appropriate.  */
	  else if (TREE_CODE (ttr) != FUNCTION_TYPE
		   && TREE_CODE (ttl) != FUNCTION_TYPE)
	    {
	      if (TYPE_QUALS (ttr) & ~TYPE_QUALS (ttl))
		warn_for_assignment ("%s discards qualifiers from pointer target type",
				     errtype, funname, parmnum);
	      /* If this is not a case of ignoring a mismatch in signedness,
		 no warning.  */
	      else if (VOID_TYPE_P (ttl) || VOID_TYPE_P (ttr)
		       || comp_target_types (type, rhstype))
		;
	      /* If there is a mismatch, do warn.  */
	      else if (pedantic)
		warn_for_assignment ("pointer targets in %s differ in signedness",
				     errtype, funname, parmnum);
	    }
	  else if (TREE_CODE (ttl) == FUNCTION_TYPE
		   && TREE_CODE (ttr) == FUNCTION_TYPE)
	    {
	      /* Because const and volatile on functions are restrictions
		 that say the function will not do certain things,
		 it is okay to use a const or volatile function
		 where an ordinary one is wanted, but not vice-versa.  */
	      if (TYPE_QUALS (ttl) & ~TYPE_QUALS (ttr))
		warn_for_assignment ("%s makes qualified function pointer from unqualified",
				     errtype, funname, parmnum);
	    }
	}
      else
	warn_for_assignment ("%s from incompatible pointer type",
			     errtype, funname, parmnum);
      return convert (type, rhs);
    }
  else if (codel == POINTER_TYPE && coder == INTEGER_TYPE)
    {
      /* An explicit constant 0 can convert to a pointer,
	 or one that results from arithmetic, even including
	 a cast to integer type.  */
      if (! (TREE_CODE (rhs) == INTEGER_CST && integer_zerop (rhs))
	  &&
	  ! (TREE_CODE (rhs) == NOP_EXPR
	     && TREE_CODE (TREE_TYPE (rhs)) == INTEGER_TYPE
	     && TREE_CODE (TREE_OPERAND (rhs, 0)) == INTEGER_CST
	     && integer_zerop (TREE_OPERAND (rhs, 0))))
	{
	  warn_for_assignment ("%s makes pointer from integer without a cast",
			       errtype, funname, parmnum);
	  return convert (type, rhs);
	}
      return null_pointer_node;
    }
  else if (codel == INTEGER_TYPE && coder == POINTER_TYPE)
    {
      warn_for_assignment ("%s makes integer from pointer without a cast",
			   errtype, funname, parmnum);
      return convert (type, rhs);
    }
  else if (codel == BOOLEAN_TYPE && coder == POINTER_TYPE)
    return convert (type, rhs);

  if (!errtype)
    {
      if (funname)
 	{
 	  tree selector = maybe_building_objc_message_expr ();
 
 	  if (selector && parmnum > 2)
 	    error ("incompatible type for argument %d of `%s'",
		   parmnum - 2, IDENTIFIER_POINTER (selector));
 	  else
	    error ("incompatible type for argument %d of `%s'",
		   parmnum, IDENTIFIER_POINTER (funname));
	}
      else
	error ("incompatible type for argument %d of indirect function call",
	       parmnum);
    }
  else
    error ("incompatible types in %s", errtype);

  return error_mark_node;
}

/* Convert VALUE for assignment into inlined parameter PARM.  */

tree
c_convert_parm_for_inlining (parm, value, fn)
     tree parm, value, fn;
{
  tree ret, type;

  /* If FN was prototyped, the value has been converted already
     in convert_arguments.  */
  if (! value || TYPE_ARG_TYPES (TREE_TYPE (fn)))
    return value;

  type = TREE_TYPE (parm);
  ret = convert_for_assignment (type, value, 
				(char *) 0 /* arg passing  */, fn,
				DECL_NAME (fn), 0);
  if (PROMOTE_PROTOTYPES
      && INTEGRAL_TYPE_P (type)
      && (TYPE_PRECISION (type) < TYPE_PRECISION (integer_type_node)))
    ret = default_conversion (ret);
  return ret;
}

/* Print a warning using MSGID.
   It gets OPNAME as its one parameter.
   If OPNAME is null, it is replaced by "passing arg ARGNUM of `FUNCTION'".
   FUNCTION and ARGNUM are handled specially if we are building an
   Objective-C selector.  */

static void
warn_for_assignment (msgid, opname, function, argnum)
     const char *msgid;
     const char *opname;
     tree function;
     int argnum;
{
  if (opname == 0)
    {
      tree selector = maybe_building_objc_message_expr ();
      char * new_opname;
      
      if (selector && argnum > 2)
	{
	  function = selector;
	  argnum -= 2;
	}
      if (function)
	{
	  /* Function name is known; supply it.  */
	  const char *const argstring = _("passing arg %d of `%s'");
	  new_opname = (char *) alloca (IDENTIFIER_LENGTH (function)
					+ strlen (argstring) + 1 + 25
					/*%d*/ + 1);
	  sprintf (new_opname, argstring, argnum,
		   IDENTIFIER_POINTER (function));
	}
      else
	{
	  /* Function name unknown (call through ptr); just give arg number.  */
	  const char *const argnofun = _("passing arg %d of pointer to function");
	  new_opname = (char *) alloca (strlen (argnofun) + 1 + 25 /*%d*/ + 1);
	  sprintf (new_opname, argnofun, argnum);
	}
      opname = new_opname;
    }
  pedwarn (msgid, opname);
}

/* If VALUE is a compound expr all of whose expressions are constant, then
   return its value.  Otherwise, return error_mark_node.

   This is for handling COMPOUND_EXPRs as initializer elements
   which is allowed with a warning when -pedantic is specified.  */

static tree
valid_compound_expr_initializer (value, endtype)
     tree value;
     tree endtype;
{
  if (TREE_CODE (value) == COMPOUND_EXPR)
    {
      if (valid_compound_expr_initializer (TREE_OPERAND (value, 0), endtype)
	  == error_mark_node)
	return error_mark_node;
      return valid_compound_expr_initializer (TREE_OPERAND (value, 1),
					      endtype);
    }
  else if (! TREE_CONSTANT (value)
	   && ! initializer_constant_valid_p (value, endtype))
    return error_mark_node;
  else
    return value;
}

/* Perform appropriate conversions on the initial value of a variable,
   store it in the declaration DECL,
   and print any error messages that are appropriate.
   If the init is invalid, store an ERROR_MARK.  */

void
store_init_value (decl, init)
     tree decl, init;
{
  tree value, type;

  /* If variable's type was invalidly declared, just ignore it.  */

  type = TREE_TYPE (decl);
  if (TREE_CODE (type) == ERROR_MARK)
    return;

  /* Digest the specified initializer into an expression.  */

  value = digest_init (type, init, TREE_STATIC (decl),
		       TREE_STATIC (decl) || (pedantic && !flag_isoc99));

  /* Store the expression if valid; else report error.  */

#if 0
  /* Note that this is the only place we can detect the error
     in a case such as   struct foo bar = (struct foo) { x, y };
     where there is one initial value which is a constructor expression.  */
  if (value == error_mark_node)
    ;
  else if (TREE_STATIC (decl) && ! TREE_CONSTANT (value))
    {
      error ("initializer for static variable is not constant");
      value = error_mark_node;
    }
  else if (TREE_STATIC (decl)
	   && initializer_constant_valid_p (value, TREE_TYPE (value)) == 0)
    {
      error ("initializer for static variable uses complicated arithmetic");
      value = error_mark_node;
    }
  else
    {
      if (pedantic && TREE_CODE (value) == CONSTRUCTOR)
	{
	  if (! TREE_CONSTANT (value))
	    pedwarn ("aggregate initializer is not constant");
	  else if (! TREE_STATIC (value))
	    pedwarn ("aggregate initializer uses complicated arithmetic");
	}
    }
#endif

  if (warn_traditional && !in_system_header
      && AGGREGATE_TYPE_P (TREE_TYPE (decl)) && ! TREE_STATIC (decl))
    warning ("traditional C rejects automatic aggregate initialization");

  DECL_INITIAL (decl) = value;

  /* ANSI wants warnings about out-of-range constant initializers.  */
  STRIP_TYPE_NOPS (value);
  constant_expression_warning (value);

  /* Check if we need to set array size from compound literal size.  */
  if (TREE_CODE (type) == ARRAY_TYPE
      && TYPE_DOMAIN (type) == 0
      && value != error_mark_node)
    {
      tree inside_init = init;

      if (TREE_CODE (init) == NON_LVALUE_EXPR)
	inside_init = TREE_OPERAND (init, 0);
      inside_init = fold (inside_init);

      if (TREE_CODE (inside_init) == COMPOUND_LITERAL_EXPR)
	{
	  tree decl = COMPOUND_LITERAL_EXPR_DECL (inside_init);

	  if (TYPE_DOMAIN (TREE_TYPE (decl)))
	    {
	      /* For int foo[] = (int [3]){1}; we need to set array size
		 now since later on array initializer will be just the
		 brace enclosed list of the compound literal.  */
	      TYPE_DOMAIN (type) = TYPE_DOMAIN (TREE_TYPE (decl));
	      layout_type (type);
	      layout_decl (decl, 0);
	    }
	}
    }
}

/* Methods for storing and printing names for error messages.  */

/* Implement a spelling stack that allows components of a name to be pushed
   and popped.  Each element on the stack is this structure.  */

struct spelling
{
  int kind;
  union
    {
      int i;
      const char *s;
    } u;
};

#define SPELLING_STRING 1
#define SPELLING_MEMBER 2
#define SPELLING_BOUNDS 3

static struct spelling *spelling;	/* Next stack element (unused).  */
static struct spelling *spelling_base;	/* Spelling stack base.  */
static int spelling_size;		/* Size of the spelling stack.  */

/* Macros to save and restore the spelling stack around push_... functions.
   Alternative to SAVE_SPELLING_STACK.  */

#define SPELLING_DEPTH() (spelling - spelling_base)
#define RESTORE_SPELLING_DEPTH(DEPTH) (spelling = spelling_base + (DEPTH))

/* Save and restore the spelling stack around arbitrary C code.  */

#define SAVE_SPELLING_DEPTH(code)		\
{						\
  int __depth = SPELLING_DEPTH ();		\
  code;						\
  RESTORE_SPELLING_DEPTH (__depth);		\
}

/* Push an element on the spelling stack with type KIND and assign VALUE
   to MEMBER.  */

#define PUSH_SPELLING(KIND, VALUE, MEMBER)				\
{									\
  int depth = SPELLING_DEPTH ();					\
									\
  if (depth >= spelling_size)						\
    {									\
      spelling_size += 10;						\
      if (spelling_base == 0)						\
	spelling_base							\
	  = (struct spelling *) xmalloc (spelling_size * sizeof (struct spelling));	\
      else								\
        spelling_base							\
	  = (struct spelling *) xrealloc (spelling_base,		\
					  spelling_size * sizeof (struct spelling));	\
      RESTORE_SPELLING_DEPTH (depth);					\
    }									\
									\
  spelling->kind = (KIND);						\
  spelling->MEMBER = (VALUE);						\
  spelling++;								\
}

/* Push STRING on the stack.  Printed literally.  */

static void
push_string (string)
     const char *string;
{
  PUSH_SPELLING (SPELLING_STRING, string, u.s);
}

/* Push a member name on the stack.  Printed as '.' STRING.  */

static void
push_member_name (decl)
     tree decl;
     
{
  const char *const string
    = DECL_NAME (decl) ? IDENTIFIER_POINTER (DECL_NAME (decl)) : "<anonymous>";
  PUSH_SPELLING (SPELLING_MEMBER, string, u.s);
}

/* Push an array bounds on the stack.  Printed as [BOUNDS].  */

static void
push_array_bounds (bounds)
     int bounds;
{
  PUSH_SPELLING (SPELLING_BOUNDS, bounds, u.i);
}

/* Compute the maximum size in bytes of the printed spelling.  */

static int
spelling_length ()
{
  int size = 0;
  struct spelling *p;

  for (p = spelling_base; p < spelling; p++)
    {
      if (p->kind == SPELLING_BOUNDS)
	size += 25;
      else
	size += strlen (p->u.s) + 1;
    }

  return size;
}

/* Print the spelling to BUFFER and return it.  */

static char *
print_spelling (buffer)
     char *buffer;
{
  char *d = buffer;
  struct spelling *p;

  for (p = spelling_base; p < spelling; p++)
    if (p->kind == SPELLING_BOUNDS)
      {
	sprintf (d, "[%d]", p->u.i);
	d += strlen (d);
      }
    else
      {
	const char *s;
	if (p->kind == SPELLING_MEMBER)
	  *d++ = '.';
	for (s = p->u.s; (*d = *s++); d++)
	  ;
      }
  *d++ = '\0';
  return buffer;
}

/* Issue an error message for a bad initializer component.
   MSGID identifies the message.
   The component name is taken from the spelling stack.  */

void
error_init (msgid)
     const char *msgid;
{
  char *ofwhat;

  error ("%s", _(msgid));
  ofwhat = print_spelling ((char *) alloca (spelling_length () + 1));
  if (*ofwhat)
    error ("(near initialization for `%s')", ofwhat);
}

/* Issue a pedantic warning for a bad initializer component.
   MSGID identifies the message.
   The component name is taken from the spelling stack.  */

void
pedwarn_init (msgid)
     const char *msgid;
{
  char *ofwhat;

  pedwarn ("%s", _(msgid));
  ofwhat = print_spelling ((char *) alloca (spelling_length () + 1));
  if (*ofwhat)
    pedwarn ("(near initialization for `%s')", ofwhat);
}

/* Issue a warning for a bad initializer component.
   MSGID identifies the message.
   The component name is taken from the spelling stack.  */

static void
warning_init (msgid)
     const char *msgid;
{
  char *ofwhat;

  warning ("%s", _(msgid));
  ofwhat = print_spelling ((char *) alloca (spelling_length () + 1));
  if (*ofwhat)
    warning ("(near initialization for `%s')", ofwhat);
}

/* Digest the parser output INIT as an initializer for type TYPE.
   Return a C expression of type TYPE to represent the initial value.

   The arguments REQUIRE_CONSTANT and CONSTRUCTOR_CONSTANT request errors
   if non-constant initializers or elements are seen.  CONSTRUCTOR_CONSTANT
   applies only to elements of constructors.  */

static tree
digest_init (type, init, require_constant, constructor_constant)
     tree type, init;
     int require_constant, constructor_constant;
{
  enum tree_code code = TREE_CODE (type);
  tree inside_init = init;

  if (type == error_mark_node
      || init == error_mark_node
      || TREE_TYPE (init) == error_mark_node)
    return error_mark_node;

  /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
  /* Do not use STRIP_NOPS here.  We do not want an enumerator
     whose value is 0 to count as a null pointer constant.  */
  if (TREE_CODE (init) == NON_LVALUE_EXPR)
    inside_init = TREE_OPERAND (init, 0);

  inside_init = fold (inside_init);

  /* Initialization of an array of chars from a string constant
     optionally enclosed in braces.  */

  if (code == ARRAY_TYPE)
    {
      tree typ1 = TYPE_MAIN_VARIANT (TREE_TYPE (type));
      if ((typ1 == char_type_node
	   || typ1 == signed_char_type_node
	   || typ1 == unsigned_char_type_node
	   || typ1 == unsigned_wchar_type_node
	   || typ1 == signed_wchar_type_node)
	  && ((inside_init && TREE_CODE (inside_init) == STRING_CST)))
	{
	  if (comptypes (TYPE_MAIN_VARIANT (TREE_TYPE (inside_init)),
			 TYPE_MAIN_VARIANT (type)))
	    return inside_init;

	  if ((TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (inside_init)))
	       != char_type_node)
	      && TYPE_PRECISION (typ1) == TYPE_PRECISION (char_type_node))
	    {
	      error_init ("char-array initialized from wide string");
	      return error_mark_node;
	    }
	  if ((TYPE_MAIN_VARIANT (TREE_TYPE (TREE_TYPE (inside_init)))
	       == char_type_node)
	      && TYPE_PRECISION (typ1) != TYPE_PRECISION (char_type_node))
	    {
	      error_init ("int-array initialized from non-wide string");
	      return error_mark_node;
	    }

	  TREE_TYPE (inside_init) = type;
	  if (TYPE_DOMAIN (type) != 0
	      && TYPE_SIZE (type) != 0
	      && TREE_CODE (TYPE_SIZE (type)) == INTEGER_CST
	      /* Subtract 1 (or sizeof (wchar_t))
		 because it's ok to ignore the terminating null char
		 that is counted in the length of the constant.  */
	      && 0 > compare_tree_int (TYPE_SIZE_UNIT (type),
				       TREE_STRING_LENGTH (inside_init)
				       - ((TYPE_PRECISION (typ1)
					   != TYPE_PRECISION (char_type_node))
					  ? (TYPE_PRECISION (wchar_type_node)
					     / BITS_PER_UNIT)
					  : 1)))
	    pedwarn_init ("initializer-string for array of chars is too long");

	  return inside_init;
	}
    }

  /* Any type can be initialized
     from an expression of the same type, optionally with braces.  */

  if (inside_init && TREE_TYPE (inside_init) != 0
      && (comptypes (TYPE_MAIN_VARIANT (TREE_TYPE (inside_init)),
		     TYPE_MAIN_VARIANT (type))
	  || (code == ARRAY_TYPE
	      && comptypes (TREE_TYPE (inside_init), type))
	  || (code == VECTOR_TYPE
	      && comptypes (TREE_TYPE (inside_init), type))
	  || (code == POINTER_TYPE
	      && (TREE_CODE (TREE_TYPE (inside_init)) == ARRAY_TYPE
		  || TREE_CODE (TREE_TYPE (inside_init)) == FUNCTION_TYPE)
	      && comptypes (TREE_TYPE (TREE_TYPE (inside_init)),
			    TREE_TYPE (type)))))
    {
      if (code == POINTER_TYPE)
	inside_init = default_function_array_conversion (inside_init);

      if (require_constant && !flag_isoc99
	  && TREE_CODE (inside_init) == COMPOUND_LITERAL_EXPR)
	{
	  /* As an extension, allow initializing objects with static storage
	     duration with compound literals (which are then treated just as
	     the brace enclosed list they contain).  */
	  tree decl = COMPOUND_LITERAL_EXPR_DECL (inside_init);
	  inside_init = DECL_INITIAL (decl);
	}

      if (code == ARRAY_TYPE && TREE_CODE (inside_init) != STRING_CST
	  && TREE_CODE (inside_init) != CONSTRUCTOR)
	{
	  error_init ("array initialized from non-constant array expression");
	  return error_mark_node;
	}

      if (optimize && TREE_CODE (inside_init) == VAR_DECL)
	inside_init = decl_constant_value_for_broken_optimization (inside_init);

      /* Compound expressions can only occur here if -pedantic or
	 -pedantic-errors is specified.  In the later case, we always want
	 an error.  In the former case, we simply want a warning.  */
      if (require_constant && pedantic
	  && TREE_CODE (inside_init) == COMPOUND_EXPR)
	{
	  inside_init
	    = valid_compound_expr_initializer (inside_init,
					       TREE_TYPE (inside_init));
	  if (inside_init == error_mark_node)
	    error_init ("initializer element is not constant");
	  else
	    pedwarn_init ("initializer element is not constant");
	  if (flag_pedantic_errors)
	    inside_init = error_mark_node;
	}
      else if (require_constant 
	       && (!TREE_CONSTANT (inside_init)
		   /* This test catches things like `7 / 0' which
		      result in an expression for which TREE_CONSTANT
		      is true, but which is not actually something
		      that is a legal constant.  We really should not
		      be using this function, because it is a part of
		      the back-end.  Instead, the expression should
		      already have been turned into ERROR_MARK_NODE.  */
		   || !initializer_constant_valid_p (inside_init,
						     TREE_TYPE (inside_init))))
	{
	  error_init ("initializer element is not constant");
	  inside_init = error_mark_node;
	}

      return inside_init;
    }

  /* Handle scalar types, including conversions.  */

  if (code == INTEGER_TYPE || code == REAL_TYPE || code == POINTER_TYPE
      || code == ENUMERAL_TYPE || code == BOOLEAN_TYPE || code == COMPLEX_TYPE)
    {
      /* Note that convert_for_assignment calls default_conversion
	 for arrays and functions.  We must not call it in the
	 case where inside_init is a null pointer constant.  */
      inside_init
	= convert_for_assignment (type, init, _("initialization"),
				  NULL_TREE, NULL_TREE, 0);

      if (require_constant && ! TREE_CONSTANT (inside_init))
	{
	  error_init ("initializer element is not constant");
	  inside_init = error_mark_node;
	}
      else if (require_constant
	       && initializer_constant_valid_p (inside_init, TREE_TYPE (inside_init)) == 0)
	{
	  error_init ("initializer element is not computable at load time");
	  inside_init = error_mark_node;
	}

      return inside_init;
    }

  /* Come here only for records and arrays.  */

  if (COMPLETE_TYPE_P (type) && TREE_CODE (TYPE_SIZE (type)) != INTEGER_CST)
    {
      error_init ("variable-sized object may not be initialized");
      return error_mark_node;
    }

  /* Traditionally, you can write  struct foo x = 0;
     and it initializes the first element of x to 0.  */
  if (flag_traditional)
    {
      tree top = 0, prev = 0, otype = type;
      while (TREE_CODE (type) == RECORD_TYPE
	     || TREE_CODE (type) == ARRAY_TYPE
	     || TREE_CODE (type) == QUAL_UNION_TYPE
	     || TREE_CODE (type) == UNION_TYPE)
	{
	  tree temp = build (CONSTRUCTOR, type, NULL_TREE, NULL_TREE);
	  if (prev == 0)
	    top = temp;
	  else
	    TREE_OPERAND (prev, 1) = build_tree_list (NULL_TREE, temp);
	  prev = temp;
	  if (TREE_CODE (type) == ARRAY_TYPE)
	    type = TREE_TYPE (type);
	  else if (TYPE_FIELDS (type))
	    type = TREE_TYPE (TYPE_FIELDS (type));
	  else
	    {
	      error_init ("invalid initializer");
	      return error_mark_node;
	    }
	}

      if (otype != type)
	{
	  TREE_OPERAND (prev, 1)
	    = build_tree_list (NULL_TREE,
			       digest_init (type, init, require_constant,
					    constructor_constant));
	  return top;
	}
      else
	return error_mark_node;
    }
  error_init ("invalid initializer");
  return error_mark_node;
}

/* Handle initializers that use braces.  */

/* Type of object we are accumulating a constructor for.
   This type is always a RECORD_TYPE, UNION_TYPE or ARRAY_TYPE.  */
static tree constructor_type;

/* For a RECORD_TYPE or UNION_TYPE, this is the chain of fields
   left to fill.  */
static tree constructor_fields;

/* For an ARRAY_TYPE, this is the specified index
   at which to store the next element we get.  */
static tree constructor_index;

/* For an ARRAY_TYPE, this is the maximum index.  */
static tree constructor_max_index;

/* For a RECORD_TYPE, this is the first field not yet written out.  */
static tree constructor_unfilled_fields;

/* For an ARRAY_TYPE, this is the index of the first element
   not yet written out.  */
static tree constructor_unfilled_index;

/* In a RECORD_TYPE, the byte index of the next consecutive field.
   This is so we can generate gaps between fields, when appropriate.  */
static tree constructor_bit_index;

/* If we are saving up the elements rather than allocating them,
   this is the list of elements so far (in reverse order,
   most recent first).  */
static tree constructor_elements;

/* 1 if constructor should be incrementally stored into a constructor chain,
   0 if all the elements should be kept in AVL tree.  */
static int constructor_incremental;

/* 1 if so far this constructor's elements are all compile-time constants.  */
static int constructor_constant;

/* 1 if so far this constructor's elements are all valid address constants.  */
static int constructor_simple;

/* 1 if this constructor is erroneous so far.  */
static int constructor_erroneous;

/* 1 if have called defer_addressed_constants.  */
static int constructor_subconstants_deferred;

/* Structure for managing pending initializer elements, organized as an
   AVL tree.  */

struct init_node
{
  struct init_node *left, *right;
  struct init_node *parent;
  int balance;
  tree purpose;
  tree value;
};

/* Tree of pending elements at this constructor level.
   These are elements encountered out of order
   which belong at places we haven't reached yet in actually
   writing the output.
   Will never hold tree nodes across GC runs.  */
static struct init_node *constructor_pending_elts;

/* The SPELLING_DEPTH of this constructor.  */
static int constructor_depth;

/* 0 if implicitly pushing constructor levels is allowed.  */
int constructor_no_implicit = 0; /* 0 for C; 1 for some other languages.  */

static int require_constant_value;
static int require_constant_elements;

/* DECL node for which an initializer is being read.
   0 means we are reading a constructor expression
   such as (struct foo) {...}.  */
static tree constructor_decl;

/* start_init saves the ASMSPEC arg here for really_start_incremental_init.  */
static const char *constructor_asmspec;

/* Nonzero if this is an initializer for a top-level decl.  */
static int constructor_top_level;

/* Nonzero if there were any member designators in this initializer.  */
static int constructor_designated;

/* Nesting depth of designator list.  */
static int designator_depth;

/* Nonzero if there were diagnosed errors in this designator list.  */
static int designator_errorneous;


/* This stack has a level for each implicit or explicit level of
   structuring in the initializer, including the outermost one.  It
   saves the values of most of the variables above.  */

struct constructor_range_stack;

struct constructor_stack
{
  struct constructor_stack *next;
  tree type;
  tree fields;
  tree index;
  tree max_index;
  tree unfilled_index;
  tree unfilled_fields;
  tree bit_index;
  tree elements;
  struct init_node *pending_elts;
  int offset;
  int depth;
  /* If nonzero, this value should replace the entire
     constructor at this level.  */
  tree replacement_value;
  struct constructor_range_stack *range_stack;
  char constant;
  char simple;
  char implicit;
  char erroneous;
  char outer;
  char incremental;
  char designated;
};

struct constructor_stack *constructor_stack;

/* This stack represents designators from some range designator up to
   the last designator in the list.  */

struct constructor_range_stack
{
  struct constructor_range_stack *next, *prev;
  struct constructor_stack *stack;
  tree range_start;
  tree index;
  tree range_end;
  tree fields;
};

struct constructor_range_stack *constructor_range_stack;

/* This stack records separate initializers that are nested.
   Nested initializers can't happen in ANSI C, but GNU C allows them
   in cases like { ... (struct foo) { ... } ... }.  */

struct initializer_stack
{
  struct initializer_stack *next;
  tree decl;
  const char *asmspec;
  struct constructor_stack *constructor_stack;
  struct constructor_range_stack *constructor_range_stack;
  tree elements;
  struct spelling *spelling;
  struct spelling *spelling_base;
  int spelling_size;
  char top_level;
  char require_constant_value;
  char require_constant_elements;
  char deferred;
};

struct initializer_stack *initializer_stack;

/* Prepare to parse and output the initializer for variable DECL.  */

void
start_init (decl, asmspec_tree, top_level)
     tree decl;
     tree asmspec_tree;
     int top_level;
{
  const char *locus;
  struct initializer_stack *p
    = (struct initializer_stack *) xmalloc (sizeof (struct initializer_stack));
  const char *asmspec = 0;

  if (asmspec_tree)
    asmspec = TREE_STRING_POINTER (asmspec_tree);

  p->decl = constructor_decl;
  p->asmspec = constructor_asmspec;
  p->require_constant_value = require_constant_value;
  p->require_constant_elements = require_constant_elements;
  p->constructor_stack = constructor_stack;
  p->constructor_range_stack = constructor_range_stack;
  p->elements = constructor_elements;
  p->spelling = spelling;
  p->spelling_base = spelling_base;
  p->spelling_size = spelling_size;
  p->deferred = constructor_subconstants_deferred;
  p->top_level = constructor_top_level;
  p->next = initializer_stack;
  initializer_stack = p;

  constructor_decl = decl;
  constructor_asmspec = asmspec;
  constructor_subconstants_deferred = 0;
  constructor_designated = 0;
  constructor_top_level = top_level;

  if (decl != 0)
    {
      require_constant_value = TREE_STATIC (decl);
      require_constant_elements
	= ((TREE_STATIC (decl) || (pedantic && !flag_isoc99))
	   /* For a scalar, you can always use any value to initialize,
	      even within braces.  */
	   && (TREE_CODE (TREE_TYPE (decl)) == ARRAY_TYPE
	       || TREE_CODE (TREE_TYPE (decl)) == RECORD_TYPE
	       || TREE_CODE (TREE_TYPE (decl)) == UNION_TYPE
	       || TREE_CODE (TREE_TYPE (decl)) == QUAL_UNION_TYPE));
      locus = IDENTIFIER_POINTER (DECL_NAME (decl));
    }
  else
    {
      require_constant_value = 0;
      require_constant_elements = 0;
      locus = "(anonymous)";
    }

  constructor_stack = 0;
  constructor_range_stack = 0;

  missing_braces_mentioned = 0;

  spelling_base = 0;
  spelling_size = 0;
  RESTORE_SPELLING_DEPTH (0);

  if (locus)
    push_string (locus);
}

void
finish_init ()
{
  struct initializer_stack *p = initializer_stack;

  /* Output subconstants (string constants, usually)
     that were referenced within this initializer and saved up.
     Must do this if and only if we called defer_addressed_constants.  */
  if (constructor_subconstants_deferred)
    output_deferred_addressed_constants ();

  /* Free the whole constructor stack of this initializer.  */
  while (constructor_stack)
    {
      struct constructor_stack *q = constructor_stack;
      constructor_stack = q->next;
      free (q);
    }

  if (constructor_range_stack)
    abort ();

  /* Pop back to the data of the outer initializer (if any).  */
  constructor_decl = p->decl;
  constructor_asmspec = p->asmspec;
  require_constant_value = p->require_constant_value;
  require_constant_elements = p->require_constant_elements;
  constructor_stack = p->constructor_stack;
  constructor_range_stack = p->constructor_range_stack;
  constructor_elements = p->elements;
  spelling = p->spelling;
  spelling_base = p->spelling_base;
  spelling_size = p->spelling_size;
  constructor_subconstants_deferred = p->deferred;
  constructor_top_level = p->top_level;
  initializer_stack = p->next;
  free (p);
}

/* Call here when we see the initializer is surrounded by braces.
   This is instead of a call to push_init_level;
   it is matched by a call to pop_init_level.

   TYPE is the type to initialize, for a constructor expression.
   For an initializer for a decl, TYPE is zero.  */

void
really_start_incremental_init (type)
     tree type;
{
  struct constructor_stack *p
    = (struct constructor_stack *) xmalloc (sizeof (struct constructor_stack));

  if (type == 0)
    type = TREE_TYPE (constructor_decl);

  p->type = constructor_type;
  p->fields = constructor_fields;
  p->index = constructor_index;
  p->max_index = constructor_max_index;
  p->unfilled_index = constructor_unfilled_index;
  p->unfilled_fields = constructor_unfilled_fields;
  p->bit_index = constructor_bit_index;
  p->elements = constructor_elements;
  p->constant = constructor_constant;
  p->simple = constructor_simple;
  p->erroneous = constructor_erroneous;
  p->pending_elts = constructor_pending_elts;
  p->depth = constructor_depth;
  p->replacement_value = 0;
  p->implicit = 0;
  p->range_stack = 0;
  p->outer = 0;
  p->incremental = constructor_incremental;
  p->designated = constructor_designated;
  p->next = 0;
  constructor_stack = p;

  constructor_constant = 1;
  constructor_simple = 1;
  constructor_depth = SPELLING_DEPTH ();
  constructor_elements = 0;
  constructor_pending_elts = 0;
  constructor_type = type;
  constructor_incremental = 1;
  constructor_designated = 0;
  designator_depth = 0;
  designator_errorneous = 0;

  if (TREE_CODE (constructor_type) == RECORD_TYPE
      || TREE_CODE (constructor_type) == UNION_TYPE)
    {
      constructor_fields = TYPE_FIELDS (constructor_type);
      /* Skip any nameless bit fields at the beginning.  */
      while (constructor_fields != 0 && DECL_C_BIT_FIELD (constructor_fields)
	     && DECL_NAME (constructor_fields) == 0)
	constructor_fields = TREE_CHAIN (constructor_fields);

      constructor_unfilled_fields = constructor_fields;
      constructor_bit_index = bitsize_zero_node;
    }
  else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      if (TYPE_DOMAIN (constructor_type))
	{
	  constructor_max_index
	    = TYPE_MAX_VALUE (TYPE_DOMAIN (constructor_type));

	  /* Detect non-empty initializations of zero-length arrays.  */
	  if (constructor_max_index == NULL_TREE
	      && TYPE_SIZE (constructor_type))
	    constructor_max_index = build_int_2 (-1, -1);

	  /* constructor_max_index needs to be an INTEGER_CST.  Attempts
	     to initialize VLAs will cause an proper error; avoid tree
	     checking errors as well by setting a safe value.  */
	  if (constructor_max_index
	      && TREE_CODE (constructor_max_index) != INTEGER_CST)
	    constructor_max_index = build_int_2 (-1, -1);

	  constructor_index
	    = convert (bitsizetype,
		       TYPE_MIN_VALUE (TYPE_DOMAIN (constructor_type)));
	}
      else
	constructor_index = bitsize_zero_node;

      constructor_unfilled_index = constructor_index;
    }
  else if (TREE_CODE (constructor_type) == VECTOR_TYPE)
    {
      /* Vectors are like simple fixed-size arrays.  */
      constructor_max_index =
	build_int_2 (TYPE_VECTOR_SUBPARTS (constructor_type) - 1, 0);
      constructor_index = convert (bitsizetype, integer_zero_node);
      constructor_unfilled_index = constructor_index;
    }
  else
    {
      /* Handle the case of int x = {5}; */
      constructor_fields = constructor_type;
      constructor_unfilled_fields = constructor_type;
    }
}

/* Push down into a subobject, for initialization.
   If this is for an explicit set of braces, IMPLICIT is 0.
   If it is because the next element belongs at a lower level,
   IMPLICIT is 1 (or 2 if the push is because of designator list).  */

void
push_init_level (implicit)
     int implicit;
{
  struct constructor_stack *p;
  tree value = NULL_TREE;

  /* If we've exhausted any levels that didn't have braces,
     pop them now.  */
  while (constructor_stack->implicit)
    {
      if ((TREE_CODE (constructor_type) == RECORD_TYPE
	   || TREE_CODE (constructor_type) == UNION_TYPE)
	  && constructor_fields == 0)
	process_init_element (pop_init_level (1));
      else if (TREE_CODE (constructor_type) == ARRAY_TYPE
	       && tree_int_cst_lt (constructor_max_index, constructor_index))
	process_init_element (pop_init_level (1));
      else
	break;
    }

  /* Unless this is an explicit brace, we need to preserve previous
     content if any.  */
  if (implicit)
    {
      if ((TREE_CODE (constructor_type) == RECORD_TYPE
	   || TREE_CODE (constructor_type) == UNION_TYPE)
	  && constructor_fields)
	value = find_init_member (constructor_fields);
      else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
	value = find_init_member (constructor_index);
    }

  p = (struct constructor_stack *) xmalloc (sizeof (struct constructor_stack));
  p->type = constructor_type;
  p->fields = constructor_fields;
  p->index = constructor_index;
  p->max_index = constructor_max_index;
  p->unfilled_index = constructor_unfilled_index;
  p->unfilled_fields = constructor_unfilled_fields;
  p->bit_index = constructor_bit_index;
  p->elements = constructor_elements;
  p->constant = constructor_constant;
  p->simple = constructor_simple;
  p->erroneous = constructor_erroneous;
  p->pending_elts = constructor_pending_elts;
  p->depth = constructor_depth;
  p->replacement_value = 0;
  p->implicit = implicit;
  p->outer = 0;
  p->incremental = constructor_incremental;
  p->designated = constructor_designated;
  p->next = constructor_stack;
  p->range_stack = 0;
  constructor_stack = p;

  constructor_constant = 1;
  constructor_simple = 1;
  constructor_depth = SPELLING_DEPTH ();
  constructor_elements = 0;
  constructor_incremental = 1;
  constructor_designated = 0;
  constructor_pending_elts = 0;
  if (!implicit)
    {
      p->range_stack = constructor_range_stack;
      constructor_range_stack = 0;
      designator_depth = 0;
      designator_errorneous = 0;
    }

  /* Don't die if an entire brace-pair level is superfluous
     in the containing level.  */
  if (constructor_type == 0)
    ;
  else if (TREE_CODE (constructor_type) == RECORD_TYPE
	   || TREE_CODE (constructor_type) == UNION_TYPE)
    {
      /* Don't die if there are extra init elts at the end.  */
      if (constructor_fields == 0)
	constructor_type = 0;
      else
	{
	  constructor_type = TREE_TYPE (constructor_fields);
	  push_member_name (constructor_fields);
	  constructor_depth++;
	}
    }
  else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      constructor_type = TREE_TYPE (constructor_type);
      push_array_bounds (tree_low_cst (constructor_index, 0));
      constructor_depth++;
    }

  if (constructor_type == 0)
    {
      error_init ("extra brace group at end of initializer");
      constructor_fields = 0;
      constructor_unfilled_fields = 0;
      return;
    }

  if (value && TREE_CODE (value) == CONSTRUCTOR)
    {
      constructor_constant = TREE_CONSTANT (value);
      constructor_simple = TREE_STATIC (value);
      constructor_elements = TREE_OPERAND (value, 1);
      if (constructor_elements
	  && (TREE_CODE (constructor_type) == RECORD_TYPE
	      || TREE_CODE (constructor_type) == ARRAY_TYPE))
	set_nonincremental_init ();
    }

  if (implicit == 1 && warn_missing_braces && !missing_braces_mentioned)
    {
      missing_braces_mentioned = 1;
      warning_init ("missing braces around initializer");
    }

  if (TREE_CODE (constructor_type) == RECORD_TYPE
	   || TREE_CODE (constructor_type) == UNION_TYPE)
    {
      constructor_fields = TYPE_FIELDS (constructor_type);
      /* Skip any nameless bit fields at the beginning.  */
      while (constructor_fields != 0 && DECL_C_BIT_FIELD (constructor_fields)
	     && DECL_NAME (constructor_fields) == 0)
	constructor_fields = TREE_CHAIN (constructor_fields);

      constructor_unfilled_fields = constructor_fields;
      constructor_bit_index = bitsize_zero_node;
    }
  else if (TREE_CODE (constructor_type) == VECTOR_TYPE)
    {
      /* Vectors are like simple fixed-size arrays.  */
      constructor_max_index =
	build_int_2 (TYPE_VECTOR_SUBPARTS (constructor_type) - 1, 0);
      constructor_index = convert (bitsizetype, integer_zero_node);
      constructor_unfilled_index = constructor_index;
    }
  else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      if (TYPE_DOMAIN (constructor_type))
	{
	  constructor_max_index
	    = TYPE_MAX_VALUE (TYPE_DOMAIN (constructor_type));

	  /* Detect non-empty initializations of zero-length arrays.  */
	  if (constructor_max_index == NULL_TREE
	      && TYPE_SIZE (constructor_type))
	    constructor_max_index = build_int_2 (-1, -1);

	  /* constructor_max_index needs to be an INTEGER_CST.  Attempts
	     to initialize VLAs will cause an proper error; avoid tree
	     checking errors as well by setting a safe value.  */
	  if (constructor_max_index
	      && TREE_CODE (constructor_max_index) != INTEGER_CST)
	    constructor_max_index = build_int_2 (-1, -1);

	  constructor_index
	    = convert (bitsizetype, 
		       TYPE_MIN_VALUE (TYPE_DOMAIN (constructor_type)));
	}
      else
	constructor_index = bitsize_zero_node;

      constructor_unfilled_index = constructor_index;
      if (value && TREE_CODE (value) == STRING_CST)
	{
	  /* We need to split the char/wchar array into individual
	     characters, so that we don't have to special case it
	     everywhere.  */
	  set_nonincremental_init_from_string (value);
	}
    }
  else
    {
      warning_init ("braces around scalar initializer");
      constructor_fields = constructor_type;
      constructor_unfilled_fields = constructor_type;
    }
}

/* At the end of an implicit or explicit brace level, 
   finish up that level of constructor.
   If we were outputting the elements as they are read, return 0
   from inner levels (process_init_element ignores that),
   but return error_mark_node from the outermost level
   (that's what we want to put in DECL_INITIAL).
   Otherwise, return a CONSTRUCTOR expression.  */

tree
pop_init_level (implicit)
     int implicit;
{
  struct constructor_stack *p;
  tree constructor = 0;

  if (implicit == 0)
    {
      /* When we come to an explicit close brace,
	 pop any inner levels that didn't have explicit braces.  */
      while (constructor_stack->implicit)
	process_init_element (pop_init_level (1));

      if (constructor_range_stack)
	abort ();
    }

  p = constructor_stack;

  /* Error for initializing a flexible array member, or a zero-length
     array member in an inappropriate context.  */
  if (constructor_type && constructor_fields
      && TREE_CODE (constructor_type) == ARRAY_TYPE
      && TYPE_DOMAIN (constructor_type)
      && ! TYPE_MAX_VALUE (TYPE_DOMAIN (constructor_type)))
    {
      /* Silently discard empty initializations.  The parser will
	 already have pedwarned for empty brackets.  */
      if (integer_zerop (constructor_unfilled_index))
	constructor_type = NULL_TREE;
      else if (! TYPE_SIZE (constructor_type))
	{
	  if (constructor_depth > 2)
	    error_init ("initialization of flexible array member in a nested context");
	  else if (pedantic)
	    pedwarn_init ("initialization of a flexible array member");

	  /* We have already issued an error message for the existence
	     of a flexible array member not at the end of the structure.
	     Discard the initializer so that we do not abort later.  */
	  if (TREE_CHAIN (constructor_fields) != NULL_TREE)
	    constructor_type = NULL_TREE;
	}
      else
	/* Zero-length arrays are no longer special, so we should no longer
	   get here.  */
	abort ();
    }

  /* Warn when some struct elements are implicitly initialized to zero.  */
  if (extra_warnings
      && constructor_type
      && TREE_CODE (constructor_type) == RECORD_TYPE
      && constructor_unfilled_fields)
    {
	/* Do not warn for flexible array members or zero-length arrays.  */
	while (constructor_unfilled_fields
	       && (! DECL_SIZE (constructor_unfilled_fields)
		   || integer_zerop (DECL_SIZE (constructor_unfilled_fields))))
	  constructor_unfilled_fields = TREE_CHAIN (constructor_unfilled_fields);

	/* Do not warn if this level of the initializer uses member
	   designators; it is likely to be deliberate.  */
	if (constructor_unfilled_fields && !constructor_designated)
	  {
	    push_member_name (constructor_unfilled_fields);
	    warning_init ("missing initializer");
	    RESTORE_SPELLING_DEPTH (constructor_depth);
	  }
    }

  /* Now output all pending elements.  */
  constructor_incremental = 1;
  output_pending_init_elements (1);

  /* Pad out the end of the structure.  */
  if (p->replacement_value)
    /* If this closes a superfluous brace pair,
       just pass out the element between them.  */
    constructor = p->replacement_value;
  else if (constructor_type == 0)
    ;
  else if (TREE_CODE (constructor_type) != RECORD_TYPE
	   && TREE_CODE (constructor_type) != UNION_TYPE
	   && TREE_CODE (constructor_type) != ARRAY_TYPE
	   && TREE_CODE (constructor_type) != VECTOR_TYPE)
    {
      /* A nonincremental scalar initializer--just return
	 the element, after verifying there is just one.  */
      if (constructor_elements == 0)
	{
	  if (!constructor_erroneous)
	    error_init ("empty scalar initializer");
	  constructor = error_mark_node;
	}
      else if (TREE_CHAIN (constructor_elements) != 0)
	{
	  error_init ("extra elements in scalar initializer");
	  constructor = TREE_VALUE (constructor_elements);
	}
      else
	constructor = TREE_VALUE (constructor_elements);
    }
  else
    {
      if (constructor_erroneous)
	constructor = error_mark_node;
      else
	{
	  constructor = build (CONSTRUCTOR, constructor_type, NULL_TREE,
			       nreverse (constructor_elements));
	  if (constructor_constant)
	    TREE_CONSTANT (constructor) = 1;
	  if (constructor_constant && constructor_simple)
	    TREE_STATIC (constructor) = 1;
	}
    }

  constructor_type = p->type;
  constructor_fields = p->fields;
  constructor_index = p->index;
  constructor_max_index = p->max_index;
  constructor_unfilled_index = p->unfilled_index;
  constructor_unfilled_fields = p->unfilled_fields;
  constructor_bit_index = p->bit_index;
  constructor_elements = p->elements;
  constructor_constant = p->constant;
  constructor_simple = p->simple;
  constructor_erroneous = p->erroneous;
  constructor_incremental = p->incremental;
  constructor_designated = p->designated;
  constructor_pending_elts = p->pending_elts;
  constructor_depth = p->depth;
  if (!p->implicit)
    constructor_range_stack = p->range_stack;
  RESTORE_SPELLING_DEPTH (constructor_depth);

  constructor_stack = p->next;
  free (p);

  if (constructor == 0)
    {
      if (constructor_stack == 0)
	return error_mark_node;
      return NULL_TREE;
    }
  return constructor;
}

/* Common handling for both array range and field name designators.
   ARRAY argument is non-zero for array ranges.  Returns zero for success.  */

static int
set_designator (array)
     int array;
{
  tree subtype;
  enum tree_code subcode;

  /* Don't die if an entire brace-pair level is superfluous
     in the containing level.  */
  if (constructor_type == 0)
    return 1;

  /* If there were errors in this designator list already, bail out silently.  */
  if (designator_errorneous)
    return 1;

  if (!designator_depth)
    {
      if (constructor_range_stack)
	abort ();

      /* Designator list starts at the level of closest explicit
	 braces.  */
      while (constructor_stack->implicit)
	process_init_element (pop_init_level (1));
      constructor_designated = 1;
      return 0;
    }

  if (constructor_no_implicit)
    {
      error_init ("initialization designators may not nest");
      return 1;
    }

  if (TREE_CODE (constructor_type) == RECORD_TYPE
      || TREE_CODE (constructor_type) == UNION_TYPE)
    {
      subtype = TREE_TYPE (constructor_fields);
      if (subtype != error_mark_node)
	subtype = TYPE_MAIN_VARIANT (subtype);
    }
  else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      subtype = TYPE_MAIN_VARIANT (TREE_TYPE (constructor_type));
    }
  else
    abort ();

  subcode = TREE_CODE (subtype);
  if (array && subcode != ARRAY_TYPE)
    {
      error_init ("array index in non-array initializer");
      return 1;
    }
  else if (!array && subcode != RECORD_TYPE && subcode != UNION_TYPE)
    {
      error_init ("field name not in record or union initializer");
      return 1;
    }

  constructor_designated = 1;
  push_init_level (2);
  return 0;
}

/* If there are range designators in designator list, push a new designator
   to constructor_range_stack.  RANGE_END is end of such stack range or
   NULL_TREE if there is no range designator at this level.  */

static void
push_range_stack (range_end)
     tree range_end;
{
  struct constructor_range_stack *p;

  p = (struct constructor_range_stack *)
      ggc_alloc (sizeof (struct constructor_range_stack));
  p->prev = constructor_range_stack;
  p->next = 0;
  p->fields = constructor_fields;
  p->range_start = constructor_index;
  p->index = constructor_index;
  p->stack = constructor_stack;
  p->range_end = range_end;
  if (constructor_range_stack)
    constructor_range_stack->next = p;
  constructor_range_stack = p;
}

/* Within an array initializer, specify the next index to be initialized.
   FIRST is that index.  If LAST is nonzero, then initialize a range
   of indices, running from FIRST through LAST.  */

void
set_init_index (first, last)
     tree first, last;
{
  if (set_designator (1))
    return;

  designator_errorneous = 1;

  while ((TREE_CODE (first) == NOP_EXPR
	  || TREE_CODE (first) == CONVERT_EXPR
	  || TREE_CODE (first) == NON_LVALUE_EXPR)
	 && (TYPE_MODE (TREE_TYPE (first))
	     == TYPE_MODE (TREE_TYPE (TREE_OPERAND (first, 0)))))
    first = TREE_OPERAND (first, 0);

  if (last)
    while ((TREE_CODE (last) == NOP_EXPR
	    || TREE_CODE (last) == CONVERT_EXPR
	    || TREE_CODE (last) == NON_LVALUE_EXPR)
	   && (TYPE_MODE (TREE_TYPE (last))
	       == TYPE_MODE (TREE_TYPE (TREE_OPERAND (last, 0)))))
      last = TREE_OPERAND (last, 0);

  if (TREE_CODE (first) != INTEGER_CST)
    error_init ("nonconstant array index in initializer");
  else if (last != 0 && TREE_CODE (last) != INTEGER_CST)
    error_init ("nonconstant array index in initializer");
  else if (TREE_CODE (constructor_type) != ARRAY_TYPE)
    error_init ("array index in non-array initializer");
  else if (constructor_max_index
	   && tree_int_cst_lt (constructor_max_index, first))
    error_init ("array index in initializer exceeds array bounds");
  else
    {
      constructor_index = convert (bitsizetype, first);

      if (last)
	{
	  if (tree_int_cst_equal (first, last))
	    last = 0;
	  else if (tree_int_cst_lt (last, first))
	    {
	      error_init ("empty index range in initializer");
	      last = 0;
	    }
	  else
	    {
	      last = convert (bitsizetype, last);
	      if (constructor_max_index != 0
		  && tree_int_cst_lt (constructor_max_index, last))
		{
		  error_init ("array index range in initializer exceeds array bounds");
		  last = 0;
		}
	    }
	}

      designator_depth++;
      designator_errorneous = 0;
      if (constructor_range_stack || last)
	push_range_stack (last);
    }
}

/* Within a struct initializer, specify the next field to be initialized.  */

void
set_init_label (fieldname)
     tree fieldname;
{
  tree tail;

  if (set_designator (0))
    return;

  designator_errorneous = 1;

  if (TREE_CODE (constructor_type) != RECORD_TYPE
      && TREE_CODE (constructor_type) != UNION_TYPE)
    {
      error_init ("field name not in record or union initializer");
      return;
    }
    
  for (tail = TYPE_FIELDS (constructor_type); tail;
       tail = TREE_CHAIN (tail))
    {
      if (DECL_NAME (tail) == fieldname)
	break;
    }

  if (tail == 0)
    error ("unknown field `%s' specified in initializer",
	   IDENTIFIER_POINTER (fieldname));
  else
    {
      constructor_fields = tail;
      designator_depth++;
      designator_errorneous = 0;
      if (constructor_range_stack)
	push_range_stack (NULL_TREE);
    }
}

/* Add a new initializer to the tree of pending initializers.  PURPOSE
   identifies the initializer, either array index or field in a structure. 
   VALUE is the value of that index or field.  */

static void
add_pending_init (purpose, value)
     tree purpose, value;
{
  struct init_node *p, **q, *r;

  q = &constructor_pending_elts;
  p = 0;

  if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      while (*q != 0)
	{
	  p = *q;
	  if (tree_int_cst_lt (purpose, p->purpose))
	    q = &p->left;
	  else if (tree_int_cst_lt (p->purpose, purpose))
	    q = &p->right;
	  else
	    {
	      if (TREE_SIDE_EFFECTS (p->value))
		warning_init ("initialized field with side-effects overwritten");
	      p->value = value;
	      return;
	    }
	}
    }
  else
    {
      tree bitpos;

      bitpos = bit_position (purpose);
      while (*q != NULL)
	{
	  p = *q;
	  if (tree_int_cst_lt (bitpos, bit_position (p->purpose)))
	    q = &p->left;
	  else if (p->purpose != purpose)
	    q = &p->right;
	  else
	    {
	      if (TREE_SIDE_EFFECTS (p->value))
		warning_init ("initialized field with side-effects overwritten");
	      p->value = value;
	      return;
	    }
	}
    }

  r = (struct init_node *) ggc_alloc (sizeof (struct init_node));
  r->purpose = purpose;
  r->value = value;

  *q = r;
  r->parent = p;
  r->left = 0;
  r->right = 0;
  r->balance = 0;

  while (p)
    {
      struct init_node *s;

      if (r == p->left)
	{
	  if (p->balance == 0)
	    p->balance = -1;
	  else if (p->balance < 0)
	    {
	      if (r->balance < 0)
		{
		  /* L rotation.  */
		  p->left = r->right;
		  if (p->left)
		    p->left->parent = p;
		  r->right = p;

		  p->balance = 0;
		  r->balance = 0;

		  s = p->parent;
		  p->parent = r;
		  r->parent = s;
		  if (s)
		    {
		      if (s->left == p)
			s->left = r;
		      else
			s->right = r;
		    }
		  else
		    constructor_pending_elts = r;
		}
	      else
		{
		  /* LR rotation.  */
		  struct init_node *t = r->right;

		  r->right = t->left;
		  if (r->right)
		    r->right->parent = r;
		  t->left = r;

		  p->left = t->right;
		  if (p->left)
		    p->left->parent = p;
		  t->right = p;

		  p->balance = t->balance < 0;
		  r->balance = -(t->balance > 0);
		  t->balance = 0;

		  s = p->parent;
		  p->parent = t;
		  r->parent = t;
		  t->parent = s;
		  if (s)
		    {
		      if (s->left == p)
			s->left = t;
		      else
			s->right = t;
		    }
		  else
		    constructor_pending_elts = t;
		}
	      break;
	    }
	  else
	    {
	      /* p->balance == +1; growth of left side balances the node.  */
	      p->balance = 0;
	      break;
	    }
	}
      else /* r == p->right */
	{
	  if (p->balance == 0)
	    /* Growth propagation from right side.  */
	    p->balance++;
	  else if (p->balance > 0)
	    {
	      if (r->balance > 0)
		{
		  /* R rotation.  */
		  p->right = r->left;
		  if (p->right)
		    p->right->parent = p;
		  r->left = p;

		  p->balance = 0;
		  r->balance = 0;

		  s = p->parent;
		  p->parent = r;
		  r->parent = s;
		  if (s)
		    {
		      if (s->left == p)
			s->left = r;
		      else
			s->right = r;
		    }
		  else
		    constructor_pending_elts = r;
		}
	      else /* r->balance == -1 */
		{
		  /* RL rotation */
		  struct init_node *t = r->left;

		  r->left = t->right;
		  if (r->left)
		    r->left->parent = r;
		  t->right = r;

		  p->right = t->left;
		  if (p->right)
		    p->right->parent = p;
		  t->left = p;

		  r->balance = (t->balance < 0);
		  p->balance = -(t->balance > 0);
		  t->balance = 0;

		  s = p->parent;
		  p->parent = t;
		  r->parent = t;
		  t->parent = s;
		  if (s)
		    {
		      if (s->left == p)
			s->left = t;
		      else
			s->right = t;
		    }
		  else
		    constructor_pending_elts = t;
		}
	      break;
	    }
	  else
	    {
	      /* p->balance == -1; growth of right side balances the node.  */
	      p->balance = 0;
	      break;
	    }
	}

      r = p;
      p = p->parent;
    }
}

/* Build AVL tree from a sorted chain.  */

static void
set_nonincremental_init ()
{
  tree chain;

  if (TREE_CODE (constructor_type) != RECORD_TYPE
      && TREE_CODE (constructor_type) != ARRAY_TYPE)
    return;

  for (chain = constructor_elements; chain; chain = TREE_CHAIN (chain))
    add_pending_init (TREE_PURPOSE (chain), TREE_VALUE (chain));
  constructor_elements = 0;
  if (TREE_CODE (constructor_type) == RECORD_TYPE)
    {
      constructor_unfilled_fields = TYPE_FIELDS (constructor_type);
      /* Skip any nameless bit fields at the beginning.  */
      while (constructor_unfilled_fields != 0
	     && DECL_C_BIT_FIELD (constructor_unfilled_fields)
	     && DECL_NAME (constructor_unfilled_fields) == 0)
	constructor_unfilled_fields = TREE_CHAIN (constructor_unfilled_fields);
      
    }
  else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      if (TYPE_DOMAIN (constructor_type))
	constructor_unfilled_index
	    = convert (bitsizetype,
		       TYPE_MIN_VALUE (TYPE_DOMAIN (constructor_type)));
      else
	constructor_unfilled_index = bitsize_zero_node;
    }
  constructor_incremental = 0;
}

/* Build AVL tree from a string constant.  */

static void
set_nonincremental_init_from_string (str)
     tree str;
{
  tree value, purpose, type;
  HOST_WIDE_INT val[2];
  const char *p, *end;
  int byte, wchar_bytes, charwidth, bitpos;

  if (TREE_CODE (constructor_type) != ARRAY_TYPE)
    abort ();

  if (TYPE_PRECISION (TREE_TYPE (TREE_TYPE (str)))
      == TYPE_PRECISION (char_type_node))
    wchar_bytes = 1;
  else if (TYPE_PRECISION (TREE_TYPE (TREE_TYPE (str)))
	   == TYPE_PRECISION (wchar_type_node))
    wchar_bytes = TYPE_PRECISION (wchar_type_node) / BITS_PER_UNIT;
  else
    abort ();

  charwidth = TYPE_PRECISION (char_type_node);
  type = TREE_TYPE (constructor_type);
  p = TREE_STRING_POINTER (str);
  end = p + TREE_STRING_LENGTH (str);

  for (purpose = bitsize_zero_node;
       p < end && !tree_int_cst_lt (constructor_max_index, purpose);
       purpose = size_binop (PLUS_EXPR, purpose, bitsize_one_node))
    {
      if (wchar_bytes == 1)
	{
	  val[1] = (unsigned char) *p++;
	  val[0] = 0;
	}
      else
	{
	  val[0] = 0;
	  val[1] = 0;
	  for (byte = 0; byte < wchar_bytes; byte++)
	    {
	      if (BYTES_BIG_ENDIAN)
		bitpos = (wchar_bytes - byte - 1) * charwidth;
	      else
		bitpos = byte * charwidth;
	      val[bitpos < HOST_BITS_PER_WIDE_INT]
		|= ((unsigned HOST_WIDE_INT) ((unsigned char) *p++))
		   << (bitpos % HOST_BITS_PER_WIDE_INT);
	    }
	}

      if (!TREE_UNSIGNED (type))
	{
	  bitpos = ((wchar_bytes - 1) * charwidth) + HOST_BITS_PER_CHAR;
	  if (bitpos < HOST_BITS_PER_WIDE_INT)
	    {
	      if (val[1] & (((HOST_WIDE_INT) 1) << (bitpos - 1)))
		{
		  val[1] |= ((HOST_WIDE_INT) -1) << bitpos;
		  val[0] = -1;
		}
	    }
	  else if (bitpos == HOST_BITS_PER_WIDE_INT)
	    {
	      if (val[1] < 0)
	        val[0] = -1;
	    }
	  else if (val[0] & (((HOST_WIDE_INT) 1)
			     << (bitpos - 1 - HOST_BITS_PER_WIDE_INT)))
	    val[0] |= ((HOST_WIDE_INT) -1)
		      << (bitpos - HOST_BITS_PER_WIDE_INT);
	}

      value = build_int_2 (val[1], val[0]);
      TREE_TYPE (value) = type;
      add_pending_init (purpose, value);
    }

  constructor_incremental = 0;
}

/* Return value of FIELD in pending initializer or zero if the field was
   not initialized yet.  */

static tree
find_init_member (field)
     tree field;
{
  struct init_node *p;

  if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    {
      if (constructor_incremental
	  && tree_int_cst_lt (field, constructor_unfilled_index))
	set_nonincremental_init ();

      p = constructor_pending_elts;
      while (p)
	{
	  if (tree_int_cst_lt (field, p->purpose))
	    p = p->left;
	  else if (tree_int_cst_lt (p->purpose, field))
	    p = p->right;
	  else
	    return p->value;
	}
    }
  else if (TREE_CODE (constructor_type) == RECORD_TYPE)
    {
      tree bitpos = bit_position (field);

      if (constructor_incremental
	  && (!constructor_unfilled_fields
	      || tree_int_cst_lt (bitpos,
				  bit_position (constructor_unfilled_fields))))
	set_nonincremental_init ();

      p = constructor_pending_elts;
      while (p)
	{
	  if (field == p->purpose)
	    return p->value;
	  else if (tree_int_cst_lt (bitpos, bit_position (p->purpose)))
	    p = p->left;
	  else
	    p = p->right;
	}
    }
  else if (TREE_CODE (constructor_type) == UNION_TYPE)
    {
      if (constructor_elements
	  && TREE_PURPOSE (constructor_elements) == field)
	return TREE_VALUE (constructor_elements);
    }
  return 0;
}

/* "Output" the next constructor element.
   At top level, really output it to assembler code now.
   Otherwise, collect it in a list from which we will make a CONSTRUCTOR.
   TYPE is the data type that the containing data type wants here.
   FIELD is the field (a FIELD_DECL) or the index that this element fills.

   PENDING if non-nil means output pending elements that belong
   right after this element.  (PENDING is normally 1;
   it is 0 while outputting pending elements, to avoid recursion.)  */

static void
output_init_element (value, type, field, pending)
     tree value, type, field;
     int pending;
{
  if (TREE_CODE (TREE_TYPE (value)) == FUNCTION_TYPE
      || (TREE_CODE (TREE_TYPE (value)) == ARRAY_TYPE
	  && !(TREE_CODE (value) == STRING_CST
	       && TREE_CODE (type) == ARRAY_TYPE
	       && TREE_CODE (TREE_TYPE (type)) == INTEGER_TYPE)
	  && !comptypes (TYPE_MAIN_VARIANT (TREE_TYPE (value)),
			 TYPE_MAIN_VARIANT (type))))
    value = default_conversion (value);

  if (TREE_CODE (value) == COMPOUND_LITERAL_EXPR
      && require_constant_value && !flag_isoc99 && pending)
    {
      /* As an extension, allow initializing objects with static storage
	 duration with compound literals (which are then treated just as
	 the brace enclosed list they contain).  */
      tree decl = COMPOUND_LITERAL_EXPR_DECL (value);
      value = DECL_INITIAL (decl);
    }

  if (value == error_mark_node)
    constructor_erroneous = 1;
  else if (!TREE_CONSTANT (value))
    constructor_constant = 0;
  else if (initializer_constant_valid_p (value, TREE_TYPE (value)) == 0
	   || ((TREE_CODE (constructor_type) == RECORD_TYPE
		|| TREE_CODE (constructor_type) == UNION_TYPE)
	       && DECL_C_BIT_FIELD (field)
	       && TREE_CODE (value) != INTEGER_CST))
    constructor_simple = 0;

  if (require_constant_value && ! TREE_CONSTANT (value))
    {
      error_init ("initializer element is not constant");
      value = error_mark_node;
    }
  else if (require_constant_elements
	   && initializer_constant_valid_p (value, TREE_TYPE (value)) == 0)
    pedwarn ("initializer element is not computable at load time");

  /* If this field is empty (and not at the end of structure),
     don't do anything other than checking the initializer.  */
  if (field
      && (TREE_TYPE (field) == error_mark_node
	  || (COMPLETE_TYPE_P (TREE_TYPE (field))
	      && integer_zerop (TYPE_SIZE (TREE_TYPE (field)))
	      && (TREE_CODE (constructor_type) == ARRAY_TYPE
		  || TREE_CHAIN (field)))))
    return;

  value = digest_init (type, value, require_constant_value,
		       require_constant_elements);
  if (value == error_mark_node)
    {
      constructor_erroneous = 1;
      return;
    }

  /* If this element doesn't come next in sequence,
     put it on constructor_pending_elts.  */
  if (TREE_CODE (constructor_type) == ARRAY_TYPE
      && (!constructor_incremental
	  || !tree_int_cst_equal (field, constructor_unfilled_index)))
    {
      if (constructor_incremental
	  && tree_int_cst_lt (field, constructor_unfilled_index))
	set_nonincremental_init ();

      add_pending_init (field, value);
      return;
    }
  else if (TREE_CODE (constructor_type) == RECORD_TYPE
	   && (!constructor_incremental
	       || field != constructor_unfilled_fields))
    {
      /* We do this for records but not for unions.  In a union,
	 no matter which field is specified, it can be initialized
	 right away since it starts at the beginning of the union.  */
      if (constructor_incremental)
	{
	  if (!constructor_unfilled_fields)
	    set_nonincremental_init ();
	  else
	    {
	      tree bitpos, unfillpos;

	      bitpos = bit_position (field);
	      unfillpos = bit_position (constructor_unfilled_fields);

	      if (tree_int_cst_lt (bitpos, unfillpos))
		set_nonincremental_init ();
	    }
	}

      add_pending_init (field, value);
      return;
    }
  else if (TREE_CODE (constructor_type) == UNION_TYPE
	   && constructor_elements)
    {
      if (TREE_SIDE_EFFECTS (TREE_VALUE (constructor_elements)))
	warning_init ("initialized field with side-effects overwritten");

      /* We can have just one union field set.  */
      constructor_elements = 0;
    }

  /* Otherwise, output this element either to
     constructor_elements or to the assembler file.  */

  if (field && TREE_CODE (field) == INTEGER_CST)
    field = copy_node (field);
  constructor_elements
    = tree_cons (field, value, constructor_elements);

  /* Advance the variable that indicates sequential elements output.  */
  if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    constructor_unfilled_index
      = size_binop (PLUS_EXPR, constructor_unfilled_index,
		    bitsize_one_node);
  else if (TREE_CODE (constructor_type) == RECORD_TYPE)
    {
      constructor_unfilled_fields
	= TREE_CHAIN (constructor_unfilled_fields);

      /* Skip any nameless bit fields.  */
      while (constructor_unfilled_fields != 0
	     && DECL_C_BIT_FIELD (constructor_unfilled_fields)
	     && DECL_NAME (constructor_unfilled_fields) == 0)
	constructor_unfilled_fields =
	  TREE_CHAIN (constructor_unfilled_fields);
    }
  else if (TREE_CODE (constructor_type) == UNION_TYPE)
    constructor_unfilled_fields = 0;

  /* Now output any pending elements which have become next.  */
  if (pending)
    output_pending_init_elements (0);
}

/* Output any pending elements which have become next.
   As we output elements, constructor_unfilled_{fields,index}
   advances, which may cause other elements to become next;
   if so, they too are output.

   If ALL is 0, we return when there are
   no more pending elements to output now.

   If ALL is 1, we output space as necessary so that
   we can output all the pending elements.  */

static void
output_pending_init_elements (all)
     int all;
{
  struct init_node *elt = constructor_pending_elts;
  tree next;

 retry:

  /* Look thru the whole pending tree.
     If we find an element that should be output now,
     output it.  Otherwise, set NEXT to the element
     that comes first among those still pending.  */
     
  next = 0;
  while (elt)
    {
      if (TREE_CODE (constructor_type) == ARRAY_TYPE)
	{
	  if (tree_int_cst_equal (elt->purpose,
				  constructor_unfilled_index))
	    output_init_element (elt->value,
				 TREE_TYPE (constructor_type),
				 constructor_unfilled_index, 0);
	  else if (tree_int_cst_lt (constructor_unfilled_index,
				    elt->purpose))
	    {
	      /* Advance to the next smaller node.  */
	      if (elt->left)
		elt = elt->left;
	      else
		{
		  /* We have reached the smallest node bigger than the
		     current unfilled index.  Fill the space first.  */
		  next = elt->purpose;
		  break;
		}
	    }
	  else
	    {
	      /* Advance to the next bigger node.  */
	      if (elt->right)
		elt = elt->right;
	      else
		{
		  /* We have reached the biggest node in a subtree.  Find
		     the parent of it, which is the next bigger node.  */
		  while (elt->parent && elt->parent->right == elt)
		    elt = elt->parent;
		  elt = elt->parent;
		  if (elt && tree_int_cst_lt (constructor_unfilled_index,
					      elt->purpose))
		    {
		      next = elt->purpose;
		      break;
		    }
		}
	    }
	}
      else if (TREE_CODE (constructor_type) == RECORD_TYPE
	       || TREE_CODE (constructor_type) == UNION_TYPE)
	{
	  tree ctor_unfilled_bitpos, elt_bitpos;

	  /* If the current record is complete we are done.  */
	  if (constructor_unfilled_fields == 0)
	    break;

	  ctor_unfilled_bitpos = bit_position (constructor_unfilled_fields);
	  elt_bitpos = bit_position (elt->purpose);
	  /* We can't compare fields here because there might be empty
	     fields in between.  */
	  if (tree_int_cst_equal (elt_bitpos, ctor_unfilled_bitpos))
	    {
	      constructor_unfilled_fields = elt->purpose;
	      output_init_element (elt->value, TREE_TYPE (elt->purpose),
				   elt->purpose, 0);
	    }
	  else if (tree_int_cst_lt (ctor_unfilled_bitpos, elt_bitpos))
	    {
	      /* Advance to the next smaller node.  */
	      if (elt->left)
		elt = elt->left;
	      else
		{
		  /* We have reached the smallest node bigger than the
		     current unfilled field.  Fill the space first.  */
		  next = elt->purpose;
		  break;
		}
	    }
	  else
	    {
	      /* Advance to the next bigger node.  */
	      if (elt->right)
		elt = elt->right;
	      else
		{
		  /* We have reached the biggest node in a subtree.  Find
		     the parent of it, which is the next bigger node.  */
		  while (elt->parent && elt->parent->right == elt)
		    elt = elt->parent;
		  elt = elt->parent;
		  if (elt
		      && (tree_int_cst_lt (ctor_unfilled_bitpos,
					   bit_position (elt->purpose))))
		    {
		      next = elt->purpose;
		      break;
		    }
		}
	    }
	}
    }

  /* Ordinarily return, but not if we want to output all
     and there are elements left.  */
  if (! (all && next != 0))
    return;

  /* If it's not incremental, just skip over the gap, so that after
     jumping to retry we will output the next successive element.  */
  if (TREE_CODE (constructor_type) == RECORD_TYPE
      || TREE_CODE (constructor_type) == UNION_TYPE)
    constructor_unfilled_fields = next;
  else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
    constructor_unfilled_index = next;

  /* ELT now points to the node in the pending tree with the next
     initializer to output.  */
  goto retry;
}

/* Add one non-braced element to the current constructor level.
   This adjusts the current position within the constructor's type.
   This may also start or terminate implicit levels
   to handle a partly-braced initializer.

   Once this has found the correct level for the new element,
   it calls output_init_element.  */

void
process_init_element (value)
     tree value;
{
  tree orig_value = value;
  int string_flag = value != 0 && TREE_CODE (value) == STRING_CST;

  designator_depth = 0;
  designator_errorneous = 0;

  /* Handle superfluous braces around string cst as in
     char x[] = {"foo"}; */
  if (string_flag
      && constructor_type
      && TREE_CODE (constructor_type) == ARRAY_TYPE
      && TREE_CODE (TREE_TYPE (constructor_type)) == INTEGER_TYPE
      && integer_zerop (constructor_unfilled_index))
    {
      if (constructor_stack->replacement_value)
        error_init ("excess elements in char array initializer");
      constructor_stack->replacement_value = value;
      return;
    }

  if (constructor_stack->replacement_value != 0)
    {
      error_init ("excess elements in struct initializer");
      return;
    }

  /* Ignore elements of a brace group if it is entirely superfluous
     and has already been diagnosed.  */
  if (constructor_type == 0)
    return;

  /* If we've exhausted any levels that didn't have braces,
     pop them now.  */
  while (constructor_stack->implicit)
    {
      if ((TREE_CODE (constructor_type) == RECORD_TYPE
	   || TREE_CODE (constructor_type) == UNION_TYPE)
	  && constructor_fields == 0)
	process_init_element (pop_init_level (1));
      else if (TREE_CODE (constructor_type) == ARRAY_TYPE
	       && (constructor_max_index == 0
		   || tree_int_cst_lt (constructor_max_index,
				       constructor_index)))
	process_init_element (pop_init_level (1));
      else
	break;
    }

  /* In the case of [LO ... HI] = VALUE, only evaluate VALUE once.  */
  if (constructor_range_stack)
    {
      /* If value is a compound literal and we'll be just using its
	 content, don't put it into a SAVE_EXPR.  */
      if (TREE_CODE (value) != COMPOUND_LITERAL_EXPR
	  || !require_constant_value
	  || flag_isoc99)
	value = save_expr (value);
    }

  while (1)
    {
      if (TREE_CODE (constructor_type) == RECORD_TYPE)
	{
	  tree fieldtype;
	  enum tree_code fieldcode;

	  if (constructor_fields == 0)
	    {
	      pedwarn_init ("excess elements in struct initializer");
	      break;
	    }

	  fieldtype = TREE_TYPE (constructor_fields);
	  if (fieldtype != error_mark_node)
	    fieldtype = TYPE_MAIN_VARIANT (fieldtype);
	  fieldcode = TREE_CODE (fieldtype);

	  /* Error for non-static initialization of a flexible array member.  */
	  if (fieldcode == ARRAY_TYPE
	      && !require_constant_value
	      && TYPE_SIZE (fieldtype) == NULL_TREE
	      && TREE_CHAIN (constructor_fields) == NULL_TREE)
	    {
	      error_init ("non-static initialization of a flexible array member");
	      break;
	    }

	  /* Accept a string constant to initialize a subarray.  */
	  if (value != 0
	      && fieldcode == ARRAY_TYPE
	      && TREE_CODE (TREE_TYPE (fieldtype)) == INTEGER_TYPE
	      && string_flag)
	    value = orig_value;
	  /* Otherwise, if we have come to a subaggregate,
	     and we don't have an element of its type, push into it.  */
	  else if (value != 0 && !constructor_no_implicit
		   && value != error_mark_node
		   && TYPE_MAIN_VARIANT (TREE_TYPE (value)) != fieldtype
		   && (fieldcode == RECORD_TYPE || fieldcode == ARRAY_TYPE
		       || fieldcode == UNION_TYPE))
	    {
	      push_init_level (1);
	      continue;
	    }

	  if (value)
	    {
	      push_member_name (constructor_fields);
	      output_init_element (value, fieldtype, constructor_fields, 1);
	      RESTORE_SPELLING_DEPTH (constructor_depth);
	    }
	  else
	    /* Do the bookkeeping for an element that was
	       directly output as a constructor.  */
	    {
	      /* For a record, keep track of end position of last field.  */
	      if (DECL_SIZE (constructor_fields))
	        constructor_bit_index
		  = size_binop (PLUS_EXPR,
			        bit_position (constructor_fields),
			        DECL_SIZE (constructor_fields));

	      constructor_unfilled_fields = TREE_CHAIN (constructor_fields);
	      /* Skip any nameless bit fields.  */
	      while (constructor_unfilled_fields != 0
		     && DECL_C_BIT_FIELD (constructor_unfilled_fields)
		     && DECL_NAME (constructor_unfilled_fields) == 0)
		constructor_unfilled_fields =
		  TREE_CHAIN (constructor_unfilled_fields);
	    }

	  constructor_fields = TREE_CHAIN (constructor_fields);
	  /* Skip any nameless bit fields at the beginning.  */
	  while (constructor_fields != 0
		 && DECL_C_BIT_FIELD (constructor_fields)
		 && DECL_NAME (constructor_fields) == 0)
	    constructor_fields = TREE_CHAIN (constructor_fields);
	}
      else if (TREE_CODE (constructor_type) == UNION_TYPE)
	{
	  tree fieldtype;
	  enum tree_code fieldcode;

	  if (constructor_fields == 0)
	    {
	      pedwarn_init ("excess elements in union initializer");
	      break;
	    }

	  fieldtype = TREE_TYPE (constructor_fields);
	  if (fieldtype != error_mark_node)
	    fieldtype = TYPE_MAIN_VARIANT (fieldtype);
	  fieldcode = TREE_CODE (fieldtype);

	  /* Warn that traditional C rejects initialization of unions.
	     We skip the warning if the value is zero.  This is done
	     under the assumption that the zero initializer in user
	     code appears conditioned on e.g. __STDC__ to avoid
	     "missing initializer" warnings and relies on default
	     initialization to zero in the traditional C case.
	     We also skip the warning if the initializer is designated,
	     again on the assumption that this must be conditional on
	     __STDC__ anyway (and we've already complained about the
	     member-designator already).  */
	  if (warn_traditional && !in_system_header && !constructor_designated
	      && !(value && (integer_zerop (value) || real_zerop (value))))
	    warning ("traditional C rejects initialization of unions");

	  /* Accept a string constant to initialize a subarray.  */
	  if (value != 0
	      && fieldcode == ARRAY_TYPE
	      && TREE_CODE (TREE_TYPE (fieldtype)) == INTEGER_TYPE
	      && string_flag)
	    value = orig_value;
	  /* Otherwise, if we have come to a subaggregate,
	     and we don't have an element of its type, push into it.  */
	  else if (value != 0 && !constructor_no_implicit
		   && value != error_mark_node
		   && TYPE_MAIN_VARIANT (TREE_TYPE (value)) != fieldtype
		   && (fieldcode == RECORD_TYPE || fieldcode == ARRAY_TYPE
		       || fieldcode == UNION_TYPE))
	    {
	      push_init_level (1);
	      continue;
	    }

	  if (value)
	    {
	      push_member_name (constructor_fields);
	      output_init_element (value, fieldtype, constructor_fields, 1);
	      RESTORE_SPELLING_DEPTH (constructor_depth);
	    }
	  else
	    /* Do the bookkeeping for an element that was
	       directly output as a constructor.  */
	    {
	      constructor_bit_index = DECL_SIZE (constructor_fields);
	      constructor_unfilled_fields = TREE_CHAIN (constructor_fields);
	    }

	  constructor_fields = 0;
	}
      else if (TREE_CODE (constructor_type) == ARRAY_TYPE)
	{
	  tree elttype = TYPE_MAIN_VARIANT (TREE_TYPE (constructor_type));
	  enum tree_code eltcode = TREE_CODE (elttype);

	  /* Accept a string constant to initialize a subarray.  */
	  if (value != 0
	      && eltcode == ARRAY_TYPE
	      && TREE_CODE (TREE_TYPE (elttype)) == INTEGER_TYPE
	      && string_flag)
	    value = orig_value;
	  /* Otherwise, if we have come to a subaggregate,
	     and we don't have an element of its type, push into it.  */
	  else if (value != 0 && !constructor_no_implicit
		   && value != error_mark_node
		   && TYPE_MAIN_VARIANT (TREE_TYPE (value)) != elttype
		   && (eltcode == RECORD_TYPE || eltcode == ARRAY_TYPE
		       || eltcode == UNION_TYPE))
	    {
	      push_init_level (1);
	      continue;
	    }

	  if (constructor_max_index != 0
	      && (tree_int_cst_lt (constructor_max_index, constructor_index)
		  || integer_all_onesp (constructor_max_index)))
	    {
	      pedwarn_init ("excess elements in array initializer");
	      break;
	    }

	  /* Now output the actual element.  */
	  if (value)
	    {
	      push_array_bounds (tree_low_cst (constructor_index, 0));
	      output_init_element (value, elttype, constructor_index, 1);
	      RESTORE_SPELLING_DEPTH (constructor_depth);
	    }

	  constructor_index
	    = size_binop (PLUS_EXPR, constructor_index, bitsize_one_node);

	  if (! value)
	    /* If we are doing the bookkeeping for an element that was
	       directly output as a constructor, we must update
	       constructor_unfilled_index.  */
	    constructor_unfilled_index = constructor_index;
	}
      else if (TREE_CODE (constructor_type) == VECTOR_TYPE)
	{
	  tree elttype = TYPE_MAIN_VARIANT (TREE_TYPE (constructor_type));

         /* Do a basic check of initializer size.  Note that vectors
            always have a fixed size derived from their type.  */
	  if (tree_int_cst_lt (constructor_max_index, constructor_index))
	    {
	      pedwarn_init ("excess elements in vector initializer");
	      break;
	    }

	  /* Now output the actual element.  */
	  if (value)
	    output_init_element (value, elttype, constructor_index, 1);

	  constructor_index
	    = size_binop (PLUS_EXPR, constructor_index, bitsize_one_node);

	  if (! value)
	    /* If we are doing the bookkeeping for an element that was
	       directly output as a constructor, we must update
	       constructor_unfilled_index.  */
	    constructor_unfilled_index = constructor_index;
	}

      /* Handle the sole element allowed in a braced initializer
	 for a scalar variable.  */
      else if (constructor_fields == 0)
	{
	  pedwarn_init ("excess elements in scalar initializer");
	  break;
	}
      else
	{
	  if (value)
	    output_init_element (value, constructor_type, NULL_TREE, 1);
	  constructor_fields = 0;
	}

      /* Handle range initializers either at this level or anywhere higher
	 in the designator stack.  */
      if (constructor_range_stack)
	{
	  struct constructor_range_stack *p, *range_stack;
	  int finish = 0;

	  range_stack = constructor_range_stack;
	  constructor_range_stack = 0;
	  while (constructor_stack != range_stack->stack)
	    {
	      if (!constructor_stack->implicit)
		abort ();
	      process_init_element (pop_init_level (1));
	    }
	  for (p = range_stack;
	       !p->range_end || tree_int_cst_equal (p->index, p->range_end);
	       p = p->prev)
	    {
	      if (!constructor_stack->implicit)
		abort ();
	      process_init_element (pop_init_level (1));
	    }

	  p->index = size_binop (PLUS_EXPR, p->index, bitsize_one_node);
	  if (tree_int_cst_equal (p->index, p->range_end) && !p->prev)
	    finish = 1;

	  while (1)
	    {
	      constructor_index = p->index;
	      constructor_fields = p->fields;
	      if (finish && p->range_end && p->index == p->range_start)
		{
		  finish = 0;
		  p->prev = 0;
		}
	      p = p->next;
	      if (!p)
		break;
	      push_init_level (2);
	      p->stack = constructor_stack;
	      if (p->range_end && tree_int_cst_equal (p->index, p->range_end))
		p->index = p->range_start;
	    }

	  if (!finish)
	    constructor_range_stack = range_stack;
	  continue;
	}

      break;
    }

  constructor_range_stack = 0;
}

/* Build a simple asm-statement, from one string literal.  */
tree
simple_asm_stmt (expr)
     tree expr;
{
  STRIP_NOPS (expr);

  if (TREE_CODE (expr) == ADDR_EXPR)
    expr = TREE_OPERAND (expr, 0);

  if (TREE_CODE (expr) == STRING_CST)
    {
      tree stmt;

      if (TREE_CHAIN (expr))
	expr = combine_strings (expr);
      stmt = add_stmt (build_stmt (ASM_STMT, NULL_TREE, expr,
				   NULL_TREE, NULL_TREE,
				   NULL_TREE));
      ASM_INPUT_P (stmt) = 1;
      return stmt;
    }

  error ("argument of `asm' is not a constant string");
  return NULL_TREE;
}

/* Build an asm-statement, whose components are a CV_QUALIFIER, a
   STRING, some OUTPUTS, some INPUTS, and some CLOBBERS.  */

tree
build_asm_stmt (cv_qualifier, string, outputs, inputs, clobbers)
     tree cv_qualifier;
     tree string;
     tree outputs;
     tree inputs;
     tree clobbers;
{
  tree tail;

  if (TREE_CHAIN (string))
    string = combine_strings (string);
  if (TREE_CODE (string) != STRING_CST)
    {
      error ("asm template is not a string constant");
      return NULL_TREE;
    }

  if (cv_qualifier != NULL_TREE
      && cv_qualifier != ridpointers[(int) RID_VOLATILE])
    {
      warning ("%s qualifier ignored on asm",
	       IDENTIFIER_POINTER (cv_qualifier));
      cv_qualifier = NULL_TREE;
    }

  /* We can remove output conversions that change the type,
     but not the mode.  */
  for (tail = outputs; tail; tail = TREE_CHAIN (tail))
    {
      tree output = TREE_VALUE (tail);

      STRIP_NOPS (output);
      TREE_VALUE (tail) = output;

      /* Allow conversions as LHS here.  build_modify_expr as called below
	 will do the right thing with them.  */
      while (TREE_CODE (output) == NOP_EXPR
	     || TREE_CODE (output) == CONVERT_EXPR
	     || TREE_CODE (output) == FLOAT_EXPR
	     || TREE_CODE (output) == FIX_TRUNC_EXPR
	     || TREE_CODE (output) == FIX_FLOOR_EXPR
	     || TREE_CODE (output) == FIX_ROUND_EXPR
	     || TREE_CODE (output) == FIX_CEIL_EXPR)
	output = TREE_OPERAND (output, 0);

      lvalue_or_else (TREE_VALUE (tail), "invalid lvalue in asm statement");
    }

  /* Remove output conversions that change the type but not the mode.  */
  for (tail = outputs; tail; tail = TREE_CHAIN (tail))
    {
      tree output = TREE_VALUE (tail);
      STRIP_NOPS (output);
      TREE_VALUE (tail) = output;
    }

  /* Perform default conversions on array and function inputs. 
     Don't do this for other types as it would screw up operands
     expected to be in memory.  */
  for (tail = inputs; tail; tail = TREE_CHAIN (tail))
    TREE_VALUE (tail) = default_function_array_conversion (TREE_VALUE (tail));

  return add_stmt (build_stmt (ASM_STMT, cv_qualifier, string,
			       outputs, inputs, clobbers));
}

/* Expand an ASM statement with operands, handling output operands
   that are not variables or INDIRECT_REFS by transforming such
   cases into cases that expand_asm_operands can handle.

   Arguments are same as for expand_asm_operands.  */

void
c_expand_asm_operands (string, outputs, inputs, clobbers, vol, filename, line)
     tree string, outputs, inputs, clobbers;
     int vol;
     const char *filename;
     int line;
{
  int noutputs = list_length (outputs);
  int i;
  /* o[I] is the place that output number I should be written.  */
  tree *o = (tree *) alloca (noutputs * sizeof (tree));
  tree tail;

  /* Record the contents of OUTPUTS before it is modified.  */
  for (i = 0, tail = outputs; tail; tail = TREE_CHAIN (tail), i++)
    o[i] = TREE_VALUE (tail);

  /* Generate the ASM_OPERANDS insn; store into the TREE_VALUEs of
     OUTPUTS some trees for where the values were actually stored.  */
  expand_asm_operands (string, outputs, inputs, clobbers, vol, filename, line);

  /* Copy all the intermediate outputs into the specified outputs.  */
  for (i = 0, tail = outputs; tail; tail = TREE_CHAIN (tail), i++)
    {
      if (o[i] != TREE_VALUE (tail))
	{
	  expand_expr (build_modify_expr (o[i], NOP_EXPR, TREE_VALUE (tail)),
		       NULL_RTX, VOIDmode, EXPAND_NORMAL);
	  free_temp_slots ();

	  /* Restore the original value so that it's correct the next
	     time we expand this function.  */
	  TREE_VALUE (tail) = o[i];
	}
      /* Detect modification of read-only values.
	 (Otherwise done by build_modify_expr.)  */
      else
	{
	  tree type = TREE_TYPE (o[i]);
	  if (TREE_READONLY (o[i])
	      || TYPE_READONLY (type)
	      || ((TREE_CODE (type) == RECORD_TYPE
		   || TREE_CODE (type) == UNION_TYPE)
		  && C_TYPE_FIELDS_READONLY (type)))
	    readonly_warning (o[i], "modification by `asm'");
	}
    }

  /* Those MODIFY_EXPRs could do autoincrements.  */
  emit_queue ();
}

/* Expand a C `return' statement.
   RETVAL is the expression for what to return,
   or a null pointer for `return;' with no value.  */

tree
c_expand_return (retval)
     tree retval;
{
  tree valtype = TREE_TYPE (TREE_TYPE (current_function_decl));

  if (TREE_THIS_VOLATILE (current_function_decl))
    warning ("function declared `noreturn' has a `return' statement");

  if (!retval)
    {
      current_function_returns_null = 1;
      if ((warn_return_type || flag_isoc99)
	  && valtype != 0 && TREE_CODE (valtype) != VOID_TYPE)
	pedwarn_c99 ("`return' with no value, in function returning non-void");
    }
  else if (valtype == 0 || TREE_CODE (valtype) == VOID_TYPE)
    {
      current_function_returns_null = 1;
      if (pedantic || TREE_CODE (TREE_TYPE (retval)) != VOID_TYPE)
	pedwarn ("`return' with a value, in function returning void");
    }
  else
    {
      tree t = convert_for_assignment (valtype, retval, _("return"),
				       NULL_TREE, NULL_TREE, 0);
      tree res = DECL_RESULT (current_function_decl);
      tree inner;

      current_function_returns_value = 1;
      if (t == error_mark_node)
	return NULL_TREE;

      inner = t = convert (TREE_TYPE (res), t);

      /* Strip any conversions, additions, and subtractions, and see if
	 we are returning the address of a local variable.  Warn if so.  */
      while (1)
	{
	  switch (TREE_CODE (inner))
	    {
	    case NOP_EXPR:   case NON_LVALUE_EXPR:  case CONVERT_EXPR:
	    case PLUS_EXPR:
	      inner = TREE_OPERAND (inner, 0);
	      continue;

	    case MINUS_EXPR:
	      /* If the second operand of the MINUS_EXPR has a pointer
		 type (or is converted from it), this may be valid, so
		 don't give a warning.  */
	      {
		tree op1 = TREE_OPERAND (inner, 1);

		while (! POINTER_TYPE_P (TREE_TYPE (op1))
		       && (TREE_CODE (op1) == NOP_EXPR
			   || TREE_CODE (op1) == NON_LVALUE_EXPR
			   || TREE_CODE (op1) == CONVERT_EXPR))
		  op1 = TREE_OPERAND (op1, 0);

		if (POINTER_TYPE_P (TREE_TYPE (op1)))
		  break;

		inner = TREE_OPERAND (inner, 0);
		continue;
	      }
	      
	    case ADDR_EXPR:
	      inner = TREE_OPERAND (inner, 0);

	      while (TREE_CODE_CLASS (TREE_CODE (inner)) == 'r')
		inner = TREE_OPERAND (inner, 0);

	      if (TREE_CODE (inner) == VAR_DECL
		  && ! DECL_EXTERNAL (inner)
		  && ! TREE_STATIC (inner)
		  && DECL_CONTEXT (inner) == current_function_decl)
		warning ("function returns address of local variable");
	      break;

	    default:
	      break;
	    }

	  break;
	}

      retval = build (MODIFY_EXPR, TREE_TYPE (res), res, t);
    }

 return add_stmt (build_return_stmt (retval));
}

struct c_switch {
  /* The SWITCH_STMT being built.  */
  tree switch_stmt;
  /* A splay-tree mapping the low element of a case range to the high
     element, or NULL_TREE if there is no high element.  Used to
     determine whether or not a new case label duplicates an old case
     label.  We need a tree, rather than simply a hash table, because
     of the GNU case range extension.  */
  splay_tree cases;
  /* The next node on the stack.  */
  struct c_switch *next;
};

/* A stack of the currently active switch statements.  The innermost
   switch statement is on the top of the stack.  There is no need to
   mark the stack for garbage collection because it is only active
   during the processing of the body of a function, and we never
   collect at that point.  */

static struct c_switch *switch_stack;

/* Start a C switch statement, testing expression EXP.  Return the new
   SWITCH_STMT.  */

tree
c_start_case (exp)
     tree exp;
{
  enum tree_code code;
  tree type, orig_type = error_mark_node;
  struct c_switch *cs;

  if (exp != error_mark_node)
    {
      code = TREE_CODE (TREE_TYPE (exp));
      orig_type = TREE_TYPE (exp);

      if (! INTEGRAL_TYPE_P (orig_type)
	  && code != ERROR_MARK)
	{
	  error ("switch quantity not an integer");
	  exp = integer_zero_node;
	}
      else
	{
	  type = TYPE_MAIN_VARIANT (TREE_TYPE (exp));

	  if (warn_traditional && !in_system_header
	      && (type == long_integer_type_node
		  || type == long_unsigned_type_node))
	    warning ("`long' switch expression not converted to `int' in ISO C");

	  exp = default_conversion (exp);
	  type = TREE_TYPE (exp);
	}
    }

  /* Add this new SWITCH_STMT to the stack.  */
  cs = (struct c_switch *) xmalloc (sizeof (*cs));
  cs->switch_stmt = build_stmt (SWITCH_STMT, exp, NULL_TREE, orig_type);
  cs->cases = splay_tree_new (case_compare, NULL, NULL);
  cs->next = switch_stack;
  switch_stack = cs;

  return add_stmt (switch_stack->switch_stmt);
}

/* Process a case label.  */

tree
do_case (low_value, high_value)
     tree low_value;
     tree high_value;
{
  tree label = NULL_TREE;

  if (switch_stack)
    {
      label = c_add_case_label (switch_stack->cases, 
				SWITCH_COND (switch_stack->switch_stmt), 
				low_value, high_value);
      if (label == error_mark_node)
	label = NULL_TREE;
    }
  else if (low_value)
    error ("case label not within a switch statement");
  else
    error ("`default' label not within a switch statement");

  return label;
}

/* Finish the switch statement.  */

void
c_finish_case ()
{
  struct c_switch *cs = switch_stack;

  RECHAIN_STMTS (cs->switch_stmt, SWITCH_BODY (cs->switch_stmt)); 

  /* Pop the stack.  */
  switch_stack = switch_stack->next;
  splay_tree_delete (cs->cases);
  free (cs);
}




/* Subroutines shared by all languages that are variants of C.
   Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
   2001, 2002 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#include "system.h"
#include "flags.h"
#include "toplev.h"
#include "output.h"
#include "c-pragma.h"
#include "rtl.h"
#include "c-common.h"
#include "tree-inline.h"
#include "diagnostic.h"
#include "tm_p.h"
#include "obstack.h"
#include "c-lex.h"
#include "cpplib.h"
cpp_reader *parse_in;		/* Declared in c-lex.h.  */

#undef WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE TYPE_PRECISION (wchar_type_node)

/* We let tm.h override the types used here, to handle trivial differences
   such as the choice of unsigned int or long unsigned int for size_t.
   When machines start needing nontrivial differences in the size type,
   it would be best to do something here to figure out automatically
   from other information what type to use.  */

#ifndef SIZE_TYPE
#define SIZE_TYPE "long unsigned int"
#endif

#ifndef WCHAR_TYPE
#define WCHAR_TYPE "int"
#endif

#ifndef PTRDIFF_TYPE
#define PTRDIFF_TYPE "long int"
#endif

#ifndef WINT_TYPE
#define WINT_TYPE "unsigned int"
#endif

#ifndef INTMAX_TYPE
#define INTMAX_TYPE ((INT_TYPE_SIZE == LONG_LONG_TYPE_SIZE)	\
		     ? "int"					\
		     : ((LONG_TYPE_SIZE == LONG_LONG_TYPE_SIZE)	\
			? "long int"				\
			: "long long int"))
#endif

#ifndef UINTMAX_TYPE
#define UINTMAX_TYPE ((INT_TYPE_SIZE == LONG_LONG_TYPE_SIZE)	\
		     ? "unsigned int"				\
		     : ((LONG_TYPE_SIZE == LONG_LONG_TYPE_SIZE)	\
			? "long unsigned int"			\
			: "long long unsigned int"))
#endif

/* The variant of the C language being processed.  */

enum c_language_kind c_language;

/* The following symbols are subsumed in the c_global_trees array, and
   listed here individually for documentation purposes.

   INTEGER_TYPE and REAL_TYPE nodes for the standard data types.

	tree short_integer_type_node;
	tree long_integer_type_node;
	tree long_long_integer_type_node;

	tree short_unsigned_type_node;
	tree long_unsigned_type_node;
	tree long_long_unsigned_type_node;

	tree boolean_type_node;
	tree boolean_false_node;
	tree boolean_true_node;

	tree ptrdiff_type_node;

	tree unsigned_char_type_node;
	tree signed_char_type_node;
	tree wchar_type_node;
	tree signed_wchar_type_node;
	tree unsigned_wchar_type_node;

	tree float_type_node;
	tree double_type_node;
	tree long_double_type_node;

	tree complex_integer_type_node;
	tree complex_float_type_node;
	tree complex_double_type_node;
	tree complex_long_double_type_node;

	tree intQI_type_node;
	tree intHI_type_node;
	tree intSI_type_node;
	tree intDI_type_node;
	tree intTI_type_node;

	tree unsigned_intQI_type_node;
	tree unsigned_intHI_type_node;
	tree unsigned_intSI_type_node;
	tree unsigned_intDI_type_node;
	tree unsigned_intTI_type_node;

	tree widest_integer_literal_type_node;
	tree widest_unsigned_literal_type_node;

   Nodes for types `void *' and `const void *'.

	tree ptr_type_node, const_ptr_type_node;

   Nodes for types `char *' and `const char *'.

	tree string_type_node, const_string_type_node;

   Type `char[SOMENUMBER]'.
   Used when an array of char is needed and the size is irrelevant.

	tree char_array_type_node;

   Type `int[SOMENUMBER]' or something like it.
   Used when an array of int needed and the size is irrelevant.

	tree int_array_type_node;

   Type `wchar_t[SOMENUMBER]' or something like it.
   Used when a wide string literal is created.

	tree wchar_array_type_node;

   Type `int ()' -- used for implicit declaration of functions.

	tree default_function_type;

   A VOID_TYPE node, packaged in a TREE_LIST.

	tree void_list_node;

  The lazily created VAR_DECLs for __FUNCTION__, __PRETTY_FUNCTION__,
  and __func__. (C doesn't generate __FUNCTION__ and__PRETTY_FUNCTION__
  VAR_DECLS, but C++ does.)

	tree function_name_decl_node;
	tree pretty_function_name_decl_node;
	tree c99_function_name_decl_node;

  Stack of nested function name VAR_DECLs.
  
	tree saved_function_name_decls;

*/

tree c_global_trees[CTI_MAX];

/* Nonzero means don't recognize the non-ANSI builtin functions.  */

int flag_no_builtin;

/* Nonzero means don't recognize the non-ANSI builtin functions.
   -ansi sets this.  */

int flag_no_nonansi_builtin;

/* Nonzero means give `double' the same size as `float'.  */

int flag_short_double;

/* Nonzero means give `wchar_t' the same size as `short'.  */

int flag_short_wchar;

/* Nonzero means warn about possible violations of sequence point rules.  */

int warn_sequence_point;

/* Nonzero means to warn about compile-time division by zero.  */
int warn_div_by_zero = 1;

/* The elements of `ridpointers' are identifier nodes for the reserved
   type names and storage classes.  It is indexed by a RID_... value.  */
tree *ridpointers;

tree (*make_fname_decl)                PARAMS ((tree, int));

/* If non-NULL, the address of a language-specific function that
   returns 1 for language-specific statement codes.  */
int (*lang_statement_code_p)           PARAMS ((enum tree_code));

/* If non-NULL, the address of a language-specific function that takes
   any action required right before expand_function_end is called.  */
void (*lang_expand_function_end)       PARAMS ((void));

/* Nonzero means the expression being parsed will never be evaluated.
   This is a count, since unevaluated expressions can nest.  */
int skip_evaluation;

/* Information about how a function name is generated.  */
struct fname_var_t
{
  tree *const decl;	/* pointer to the VAR_DECL.  */
  const unsigned rid;	/* RID number for the identifier.  */
  const int pretty;	/* How pretty is it? */
};

/* The three ways of getting then name of the current function.  */

const struct fname_var_t fname_vars[] =
{
  /* C99 compliant __func__, must be first.  */
  {&c99_function_name_decl_node, RID_C99_FUNCTION_NAME, 0},
  /* GCC __FUNCTION__ compliant.  */
  {&function_name_decl_node, RID_FUNCTION_NAME, 0},
  /* GCC __PRETTY_FUNCTION__ compliant.  */
  {&pretty_function_name_decl_node, RID_PRETTY_FUNCTION_NAME, 1},
  {NULL, 0, 0},
};

static int constant_fits_type_p		PARAMS ((tree, tree));

/* Keep a stack of if statements.  We record the number of compound
   statements seen up to the if keyword, as well as the line number
   and file of the if.  If a potentially ambiguous else is seen, that
   fact is recorded; the warning is issued when we can be sure that
   the enclosing if statement does not have an else branch.  */
typedef struct
{
  int compstmt_count;
  int line;
  const char *file;
  int needs_warning;
  tree if_stmt;
} if_elt;

static if_elt *if_stack;

/* Amount of space in the if statement stack.  */
static int if_stack_space = 0;

/* Stack pointer.  */
static int if_stack_pointer = 0;

/* Record the start of an if-then, and record the start of it
   for ambiguous else detection.

   COND is the condition for the if-then statement.

   IF_STMT is the statement node that has already been created for
   this if-then statement.  It is created before parsing the
   condition to keep line number information accurate.  */

void
c_expand_start_cond (cond, compstmt_count, if_stmt)
     tree cond;
     int compstmt_count;
     tree if_stmt;
{
  /* Make sure there is enough space on the stack.  */
  if (if_stack_space == 0)
    {
      if_stack_space = 10;
      if_stack = (if_elt *) xmalloc (10 * sizeof (if_elt));
    }
  else if (if_stack_space == if_stack_pointer)
    {
      if_stack_space += 10;
      if_stack = (if_elt *) xrealloc (if_stack, if_stack_space * sizeof (if_elt));
    }

  IF_COND (if_stmt) = cond;
  add_stmt (if_stmt);

  /* Record this if statement.  */
  if_stack[if_stack_pointer].compstmt_count = compstmt_count;
  if_stack[if_stack_pointer].file = input_filename;
  if_stack[if_stack_pointer].line = lineno;
  if_stack[if_stack_pointer].needs_warning = 0;
  if_stack[if_stack_pointer].if_stmt = if_stmt;
  if_stack_pointer++;
}

/* Called after the then-clause for an if-statement is processed.  */

void
c_finish_then ()
{
  tree if_stmt = if_stack[if_stack_pointer - 1].if_stmt;
  RECHAIN_STMTS (if_stmt, THEN_CLAUSE (if_stmt));
}

/* Record the end of an if-then.  Optionally warn if a nested
   if statement had an ambiguous else clause.  */

void
c_expand_end_cond ()
{
  if_stack_pointer--;
  if (if_stack[if_stack_pointer].needs_warning)
    warning_with_file_and_line (if_stack[if_stack_pointer].file,
				if_stack[if_stack_pointer].line,
				"suggest explicit braces to avoid ambiguous `else'");
  last_expr_type = NULL_TREE;
}

/* Called between the then-clause and the else-clause
   of an if-then-else.  */

void
c_expand_start_else ()
{
  /* An ambiguous else warning must be generated for the enclosing if
     statement, unless we see an else branch for that one, too.  */
  if (warn_parentheses
      && if_stack_pointer > 1
      && (if_stack[if_stack_pointer - 1].compstmt_count
	  == if_stack[if_stack_pointer - 2].compstmt_count))
    if_stack[if_stack_pointer - 2].needs_warning = 1;

  /* Even if a nested if statement had an else branch, it can't be
     ambiguous if this one also has an else.  So don't warn in that
     case.  Also don't warn for any if statements nested in this else.  */
  if_stack[if_stack_pointer - 1].needs_warning = 0;
  if_stack[if_stack_pointer - 1].compstmt_count--;
}

/* Called after the else-clause for an if-statement is processed.  */

void
c_finish_else ()
{
  tree if_stmt = if_stack[if_stack_pointer - 1].if_stmt;
  RECHAIN_STMTS (if_stmt, ELSE_CLAUSE (if_stmt));
}

/* Begin an if-statement.  Returns a newly created IF_STMT if
   appropriate.

   Unlike the C++ front-end, we do not call add_stmt here; it is
   probably safe to do so, but I am not very familiar with this
   code so I am being extra careful not to change its behavior
   beyond what is strictly necessary for correctness.  */

tree
c_begin_if_stmt ()
{
  tree r;
  r = build_stmt (IF_STMT, NULL_TREE, NULL_TREE, NULL_TREE);
  return r;
}

/* Begin a while statement.  Returns a newly created WHILE_STMT if
   appropriate.

   Unlike the C++ front-end, we do not call add_stmt here; it is
   probably safe to do so, but I am not very familiar with this
   code so I am being extra careful not to change its behavior
   beyond what is strictly necessary for correctness.  */

tree
c_begin_while_stmt ()
{
  tree r;
  r = build_stmt (WHILE_STMT, NULL_TREE, NULL_TREE);
  return r;
}

void
c_finish_while_stmt_cond (cond, while_stmt)
     tree while_stmt;
     tree cond;
{
  WHILE_COND (while_stmt) = cond;
}

/* Push current bindings for the function name VAR_DECLS.  */

void
start_fname_decls ()
{
  unsigned ix;
  tree saved = NULL_TREE;
  
  for (ix = 0; fname_vars[ix].decl; ix++)
    {
      tree decl = *fname_vars[ix].decl;

      if (decl)
	{
	  saved = tree_cons (decl, build_int_2 (ix, 0), saved);
	  *fname_vars[ix].decl = NULL_TREE;
	}
    }
  if (saved || saved_function_name_decls)
    /* Normally they'll have been NULL, so only push if we've got a
       stack, or they are non-NULL.  */
    saved_function_name_decls = tree_cons (saved, NULL_TREE,
					   saved_function_name_decls);
}

/* Finish up the current bindings, adding them into the
   current function's statement tree. This is done by wrapping the
   function's body in a COMPOUND_STMT containing these decls too. This
   must be done _before_ finish_stmt_tree is called. If there is no
   current function, we must be at file scope and no statements are
   involved. Pop the previous bindings.  */

void
finish_fname_decls ()
{
  unsigned ix;
  tree body = NULL_TREE;
  tree stack = saved_function_name_decls;

  for (; stack && TREE_VALUE (stack); stack = TREE_CHAIN (stack))
    body = chainon (TREE_VALUE (stack), body);
  
  if (body)
    {
      /* They were called into existence, so add to statement tree.  */
      body = chainon (body,
		      TREE_CHAIN (DECL_SAVED_TREE (current_function_decl)));
      body = build_stmt (COMPOUND_STMT, body);
      
      COMPOUND_STMT_NO_SCOPE (body) = 1;
      TREE_CHAIN (DECL_SAVED_TREE (current_function_decl)) = body;
    }
  
  for (ix = 0; fname_vars[ix].decl; ix++)
    *fname_vars[ix].decl = NULL_TREE;
  
  if (stack)
    {
      /* We had saved values, restore them.  */
      tree saved;

      for (saved = TREE_PURPOSE (stack); saved; saved = TREE_CHAIN (saved))
	{
	  tree decl = TREE_PURPOSE (saved);
	  unsigned ix = TREE_INT_CST_LOW (TREE_VALUE (saved));
	  
	  *fname_vars[ix].decl = decl;
	}
      stack = TREE_CHAIN (stack);
    }
  saved_function_name_decls = stack;
}

/* Return the text name of the current function, suitable prettified
   by PRETTY_P.  */

const char *
fname_as_string (pretty_p)
     int pretty_p;
{
  const char *name = NULL;
  
  if (pretty_p)
    name = (current_function_decl
	    ? (*decl_printable_name) (current_function_decl, 2)
	    : "top level");
  else if (current_function_decl && DECL_NAME (current_function_decl))
    name = IDENTIFIER_POINTER (DECL_NAME (current_function_decl));
  else
    name = "";
  return name;
}

/* Return the text name of the current function, formatted as
   required by the supplied RID value.  */

const char *
fname_string (rid)
     unsigned rid;
{
  unsigned ix;
  
  for (ix = 0; fname_vars[ix].decl; ix++)
    if (fname_vars[ix].rid == rid)
      break;
  return fname_as_string (fname_vars[ix].pretty);
}

/* Return the VAR_DECL for a const char array naming the current
   function. If the VAR_DECL has not yet been created, create it
   now. RID indicates how it should be formatted and IDENTIFIER_NODE
   ID is its name (unfortunately C and C++ hold the RID values of
   keywords in different places, so we can't derive RID from ID in
   this language independent code.  */

tree
fname_decl (rid, id)
     unsigned rid;
     tree id;
{
  unsigned ix;
  tree decl = NULL_TREE;

  for (ix = 0; fname_vars[ix].decl; ix++)
    if (fname_vars[ix].rid == rid)
      break;

  decl = *fname_vars[ix].decl;
  if (!decl)
    {
      tree saved_last_tree = last_tree;
      
      decl = (*make_fname_decl) (id, fname_vars[ix].pretty);
      if (last_tree != saved_last_tree)
	{
	  /* We created some statement tree for the decl. This belongs
	     at the start of the function, so remove it now and reinsert
	     it after the function is complete.  */
	  tree stmts = TREE_CHAIN (saved_last_tree);

	  TREE_CHAIN (saved_last_tree) = NULL_TREE;
	  last_tree = saved_last_tree;
	  saved_function_name_decls = tree_cons (decl, stmts,
						 saved_function_name_decls);
	}
      *fname_vars[ix].decl = decl;
    }
  if (!ix && !current_function_decl)
    pedwarn_with_decl (decl, "`%s' is not defined outside of function scope");
  
  return decl;
}

/* Given a chain of STRING_CST nodes,
   concatenate them into one STRING_CST
   and give it a suitable array-of-chars data type.  */

tree
combine_strings (strings)
     tree strings;
{
  tree value, t;
  int length = 1;
  int wide_length = 0;
  int wide_flag = 0;
  int wchar_bytes = TYPE_PRECISION (wchar_type_node) / BITS_PER_UNIT;
  int nchars;
  const int nchars_max = flag_isoc99 ? 4095 : 509;

  if (TREE_CHAIN (strings))
    {
      /* More than one in the chain, so concatenate.  */
      char *p, *q;

      /* Don't include the \0 at the end of each substring,
	 except for the last one.
	 Count wide strings and ordinary strings separately.  */
      for (t = strings; t; t = TREE_CHAIN (t))
	{
	  if (TREE_TYPE (t) == wchar_array_type_node)
	    {
	      wide_length += (TREE_STRING_LENGTH (t) - wchar_bytes);
	      wide_flag = 1;
	    }
	  else
	    {
	      length += (TREE_STRING_LENGTH (t) - 1);
	      if (C_ARTIFICIAL_STRING_P (t) && !in_system_header)
		warning ("concatenation of string literals with __FUNCTION__ is deprecated"); 
	    }
	}

      /* If anything is wide, the non-wides will be converted,
	 which makes them take more space.  */
      if (wide_flag)
	length = length * wchar_bytes + wide_length;

      p = xmalloc (length);

      /* Copy the individual strings into the new combined string.
	 If the combined string is wide, convert the chars to ints
	 for any individual strings that are not wide.  */

      q = p;
      for (t = strings; t; t = TREE_CHAIN (t))
	{
	  int len = (TREE_STRING_LENGTH (t)
		     - ((TREE_TYPE (t) == wchar_array_type_node)
			? wchar_bytes : 1));
	  if ((TREE_TYPE (t) == wchar_array_type_node) == wide_flag)
	    {
	      memcpy (q, TREE_STRING_POINTER (t), len);
	      q += len;
	    }
	  else
	    {
	      int i, j;
	      for (i = 0; i < len; i++)
		{
		  if (BYTES_BIG_ENDIAN)
		    {
		      for (j=0; j<(WCHAR_TYPE_SIZE / BITS_PER_UNIT)-1; j++)
			*q++ = 0;
		      *q++ = TREE_STRING_POINTER (t)[i];
		    }
		  else
		    {
		      *q++ = TREE_STRING_POINTER (t)[i];
		      for (j=0; j<(WCHAR_TYPE_SIZE / BITS_PER_UNIT)-1; j++)
			*q++ = 0;
		    }
		}
	    }
	}
      if (wide_flag)
	{
	  int i;
	  for (i = 0; i < wchar_bytes; i++)
	    *q++ = 0;
	}
      else
	*q = 0;

      value = build_string (length, p);
      free (p);
    }
  else
    {
      value = strings;
      length = TREE_STRING_LENGTH (value);
      if (TREE_TYPE (value) == wchar_array_type_node)
	wide_flag = 1;
    }

  /* Compute the number of elements, for the array type.  */
  nchars = wide_flag ? length / wchar_bytes : length;

  if (pedantic && nchars - 1 > nchars_max && c_language == clk_c)
    pedwarn ("string length `%d' is greater than the length `%d' ISO C%d compilers are required to support",
	     nchars - 1, nchars_max, flag_isoc99 ? 99 : 89);

  /* Create the array type for the string constant.
     -Wwrite-strings says make the string constant an array of const char
     so that copying it to a non-const pointer will get a warning.
     For C++, this is the standard behavior.  */
  if (flag_const_strings
      && (! flag_traditional  && ! flag_writable_strings))
    {
      tree elements
	= build_type_variant (wide_flag ? wchar_type_node : char_type_node,
			      1, 0);
      TREE_TYPE (value)
	= build_array_type (elements,
			    build_index_type (build_int_2 (nchars - 1, 0)));
    }
  else
    TREE_TYPE (value)
      = build_array_type (wide_flag ? wchar_type_node : char_type_node,
			  build_index_type (build_int_2 (nchars - 1, 0)));

  TREE_CONSTANT (value) = 1;
  TREE_READONLY (value) = ! flag_writable_strings;
  TREE_STATIC (value) = 1;
  return value;
}

static int is_valid_printf_arglist PARAMS ((tree));
static rtx c_expand_builtin PARAMS ((tree, rtx, enum machine_mode, enum expand_modifier));
static rtx c_expand_builtin_printf PARAMS ((tree, rtx, enum machine_mode,
					    enum expand_modifier, int, int));
static rtx c_expand_builtin_fprintf PARAMS ((tree, rtx, enum machine_mode,
					     enum expand_modifier, int, int));

/* Print a warning if a constant expression had overflow in folding.
   Invoke this function on every expression that the language
   requires to be a constant expression.
   Note the ANSI C standard says it is erroneous for a
   constant expression to overflow.  */

void
constant_expression_warning (value)
     tree value;
{
  if ((TREE_CODE (value) == INTEGER_CST || TREE_CODE (value) == REAL_CST
       || TREE_CODE (value) == VECTOR_CST
       || TREE_CODE (value) == COMPLEX_CST)
      && TREE_CONSTANT_OVERFLOW (value) && pedantic)
    pedwarn ("overflow in constant expression");
}

/* Print a warning if an expression had overflow in folding.
   Invoke this function on every expression that
   (1) appears in the source code, and
   (2) might be a constant expression that overflowed, and
   (3) is not already checked by convert_and_check;
   however, do not invoke this function on operands of explicit casts.  */

void
overflow_warning (value)
     tree value;
{
  if ((TREE_CODE (value) == INTEGER_CST
       || (TREE_CODE (value) == COMPLEX_CST
	   && TREE_CODE (TREE_REALPART (value)) == INTEGER_CST))
      && TREE_OVERFLOW (value))
    {
      TREE_OVERFLOW (value) = 0;
      if (skip_evaluation == 0)
	warning ("integer overflow in expression");
    }
  else if ((TREE_CODE (value) == REAL_CST
	    || (TREE_CODE (value) == COMPLEX_CST
		&& TREE_CODE (TREE_REALPART (value)) == REAL_CST))
	   && TREE_OVERFLOW (value))
    {
      TREE_OVERFLOW (value) = 0;
      if (skip_evaluation == 0)
	warning ("floating point overflow in expression");
    }
  else if (TREE_CODE (value) == VECTOR_CST && TREE_OVERFLOW (value))
    {
      TREE_OVERFLOW (value) = 0;
      if (skip_evaluation == 0)
	warning ("vector overflow in expression");
    }
}

/* Print a warning if a large constant is truncated to unsigned,
   or if -Wconversion is used and a constant < 0 is converted to unsigned.
   Invoke this function on every expression that might be implicitly
   converted to an unsigned type.  */

void
unsigned_conversion_warning (result, operand)
     tree result, operand;
{
  if (TREE_CODE (operand) == INTEGER_CST
      && TREE_CODE (TREE_TYPE (result)) == INTEGER_TYPE
      && TREE_UNSIGNED (TREE_TYPE (result))
      && skip_evaluation == 0
      && !int_fits_type_p (operand, TREE_TYPE (result)))
    {
      if (!int_fits_type_p (operand, signed_type (TREE_TYPE (result))))
	/* This detects cases like converting -129 or 256 to unsigned char.  */
	warning ("large integer implicitly truncated to unsigned type");
      else if (warn_conversion)
	warning ("negative integer implicitly converted to unsigned type");
    }
}

/* Nonzero if constant C has a value that is permissible
   for type TYPE (an INTEGER_TYPE).  */

static int
constant_fits_type_p (c, type)
     tree c, type;
{
  if (TREE_CODE (c) == INTEGER_CST)
    return int_fits_type_p (c, type);

  c = convert (type, c);
  return !TREE_OVERFLOW (c);
}     

/* Convert EXPR to TYPE, warning about conversion problems with constants.
   Invoke this function on every expression that is converted implicitly,
   i.e. because of language rules and not because of an explicit cast.  */

tree
convert_and_check (type, expr)
     tree type, expr;
{
  tree t = convert (type, expr);
  if (TREE_CODE (t) == INTEGER_CST)
    {
      if (TREE_OVERFLOW (t))
	{
	  TREE_OVERFLOW (t) = 0;

	  /* Do not diagnose overflow in a constant expression merely
	     because a conversion overflowed.  */
	  TREE_CONSTANT_OVERFLOW (t) = TREE_CONSTANT_OVERFLOW (expr);

	  /* No warning for converting 0x80000000 to int.  */
	  if (!(TREE_UNSIGNED (type) < TREE_UNSIGNED (TREE_TYPE (expr))
		&& TREE_CODE (TREE_TYPE (expr)) == INTEGER_TYPE
		&& TYPE_PRECISION (type) == TYPE_PRECISION (TREE_TYPE (expr))))
	    /* If EXPR fits in the unsigned version of TYPE,
	       don't warn unless pedantic.  */
	    if ((pedantic
		 || TREE_UNSIGNED (type)
		 || ! constant_fits_type_p (expr, unsigned_type (type)))
	        && skip_evaluation == 0)
	      warning ("overflow in implicit constant conversion");
	}
      else
	unsigned_conversion_warning (t, expr);
    }
  return t;
}

/* A node in a list that describes references to variables (EXPR), which are
   either read accesses if WRITER is zero, or write accesses, in which case
   WRITER is the parent of EXPR.  */
struct tlist
{
  struct tlist *next;
  tree expr, writer;
};

/* Used to implement a cache the results of a call to verify_tree.  We only
   use this for SAVE_EXPRs.  */
struct tlist_cache
{
  struct tlist_cache *next;
  struct tlist *cache_before_sp;
  struct tlist *cache_after_sp;
  tree expr;
};

/* Obstack to use when allocating tlist structures, and corresponding
   firstobj.  */
static struct obstack tlist_obstack;
static char *tlist_firstobj = 0;

/* Keep track of the identifiers we've warned about, so we can avoid duplicate
   warnings.  */
static struct tlist *warned_ids;
/* SAVE_EXPRs need special treatment.  We process them only once and then
   cache the results.  */
static struct tlist_cache *save_expr_cache;

static void add_tlist PARAMS ((struct tlist **, struct tlist *, tree, int));
static void merge_tlist PARAMS ((struct tlist **, struct tlist *, int));
static void verify_tree PARAMS ((tree, struct tlist **, struct tlist **, tree));
static int warning_candidate_p PARAMS ((tree));
static void warn_for_collisions PARAMS ((struct tlist *));
static void warn_for_collisions_1 PARAMS ((tree, tree, struct tlist *, int));
static struct tlist *new_tlist PARAMS ((struct tlist *, tree, tree));
static void verify_sequence_points PARAMS ((tree));

/* Create a new struct tlist and fill in its fields.  */
static struct tlist *
new_tlist (next, t, writer)
     struct tlist *next;
     tree t;
     tree writer;
{
  struct tlist *l;
  l = (struct tlist *) obstack_alloc (&tlist_obstack, sizeof *l);
  l->next = next;
  l->expr = t;
  l->writer = writer;
  return l;
}

/* Add duplicates of the nodes found in ADD to the list *TO.  If EXCLUDE_WRITER
   is nonnull, we ignore any node we find which has a writer equal to it.  */

static void
add_tlist (to, add, exclude_writer, copy)
     struct tlist **to;
     struct tlist *add;
     tree exclude_writer;
     int copy;
{
  while (add)
    {
      struct tlist *next = add->next;
      if (! copy)
	add->next = *to;
      if (! exclude_writer || add->writer != exclude_writer)
	*to = copy ? new_tlist (*to, add->expr, add->writer) : add;
      add = next;
    }
}

/* Merge the nodes of ADD into TO.  This merging process is done so that for
   each variable that already exists in TO, no new node is added; however if
   there is a write access recorded in ADD, and an occurrence on TO is only
   a read access, then the occurrence in TO will be modified to record the
   write.  */

static void
merge_tlist (to, add, copy)
     struct tlist **to;
     struct tlist *add;
     int copy;
{
  struct tlist **end = to;

  while (*end)
    end = &(*end)->next;

  while (add)
    {
      int found = 0;
      struct tlist *tmp2;
      struct tlist *next = add->next;

      for (tmp2 = *to; tmp2; tmp2 = tmp2->next)
	if (tmp2->expr == add->expr)
	  {
	    found = 1;
	    if (! tmp2->writer)
	      tmp2->writer = add->writer;
	  }
      if (! found)
	{
	  *end = copy ? add : new_tlist (NULL, add->expr, add->writer);
	  end = &(*end)->next;
	  *end = 0;
	}
      add = next;
    }
}

/* WRITTEN is a variable, WRITER is its parent.  Warn if any of the variable
   references in list LIST conflict with it, excluding reads if ONLY writers
   is nonzero.  */

static void
warn_for_collisions_1 (written, writer, list, only_writes)
     tree written, writer;
     struct tlist *list;
     int only_writes;
{
  struct tlist *tmp;

  /* Avoid duplicate warnings.  */
  for (tmp = warned_ids; tmp; tmp = tmp->next)
    if (tmp->expr == written)
      return;

  while (list)
    {
      if (list->expr == written
	  && list->writer != writer
	  && (! only_writes || list->writer))
	{
	  warned_ids = new_tlist (warned_ids, written, NULL_TREE);
	  warning ("operation on `%s' may be undefined",
		   IDENTIFIER_POINTER (DECL_NAME (list->expr)));
	}
      list = list->next;
    }
}

/* Given a list LIST of references to variables, find whether any of these
   can cause conflicts due to missing sequence points.  */

static void
warn_for_collisions (list)
     struct tlist *list;
{
  struct tlist *tmp;
  
  for (tmp = list; tmp; tmp = tmp->next)
    {
      if (tmp->writer)
	warn_for_collisions_1 (tmp->expr, tmp->writer, list, 0);
    }
}

/* Return nonzero if X is a tree that can be verified by the sequence point
   warnings.  */
static int
warning_candidate_p (x)
     tree x;
{
  return TREE_CODE (x) == VAR_DECL || TREE_CODE (x) == PARM_DECL;
}

/* Walk the tree X, and record accesses to variables.  If X is written by the
   parent tree, WRITER is the parent.
   We store accesses in one of the two lists: PBEFORE_SP, and PNO_SP.  If this
   expression or its only operand forces a sequence point, then everything up
   to the sequence point is stored in PBEFORE_SP.  Everything else gets stored
   in PNO_SP.
   Once we return, we will have emitted warnings if any subexpression before
   such a sequence point could be undefined.  On a higher level, however, the
   sequence point may not be relevant, and we'll merge the two lists.

   Example: (b++, a) + b;
   The call that processes the COMPOUND_EXPR will store the increment of B
   in PBEFORE_SP, and the use of A in PNO_SP.  The higher-level call that
   processes the PLUS_EXPR will need to merge the two lists so that
   eventually, all accesses end up on the same list (and we'll warn about the
   unordered subexpressions b++ and b.

   A note on merging.  If we modify the former example so that our expression
   becomes
     (b++, b) + a
   care must be taken not simply to add all three expressions into the final
   PNO_SP list.  The function merge_tlist takes care of that by merging the
   before-SP list of the COMPOUND_EXPR into its after-SP list in a special
   way, so that no more than one access to B is recorded.  */

static void
verify_tree (x, pbefore_sp, pno_sp, writer)
     tree x;
     struct tlist **pbefore_sp, **pno_sp;
     tree writer;
{
  struct tlist *tmp_before, *tmp_nosp, *tmp_list2, *tmp_list3;
  enum tree_code code;
  char class;

  /* X may be NULL if it is the operand of an empty statement expression
     ({ }).  */
  if (x == NULL)
    return;

 restart:
  code = TREE_CODE (x);
  class = TREE_CODE_CLASS (code);

  if (warning_candidate_p (x))
    {
      *pno_sp = new_tlist (*pno_sp, x, writer);
      return;
    }

  switch (code)
    {
    case CONSTRUCTOR:
      return;

    case COMPOUND_EXPR:
    case TRUTH_ANDIF_EXPR:
    case TRUTH_ORIF_EXPR:
      tmp_before = tmp_nosp = tmp_list3 = 0;
      verify_tree (TREE_OPERAND (x, 0), &tmp_before, &tmp_nosp, NULL_TREE);
      warn_for_collisions (tmp_nosp);
      merge_tlist (pbefore_sp, tmp_before, 0);
      merge_tlist (pbefore_sp, tmp_nosp, 0);
      verify_tree (TREE_OPERAND (x, 1), &tmp_list3, pno_sp, NULL_TREE);
      merge_tlist (pbefore_sp, tmp_list3, 0);
      return;

    case COND_EXPR:
      tmp_before = tmp_list2 = 0;
      verify_tree (TREE_OPERAND (x, 0), &tmp_before, &tmp_list2, NULL_TREE);
      warn_for_collisions (tmp_list2);
      merge_tlist (pbefore_sp, tmp_before, 0);
      merge_tlist (pbefore_sp, tmp_list2, 1);

      tmp_list3 = tmp_nosp = 0;
      verify_tree (TREE_OPERAND (x, 1), &tmp_list3, &tmp_nosp, NULL_TREE);
      warn_for_collisions (tmp_nosp);
      merge_tlist (pbefore_sp, tmp_list3, 0);

      tmp_list3 = tmp_list2 = 0;
      verify_tree (TREE_OPERAND (x, 2), &tmp_list3, &tmp_list2, NULL_TREE);
      warn_for_collisions (tmp_list2);
      merge_tlist (pbefore_sp, tmp_list3, 0);
      /* Rather than add both tmp_nosp and tmp_list2, we have to merge the
	 two first, to avoid warning for (a ? b++ : b++).  */
      merge_tlist (&tmp_nosp, tmp_list2, 0);
      add_tlist (pno_sp, tmp_nosp, NULL_TREE, 0);
      return;

    case PREDECREMENT_EXPR:
    case PREINCREMENT_EXPR:
    case POSTDECREMENT_EXPR:
    case POSTINCREMENT_EXPR:
      verify_tree (TREE_OPERAND (x, 0), pno_sp, pno_sp, x);
      return;

    case MODIFY_EXPR:
      tmp_before = tmp_nosp = tmp_list3 = 0;
      verify_tree (TREE_OPERAND (x, 1), &tmp_before, &tmp_nosp, NULL_TREE);
      verify_tree (TREE_OPERAND (x, 0), &tmp_list3, &tmp_list3, x);
      /* Expressions inside the LHS are not ordered wrt. the sequence points
	 in the RHS.  Example:
	   *a = (a++, 2)
	 Despite the fact that the modification of "a" is in the before_sp
	 list (tmp_before), it conflicts with the use of "a" in the LHS.
	 We can handle this by adding the contents of tmp_list3
	 to those of tmp_before, and redoing the collision warnings for that
	 list.  */
      add_tlist (&tmp_before, tmp_list3, x, 1);
      warn_for_collisions (tmp_before);
      /* Exclude the LHS itself here; we first have to merge it into the
	 tmp_nosp list.  This is done to avoid warning for "a = a"; if we
	 didn't exclude the LHS, we'd get it twice, once as a read and once
	 as a write.  */
      add_tlist (pno_sp, tmp_list3, x, 0);
      warn_for_collisions_1 (TREE_OPERAND (x, 0), x, tmp_nosp, 1);

      merge_tlist (pbefore_sp, tmp_before, 0);
      if (warning_candidate_p (TREE_OPERAND (x, 0)))
	merge_tlist (&tmp_nosp, new_tlist (NULL, TREE_OPERAND (x, 0), x), 0);
      add_tlist (pno_sp, tmp_nosp, NULL_TREE, 1);
      return;

    case CALL_EXPR:
      /* We need to warn about conflicts among arguments and conflicts between
	 args and the function address.  Side effects of the function address,
	 however, are not ordered by the sequence point of the call.  */
      tmp_before = tmp_nosp = tmp_list2 = tmp_list3 = 0;
      verify_tree (TREE_OPERAND (x, 0), &tmp_before, &tmp_nosp, NULL_TREE);
      if (TREE_OPERAND (x, 1))
	verify_tree (TREE_OPERAND (x, 1), &tmp_list2, &tmp_list3, NULL_TREE);
      merge_tlist (&tmp_list3, tmp_list2, 0);
      add_tlist (&tmp_before, tmp_list3, NULL_TREE, 0);
      add_tlist (&tmp_before, tmp_nosp, NULL_TREE, 0);
      warn_for_collisions (tmp_before);
      add_tlist (pbefore_sp, tmp_before, NULL_TREE, 0);
      return;

    case TREE_LIST:
      /* Scan all the list, e.g. indices of multi dimensional array.  */
      while (x)
	{
	  tmp_before = tmp_nosp = 0;
	  verify_tree (TREE_VALUE (x), &tmp_before, &tmp_nosp, NULL_TREE);
	  merge_tlist (&tmp_nosp, tmp_before, 0);
	  add_tlist (pno_sp, tmp_nosp, NULL_TREE, 0);
	  x = TREE_CHAIN (x);
	}
      return;

    case SAVE_EXPR:
      {
	struct tlist_cache *t;
	for (t = save_expr_cache; t; t = t->next)
	  if (t->expr == x)
	    break;

	if (! t)
	  {
	    t = (struct tlist_cache *) obstack_alloc (&tlist_obstack,
						      sizeof *t);
	    t->next = save_expr_cache;
	    t->expr = x;
	    save_expr_cache = t;

	    tmp_before = tmp_nosp = 0;
	    verify_tree (TREE_OPERAND (x, 0), &tmp_before, &tmp_nosp, NULL_TREE);
	    warn_for_collisions (tmp_nosp);

	    tmp_list3 = 0;
	    while (tmp_nosp)
	      {
		struct tlist *t = tmp_nosp;
		tmp_nosp = t->next;
		merge_tlist (&tmp_list3, t, 0);
	      }
	    t->cache_before_sp = tmp_before;
	    t->cache_after_sp = tmp_list3;
	  }
	merge_tlist (pbefore_sp, t->cache_before_sp, 1);
	add_tlist (pno_sp, t->cache_after_sp, NULL_TREE, 1);
	return;
      }
    default:
      break;
    }

  if (class == '1')
    {
      if (first_rtl_op (code) == 0)
	return;
      x = TREE_OPERAND (x, 0);
      writer = 0;
      goto restart;
    }

  switch (class)
    {
    case 'r':
    case '<':
    case '2':
    case 'b':
    case 'e':
    case 's':
    case 'x':
      {
	int lp;
	int max = first_rtl_op (TREE_CODE (x));
	for (lp = 0; lp < max; lp++)
	  {
	    tmp_before = tmp_nosp = 0;
	    verify_tree (TREE_OPERAND (x, lp), &tmp_before, &tmp_nosp, NULL_TREE);
	    merge_tlist (&tmp_nosp, tmp_before, 0);
	    add_tlist (pno_sp, tmp_nosp, NULL_TREE, 0);
	  }
	break;
      }
    }
}

/* Try to warn for undefined behaviour in EXPR due to missing sequence
   points.  */

static void
verify_sequence_points (expr)
     tree expr;
{
  struct tlist *before_sp = 0, *after_sp = 0;

  warned_ids = 0;
  save_expr_cache = 0;
  if (tlist_firstobj == 0)
    {
      gcc_obstack_init (&tlist_obstack);
      tlist_firstobj = obstack_alloc (&tlist_obstack, 0);
    }

  verify_tree (expr, &before_sp, &after_sp, 0);
  warn_for_collisions (after_sp);
  obstack_free (&tlist_obstack, tlist_firstobj);
}

tree
c_expand_expr_stmt (expr)
     tree expr;
{
  /* Do default conversion if safe and possibly important,
     in case within ({...}).  */
  if ((TREE_CODE (TREE_TYPE (expr)) == ARRAY_TYPE
       && (flag_isoc99 || lvalue_p (expr)))
      || TREE_CODE (TREE_TYPE (expr)) == FUNCTION_TYPE)
    expr = default_conversion (expr);

  if (warn_sequence_point)
    verify_sequence_points (expr);

  if (TREE_TYPE (expr) != error_mark_node
      && !COMPLETE_OR_VOID_TYPE_P (TREE_TYPE (expr))
      && TREE_CODE (TREE_TYPE (expr)) != ARRAY_TYPE)
    error ("expression statement has incomplete type");

  last_expr_type = TREE_TYPE (expr); 
  return add_stmt (build_stmt (EXPR_STMT, expr));
}

/* Validate the expression after `case' and apply default promotions.  */

tree
check_case_value (value)
     tree value;
{
  if (value == NULL_TREE)
    return value;

  /* Strip NON_LVALUE_EXPRs since we aren't using as an lvalue.  */
  STRIP_TYPE_NOPS (value);
  /* In C++, the following is allowed:

       const int i = 3;
       switch (...) { case i: ... }

     So, we try to reduce the VALUE to a constant that way.  */
  if (c_language == clk_cplusplus)
    {
      value = decl_constant_value (value);
      STRIP_TYPE_NOPS (value);
      value = fold (value);
    }

  if (TREE_CODE (value) != INTEGER_CST
      && value != error_mark_node)
    {
      error ("case label does not reduce to an integer constant");
      value = error_mark_node;
    }
  else
    /* Promote char or short to int.  */
    value = default_conversion (value);

  constant_expression_warning (value);

  return value;
}

/* Return an integer type with BITS bits of precision,
   that is unsigned if UNSIGNEDP is nonzero, otherwise signed.  */

tree
type_for_size (bits, unsignedp)
     unsigned bits;
     int unsignedp;
{
  if (bits == TYPE_PRECISION (integer_type_node))
    return unsignedp ? unsigned_type_node : integer_type_node;

  if (bits == TYPE_PRECISION (signed_char_type_node))
    return unsignedp ? unsigned_char_type_node : signed_char_type_node;

  if (bits == TYPE_PRECISION (short_integer_type_node))
    return unsignedp ? short_unsigned_type_node : short_integer_type_node;

  if (bits == TYPE_PRECISION (long_integer_type_node))
    return unsignedp ? long_unsigned_type_node : long_integer_type_node;

  if (bits == TYPE_PRECISION (long_long_integer_type_node))
    return (unsignedp ? long_long_unsigned_type_node
	    : long_long_integer_type_node);

  if (bits == TYPE_PRECISION (widest_integer_literal_type_node))
    return (unsignedp ? widest_unsigned_literal_type_node
	    : widest_integer_literal_type_node);

  if (bits <= TYPE_PRECISION (intQI_type_node))
    return unsignedp ? unsigned_intQI_type_node : intQI_type_node;

  if (bits <= TYPE_PRECISION (intHI_type_node))
    return unsignedp ? unsigned_intHI_type_node : intHI_type_node;

  if (bits <= TYPE_PRECISION (intSI_type_node))
    return unsignedp ? unsigned_intSI_type_node : intSI_type_node;

  if (bits <= TYPE_PRECISION (intDI_type_node))
    return unsignedp ? unsigned_intDI_type_node : intDI_type_node;

  return 0;
}

/* Return a data type that has machine mode MODE.
   If the mode is an integer,
   then UNSIGNEDP selects between signed and unsigned types.  */

tree
type_for_mode (mode, unsignedp)
     enum machine_mode mode;
     int unsignedp;
{
  if (mode == TYPE_MODE (integer_type_node))
    return unsignedp ? unsigned_type_node : integer_type_node;

  if (mode == TYPE_MODE (signed_char_type_node))
    return unsignedp ? unsigned_char_type_node : signed_char_type_node;

  if (mode == TYPE_MODE (short_integer_type_node))
    return unsignedp ? short_unsigned_type_node : short_integer_type_node;

  if (mode == TYPE_MODE (long_integer_type_node))
    return unsignedp ? long_unsigned_type_node : long_integer_type_node;

  if (mode == TYPE_MODE (long_long_integer_type_node))
    return unsignedp ? long_long_unsigned_type_node : long_long_integer_type_node;

  if (mode == TYPE_MODE (widest_integer_literal_type_node))
    return unsignedp ? widest_unsigned_literal_type_node
                     : widest_integer_literal_type_node;

  if (mode == QImode)
    return unsignedp ? unsigned_intQI_type_node : intQI_type_node;

  if (mode == HImode)
    return unsignedp ? unsigned_intHI_type_node : intHI_type_node;

  if (mode == SImode)
    return unsignedp ? unsigned_intSI_type_node : intSI_type_node;

  if (mode == DImode)
    return unsignedp ? unsigned_intDI_type_node : intDI_type_node;

#if HOST_BITS_PER_WIDE_INT >= 64
  if (mode == TYPE_MODE (intTI_type_node))
    return unsignedp ? unsigned_intTI_type_node : intTI_type_node;
#endif

  if (mode == TYPE_MODE (float_type_node))
    return float_type_node;

  if (mode == TYPE_MODE (double_type_node))
    return double_type_node;

  if (mode == TYPE_MODE (long_double_type_node))
    return long_double_type_node;

  if (mode == TYPE_MODE (build_pointer_type (char_type_node)))
    return build_pointer_type (char_type_node);

  if (mode == TYPE_MODE (build_pointer_type (integer_type_node)))
    return build_pointer_type (integer_type_node);

#ifdef VECTOR_MODE_SUPPORTED_P
  if (VECTOR_MODE_SUPPORTED_P (mode))
    {
      switch (mode)
	{
	case V16QImode:
	  return unsignedp ? unsigned_V16QI_type_node : V16QI_type_node;
	case V8HImode:
	  return unsignedp ? unsigned_V8HI_type_node : V8HI_type_node;
	case V4SImode:
	  return unsignedp ? unsigned_V4SI_type_node : V4SI_type_node;
	case V2SImode:
	  return unsignedp ? unsigned_V2SI_type_node : V2SI_type_node;
	case V4HImode:
	  return unsignedp ? unsigned_V4HI_type_node : V4HI_type_node;
	case V8QImode:
	  return unsignedp ? unsigned_V8QI_type_node : V8QI_type_node;
	case V16SFmode:
	  return V16SF_type_node;
	case V4SFmode:
	  return V4SF_type_node;
	case V2SFmode:
	  return V2SF_type_node;
	default:
	  break;
	}
    }
#endif

  return 0;
}

/* Return an unsigned type the same as TYPE in other respects.  */
tree
unsigned_type (type)
     tree type;
{
  tree type1 = TYPE_MAIN_VARIANT (type);
  if (type1 == signed_char_type_node || type1 == char_type_node)
    return unsigned_char_type_node;
  if (type1 == integer_type_node)
    return unsigned_type_node;
  if (type1 == short_integer_type_node)
    return short_unsigned_type_node;
  if (type1 == long_integer_type_node)
    return long_unsigned_type_node;
  if (type1 == long_long_integer_type_node)
    return long_long_unsigned_type_node;
  if (type1 == widest_integer_literal_type_node)
    return widest_unsigned_literal_type_node;
#if HOST_BITS_PER_WIDE_INT >= 64
  if (type1 == intTI_type_node)
    return unsigned_intTI_type_node;
#endif
  if (type1 == intDI_type_node)
    return unsigned_intDI_type_node;
  if (type1 == intSI_type_node)
    return unsigned_intSI_type_node;
  if (type1 == intHI_type_node)
    return unsigned_intHI_type_node;
  if (type1 == intQI_type_node)
    return unsigned_intQI_type_node;

  return signed_or_unsigned_type (1, type);
}

/* Return a signed type the same as TYPE in other respects.  */

tree
signed_type (type)
     tree type;
{
  tree type1 = TYPE_MAIN_VARIANT (type);
  if (type1 == unsigned_char_type_node || type1 == char_type_node)
    return signed_char_type_node;
  if (type1 == unsigned_type_node)
    return integer_type_node;
  if (type1 == short_unsigned_type_node)
    return short_integer_type_node;
  if (type1 == long_unsigned_type_node)
    return long_integer_type_node;
  if (type1 == long_long_unsigned_type_node)
    return long_long_integer_type_node;
  if (type1 == widest_unsigned_literal_type_node)
    return widest_integer_literal_type_node;
#if HOST_BITS_PER_WIDE_INT >= 64
  if (type1 == unsigned_intTI_type_node)
    return intTI_type_node;
#endif
  if (type1 == unsigned_intDI_type_node)
    return intDI_type_node;
  if (type1 == unsigned_intSI_type_node)
    return intSI_type_node;
  if (type1 == unsigned_intHI_type_node)
    return intHI_type_node;
  if (type1 == unsigned_intQI_type_node)
    return intQI_type_node;

  return signed_or_unsigned_type (0, type);
}

/* Return a type the same as TYPE except unsigned or
   signed according to UNSIGNEDP.  */

tree
signed_or_unsigned_type (unsignedp, type)
     int unsignedp;
     tree type;
{
  if (! INTEGRAL_TYPE_P (type)
      || TREE_UNSIGNED (type) == unsignedp)
    return type;

  if (TYPE_PRECISION (type) == TYPE_PRECISION (signed_char_type_node))
    return unsignedp ? unsigned_char_type_node : signed_char_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (integer_type_node))
    return unsignedp ? unsigned_type_node : integer_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (short_integer_type_node))
    return unsignedp ? short_unsigned_type_node : short_integer_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (long_integer_type_node))
    return unsignedp ? long_unsigned_type_node : long_integer_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (long_long_integer_type_node))
    return (unsignedp ? long_long_unsigned_type_node
	    : long_long_integer_type_node);
  if (TYPE_PRECISION (type) == TYPE_PRECISION (widest_integer_literal_type_node))
    return (unsignedp ? widest_unsigned_literal_type_node
	    : widest_integer_literal_type_node);

#if HOST_BITS_PER_WIDE_INT >= 64
  if (TYPE_PRECISION (type) == TYPE_PRECISION (intTI_type_node))
    return unsignedp ? unsigned_intTI_type_node : intTI_type_node;
#endif
  if (TYPE_PRECISION (type) == TYPE_PRECISION (intDI_type_node))
    return unsignedp ? unsigned_intDI_type_node : intDI_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (intSI_type_node))
    return unsignedp ? unsigned_intSI_type_node : intSI_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (intHI_type_node))
    return unsignedp ? unsigned_intHI_type_node : intHI_type_node;
  if (TYPE_PRECISION (type) == TYPE_PRECISION (intQI_type_node))
    return unsignedp ? unsigned_intQI_type_node : intQI_type_node;

  return type;
}

/* Return the minimum number of bits needed to represent VALUE in a
   signed or unsigned type, UNSIGNEDP says which.  */

unsigned int
min_precision (value, unsignedp)
     tree value;
     int unsignedp;
{
  int log;

  /* If the value is negative, compute its negative minus 1.  The latter
     adjustment is because the absolute value of the largest negative value
     is one larger than the largest positive value.  This is equivalent to
     a bit-wise negation, so use that operation instead.  */

  if (tree_int_cst_sgn (value) < 0)
    value = fold (build1 (BIT_NOT_EXPR, TREE_TYPE (value), value));

  /* Return the number of bits needed, taking into account the fact
     that we need one more bit for a signed than unsigned type.  */

  if (integer_zerop (value))
    log = 0;
  else
    log = tree_floor_log2 (value);

  return log + 1 + ! unsignedp;
}

/* Print an error message for invalid operands to arith operation CODE.
   NOP_EXPR is used as a special case (see truthvalue_conversion).  */

void
binary_op_error (code)
     enum tree_code code;
{
  const char *opname;

  switch (code)
    {
    case NOP_EXPR:
      error ("invalid truth-value expression");
      return;

    case PLUS_EXPR:
      opname = "+"; break;
    case MINUS_EXPR:
      opname = "-"; break;
    case MULT_EXPR:
      opname = "*"; break;
    case MAX_EXPR:
      opname = "max"; break;
    case MIN_EXPR:
      opname = "min"; break;
    case EQ_EXPR:
      opname = "=="; break;
    case NE_EXPR:
      opname = "!="; break;
    case LE_EXPR:
      opname = "<="; break;
    case GE_EXPR:
      opname = ">="; break;
    case LT_EXPR:
      opname = "<"; break;
    case GT_EXPR:
      opname = ">"; break;
    case LSHIFT_EXPR:
      opname = "<<"; break;
    case RSHIFT_EXPR:
      opname = ">>"; break;
    case TRUNC_MOD_EXPR:
    case FLOOR_MOD_EXPR:
      opname = "%"; break;
    case TRUNC_DIV_EXPR:
    case FLOOR_DIV_EXPR:
      opname = "/"; break;
    case BIT_AND_EXPR:
      opname = "&"; break;
    case BIT_IOR_EXPR:
      opname = "|"; break;
    case TRUTH_ANDIF_EXPR:
      opname = "&&"; break;
    case TRUTH_ORIF_EXPR:
      opname = "||"; break;
    case BIT_XOR_EXPR:
      opname = "^"; break;
    case LROTATE_EXPR:
    case RROTATE_EXPR:
      opname = "rotate"; break;
    default:
      opname = "unknown"; break;
    }
  error ("invalid operands to binary %s", opname);
}

/* Subroutine of build_binary_op, used for comparison operations.
   See if the operands have both been converted from subword integer types
   and, if so, perhaps change them both back to their original type.
   This function is also responsible for converting the two operands
   to the proper common type for comparison.

   The arguments of this function are all pointers to local variables
   of build_binary_op: OP0_PTR is &OP0, OP1_PTR is &OP1,
   RESTYPE_PTR is &RESULT_TYPE and RESCODE_PTR is &RESULTCODE.

   If this function returns nonzero, it means that the comparison has
   a constant value.  What this function returns is an expression for
   that value.  */

tree
shorten_compare (op0_ptr, op1_ptr, restype_ptr, rescode_ptr)
     tree *op0_ptr, *op1_ptr;
     tree *restype_ptr;
     enum tree_code *rescode_ptr;
{
  tree type;
  tree op0 = *op0_ptr;
  tree op1 = *op1_ptr;
  int unsignedp0, unsignedp1;
  int real1, real2;
  tree primop0, primop1;
  enum tree_code code = *rescode_ptr;

  /* Throw away any conversions to wider types
     already present in the operands.  */

  primop0 = get_narrower (op0, &unsignedp0);
  primop1 = get_narrower (op1, &unsignedp1);

  /* Handle the case that OP0 does not *contain* a conversion
     but it *requires* conversion to FINAL_TYPE.  */

  if (op0 == primop0 && TREE_TYPE (op0) != *restype_ptr)
    unsignedp0 = TREE_UNSIGNED (TREE_TYPE (op0));
  if (op1 == primop1 && TREE_TYPE (op1) != *restype_ptr)
    unsignedp1 = TREE_UNSIGNED (TREE_TYPE (op1));

  /* If one of the operands must be floated, we cannot optimize.  */
  real1 = TREE_CODE (TREE_TYPE (primop0)) == REAL_TYPE;
  real2 = TREE_CODE (TREE_TYPE (primop1)) == REAL_TYPE;

  /* If first arg is constant, swap the args (changing operation
     so value is preserved), for canonicalization.  Don't do this if
     the second arg is 0.  */

  if (TREE_CONSTANT (primop0)
      && ! integer_zerop (primop1) && ! real_zerop (primop1))
    {
      tree tem = primop0;
      int temi = unsignedp0;
      primop0 = primop1;
      primop1 = tem;
      tem = op0;
      op0 = op1;
      op1 = tem;
      *op0_ptr = op0;
      *op1_ptr = op1;
      unsignedp0 = unsignedp1;
      unsignedp1 = temi;
      temi = real1;
      real1 = real2;
      real2 = temi;

      switch (code)
	{
	case LT_EXPR:
	  code = GT_EXPR;
	  break;
	case GT_EXPR:
	  code = LT_EXPR;
	  break;
	case LE_EXPR:
	  code = GE_EXPR;
	  break;
	case GE_EXPR:
	  code = LE_EXPR;
	  break;
	default:
	  break;
	}
      *rescode_ptr = code;
    }

  /* If comparing an integer against a constant more bits wide,
     maybe we can deduce a value of 1 or 0 independent of the data.
     Or else truncate the constant now
     rather than extend the variable at run time.

     This is only interesting if the constant is the wider arg.
     Also, it is not safe if the constant is unsigned and the
     variable arg is signed, since in this case the variable
     would be sign-extended and then regarded as unsigned.
     Our technique fails in this case because the lowest/highest
     possible unsigned results don't follow naturally from the
     lowest/highest possible values of the variable operand.
     For just EQ_EXPR and NE_EXPR there is another technique that
     could be used: see if the constant can be faithfully represented
     in the other operand's type, by truncating it and reextending it
     and see if that preserves the constant's value.  */

  if (!real1 && !real2
      && TREE_CODE (primop1) == INTEGER_CST
      && TYPE_PRECISION (TREE_TYPE (primop0)) < TYPE_PRECISION (*restype_ptr))
    {
      int min_gt, max_gt, min_lt, max_lt;
      tree maxval, minval;
      /* 1 if comparison is nominally unsigned.  */
      int unsignedp = TREE_UNSIGNED (*restype_ptr);
      tree val;

      type = signed_or_unsigned_type (unsignedp0, TREE_TYPE (primop0));

      /* If TYPE is an enumeration, then we need to get its min/max
	 values from it's underlying integral type, not the enumerated
	 type itself.  */
      if (TREE_CODE (type) == ENUMERAL_TYPE)
	type = type_for_size (TYPE_PRECISION (type), unsignedp0);

      maxval = TYPE_MAX_VALUE (type);
      minval = TYPE_MIN_VALUE (type);

      if (unsignedp && !unsignedp0)
	*restype_ptr = signed_type (*restype_ptr);

      if (TREE_TYPE (primop1) != *restype_ptr)
	primop1 = convert (*restype_ptr, primop1);
      if (type != *restype_ptr)
	{
	  minval = convert (*restype_ptr, minval);
	  maxval = convert (*restype_ptr, maxval);
	}

      if (unsignedp && unsignedp0)
	{
	  min_gt = INT_CST_LT_UNSIGNED (primop1, minval);
	  max_gt = INT_CST_LT_UNSIGNED (primop1, maxval);
	  min_lt = INT_CST_LT_UNSIGNED (minval, primop1);
	  max_lt = INT_CST_LT_UNSIGNED (maxval, primop1);
	}
      else
	{
	  min_gt = INT_CST_LT (primop1, minval);
	  max_gt = INT_CST_LT (primop1, maxval);
	  min_lt = INT_CST_LT (minval, primop1);
	  max_lt = INT_CST_LT (maxval, primop1);
	}

      val = 0;
      /* This used to be a switch, but Genix compiler can't handle that.  */
      if (code == NE_EXPR)
	{
	  if (max_lt || min_gt)
	    val = boolean_true_node;
	}
      else if (code == EQ_EXPR)
	{
	  if (max_lt || min_gt)
	    val = boolean_false_node;
	}
      else if (code == LT_EXPR)
	{
	  if (max_lt)
	    val = boolean_true_node;
	  if (!min_lt)
	    val = boolean_false_node;
	}
      else if (code == GT_EXPR)
	{
	  if (min_gt)
	    val = boolean_true_node;
	  if (!max_gt)
	    val = boolean_false_node;
	}
      else if (code == LE_EXPR)
	{
	  if (!max_gt)
	    val = boolean_true_node;
	  if (min_gt)
	    val = boolean_false_node;
	}
      else if (code == GE_EXPR)
	{
	  if (!min_lt)
	    val = boolean_true_node;
	  if (max_lt)
	    val = boolean_false_node;
	}

      /* If primop0 was sign-extended and unsigned comparison specd,
	 we did a signed comparison above using the signed type bounds.
	 But the comparison we output must be unsigned.

	 Also, for inequalities, VAL is no good; but if the signed
	 comparison had *any* fixed result, it follows that the
	 unsigned comparison just tests the sign in reverse
	 (positive values are LE, negative ones GE).
	 So we can generate an unsigned comparison
	 against an extreme value of the signed type.  */

      if (unsignedp && !unsignedp0)
	{
	  if (val != 0)
	    switch (code)
	      {
	      case LT_EXPR:
	      case GE_EXPR:
		primop1 = TYPE_MIN_VALUE (type);
		val = 0;
		break;

	      case LE_EXPR:
	      case GT_EXPR:
		primop1 = TYPE_MAX_VALUE (type);
		val = 0;
		break;

	      default:
		break;
	      }
	  type = unsigned_type (type);
	}

      if (!max_gt && !unsignedp0 && TREE_CODE (primop0) != INTEGER_CST)
	{
	  /* This is the case of (char)x >?< 0x80, which people used to use
	     expecting old C compilers to change the 0x80 into -0x80.  */
	  if (val == boolean_false_node)
	    warning ("comparison is always false due to limited range of data type");
	  if (val == boolean_true_node)
	    warning ("comparison is always true due to limited range of data type");
	}

      if (!min_lt && unsignedp0 && TREE_CODE (primop0) != INTEGER_CST)
	{
	  /* This is the case of (unsigned char)x >?< -1 or < 0.  */
	  if (val == boolean_false_node)
	    warning ("comparison is always false due to limited range of data type");
	  if (val == boolean_true_node)
	    warning ("comparison is always true due to limited range of data type");
	}

      if (val != 0)
	{
	  /* Don't forget to evaluate PRIMOP0 if it has side effects.  */
	  if (TREE_SIDE_EFFECTS (primop0))
	    return build (COMPOUND_EXPR, TREE_TYPE (val), primop0, val);
	  return val;
	}

      /* Value is not predetermined, but do the comparison
	 in the type of the operand that is not constant.
	 TYPE is already properly set.  */
    }
  else if (real1 && real2
	   && (TYPE_PRECISION (TREE_TYPE (primop0))
	       == TYPE_PRECISION (TREE_TYPE (primop1))))
    type = TREE_TYPE (primop0);

  /* If args' natural types are both narrower than nominal type
     and both extend in the same manner, compare them
     in the type of the wider arg.
     Otherwise must actually extend both to the nominal
     common type lest different ways of extending
     alter the result.
     (eg, (short)-1 == (unsigned short)-1  should be 0.)  */

  else if (unsignedp0 == unsignedp1 && real1 == real2
	   && TYPE_PRECISION (TREE_TYPE (primop0)) < TYPE_PRECISION (*restype_ptr)
	   && TYPE_PRECISION (TREE_TYPE (primop1)) < TYPE_PRECISION (*restype_ptr))
    {
      type = common_type (TREE_TYPE (primop0), TREE_TYPE (primop1));
      type = signed_or_unsigned_type (unsignedp0
				      || TREE_UNSIGNED (*restype_ptr),
				      type);
      /* Make sure shorter operand is extended the right way
	 to match the longer operand.  */
      primop0 = convert (signed_or_unsigned_type (unsignedp0, TREE_TYPE (primop0)),
			 primop0);
      primop1 = convert (signed_or_unsigned_type (unsignedp1, TREE_TYPE (primop1)),
			 primop1);
    }
  else
    {
      /* Here we must do the comparison on the nominal type
	 using the args exactly as we received them.  */
      type = *restype_ptr;
      primop0 = op0;
      primop1 = op1;

      if (!real1 && !real2 && integer_zerop (primop1)
	  && TREE_UNSIGNED (*restype_ptr))
	{
	  tree value = 0;
	  switch (code)
	    {
	    case GE_EXPR:
	      /* All unsigned values are >= 0, so we warn if extra warnings
		 are requested.  However, if OP0 is a constant that is
		 >= 0, the signedness of the comparison isn't an issue,
		 so suppress the warning.  */
	      if (extra_warnings && !in_system_header
		  && ! (TREE_CODE (primop0) == INTEGER_CST
			&& ! TREE_OVERFLOW (convert (signed_type (type),
						     primop0))))
		warning ("comparison of unsigned expression >= 0 is always true");
	      value = boolean_true_node;
	      break;

	    case LT_EXPR:
	      if (extra_warnings && !in_system_header
		  && ! (TREE_CODE (primop0) == INTEGER_CST
			&& ! TREE_OVERFLOW (convert (signed_type (type),
						     primop0))))
		warning ("comparison of unsigned expression < 0 is always false");
	      value = boolean_false_node;
	      break;

	    default:
	      break;
	    }

	  if (value != 0)
	    {
	      /* Don't forget to evaluate PRIMOP0 if it has side effects.  */
	      if (TREE_SIDE_EFFECTS (primop0))
		return build (COMPOUND_EXPR, TREE_TYPE (value),
			      primop0, value);
	      return value;
	    }
	}
    }

  *op0_ptr = convert (type, primop0);
  *op1_ptr = convert (type, primop1);

  *restype_ptr = boolean_type_node;

  return 0;
}

/* Return a tree for the sum or difference (RESULTCODE says which)
   of pointer PTROP and integer INTOP.  */

tree
pointer_int_sum (resultcode, ptrop, intop)
     enum tree_code resultcode;
     tree ptrop, intop;
{
  tree size_exp;

  tree result;
  tree folded;

  /* The result is a pointer of the same type that is being added.  */

  tree result_type = TREE_TYPE (ptrop);

  if (TREE_CODE (TREE_TYPE (result_type)) == VOID_TYPE)
    {
      if (pedantic || warn_pointer_arith)
	pedwarn ("pointer of type `void *' used in arithmetic");
      size_exp = integer_one_node;
    }
  else if (TREE_CODE (TREE_TYPE (result_type)) == FUNCTION_TYPE)
    {
      if (pedantic || warn_pointer_arith)
	pedwarn ("pointer to a function used in arithmetic");
      size_exp = integer_one_node;
    }
  else if (TREE_CODE (TREE_TYPE (result_type)) == METHOD_TYPE)
    {
      if (pedantic || warn_pointer_arith)
	pedwarn ("pointer to member function used in arithmetic");
      size_exp = integer_one_node;
    }
  else if (TREE_CODE (TREE_TYPE (result_type)) == OFFSET_TYPE)
    {
      if (pedantic || warn_pointer_arith)
	pedwarn ("pointer to a member used in arithmetic");
      size_exp = integer_one_node;
    }
  else
    size_exp = size_in_bytes (TREE_TYPE (result_type));

  /* If what we are about to multiply by the size of the elements
     contains a constant term, apply distributive law
     and multiply that constant term separately.
     This helps produce common subexpressions.  */

  if ((TREE_CODE (intop) == PLUS_EXPR || TREE_CODE (intop) == MINUS_EXPR)
      && ! TREE_CONSTANT (intop)
      && TREE_CONSTANT (TREE_OPERAND (intop, 1))
      && TREE_CONSTANT (size_exp)
      /* If the constant comes from pointer subtraction,
	 skip this optimization--it would cause an error.  */
      && TREE_CODE (TREE_TYPE (TREE_OPERAND (intop, 0))) == INTEGER_TYPE
      /* If the constant is unsigned, and smaller than the pointer size,
	 then we must skip this optimization.  This is because it could cause
	 an overflow error if the constant is negative but INTOP is not.  */
      && (! TREE_UNSIGNED (TREE_TYPE (intop))
	  || (TYPE_PRECISION (TREE_TYPE (intop))
	      == TYPE_PRECISION (TREE_TYPE (ptrop)))))
    {
      enum tree_code subcode = resultcode;
      tree int_type = TREE_TYPE (intop);
      if (TREE_CODE (intop) == MINUS_EXPR)
	subcode = (subcode == PLUS_EXPR ? MINUS_EXPR : PLUS_EXPR);
      /* Convert both subexpression types to the type of intop,
	 because weird cases involving pointer arithmetic
	 can result in a sum or difference with different type args.  */
      ptrop = build_binary_op (subcode, ptrop,
			       convert (int_type, TREE_OPERAND (intop, 1)), 1);
      intop = convert (int_type, TREE_OPERAND (intop, 0));
    }

  /* Convert the integer argument to a type the same size as sizetype
     so the multiply won't overflow spuriously.  */

  if (TYPE_PRECISION (TREE_TYPE (intop)) != TYPE_PRECISION (sizetype)
      || TREE_UNSIGNED (TREE_TYPE (intop)) != TREE_UNSIGNED (sizetype))
    intop = convert (type_for_size (TYPE_PRECISION (sizetype), 
				    TREE_UNSIGNED (sizetype)), intop);

  /* Replace the integer argument with a suitable product by the object size.
     Do this multiplication as signed, then convert to the appropriate
     pointer type (actually unsigned integral).  */

  intop = convert (result_type,
		   build_binary_op (MULT_EXPR, intop,
				    convert (TREE_TYPE (intop), size_exp), 1));

  /* Create the sum or difference.  */

  result = build (resultcode, result_type, ptrop, intop);

  folded = fold (result);
  if (folded == result)
    TREE_CONSTANT (folded) = TREE_CONSTANT (ptrop) & TREE_CONSTANT (intop);
  return folded;
}

/* Prepare expr to be an argument of a TRUTH_NOT_EXPR,
   or validate its data type for an `if' or `while' statement or ?..: exp.

   This preparation consists of taking the ordinary
   representation of an expression expr and producing a valid tree
   boolean expression describing whether expr is nonzero.  We could
   simply always do build_binary_op (NE_EXPR, expr, boolean_false_node, 1),
   but we optimize comparisons, &&, ||, and !.

   The resulting type should always be `boolean_type_node'.  */

tree
truthvalue_conversion (expr)
     tree expr;
{
  if (TREE_CODE (expr) == ERROR_MARK)
    return expr;

#if 0 /* This appears to be wrong for C++.  */
  /* These really should return error_mark_node after 2.4 is stable.
     But not all callers handle ERROR_MARK properly.  */
  switch (TREE_CODE (TREE_TYPE (expr)))
    {
    case RECORD_TYPE:
      error ("struct type value used where scalar is required");
      return boolean_false_node;

    case UNION_TYPE:
      error ("union type value used where scalar is required");
      return boolean_false_node;

    case ARRAY_TYPE:
      error ("array type value used where scalar is required");
      return boolean_false_node;

    default:
      break;
    }
#endif /* 0 */

  switch (TREE_CODE (expr))
    {
    case EQ_EXPR:
    case NE_EXPR: case LE_EXPR: case GE_EXPR: case LT_EXPR: case GT_EXPR:
    case TRUTH_ANDIF_EXPR:
    case TRUTH_ORIF_EXPR:
    case TRUTH_AND_EXPR:
    case TRUTH_OR_EXPR:
    case TRUTH_XOR_EXPR:
    case TRUTH_NOT_EXPR:
      TREE_TYPE (expr) = boolean_type_node;
      return expr;

    case ERROR_MARK:
      return expr;

    case INTEGER_CST:
      return integer_zerop (expr) ? boolean_false_node : boolean_true_node;

    case REAL_CST:
      return real_zerop (expr) ? boolean_false_node : boolean_true_node;

    case ADDR_EXPR:
      /* If we are taking the address of an external decl, it might be zero
	 if it is weak, so we cannot optimize.  */
      if (DECL_P (TREE_OPERAND (expr, 0))
	  && DECL_EXTERNAL (TREE_OPERAND (expr, 0)))
	break;

      if (TREE_SIDE_EFFECTS (TREE_OPERAND (expr, 0)))
	return build (COMPOUND_EXPR, boolean_type_node,
		      TREE_OPERAND (expr, 0), boolean_true_node);
      else
	return boolean_true_node;

    case COMPLEX_EXPR:
      return build_binary_op ((TREE_SIDE_EFFECTS (TREE_OPERAND (expr, 1))
			       ? TRUTH_OR_EXPR : TRUTH_ORIF_EXPR),
			      truthvalue_conversion (TREE_OPERAND (expr, 0)),
			      truthvalue_conversion (TREE_OPERAND (expr, 1)),
			      0);

    case NEGATE_EXPR:
    case ABS_EXPR:
    case FLOAT_EXPR:
    case FFS_EXPR:
      /* These don't change whether an object is non-zero or zero.  */
      return truthvalue_conversion (TREE_OPERAND (expr, 0));

    case LROTATE_EXPR:
    case RROTATE_EXPR:
      /* These don't change whether an object is zero or non-zero, but
	 we can't ignore them if their second arg has side-effects.  */
      if (TREE_SIDE_EFFECTS (TREE_OPERAND (expr, 1)))
	return build (COMPOUND_EXPR, boolean_type_node, TREE_OPERAND (expr, 1),
		      truthvalue_conversion (TREE_OPERAND (expr, 0)));
      else
	return truthvalue_conversion (TREE_OPERAND (expr, 0));

    case COND_EXPR:
      /* Distribute the conversion into the arms of a COND_EXPR.  */
      return fold (build (COND_EXPR, boolean_type_node, TREE_OPERAND (expr, 0),
			  truthvalue_conversion (TREE_OPERAND (expr, 1)),
			  truthvalue_conversion (TREE_OPERAND (expr, 2))));

    case CONVERT_EXPR:
      /* Don't cancel the effect of a CONVERT_EXPR from a REFERENCE_TYPE,
	 since that affects how `default_conversion' will behave.  */
      if (TREE_CODE (TREE_TYPE (expr)) == REFERENCE_TYPE
	  || TREE_CODE (TREE_TYPE (TREE_OPERAND (expr, 0))) == REFERENCE_TYPE)
	break;
      /* fall through...  */
    case NOP_EXPR:
      /* If this is widening the argument, we can ignore it.  */
      if (TYPE_PRECISION (TREE_TYPE (expr))
	  >= TYPE_PRECISION (TREE_TYPE (TREE_OPERAND (expr, 0))))
	return truthvalue_conversion (TREE_OPERAND (expr, 0));
      break;

    case MINUS_EXPR:
      /* With IEEE arithmetic, x - x may not equal 0, so we can't optimize
	 this case.  */
      if (TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT
	  && TREE_CODE (TREE_TYPE (expr)) == REAL_TYPE)
	break;
      /* fall through...  */
    case BIT_XOR_EXPR:
      /* This and MINUS_EXPR can be changed into a comparison of the
	 two objects.  */
      if (TREE_TYPE (TREE_OPERAND (expr, 0))
	  == TREE_TYPE (TREE_OPERAND (expr, 1)))
	return build_binary_op (NE_EXPR, TREE_OPERAND (expr, 0),
				TREE_OPERAND (expr, 1), 1);
      return build_binary_op (NE_EXPR, TREE_OPERAND (expr, 0),
			      fold (build1 (NOP_EXPR,
					    TREE_TYPE (TREE_OPERAND (expr, 0)),
					    TREE_OPERAND (expr, 1))), 1);

    case BIT_AND_EXPR:
      if (integer_onep (TREE_OPERAND (expr, 1))
	  && TREE_TYPE (expr) != boolean_type_node)
	/* Using convert here would cause infinite recursion.  */
	return build1 (NOP_EXPR, boolean_type_node, expr);
      break;

    case MODIFY_EXPR:
      if (warn_parentheses && C_EXP_ORIGINAL_CODE (expr) == MODIFY_EXPR)
	warning ("suggest parentheses around assignment used as truth value");
      break;

    default:
      break;
    }

  if (TREE_CODE (TREE_TYPE (expr)) == COMPLEX_TYPE)
    {
      tree tem = save_expr (expr);
      return (build_binary_op
	      ((TREE_SIDE_EFFECTS (expr)
		? TRUTH_OR_EXPR : TRUTH_ORIF_EXPR),
	       truthvalue_conversion (build_unary_op (REALPART_EXPR, tem, 0)),
	       truthvalue_conversion (build_unary_op (IMAGPART_EXPR, tem, 0)),
	       0));
    }

  return build_binary_op (NE_EXPR, expr, integer_zero_node, 1);
}

static tree builtin_function_2 PARAMS ((const char *, const char *, tree, tree,
					int, enum built_in_class, int, int,
					int));

/* Make a variant type in the proper way for C/C++, propagating qualifiers
   down to the element type of an array.  */

tree
c_build_qualified_type (type, type_quals)
     tree type;
     int type_quals;
{
  /* A restrict-qualified pointer type must be a pointer to object or
     incomplete type.  Note that the use of POINTER_TYPE_P also allows
     REFERENCE_TYPEs, which is appropriate for C++.  Unfortunately,
     the C++ front-end also use POINTER_TYPE for pointer-to-member
     values, so even though it should be illegal to use `restrict'
     with such an entity we don't flag that here.  Thus, special case
     code for that case is required in the C++ front-end.  */
  if ((type_quals & TYPE_QUAL_RESTRICT)
      && (!POINTER_TYPE_P (type)
	  || !C_TYPE_OBJECT_OR_INCOMPLETE_P (TREE_TYPE (type))))
    {
      error ("invalid use of `restrict'");
      type_quals &= ~TYPE_QUAL_RESTRICT;
    }

  if (TREE_CODE (type) == ARRAY_TYPE)
    return build_array_type (c_build_qualified_type (TREE_TYPE (type),
						     type_quals),
			     TYPE_DOMAIN (type));
  return build_qualified_type (type, type_quals);
}

/* Apply the TYPE_QUALS to the new DECL.  */

void
c_apply_type_quals_to_decl (type_quals, decl)
     int type_quals;
     tree decl;
{
  if ((type_quals & TYPE_QUAL_CONST)
      || (TREE_TYPE (decl) 
	  && TREE_CODE (TREE_TYPE (decl)) == REFERENCE_TYPE))
    TREE_READONLY (decl) = 1;
  if (type_quals & TYPE_QUAL_VOLATILE)
    {
      TREE_SIDE_EFFECTS (decl) = 1;
      TREE_THIS_VOLATILE (decl) = 1;
    }
  if (type_quals & TYPE_QUAL_RESTRICT)
    {
      if (!TREE_TYPE (decl)
	  || !POINTER_TYPE_P (TREE_TYPE (decl))
	  || !C_TYPE_OBJECT_OR_INCOMPLETE_P (TREE_TYPE (TREE_TYPE (decl))))
	error ("invalid use of `restrict'");
      else if (flag_strict_aliasing)
	/* Indicate we need to make a unique alias set for this pointer.
	   We can't do it here because it might be pointing to an
	   incomplete type.  */
	DECL_POINTER_ALIAS_SET (decl) = -2;
    }
}


/* Return the typed-based alias set for T, which may be an expression
   or a type.  Return -1 if we don't do anything special.  */

HOST_WIDE_INT
c_common_get_alias_set (t)
     tree t;
{
  tree u;
  
  /* We know nothing about vector types */
  if (TREE_CODE (t) == VECTOR_TYPE)
    return 0;          
  
  /* Permit type-punning when accessing a union, provided the access
     is directly through the union.  For example, this code does not
     permit taking the address of a union member and then storing
     through it.  Even the type-punning allowed here is a GCC
     extension, albeit a common and useful one; the C standard says
     that such accesses have implementation-defined behavior.  */
  for (u = t;
       TREE_CODE (u) == COMPONENT_REF || TREE_CODE (u) == ARRAY_REF;
       u = TREE_OPERAND (u, 0))
    if (TREE_CODE (u) == COMPONENT_REF
	&& TREE_CODE (TREE_TYPE (TREE_OPERAND (u, 0))) == UNION_TYPE)
      return 0;

  /* If this is a char *, the ANSI C standard says it can alias
     anything.  Note that all references need do this.  */
  if (TREE_CODE_CLASS (TREE_CODE (t)) == 'r'
      && TREE_CODE (TREE_TYPE (t)) == INTEGER_TYPE
      && TYPE_PRECISION (TREE_TYPE (t)) == TYPE_PRECISION (char_type_node))
    return 0;

  /* That's all the expressions we handle specially.  */
  if (! TYPE_P (t))
    return -1;

  /* The C standard specifically allows aliasing between signed and
     unsigned variants of the same type.  We treat the signed
     variant as canonical.  */
  if (TREE_CODE (t) == INTEGER_TYPE && TREE_UNSIGNED (t))
    {
      tree t1 = signed_type (t);

      /* t1 == t can happen for boolean nodes which are always unsigned.  */
      if (t1 != t)
	return get_alias_set (t1);
    }
  else if (POINTER_TYPE_P (t))
    {
      tree t1;

      /* Unfortunately, there is no canonical form of a pointer type.
	 In particular, if we have `typedef int I', then `int *', and
	 `I *' are different types.  So, we have to pick a canonical
	 representative.  We do this below.

	 Technically, this approach is actually more conservative that
	 it needs to be.  In particular, `const int *' and `int *'
	 should be in different alias sets, according to the C and C++
	 standard, since their types are not the same, and so,
	 technically, an `int **' and `const int **' cannot point at
	 the same thing.

         But, the standard is wrong.  In particular, this code is
	 legal C++:

            int *ip;
            int **ipp = &ip;
            const int* const* cipp = &ipp;

         And, it doesn't make sense for that to be legal unless you
	 can dereference IPP and CIPP.  So, we ignore cv-qualifiers on
	 the pointed-to types.  This issue has been reported to the
	 C++ committee.  */
      t1 = build_type_no_quals (t);
      if (t1 != t)
	return get_alias_set (t1);
    }

  return -1;
}

/* Implement the __alignof keyword: Return the minimum required
   alignment of TYPE, measured in bytes.  */

tree
c_alignof (type)
     tree type;
{
  enum tree_code code = TREE_CODE (type);
  tree t;

  /* In C++, sizeof applies to the referent.  Handle alignof the same way.  */
  if (code == REFERENCE_TYPE)
    {
      type = TREE_TYPE (type);
      code = TREE_CODE (type);
    }

  if (code == FUNCTION_TYPE)
    t = size_int (FUNCTION_BOUNDARY / BITS_PER_UNIT);
  else if (code == VOID_TYPE || code == ERROR_MARK)
    t = size_one_node;
  else if (!COMPLETE_TYPE_P (type))
    {
      error ("__alignof__ applied to an incomplete type");
      t = size_zero_node;
    }
  else
    t = size_int (TYPE_ALIGN (type) / BITS_PER_UNIT);

  return fold (build1 (NOP_EXPR, c_size_type_node, t));
}

/* Implement the __alignof keyword: Return the minimum required
   alignment of EXPR, measured in bytes.  For VAR_DECL's and
   FIELD_DECL's return DECL_ALIGN (which can be set from an
   "aligned" __attribute__ specification).  */

tree
c_alignof_expr (expr)
     tree expr;
{
  tree t;

  if (TREE_CODE (expr) == VAR_DECL)
    t = size_int (DECL_ALIGN (expr) / BITS_PER_UNIT);
 
  else if (TREE_CODE (expr) == COMPONENT_REF
	   && DECL_C_BIT_FIELD (TREE_OPERAND (expr, 1)))
    {
      error ("`__alignof' applied to a bit-field");
      t = size_one_node;
    }
  else if (TREE_CODE (expr) == COMPONENT_REF
	   && TREE_CODE (TREE_OPERAND (expr, 1)) == FIELD_DECL)
    t = size_int (DECL_ALIGN (TREE_OPERAND (expr, 1)) / BITS_PER_UNIT);
 
  else if (TREE_CODE (expr) == INDIRECT_REF)
    {
      tree t = TREE_OPERAND (expr, 0);
      tree best = t;
      int bestalign = TYPE_ALIGN (TREE_TYPE (TREE_TYPE (t)));
 
      while (TREE_CODE (t) == NOP_EXPR
	     && TREE_CODE (TREE_TYPE (TREE_OPERAND (t, 0))) == POINTER_TYPE)
	{
	  int thisalign;

	  t = TREE_OPERAND (t, 0);
	  thisalign = TYPE_ALIGN (TREE_TYPE (TREE_TYPE (t)));
	  if (thisalign > bestalign)
	    best = t, bestalign = thisalign;
	}
      return c_alignof (TREE_TYPE (TREE_TYPE (best)));
    }
  else
    return c_alignof (TREE_TYPE (expr));

  return fold (build1 (NOP_EXPR, c_size_type_node, t));
}

/* Give the specifications for the format attributes, used by C and all
   descendents.  */

static const struct attribute_spec c_format_attribute_table[] =
{
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  { "format",                 3, 3, false, true,  true,
			      handle_format_attribute },
  { "format_arg",             1, 1, false, true,  true,
			      handle_format_arg_attribute },
  { NULL,                     0, 0, false, false, false, NULL }
};

/* Build tree nodes and builtin functions common to both C and C++ language
   frontends.  */

void
c_common_nodes_and_builtins ()
{
  enum builtin_type 
  {
#define DEF_PRIMITIVE_TYPE(NAME, VALUE) NAME,
#define DEF_FUNCTION_TYPE_0(NAME, RETURN) NAME,
#define DEF_FUNCTION_TYPE_1(NAME, RETURN, ARG1) NAME,
#define DEF_FUNCTION_TYPE_2(NAME, RETURN, ARG1, ARG2) NAME,
#define DEF_FUNCTION_TYPE_3(NAME, RETURN, ARG1, ARG2, ARG3) NAME,
#define DEF_FUNCTION_TYPE_4(NAME, RETURN, ARG1, ARG2, ARG3, ARG4) NAME,
#define DEF_FUNCTION_TYPE_VAR_0(NAME, RETURN) NAME,
#define DEF_FUNCTION_TYPE_VAR_1(NAME, RETURN, ARG1) NAME,
#define DEF_FUNCTION_TYPE_VAR_2(NAME, RETURN, ARG1, ARG2) NAME,
#define DEF_POINTER_TYPE(NAME, TYPE) NAME,
#include "builtin-types.def"
#undef DEF_PRIMITIVE_TYPE
#undef DEF_FUNCTION_TYPE_0
#undef DEF_FUNCTION_TYPE_1
#undef DEF_FUNCTION_TYPE_2
#undef DEF_FUNCTION_TYPE_3
#undef DEF_FUNCTION_TYPE_4
#undef DEF_FUNCTION_TYPE_VAR_0
#undef DEF_FUNCTION_TYPE_VAR_1
#undef DEF_FUNCTION_TYPE_VAR_2
#undef DEF_POINTER_TYPE
    BT_LAST
  };

  typedef enum builtin_type builtin_type;

  tree builtin_types[(int) BT_LAST];
  int wchar_type_size;
  tree array_domain_type;
  /* Either char* or void*.  */
  tree traditional_ptr_type_node;
  /* Either const char* or const void*.  */
  tree traditional_cptr_type_node;
  tree traditional_len_type_node;
  tree va_list_ref_type_node;
  tree va_list_arg_type_node;

  /* We must initialize this before any builtin functions (which might have
     attributes) are declared.  (c_common_init is too late.)  */
  format_attribute_table = c_format_attribute_table;

  /* Define `int' and `char' first so that dbx will output them first.  */
  record_builtin_type (RID_INT, NULL, integer_type_node);
  record_builtin_type (RID_CHAR, "char", char_type_node);

  /* `signed' is the same as `int'.  FIXME: the declarations of "signed",
     "unsigned long", "long long unsigned" and "unsigned short" were in C++
     but not C.  Are the conditionals here needed?  */
  if (c_language == clk_cplusplus)
    record_builtin_type (RID_SIGNED, NULL, integer_type_node);
  record_builtin_type (RID_LONG, "long int", long_integer_type_node);
  record_builtin_type (RID_UNSIGNED, "unsigned int", unsigned_type_node);
  record_builtin_type (RID_MAX, "long unsigned int",
		       long_unsigned_type_node);
  if (c_language == clk_cplusplus)
    record_builtin_type (RID_MAX, "unsigned long", long_unsigned_type_node);
  record_builtin_type (RID_MAX, "long long int",
		       long_long_integer_type_node);
  record_builtin_type (RID_MAX, "long long unsigned int",
		       long_long_unsigned_type_node);
  if (c_language == clk_cplusplus)
    record_builtin_type (RID_MAX, "long long unsigned",
			 long_long_unsigned_type_node);
  record_builtin_type (RID_SHORT, "short int", short_integer_type_node);
  record_builtin_type (RID_MAX, "short unsigned int",
		       short_unsigned_type_node);
  if (c_language == clk_cplusplus)
    record_builtin_type (RID_MAX, "unsigned short",
			 short_unsigned_type_node);

  /* Define both `signed char' and `unsigned char'.  */
  record_builtin_type (RID_MAX, "signed char", signed_char_type_node);
  record_builtin_type (RID_MAX, "unsigned char", unsigned_char_type_node);

  /* These are types that type_for_size and type_for_mode use.  */
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, intQI_type_node));
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, intHI_type_node));
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, intSI_type_node));
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, intDI_type_node));
#if HOST_BITS_PER_WIDE_INT >= 64
  pushdecl (build_decl (TYPE_DECL, get_identifier ("__int128_t"), intTI_type_node));
#endif
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, unsigned_intQI_type_node));
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, unsigned_intHI_type_node));
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, unsigned_intSI_type_node));
  pushdecl (build_decl (TYPE_DECL, NULL_TREE, unsigned_intDI_type_node));
#if HOST_BITS_PER_WIDE_INT >= 64
  pushdecl (build_decl (TYPE_DECL, get_identifier ("__uint128_t"), unsigned_intTI_type_node));
#endif

  /* Create the widest literal types.  */
  widest_integer_literal_type_node
    = make_signed_type (HOST_BITS_PER_WIDE_INT * 2);
  pushdecl (build_decl (TYPE_DECL, NULL_TREE,
			widest_integer_literal_type_node));

  widest_unsigned_literal_type_node
    = make_unsigned_type (HOST_BITS_PER_WIDE_INT * 2);
  pushdecl (build_decl (TYPE_DECL, NULL_TREE,
			widest_unsigned_literal_type_node));

  /* `unsigned long' is the standard type for sizeof.
     Note that stddef.h uses `unsigned long',
     and this must agree, even if long and int are the same size.  */
  c_size_type_node =
    TREE_TYPE (identifier_global_value (get_identifier (SIZE_TYPE)));
  signed_size_type_node = signed_type (c_size_type_node);
  if (flag_traditional)
    c_size_type_node = signed_size_type_node;
  set_sizetype (c_size_type_node);

  build_common_tree_nodes_2 (flag_short_double);

  record_builtin_type (RID_FLOAT, NULL, float_type_node);
  record_builtin_type (RID_DOUBLE, NULL, double_type_node);
  record_builtin_type (RID_MAX, "long double", long_double_type_node);

  pushdecl (build_decl (TYPE_DECL, get_identifier ("complex int"),
			complex_integer_type_node));
  pushdecl (build_decl (TYPE_DECL, get_identifier ("complex float"),
			complex_float_type_node));
  pushdecl (build_decl (TYPE_DECL, get_identifier ("complex double"),
			complex_double_type_node));
  pushdecl (build_decl (TYPE_DECL, get_identifier ("complex long double"),
			complex_long_double_type_node));

  record_builtin_type (RID_VOID, NULL, void_type_node);

  void_zero_node = build_int_2 (0, 0);
  TREE_TYPE (void_zero_node) = void_type_node;

  void_list_node = build_void_list_node ();

  /* Make a type to be the domain of a few array types
     whose domains don't really matter.
     200 is small enough that it always fits in size_t
     and large enough that it can hold most function names for the
     initializations of __FUNCTION__ and __PRETTY_FUNCTION__.  */
  array_domain_type = build_index_type (size_int (200));

  /* Make a type for arrays of characters.
     With luck nothing will ever really depend on the length of this
     array type.  */
  char_array_type_node
    = build_array_type (char_type_node, array_domain_type);

  /* Likewise for arrays of ints.  */
  int_array_type_node
    = build_array_type (integer_type_node, array_domain_type);

  string_type_node = build_pointer_type (char_type_node);
  const_string_type_node
    = build_pointer_type (build_qualified_type
			  (char_type_node, TYPE_QUAL_CONST));

  traditional_ptr_type_node = ((flag_traditional && 
				c_language != clk_cplusplus)
			       ? string_type_node : ptr_type_node);
  traditional_cptr_type_node = ((flag_traditional && 
				 c_language != clk_cplusplus)
			       ? const_string_type_node : const_ptr_type_node);

  (*targetm.init_builtins) ();

  /* This is special for C++ so functions can be overloaded.  */
  wchar_type_node = get_identifier (flag_short_wchar
				    ? "short unsigned int"
				    : WCHAR_TYPE);
  wchar_type_node = TREE_TYPE (identifier_global_value (wchar_type_node));
  wchar_type_size = TYPE_PRECISION (wchar_type_node);
  if (c_language == clk_cplusplus)
    {
      if (TREE_UNSIGNED (wchar_type_node))
	wchar_type_node = make_unsigned_type (wchar_type_size);
      else
	wchar_type_node = make_signed_type (wchar_type_size);
      record_builtin_type (RID_WCHAR, "wchar_t", wchar_type_node);
    }
  else
    {
      signed_wchar_type_node = signed_type (wchar_type_node);
      unsigned_wchar_type_node = unsigned_type (wchar_type_node);
    }

  /* This is for wide string constants.  */
  wchar_array_type_node
    = build_array_type (wchar_type_node, array_domain_type);

  wint_type_node =
    TREE_TYPE (identifier_global_value (get_identifier (WINT_TYPE)));

  intmax_type_node =
    TREE_TYPE (identifier_global_value (get_identifier (INTMAX_TYPE)));
  uintmax_type_node =
    TREE_TYPE (identifier_global_value (get_identifier (UINTMAX_TYPE)));

  default_function_type = build_function_type (integer_type_node, NULL_TREE);
  ptrdiff_type_node
    = TREE_TYPE (identifier_global_value (get_identifier (PTRDIFF_TYPE)));
  unsigned_ptrdiff_type_node = unsigned_type (ptrdiff_type_node);

  pushdecl (build_decl (TYPE_DECL, get_identifier ("__builtin_va_list"),
			va_list_type_node));

  pushdecl (build_decl (TYPE_DECL, get_identifier ("__builtin_ptrdiff_t"),
			ptrdiff_type_node));

  pushdecl (build_decl (TYPE_DECL, get_identifier ("__builtin_size_t"),
			sizetype));

  if (TREE_CODE (va_list_type_node) == ARRAY_TYPE)
    {
      va_list_arg_type_node = va_list_ref_type_node =
	build_pointer_type (TREE_TYPE (va_list_type_node));
    }
  else
    {
      va_list_arg_type_node = va_list_type_node;
      va_list_ref_type_node = build_reference_type (va_list_type_node);
    }
 
  traditional_len_type_node = ((flag_traditional && 
				c_language != clk_cplusplus)
			       ? integer_type_node : sizetype);

#define DEF_PRIMITIVE_TYPE(ENUM, VALUE) \
  builtin_types[(int) ENUM] = VALUE;
#define DEF_FUNCTION_TYPE_0(ENUM, RETURN)		\
  builtin_types[(int) ENUM]				\
    = build_function_type (builtin_types[(int) RETURN],	\
			   void_list_node);
#define DEF_FUNCTION_TYPE_1(ENUM, RETURN, ARG1)				\
  builtin_types[(int) ENUM]						\
    = build_function_type (builtin_types[(int) RETURN],			\
			   tree_cons (NULL_TREE,			\
				      builtin_types[(int) ARG1],	\
				      void_list_node));
#define DEF_FUNCTION_TYPE_2(ENUM, RETURN, ARG1, ARG2)	\
  builtin_types[(int) ENUM]				\
    = build_function_type 				\
      (builtin_types[(int) RETURN],			\
       tree_cons (NULL_TREE,				\
		  builtin_types[(int) ARG1],		\
		  tree_cons (NULL_TREE,			\
			     builtin_types[(int) ARG2],	\
			     void_list_node)));
#define DEF_FUNCTION_TYPE_3(ENUM, RETURN, ARG1, ARG2, ARG3)		 \
  builtin_types[(int) ENUM]						 \
    = build_function_type						 \
      (builtin_types[(int) RETURN],					 \
       tree_cons (NULL_TREE,						 \
		  builtin_types[(int) ARG1],				 \
		  tree_cons (NULL_TREE,					 \
			     builtin_types[(int) ARG2],			 \
			     tree_cons (NULL_TREE,			 \
					builtin_types[(int) ARG3],	 \
					void_list_node))));
#define DEF_FUNCTION_TYPE_4(ENUM, RETURN, ARG1, ARG2, ARG3, ARG4)	\
  builtin_types[(int) ENUM]						\
    = build_function_type						\
      (builtin_types[(int) RETURN],					\
       tree_cons (NULL_TREE,						\
		  builtin_types[(int) ARG1],				\
		  tree_cons (NULL_TREE,					\
			     builtin_types[(int) ARG2],			\
			     tree_cons 					\
			     (NULL_TREE,				\
			      builtin_types[(int) ARG3],	 	\
			      tree_cons (NULL_TREE,			\
					 builtin_types[(int) ARG4],	\
					 void_list_node)))));
#define DEF_FUNCTION_TYPE_VAR_0(ENUM, RETURN)				\
  builtin_types[(int) ENUM]						\
    = build_function_type (builtin_types[(int) RETURN], NULL_TREE);
#define DEF_FUNCTION_TYPE_VAR_1(ENUM, RETURN, ARG1)			 \
   builtin_types[(int) ENUM]						 \
    = build_function_type (builtin_types[(int) RETURN], 		 \
			   tree_cons (NULL_TREE,			 \
				      builtin_types[(int) ARG1],	 \
				      NULL_TREE));

#define DEF_FUNCTION_TYPE_VAR_2(ENUM, RETURN, ARG1, ARG2)	\
   builtin_types[(int) ENUM]					\
    = build_function_type 					\
      (builtin_types[(int) RETURN],				\
       tree_cons (NULL_TREE,					\
		  builtin_types[(int) ARG1],			\
		  tree_cons (NULL_TREE,				\
			     builtin_types[(int) ARG2],		\
			     NULL_TREE)));
#define DEF_POINTER_TYPE(ENUM, TYPE)			\
  builtin_types[(int) ENUM]				\
    = build_pointer_type (builtin_types[(int) TYPE]);
#include "builtin-types.def"
#undef DEF_PRIMITIVE_TYPE
#undef DEF_FUNCTION_TYPE_1
#undef DEF_FUNCTION_TYPE_2
#undef DEF_FUNCTION_TYPE_3
#undef DEF_FUNCTION_TYPE_4
#undef DEF_FUNCTION_TYPE_VAR_0
#undef DEF_FUNCTION_TYPE_VAR_1
#undef DEF_POINTER_TYPE

#define DEF_BUILTIN(ENUM, NAME, CLASS,					\
                    TYPE, LIBTYPE, BOTH_P, FALLBACK_P, NONANSI_P)	\
  if (NAME)								\
    {									\
      tree decl;							\
									\
      if (strncmp (NAME, "__builtin_", strlen ("__builtin_")) != 0)	\
	abort ();							\
									\
      if (!BOTH_P)							\
	decl = builtin_function (NAME, builtin_types[TYPE], ENUM,	\
				 CLASS,					\
				 (FALLBACK_P				\
				  ? (NAME + strlen ("__builtin_"))	\
				  : NULL));				\
      else								\
	decl = builtin_function_2 (NAME,				\
				   NAME + strlen ("__builtin_"),	\
				   builtin_types[TYPE],			\
				   builtin_types[LIBTYPE],		\
				   ENUM,				\
				   CLASS,				\
				   FALLBACK_P,				\
				   NONANSI_P,				\
				   /*noreturn_p=*/0);			\
									\
      built_in_decls[(int) ENUM] = decl;				\
    }									
#include "builtins.def"
#undef DEF_BUILTIN

  /* Declare _exit and _Exit just to mark them as non-returning.  */
  builtin_function_2 (NULL, "_exit", NULL_TREE, 
		      builtin_types[BT_FN_VOID_INT],
		      0, NOT_BUILT_IN, 0, 1, 1);
  builtin_function_2 (NULL, "_Exit", NULL_TREE, 
		      builtin_types[BT_FN_VOID_INT],
		      0, NOT_BUILT_IN, 0, !flag_isoc99, 1);

  /* Declare these functions non-returning
     to avoid spurious "control drops through" warnings.  */
  builtin_function_2 (NULL, "abort",
		      NULL_TREE, ((c_language == clk_cplusplus)
				  ? builtin_types[BT_FN_VOID]
				  : builtin_types[BT_FN_VOID_VAR]),
		      0, NOT_BUILT_IN, 0, 0, 1);

  builtin_function_2 (NULL, "exit",
		      NULL_TREE, ((c_language == clk_cplusplus)
				  ? builtin_types[BT_FN_VOID_INT]
				  : builtin_types[BT_FN_VOID_VAR]),
		      0, NOT_BUILT_IN, 0, 0, 1);

  main_identifier_node = get_identifier ("main");

  /* ??? Perhaps there's a better place to do this.  But it is related
     to __builtin_va_arg, so it isn't that off-the-wall.  */
  lang_type_promotes_to = simple_type_promotes_to;
}

tree
build_va_arg (expr, type)
     tree expr, type;
{
  return build1 (VA_ARG_EXPR, type, expr);
}


/* Linked list of disabled built-in functions.  */

typedef struct disabled_builtin
{
  const char *name;
  struct disabled_builtin *next;
} disabled_builtin;
static disabled_builtin *disabled_builtins = NULL;

static bool builtin_function_disabled_p PARAMS ((const char *));

/* Disable a built-in function specified by -fno-builtin-NAME.  If NAME
   begins with "__builtin_", give an error.  */

void
disable_builtin_function (name)
     const char *name;
{
  if (strncmp (name, "__builtin_", strlen ("__builtin_")) == 0)
    error ("cannot disable built-in function `%s'", name);
  else
    {
      disabled_builtin *new = xmalloc (sizeof (disabled_builtin));
      new->name = name;
      new->next = disabled_builtins;
      disabled_builtins = new;
    }
}


/* Return true if the built-in function NAME has been disabled, false
   otherwise.  */

static bool
builtin_function_disabled_p (name)
     const char *name;
{
  disabled_builtin *p;
  for (p = disabled_builtins; p != NULL; p = p->next)
    {
      if (strcmp (name, p->name) == 0)
	return true;
    }
  return false;
}


/* Possibly define a builtin function with one or two names.  BUILTIN_NAME
   is an __builtin_-prefixed name; NAME is the ordinary name; one or both
   of these may be NULL (though both being NULL is useless).
   BUILTIN_TYPE is the type of the __builtin_-prefixed function;
   TYPE is the type of the function with the ordinary name.  These
   may differ if the ordinary name is declared with a looser type to avoid
   conflicts with headers.  FUNCTION_CODE and CLASS are as for
   builtin_function.  If LIBRARY_NAME_P is nonzero, NAME is passed as
   the LIBRARY_NAME parameter to builtin_function when declaring BUILTIN_NAME.
   If NONANSI_P is nonzero, the name NAME is treated as a non-ANSI name; if
   NORETURN_P is nonzero, the function is marked as non-returning.
   Returns the declaration of BUILTIN_NAME, if any, otherwise
   the declaration of NAME.  Does not declare NAME if flag_no_builtin,
   or if NONANSI_P and flag_no_nonansi_builtin.  */

static tree
builtin_function_2 (builtin_name, name, builtin_type, type, function_code,
		    class, library_name_p, nonansi_p, noreturn_p)
     const char *builtin_name;
     const char *name;
     tree builtin_type;
     tree type;
     int function_code;
     enum built_in_class class;
     int library_name_p;
     int nonansi_p;
     int noreturn_p;
{
  tree bdecl = NULL_TREE;
  tree decl = NULL_TREE;
  if (builtin_name != 0)
    {
      bdecl = builtin_function (builtin_name, builtin_type, function_code,
				class, library_name_p ? name : NULL);
      if (noreturn_p)
	{
	  TREE_THIS_VOLATILE (bdecl) = 1;
	  TREE_SIDE_EFFECTS (bdecl) = 1;
	}
    }
  if (name != 0 && !flag_no_builtin && !builtin_function_disabled_p (name)
      && !(nonansi_p && flag_no_nonansi_builtin))
    {
      decl = builtin_function (name, type, function_code, class, NULL);
      if (nonansi_p)
	DECL_BUILT_IN_NONANSI (decl) = 1;
      if (noreturn_p)
	{
	  TREE_THIS_VOLATILE (decl) = 1;
	  TREE_SIDE_EFFECTS (decl) = 1;
	}
    }
  return (bdecl != 0 ? bdecl : decl);
}

/* Nonzero if the type T promotes to int.  This is (nearly) the
   integral promotions defined in ISO C99 6.3.1.1/2.  */

bool
c_promoting_integer_type_p (t)
     tree t;
{
  switch (TREE_CODE (t))
    {
    case INTEGER_TYPE:
      return (TYPE_MAIN_VARIANT (t) == char_type_node
	      || TYPE_MAIN_VARIANT (t) == signed_char_type_node
	      || TYPE_MAIN_VARIANT (t) == unsigned_char_type_node
	      || TYPE_MAIN_VARIANT (t) == short_integer_type_node
	      || TYPE_MAIN_VARIANT (t) == short_unsigned_type_node
	      || TYPE_PRECISION (t) < TYPE_PRECISION (integer_type_node));

    case ENUMERAL_TYPE:
      /* ??? Technically all enumerations not larger than an int
	 promote to an int.  But this is used along code paths
	 that only want to notice a size change.  */
      return TYPE_PRECISION (t) < TYPE_PRECISION (integer_type_node);

    case BOOLEAN_TYPE:
      return 1;

    default:
      return 0;
    }
}

/* Given a type, apply default promotions wrt unnamed function arguments
   and return the new type.  Return NULL_TREE if no change.  */
/* ??? There is a function of the same name in the C++ front end that
   does something similar, but is more thorough and does not return NULL
   if no change.  We could perhaps share code, but it would make the
   self_promoting_type property harder to identify.  */

tree
simple_type_promotes_to (type)
     tree type;
{
  if (TYPE_MAIN_VARIANT (type) == float_type_node)
    return double_type_node;

  if (c_promoting_integer_type_p (type))
    {
      /* Traditionally, unsignedness is preserved in default promotions.
         Also preserve unsignedness if not really getting any wider.  */
      if (TREE_UNSIGNED (type)
          && (flag_traditional
              || TYPE_PRECISION (type) == TYPE_PRECISION (integer_type_node)))
        return unsigned_type_node;
      return integer_type_node;
    }

  return NULL_TREE;
}

/* Return 1 if PARMS specifies a fixed number of parameters
   and none of their types is affected by default promotions.  */

int
self_promoting_args_p (parms)
     tree parms;
{
  tree t;
  for (t = parms; t; t = TREE_CHAIN (t))
    {
      tree type = TREE_VALUE (t);

      if (TREE_CHAIN (t) == 0 && type != void_type_node)
	return 0;

      if (type == 0)
	return 0;

      if (TYPE_MAIN_VARIANT (type) == float_type_node)
	return 0;

      if (c_promoting_integer_type_p (type))
	return 0;
    }
  return 1;
}

/* Recursively examines the array elements of TYPE, until a non-array
   element type is found.  */

tree
strip_array_types (type)
     tree type;
{
  while (TREE_CODE (type) == ARRAY_TYPE)
    type = TREE_TYPE (type);

  return type;
}

/* Recognize certain built-in functions so we can make tree-codes
   other than CALL_EXPR.  We do this when it enables fold-const.c
   to do something useful.  */
/* ??? By rights this should go in builtins.c, but only C and C++
   implement build_{binary,unary}_op.  Not exactly sure what bits
   of functionality are actually needed from those functions, or
   where the similar functionality exists in the other front ends.  */

tree
expand_tree_builtin (function, params, coerced_params)
     tree function, params, coerced_params;
{
  enum tree_code code;

  if (DECL_BUILT_IN_CLASS (function) != BUILT_IN_NORMAL)
    return NULL_TREE;

  switch (DECL_FUNCTION_CODE (function))
    {
    case BUILT_IN_ABS:
    case BUILT_IN_LABS:
    case BUILT_IN_LLABS:
    case BUILT_IN_IMAXABS:
    case BUILT_IN_FABS:
    case BUILT_IN_FABSL:
    case BUILT_IN_FABSF:
      if (coerced_params == 0)
	return integer_zero_node;
      return build_unary_op (ABS_EXPR, TREE_VALUE (coerced_params), 0);

    case BUILT_IN_CONJ:
    case BUILT_IN_CONJF:
    case BUILT_IN_CONJL:
      if (coerced_params == 0)
	return integer_zero_node;
      return build_unary_op (CONJ_EXPR, TREE_VALUE (coerced_params), 0);

    case BUILT_IN_CREAL:
    case BUILT_IN_CREALF:
    case BUILT_IN_CREALL:
      if (coerced_params == 0)
	return integer_zero_node;
      return build_unary_op (REALPART_EXPR, TREE_VALUE (coerced_params), 0);

    case BUILT_IN_CIMAG:
    case BUILT_IN_CIMAGF:
    case BUILT_IN_CIMAGL:
      if (coerced_params == 0)
	return integer_zero_node;
      return build_unary_op (IMAGPART_EXPR, TREE_VALUE (coerced_params), 0);

    case BUILT_IN_ISGREATER:
      if (TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT)
	code = UNLE_EXPR;
      else
	code = LE_EXPR;
      goto unordered_cmp;

    case BUILT_IN_ISGREATEREQUAL:
      if (TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT)
	code = UNLT_EXPR;
      else
	code = LT_EXPR;
      goto unordered_cmp;

    case BUILT_IN_ISLESS:
      if (TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT)
	code = UNGE_EXPR;
      else
	code = GE_EXPR;
      goto unordered_cmp;

    case BUILT_IN_ISLESSEQUAL:
      if (TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT)
	code = UNGT_EXPR;
      else
	code = GT_EXPR;
      goto unordered_cmp;

    case BUILT_IN_ISLESSGREATER:
      if (TARGET_FLOAT_FORMAT == IEEE_FLOAT_FORMAT)
	code = UNEQ_EXPR;
      else
	code = EQ_EXPR;
      goto unordered_cmp;

    case BUILT_IN_ISUNORDERED:
      if (TARGET_FLOAT_FORMAT != IEEE_FLOAT_FORMAT)
	return integer_zero_node;
      code = UNORDERED_EXPR;
      goto unordered_cmp;

    unordered_cmp:
      {
	tree arg0, arg1;

	if (params == 0
	    || TREE_CHAIN (params) == 0)
	  {
	    error ("too few arguments to function `%s'",
		   IDENTIFIER_POINTER (DECL_NAME (function)));
	    return error_mark_node;
	  }
	else if (TREE_CHAIN (TREE_CHAIN (params)) != 0)
	  {
	    error ("too many arguments to function `%s'",
		   IDENTIFIER_POINTER (DECL_NAME (function)));
	    return error_mark_node;
	  }

	arg0 = TREE_VALUE (params);
	arg1 = TREE_VALUE (TREE_CHAIN (params));
	arg0 = build_binary_op (code, arg0, arg1, 0);
	if (code != UNORDERED_EXPR)
	  arg0 = build_unary_op (TRUTH_NOT_EXPR, arg0, 0);
	return arg0;
      }
      break;

    default:
      break;
    }

  return NULL_TREE;
}

/* Returns non-zero if CODE is the code for a statement.  */

int
statement_code_p (code)
     enum tree_code code;
{
  switch (code)
    {
    case CLEANUP_STMT:
    case EXPR_STMT:
    case COMPOUND_STMT:
    case DECL_STMT:
    case IF_STMT:
    case FOR_STMT:
    case WHILE_STMT:
    case DO_STMT:
    case RETURN_STMT:
    case BREAK_STMT:
    case CONTINUE_STMT:
    case SCOPE_STMT:
    case SWITCH_STMT:
    case GOTO_STMT:
    case LABEL_STMT:
    case ASM_STMT:
    case FILE_STMT:
    case CASE_LABEL:
      return 1;

    default:
      if (lang_statement_code_p)
	return (*lang_statement_code_p) (code);
      return 0;
    }
}

/* Walk the statement tree, rooted at *tp.  Apply FUNC to all the
   sub-trees of *TP in a pre-order traversal.  FUNC is called with the
   DATA and the address of each sub-tree.  If FUNC returns a non-NULL
   value, the traversal is aborted, and the value returned by FUNC is
   returned.  If FUNC sets WALK_SUBTREES to zero, then the subtrees of
   the node being visited are not walked.

   We don't need a without_duplicates variant of this one because the
   statement tree is a tree, not a graph.  */

tree 
walk_stmt_tree (tp, func, data)
     tree *tp;
     walk_tree_fn func;
     void *data;
{
  enum tree_code code;
  int walk_subtrees;
  tree result;
  int i, len;

#define WALK_SUBTREE(NODE)				\
  do							\
    {							\
      result = walk_stmt_tree (&(NODE), func, data);	\
      if (result)					\
	return result;					\
    }							\
  while (0)

  /* Skip empty subtrees.  */
  if (!*tp)
    return NULL_TREE;

  /* Skip subtrees below non-statement nodes.  */
  if (!statement_code_p (TREE_CODE (*tp)))
    return NULL_TREE;

  /* Call the function.  */
  walk_subtrees = 1;
  result = (*func) (tp, &walk_subtrees, data);

  /* If we found something, return it.  */
  if (result)
    return result;

  /* FUNC may have modified the tree, recheck that we're looking at a
     statement node.  */
  code = TREE_CODE (*tp);
  if (!statement_code_p (code))
    return NULL_TREE;

  /* Visit the subtrees unless FUNC decided that there was nothing
     interesting below this point in the tree.  */
  if (walk_subtrees)
    {
      /* Walk over all the sub-trees of this operand.  Statement nodes
	 never contain RTL, and we needn't worry about TARGET_EXPRs.  */
      len = TREE_CODE_LENGTH (code);

      /* Go through the subtrees.  We need to do this in forward order so
	 that the scope of a FOR_EXPR is handled properly.  */
      for (i = 0; i < len; ++i)
	WALK_SUBTREE (TREE_OPERAND (*tp, i));
    }

  /* Finally visit the chain.  This can be tail-recursion optimized if
     we write it this way.  */
  return walk_stmt_tree (&TREE_CHAIN (*tp), func, data);

#undef WALK_SUBTREE
}

/* Used to compare case labels.  K1 and K2 are actually tree nodes
   representing case labels, or NULL_TREE for a `default' label.
   Returns -1 if K1 is ordered before K2, -1 if K1 is ordered after
   K2, and 0 if K1 and K2 are equal.  */

int
case_compare (k1, k2)
     splay_tree_key k1;
     splay_tree_key k2;
{
  /* Consider a NULL key (such as arises with a `default' label) to be
     smaller than anything else.  */
  if (!k1)
    return k2 ? -1 : 0;
  else if (!k2)
    return k1 ? 1 : 0;

  return tree_int_cst_compare ((tree) k1, (tree) k2);
}

/* Process a case label for the range LOW_VALUE ... HIGH_VALUE.  If
   LOW_VALUE and HIGH_VALUE are both NULL_TREE then this case label is
   actually a `default' label.  If only HIGH_VALUE is NULL_TREE, then
   case label was declared using the usual C/C++ syntax, rather than
   the GNU case range extension.  CASES is a tree containing all the
   case ranges processed so far; COND is the condition for the
   switch-statement itself.  Returns the CASE_LABEL created, or
   ERROR_MARK_NODE if no CASE_LABEL is created.  */

tree
c_add_case_label (cases, cond, low_value, high_value)
     splay_tree cases;
     tree cond;
     tree low_value;
     tree high_value;
{
  tree type;
  tree label;
  tree case_label;
  splay_tree_node node;

  /* Create the LABEL_DECL itself.  */
  label = build_decl (LABEL_DECL, NULL_TREE, NULL_TREE);
  DECL_CONTEXT (label) = current_function_decl;

  /* If there was an error processing the switch condition, bail now
     before we get more confused.  */
  if (!cond || cond == error_mark_node)
    {
      /* Add a label anyhow so that the back-end doesn't think that
	 the beginning of the switch is unreachable.  */
      if (!cases->root)
	add_stmt (build_case_label (NULL_TREE, NULL_TREE, label));
      return error_mark_node;
    }

  if ((low_value && TREE_TYPE (low_value) 
       && POINTER_TYPE_P (TREE_TYPE (low_value))) 
      || (high_value && TREE_TYPE (high_value)
	  && POINTER_TYPE_P (TREE_TYPE (high_value))))
    error ("pointers are not permitted as case values");

  /* Case ranges are a GNU extension.  */
  if (high_value && pedantic)
    {
      if (c_language == clk_cplusplus)
	pedwarn ("ISO C++ forbids range expressions in switch statements");
      else
	pedwarn ("ISO C forbids range expressions in switch statements");
    }

  type = TREE_TYPE (cond);
  if (low_value)
    {
      low_value = check_case_value (low_value);
      low_value = convert_and_check (type, low_value);
    }
  if (high_value)
    {
      high_value = check_case_value (high_value);
      high_value = convert_and_check (type, high_value);
    }

  /* If an error has occurred, bail out now.  */
  if (low_value == error_mark_node || high_value == error_mark_node)
    {
      if (!cases->root)
	add_stmt (build_case_label (NULL_TREE, NULL_TREE, label));
      return error_mark_node;
    }

  /* If the LOW_VALUE and HIGH_VALUE are the same, then this isn't
     really a case range, even though it was written that way.  Remove
     the HIGH_VALUE to simplify later processing.  */
  if (tree_int_cst_equal (low_value, high_value))
    high_value = NULL_TREE;
  if (low_value && high_value 
      && !tree_int_cst_lt (low_value, high_value)) 
    warning ("empty range specified");

  /* Look up the LOW_VALUE in the table of case labels we already
     have.  */
  node = splay_tree_lookup (cases, (splay_tree_key) low_value);
  /* If there was not an exact match, check for overlapping ranges.
     There's no need to do this if there's no LOW_VALUE or HIGH_VALUE;
     that's a `default' label and the only overlap is an exact match.  */
  if (!node && (low_value || high_value))
    {
      splay_tree_node low_bound;
      splay_tree_node high_bound;

      /* Even though there wasn't an exact match, there might be an
	 overlap between this case range and another case range.
	 Since we've (inductively) not allowed any overlapping case
	 ranges, we simply need to find the greatest low case label
	 that is smaller that LOW_VALUE, and the smallest low case
	 label that is greater than LOW_VALUE.  If there is an overlap
	 it will occur in one of these two ranges.  */
      low_bound = splay_tree_predecessor (cases,
					  (splay_tree_key) low_value);
      high_bound = splay_tree_successor (cases,
					 (splay_tree_key) low_value);

      /* Check to see if the LOW_BOUND overlaps.  It is smaller than
	 the LOW_VALUE, so there is no need to check unless the
	 LOW_BOUND is in fact itself a case range.  */
      if (low_bound
	  && CASE_HIGH ((tree) low_bound->value)
	  && tree_int_cst_compare (CASE_HIGH ((tree) low_bound->value),
				    low_value) >= 0)
	node = low_bound;
      /* Check to see if the HIGH_BOUND overlaps.  The low end of that
	 range is bigger than the low end of the current range, so we
	 are only interested if the current range is a real range, and
	 not an ordinary case label.  */
      else if (high_bound 
	       && high_value
	       && (tree_int_cst_compare ((tree) high_bound->key,
					 high_value)
		   <= 0))
	node = high_bound;
    }
  /* If there was an overlap, issue an error.  */
  if (node)
    {
      tree duplicate = CASE_LABEL_DECL ((tree) node->value);

      if (high_value)
	{
	  error ("duplicate (or overlapping) case value");
	  error_with_decl (duplicate, 
			   "this is the first entry overlapping that value");
	}
      else if (low_value)
	{
	  error ("duplicate case value") ;
	  error_with_decl (duplicate, "previously used here");
	}
      else
	{
	  error ("multiple default labels in one switch");
	  error_with_decl (duplicate, "this is the first default label");
	}
      if (!cases->root)
	add_stmt (build_case_label (NULL_TREE, NULL_TREE, label));
    }

  /* Add a CASE_LABEL to the statement-tree.  */
  case_label = add_stmt (build_case_label (low_value, high_value, label));
  /* Register this case label in the splay tree.  */
  splay_tree_insert (cases, 
		     (splay_tree_key) low_value,
		     (splay_tree_value) case_label);

  return case_label;
}

/* Finish an expression taking the address of LABEL.  Returns an
   expression for the address.  */

tree 
finish_label_address_expr (label)
     tree label;
{
  tree result;

  if (pedantic)
    {
      if (c_language == clk_cplusplus)
	pedwarn ("ISO C++ forbids taking the address of a label");
      else
	pedwarn ("ISO C forbids taking the address of a label");
    }

  label = lookup_label (label);
  if (label == NULL_TREE)
    result = null_pointer_node;
  else
    {
      TREE_USED (label) = 1;
      result = build1 (ADDR_EXPR, ptr_type_node, label);
      TREE_CONSTANT (result) = 1;
      /* The current function in not necessarily uninlinable.
	 Computed gotos are incompatible with inlining, but the value
	 here could be used only in a diagnostic, for example.  */
    }

  return result;
}

/* Mark P (a stmt_tree) for GC.  The use of a `void *' for the
   parameter allows this function to be used as a GC-marking
   function.  */

void
mark_stmt_tree (p)
     void *p;
{
  stmt_tree st = (stmt_tree) p;

  ggc_mark_tree (st->x_last_stmt);
  ggc_mark_tree (st->x_last_expr_type);
}

/* Mark LD for GC.  */

void
c_mark_lang_decl (c)
     struct c_lang_decl *c ATTRIBUTE_UNUSED;
{
}

/* Mark F for GC.  */

void
mark_c_language_function (f)
     struct language_function *f;
{
  if (!f)
    return;

  mark_stmt_tree (&f->x_stmt_tree);
  ggc_mark_tree (f->x_scope_stmt_stack);
}

/* Hook used by expand_expr to expand language-specific tree codes.  */

rtx
c_expand_expr (exp, target, tmode, modifier)
     tree exp;
     rtx target;
     enum machine_mode tmode;
     enum expand_modifier modifier;
{
  switch (TREE_CODE (exp))
    {
    case STMT_EXPR:
      {
	tree rtl_expr;
	rtx result;
	bool preserve_result = false;

	/* Since expand_expr_stmt calls free_temp_slots after every
	   expression statement, we must call push_temp_slots here.
	   Otherwise, any temporaries in use now would be considered
	   out-of-scope after the first EXPR_STMT from within the
	   STMT_EXPR.  */
	push_temp_slots ();
	rtl_expr = expand_start_stmt_expr (!STMT_EXPR_NO_SCOPE (exp));

	/* If we want the result of this expression, find the last
           EXPR_STMT in the COMPOUND_STMT and mark it as addressable.  */
	if (target != const0_rtx
	    && TREE_CODE (STMT_EXPR_STMT (exp)) == COMPOUND_STMT
	    && TREE_CODE (COMPOUND_BODY (STMT_EXPR_STMT (exp))) == SCOPE_STMT)
	  {
	    tree expr = COMPOUND_BODY (STMT_EXPR_STMT (exp));
	    tree last = TREE_CHAIN (expr);

	    while (TREE_CHAIN (last))
	      {
		expr = last;
		last = TREE_CHAIN (last);
	      }

	    if (TREE_CODE (last) == SCOPE_STMT
		&& TREE_CODE (expr) == EXPR_STMT)
	      {
	        TREE_ADDRESSABLE (expr) = 1;
		preserve_result = true;
	      }
	  }

	bli_expand_stmt (STMT_EXPR_STMT (exp));
	expand_end_stmt_expr (rtl_expr);

	result = expand_expr (rtl_expr, target, tmode, modifier);
	if (preserve_result && GET_CODE (result) == MEM)
	  {
	    if (GET_MODE (result) != BLKmode)
	      result = copy_to_reg (result);
	    else
	      preserve_temp_slots (result);
	  }

	/* If the statment-expression does not have a scope, then the
	   new temporaries we created within it must live beyond the
	   statement-expression.  */
	if (STMT_EXPR_NO_SCOPE (exp))
	  preserve_temp_slots (NULL_RTX);

	pop_temp_slots ();
	return result;
      }
      break;
      
    case CALL_EXPR:
      {
	if (TREE_CODE (TREE_OPERAND (exp, 0)) == ADDR_EXPR
	    && (TREE_CODE (TREE_OPERAND (TREE_OPERAND (exp, 0), 0))
		== FUNCTION_DECL)
	    && DECL_BUILT_IN (TREE_OPERAND (TREE_OPERAND (exp, 0), 0))
	    && (DECL_BUILT_IN_CLASS (TREE_OPERAND (TREE_OPERAND (exp, 0), 0))
		== BUILT_IN_FRONTEND))
	  return c_expand_builtin (exp, target, tmode, modifier);
	else
	  abort ();
      }
      break;

    case COMPOUND_LITERAL_EXPR:
      {
	/* Initialize the anonymous variable declared in the compound
	   literal, then return the variable.  */
	tree decl = COMPOUND_LITERAL_EXPR_DECL (exp);
	emit_local_var (decl);
	return expand_expr (decl, target, tmode, modifier);
      }

    default:
      abort ();
    }

  abort ();
  return NULL;
}

/* Hook used by safe_from_p to handle language-specific tree codes.  */

int
c_safe_from_p (target, exp)
     rtx target;
     tree exp;
{
  /* We can see statements here when processing the body of a
     statement-expression.  For a declaration statement declaring a
     variable, look at the variable's initializer.  */
  if (TREE_CODE (exp) == DECL_STMT) 
    {
      tree decl = DECL_STMT_DECL (exp);

      if (TREE_CODE (decl) == VAR_DECL
	  && DECL_INITIAL (decl)
	  && !safe_from_p (target, DECL_INITIAL (decl), /*top_p=*/0))
	return 0;
    }

  /* For any statement, we must follow the statement-chain.  */
  if (statement_code_p (TREE_CODE (exp)) && TREE_CHAIN (exp))
    return safe_from_p (target, TREE_CHAIN (exp), /*top_p=*/0);

  /* Assume everything else is safe.  */
  return 1;
}

/* Hook used by unsafe_for_reeval to handle language-specific tree codes.  */

int
c_unsafe_for_reeval (exp)
     tree exp;
{
  /* Statement expressions may not be reevaluated, likewise compound
     literals.  */
  if (TREE_CODE (exp) == STMT_EXPR
      || TREE_CODE (exp) == COMPOUND_LITERAL_EXPR)
    return 2;

  /* Walk all other expressions.  */
  return -1;
}

/* Hook used by staticp to handle language-specific tree codes.  */

int
c_staticp (exp)
     tree exp;
{
  if (TREE_CODE (exp) == COMPOUND_LITERAL_EXPR
      && TREE_STATIC (COMPOUND_LITERAL_EXPR_DECL (exp)))
    return 1;
  return 0;
}

/* Tree code classes.  */

#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) TYPE,

static const char c_tree_code_type[] = {
  'x',
#include "c-common.def"
};
#undef DEFTREECODE

/* Table indexed by tree code giving number of expression
   operands beyond the fixed part of the node structure.
   Not used for types or decls.  */

#define DEFTREECODE(SYM, NAME, TYPE, LENGTH) LENGTH,

static const int c_tree_code_length[] = {
  0,
#include "c-common.def"
};
#undef DEFTREECODE

/* Names of tree components.
   Used for printing out the tree and error messages.  */
#define DEFTREECODE(SYM, NAME, TYPE, LEN) NAME,

static const char *const c_tree_code_name[] = {
  "@@dummy",
#include "c-common.def"
};
#undef DEFTREECODE

/* Adds the tree codes specific to the C front end to the list of all
   tree codes.  */

void
add_c_tree_codes ()
{
  memcpy (tree_code_type + (int) LAST_AND_UNUSED_TREE_CODE,
	  c_tree_code_type,
	  (int) LAST_C_TREE_CODE - (int) LAST_AND_UNUSED_TREE_CODE);
  memcpy (tree_code_length + (int) LAST_AND_UNUSED_TREE_CODE,
	  c_tree_code_length,
	  (LAST_C_TREE_CODE - (int) LAST_AND_UNUSED_TREE_CODE) * sizeof (int));
  memcpy (tree_code_name + (int) LAST_AND_UNUSED_TREE_CODE,
	  c_tree_code_name,
	  (LAST_C_TREE_CODE - (int) LAST_AND_UNUSED_TREE_CODE) * sizeof (char *));
  lang_unsafe_for_reeval = c_unsafe_for_reeval;
}

#define CALLED_AS_BUILT_IN(NODE) \
   (!strncmp (IDENTIFIER_POINTER (DECL_NAME (NODE)), "__builtin_", 10))

static rtx
c_expand_builtin (exp, target, tmode, modifier)
     tree exp;
     rtx target;
     enum machine_mode tmode;
     enum expand_modifier modifier;
{
  tree type = TREE_TYPE (exp);
  tree fndecl = TREE_OPERAND (TREE_OPERAND (exp, 0), 0);
  tree arglist = TREE_OPERAND (exp, 1);
  enum built_in_function fcode = DECL_FUNCTION_CODE (fndecl);
  enum tree_code code = TREE_CODE (exp);
  const int ignore = (target == const0_rtx
		      || ((code == NON_LVALUE_EXPR || code == NOP_EXPR
			   || code == CONVERT_EXPR || code == REFERENCE_EXPR
			   || code == COND_EXPR)
			  && TREE_CODE (type) == VOID_TYPE));

  if (! optimize && ! CALLED_AS_BUILT_IN (fndecl))
    return expand_call (exp, target, ignore);

  switch (fcode)
    {
    case BUILT_IN_PRINTF:
      target = c_expand_builtin_printf (arglist, target, tmode,
					modifier, ignore, /*unlocked=*/ 0);
      if (target)
	return target;
      break;

    case BUILT_IN_PRINTF_UNLOCKED:
      target = c_expand_builtin_printf (arglist, target, tmode,
					modifier, ignore, /*unlocked=*/ 1);
      if (target)
	return target;
      break;

    case BUILT_IN_FPRINTF:
      target = c_expand_builtin_fprintf (arglist, target, tmode,
					 modifier, ignore, /*unlocked=*/ 0);
      if (target)
	return target;
      break;

    case BUILT_IN_FPRINTF_UNLOCKED:
      target = c_expand_builtin_fprintf (arglist, target, tmode,
					 modifier, ignore, /*unlocked=*/ 1);
      if (target)
	return target;
      break;

    default:			/* just do library call, if unknown builtin */
      error ("built-in function `%s' not currently supported",
	     IDENTIFIER_POINTER (DECL_NAME (fndecl)));
    }

  /* The switch statement above can drop through to cause the function
     to be called normally.  */
  return expand_call (exp, target, ignore);
}

/* Check an arglist to *printf for problems.  The arglist should start
   at the format specifier, with the remaining arguments immediately
   following it.  */
static int
is_valid_printf_arglist (arglist)
     tree arglist;
{
  /* Save this value so we can restore it later.  */
  const int SAVE_pedantic = pedantic;
  int diagnostic_occurred = 0;
  tree attrs;

  /* Set this to a known value so the user setting won't affect code
     generation.  */
  pedantic = 1;
  /* Check to make sure there are no format specifier errors.  */
  attrs = tree_cons (get_identifier ("format"),
		     tree_cons (NULL_TREE,
				get_identifier ("printf"),
				tree_cons (NULL_TREE,
					   integer_one_node,
					   tree_cons (NULL_TREE,
						      build_int_2 (2, 0),
						      NULL_TREE))),
		     NULL_TREE);
  check_function_format (&diagnostic_occurred, attrs, arglist);

  /* Restore the value of `pedantic'.  */
  pedantic = SAVE_pedantic;

  /* If calling `check_function_format_ptr' produces a warning, we
     return false, otherwise we return true.  */
  return ! diagnostic_occurred;
}

/* If the arguments passed to printf are suitable for optimizations,
   we attempt to transform the call.  */
static rtx
c_expand_builtin_printf (arglist, target, tmode, modifier, ignore, unlocked)
     tree arglist;
     rtx target;
     enum machine_mode tmode;
     enum expand_modifier modifier;
     int ignore;
     int unlocked;
{
  tree fn_putchar = unlocked ?
    built_in_decls[BUILT_IN_PUTCHAR_UNLOCKED] : built_in_decls[BUILT_IN_PUTCHAR];
  tree fn_puts = unlocked ?
    built_in_decls[BUILT_IN_PUTS_UNLOCKED] : built_in_decls[BUILT_IN_PUTS];
  tree fn, format_arg, stripped_string;

  /* If the return value is used, or the replacement _DECL isn't
     initialized, don't do the transformation.  */
  if (!ignore || !fn_putchar || !fn_puts)
    return 0;

  /* Verify the required arguments in the original call.  */
  if (arglist == 0
      || (TREE_CODE (TREE_TYPE (TREE_VALUE (arglist))) != POINTER_TYPE))
    return 0;
  
  /* Check the specifier vs. the parameters.  */
  if (!is_valid_printf_arglist (arglist))
    return 0;
  
  format_arg = TREE_VALUE (arglist);
  stripped_string = format_arg;
  STRIP_NOPS (stripped_string);
  if (stripped_string && TREE_CODE (stripped_string) == ADDR_EXPR)
    stripped_string = TREE_OPERAND (stripped_string, 0);

  /* If the format specifier isn't a STRING_CST, punt.  */
  if (TREE_CODE (stripped_string) != STRING_CST)
    return 0;
  
  /* OK!  We can attempt optimization.  */

  /* If the format specifier was "%s\n", call __builtin_puts(arg2).  */
  if (strcmp (TREE_STRING_POINTER (stripped_string), "%s\n") == 0)
    {
      arglist = TREE_CHAIN (arglist);
      fn = fn_puts;
    }
  /* If the format specifier was "%c", call __builtin_putchar (arg2).  */
  else if (strcmp (TREE_STRING_POINTER (stripped_string), "%c") == 0)
    {
      arglist = TREE_CHAIN (arglist);
      fn = fn_putchar;
    }
  else
    {
      /* We can't handle anything else with % args or %% ... yet.  */
      if (strchr (TREE_STRING_POINTER (stripped_string), '%'))
	return 0;
      
      /* If the resulting constant string has a length of 1, call
         putchar.  Note, TREE_STRING_LENGTH includes the terminating
         NULL in its count.  */
      if (TREE_STRING_LENGTH (stripped_string) == 2)
        {
	  /* Given printf("c"), (where c is any one character,)
             convert "c"[0] to an int and pass that to the replacement
             function.  */
	  arglist = build_int_2 (TREE_STRING_POINTER (stripped_string)[0], 0);
	  arglist = build_tree_list (NULL_TREE, arglist);
	  
	  fn = fn_putchar;
        }
      /* If the resulting constant was "string\n", call
         __builtin_puts("string").  Ensure "string" has at least one
         character besides the trailing \n.  Note, TREE_STRING_LENGTH
         includes the terminating NULL in its count.  */
      else if (TREE_STRING_LENGTH (stripped_string) > 2
	       && TREE_STRING_POINTER (stripped_string)
	       [TREE_STRING_LENGTH (stripped_string) - 2] == '\n')
        {
	  /* Create a NULL-terminated string that's one char shorter
	     than the original, stripping off the trailing '\n'.  */
	  const int newlen = TREE_STRING_LENGTH (stripped_string) - 1;
	  char *newstr = (char *) alloca (newlen);
	  memcpy (newstr, TREE_STRING_POINTER (stripped_string), newlen - 1);
	  newstr[newlen - 1] = 0;
	  
	  arglist = combine_strings (build_string (newlen, newstr));
	  arglist = build_tree_list (NULL_TREE, arglist);
	  fn = fn_puts;
	}
      else
	/* We'd like to arrange to call fputs(string) here, but we
           need stdout and don't have a way to get it ... yet.  */
	return 0;
    }
  
  return expand_expr (build_function_call (fn, arglist),
		      (ignore ? const0_rtx : target),
		      tmode, modifier);
}

/* If the arguments passed to fprintf are suitable for optimizations,
   we attempt to transform the call.  */
static rtx
c_expand_builtin_fprintf (arglist, target, tmode, modifier, ignore, unlocked)
     tree arglist;
     rtx target;
     enum machine_mode tmode;
     enum expand_modifier modifier;
     int ignore;
     int unlocked;
{
  tree fn_fputc = unlocked ?
    built_in_decls[BUILT_IN_FPUTC_UNLOCKED] : built_in_decls[BUILT_IN_FPUTC];
  tree fn_fputs = unlocked ?
    built_in_decls[BUILT_IN_FPUTS_UNLOCKED] : built_in_decls[BUILT_IN_FPUTS];
  tree fn, format_arg, stripped_string;

  /* If the return value is used, or the replacement _DECL isn't
     initialized, don't do the transformation.  */
  if (!ignore || !fn_fputc || !fn_fputs)
    return 0;

  /* Verify the required arguments in the original call.  */
  if (arglist == 0
      || (TREE_CODE (TREE_TYPE (TREE_VALUE (arglist))) != POINTER_TYPE)
      || (TREE_CHAIN (arglist) == 0)
      || (TREE_CODE (TREE_TYPE (TREE_VALUE (TREE_CHAIN (arglist)))) !=
	  POINTER_TYPE))
    return 0;
  
  /* Check the specifier vs. the parameters.  */
  if (!is_valid_printf_arglist (TREE_CHAIN (arglist)))
    return 0;
  
  format_arg = TREE_VALUE (TREE_CHAIN (arglist));
  stripped_string = format_arg;
  STRIP_NOPS (stripped_string);
  if (stripped_string && TREE_CODE (stripped_string) == ADDR_EXPR)
    stripped_string = TREE_OPERAND (stripped_string, 0);

  /* If the format specifier isn't a STRING_CST, punt.  */
  if (TREE_CODE (stripped_string) != STRING_CST)
    return 0;
  
  /* OK!  We can attempt optimization.  */

  /* If the format specifier was "%s", call __builtin_fputs(arg3, arg1).  */
  if (strcmp (TREE_STRING_POINTER (stripped_string), "%s") == 0)
    {
      tree newarglist = build_tree_list (NULL_TREE, TREE_VALUE (arglist));
      arglist = tree_cons (NULL_TREE,
			   TREE_VALUE (TREE_CHAIN (TREE_CHAIN (arglist))),
			   newarglist);
      fn = fn_fputs;
    }
  /* If the format specifier was "%c", call __builtin_fputc (arg3, arg1).  */
  else if (strcmp (TREE_STRING_POINTER (stripped_string), "%c") == 0)
    {
      tree newarglist = build_tree_list (NULL_TREE, TREE_VALUE (arglist));
      arglist = tree_cons (NULL_TREE,
			   TREE_VALUE (TREE_CHAIN (TREE_CHAIN (arglist))),
			   newarglist);
      fn = fn_fputc;
    }
  else
    {
      /* We can't handle anything else with % args or %% ... yet.  */
      if (strchr (TREE_STRING_POINTER (stripped_string), '%'))
	return 0;
      
      /* When "string" doesn't contain %, replace all cases of
         fprintf(stream,string) with fputs(string,stream).  The fputs
         builtin will take take of special cases like length==1.  */
      arglist = tree_cons (NULL_TREE, TREE_VALUE (TREE_CHAIN (arglist)),
			   build_tree_list (NULL_TREE, TREE_VALUE (arglist)));
      fn = fn_fputs;
    }
  
  return expand_expr (build_function_call (fn, arglist),
		      (ignore ? const0_rtx : target),
		      tmode, modifier);
}


/* Given a boolean expression ARG, return a tree representing an increment
   or decrement (as indicated by CODE) of ARG.  The front end must check for
   invalid cases (e.g., decrement in C++).  */
tree
boolean_increment (code, arg)
     enum tree_code code;
     tree arg;
{
  tree val;
  tree true_res = (c_language == clk_cplusplus
		   ? boolean_true_node
		   : c_bool_true_node);
  arg = stabilize_reference (arg);
  switch (code)
    {
    case PREINCREMENT_EXPR:
      val = build (MODIFY_EXPR, TREE_TYPE (arg), arg, true_res);
      break;
    case POSTINCREMENT_EXPR:
      val = build (MODIFY_EXPR, TREE_TYPE (arg), arg, true_res);
      arg = save_expr (arg);
      val = build (COMPOUND_EXPR, TREE_TYPE (arg), val, arg);
      val = build (COMPOUND_EXPR, TREE_TYPE (arg), arg, val);
      break;
    case PREDECREMENT_EXPR:
      val = build (MODIFY_EXPR, TREE_TYPE (arg), arg, invert_truthvalue (arg));
      break;
    case POSTDECREMENT_EXPR:
      val = build (MODIFY_EXPR, TREE_TYPE (arg), arg, invert_truthvalue (arg));
      arg = save_expr (arg);
      val = build (COMPOUND_EXPR, TREE_TYPE (arg), val, arg);
      val = build (COMPOUND_EXPR, TREE_TYPE (arg), arg, val);
      break;
    default:
      abort ();
    }
  TREE_SIDE_EFFECTS (val) = 1;
  return val;
}

/* Handle C and C++ default attributes.  */

enum built_in_attribute
{
#define DEF_ATTR_NULL_TREE(ENUM) ENUM,
#define DEF_ATTR_INT(ENUM, VALUE) ENUM,
#define DEF_ATTR_IDENT(ENUM, STRING) ENUM,
#define DEF_ATTR_TREE_LIST(ENUM, PURPOSE, VALUE, CHAIN) ENUM,
#define DEF_FN_ATTR(NAME, ATTRS, PREDICATE) /* No entry needed in enum.  */
#include "builtin-attrs.def"
#undef DEF_ATTR_NULL_TREE
#undef DEF_ATTR_INT
#undef DEF_ATTR_IDENT
#undef DEF_ATTR_TREE_LIST
#undef DEF_FN_ATTR
  ATTR_LAST
};

static tree built_in_attributes[(int) ATTR_LAST];

static bool c_attrs_initialized = false;

static void c_init_attributes PARAMS ((void));

/* Output a -Wshadow warning MSGID about NAME, an IDENTIFIER_NODE, and
   additionally give the location of the previous declaration DECL.  */
void
shadow_warning (msgid, name, decl)
     const char *msgid;
     tree name, decl;
{
  warning ("declaration of `%s' shadows %s", IDENTIFIER_POINTER (name), msgid);
  warning_with_file_and_line (DECL_SOURCE_FILE (decl),
			      DECL_SOURCE_LINE (decl),
			      "shadowed declaration is here");
}

/* Language-level data type conversion for GNU C.
   Copyright (C) 1987, 1988, 1991, 1998 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */


/* This file contains the functions for converting C expressions
   to different data types.  The only entry point is `convert'.
   Every language front end must have a `convert' function
   but what kind of conversions it does will depend on the language.  */

#include "system.h"
#include "flags.h"
#include "convert.h"
#include "toplev.h"

/* Change of width--truncation and extension of integers or reals--
   is represented with NOP_EXPR.  Proper functioning of many things
   assumes that no other conversions can be NOP_EXPRs.

   Conversion between integer and pointer is represented with CONVERT_EXPR.
   Converting integer to real uses FLOAT_EXPR
   and real to integer uses FIX_TRUNC_EXPR.

   Here is a list of all the functions that assume that widening and
   narrowing is always done with a NOP_EXPR:
     In convert.c, convert_to_integer.
     In c-typeck.c, build_binary_op (boolean ops), and truthvalue_conversion.
     In expr.c: expand_expr, for operands of a MULT_EXPR.
     In fold-const.c: fold.
     In tree.c: get_narrower and get_unwidened.  */

/* Subroutines of `convert'.  */



/* Create an expression whose value is that of EXPR,
   converted to type TYPE.  The TREE_TYPE of the value
   is always TYPE.  This function implements all reasonable
   conversions; callers should filter out those that are
   not permitted by the language being compiled.  */

tree
convert (type, expr)
     tree type, expr;
{
  tree e = expr;
  enum tree_code code = TREE_CODE (type);

  if (type == TREE_TYPE (expr)
      || TREE_CODE (expr) == ERROR_MARK
      || code == ERROR_MARK || TREE_CODE (TREE_TYPE (expr)) == ERROR_MARK)
    return expr;

  if (TYPE_MAIN_VARIANT (type) == TYPE_MAIN_VARIANT (TREE_TYPE (expr)))
    return fold (build1 (NOP_EXPR, type, expr));
  if (TREE_CODE (TREE_TYPE (expr)) == ERROR_MARK)
    return error_mark_node;
  if (TREE_CODE (TREE_TYPE (expr)) == VOID_TYPE)
    {
      error ("void value not ignored as it ought to be");
      return error_mark_node;
    }
  if (code == VOID_TYPE)
    return build1 (CONVERT_EXPR, type, e);
#if 0
  /* This is incorrect.  A truncation can't be stripped this way.
     Extensions will be stripped by the use of get_unwidened.  */
  if (TREE_CODE (expr) == NOP_EXPR)
    return convert (type, TREE_OPERAND (expr, 0));
#endif
  if (code == INTEGER_TYPE || code == ENUMERAL_TYPE)
    return fold (convert_to_integer (type, e));
  if (code == BOOLEAN_TYPE)
    {
      tree t = truthvalue_conversion (expr);
      /* If truthvalue_conversion returns a NOP_EXPR, we must fold it here
	 to avoid infinite recursion between fold () and convert ().  */
      if (TREE_CODE (t) == NOP_EXPR)
	return fold (build1 (NOP_EXPR, type, TREE_OPERAND (t, 0)));
      else
	return fold (build1 (NOP_EXPR, type, t));
    }
  if (code == POINTER_TYPE || code == REFERENCE_TYPE)
    return fold (convert_to_pointer (type, e));
  if (code == REAL_TYPE)
    return fold (convert_to_real (type, e));
  if (code == COMPLEX_TYPE)
    return fold (convert_to_complex (type, e));
  if (code == VECTOR_TYPE)
    return fold (convert_to_vector (type, e));

  error ("conversion to non-scalar type requested");
  return error_mark_node;
}

#if 0
static tree current_static_block = NULL_TREE;

tree
bli_add_actn (fndecl, expr)
     tree fndecl, expr;
{
#define DECL_FUNCTION_BODY(DECL) (DECL_LANG_SPECIFIC(DECL)->function_decl_body)
#define GET_CURRENT_BLOCK(F) ((F) ? DECL_FUNCTION_BODY ((F)) :  current_static_block)
  if (!GET_CURRENT_BLOCK (fndecl))
    return NULL_TREE;
  return add_stmt_to_block (GET_CURRENT_BLOCK (fndecl), NULL_TREE, expr);
}

static tree
add_stmt_to_block (b, type, stmt)
     tree b, type, stmt;
{
#define BLOCK_EXPR_BODY(NODE)   BLOCK_SUBBLOCKS(NODE)
  tree body = BLOCK_EXPR_BODY (b), c;
  
  if ((c = add_stmt_to_compound (body, type, stmt)) == body)
    return body;

  BLOCK_EXPR_BODY (b) = c;
  TREE_SIDE_EFFECTS (c) = 1;
  return c;
}

tree
add_stmt_to_compound (existing, type, stmt)
     tree existing, type, stmt;
{
  if (existing)
    return build (COMPOUND_EXPR, type, existing, stmt);
  else
    return stmt;
}
#endif


tree
bli_add_stmt (t)
     tree t;
{
  if (input_filename != last_expr_filename)
    {
      /* If the filename has changed, also add in a FILE_STMT.  Do a string
         compare first, though, as it might be an equivalent string.  */
      int add = (strcmp (input_filename, last_expr_filename) != 0);
      last_expr_filename = input_filename;
      if (add)
        {
          tree pos = build_nt (FILE_STMT, get_identifier (input_filename));
          add_stmt (pos);
        }
    }

  /* Add T to the statement-tree.  */
  TREE_CHAIN (last_tree) = t;
  last_tree = t;
  
  /* When we expand a statement-tree, we must know whether or not the
     statements are full-expressions.  We record that fact here.  */
  STMT_IS_FULL_EXPR_P (last_tree) = stmts_are_full_exprs_p ();

  /* Keep track of the number of statements in this function.  */
  if (current_function_decl)
    ++DECL_NUM_STMTS (current_function_decl);

  return t;
}

void
bli_expand_stmt (t)
     tree t;
{
  while (t && t != error_mark_node)
    {
      int saved_stmts_are_full_exprs_p;

      /* Set up context appropriately for handling this statement.  */
      saved_stmts_are_full_exprs_p = stmts_are_full_exprs_p ();
      bli_prep_stmt (t);
      switch (TREE_CODE (t))
        {
	default:
	  abort();
	  break;
	}
      /* Restore saved state.  */
      current_stmt_tree ()->stmts_are_full_exprs_p
        = saved_stmts_are_full_exprs_p;

      /* Go on to the next statement in this scope.  */
      t = TREE_CHAIN (t);
    }

}



bli_prep_stmt (t)
     tree t;
{
  if (!STMT_LINENO_FOR_FN_P (t))
    lineno = STMT_LINENO (t);
  current_stmt_tree ()->stmts_are_full_exprs_p = STMT_IS_FULL_EXPR_P (t);
}
