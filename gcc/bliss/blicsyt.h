/*  

     BLISS Compiler parser symbol table header (blicsyt.h)
     Copyright (C) 1999, 2000, 2001 Free Software Foudnation, Inc.

     ---------------------------------------------------------------------------

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

#ifndef BLICSTY_H
#define BLICSTY_H

/* Parse tree node types.  */

enum bli_tok_type
{
  BLI_VERY_EMPTY = 0
};

typedef enum bli_prod_type {
  integer_literal,
  addressing_mode_attribute,
  weak_attribute,
  unlabeled_block,
  string_type,
  plit2,
  p_chesc,
  p_char,
  p_string,
  case_label_list,
  mode_16,
  mode_32,
  list_option,
  access_part,
  string_par_list,
  common_switch,
  bliss_16_switch,
  bliss_32_switch,
  bliss_36_switch,
  environment,
  addr_external,
  addr_non_external,
  sw_addressing_mode,
  environ_16_option,
  addressing_mode,
  string_literal,
  char_par_list,
  lexical_function,
  own_attribute_list,
  own_attribute,
  case_label,
  initial_group,
  pre_tested_loop,
  post_tested_loop,
  leave_expression,
  plit,
  plit3,
  plit_group,
  conditional_expression,
  conditional_expression2,
  alloc_actual_list,
  tname_list2,
  op_exp,
  opexp,
  opexp1,
  opexp2,
  opexp3,
  opexp4,
  opexp5,
  opexp6,
  opexp7,
  io_actual_parameter_list,
  map_declaration_attribute_list,
  external_declaration,
  io_actual_parameter,
  opexp8,
  opexp9,
  opexp10,
  opexp11,
  pot_expression,
  opexp12,
  attached_label_list,
  io_list3,
  ordinary_routine_call,
  infix_expression,
  general_routine_call,
  attribute_list,
  codecomment,
  decimal_literal,
  digits,
  field_stuff_list,
  forward_item,
  global_reg_item_list,
  initial_attribute,
  initial_item_list,
  integer_digit_list,
  labeled_block,
  local_item,
  map_item,
  plit_item_list,
  preset_attribute,
  psect_allocation,
  quoted_string_list,
  segment_expression,
  field_attribute,
  register_item,
  external_reg_item,
  global_reg_item,
  preset_item,
  decl_list,
  ordinary_routine_declaration,
  external_routine_declaration,
  external_routine_item_list,
  routine_definition_list,
  formal_item_list,
  module_head,
  own_declaration_list,
  own_item_list,
  own_declaration,
  block_action_list,
  maybe_block_action_list,
  block_action,
  alignment_attribute,
  range_attribute,
  structure_attribute,
  allocation_unit,
  extension_attribute,
  language_name_list,
  list_option_list,
  ident,
  language,
  linkage,
  list,
  structure,
  b_main,
  optlevel,
  prversion,
  title,
  linkage_definition_list,
  linkage_definition,
  input_parameter_location_list,
  input_parameter_location,
  output_parameter_location_list,
  output_parameter_location,
  tname_list,
  bind_data_attribute,
  psect_item_list,
  psect_item,
  storage_class,
  psect_attribute_list,
  psect_attribute,
  b32_psect_attribute,
  bind_data_attribute_list,
  bind_data_declaration,
  bind_data_item,
  bind_data_item_list,
  bind_routine_attribute,
  bind_routine_attribute_list,
  bind_routine_declaration,
  bind_routine_item,
  bind_routine_item_list,
  iterative_macro_definition2,
  keyword_macro_definition,
  key_macro_definition,
  simple_macro_definition,
  conditional_macro_definition,
  external_literal_declaration,
  external_literal_item,
  external_literal_item_list,
  literal_attribute,
  key_macro_declaration,
  positional_macro_declaration,
  positional_macro_definition_list,
  positional_macro_definition,
  iterative_macro_definition,
  keyword_macro_definition_list,
  macro_body,
  default_actual,
  keyword_macro_declaration,
  keyword_pair,
  keyword_pair_list,
  lexeme_list,
  positional_macro_definiton,
  positional_macro_definiton_list,
  literal_attribute_list,
  literal_declaration,
  literal_item,
  literal_item_list,
  language_name,
  volatile_attribute,
  novalue_attribute,
  linkage_attribute,
  block_body,
  block_body2,
  assign_expression,
  own_item,
  module,
  return_expression,
  global_routine_definiton,
  mode_spec_list,
  cond_handling_function_name,
  field_declaration,
  declaration_list,
  enable_declaration,
  bound_declaration,
  compiletime_declaration,
  macro_declaration,
  require_declaration,
  library_declaration,
  psect_declaration,
  switches_declaration,
  label_declaration,
  builtin_declaration,
  undeclare_declaration,
  linkage_declaration,
  global_routine_declaration,
  forward_routine_declaration,
  routine_declaration,
  routine_definition,
  io_list,
  io_list2,
  formal_item,
  routine_attribute_list,
  formal_attribute_list,
  routine_attribute,
  map_declaration_attribute,
  global_routine_definition_list,
  access_actual_list,
  select_line_list,
  executable_function,
  case_line_list,
  select_label_list,
  actual_parameter_list,
  access_formal_list,
  allocation_formal_list,
  global_declaration,
  global_item_list,
  external_item_list,
  forward_declaration,
  forward_item_list,
  local_declaration,
  local_item_list,
  stackglocal_declaration,
  register_declaration,
  register_item_list,
  global_register_declaration,
  external_register_declaration,
  external_reg_item_list,
  map_declaration,
  map_item_list,
  structure_declaration,
  structure_definition_list,
  structure_definition,
  allocation_formal,
  fetch_expression,
  default_structure_reference,
  global_routine_definition,
  global_routine_attribute,
  forward_routine_attribute,
  external_routine_attribute,
  global_io_list2,
  opt_mhargs,
  ms_list,
  module_body,
  on_off_switch,
  special_switch,
  name_list,
  tname,
  tvalue,
  global_routine_attribute_list,
  forward_routine_item_list,
  forward_routine_attribute_list,
  ext_routine_attribute_list,
  external_item,
  global_item,
  initial_item,
  plit_item,
  forward_routine_item,
  external_routine_item,
  ext_routine_attribute,
  external_routine_attribute_list,
  ext_routine_item_list,
  ext_routine_item,
  gdummy
} node_Attr_t;

enum bli_prod_type_not
{
  BLI_PROD_TYPE_PARSE_TREE_TOP = 0,
  BLI_PROD_TYPE_PROGRAM_TOP = 1,
  BLI_PROD_TYPE_IDENTIFICATION_DIVISION,
  BLI_PROD_TYPE_ENVIRONMENT_DIVISION,
  BLI_PROD_TYPE_DATA_DIVISION,
  BLI_PROD_TYPE_PROCEDURE_DIVISION,  /* 5.  */
  BLI_PROD_TYPE_PROGRAM_ID,
  BLI_PROD_TYPE_STORAGE,
  BLI_PROD_TYPE_DATA_ITEM,
  BLI_PROD_TYPE_USAGE,
  BLI_PROD_TYPE_PIC,  /* 10.  */
  BLI_PROD_TYPE_REDEFINES,
  BLI_PROD_TYPE_EXTERNAL,
  BLI_PROD_TYPE_VALUE_ITEM,
  BLI_PROD_TYPE_OCCURS_CLAUSE,
  BLI_PROD_TYPE_ITEM_LIST,  /* 15.  */
  BLI_PROD_TYPE_VERB_DISPLAY,
  BLI_PROD_TYPE_VERB_STOP,
  BLI_PROD_TYPE_UNUSED18,
  BLI_PROD_TYPE_EXPRESSION,
  BLI_PROD_TYPE_VERB_SET,  /* 20.  */
  BLI_PROD_TYPE_SECTION,  /* Uses bli_struct_label structure.  */
  BLI_PROD_TYPE_PARAGRAPH,  /* Uses bli_struct_label structure.  */
  BLI_PROD_TYPE_PROCEDURE_REF,
  BLI_PROD_TYPE_VERB_GOTO,
  BLI_PROD_TYPE_VERB_COMPUTE,  /* 25.  */
  BLI_PROD_TYPE_VERB_MOVE,
  BLI_PROD_TYPE_ITEM_DOUBLE_ENTRY_LIST,
  BLI_PROD_TYPE_VERB_CONTINUE,
  BLI_PROD_TYPE_VERB_EXIT,
  BLI_PROD_TYPE_VERB_FULLSTOP,  /* 30.  */
  BLI_PROD_TYPE_VERB_IF,
  BLI_PROD_TYPE_END_PROGRAM,
  BLI_PROD_TYPE_VERB_NEXT_SENTENCE,
  BLI_PROD_TYPE_SAVE_DETAILS,
  BLI_PROD_TYPE_VALUE_CLAUSE, /* 35.  */
  BLI_PROD_TYPE_VERB_PERFORM, 
  BLI_PROD_TYPE_LABELS, 
  BLI_PROD_TYPE_PERFORM_VARYING, 
  BLI_PROD_TYPE_VERB_CALL, 
  
  BLI_PROD_TYPE_TOO_HIGH
};

/* Parse tree data item value types.  */

enum VALUE_TYPE
{
  VALUE_TYPE_ABSENT = 0,
  VALUE_TYPE_STRING = 1,
  VALUE_TYPE_SIGNED = 2,
  VALUE_TYPE_LONG_LONG_SIGNED = 3,
  VALUE_TYPE_UNSIGNED = 4,
  VALUE_TYPE_LONG_LONG_UNSIGNED = 5,
  VALUE_TYPE_POINTER = 6
};

struct bli_token_struct;
struct bli_file_struct;
typedef struct bli_token_struct bli_token;
union bli_item_union;
/* Token or language item.  */
typedef union bli_item_union bli_item;

struct bli_tree_struct_parse_tree_top;
struct bli_tree_struct_program_top;
struct bli_tree_struct_identification_division;
struct bli_tree_struct_environment_division;
struct bli_tree_struct_data_division;
struct bli_tree_struct_procedure_division;
struct bli_tree_struct_program_id;
struct bli_tree_struct_storage;
struct bli_tree_struct_data_item;
struct bli_tree_struct_usage;
struct bli_tree_struct_pic;
struct bli_tree_struct_redefines;
struct bli_tree_struct_external;
struct bli_tree_struct_value_item;
struct bli_tree_struct_occurs_clause;
struct bli_tree_struct_item_list;
struct bli_tree_struct_verb_display;         
struct bli_tree_struct_verb_stop;      
struct bli_tree_struct_expression;     
struct bli_tree_struct_verb_set;       
struct bli_tree_struct_section;        
struct bli_tree_struct_paragraph;      
struct bli_tree_struct_procedure_ref;  
struct bli_tree_struct_verb_goto;      
struct bli_tree_struct_verb_compute;   
struct bli_tree_struct_verb_move;      
struct bli_tree_struct_double_entry_list;
struct bli_tree_struct_verb_continue;  
struct bli_tree_struct_verb_exit;      
struct bli_tree_struct_verb_fullstop;  
struct bli_tree_struct_verb_if;        
struct bli_tree_struct_end_program;    
struct bli_tree_struct_verb_next_sentence;
struct bli_tree_struct_save_details;   
struct bli_tree_struct_value_clause;   
struct bli_tree_struct_verb_perform;   
struct bli_tree_struct_labels;         
struct bli_tree_struct_perform_varying;
struct bli_tree_struct_verb_call;    

/* 
   1. Mini-symbol table to support the lexer hacks to get around bliss not being lalr(1).  */

/* Notes: 

   - the symbol table will need to be revamped to support nested programs 
   - all symbol names must be upper case when stored 
   - char * provided on entry is used as name, so must be permanent.  */
/* Number of levels allowed in data structure.  */
#define LEVELS_ARRAY_SIZE 50

/* Mini symbol table (for grammar hacks) entry types.  */

enum symtab_entry_type_enum
{
  symtab_entry_type_not = 0, 
  symtab_entry_type_condition = 1,  /* 88 Level.  */
  symtab_entry_type_data,  /* Data item 01-49,66,77.  */
  symtab_entry_type_switch_name,  /* Special names switch name.  */
  symtab_entry_type_function_name,  /* Name of a function.  */
  symtab_entry_type_class_condition,  /* Special names character class condition.  */
  symtab_entry_type_symbolic_character_name,  /* Special names symbolic character name.  */
  symtab_entry_type_label /* Paragraph or section.  */
};

/* Internal symbol table entry format.  */
struct symtab_entry GTY(())
{
  uint32 magic;  /* Make sure it really is symtab entry.  */
  enum symtab_entry_type_enum symtab_entry_type;
  bli_item *symtab_entry_item;  /* Token or bli_data_item or NULL if neither.  */
  struct symtab_entry *symtab_entry_owner;  /* Hierarchical owner.  */
  uchar *symtab_entry_name;  /* Name; not protected from ggc.  */
  uint32 symtab_entry_name_length;  /* Name length; not protected from ggc.  */
  uint32 symtab_program_count;
  uint32 symtab_entry_flags;
  struct symtab_entry *symtab_hash_next;
};

/* Mini symbol table management functions.  */

void create_symbol_table (uint32 token_count);
void symbol_table_new_pgm (void);
void *create_symtab_entry (enum symtab_entry_type_enum type, void *item);
void put_symtab_entry_attribute_flags (void *entry, uint32 flags,
                                              uint32 flags_mask);
uint32 get_symtab_entry_attribute_flags (void *entry, uint32 flags_mask);
uchar *get_symtab_entry_name (void *entry,  /* Returns.  */uint32 *length);
void *get_symtab_entry_owner (void *entry);
enum symtab_entry_type_enum get_symtab_entry_type (void *entry);
void set_symtab_entry_owner (void *entry, void *owner);
void set_symtab_entry_item (void *entry, void *item);
void *find_symtab_entry (uchar *name, uint32 name_length);
void *find_next_symtab_entry (void *entry, uchar *name, uint32 name_length);
uint32 find_symtab_entry_hierarchy (uint32 nbr_names, uchar **names, 
                                               uint32 *name_lengths,  /* Return.  */void **entry,
                                               enum symtab_entry_type_enum type);
uint32 find_next_symtab_entry_hierarchy (uint32 nbr_names, 
                                             uchar **names, uint32 *name_lengths,
                                             /* Input and return.  */void **entry, 
                                             enum symtab_entry_type_enum type);

struct bli_tree_struct_data_item;  /* Struct forward refs to get us by for now.  */
struct bli_tree_struct_data_division;
struct bli_tree_struct_procedure_division;


uint32 data_item_top_level (struct bli_tree_struct_data_item *var);
void *get_name_symtab_entry (bli_token *tk, uint32 *status,
                             enum symtab_entry_type_enum type); 
void *get_next_name_symtab_entry (void *current, bli_token *tk, uint32 *status,
                             enum symtab_entry_type_enum type); 
void blicsyt_mark_symbol_table (void *m);
void cleanup_symbol_table (void);
void *get_symtab_entry_item (void *entry);
void validate_data_division (struct bli_tree_struct_data_division *current_data_division);
void validate_parse_tree_descriptor_array (void);
void validate_procedure_division (struct bli_tree_struct_procedure_division *current_procedure_division);
uint32 check_symtab_entry_namespace (bli_token *token, enum symtab_entry_type_enum type);


/* Mask for occurs flag for data item.  */
#define SYMBOL_TABLE_FLAG_OCCURS  ((uint32)0x80000000u)
/* Global - keep across pgms.  */
#define SYMBOL_TABLE_FLAG_GLOBAL  ((uint32)0x00000001u)

/* Parse tree management functions.  */

void blic_bei (struct bli_tree_struct_parse_tree_top *parse_tree_top);

/*
  2. Common symbols and structures.  */

#define VERIFY_TYPE(ITEM, CATEGORY, TYPE, ZERO_OK)\
 {\
   struct bli_tree_noncons *work;\
   work=(void*)ITEM;\
   BLI_ASSERT (work || ZERO_OK);\
   if (work)\
     {\
       BLI_ASSERT (work->pfx.cat == CATEGORY);\
       BLI_ASSERT(! TYPE || (work->pfx.type.tok_type == TYPE));\
     }\
 }

/* Keyword name to ID.  */
struct keyword
{
  char *keyword_name; 
  int keyword_id; 
};

/* Distinguish between tokens and productions in the parse tree.  */
enum type_token_or_production 
{
  type_token = 303,
  type_production = 606
};

/* Interned string.  */

struct interned_token_string GTY(())
{
  uint32 length;
  uchar *string;
  struct interned_token_string *string_upper;
};

/* Prefix for token/language constructs.  */

union bli_item_code_union GTY ((desc ("(((union bli_item_code_union *)&%1).cat)"))) 
{
  enum bli_prod_type GTY ((tag ("type_token)"))) prod_type;
  enum bli_tok_type GTY ((tag ("type_production)"))) tok_type;
};

typedef union bli_item_code_union bli_item_code;

struct item_prefix_struct GTY(())
{
  enum type_token_or_production cat;
  union bli_item_code_union GTY ((desc ("%1.cat"))) type;
};

typedef struct item_prefix_struct item_prefix;

/* Structure for token in compiler.  */

typedef struct bli_token_struct *ignodeptr;

struct bli_token_struct GTY(())
{
  item_prefix pfx;
  struct interned_token_string *string_details;
  bli_token *bli_token_next;
  bli_token *bli_token_prev;
  uint32 bli_token_debug: 1;
  uint32 bli_token_fileno; 
  uint32 bli_token_lineno; 
  uint32 bli_token_charno;

  int type_int;
  char     *type_str;
  ignodeptr  type_node_p;

  int type;
  char name[80];
  int linenumb;
  ignodeptr next;
  ignodeptr left;
  ignodeptr middle;
  int value;
  char *id;

};

typedef struct bli_token_struct ignode;
ignodeptr igroot;

/* 
   3. Parse tree and symbol table proper.  */

/* Noncons starts a parse tree structure that has no 'pointer to next'
   or 'pointer to child'.  It is used in the parse tree entries proper,
   later on.  */

struct bli_tree_noncons GTY(())
{
  item_prefix pfx;
};

/* Cons starts a parse tree structure that has 'pointer to next'
   but no 'pointer to child'.  It is used in the parse tree entries proper,
   later on.  */

struct bli_tree_cons GTY(())
{
  item_prefix pfx;
  bli_item *next;
};

/* Branch starts a parse tree structure that has 'pointer to next' and
   'pointer to child'.  The pointer to next is not used in some cases;
   but if you want a pointer to child you have to have a pointer to
   next. It is used in the parse tree entries proper, later on.  */

struct bli_tree_branch GTY(())
{
  item_prefix pfx;
  bli_item *next;
  bli_item *child;
};

/* Parse tree entries proper.  */

/* Root of the parse tree.  */

struct bli_tree_struct_parse_tree_top GTY(())
{
  struct bli_tree_branch branch;  /* Child is the first program in a linked list.  */
};

/* Top of a program.  */
struct bli_tree_struct_program_top GTY(())
{
  struct bli_tree_cons cons;  /* Next is next program.  */
  bli_token *reference_token;  /* First token.  */
  struct bli_tree_struct_identification_division *identification_division;  /* ID division.  */
  struct bli_tree_struct_environment_division *environment_division;  /* Environment division.  */
  struct bli_tree_struct_data_division *data_division;  /* Data division.  */
  struct bli_tree_struct_procedure_division *procedure_division;  /* Procedure division header.  */
  struct bli_tree_struct_end_program *end_program;  /* End of program details.  */
  tree bli_tree_struct_decl;
  unsigned program_is_main: 1; 
};

/* Generic parse tree for list of things.  */
struct bli_tree_struct_item_list GTY(())
{
  struct bli_tree_branch branch;
};

/* IDENTIFICATION DIVISION.  */
struct bli_tree_struct_identification_division GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  struct bli_tree_struct_program_id *program_name;  /* Program name details.  */
};

/* ENVIRONMENT DIVISION.  */
struct bli_tree_struct_environment_division GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
};

/* DATA DIVISION.  */
struct bli_tree_struct_data_division GTY(())
{
  struct bli_tree_branch branch; 
  bli_token *reference_token; 
  struct bli_tree_struct_occurs_clause *occurs_first;
};

/* PROCEDURE DIVISION.  */
struct bli_tree_struct_procedure_division GTY(())
{
  struct bli_tree_branch branch; 
  bli_token *reference_token; 
  struct bli_tree_struct_label *label_first;
  struct bli_tree_struct_verb_fullstop *end_of_sentence_first;
  int32 exit_label_count;
};

/* PROGRAM ID.  */
struct bli_tree_struct_program_id GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  bli_token *id_keyword;  /* Program-id token.  */
  bli_token *id;  /* The ID value token.  */
  /* X as "y"; this is the Y value - list of tokens (merged into a
     single item list.  */
  bli_token *as_value; 
  bli_token *language_option;
};

/* Contains details of WORKING STORAGE LOCAL STORAGE or LINKAGE.  */

struct bli_tree_struct_storage GTY(())
{
  struct bli_tree_branch branch;  /* Child is first top level data item.  */
  bli_token *reference_token; 
  struct bli_tree_struct_item_list *first_external;
  struct bli_tree_struct_item_list *last_external;
};

/* EXPRESSION.  */

struct bli_tree_struct_expression GTY(())
{
  struct bli_tree_cons cons;  /* Next at same level eg in array reference list.  */
  bli_token *reference_token; 
  /* The following two can be just the first token of a data reference until resolved by blicast.c.  */
  bli_item *operand1;  /* Operator 1 data item or expression.  */
  bli_item *operand2;  /* Operator 2 data item or expression.  */
  /* Operator token type or pseudo token type - 0 means just the expression in operand1.  */
  uint32 operator; 
  unsigned save_expr_needed: 1;  /* Item used more than once, save expression value.  */
};

/* SAVE_DETAILS.  */

struct bli_tree_struct_save_details GTY(())
{
  struct bli_tree_cons noncons;  
  tree save_length;
  tree save_address;
  tree save_value;
};

/* A DATA ITEM - 01 77 88 77 66 nn level item.  */

struct bli_tree_struct_data_item GTY(())
{
  struct bli_tree_branch branch;  /* Next at same level; first child.  */
  bli_token *reference_token;  /* First token.  */
  bli_token *level_number_token;  /* Token containing the level number.  */
  bli_token *data_name_token;  /* Token containing the data name.  */

  struct bli_tree_struct_usage *usage_details;  /* Usage details.  */
  bli_token *pic_token;  /* Token containing the PIC value.  */
  struct bli_tree_struct_redefines *redefines_details;  /* Details of the redefines clause.  */
  struct bli_tree_struct_external *external_details;  /* Details of the external clause.  */
  struct bli_tree_struct_value_item *value_list;  /* First of list of value items.  */
  struct bli_tree_struct_occurs_clause *occurs_details;  /* Occurs clause if any.  */
  tree save_decl;  /* Decl for this data item.  */
  struct symtab_entry *symbol_table_entry;  /* Entry in symbol table.  */
  bli_item *owner;  /* Enclosing data structure, file, report, etc.  */
  struct bli_tree_struct_data_item *level88;  /* Level 88 list.  */

  uint32 data_length_bytes;  /* Length in bytes excluding nested occurs depending on items.  */

  unsigned usage_type: 16;  /* Usage token ID (or DISPLAY if group).  */
  unsigned digits: 16;  /* Digits before decimal point.  */
  unsigned lvl: 8;  /* Level 01-50, 66, 77, 88.  */
  unsigned signed_flag: 1;  /* 1 If signed.  */
  unsigned group_flag: 1;  /* 1 If group item.  */
  unsigned has_subordinate_flag: 1;  /* 1 If has subordinate item.  */
  unsigned duplicate_notfirst_flag: 1;  /* 1 If duplicate name within hierarchy but not first.  */
  unsigned duplicate_flag: 1;  /* 1 If duplicate name within hierarchy.  */
  unsigned redefine_master_flag: 1;  /* 1 If this is the
  representation of the redefines group.  Its children will be all the
  members of the redefines group including the first one (the
  redefined one). This works much like a structure but instead of
  being a C struct it is a C union.  The implication of this flag
  being on is that this data item is a dummy/fake.  */
  unsigned redefine_member_flag: 1; 
  unsigned redefine_done_flag: 1; 
  unsigned contains_occurs_depending_on_flag: 1; 
  unsigned occurs_done: 1; 
  unsigned based_flag: 1; 
  unsigned filler_flag: 1; 
  unsigned includes_non_display: 1;  /* Item is or includes something that is not usage display.  */
  unsigned in_dynamic_storage: 1;  /* Item is in dynamic storage.  */
  unsigned save_expr_needed: 1;  /* Item used more than once, save expression value.  */
  unsigned numeric_flag: 1;  /* Item used more than once, save expression value.  */
};

/* Parse tree for USAGE clause.  */
struct bli_tree_struct_usage GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  bli_token *usage_token; 
  bli_token *sign_token; 
};

/* PIC clause parse tree.  */
struct bli_tree_struct_pic GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  bli_token *pic_token; 
};

/* REDEFINES clause parse tree.  */
struct bli_tree_struct_redefines GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  bli_token *redefines_token; 
  struct bli_tree_struct_data_item *redefines_master;
};

/* EXTERNAL clause parse tree.  */
struct bli_tree_struct_external GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  bli_token *external_token; 
  bli_token *language_option; 
  bli_token *alt_data_name_token;  /* Token containing the alternate data name.  */
};

/* One in list of VALUEs or pairs of values in value clause parse tree.  */
struct bli_tree_struct_value_item GTY(())
{
  struct bli_tree_cons cons; 
  bli_token *reference_token; 
  bli_item *value_ptr;  /* Token or ALL exp.  */
  bli_token *value_tok;  /* Token.  */
  struct bli_tree_struct_value_item *second_value;  /* Second value if present.  */
  enum VALUE_TYPE value_type;  /* Enum VALUE_TYPE.  */
  unsigned value_all: 1;  /* Value is 'all' something.  */
  union value_number_u
    { /* The tags are irrelevant for ggc as these are embedded scalars
         so just put anything.  */
      uint64 GTY ((tag ("1"))) value_unsigned;
      int64 GTY ((tag ("2"))) value_signed;
    } GTY ((desc ("1"))) value_number;
  uchar *string;
  uint32 length;
};

/* Details of OCCURS clause.  */
struct bli_tree_struct_occurs_clause GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token; 
  bli_token *occurs_from; 
  bli_token *occurs_to; 
  bli_token *occurs_depending_token;
  struct bli_tree_struct_data_item *occurs_depending_item;
  struct bli_tree_struct_occurs_clause *occurs_next;
  struct bli_tree_struct_data_item *owner; 
  uint32 occurs_from_int;
  uint32 occurs_to_int;
  unsigned occurs_depending: 1;
};

/* Common statement prefix.  */

struct statement_prefix GTY(())
{
  struct bli_tree_branch branch; 
  bli_token *reference_token;
};

/* DISPLAY verb details.  */

struct bli_tree_struct_verb_display GTY(())
{
  struct statement_prefix pfx;  /* Child is first thing to display.  */
};

/* STOP verb details.  */

struct bli_tree_struct_verb_stop GTY(())
{
  struct statement_prefix pfx;
};

bli_token *cleanup_string (bli_token *current_string);
bli_token *cleanup_strings (struct bli_tree_branch *maybe_string);

/* SET verb details.  */

enum
{
  set_type_address = 1,
  set_type_condition
} set_type;

struct bli_tree_struct_verb_set GTY(())
{
  struct statement_prefix pfx; 
  /* 'Up' 'down' or 'true' token or null if none of the above.  */
  bli_token *up_down_true_token; 
  bli_item *set_to;  /* Where the result of the expression gets put (list).  */
  bli_item *set_expression;  /* The 'from' expression.  */
  uchar set_type;  /* See set_type above.  */
};

/* PROCEDURE_REF details.  */

struct bli_tree_struct_procedure_ref GTY(())
{
  struct statement_prefix pfx; 
  bli_token *first_name;
  bli_token *second_name;
  bli_item *label;  /* One label or many chained off tree_lists.  */
};

/* GOTO verb details.  */

struct bli_tree_struct_verb_goto GTY(())
{
  struct statement_prefix pfx; 
  bli_item *proc;  /* One label or list of them pointed to by 'branch' structs.  */
  bli_item *expression;  /* Data reference for goto depending on expression.  */
};

/* LABEL details - paragraph or section, a pseudo verb.  */

struct bli_tree_struct_label GTY(())
{
  struct statement_prefix pfx; 
  struct bli_tree_struct_label *label_next;
  tree save_decl;  /* GCC decl.  */
  /* Used as index into perform return arrays.  Value is -1 if label
     not subject of perform exit.  */
  int32 label_exit_number; 
};

struct bli_tree_struct_section GTY(())
{
  struct bli_tree_struct_label l;
};

struct bli_tree_struct_paragraph GTY(())
{
  struct bli_tree_struct_label l;
};

/* COMPUTE verb details.  */

struct bli_tree_struct_verb_compute GTY(())
{
  struct statement_prefix pfx; 
  bli_item *compute_to;  /* Where the result of the expression gets put (list).  */
  bli_item *compute_expression;  /* The 'from' expression.  */
};

/* MOVE verb details.  */
struct bli_tree_struct_verb_move GTY(())
{
  struct statement_prefix pfx; 
  bli_token *corr_token; 
  bli_item *move_to;  /* Where the result of the expression gets put (list).  */
  bli_item *move_from;  /* The 'from' data reference or literal.  */
  bli_item *move_corr_list;  /* List of the actual corresponding moves.  */
};

/* Generic parse tree for list of things with two things in each entry.  */
struct bli_tree_struct_double_entry_list GTY(())
{
  struct bli_tree_branch branch;
  bli_item *child2;
};

/* CONTINUE verb details.  */
struct bli_tree_struct_verb_continue GTY(())
{
  struct statement_prefix pfx; 
 };

/* EXIT verb details.  */
struct bli_tree_struct_verb_exit GTY(())
{
  struct statement_prefix pfx; 
  bli_token *program;
};

/* NEXT SENTENCE verb details.  */
struct bli_tree_struct_verb_next_sentence GTY(())
{
  struct statement_prefix pfx; 
  struct bli_tree_struct_verb_fullstop *end_of_sentence;
  struct bli_tree_struct_verb_next_sentence *next_pending;
};

/* FULLSTOP pseudo verb details.  */
struct bli_tree_struct_verb_fullstop GTY(())
{
  struct statement_prefix pfx; 
  struct bli_tree_struct_verb_fullstop *next_to_make_decls;
  tree save_decl;  /* GCC decl.  */
  uint32 used; /* Someone wants to jump here.  */
};

/* IF verb details.  */
struct bli_tree_struct_verb_if GTY(())
{
  struct statement_prefix pfx; 
  bli_item *if_condition;  /* Decision expression or conditional variable.  */
  bli_item *if_true;  /* Stmts or SENTENCE for true case.  */
  bli_item *if_false;  /* Stmts or SENTENCE for false case.  */
};

/* End of a program.  */
struct bli_tree_struct_end_program GTY(())
{
  struct bli_tree_cons noncons; 
  bli_token *reference_token;  /* END.  */
  bli_token *type_token;  /* PROGRAM.  */
  bli_token *name_token;  /* Name.  */
};

/* One in list of VALUE clauses.  */
struct bli_tree_struct_value_clause GTY(())
{
  struct bli_tree_cons cons; 
  bli_token *reference_token; 
  struct bli_tree_struct_value_item *value_items; 
};


/* PERFORM verb details.  */
struct bli_tree_struct_verb_perform GTY(())
{
  struct statement_prefix pfx; 
  bli_item *what_to_perform;  /* label range or first statement.  */
  bli_token *when_to_test;  /* BEFORE/AFTER or NULL.  */
  bli_item *action;  /* varying/after or TIMES details. */
  uint32 variant; /* TIMES VARYING (token ID) etc.  */
};

/* Details of label FROM THRU clause.  */
struct bli_tree_struct_labels GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token;  /* First token.  */
  struct bli_tree_struct_procedure_ref *from;
  struct bli_tree_struct_procedure_ref *thru;
};

/* Details of perform varying / after clause.  */
struct bli_tree_struct_perform_varying GTY(())
{
  struct bli_tree_noncons noncons; 
  bli_token *reference_token;  /* First token.  */
  bli_item *variable;
  bli_item *from;
  bli_item *by;
  bli_item *until;
  tree from_expr; /* GCC tree expressions the the components of the varying clause.  */
  tree by_expr;
  tree var_expr;
};

/* CALL verb details.  */
struct bli_tree_struct_verb_call GTY(())
{
  struct statement_prefix pfx; 
  bli_item *what_to_call;  /* string or identifier.  */
  bli_item *using_list;  /* TREE_LIST of usings.  */
  bli_token *language_option; 
  uint32 as; /* Flag to say name is not mapped.  */
};
  
/* Union for token or language construct.  */

union bli_item_union GTY ((desc ("((((struct bli_token_struct*)&%0)->pfx.cat)|((((struct bli_token_struct*)&%0)->pfx.cat==type_token)?0:(((struct bli_token_struct*)&%0)->pfx.type.prod_type<<16)))")))
{
  bli_token GTY ((tag ("type_token"))) s000;
  struct bli_tree_struct_parse_tree_top * GTY ((tag ("type_production | (BLI_PROD_TYPE_PARSE_TREE_TOP<<16)"))) s001;
  struct bli_tree_struct_program_top GTY ((tag ("type_production | (BLI_PROD_TYPE_PROGRAM_TOP<<16)")))  s002;
  struct bli_tree_struct_identification_division GTY ((tag ("type_production | (BLI_PROD_TYPE_IDENTIFICATION_DIVISION<<16)")))  s003;
  struct bli_tree_struct_environment_division GTY ((tag ("type_production | (BLI_PROD_TYPE_ENVIRONMENT_DIVISION<<16)")))  s004;
  struct bli_tree_struct_data_division GTY ((tag ("type_production | (BLI_PROD_TYPE_DATA_DIVISION<<16)"))) s005;
  struct bli_tree_struct_procedure_division GTY ((tag ("type_production | (BLI_PROD_TYPE_PROCEDURE_DIVISION <<16)"))) s006;  
  struct bli_tree_struct_program_id GTY ((tag ("type_production | (BLI_PROD_TYPE_PROGRAM_ID<<16)"))) s007;
  struct bli_tree_struct_storage GTY ((tag ("type_production | (BLI_PROD_TYPE_STORAGE<<16)"))) s008;
  struct bli_tree_struct_data_item GTY ((tag ("type_production | (BLI_PROD_TYPE_DATA_ITEM<<16)"))) s009;
  struct bli_tree_struct_usage GTY ((tag ("type_production | (BLI_PROD_TYPE_USAGE<<16)"))) s010;
  struct bli_tree_struct_pic GTY ((tag ("type_production | (BLI_PROD_TYPE_PIC<<16)"))) s011;
  struct bli_tree_struct_redefines GTY ((tag ("type_production | (BLI_PROD_TYPE_REDEFINES<<16)"))) s012;
  struct bli_tree_struct_external GTY ((tag ("type_production | (BLI_PROD_TYPE_EXTERNAL<<16)"))) s013;
  struct bli_tree_struct_value_item GTY ((tag ("type_production | (BLI_PROD_TYPE_VALUE_ITEM<<16)"))) s014;
  struct bli_tree_struct_occurs_clause GTY ((tag ("type_production | (BLI_PROD_TYPE_OCCURS_CLAUSE<<16)"))) s015;
  struct bli_tree_struct_item_list GTY ((tag ("type_production | (BLI_PROD_TYPE_ITEM_LIST<<16)"))) s016; 
  struct bli_tree_struct_verb_display GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_DISPLAY<<16)"))) s017;
  struct bli_tree_struct_verb_stop GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_STOP<<16)"))) s018;
  /*  struct bli_tree_struct_unused18 GTY ((tag ("type_production | (BLI_PROD_TYPE_<<16)"))) s019; */
  struct bli_tree_struct_expression GTY ((tag ("type_production | (BLI_PROD_TYPE_EXPRESSION<<16)"))) s020;
  struct bli_tree_struct_verb_set GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_SET<<16)"))) s021;  
  struct bli_tree_struct_section GTY ((tag ("type_production | (BLI_PROD_TYPE_SECTION<<16)"))) s022;  
  struct bli_tree_struct_paragraph GTY ((tag ("type_production | (BLI_PROD_TYPE_PARAGRAPH<<16)"))) s023;
  struct bli_tree_struct_procedure_ref GTY ((tag ("type_production | (BLI_PROD_TYPE_PROCEDURE_REF<<16)"))) s024;
  struct bli_tree_struct_verb_goto GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_GOTO<<16)"))) s025;
  struct bli_tree_struct_verb_compute GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_COMPUTE<<16)"))) s026;
  struct bli_tree_struct_verb_move GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_MOVE<<16)"))) s027;
  struct bli_tree_struct_double_entry_list GTY ((tag ("type_production | (BLI_PROD_TYPE_ITEM_DOUBLE_ENTRY_LIST<<16)"))) s028;
  struct bli_tree_struct_verb_continue GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_CONTINUE<<16)"))) s029;
  struct bli_tree_struct_verb_exit GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_EXIT<<16)"))) s030;
  struct bli_tree_struct_verb_fullstop GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_FULLSTOP<<16)"))) s031; 
  struct bli_tree_struct_verb_if GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_IF<<16)"))) s032;
  struct bli_tree_struct_end_program GTY ((tag ("type_production | (BLI_PROD_TYPE_END_PROGRAM<<16)"))) s033;
  struct bli_tree_struct_verb_next_sentence GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_NEXT_SENTENCE<<16)"))) s034;
  struct bli_tree_struct_save_details GTY ((tag ("type_production | (BLI_PROD_TYPE_SAVE_DETAILS<<16)"))) s035;
  struct bli_tree_struct_value_clause GTY ((tag ("type_production | (BLI_PROD_TYPE_VALUE_CLAUSE<<16)"))) s036; 
  struct bli_tree_struct_verb_perform GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_PERFORM<<16)"))) s037; 
  struct bli_tree_struct_section GTY ((tag ("type_production | (BLI_PROD_TYPE_SECTION<<16)"))) s038a; 
  struct bli_tree_struct_paragraph GTY ((tag ("type_production | (BLI_PROD_TYPE_PARAGRAPH<<16)"))) s038b; 
  struct bli_tree_struct_perform_varying GTY ((tag ("type_production | (BLI_PROD_TYPE_PERFORM_VARYING<<16)"))) s039; 
  struct bli_tree_struct_verb_call GTY ((tag ("type_production | (BLI_PROD_TYPE_VERB_CALL<<16)"))) s040; 
};
#endif
