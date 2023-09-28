/* Process declarations and variables for C compiler.
   Copyright (C) 1988, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
   2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

/* Process declarations and symbol lookup for C front end.
   Also constructs types; the standard scalar types at initialization,
   and structure, union, array and enum types when they are declared.  */

/* ??? not all decl nodes are given the most useful possible
   line numbers.  For example, the CONST_DECLs for enum values.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "input.h"
#include "tm.h"
#include "intl.h"
#include "tree.h"
#include "tree-inline.h"
#include "rtl.h"
#include "flags.h"
#include "function.h"
#include "output.h"
#include "expr.h"
//#include "c-tree.h"
#include "toplev.h"
#include "ggc.h"
#include "tm_p.h"
#include "cpplib.h"
#include "target.h"
#include "debug.h"
#include "opts.h"
#include "timevar.h"
//#include "c/c-common.h"
#include "c/c-tree.h"
#if 0
#include "c-pragma.h"
#endif
#include "langhooks.h"
//#include "tree-mudflap.h"
#include "tree-iterator.h"
#include "diagnostic.h"
#include "tree-dump.h"
#include "cgraph.h"
#include "hashtab.h"
#include "libfuncs.h"
#include "except.h"
#include "langhooks-def.h"
//#include "pointer-set.h"

// BLISS adds

#if 0
#include "bliss-tree.h"
#endif

struct GTY((chain_next ("%h.prev"))) c_binding {
  union GTY(()) {		/* first so GTY desc can use decl */
    tree GTY((tag ("0"))) type; /* the type in this scope */
    struct c_label_vars * GTY((tag ("1"))) label; /* for warnings */
  } GTY((desc ("TREE_CODE (%0.decl) == LABEL_DECL"))) u;
  tree decl;			/* the decl bound */
  tree id;			/* the identifier it's bound to */
  struct c_binding *prev;	/* the previous decl in this scope */
  struct c_binding *shadowed;	/* the innermost decl shadowed by this one */
  unsigned int depth : 28;      /* depth of this scope */
  BOOL_BITFIELD invisible : 1;  /* normal lookup should ignore this binding */
  BOOL_BITFIELD nested : 1;     /* do not set DECL_CONTEXT when popping */
  BOOL_BITFIELD inner_comp : 1; /* incomplete array completed in inner scope */
  BOOL_BITFIELD in_struct : 1;	/* currently defined as struct field */
  location_t locus;		/* location for nested bindings */
};

#define I_LABEL_BINDING(node) (*i_label_binding (node))
#define I_SYMBOL_BINDING(node) (*i_symbol_binding (node))

static GTY(()) struct c_scope *current_function_scope;
static GTY(()) struct c_scope *current_scope;
static GTY(()) struct c_scope *file_scope;

enum decl_context
{ NORMAL,			/* Ordinary declaration */
  FUNCDEF,			/* Function definition */
  PARM,				/* Declaration of parm before function body */
  FIELD,			/* Declaration inside struct or union */
  TYPENAME};			/* Typename (inside cast or sizeof)  */

enum deprecated_states {
  DEPRECATED_NORMAL,
  DEPRECATED_SUPPRESS,
  UNAVAILABLE_DEPRECATED_SUPPRESS
};

extern tree
lookup_tag (enum tree_code code, tree name, bool thislevel_only,
            location_t *ploc);
extern void
pushtag (location_t loc, tree name, tree type);

extern tree
grokdeclarator (const struct c_declarator *declarator,
		struct c_declspecs *declspecs,
		enum decl_context decl_context, bool initialized, tree *width,
		tree *decl_attrs, tree *expr, bool *expr_const_operands,
		enum deprecated_states deprecated_state);

extern struct c_binding **
i_label_binding (tree node);

extern struct c_binding **
i_symbol_binding (tree node);

tree
start_structure (
		 enum tree_code code,
		 tree name)
{
  /* If there is already a tag defined at this binding level
     (as a forward reference), just return it.  */

  tree ref = 0;

  if (name != 0)
    ref = lookup_tag (code, name, 1, &input_location);
  if (ref && TREE_CODE (ref) == code)
    {
      C_TYPE_BEING_DEFINED (ref) = 1;
      TYPE_PACKED (ref) = flag_pack_struct;
      if (TYPE_FIELDS (ref))
        {
	  if (code == STRUCTURE_TYPE)
	    error ("redefinition of `union %s'",
		   IDENTIFIER_POINTER (name));
	}  

      return ref;
    }

  /* Otherwise create a forward-reference just so the tag is in scope.  */

  ref = make_node (code);
  pushtag (input_location, name, ref);
  C_TYPE_BEING_DEFINED (ref) = 1;
  //  TYPE_PACKED (ref) = flag_pack_struct;
  return ref;
}

tree
finish_structure (
		  tree t,
		  tree fieldlist,
		  tree access_formal,
		  tree allocation_formal,
		  tree structure_size,
		  tree structure_body,
		  tree attributes)
{
  tree x;
  int toplevel = file_scope == current_scope;
  int saw_named_field;

  /* If this type was previously laid out as a forward reference,
     make sure we lay it out again.  */

  TYPE_SIZE (t) = 0;

#if 0
  decl_attributes (&t, attributes, (int) ATTR_FLAG_TYPE_IN_PLACE);
#endif

  /* Install struct as DECL_CONTEXT of each field decl.
     Also process specified field sizes,m which is found in the DECL_INITIAL.
     Store 0 there, except for ": 0" fields (so we can find them
     and delete them, below).  */

  saw_named_field = 0;
  /* Now we have the nearly final fieldlist.  Record it,
     then lay out the structure or union (including the fields).  */

  TYPE_FIELDS (t) = fieldlist;

  /* Now we have the truly final field list.
     Store it in this type and in the variants.  */

  TYPE_FIELDS (t) = fieldlist;

  return t;
}

void
push_parm_decl_init (tree parm, tree init)
{
  tree decl;

  /* Don't attempt to expand sizes while parsing this decl.
     (We can get here with i_s_e 1 somehow from Objective-C.)  */
#if 0
  int save_immediate_size_expand = immediate_size_expand;
  immediate_size_expand = 0;
#endif

  decl = grokdeclarator ((const struct c_declarator *) TREE_VALUE (TREE_PURPOSE (parm)),
			 (struct c_declspecs *) TREE_PURPOSE (TREE_PURPOSE (parm)),
			 PARM, 0, NULL, NULL, NULL, NULL, DEPRECATED_NORMAL);
#if 0
  // check
  decl_attributes (&decl, TREE_VALUE (parm), 0);
#endif

  decl = pushdecl (decl);

  finish_decl (decl, input_location, NULL_TREE, NULL_TREE, NULL_TREE);
  DECL_INITIAL(decl)=init;
  DECL_RESULT(decl)=init; // borrow this field?
  // TODO DECL_LANG_SPECIFIC(decl)=init; // borrow this field?

#if 0
  immediate_size_expand = save_immediate_size_expand;
#endif
}

tree
my_lookup_tag(tree t)
{
  return lookup_tag(LINKAGE, t, 1, &input_location);
}

void
reset_symbol(tree t)
{
  // TODO I_SYMBOL_BINDING(t) = 0;
}

tree
get_label_binding(tree t)
{
  return 0; // TODOI_LABEL_BINDING(t);
}

void
set_label_binding(tree t)
{
  //I_LABEL_BINDING(t) = 0;
  //c_binding ** b = I_LABEL_BINDING(t);
  // TODO * b = 0;
}


tree
binding_decl(struct c_binding * c) {
  return c->decl;
}
