/* Subroutines shared by all languages that are variants of C.
   Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "intl.h"
#include "tree.h"
#include "flags.h"
#include "output.h"
//#include "c-pragma.h"
#include "rtl.h"
#include "ggc.h"
//#include "varray.h"
#include "expr.h"
//#include "c-common.h"
#include "diagnostic.h"
#include "tm_p.h"
#include "obstack.h"
#include "cpplib.h"
#include "target.h"
#include "langhooks.h"
#include "tree-inline.h"
//#include "c-tree.h"
#include "toplev.h"
#include "tree-iterator.h"
#include "hashtab.h"
//#include "tree-mudflap.h"
#include "opts.h"
#include "real.h"
#include "cgraph.h"

// BLISS adds

/*
rtx
bli_expand_expr (exp, target, tmode, modifier, alt_rtl)
     tree exp;
     rtx target;
     enum machine_mode tmode;
     int modifier;
     rtx *alt_rtl;
{
  switch (TREE_CODE (exp))
    {
    case BIT_FIELD_REFS:
      {
	return expand_expr (TREE_OPERAND (exp, 0), target, tmode, modifier);
	break;
      }

    default:
		//abort();
      return c_expand_expr (exp, target, tmode, modifier, alt_rtl);
    }

  abort ();
  return NULL;
}
*/

int pending_lang_change;
