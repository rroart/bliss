/* Definitions for switches for BLISS.

   Copyright (C) 1995, 96-98, 1999, 2000, 2001 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

DEFINE_LANG_NAME ("bliss.cp")
     
/* This is the contribution to the `lang_options' array in gcc.c for
   bliss.  */
/* -FnameXX...X is passed to bli1 to make top more useful when running
   lots of tests - see where you are up to.  Don't clutter doco with
   this.  */
 {"--help", "help"},
 {"-v", "verbose/version"},
 {"-fimmediate-msg-output", "output msgs when detected, rather than output sorted msgs at end"},
 {"-fmain=name", "\'name\' is the main program"},
 {"-fparser-trace", "(debug) trace parsing process"},
 {"-flexer-trace", "(debug) trace lexical analysis"},
 {"-flexer-only", "(debug) only run the lexical abalysis"},
 {"-fseq", "file has sequence numbers (default)"},
 {"-fnoseq", "file has no sequence numbers"},
 {"-fcompiler-subset", "allow bliss compiler internal use subset and its extensions only"},
 {"-pedantic", "warning and error on non-standard features"},
 {"-pedantic-errors", "non standard features produce error not justwarning"},
 {"-w", "no warnings"},
 {"-Wall", "all warnings"},
