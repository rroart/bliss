/* Build expressions with type checking for C compiler.
   Copyright (C) 1987, 1988, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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


/* This file is part of the C front end.
   It contains routines to build C expressions given their operands,
   including computing the types of the result, C-specific error checks,
   and some optimization.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "langhooks.h"
//#include "c-tree.h"
#include "tm_p.h"
#include "flags.h"
#include "output.h"
#include "expr.h"
#include "toplev.h"
#include "intl.h"
#include "ggc.h"
#include "target.h"
#include "tree-iterator.h"
//#include "tree-gimple.h"
//#include "tree-flow.h"

#include "bliss.h"

struct c_switch {
  /* The SWITCH_STMT being built.  */
  tree switch_stmt;

  /* The original type of the testing expression, i.e. before the
     default conversion is applied.  */
  tree orig_type;

  /* A splay-tree mapping the low element of a case range to the high
     element, or NULL_TREE if there is no high element.  Used to
     determine whether or not a new case label duplicates an old case
     label.  We need a tree, rather than simply a hash table, because
     of the GNU case range extension.  */
  // TODO splay_tree cases;

  /* The bindings at the point of the switch.  This is used for
     warnings crossing decls when branching to a case label.  */
  struct c_spot_bindings *bindings;

  /* Whether the switch includes any break statements.  */
  bool break_stmt_seen_p;

  /* The next node on the stack.  */
  struct c_switch *next;

  /* Remember whether the controlling expression had boolean type
     before integer promotions for the sake of -Wswitch-bool.  */
  bool bool_cond_p;
};

struct c_switch *c_switch_stack;

tree
c_start_case_2 (tree start, tree end)
{
  struct c_switch *cs = c_switch_stack;
  // TODO cs->start = start;
  // TODO cs->end = end;
}

extern tree cs_start, cs_end;
extern tree
do_case (location_t loc, tree low_value, tree high_value, tree attrs);

tree
build_inrange ()
{
  struct c_switch *cs = c_switch_stack;
  return 0; // TODO do_case (input_location, cs->start, cs->end, 0);
}

tree
build_outrange ()
{
  struct c_switch *cs = c_switch_stack;
  // TODO tree mystart=fold(/*parser_*/build_binary_op(MINUS_EXPR,cs->start,build_int_cst (long_integer_type_node, 1),1)); // check -parser
  // TODO tree myend=fold(/*parser_*/build_binary_op(PLUS_EXPR,cs->end,build_int_cst (long_integer_type_node, 1),1)); // check -parser
  // TODO do_case (build_int_cst (long_integer_type_node, -0x7fffffff), mystart);
  return 0; // TODO do_case (input_location, myend, build_int_cst (long_integer_type_node, 0x7fffffff), 0);
}
