/* Here we -*-c-*- tell emacs to use c mode edit.  */ 
/* Template for autogen, suffix of output file is .y thence .c.  */


/* Autogen if suffix is .h.  */

  /* 

     BLISS Compiler preprocessor parser (bliplx2.tpl) for the copy and
     replace statments. bliplx2.h is a generated file; do not edit it.

     ---------------------------------------------------------------------

     Copyright (C) 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
 
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

     ---------------------------------------------------------------------  */

 
entry(PREC_LESS_LPAREN, 301)  
entry(PREC_LESS_PLUS_MINUS, 302)  
entry(LPAREN, 303)  
entry(NEWLINE, 304)  
entry(BLANKS, 305)  
entry(SEMICOLON_SPACE, 306)  
entry(SEMICOLON_NOSPACE, 307)  
entry(COMMA_SPACE, 308)  
entry(COMMA_NOSPACE, 309)  
entry(COMMENT, 310)  
entry(NON_COMMENT, 311)  
entry(COMMENT_ENTRY_PARA, 312)  
entry(PERIOD_SPACE, 313)  
entry(PERIOD_NOSPACE, 314)  
entry(STRING, 315)  
entry(STRING_UNTERM, 316)  
entry(UMINUS, 317)  
entry(UPLUS, 318)  
entry(ASTER_ASTER, 319)  
entry(ASTER, 320)  
entry(SLASH, 321)  
entry(PLUS, 322)  
entry(MINUS, 323)  
entry(EQUAL, 324)  
entry(RPAREN, 325)  
entry(COLON, 326)  
entry(GE, 327)  
entry(LE, 328)  
entry(GT, 329)  
entry(LT, 330)  
entry(PSEUDOTEXTDELIM, 331)  
entry(NUMBER, 332)  
entry(NUMBERORNAMEORPIC, 333)  
entry(NUMBERORNAMEORPIC_IDENTIFIER_BASE, 334)  
entry(NUMBERORNAMEORPIC_REF_MOD_ONLY, 335)  
entry(NUMBERORNAMEORPIC_ARRAY_REF_MOD, 336)  
entry(ERROR_UNRECOGNISED_BLI_CHARACTER, 337)  
entry(AND_CHAR, 338)  
entry(COPY, 339)  
entry(OF, 340)  
entry(IN, 341)  
entry(BY, 342)  
entry(REPLACING, 343)  
entry(REPLACE, 344)  
entry(OFF, 345)  
entry(FUNCTION, 346)  
entry(EOF_symbol, 347)  
entry(YYMAXUTOK, 348) 


