/* Definitions for specs for BLISS

   The format of the specs file is docuemnted in gcc.c

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

/* 
   This is the contribution to the `default_compilers' array in gcc.c for
   bliss.  
   
   This file must compile with 'traditional', so no ANSII string continuations 
   
*/

{".bli", "@bliss", NULL},
{".BLI", "@bliss", NULL},
{"@bliss",
    "blipre \
       %{C}\
       %{I*}\
       %{P}\
       %{$}\
       %{ansi}\
       %{v}\
       %{H}\
       %{--help:--help}\
       %{W*}\
       %{w}\
       %{pedantic*}\
       %{d*}\
       %{!E:-o %g.blii}\
       %{E:%W{o*}}\
       -i %i\n\
\
       %{!E:bli1\
       %{!Q:-quiet}\
       %{d*}\
       %{m*}\
       %{a}\
       %{g*}\
       %{O*}\
       %{W*}\
       %{w}\
       %{--help:--help}\
       %{pedantic*}\
       %{ansi}\
       %{v}\
       %{pg:-p}\
       %{p}\
       %{f*}\
       %{pg|p:%{fomit-frame-pointer:%e-pg or -p and -fomit-frame-pointer are incompatible}}\
       %{S:%W{o*}%{!o*:-o %b.s}}\
       %{!S:-o %g.s}\
       %g.blii\n\
\
       %{!S:as %a\
       %Y\
       %{c:%W{o*}%{!o*:-o %w%b%O}}\
       %{!c:-o %d%w%u%O}\
       %g.s\
       %A\n}\
       }", NULL
},
{".blii", "@bliss-preprocessed", NULL},
{".BLII", "@bliss-preprocessed", NULL},
{"@bliss-preprocessed",
    " %{!E:bli1\
       %{!Q:-quiet}\
       %{d*}\
       %{m*}\
       %{a}\
       %{g*}\
       %{O*}\
       %{W*}\
       %{w}\
       %{pedantic*}\
       %{ansi}\
       %{v}\
       %{--help:--help}\
       %{pg:-p}\
       %{p}\
       %{f*}\
       %{pg|p:%{fomit-frame-pointer:%e-pg or -p and -fomit-frame-pointer are incompatible}}\
       %{S:%W{o*}%{!o*:-o %b.s}}\
       %{!S:-o %g.s}\
       %g.blii\n\
       %{!S:as %a\
       %Y\
       %{c:%W{o*}%{!o*:-o %w%b%O}}\
       %{!c:-o %d%w%u%O}\
       %g.s\
       %A\n}\
       }", NULL
},

