 /* 
   
   BLISS Compiler 
   Common header info for compiler

   Copyright (C) 1986, 87, 89, 92-96, 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   Written by Paul Rubin, June 1986
   Adapted to ANSI C, Richard Stallman, Jan 1987
   Adapted to BLISS by Tim Josling Mar 1999
   
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

#define KEYWORD_FLAG_STANDARD (1)
#define KEYWORD_FLAG_BLISS4   (2)
#define KEYWORD_FLAG_SUBSET   (4) /* minimal subset for use in writing compiler and runtime */

#include "bli_version.h"
#include <stdio.h>
#ifndef GTY
#define GTY(a)  
#endif

uchar **token_lit_ptrs;
uint32 max_token_nbr;

/* File names, copy file name hierarchy a<=n:b<=n:c, cmp_file_struct; all indexed by file number.  */
extern GTY((length ("2"))) struct bli_file_struct **file_structs_ptrs;

/* Details of one file being processed.  */

struct bli_file_struct GTY(())
{
  uint32 bli_file_length;
  uchar *bli_file_buffer;
  uchar *bli_file_name;
  uchar *bli_current_line_start;
  uchar *bli_current_position;
  uchar *bli_end_position;
};

struct bli_token_struct;

/* Allocate and init an entry in the parse tree.  */

#define BLI_ALLOC_TREE(where_to_store, type_value, structure)\
{\
 where_to_store  =  BLI_ALLOC (sizeof(struct structure));\
 ((struct bli_tree_noncons*)where_to_store)->pfx.type.prod_type =  type_value;\
 ((struct bli_tree_noncons*)where_to_store)->pfx.cat = type_production;\
}

struct token_flag_struct
{
  int token_nbr;
  uint32 token_flags;
};

#define TOKEN_FLAG_STD 1
#define TOKEN_FLAG_CORE 2
#define TOKEN_FLAG_IBM 4

extern uint32 token_mask;

/* Top of parse tree.  */
extern GTY(()) struct bli_tree_struct_parse_tree_top *parse_tree_top;

/* Token tables - names, flags, etc.  */
extern uchar ** GTY(()) token_lit_ptrs;
extern struct token_flag_struct * GTY(()) token_flags;

/*
 * Options.  */

/* See usage() in blicc1.c for meaning of options.  */

extern uint32 option_no_seq; 
extern uint32 option_parser_trace;
extern uint32 comment_offset;
extern uint32 area_a_offset;
extern uint32 area_b_offset;
extern uint32 option_lexer_trace;

int bliss_decode_option (int, char **);
const char *bliss_init (const char *); 

void
cmp_dump_file (uchar *filename, uint32 length, uchar *buffer);

void
cmp_dump_token (FILE *f, struct bli_token_struct *token);

void
cmp_dump_production (FILE *f, void *cmp_production, uint32 level);

struct bli_token_struct *
cmp_find_single_nested_token (void *cmp_prod_or_token);

struct token_struct *
cmp_find_single_nested_token_or_null (void *cmp_prod_or_token);

void
cmp_dump_token_maybe_with_lit (FILE *f, uchar *lit,
                               struct bli_token_struct *token);

void
cmp_print_token (FILE *file, uint32 type, void *value);

void
cmp_bligcmp_options (uint32 argc, uchar **argv, uchar **in_fname_ptr, 
                 char **out_fname_ptr);

int
cmp_getsev (const uchar *optarg, const uchar *option_name);

void
cmp_check_area_b (struct bli_token_struct *tk);

void
cmp_process_file (struct bli_file_struct *file_ptr,
                 void **parse_tree_ptr);

void
dump_token (FILE *f, const uchar *prefix, struct bli_token_struct *t);

void
parse_init (struct bli_token_struct *first_token, uint32 parser_trace_flag);

void bliss_parse_file (int);

void
bliss_init_decl_processing (void);

void
rsn_error (struct bli_token_struct *token, const uchar *text);

void
blicprs_add_gc_roots (void);

void
blicprs_del_gc_roots (void);

/* If option is on, issue error message.  */
#define EXCLUDED_BY_OPTION(FLAG, TOKEN, TEXT) \
if (token_mask & FLAG)\
{\
   blic_turned_off_error(TOKEN, (uchar*)TEXT);\
}\

void
blic_turned_off_error (struct bli_token_struct *token, const uchar *text);

void intern_string_init (void);

void intern_string_wrap (void); 

struct interned_token_string *
intern_string (uchar *text, uint32 length);

/* Reverse a linked list.  They are all built in reverse order to make
   the parse simpler, then reversed later.  */
#define REVERSE_LIST(header) reverse_list(header);
void reverse_list (void *header_address);

uint32 get_string_unsigned_value (uint32 length, uchar *string, 
                                                       uint32 *error_flag);
uint32 get_token_unsigned_value (struct bli_token_struct *tok, uint32 *error_flag);
uint64 get_string_unsigned_long_long_value (uint32 length, uchar *string, 
                                                        uint32 *error_flag);
int64 get_string_signed_long_long_value (uint32 length, uchar *string, 
                                             uint32 *error_flag);
uint64 get_string_nonsigned_long_long_value (uint32 length, uchar *string,
                                                                uint32 *error_flag);
uint32 get_token_nonsigned_value (struct bli_token_struct *tok, uint32 *error_flag);
uint32 get_string_nonsigned_value (uint32 length, uchar *string, 
                                              uint32 *error_flag);
int32 get_string_signed_value (uint32 length, uchar *string, 
                                          uint32 *error_flag);
uint32
get_type (void *item, void *token, void *data_item, uint32 *all_flag);
void
check_number_of_digits (uint32 digits, uint32 length, uchar *string,
                       struct bli_token_struct *value_tok);
void dump_syntax_tree (void *starting_point, uint32 level, uint32 next);
uint32 token_is_verb (uint32 token_type);
uint32 token_is_generic_name (uint32 token_type);

uint32 check_save (void * item);

