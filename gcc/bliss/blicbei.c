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
maybe_objc_comptypes2 (tree lhs ATTRIBUTE_UNUSED,
                      tree rhs ATTRIBUTE_UNUSED,
                      int reflexive ATTRIBUTE_UNUSED)
{
  return -1;
}

/* Used by c-typeck.c (build_external_ref), but only for objc.  */

tree
lookup_objc_ivar2 (tree id ATTRIBUTE_UNUSED)
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
maybe_building_objc_message_expr2 ()
{
  return 0;
}

/* C warning, ignore.  */

void
pedwarn_c99 VPARAMS ((const char *msgid ATTRIBUTE_UNUSED, ...))
{
  return;
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

void
maybe_objc_check_decl2 (tree decl ATTRIBUTE_UNUSED)
 {
   abort ();
 }

