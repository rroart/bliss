/* 

   BLISS Compiler preprocessor lexical analysis (bliplex.h)
   Header file

   --------------------------------------------------------------------

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999,2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Converted for BLISS by Tim Josling
   
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
   
   --------------------------------------------------------------------.  */
/* 
   If you define INCLUDE_TOKENLITS it will include the token literal
   values for you in the tokenlits array, indexed by the token number.  */

#ifndef INCLUDED_BLIPLEX
#define INCLUDED_BLIPLEX

/* Externs.  */
extern int yyleng;
extern uint32 blipre_lineno;  /* Line # current token.  */
extern uint32 blipre_charno;  /* Char nbr current token.  */
extern uint32 blipre_debug_line;  /* Current token part of debug line.  */
extern char *yytext;

/* Function prototypes.  */
extern int getinput (char *buf, int max_size);
int32 yylex (void);

/* Tokens for preprocessor.  */

#define entry(a,b) a=b,

/* Token numbers.  */

enum token_enum{
  dummy0 = 0,
#include "bliplx2.h"
 dummy9
};

#ifdef INCLUDE_TOKENLITS
#undef entry

#define entry(a,b) (uchar*)#a,

/* Token names.  */

static const uchar *token_lits[] = {
#include "bliplx2.h"
};

#undef entry
#define entry(a,b) b,

/* (Reverse) lookup for token ID to index in literal table.  */
static uint32 token_lit_nbrs[] = {
#include "bliplx2.h"
};
#endif

#endif

void lexinit (int init_value, struct file_struct *file_ptr);

/* Init_value ...  */
#define LEXINIT_LEAVE_ALONE 0 /* not used */
#define LEXINIT_FILE 1 /* lexing a file */
#define LEXINIT_REDO_TOKENS 2 /* lexing to repaste tokens */
#define LEXINIT_INVALID 3 /* invalid to ensure someone sets it */







