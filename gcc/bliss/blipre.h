/* 
   
   BLISS Compiler preprocessor phase 1 (blipre)
   Common header info for preprocessor

   ---------------------------------------------------------------------------

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
   what you give them.   Help stamp out software-hoarding!  
   
   ---------------------------------------------------------------------------.  */

/* The standard limits the word length.  */

#define MAX_BLISS_WORD_LENGTH 30

/* Details of one source line source for spliced line.  */

struct line_details_struct
{
  uint32 line_details_lineno;
  uint32 line_details_charno;
  uint32 line_details_offset;
};

typedef struct line_details_struct line_details;

struct GTY(()) dynarray_line_details_struct
{
  struct dynarray_struct dynarray_details;
  line_details * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_line_details_struct dynarray_line_details;

/* Struct for line being from multiple line #s.  */

struct lineno_struct
{
  uint32 lineno_count;
  dynarray_line_details *lineno_line_details;
};

/* Details of source line.  */

struct line_struct
{
  uchar *line_start_orig;  /* As read in.  */
  uchar *line_end_orig;  /* Last char +1.  */
  /* As modified to 
     - fold continuation lines
     - add extra space at end of line if not continuation, if needed
     - remove sequence numbers (col1-6).  */
  uchar *line_start_new;  /* After sanitise has done with it
				   (char after indicator).  */
  uchar *line_end_new;  /* Last char +1.  */
  /* Line numbers that make up this line if more than one line
     or NULL.  */
  struct lineno_struct *line_lineno_ptr;

  uint32 line_continuation_line_nbr;  /* Line # that continues this one.  */
  uint32 line_continued_line_nbr;  /* Line # that this one continues.  */
  uchar line_ind_char;
  uchar line_string_char;
  unsigned line_flag_indicator : 1;  /* This line has indicator eg debug/comment.  */
  unsigned line_flag_comment : 1;  /* This line is a comment.  */
  unsigned line_flag_blank : 1;  /* This line is blank 
				   (including indicator field but ignoring sequence field).  */
  unsigned line_flag_continued : 1;  /* This line is continued by another line.  */
  unsigned line_flag_continuation : 1;  /* This line is a continuation 
					   of another line.  */
  unsigned line_flag_partstring : 1;  /* This line ends in a partial string.  */
  unsigned line_flag_termstring : 1;  /* This line ends in a terminated string.  */
  unsigned line_flag_needs_delimiter : 1;  /* Line not properly terminated 
					   - must add space at end.  */
  unsigned line_flag_wrap_string : 1;  /* Need to close out string.  */
  unsigned line_flag_ignore : 1;  /* Ignore for some reason.  */
};

typedef struct line_struct line_t;

struct GTY(()) dynarray_line_struct
{
  struct dynarray_struct dynarray_details;
  line_t * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_line_struct dynarray_line;

/* Replacing a by b part of copy or replace verb; may be repeated several times.  */

struct replacing_struct
{

  struct token_struct *replacing_from_first_token_ptr;
  struct token_struct *replacing_from_last_token_ptr;
  struct token_struct *replacing_to_first_token_ptr;
  struct token_struct *replacing_to_last_token_ptr;
  unsigned replacing_from_pseudo_text_flag : 1;
  unsigned replacing_to_pseudo_text_flag : 1;
};

typedef struct replacing_struct replacing;

struct GTY(()) dynarray_replacing_struct
{
  struct dynarray_struct dynarray_details;
  replacing * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_replacing_struct dynarray_replacing;

/* Encoded copy or replace verb.  */

struct copyrepl_struct
{

  struct token_struct *copyrepl_copyrepl_token_ptr;
  struct token_struct *copyrepl_fullstop_token_ptr;  /* TOK_EOF token if none.  */

  struct token_struct *copyrepl_filename_token_ptr;
  struct token_struct *copyrepl_library_token_ptr;

  struct file_struct *copyrepl_copied_file_ptr;

  uint32 copyrepl_replacing_count;
  uint32 copyrepl_replacing_allocated;
  dynarray_replacing *copyrepl_replacing;
  unsigned copyrepl_is_copy_flag : 1;  /* 0 If replace  stmt.  */
  unsigned copyrepl_valid_flag : 1;
  unsigned copyrepl_replace_off_flag : 1;
};

typedef struct copyrepl_struct copyrepl;

struct GTY(()) dynarray_copyrepl_struct
{
  struct dynarray_struct dynarray_details;
  copyrepl * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_copyrepl_struct dynarray_copyrepl;

/* Details of one file being processed.  */

struct file_struct
{
  uint32 file_number;
  uint32 file_length;
  uchar *file_buffer;
  dynarray_line *file_lines;
  uint32 file_line_count;
  uchar *file_name;
  uchar *file_short_name;
  struct token_struct *file_first_token;
  struct token_struct *file_last_token;
  uint32 file_token_count;
  dynarray_copyrepl *file_copyrepl_array;
  uint32 file_copyrepl_array_count;
  struct token_struct *file_copy_token_ptr;
};

typedef struct file_struct *file_ptr;

struct GTY(()) dynarray_file_ptr_struct
{
  struct dynarray_struct dynarray_details;
  file_ptr * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_file_ptr_struct dynarray_file_ptr;

#define BLIPRE_GET_LINENO (token) 

/* Structure for token in preprocessor.  */

struct token_struct
{
  uint16 token_type;  /* TOK_...  out of parser blippr1.y.  */
  uint32 token_lineno;
  uint32 token_charno;
  struct file_struct *token_file; 

  uchar *token_chars; 
  uchar *token_chars_upper;  /* Upper case version.  */
  uint32 token_length;

  struct token_struct *token_next;
  struct token_struct *token_prev;

  unsigned token_flag_debug_line : 1;
  unsigned token_flag_replacement : 1;
  unsigned token_flag_recent_replacement : 1;  /* Replaced in current operation.  */
  unsigned token_flag_from_pseudo_text : 1;
};

typedef struct token_struct token;

struct GTY(()) dynarray_token_struct
{
  struct dynarray_struct dynarray_details;
  token * GTY ((length ("%1.dynarray_details.entry_count"))) elem;
};

typedef struct dynarray_token_struct dynarray_token;

/* Basic element of parse tree.  */

struct production
{
  uint32 production_ID;  /* See enum below.  */
  uint32 production_sub_production_count;  /* Count of elements.  */
  /* -> Array of -> productions or tokens.  */
  struct production **production_sub_productions;
};

#ifdef PRODUCTION_TOKEN
#undef PRODUCTION_TOKEN
#endif

#define PRODUCTION_TOKEN(a) a,
enum
{
  PR_dummy_start = 1024,  /* Avoid clash with token IDs.  */
#include "blippr1b.h"
  PR_dummy_end
};

#ifdef  INCLUDE_PRODUCTIONLITS
#ifdef PRODUCTION_TOKEN
#undef PRODUCTION_TOKEN
#endif

#define PRODUCTION_TOKEN(a) (uchar *)#a,

static const uchar *production_lits[] = {
#include "blippr1b.h"
};
#endif


/* Scan the tokens and parse the copy or replace verb.  */

uint32 blippr1 ( /* Input parms.  */
                       struct token_struct *first_token, 
                       uint32 copy_flag,  /* 1=Copy 0=replace verb.  */
                       /* Output parms.  */
                       dynarray_copyrepl **copy_array,
                       uint32 *copy_array_count);

/*
 * Options.  */

/* See usage() in blipre.c for meaning of options.  */

extern uint32 option_fix_eol;
extern uint32 option_no_seq;
extern uint32 option_parser_trace;
extern uint32 option_R_margin;
extern uint32 comment_offset;
extern uint32 area_a_offset;
extern uint32 area_b_offset;

/* Utility routines.  */

void dump_file (uchar *filename, uint32 length, uchar *buffer);

void dump_lines (uchar *filename, uint32 line_count, dynarray_line *lines);

void dump_line (uchar *filename, uint32 line_nbr, struct line_struct *line);

void
dump_tokens (FILE *f, uchar *filename,
                    struct token_struct *first_token, uint32 token_count);

void dump_token (FILE *f, struct token_struct *token);

void
dump_copyrepl (FILE *f, uchar *filename,
                      dynarray_copyrepl *copyrepl_array,
                      uint32 copyrepl_array_count);

void resolve_token_string_ptrs (void);

void dump_production (FILE *f, void *production, uint32 level);

struct token_struct *find_single_nested_token (void *prod_or_token);

struct token_struct *find_single_nested_token_or_null (void *prod_or_token);

void
get_replacing_from_to_details (struct production *prod, uint32 *delimited_flag,
                                      struct token_struct **first_token,
                                      struct token_struct **last_token);
void
scan_production (struct production *prod, uint32 *delimited_flag,
                        struct token_struct **first_token,
                        struct token_struct **last_token);

void allocate_copyrepl_struct (void);

void allocate_replacing_struct (void);

void
dump_token_maybe_with_lit (FILE *f, const uchar *lit, struct token_struct *token);

void
fill_out_replacing_details (struct copyrepl_struct *copyrepl_details,
                                   void *prod);

uint32 validate_number (struct token_struct *token);

uint32 validate_integer (struct token_struct *token);

uint32 validate_bliss_word (struct token_struct *token);

uint32
validate_bliss_word_strict (struct token_struct *token);

void ensure_token_upper (struct token_struct *token);

void print_token (FILE *file, uint32 type, void *value);

void process_file (struct file_struct *file_ptr);

void
blipre_options (uint32 argc, uchar **argv, uchar **in_fname_ptr,
                       uchar **out_fname_ptr);

void process_copies (struct file_struct *file_ptr);

void
get_string_value (uchar **sanitised_value,
                         uchar **sanitised_value_upper,
                         uint32 *sanitised_length, struct token_struct *token_ptr);

#define SKIP_LEADING_WHITESPACE 1
#define DONT_SKIP_LEADING_WHITESPACE 0
void
process_replaces (struct copyrepl_struct *copyrepl_ptr,
                         struct token_struct *start_token,
                         struct token_struct *end_token,
                         uint32 skip_leading_whitespace_flag);
void process_replace (struct file_struct *file_ptr);

void output_resulting_file (struct file_struct *file_ptr, uchar *out_fname);

void
preprocess_copy_identifiers (struct token_struct *first_token,
                                    struct token_struct *last_token);

uint32
scan_for_type (uint32 type_to_find, uint32 show_stopper_count,
                      uint32 *type_show_stopper_array,
                      struct token_struct **current_token_ptr);

void fix_copy_statement (struct token_struct **current_token_ptr);

void grok_identifier_type (struct token_struct **current_token_ptr);

void
blipre1_fix_lines (void *buffer, uint32 length, uchar *filename,
                   uint32 file_nbr,
                   dynarray_line **lines_ptr,
                   uint32 *line_count, struct file_struct *file_ptr);

extern void blipre1_tokenize_file (struct file_struct *file_ptr);

/* Values for token classes, mainly for ignoring things.  */

#define IGNORE_TOKENS_BLANK 1
#define IGNORE_TOKENS_COMMENT 2
#define IGNORE_TOKENS_NOISE 3


void
paste_token (dynarray_uchar *current_line_ptr, uint32 *current_line_used_ptr,
             uint32 *current_line_used_count_ptr,
             struct token_struct *tk1);

void
paste_text (dynarray_uchar *current_line_ptr, uint32 *current_line_used_ptr,
            uint32 *current_line_used_count_ptr,
            const uchar *s, uint32 l);
void
flush_line (FILE *f,
            dynarray_uchar *current_line_ptr, uint32 *current_line_used,
            uint32 *current_line_used_count_ptr,
            dynarray_uchar *current_debug_info, uint32 *current_debug_info_used,
            uint32 *blanks_passed);

void
init_line (dynarray_uchar *current_line_ptr, uint32 *current_line_used,
                  uint32 *current_line_used_count_ptr,
                  dynarray_uchar *current_debug_info_ptr,
                  uint32 *current_debug_info_length_ptr,
                  uint32 *current_debug_info_used_ptr,
                  uint32 *blanks_passed, struct token_struct *tk1);

void redo_tokenising (struct file_struct *file_ptr);

extern uint32 getinput_repaste (uchar *buf, uint32 max_size);

uint32 getsev (const uchar *optarg, const uchar *option_name);

void check_area_b (struct token_struct *tk);

void init_ignore_tokens (void);

void blipre1_R_margin (struct file_struct *file_ptr);

void print_file_hierarchy (struct file_struct *file_ptr, FILE *printfile);
