/* 

   BLISS Compiler main lexical analysis (bliclex)
   Header file

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
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
   what you give them.   Help stamp out software-hoarding!  */

/* Externs.  */

/* Length and address of token.  */
extern int yyleng;
extern char *yytext;

#define QUE "%BLS-F-QUAL-QUE"
#define NOQUAL "%BLS-W-QUAL-NOT-IMPLEMENTED-"
#define OK "%BLS-I-PARSED-OK-That's a module alright"
#define GEN "%BLS-I-GENERATING-CODE"
#define KOR "%BLS-I-RUN-K|r s} det ryker!"

//#define YYSTYPE void* 
/* Struct cmp_token_struct *
   or 
   (one of the production types in blisyt.h) *  */

/* Token location information.  */

struct posn
{
  char *filename; 
  int lineno; 
  int charno;
};

#define YYPOSN struct posn 

/* Function prototypes.  */
struct keyword *kw_lookup (const char *str, unsigned int len);
extern int getcompinput (char *buf, int max_size);
/* Real initial lex routine.  */
int z1zlex (void);
/* Pseudo lexical analysis routine to return tokens to parser.  */
int yylex (void);
void yylexinit (unsigned char *filename, unsigned char *buffer, unsigned length);
void dump_lex_return (int lexret);
struct bli_token_struct *initial_lexical_analysis (unsigned int *token_count);







