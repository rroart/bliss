/*  

   BLISS Compiler common debugging settings (blidebug)

   Written by Tim Josling
   Changes Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
   
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
   what you give them.   Help stamp out software-hoarding!  */

/* Placeholders for incomplete parts of the compiler.  */


/* The FSF coding standard disallows assert.  But says nothing about
   BLI_ASSERT.  */

#include <stdlib.h>

#ifdef NDEBUG
#define BLI_ASSERT(x) 
#else
#define BLI_ASSERT(x) ((void)((!(x)) ? (myabort (#x, __FILE__, __LINE__), 0) : 1))
#endif

#define RSN(TOKEN, TEXT) rsn_error(TOKEN, (uchar*)TEXT)
#define RSN_ERRMSG myabort (); /* There should be an error message here.  */

void myabort (const char * const msg, const char * const file, int line);
