%{
  /*
	*
	* Taken over by
	* Roar Thronæs <roart@nvg.ntnu.no>
	* 7-MAY-2000
	*
*/
  int yy2debug=0;
  int yydebug=0;
#define YYERROR_VERBOSE
#define YYDEBUG 1
//#define YYSTYPE struct cmp_token_struct

#include "blir_temp_config.h"
#include "ignorance.h"
#include "symbtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  extern int undefmode;
  extern int tnamemode;
  extern int linenumb;

#define malloc xmalloc

#include "gansidecl.h"
#include "config.h"

#undef IN_GCC
#include "ggc.h"
#define IN_GCC

#define malloc xmalloc

#include "tree.h"

#include "blidebug.h"
#include "bliumem.h"
#include "bliumsg.h"

#include "bliclex.h"
#include "blicc1.h"
#include "blicsyt.h"
#include "blicpru.h"

#include "c-common.h"

//#define YYSTYPE struct cmp_token_struct

static struct bli_token_struct * current_token=NULL;
static struct bli_token_struct * first_available_token=NULL;
 static int compstmt_count;

struct bli_tree_struct_parse_tree_top* parse_tree_top=NULL;
 static tree current_declspecs = NULL_TREE;
 static tree prefix_attributes = NULL_TREE;
 static tree all_prefix_attributes = NULL_TREE;
 static tree declspec_stack;

extern tree  build_modify_expr (tree, enum tree_code, tree);

#define symbtabinfo
  typedef int nodeAttr_t;

  tree  creatnode (nodeAttr_t, tree, tree);
  tree  creatid (char *);
  tree  creatvalue (int);
  tree  ForExpand(char *, int, int, tree);
//  int      yyerror (char *);
  int lineno=1;

void 
bli_error2 PARAMS ((char * msg));

int 
bli_lex PARAMS((void));

int 
bli_parse PARAMS((void));

 tree igroot;

%}

%start mystart

%union {
  int type_int;
  char     *type_str;
  tree  type_node_p;
  int type_node_code;
  const char *filename;
  int lineno;
  enum tree_code code;
}

/* test */

%token_table
%token <type_node_p> T_DIGITS
%token <type_node_p> T_NAME T_STRING T_IDIGITS

%token equal '='
%token plus '+'
%token minus '-'
%token colon ':'
%token semicolon ';'
/*%ttype <type_str> equal plus minus colon semicolon*/
%token uplus '+'
%token uminus '-'
%token bplus '+'
%token bminus '-'
/*%light uplus uminus*/
%right '='
%left K_EQV K_XOR
%left K_OR
%left K_AND
%right K_NOT
%left K_EQL  K_EQLA  K_EQLU  K_NEQ  K_NEQA  K_NEQU K_LSS  K_LSSA  K_LSSU  K_LEQ  K_LEQA  K_LEQU K_GTR  K_GTRA  K_GTRU  K_GEQ  K_GEQA  K_GEQU
%left  '+' '-'
%left K_MOD '*' '/'
%left '^'
%left UMINUS UPLUS
%right '.'
%token apo '''
/* You won't see these tokens
* %token EOF NIL ECOMM TCOMM
* %token ASSIGN COLON COMMA DIGITS DIV DOT LANG LBRACK LPAREN
* %token MINUS MUL NAME PERC PLUS POW RANG RBRACK RPAREN SEMI STRING
*/

/* These tokens will be recognized by the lexical analyzer */
/*%token T_DIGITS T_NAME T_STRING*/

/* These tokens are reserved keywords */
%token K_ADDRESSING_MODE
%token K_ALIGN K_ALWAYS K_AND K_BEGIN K_BIND K_BIT
%token K_BUILTIN K_BY K_BYTE K_CASE K_CODECOMMENT K_COMPILETIME K_DECR
%token K_DECRA K_DECRU K_DO K_ELSE K_ELUDOM K_ENABLE K_END K_EQL
%token K_EQLA K_EQLU K_EQV K_EXITLOOP K_EXTERNAL K_FIELD K_FORWARD
%token K_FROM K_GEQ K_GEQA K_GEQU K_GLOBAL K_GTR K_GTRA K_GTRU K_IF
%token K_INCR K_INCRA K_INCRU K_INITIAL K_INRANGE K_IOPAGE
%token K_KEYWORDMACRO K_LABEL K_LEAVE K_LEQ K_LEQA K_LEQU K_LIBRARY
%token K_LINKAGE K_LITERAL K_LOCAL K_LONG K_LSS K_LSSA K_LSSU K_MACRO
%token K_MAP K_MOD K_MODULE K_NEQ K_NEQA K_NEQU K_NOT K_NOVALUE K_OF
%token K_OR K_OTHERWISE K_OUTRANGE K_OWN K_PLIT K_PRESET K_PSECT
%token K_RECORD K_REF K_REGISTER K_REP K_REQUIRE K_RETURN K_ROUTINE
%token K_SELECT K_SELECTA K_SELECTONE K_SELECTONEA K_SELECTONEU
%token K_SELECTU K_SET K_SHOW K_SIGNED K_STACKLOCAL K_STRUCTURE
%token K_SWITCHES K_TES K_THEN K_TO K_UNDECLARE K_UNSIGNED K_UNTIL
%token K_UPLIT K_VOLATILE K_WEAK K_WHILE K_WITH K_WORD K_XOR
%token K_SIGNAL K_STOP K_SETUNWIND
/* These tokens are reserved keywords with a leading percent */  
%token P_ALLOCATION P_ASCIC P_ASCID P_ASCII P_ASCIZ P_ASSIGN P_B
%token P_BLISS P_BLISS16 P_BLISS32 P_BLISS36 P_BPADDR P_BPUNIT P_BPVAL
%token P_C P_CHAR P_CHARCOUNT P_COUNT P_CTCE P_D P_DECIMAL P_DECLARED
%token P_CHESC
%token P_E P_ELSE P_ERROR P_ERRORMACRO P_EXACTSTRING P_EXITITERATION
%token P_EXITMACRO P_EXPAND P_EXPLODE P_FI P_FIELDEXPAND P_G P_H
%token P_IDENTICAL P_IF P_INFORM P_ISSTRING P_LENGTH P_LTCE P_MESSAGE
%token P_NAME P_NBITS P_NBITSU P_NULL P_NUMBER P_O P_P P_PRINT P_QUOTE
%token P_QUOTENAME P_RAD50_10 P_RAD50_11 P_REF P_REMAINING P_REMOVE
%token P_REQUIRE P_SBTTL P_SIXBIT P_SIZE P_STRING P_SWITCHES P_THE
%token P_TITLE P_UNQUOTE P_UPVAL P_VARIANT P_WARN P_X
/* tname related */
%token CODE NOCODE DEBUG NODEBUG ERRS NOERRS OPTIMIZE NOOPTIMIZE
%token UNAMES NOUNAMES SAFE NOSAFE ZIP NOZIP
/* unreserved keywords */
%token U_ABSOLUTE U_ASSEMBLY U_BINARY U_BLISS10_OTS U_BLISS16 U_BLISS32 U_BLISS36
%token U_BLISS36C_OTS U_CALL U_CLEARSTACK U_CODE U_COMMENTARY U_CONCATENATE U_DEBUG
%token U_EMT U_ENTRY U_ENVIRONMENT U_ERRS U_EXECUTE U_EXPAND U_EXTENDED U_GENERAL
%token U_IDENT U_INDIRECT U_INTERRUPT U_IOT U_JSB U_JSR U_JSYS U_KA10 U_KI10 U_KL10
%token U_KS10 U_LANGUAGE U_LINKAGE_REGS U_LIST U_LONG_RELATIVE U_LSI11 U_MAIN
%token U_NOASSEMBLY U_NOBINARY U_NOCODE U_NOCOMMENTARY U_NODEBUG U_NODEFAULT
%token U_NOERRS U_NOEXECUTE U_NOEXPAND U_NOINDIRECT U_NOLIBRARY U_NONEXTERNAL
%token U_NOOBJECT U_NOOPTIMIZE U_NOPIC U_NOPRESERVE U_NOREAD U_NOREQUIRE U_NOSAFE
%token U_NOSHARE U_NOSOURCE U_NOSYMBOLIC U_NOTRACE U_NOTUSED U_NOUNAMES U_NOWRITE
%token U_NOZIP U_OBJECT U_OPTIMIZE U_OPTLEVEL U_ORIGIN U_OTS U_OTS_LINKAGE U_OVERLAY
%token U_PIC U_PORTAL U_PRESERVE U_PS_INTERRUPT U_PUSHJ U_READ U_RELATIVE
%token U_RELOCATABLE U_RSX_AST U_RTT U_SAFE U_SHARE U_SKIP U_SOURCE U_STACK U_STANDARD
%token U_STANDARD_OTS U_SYMBOLIC U_T11 U_TOPS10 U_TOPS20 U_TRACE U_TRAP U_UNAMES
%token U_VALUECBIT U_VERSION U_WORD_RELATIVE U_WRITE U_ZIP
%token U_LINKAGE U_REQUIRE U_LIBRARY U_EIS U_NOEIS U_ODT
/* predefined */
%token PD_ABS PD_BITVECTOR PD_BLISS PD_BLISS10 PD_BLISS36C PD_BLOCK PD_BLOCKVECTOR
%token PD_CH_DOLLAR_A_RCHAR PD_CH_DOLLAR_A_WCHAR PD_CH_DOLLAR_ALLOCATION PD_CH_DOLLAR_COMPARE PD_CH_DOLLAR_COPY PD_CH_DOLLAR_DIFF
%token PD_CH_DOLLAR_EQL PD_CH_DOLLAR_FAIL PD_CH_DOLLAR_FILL PD_CH_DOLLAR_FIND_CH PD_CH_DOLLAR_FIND_NOT_CH PD_CH_DOLLAR_FIND_SUB
%token PD_CH_DOLLAR_GEQ PD_CH_DOLLAR_GTR PD_CH_DOLLAR_LEQ PD_CH_DOLLAR_LSS PD_CH_DOLLAR_MOVE PD_CH_DOLLAR_NEQ PD_CH_DOLLAR_PLUS PD_CH_DOLLAR_PTR
%token PD_CH_DOLLAR_RCHAR PD_CH_DOLLAR_RCHAR_A PD_CH_DOLLAR_SIZE PD_CH_DOLLAR_TRANSLATE PD_CH_DOLLAR_TRANSTABLE PD_CH_DOLLAR_WCHAR
%token PD_CH_DOLLAR_WCHAR_A PD_FORTRAN PD_FORTRAN_FUNC PD_FORTRAN_SUB PD_F10 PD_MAX PD_MAXA PD_MAXU
%token PD_MIN PD_MINA PD_MINU PD_SETUNWIND PD_SIGN PD_SIGNAL PD_SIGNAL_STOP PD_VECTOR PD__DOLLAR_CODE_DOLLAR_
%token PD__DOLLAR_GLOBAL_DOLLAR_ PD__DOLLAR_HIGH_DOLLAR_ PD__DOLLAR_LOW_DOLLAR_ PD__DOLLAR_OWN_DOLLAR_ PD__DOLLAR_PLIT_DOLLAR_ U_VECTOR
%token REALLY_MAX

/* types */
%type <type_int> onoffmodes CODE NOCODE DEBUG NODEBUG ERRS NOERRS OPTIMIZE
%type <type_int> NOOPTIMIZE UNAMES NOUNAMES SAFE NOSAFE ZIP NOZIP
%type <type_int> P_ASCII P_ASCIZ P_ASCIC P_ASCID P_RAD50_11 P_RAD50_10 P_SIXBIT P_P
%type <type_int> U_EIS U_NOEIS U_LSI11 U_T11 U_PIC U_ODT
%type <type_node_p> pushlevel poplevel
%type <type_node_p> mystart module module_head module_body opt_mhargs ms_list
%type <type_node_p> decl_list module_switch unlabeled_block start_block
%type <type_node_p> unlabeled_block_start
%type <type_node_p> environ_16_option mode_spec mode_spec_list
%type <type_node_p>  common_switch bliss_16_switch bliss_32_switch bliss_36_switch
/*%type <type_int> T_NAME T_STRING T_DIGITS*/
%type <type_node_p> declaration special_switch on_off_switch lowlevel
%type <type_node_p> expression primary operator_expression operator_expression_not
%type <type_int>  P_TITLE U_CALL U_STANDARD
%type <type_node_p> linkage_definition_list linkage_definition p_stuff
%type <type_int> U_BLISS16 U_BLISS32 U_BLISS36 
%type <type_node_p> infix_operator
/*%type <type_node_p> executable_expression control_expression*/
%type <type_node_p> input_parameter_location_list input_parameter_location
%type <type_node_p>  output_parameter_location_list output_parameter_location
%type <type_node_p> linkage_type maybe_forward_routine_attribute_list
%type <type_node_p> control_expression language_list list_option_list
%type <type_node_p> numeric_literal decimal_literal character_code_literal
%type <type_node_p> float_literal integer_literal language_name_list
%type <type_node_p> character_code_literal single_precision_float_literal 
%type <type_node_p> double_precision_float_literal extended_exponent_double_precision_float_literal
%type <type_node_p> extended_exponent_extended_precision_float_literal mantissa
%type <type_node_p> string_literal plit3 plit_item_list
%type <type_node_p> forward_routine_attribute_list plit_group allocation_unit
%type <type_node_p> string_type plit2 
%type <type_node_p> ctce replicator plit_expression linkage_time_constant_expression
%type <type_node_p> plit  plit_item plit_group list_option_list
%type <type_int> U_SOURCE  U_NOSOURCE  U_REQUIRE  U_NOREQUIRE
%type <type_int> U_EXPAND  U_NOEXPAND  U_TRACE  U_NOTRACE
%type <type_int> U_LIBRARY  U_NOLIBRARY  U_OBJECT  U_NOOBJECT
%type <type_int> U_ASSEMBLY  U_NOASSEMBLY  U_SYMBOLIC  U_NOSYMBOLIC
%type <type_int> U_BINARY  U_NOBINARY  U_COMMENTARY  U_NOCOMMENTARY
%type <type_node_p>  plit_string attached_label_list list_option language_name
%type <type_node_p> block labeled_block attached_label unlabeled_block
%type <type_node_p> block_body block_action block_value structure_reference
%type <type_node_p> ordinary_structure_reference default_structure_reference
%type <type_node_p> general_structure_reference segment_name access_actual
%type <type_node_p> access_part field_set_name field_name declaration attribute
%type <type_node_p> data_declaration structure_declaration
%type <type_node_p> field_declaration routine_declaration access_actual_list
%type <type_node_p> ordinary_routine_declaration global_routine_declaration
%type <type_node_p> forward_routine_declaration external_routine_declaration
%type <type_node_p> linkage_declaration enable_declaration
%type <type_node_p> bound_declaration compiletime_declaration
%type <type_node_p> macro_declaration require_declaration
%type <type_node_p> library_declaration psect_declaration
%type <type_node_p> switches_declaration label_declaration psect_allocation
%type <type_node_p> builtin_declaration undeclare_declaration structure_attribute
%type <type_node_p> extension_attribute T_STRING2 string_par_list string_par
%type <type_node_p>  field_attribute cond_handling_function_name
%type <type_node_p>  initial_attribute executable_function_named
%type <type_node_p> alignment_attribute preset_attribute 
%type <type_int> K_AND K_OR K_NOT K_EQV K_XOR op7
%type <type_int> K_EQL K_EQLA K_EQLU K_NEQ K_NEQA K_NEQU K_LSS K_LSSA K_LSSU
%type <type_int> K_LEQ K_LEQA K_LEQU K_GTR K_GTRA K_GTRU K_GEQ K_GEQA K_GEQU
%type <type_int>
%type <type_int>
%type <type_node_p> linkage_attribute tname_list tname_list2 char_par_list char_par
%type <type_node_p> range_attribute novalue_attribute /*pot_expression*/
%type <type_node_p> /*opexp1 opexp2 opexp3 opexp4 opexp5 opexp6 opexp7 opexp8*/
%type <type_node_p> opexp9 lexical_function lexical_function_name 
%type <type_node_p> addressing_mode_attribute weak_attribute own_attribute
%type <type_node_p> address lexical_actual_parameter own_attribute_list
%type <type_node_p> initial_item initial_group initial_expression
%type <type_node_p>  preset_item ctce_access_actual io_actual_parameter_list io_actual_parameter
%type <type_node_p> preset_value psect_item_list psect_item storage_class
%type <type_node_p> volatile_attribute psect_attribute_list psect_attribute
%type <type_node_p> own_declaration global_declaration
%type <type_node_p> external_declaration forward_declaration
%type <type_node_p> local_declaration stackglocal_declaration
%type <type_node_p> register_declaration
%type <type_node_p> global_register_declaration
%type <type_node_p> external_register_declaration
%type <type_node_p> map_declaration
%type <type_node_p> own_item global_item
%type <type_node_p> external_item forward_item
%type <type_node_p> local_item 
%type <type_node_p> register_item
%type <type_node_p> global_reg_item global_reg_item
%type <type_node_p>  external_reg_item external_reg_item_list
%type <type_node_p> map_item map_item_list segment_expression
%type <type_node_p> allocation_name access_formal structure_body structure_size
%type <type_node_p> expression executable_function routine_call codecomment
%type <type_node_p> field_reference integer_literal integer_digit_list
%type <type_node_p> integer_digit digits field_stuff_list
%type <type_node_p> opt_sign psect_name declaration_list access_actual_list
%type <type_node_p> block_action_list dsr1
%type <type_node_p> structure_definition structure_definition_list builtin_name
%type <type_node_p>  global_item_list external_item_list
%type <type_node_p> machine_specific_function linkage_function
%type <type_node_p> forward_item_list local_item_list register_item_list
%type <type_node_p> global_reg_item_list alloc_actual_list
%type <type_node_p> alloc_actual sign_ext_flag
%type <type_node_p> initial_item_list ctce_access_actual_list preset_item_list
%type <type_node_p> initial_string own_item_list global_item_list
%type <type_node_p> attribute_list local_item_list
%type <type_node_p> routine_call ordinary_routine_call general_routine_call
%type <type_node_p> routine_designator actual_parameter_list
%type <type_node_p> actual_parameter_list actual_parameter_list linkage_name
%type <type_node_p> actual_parameter routine_address block
%type <type_node_p> field_selector codecomment quoted_string_list
%type <type_node_p> operator_expression fetch_expression /*prefix_expression*/
%type <type_node_p> infix_expression assign_expression op_exp op_exp1
%type <type_node_p> op_exp2 op_exp3 op_exp4 op_exp5 op_exp6 op_exp7 op_exp8
%type <type_node_p> op_exp9 op_exp10 op_exp11 op_exp12 b32_psect_attribute
%type <type_int> U_READ U_NOREAD U_SHARE U_NOSHARE U_PIC U_NOPIC K_LOCAL 
%type <type_node_p> actual_parameter standard_function_name executable_function
%type <type_int> K_SETUNWIND  K_SIGNAL K_STOP U_OVERLAY  U_CONCATENATE U_VECTOR
%type <type_int>  K_REF U_CODE U_NODEFAULT U_WRITE U_NOWRITE U_EXECUTE U_NOEXECUTE
%type <type_node_p> control_expression conditional_expression  case_expression
%type <type_node_p> select_expression loop_expression case_label_list 
%type <type_int>  select_type K_OTHERWISE  K_ALWAYS
%type <type_node_p> case_line case_line_list select_line select_line_list
%type <type_node_p> select_label select_label_list case_action select_action
%type <type_node_p> loop_expression indexed_loop_expression tested_loop_expression
%type <type_node_p> leave_expression exitloop_expression case_label
%type <type_node_p> unlabeled_block block_body ltce
%type <type_node_p> ordinary_structure_reference field_declaration
%type <type_node_p> routine_declaration linkage_declaration enable_declaration
%type <type_node_p> bound_declaration compiletime_declaration
%type <type_node_p>  pre_tested_loop post_tested_loop
%type <type_int> K_TO K_INRANGE K_OUTRANGE indexed_loop_type
%type <type_int> K_INCR K_INCRA K_INCRU K_DECR K_DECRA K_DECRU
%type <type_int> K_SELECT K_SELECTA K_SELECTU K_WHILE K_DO K_UNTIL
%type <type_int> K_SELECTONE K_SELECTONEA K_SELECTONEU
%type <type_int> K_LEAVE K_WITH  K_EXITLOOP K_RETURN
%type <type_node_p> executable_function_name standard_function_name 
%type <type_node_p> linkage_function_name character_handling_function_name
%type <type_node_p> machine_specific_function_name select_index
%type <type_node_p> routine_address position_exp size_exp 
%type <type_node_p> exit_expression loop_expression return_expression exp
%type <type_node_p> T_NAME
%type <type_int> K_PLIT K_UPLIT K_LONG K_WORD K_BYTE P_CTCE P_LTCE
%type <type_int> K_SIGNED  K_UNSIGNED K_OWN K_GLOBAL K_VOLATILE
%type <type_int> K_FORWARD K_STACKLOCAL  K_REGISTER K_LOCAL K_FORWARD
%type <type_int> K_SIGNED K_UNSIGNED K_VOLATILE K_ROUTINE
%type <type_node_p> setspecs declspecs_ts
%type <type_node_p> allocation_default allocation_name allocation_formal
%type <type_node_p> routine_definition_list io_list2 formal_item_list
%type <type_node_p> ordinary_routine_declaration global_routine_declaration
%type <type_node_p> forward_routine_declaration external_routine_declaration
%type <type_node_p> routine_declaration routine_attribute_list
%type <type_node_p> formal_item io_list routine_definition io_list
%type <type_node_p> formal_attribute_list exp io_list3 global_io_list2
%type <type_node_p> routine_attribute map_declaration_attribute routine_attribute
%type <type_node_p> forward_routine_attribute
%type <type_node_p> global_routine_attribute  ext_routine_attribute
%type <type_node_p> global_routine_declaration global_routine_definition_list
%type <type_node_p> global_routine_definition global_routine_attribute_list
%type <type_node_p> forward_routine_item_list forward_routine_attribute_list
%type <type_node_p> forward_routine_declaration external_routine_item_list
%type <type_node_p> ext_routine_attribute_list forward_routine_item
%type <type_node_p> external_routine_declaration map_declaration_attribute_list
%type <type_node_p> external_routine_item
%type <type_node_p> field_declaration enable_declaration bound_declaration
%type <type_node_p> compiletime_declaration macro_declaration require_declaration
%type <type_node_p> library_declaration psect_declaration switches_declaration 
%type <type_node_p> label_declaration builtin_declaration undeclare_declaration
%type <type_node_p> linkage_declaration
%type <type_node_p>  maybe_declaration_list maybe_block_action_list maybe_block_value
%type <type_node_p> executable_function initial_item_list
%type <type_node_p> actual_parameter_list access_formal_list  allocation_formal_list
%type <type_node_p> compile_time_constant_expression
%type <type_node_p> structure_size  structure_body
%type <type_node_p> require_declaration library_declaration
%type <type_int>  K_STRUCTURE K_FIELD K_ALIGN K_INITIAL
%type <type_int> K_PSECT K_VOLATILE K_NOVALUE K_LINKAGE 
%type <type_int> K_ADDRESSING_MODE K_WEAK K_PRESET
%type <type_int> P_B P_O P_DECIMAL P_X
%type <type_int> 
%type <type_node_p> bind_data_attribute positional_macro_declaration
%type <type_node_p> keyword_macro_declaration
%type <type_node_p> bind_data_attribute_list bind_data_declaration
%type <type_node_p> bind_data_item bind_data_item_list bind_routine_attribute
%type <type_node_p> bind_routine_attribute_list bind_routine_declaration
%type <type_node_p> bind_routine_item bind_routine_item_list
%type <type_node_p> bound_declaration external_literal_declaration
%type <type_node_p> external_literal_item external_literal_item_list
%type <type_node_p> literal_attribute literal_attribute_list
%type <type_node_p>  literal_declaration literal_item literal_item_list
%type <type_node_p> positional_macro_definition mode_16 mode_32
%type <type_node_p> keyword_macro_definition simple_macro_definition
%type <type_node_p> conditional_macro_definition iterative_macro_definition
%type <type_node_p> conditional_macro_definition 
%type <type_node_p> default_actual iterative_macro_definition 
%type <type_node_p> keyword_macro_declaration keyword_macro_definition_list
%type <type_node_p> keyword_pair keyword_pair_list lexeme_list macro_body
%type <type_node_p> positional_macro_declaration positional_macro_definition
%type <type_node_p> positional_macro_definition_list simple_macro_definition
%type <type_node_p> conditional_macro_definition iterative_macro_definition
%type <type_node_p> keyword_macro_definition_list macro_body
%type <type_node_p> keyword_macro_definition
%type <type_node_p> positional_macro_definition_list positional_macro_definition
%type <type_int> U_ABSOLUTE U_RELATIVE U_GENERAL U_LONG_RELATIVE U_WORD_RELATIVE
/*%type <type_node_p> test tok*/
%type <filename> save_filename
%type <lineno> save_lineno

%{
#define PUSH_DECLSPEC_STACK                                              \
  do {                                                                   \
    declspec_stack = tree_cons (build_tree_list (prefix_attributes,      \
                                                 all_prefix_attributes), \
                                current_declspecs,                       \
                                declspec_stack);                         \
  } while (0)

#define POP_DECLSPEC_STACK                                              \
  do {                                                                  \
    current_declspecs = TREE_VALUE (declspec_stack);                    \
    prefix_attributes = TREE_PURPOSE (TREE_PURPOSE (declspec_stack));   \
    all_prefix_attributes = TREE_VALUE (TREE_PURPOSE (declspec_stack)); \
    declspec_stack = TREE_CHAIN (declspec_stack);                       \
  } while (0)

#define SAVE_WARN_FLAGS()                       \
        size_int (pedantic                      \
                  | (warn_pointer_arith << 1)   \
                  | (warn_traditional << 2))

#define RESTORE_WARN_FLAGS(tval)                \
  do {                                          \
    int val = tree_low_cst (tval, 0);           \
    pedantic = val & 1;                         \
    warn_pointer_arith = (val >> 1) & 1;        \
    warn_traditional = (val >> 2) & 1;          \
  } while (0)

  %}

%%

/**** 1.0 MODULES ***************************************************/

save_filename:
{ if (yychar == YYEMPTY)
  yychar = YYLEX;
 $$ = input_filename; }
;

save_lineno:
{ if (yychar == YYEMPTY)
  yychar = YYLEX;
 $$ = lineno; }
;

mystart: module
{ 
  struct bli_tree_struct_program_top* pgm;
  pgm = $1;
  BLI_ALLOC_TREE (parse_tree_top, BLI_PROD_TYPE_PARSE_TREE_TOP, 
                 bli_tree_struct_parse_tree_top);
  parse_tree_top->branch.child = (bli_item*) pgm;
  $$ = pgm;
  igroot=parse_tree_top;
  igroot=$1;
};
/*module_body*/
module		: K_MODULE module_head '=' 
{
  last_expr_filename=input_filename;
  //current_function_decl=$2;
  //  bli_start_function (current_declspecs, , all_prefix_attributes);
  // store_parm_decls();
}
save_filename save_lineno
 start_block K_ELUDOM
{
  $$=$7;
  //  DECL_SOURCE_FILE (current_function_decl) = $5;
  //  DECL_SOURCE_LINE (current_function_decl) = $6;
  //  bli_finish_function (0, 1); 
  //  POP_DECLSPEC_STACK;
  fprintf (stderr, "%s\n",OK);
  while (! global_bindings_p ())
    poplevel (0, 0, 0);
  finish_fname_decls ();
  finish_file ();
}
;

module_head	: T_NAME
opt_mhargs
{ /* set the default switches */
  /*ig_name=(char *)$1;*/
  /*   $$->id=$1; */
  char *c=$1;
  $$=build_string(strlen(c),c);
}
;

opt_mhargs	: /* empty */ { $$=0; }
| '(' ms_list ')' {  }
/*| '(' ms_list ')' { fprintf(stderr,"err %x \n",$2); fflush(stderr); }*/
  		;

ms_list		: { undefmode=1; } module_switch { undefmode=0; $$ = $2; }
| ms_list ',' { undefmode=1; } module_switch { undefmode=0;  }
;

module_body: /* empty */ { $$=0; }
| K_BEGIN decl_list K_END
{ }
| '(' decl_list ')'
{ }
;

decl_list	: declaration { $$=$1; }
| decl_list declaration {  }
;

/* 1.1 Module Switches */

module_switch	: on_off_switch { $$= $1; }
| special_switch { $$= $1; }
;

on_off_switch: onoffmodes { };

onoffmodes:
CODE		/* default */
| NOCODE
| DEBUG
| NODEBUG		/* default */
| ERRS			/* default */
| NOERRS
| OPTIMIZE		/* default */
| NOOPTIMIZE
| UNAMES
| NOUNAMES		/* default */
| SAFE			/* default */
| NOSAFE
| ZIP
| NOZIP			/* default */
;

special_switch:
common_switch { $$=$1; }
/*|bliss_16_switch { $$=$1; }*/
|bliss_32_switch { $$=$1; }
/*|bliss_36_switch { $$=$1; }*/
;

bliss_16_switch:
K_ADDRESSING_MODE '(' mode_16 ')' {  }
/*|U_ENVIRONMENT '(' environ_16_option ')'  {  }*/
;

environ_16_option:
U_EIS { }
| U_NOEIS { }
| U_LSI11 { }
| U_T11 { }
|	 U_PIC { }
| U_ODT { }
;

bliss_32_switch:
K_ADDRESSING_MODE '(' mode_spec_list ')' {  }
;

mode_spec_list: mode_spec_list ',' mode_spec {  }
| mode_spec { $$=$1; }
;

mode_spec:
K_EXTERNAL '='  mode_32 {  }
| U_NONEXTERNAL '='  mode_32 {  }
;

bliss_36_switch: /*nonfin*/ P_ASCIZ { $$=0; }
;

common_switch	:  U_IDENT '=' T_STRING { }
| U_LANGUAGE '(' language_list ')' {  }
| U_LINKAGE '(' T_NAME ')' { }
| U_LIST '(' list_option_list ')' {  }
| K_STRUCTURE '(' structure_attribute ')' {  }
| U_MAIN '=' T_NAME { }
| U_OPTLEVEL '=' T_DIGITS { }
| U_VERSION '=' T_STRING { }

/*| P_TITLE T_STRING {  $$->id=$2; nonfin}*/
/*
  |T_NAME "equal" T_NAME
  {
   $$->id=$1;
  fprintf(stderr, "swit %x %x %x\n",$$,$1,$3);
  switch (ukeyword( $1 )) {
  case U_MAIN:
  ig_main  = $3;
  fprintf(stderr,"main= %d %s \n",switch_main,ig_main);
  break;
  default:
  yyerror ("here3");
  }
  }
  | T_NAME "equal" T_DIGITS
  {
   $$->id=$1;
  switch (ukeyword( $1 )) {
  case U_OPTLEVEL:
  switch_optlevel = (int )$3;
  0 | 1 | 2 | 3 
  break;
  default:
  yyerror ("here2");
  }
  }
  | T_NAME "equal" T_STRING
  {
   $$->id=$1;
  switch (ukeyword( $1 )) {
  case U_IDENT:
  switch_ident = (int)$3;
  break;
  case U_VERSION:
  switch_version = (int)$3;
  break;
  default:
  yyerror ("here1");
  }
  }
  | T_NAME '(' name_list ')'
  {*/ /* declare p */
/*			  int i;
			  ignodeptr p;
			  			   $$->id=$1;

			  switch (ukeyword( $1 )) {
			  case U_LANGUAGE:
			    switch_language = (int)$3;*/
			    /* language_list */
			    /* U_COMMON, U_BLISSxy */
/*			    break;
			  case U_LINKAGE:*/
			    /* linkage_name */
/*			    break;
			  case U_LIST:*/
			    /* list_option ,... */
/*			    i = 1 | 32 | 128 | 256 | 512;*/
				 /*			    for (p=$3; p; p = p->next)*/
/*			    for (p=$3; p; p = 0)
			      {
					  				switch (p->type)
				  {
				  case U_SOURCE:   i |=   1; break;
				  case U_NOSOURCE: i &=  ~1; break;
				  case U_REQUIRE:  i |=   2; break;
				  case U_NOREQUIRE:i &=  ~2; break;
				  case U_EXPAND:   i |=   4; break;
				  case U_NOEXPAND: i &=  ~4; break;
				  case U_TRACE:	   i |=   8; break;
				  case U_NOTRACE:  i &=  ~8; break;
				  case U_LIBRARY:  i |=  16; break;
				  case U_NOLIBRARY:i &= ~16; break;
				  case U_OBJECT:   i |=  32; break;
				  case U_NOOBJECT: i &= ~32; break;
				  case U_ASSEMBLY: i |=  64; break;
				  case U_NOASSEMBLY:i &= ~64; break;
				  case U_SYMBOLIC:  i |= 128; break;
				  case U_NOSYMBOLIC:i &= ~128; break;
				  case U_BINARY:    i |=  256; break;
				  case U_NOBINARY:  i &= ~256; break;
				  case U_COMMENTARY:i |=  512; break;
				  case U_NOCOMMENTARY:
				    i &= ~512; break;
				  default:
				    yyerror ("here");
				  }
				switch_list = i;
			      }
			  }
			  */		  /*			  case K_STRUCTURE:*/
			    /* optional structure_attribute */
			  /*			    break;*/
/*		  default:
			    yyerror ("her");
			}*/
		;

language_list: { $$=0; }
|"common" { }
| language_name_list { $$=$1;}
;
language_name_list: language_name_list ',' language_name {  }
|language_name { $$=$1; }
;
language_name:  U_BLISS16 { }
| U_BLISS32  { }
| U_BLISS36  { }
;
list_option_list: list_option_list list_option {  }
|list_option { $$=$1; }
;
list_option:  U_SOURCE   { }
| U_NOSOURCE    { }
| U_REQUIRE    { }
| U_NOREQUIRE   { }
| U_EXPAND    { }
| U_NOEXPAND   { }
| U_TRACE   { }
| U_NOTRACE   { }
| U_LIBRARY   { }
| U_NOLIBRARY   { }
| U_OBJECT   { }
| U_NOOBJECT   { }
| U_ASSEMBLY   { }
| U_NOASSEMBLY   { }
| U_SYMBOLIC   { }
| U_NOSYMBOLIC   { }
| U_BINARY   { }
| U_NOBINARY   { }
| U_COMMENTARY   { }
| U_NOCOMMENTARY    { }
;

tname_list: tname_list ',' T_NAME {  }
|T_NAME {  }
;

tname_list2: tname_list2  T_NAME {  }
|T_NAME {  }
;
/*
  name_list	: T_NAME
  {
  int i=ukeyword( $1 );
  
  fprintf(stderr, "$1 %s\n",$1);
  if (i>0)
  
  else {
  
  $$->id=(char *) $1;
  }
  }
  | name_list ',' T_NAME
  {
  int i=ukeyword( $3 );
  
  fprintf(stderr, "$1 %s\n",$1);
  if (i>0)
  
  else {
  
  $$->id=(char *) $1;
  }
  }
  |*/ /* empty */
/*			{
			$$ = NULL;
			}
			;
		*/

lowlevel: T_DIGITS {  }
| T_NAME {  }
;

/**** 2.0 EXPRESSIONS ***********************************************/
expression: 
primary  { $$=$1; }
| operator_expression  { $$=c_expand_expr_stmt($1); }
| executable_function  { $$=$1; }
| control_expression   { $$=$1; }
| p_stuff { $$=$1; };

p_stuff:
P_REMAINING {  }

primary: 
numeric_literal  { $$=$1; }
| string_literal  { $$=$1; }
| plit { $$=$1; }
| T_NAME { 
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  $$ = build_external_ref ($1, yychar == '(');
}
| block  { $$=$1; }
| structure_reference { $$=$1; }
| routine_call  { $$ = c_expand_expr_stmt ($1); }
| field_reference { $$=$1; }
| codecomment { $$=$1; }
;
numeric_literal: 
decimal_literal  { $$=$1; /* TREE_TYPE($1)=integer_type_node;*/ }
| integer_literal { $$=$1; }
| character_code_literal { $$=$1; }
| float_literal  { $$=$1; }
;

decimal_literal: 
T_DIGITS { 
  tree t;
  t = build_int_2(atoi($1->identifier.id.str),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
  $$ = t;
 }
;

opt_sign: { $$=0; }
|'+' {  }
|'-' {  }
;

integer_literal: 
P_B T_STRING { }
| P_O T_STRING { }
| P_DECIMAL T_STRING { }
| P_X T_STRING { }
;                  
/*
  P_B apo opt_sign T_NAME apo {  }
  | P_O apo opt_sign T_NAME apo {  }
  | P_DECIMAL apo opt_sign T_NAME apo {  }
  | P_X apo opt_sign T_NAME apo {  }
  */
						
integer_digit_list:  
integer_digit_list integer_digit {  }
|integer_digit { $$ = $1; }
;

integer_digit:   digits  { $$=$1; }
| T_NAME  {  }
;

character_code_literal: P_C T_STRING { }
;

/*The quoted_character is defined in the LEXICAL DEFINITION part of this
text. Here, only one quoted_character must appear.*/

float_literal: 
single_precision_float_literal { $$=$1; }
| double_precision_float_literal { $$=$1; }
| extended_exponent_double_precision_float_literal { $$=$1; }
| extended_exponent_extended_precision_float_literal { $$=$1; }
;

single_precision_float_literal:
P_E " mantissa {E exponent | nothing } " {  }
;

double_precision_float_literal:
P_D " mantissa { D exponent | nothing } " {  }
;

extended_exponent_double_precision_float_literal:
P_G " mantissa { { G | Q } exponent | nothing } " {  }
;

extended_exponent_extended_precision_float_literal:
P_H " mantissa { Q exponent | nothing } " {  }
;

mantissa: opt_sign
digits | digits '.' | '.' digits { $$=$2; } | digits '.' digits 
;

digits: digits T_DIGITS { }
| T_DIGITS {}
;

string_literal:  string_type T_STRING2 {  }
| T_STRING2 {  }
;

/*T_STRING2 is a new extension nonfin*/
T_STRING2:
T_STRING {  }
|P_STRING '(' string_par_list ')' {  }
|P_CHAR '(' char_par_list ')' {  }
|P_CHESC {  }
;

string_par_list:string_par_list ',' string_par {  }
|string_par { $$=$1; }
;

string_par:
numeric_literal { $$=$1 ; }
|string_literal { $$=$1; }
;

char_par_list:char_par_list ',' char_par {  }
|char_par { $$=$1; }
;

char_par: ctce { $$=$1; }
;

string_type:  P_ASCII { }
| P_ASCIZ  { }
| P_ASCIC  { }
| P_ASCID  { }
| P_RAD50_11 { }
| P_RAD50_10  { }
| P_SIXBIT  { }
| P_P  { }
;

plit2:
K_PLIT { }
|K_UPLIT { }
;

plit3: { $$=0; }
| allocation_unit {  }
| psect_allocation{  }
| psect_allocation allocation_unit{  }
;

plit: plit2 plit3 '(' plit_item_list ')' { }
;

plit_item_list: plit_item_list ',' plit_item {  }
|plit_item { $$=$1; }
;

psect_allocation: K_PSECT '(' T_NAME ')' { }
;

psect_name: T_NAME {  }

plit_item: 
plit_group  { $$=$1; }
| plit_expression { $$=$1; }
| plit_string { $$=$1; }
;

plit_group: 
allocation_unit {  }
| K_REP replicator K_OF {  }
| K_REP replicator K_OF allocation_unit {  }
;

allocation_unit:  K_LONG  { }
| K_WORD  { }
| K_BYTE { }
;

replicator: ctce { $$=$1; }
;

plit_expression: ltce { $$=$1; }
;

plit_string: string_literal { $$=$1; }
;

block: 
labeled_block 
| unlabeled_block
;

labeled_block: attached_label_list unlabeled_block { $$=chainon($1,$2); }
;

attached_label_list: attached_label_list attached_label {  }
| attached_label { $$=$1; }
;

attached_label: 
T_NAME ':' {  }
;

unlabeled_block_start: K_BEGIN { 
$$=c_begin_compound_stmt ();
}

unlabeled_block: unlabeled_block_start
pushlevel block_body K_END poplevel
{ 
$$=poplevel (kept_level_p (), 1, 0);
 SCOPE_STMT_BLOCK (TREE_PURPOSE ($5))
   = SCOPE_STMT_BLOCK (TREE_VALUE ($5))
   = $$;

 RECHAIN_STMTS ($1, COMPOUND_BODY ($1)); 
 last_expr_type = NULL_TREE;
 $$=$1;
}
| '(' block_body ')' { $$=$2;} 
;

start_block: K_BEGIN { 
  // find out how to do with/without $$=c_begin_compound_stmt ();
  // RECHAIN_STMTS ($$, COMPOUND_BODY ($$)); 
  //last_expr_type = NULL_TREE;
}
block_body K_END
{ 
}
| '(' block_body ')' { $$=$2;} 
;


block_body: 
maybe_declaration_list 
{ /* $$ = bli_begin_compound_stmt (); */ }
maybe_block_action_list 
maybe_block_value /*  $$->middle=$2;  */
/* {  }*/
{
  //tree decl;
  //DECL_NAME(decl)=
  //$$=decl;
  void * v = $1;
  void * w = $3;
  fprintf(stderr,"vw %x %x\n",v,w);
  //  $$ = chainon(0, $3);
  $$=$3;
}
;
/*
maybe_declaration_list: { $$=0; }
| declaration_list { $$=$1; }
;
*/
maybe_declaration_list: { $$=NULL_TREE; }
| maybe_declaration_list declaration {
  /*  */
  $$ = chainon($1, $2);
 }
;

declaration_list: declaration_list declaration {
  /*   */
  $$=chainon($1,$2);
 }
|declaration { $$=$1; }
;
/*
  maybe_block_action_list: { $$=0; }
  |block_action_list  { $$=$1; }
  ;
*/
maybe_block_action_list: { $$=0; }
|maybe_block_action_list block_action { 
  $$=chainon($1, $2); 
}
;

block_action_list: block_action_list block_action { $$ = chainon ($1, $2); }
|block_action { $$=$1; }
;
/*The block_body must not be null.*/

block_action: expression ';' { 
  /*bli_add_stmt ($1);*/
}
;

maybe_block_value: { $$=0; }
|block_value { $$=$1; }
;

block_value: expression { $$=$1; }
;

structure_reference:
  ordinary_structure_reference { $$=$1; }
| default_structure_reference { $$=$1; }
| general_structure_reference { $$=$1; }
;

ordinary_structure_reference:
T_NAME '[' access_actual_list ']' { $$=$3; }
;

access_actual_list: access_actual_list ',' access_actual {  }
|access_actual { $$=$1; }
;

segment_name: T_NAME {  }
;

access_actual: { $$=0; }
|expression  { $$=$1; }
| field_name  { $$=$1; }
;

access_part:  
segment_expression ',' access_actual_list {  }
|segment_expression  {  }
;

segment_expression: exp { $$=$1; }
;

/*field_name      Note: See field_attribute, Section 4.1*/

field_name: T_NAME {  }
;

field_set_name: T_NAME {  }
;

dsr1:
primary  { $$=$1; }
| executable_function { $$=$1; }
;

default_structure_reference: dsr1
'[' access_actual_list ']' {  }
;

general_structure_reference:
T_NAME '[' access_part ';' alloc_actual_list ']' { }
|T_NAME '[' access_part ']' { }
;

alloc_actual_list: alloc_actual_list ',' alloc_actual {  }
|alloc_actual { $$=$1; }
;

routine_call: ordinary_routine_call  { $$=$1; }
| general_routine_call { $$=$1; }
;

ordinary_routine_call:
T_NAME '(' io_list3 ')' { 
  void * ref;
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  ref = build_external_ref ($1, 1);
  $$ = build_function_call (ref, $3); 
}
/*nonfin should be routine_designator, but it did not work*/
;

routine_designator:
primary  { $$=$1; }
/*| executable_function { $$=$1; }*/
;

io_list3: { $$=0; }
| io_actual_parameter_list  {  }
| io_actual_parameter_list ';' io_actual_parameter_list  {  }
| ';' io_actual_parameter_list  {  }


actual_parameter_list: actual_parameter_list ',' actual_parameter {  }
|actual_parameter { $$=$1; }
;

io_actual_parameter_list: io_actual_parameter_list ',' io_actual_parameter { chainon ($1, build_tree_list (NULL_TREE, $3)); }
|io_actual_parameter { $$ = build_tree_list (NULL_TREE, $1);  }
;

io_actual_parameter: { $$=0 }
|expression 
;

general_routine_call:
T_NAME '(' routine_address ',' io_list3 ')'{ }
|T_NAME '(' routine_address ')'{ }
;

linkage_name: T_NAME {  }
;


routine_address: expression { $$=$1; }
;

field_reference: address '{' field_selector '}' { $$=$1; }
;

address: 
primary  { $$=$1; }
| executable_function { $$=$1; }
;

field_selector: position_exp ',' size_exp '{' ',' sign_ext_flag '}'
{ $$=$1; }
;

sign_ext_flag: ctce
;

codecomment: K_CODECOMMENT quoted_string_list ':' block
{  }
;

quoted_string_list: quoted_string_list T_STRING {  }
|T_STRING {  }
;
/*
  operator_expression:
  fetch_expression  { $$=$1; }
  | prefix_expression { $$=$1; }
  | infix_expression  { $$=$1; }
  ;
*/
op_exp:
primary  { $$=$1; }
| operator_expression  { $$=$1; }
|executable_function { $$=$1; }
;

fetch_expression:
'.' op_exp12 {  }
;

operator_expression_not: 
op_exp12 { $$=$1; }
;

operator_expression:
'.' opexp9 {  }
/*| '+' opexp9 %prec UMINUS {  $$->id="+"; }
| '-' opexp9 %prec UPLUS {  $$->id="-"; } nonfin*/
| opexp9 '^' opexp9 { }
| opexp9 K_MOD opexp9 { }
| opexp9 '*' opexp9 { }
| opexp9 '/' opexp9 { }
| opexp9 '+' opexp9 { $$ = parser_build_binary_op (PLUS_EXPR, $1, $3); }
| opexp9 '-' opexp9 { $$ = parser_build_binary_op (MINUS_EXPR, $1, $3); }
| opexp9 infix_operator opexp9 { $$ = parser_build_binary_op ($2, $1, $3); }
| K_NOT opexp9 {  }
| opexp9 K_AND opexp9 { }
|  opexp9 K_OR opexp9 { }
| opexp9 K_EQV opexp9 { }
| opexp9 K_XOR  opexp9 { }
| opexp9 '=' opexp9 { $$=build_modify_expr($1, NOP_EXPR, $3);  }
;

opexp9:
primary  { $$=$1; }
| operator_expression  { /*$$=c_expand_expr_stmt($1); abort(); */}
|executable_function { $$=$1; }
;
infix_expression: op_exp infix_operator op_exp { abort(); }
;

/*  bplus  { $$='+'; }
 | bminus  { $$='-'; }
 | '*'  { $$='*'; }
 | '/'  { $$='/'; }
 | K_MOD  { $$=K_MOD; }
 | '^' { $$="^";} 
 |*/
infix_operator:  
K_EQL  { $$=$1; }
| K_EQLA  { $$=$1; }
| K_EQLU { $$=$1; }
| K_NEQ  { $$=$1; }
| K_NEQA  { $$=$1; }
| K_NEQU  { $$=$1; }
| K_LSS  { $$=$1; }
| K_LSSA  { $$=$1; }
| K_LSSU  { $$=$1; }
| K_LEQ { $$=$1; }
| K_LEQA  { $$=$1; }
| K_LEQU  { $$=$1; }
| K_GTR  { $$=$1; }
| K_GTRA  { $$=$1; }
| K_GTRU  { $$=$1; }
| K_GEQ  { $$=$1; }
| K_GEQA { $$=$1; }
| K_GEQU  { $$=$1; }
/* | K_AND  { $$=$1; }
 | K_OR  { $$=$1; }
 | K_EQV  { $$=$1; }
 | K_XOR  { $$=$1; }*/
;


assign_expression: K_IF K_RETURN K_IF
/* op_exp '=' op_exp */
{
  // $$=build_modify_expr($1, '=', $3);
  }
;


op_exp1: op_exp { $$=$1; }
;
op_exp2:  '.' op_exp2 { $$=$2; }
| op_exp { $$=$1; } 
;
op_exp3:/*  '+' op_exp3 { $$=$2; }
				| '-' op_exp3 { $$=$2; }
				|*/ op_exp2 { $$=$1; } 
;
op_exp4:  op_exp4 '^' op_exp3  { $$=$1; }
| op_exp3  { $$=$1; }
;
op_exp5:  op_exp5 K_MOD op_exp4  { $$=$1; }
| op_exp5 '/' op_exp4 { $$=$1; }
| op_exp5 '*' op_exp4  { $$=$1; }
| op_exp4  { $$=$1; }
;
op_exp6:  op_exp6 '+' op_exp5  { $$=$1; }
| op_exp6 '-' op_exp5   {  }
| op_exp5  { $$=$1; }
;
op_exp7:  op_exp7 op7 op_exp6  { $$=$1; }
| op_exp6  { $$=$1; }
;
op7:  K_EQL { $$=$1; }
| K_EQLA { $$=$1; }
| K_EQLU { $$=$1; }
| K_NEQ { $$=$1; }
| K_NEQA { $$=$1; }
| K_NEQU { $$=$1; }
| K_LSS { $$=$1; }
| K_LSSA { $$=$1; }
| K_LSSU { $$=$1; }
| K_LEQ { $$=$1; }
| K_LEQA { $$=$1; }
| K_LEQU { $$=$1; }
| K_GTR { $$=$1; }
| K_GTRA { $$=$1; }
| K_GTRU { $$=$1; }
| K_GEQ { $$=$1; }
| K_GEQA { $$=$1; }
| K_GEQU  { $$=$1; }
;
op_exp8:  K_NOT op_exp8 { $$=$2; }
| op_exp7  { $$=$1; }
;
op_exp9:  op_exp9 K_AND op_exp8  { $$=$1; }
| op_exp8  { $$=$1; }
;
op_exp10:  op_exp10 K_OR op_exp9  { $$=$1; }
| op_exp9  { $$=$1; }
;
op_exp11:  op_exp11 K_EQV op_exp10 { $$=$1; }
| op_exp11 K_XOR op_exp10  { $$=$1; }
| op_exp10  { $$=$1; }
;
op_exp12:  op_exp11 '=' op_exp12  { $$=$1; }
| op_exp11  { $$=$1; }
;
/*
  operator_expression: op_exp12 { $$=$1; }
  ;
*/
executable_function:
executable_function_name '('  actual_parameter_list  ')' {
  /* */
 }
|executable_function_name '(' ')' {  }
;

executable_function_named:
/* standard_function_name  { $$=$1; }
| linkage_function_name { $$=$1; }
| character_handling_function_name { $$=$1; }
| machine_specific_function_name { $$=$1; }
|*/ cond_handling_function_name { $$=$1; }
;


executable_function_name:
T_NAME {  } 
| '%' T_NAME {  }
|executable_function_named { $$=$1; }
;

lexical_function: 
lexical_function_name '(' lexical_actual_parameter ')' {  }
| lexical_function_name expression {  }
| lexical_function_name  {  }
;

lexical_actual_parameter: lexeme_list { $$=$1 ; }
;

lexical_function_name:
P_REMAINING { $$=0; }
/* really %name nonfin*/
;

actual_parameter: expression { $$=$1; }
;
standard_function_name: T_NAME {  }
;
character_handling_function_name: T_NAME {  }
;
machine_specific_function_name: T_NAME {  }
;
cond_handling_function_name:
K_SIGNAL { }
| K_STOP { }
| K_SETUNWIND  { }
;

size_exp: exp { $$=$1; }
;

position_exp: exp { $$=$1; }
;

control_expression:  conditional_expression  { $$=$1; }
| case_expression { $$=$1; }
| select_expression { $$=$1; }
| loop_expression { $$=$1; }
| exit_expression { $$=$1; }
| return_expression  { $$=$1; }
;

conditional_expression: 
/* K_IF exp K_THEN exp  K_ELSE exp ';' { }
| */
K_IF
{
  $<type_node_p>$ = c_begin_if_stmt (); 
}
exp
{
  c_expand_start_cond (truthvalue_conversion ($3), 
		       compstmt_count,$<type_node_p>2);
}
K_THEN exp
{
  c_finish_then ();
  c_expand_end_cond ();
}
/*K_IF exp K_THEN exp  K_ELSE exp {  }
|K_IF exp K_THEN exp  {  }*/
;

exp: expression { $$=$1; }
;

case_expression: K_CASE exp K_FROM ctce K_TO ctce K_OF { $$=$2; }
K_SET case_line_list K_TES { $$=$2; }
;

case_line_list: case_line_list case_line {  }
|case_line { $$=$1; }
;

case_line: '[' case_label_list ']' ':' case_action ';' { $$=$2; }
;

case_label_list: case_label_list ','  case_label {  }
|case_label { $$=$1; }
;

case_label: { $$=0; }
| ctce K_TO ctce { }
| K_INRANGE { $$=0; }
| K_OUTRANGE { $$=0; }
;
case_action: expression { $$=$1; }
;


select_expression: select_type select_index K_OF K_SET select_line_list K_TES
{ $$=$2; }
;
select_type:  K_SELECT | K_SELECTA | K_SELECTU
| K_SELECTONE | K_SELECTONEA | K_SELECTONEU 
;
select_index: expression { $$=$1; }
;

select_line_list: select_line_list select_line {  }
|select_line { $$=$1; }
;

select_line: '[' select_label_list ']' ':' select_action ';' { $$=$2; }
;
select_label_list: select_label_list select_label {  }
|select_label { $$=$1; }
;

select_label:  exp  { $$=$1; }
| exp K_TO exp  { $$=$1; }
| K_OTHERWISE {  }
| K_ALWAYS {  }
;
select_action: expression { $$=$1; }
;


loop_expression:  indexed_loop_expression  {  }
| tested_loop_expression  { $$=$1; }
;
indexed_loop_expression:
indexed_loop_type
{
  $<type_node_p>$ = build_stmt (FOR_STMT, NULL_TREE, NULL_TREE,
			  NULL_TREE, NULL_TREE);
  add_stmt ($<type_node_p>$);  
}
 T_NAME
K_FROM exp   K_TO exp   K_BY exp  K_DO exp { }
;
indexed_loop_type:
K_INCR | K_INCRA | K_INCRU  | K_DECR | K_DECRA | K_DECRU 
;
tested_loop_expression:
pre_tested_loop  { $$=$1; }
| post_tested_loop  { $$=$1; }
;
pre_tested_loop:  
K_WHILE 
{ $<type_node_p>$ = c_begin_while_stmt(); }
exp
{ $3 = truthvalue_conversion ( $3 ); 
 c_finish_while_stmt_cond (truthvalue_conversion ( $3 ),
			   $<type_node_p>2);
 $<type_node_p>$ = add_stmt ($<type_node_p>2); }
 K_DO exp  {
  RECHAIN_STMTS ($<type_node_p>4, WHILE_BODY ($<type_node_p>4));
 } 
| K_UNTIL  exp K_DO exp { }
;
post_tested_loop:
K_DO exp K_WHILE exp  { }
| K_DO exp K_UNTIL  exp { }
;


exit_expression:  leave_expression  { $$=$1; }
| exitloop_expression  { $$=$1; }
;
leave_expression: 
K_LEAVE T_NAME K_WITH exp { } 
|K_LEAVE T_NAME { } 
;
exitloop_expression: 
K_EXITLOOP  exp  { $$=$2; }
|K_EXITLOOP    { $$=0; }
;

return_expression: 
K_RETURN  exp { $$ = c_expand_return (build_compound_expr($2)); }
|K_RETURN { $$ = c_expand_return (NULL_TREE); }
;
/**** 3.0 CONSTANT EXPRESSIONS **************************************/
/**** 4.0 DECLARATIONS **********************************************/
declaration: data_declaration { $$=$1; }
| structure_declaration { $$=$1; }
| field_declaration  { $$=$1; }
| routine_declaration { $$=$1; }
| linkage_declaration  { $$=$1; }
| enable_declaration { $$=$1; }
| bound_declaration  { $$=$1; }
| compiletime_declaration { $$=$1; }
| macro_declaration  { $$=$1; }
| require_declaration { $$=$1; }
| library_declaration  { $$=$1; }
| psect_declaration { $$=$1; }
| switches_declaration  { $$=$1; }
| label_declaration { $$=$1; }
| builtin_declaration  { $$=$1; }
| undeclare_declaration { $$=$1; }
;

attribute_list:attribute_list attribute {  }
|attribute { $$=$1; }
;
attribute:  allocation_unit { $$=$1; }
| extension_attribute { $$=$1; }
| structure_attribute { $$=$1; } 
| field_attribute { $$=$1; }
| alignment_attribute { $$=$1; } 
| initial_attribute { $$=$1; }
| preset_attribute { $$=$1; }
| { undefmode=1; } psect_allocation{ undefmode=0; $$=$2; }
| volatile_attribute { $$=$1; }
| novalue_attribute { $$=$1; }
| linkage_attribute { $$=$1; } 
| range_attribute { $$=$1; }
| { undefmode=1; fprintf(stderr, "undefmode\n\n\n\n"); } addressing_mode_attribute { undefmode=0; $$=$2; } 
| weak_attribute { $$=$1; } 
;
/*
  allocation_unit:  K_LONG | K_WORD | K_BYTE 
  ;
*/
/*extension_attribute:  K_SIGNED | K_UNSIGNED 
;*/
structure_attribute:
  K_REF T_NAME '[' alloc_actual_list ']' { }
|  K_REF T_NAME  { }
|   T_NAME '[' alloc_actual_list ']' { }
|   T_NAME  { }
;

extension_attribute: K_SIGNED { }
|K_UNSIGNED { }
;
/*
  field_attribute: K_FIELD {  }
  ;
*/
alignment_attribute: K_ALIGN  compile_time_constant_expression {  }
;
/*
  preset_attribute: K_PRESET {  }
  ;
*/
/*
psect_allocation: K_PSECT {  }
;
*/

volatile_attribute: K_VOLATILE {  }
;

novalue_attribute: K_NOVALUE {  }
;
linkage_attribute: T_NAME { }
;
range_attribute: K_SIGNED compile_time_constant_expression { }
|K_UNSIGNED compile_time_constant_expression { }
;

ctce: compile_time_constant_expression { $$=$1; }
;

ltce: linkage_time_constant_expression { $$=$1; }
;
linkage_time_constant_expression:expression { $$=$1; /*nonfin*/ }
;

compile_time_constant_expression: expression { $$=$1; /*nonfin*/ }
;

addressing_mode_attribute: 
/*K_ADDRESSING_MODE { undefmode=1; } '(' mode_16 ')' {  }
  |*/{ undefmode=1; fprintf(stderr, "undefmode\n\n\n\n"); } K_ADDRESSING_MODE '('  mode_32 ')' {  undefmode=0; }
;

mode_16:
  U_ABSOLUTE { }
|U_RELATIVE { }
;

mode_32:
U_ABSOLUTE { }
|U_GENERAL { }
|U_LONG_RELATIVE { }
|U_WORD_RELATIVE { }
;

weak_attribute: K_WEAK {  }
;

/*
alloc_actual_list: alloc_actual_list alloc_actual|alloc_actual;
*/


alloc_actual:  { $$=(int) 0; }
| ctce { $$=$1; }
|allocation_unit { $$=$1; }
|extension_attribute { $$=$1; }
;

field_attribute: K_FIELD field_stuff_list {  }
;

field_stuff_list: field_stuff_list ','  field_name {  }
| field_stuff_list ','  field_set_name{  }
| field_set_name { $$=$1; }
| field_name { $$=$1; }
;

initial_attribute: K_INITIAL '(' initial_item_list ')' {  }
;

initial_item_list: initial_item_list ',' initial_item {  }
|initial_item { $$=$1; }
;

initial_item: 
initial_group  { $$=$1; }
| initial_expression { $$=$1; }
| initial_string { $$=$1; }
;

initial_group: 
allocation_unit '(' initial_item_list ')' { }
| K_REP replicator K_OF '(' initial_item_list ')'  {  }
| K_REP replicator K_OF allocation_unit '(' initial_item_list ')'  { }
/*| '(' initial_item_list ')' {  }*/
;

/*
replicator: ctce { $$=$1; }
;
*/

initial_expression: expression { $$=$1; }
;

/*
An initial_expression must be an ltce for OWN and GLOBAL declarations.
*/

initial_string: string_literal { $$=$1; }
;

preset_item_list: preset_item_list ',' preset_item|preset_item;

preset_attribute: K_PRESET '(' preset_item_list ')' {  }
;

preset_item:  '[' ctce_access_actual_list ']' '=' preset_value {  }
;

ctce_access_actual_list: ctce_access_actual_list ',' ctce_access_actual|ctce_access_actual;

ctce_access_actual: 
ctce { $$=$1; }
| field_name { $$=$1; }
;

preset_value: expression { $$=$1; }
;

psect_allocation: K_PSECT '(' T_NAME ')' { }
;
/*
  volatile_attribute: K_VOLATILE
  ;
*/

data_declaration:  own_declaration {
  $$ = $1;
}
| global_declaration { $$=$1; }
| external_declaration  { $$=$1; }
| forward_declaration { $$=$1; }
| local_declaration  { $$=$1; }
| stackglocal_declaration { $$=$1; }
| register_declaration { $$=$1; }
| global_register_declaration { $$=$1; }
| external_register_declaration { $$=$1; }
| map_declaration  { $$=$1; }
;

/*
own_declaration_list: own_declaration_list own_declaration {  }
|own_declaration;
*/

own_declaration: K_OWN own_item_list ';' {
  $$ = $2;
}
;

own_item_list: own_item_list ',' own_item {
  //$$ = chainon($1, $3);
  /* */
 }
|declspecs_ts setspecs own_item { 
  //  $$ = $1;
}
;

own_item: T_NAME { 
  //$$ = build_decl (VAR_DECL, $1, integer_type_node);
  TREE_TYPE($1)=integer_type_node;
  tree d = start_decl ($1, current_declspecs, 0,
		       chainon (NULL_TREE, all_prefix_attributes));
 finish_decl (d, NULL_TREE, NULL_TREE);
  // $$ = build_tree_list ($1, NULL_TREE);
  /*  */
  
}
|T_NAME ':' own_attribute_list {
   
  
}
;
own_attribute_list:
own_attribute_list own_attribute {  }
|own_attribute { $$=$1; }
;

own_attribute:
allocation_unit { $$=$1; }
|extension_attribute { $$=$1; }
|structure_attribute { $$=$1; }
|field_attribute { $$=$1; }
|alignment_attribute { $$=$1; }
|initial_attribute { $$=$1; }
|preset_attribute { $$=$1; }
|psect_allocation { $$=$1; }
|volatile_attribute { $$=$1; }
;

global_declaration: K_GLOBAL global_item_list  ';' {  }
;

global_item_list:  global_item_list global_item {  }
|global_item { $$=$1; }
;

global_item: T_NAME ':' attribute_list { }
;

external_declaration: K_EXTERNAL external_item_list ';' {  }
;

external_item_list: external_item_list external_item {  }
|external_item { $$=$1; }
;

external_item: T_NAME ':' attribute_list {  }
;

forward_declaration: K_FORWARD forward_item_list ';'  { }
;

forward_item: T_NAME ':' attribute_list {  }
;

forward_item_list: forward_item_list forward_item {  }
|forward_item { $$=$1; }
;

local_declaration: K_LOCAL local_item_list ';' { } ;

local_item_list: local_item_list ',' local_item {  }
|local_item { $$=$1; }
;

local_item: 
T_NAME ':' attribute_list { }
|T_NAME { }
;

stackglocal_declaration: K_STACKLOCAL local_item_list ';' { }
;

register_declaration: K_REGISTER register_item_list ';' { }
;

register_item_list: register_item_list register_item {  }
|register_item { $$=$1; }
;

register_item: T_NAME '=' ctce ':' attribute_list {  }
;

global_register_declaration:
K_GLOBAL K_REGISTER global_reg_item_list ';' {  }
;

global_reg_item_list: global_reg_item_list global_reg_item {  }
|global_reg_item { $$=$1; }
;

global_reg_item: T_NAME '=' ctce attribute_list {  }
;

external_register_declaration:
K_EXTERNAL K_REGISTER external_reg_item_list ';' {  }
;

external_reg_item_list:external_reg_item_list external_reg_item {  }
|external_reg_item { $$=$1; }
;

external_reg_item: T_NAME '=' ctce ':' attribute_list {  }
;

map_declaration: K_MAP map_item_list ';' {  }
;

map_item_list: map_item_list ',' map_item {  }
|map_item { $$=$1; }
;

map_item: T_NAME ':' attribute_list { }
;

structure_declaration:
  K_STRUCTURE structure_definition_list ';' {  }
;

structure_definition_list: structure_definition_list structure_definition {  }
|structure_definition { $$=$1; }
;

structure_definition:
  T_NAME 
  access_formal_list  allocation_formal_list 
  structure_size  structure_body
{ }
/* nonfin */
;

allocation_formal_list: allocation_formal_list allocation_formal {  }
|allocation_formal  { $$=$1; }
;

allocation_formal:
allocation_name '{' '=' allocation_default '}' {  }
;

allocation_default: exp { $$=$1; }
;

structure_size: expression { $$=$1; }
;

structure_body: expression { $$=$1; }
;

access_formal_list: access_formal_list access_formal {  }
| access_formal_list  { $$=$1; }
;

access_formal: T_NAME {  }
;

allocation_name: T_NAME {  }
;

field_declaration: K_FIELD
{  } 
;

routine_declaration:
ordinary_routine_declaration { $$=$1; }
|global_routine_declaration { $$=$1; }
|forward_routine_declaration { $$=$1; }
|external_routine_declaration { $$=$1; }
;
ordinary_routine_declaration: 
K_ROUTINE routine_definition_list ';' { 
  //
  $$ = $2;
}
;

routine_definition_list: routine_definition_list ',' routine_definition {  }
|routine_definition { $$=$1; }
;

routine_definition: declspecs_ts setspecs T_NAME io_list ':'
{
  //bli_start_function (current_declspecs, $2, all_prefix_attributes);
}
 routine_attribute_list '=' 
{
  //store_parm_decls ();
}
save_filename save_lineno exp 
{ /*  $$->middle=$4; $$->id=$1;  */
  //DECL_SOURCE_FILE (current_function_decl) = $6;
  //DECL_SOURCE_LINE (current_function_decl) = $7;
  //bli_finish_function (0, 1); 
  //POP_DECLSPEC_STACK;
}
|declspecs_ts setspecs T_NAME 
{
  //current_function_decl=$3;
  //void * ref = build_external_ref ($3, 1);
  void * v = build_tree_list (NULL_TREE, NULL_TREE);
  void * vo = build_nt (CALL_EXPR, $3, v, NULL_TREE);
  start_function (current_declspecs, vo, all_prefix_attributes);
}
io_list 
{
  store_parm_decls ();
}
'=' save_filename save_lineno exp 
{ 
  /* $$->id=$1;  */
  //DECL_SOURCE_FILE (current_function_decl) = $6;
  //DECL_SOURCE_LINE (current_function_decl) = $7;
  //$$=chainon($3,$10);
  //  DECL_SAVED_TREE (current_function_decl) = $$;
  finish_function (0, 1); 
  POP_DECLSPEC_STACK;
  //tree decl=$3;
  //DECL_NAME(decl)=$3;
  //$$=decl;
}
;

setspecs: /* empty */
{ pending_xref_error ();
 PUSH_DECLSPEC_STACK;
 split_specs_attrs ($<type_node_p>0,
		    &current_declspecs, &prefix_attributes);
 all_prefix_attributes = prefix_attributes; }
;

declspecs_ts:
{
  $$ = tree_cons (NULL_TREE, NULL_TREE, NULL_TREE);
  TREE_STATIC ($$) = 1;
}

io_list2: 
io_list ':' routine_attribute_list {  }
|io_list {  }
;

io_list: { $$=0; }
|'(' formal_item_list ')' {  }
|'(' formal_item_list ';' formal_item_list ')' {  }
|'(' ';' formal_item_list ')' {  }
;

formal_item_list:
/* empty */ { $$ = 0 ; } 
|formal_item_list ','  formal_item
|declspecs_ts setspecs formal_item  
;

formal_item: T_NAME ':' formal_attribute_list 
|T_NAME { tree d = start_decl ($1, current_declspecs, 0,
			       chainon (NULL_TREE, all_prefix_attributes));
 finish_decl (d, NULL_TREE, NULL_TREE); 
}
;
routine_attribute_list:
routine_attribute_list routine_attribute {  }
|routine_attribute { $$=$1; }
;
formal_attribute_list:  map_declaration_attribute_list  { $$=$1; }
;
/*
  ?? nonfin
  formal_attribute_list: '(' map_declaration_attribute_list ')' { $$=$2; }
  ;
*/
map_declaration_attribute_list: map_declaration_attribute_list map_declaration_attribute {  }
|map_declaration_attribute { $$=$1; }
;

map_declaration_attribute:
allocation_unit { $$=$1; }
|extension_attribute { $$=$1; }
|structure_attribute { $$=$1; }
|field_attribute { $$=$1; }
|volatile_attribute { $$=$1; }
;
routine_attribute: novalue_attribute { $$=$1; }
|linkage_attribute { $$=$1; }
|psect_allocation { $$=$1; }
|addressing_mode_attribute { $$=$1; }
|weak_attribute { $$=$1; }
;

global_routine_declaration:
K_GLOBAL K_ROUTINE global_routine_definition_list ';' {  }
;
global_routine_definition_list: global_routine_definition_list ',' global_routine_definition {  }
|global_routine_definition { $$=$1; }
;
global_routine_definition: T_NAME io_list ':' global_routine_attribute_list '=' exp { }
|T_NAME io_list '=' exp { }
;

global_io_list2: 
io_list ':'  global_routine_attribute_list {  }
|io_list  {  }
;

global_routine_attribute_list: { $$=0; }
|global_routine_attribute_list global_routine_attribute {  }
|global_routine_attribute { $$=$1; }
;
global_routine_attribute: novalue_attribute { $$=$1; }
|linkage_attribute { $$=$1; }
|psect_allocation { $$=$1; }
|addressing_mode_attribute { $$=$1; }
|weak_attribute { $$=$1; }
;

forward_routine_declaration:
K_FORWARD K_ROUTINE forward_routine_item_list ';' {  }
;

forward_routine_item_list: forward_routine_item_list ',' forward_routine_item {  }
|forward_routine_item  { $$=$1; }
;
forward_routine_item: 
T_NAME maybe_forward_routine_attribute_list { }
/*|T_NAME  {  }*/
;
maybe_forward_routine_attribute_list: { $$=0; }
|':' forward_routine_attribute_list { $$=$2; }
;
forward_routine_attribute_list:
forward_routine_attribute_list forward_routine_attribute {  }
|forward_routine_attribute { $$=$1; }
;
forward_routine_attribute: novalue_attribute { $$=$1; }
|linkage_attribute { $$=$1; }
|psect_allocation { $$=$1; }
|addressing_mode_attribute { $$=$1; }
;
external_routine_declaration:
K_EXTERNAL K_ROUTINE external_routine_item_list ';' { }
;

external_routine_item_list: external_routine_item_list ',' external_routine_item { }
|external_routine_item { $$=$1; }
;
external_routine_item: T_NAME ':' ext_routine_attribute_list { }
| T_NAME { 
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  $$ = build_external_ref ($1, 1);
}
;
ext_routine_attribute_list:
ext_routine_attribute_list ext_routine_attribute {  }
|ext_routine_attribute { $$=$1; }
;
ext_routine_attribute: novalue_attribute { $$=$1; }
|linkage_attribute { $$=$1; }
|psect_allocation { $$=$1; }
|addressing_mode_attribute { $$=$1; }
|weak_attribute { $$=$1; }
;
linkage_declaration: K_LINKAGE linkage_definition_list ';' {  }
;
enable_declaration: K_ENABLE T_NAME '(' tname_list ')' ';' { }
|K_ENABLE T_NAME ';' { }
;
bound_declaration: 
literal_declaration { $$= $1; }
|external_literal_declaration { $$= $1; }
|bind_data_declaration { $$= $1; }
|bind_routine_declaration { $$= $1; }
;
compiletime_declaration: K_COMPILETIME
{  } ;
macro_declaration: 
positional_macro_declaration { $$=$1; }
|keyword_macro_declaration { $$=$1; }
;
require_declaration: K_REQUIRE T_STRING ';'
{ } ;
library_declaration: K_LIBRARY T_STRING ';'
{ } ;
psect_declaration: K_PSECT psect_item_list ';'
{  } ;
switches_declaration: K_SWITCHES
{  } ;
label_declaration: K_LABEL
{  } ;
builtin_declaration: K_BUILTIN
{  } ;
undeclare_declaration: K_UNDECLARE tname_list ';'
{  } ;

literal_declaration: K_LITERAL literal_item_list ';' {  }
| K_GLOBAL K_LITERAL literal_item_list{  }
;

literal_item_list: literal_item_list ',' literal_item {  }
| literal_item { $$=$1; }
;

literal_item: T_NAME '=' compile_time_constant_expression ':' literal_attribute_list { }
|T_NAME '=' compile_time_constant_expression { }
;

literal_attribute_list:
literal_attribute_list literal_attribute {  }
|literal_attribute { $$=$1; }
;

literal_attribute: weak_attribute { $$=$1; }
|range_attribute { $$=$1; }
;
external_literal_declaration: K_EXTERNAL K_LITERAL external_literal_item_list ';' {  }
;

external_literal_item_list: external_literal_item_list ',' external_literal_item {  }
| external_literal_item { $$=$1; }
;

external_literal_item: T_NAME  ':' literal_attribute_list { }
|T_NAME  { }
;

bind_data_declaration: K_BIND bind_data_item_list ';' {  }
| K_GLOBAL K_BIND bind_data_item_list{  }
;

bind_data_item_list: bind_data_item_list ',' bind_data_item {  }
| bind_data_item { $$=$1; }
;

bind_data_item: T_NAME '=' expression ':' bind_data_attribute_list { }
|T_NAME '=' expression { }
;

bind_data_attribute_list:
bind_data_attribute_list bind_data_attribute {  }
|bind_data_attribute { $$=$1; }

bind_data_attribute:
allocation_unit { $$=$1; }
|extension_attribute { $$=$1; }
|structure_attribute { $$=$1; }
|field_attribute { $$=$1; }
|volatile_attribute { $$=$1; }
|weak_attribute { $$=$1; }
;

bind_routine_declaration: K_BIND K_ROUTINE bind_routine_item_list ';' {  }
| K_GLOBAL K_BIND K_ROUTINE bind_routine_item_list{  }
;

bind_routine_item_list: bind_routine_item_list ',' bind_routine_item {  }
| bind_routine_item { $$=$1; }
;

bind_routine_item: T_NAME '=' expression ':' bind_routine_attribute_list { }
|T_NAME '=' expression { }
;

bind_routine_attribute_list:
bind_routine_attribute_list bind_routine_attribute {  }
|bind_routine_attribute { $$=$1; }

bind_routine_attribute:
linkage_attribute { $$=$1; }
|novalue_attribute { $$=$1; }
|weak_attribute { $$=$1; }
;

positional_macro_declaration: K_MACRO positional_macro_definition_list ';' {  }
;

positional_macro_definition_list: positional_macro_definition_list ',' positional_macro_definition {  }
|positional_macro_definition { $$=$1; }
;

positional_macro_definition:
simple_macro_definition { $$=$1; }
|conditional_macro_definition { $$=$1; }
|iterative_macro_definition { $$=$1; }

simple_macro_definition: 
T_NAME '(' tname_list ')'  '=' macro_body '%' { }
|T_NAME  '=' macro_body '%'{ }
;
conditional_macro_definition:
T_NAME '(' tname_list ')'  '[' ']' '=' macro_body '%' { }
|T_NAME '[' ']' '=' macro_body '%'{ }
;
iterative_macro_definition:
T_NAME '(' tname_list ')' '[' tname_list ']' '=' macro_body '%' { }
|T_NAME '[' tname_list ']' '=' macro_body '%'{ }
;

default_actual: { $$=0; }
| lexeme_list { $$=$1; }
;

macro_body: { $$=0; }
| { tnamemode=0; } lexeme_list { tnamemode=0; $$=$2; }
;
/*| '<' { anymode=1; } lexeme_list { anymode=0; } '>' { $$=$3; }*/


lexeme_list: expression { $$=$1; /*nonfin*/ }
;
/*
  lexeme_list:  tname_list2 { $$=$1; *nonfin* }
  ;
*/
keyword_macro_declaration: K_KEYWORDMACRO keyword_macro_definition_list ';' {  }
;

keyword_macro_definition_list:
keyword_macro_definition_list ',' keyword_macro_definition {  }
|keyword_macro_definition { $$=$1; }
;

keyword_macro_definition:
T_NAME '(' keyword_pair_list ')' '=' macro_body '%' { }
;

keyword_pair_list: keyword_pair_list ',' keyword_pair {  }
|keyword_pair { $$=$1; }
;

keyword_pair:
T_NAME '=' default_actual { }
|T_NAME { }
;

linkage_definition_list: linkage_definition_list ',' linkage_definition {  }
|linkage_definition { $$=$1; }
;
linkage_definition: T_NAME '=' linkage_type {  }
;

linkage_type: U_CALL {  }
| T_NAME {  }
;

input_parameter_location_list:input_parameter_location_list ',' input_parameter_location {  }
|input_parameter_location { $$=$1;}
;
output_parameter_location_list:output_parameter_location_list ',' output_parameter_location {  }
|output_parameter_location { $$=$1;}
;

input_parameter_location: U_STANDARD {  }
|K_REGISTER '=' T_DIGITS { }
| { $$=0; }
;

output_parameter_location:
'(' K_REGISTER '=' T_DIGITS ')' { }
;

psect_item_list: psect_item_list ',' psect_item {  }
|psect_item { $$=$1; }
;

psect_attribute_list: psect_attribute_list ',' psect_attribute {  }
|psect_attribute { $$=$1; }
;

psect_item:
storage_class '=' T_NAME '(' psect_attribute_list ')' { }
|storage_class '=' T_NAME { }
;

storage_class:
K_OWN { }
|K_GLOBAL { }
|K_PLIT { }
|U_CODE { }
|U_NODEFAULT { }
;

psect_attribute:
U_WRITE  { }
| U_NOWRITE { }
|U_EXECUTE { }
| U_NOEXECUTE { }
|U_OVERLAY { }
| U_CONCATENATE { }
|  b32_psect_attribute { $$=$1; }
;

b32_psect_attribute:
U_READ { }
|U_NOREAD { }
|U_SHARE { }
|U_NOSHARE { }
|U_PIC { }
|U_NOPIC { }
|K_LOCAL { }
|K_GLOBAL { }
|U_VECTOR { }
|alignment_attribute { $$=$1; }
|addressing_mode_attribute { $$=$1; }
;


/**** 5.0 LEXICAL PROCESSING FACILITIES *****************************/
/**** 7.0 MACHINE SPECIFIC NAMES ************************************/

builtin_name: T_NAME  {  }
| machine_specific_function  {  }
| linkage_function {  }
;

machine_specific_function: T_NAME
;

linkage_function: T_NAME
;

linkage_function_name: T_NAME {  }
;
/*
  test		: tok
  | test tok
  ;

  tok		: '='
  { fprintf (stderr, " '=' \n"); }
  | ':'
  { fprintf (stderr, " ':' \n"); }
  | ','
  { fprintf (stderr, " ',' \n"); }
  | '/'
  { fprintf (stderr, " '/' \n"); }
  | '.'
  { fprintf (stderr, " '.' \n"); }
  | '<'
  { fprintf (stderr, " '<' \n"); }
  | '['
  { fprintf (stderr, " '[' \n"); }
  | '('
  { fprintf (stderr, " '(' \n"); }
  | '_'
  { fprintf (stderr, " '_' \n"); }
  | '*'
  { fprintf (stderr, " '*' \n"); }
  | '+'
  { fprintf (stderr, " '+' \n"); }
  | '^'
  { fprintf (stderr, " '^' \n"); }
  | '>'
  { fprintf (stderr, " '>' \n"); }
  | ']'
  { fprintf (stderr, " ']' \n"); }
  | ')'
  { fprintf (stderr, " ')' \n"); }
  | ';'
  { fprintf (stderr, " ';' \n"); }
  | '%'
  { fprintf (stderr, " '%' \n"); }
  | T_DIGITS
  { fprintf (stderr, "Integer constant \n"); }
  | T_NAME
  { fprintf (stderr, "Name \n"); }
  | T_STRING
  { fprintf (stderr, "Quoted string \n"); }
  | K_ADDRESSING_MODE
  { fprintf (stderr, "ADDRESSING_MODE \n"); }
  | K_ALIGN
  { fprintf (stderr, "ALIGN \n"); }
  | K_ALWAYS
  { fprintf (stderr, "ALWAYS \n"); }
  | K_AND
  { fprintf (stderr, "AND \n"); }
  | K_BEGIN
  { fprintf (stderr, "BEGIN \n"); }
  | K_BIND
  { fprintf (stderr, "BIND \n"); }
  | K_BIT
  { fprintf (stderr, "BIT \n"); }
  | K_BUILTIN
  { fprintf (stderr, "BUILTIN \n"); }
  | K_BY
  { fprintf (stderr, "BY \n"); }
  | K_BYTE
  { fprintf (stderr, "BYTE \n"); }
  | K_CASE
  { fprintf (stderr, "CASE \n"); }
  | K_CODECOMMENT
  { fprintf (stderr, "CODECOMMENT \n"); }
  | K_COMPILETIME
  { fprintf (stderr, "COMPILETIME \n"); }
  | K_DECR
  { fprintf (stderr, "DECR \n"); }
  | K_DECRA
  { fprintf (stderr, "DECRA \n"); }
  | K_DECRU
  { fprintf (stderr, "DECRU \n"); }
  | K_DO
  { fprintf (stderr, "DO \n"); }
  | K_ELSE
  { fprintf (stderr, "ELSE \n"); }
  | K_ELUDOM
  { fprintf (stderr, "ELUDOM \n"); }
  | K_ENABLE
  { fprintf (stderr, "ENABLE \n"); }
  | K_END
  { fprintf (stderr, "END \n"); }
  | K_EQL
  { fprintf (stderr, "EQL \n"); }
  | K_EQLA
  { fprintf (stderr, "EQLA \n"); }
  | K_EQLU
  { fprintf (stderr, "EQLU \n"); }
  | K_EQV
  { fprintf (stderr, "EQV \n"); }
  | K_EXITLOOP
  { fprintf (stderr, "EXITLOOP \n"); }
  | K_EXTERNAL
  { fprintf (stderr, "EXTERNAL \n"); }
  | K_FIELD
  { fprintf (stderr, "FIELD \n"); }
  | K_FORWARD
  { fprintf (stderr, "FORWARD \n"); }
  | K_FROM
  { fprintf (stderr, "FROM \n"); }
  | K_GEQ
  { fprintf (stderr, "GEQ \n"); }
  | K_GEQA
  { fprintf (stderr, "GEQA \n"); }
  | K_GEQU
  { fprintf (stderr, "GEQU \n"); }
  | K_GLOBAL
  { fprintf (stderr, "GLOBAL \n"); }
  | K_GTR
  { fprintf (stderr, "GTR \n"); }
  | K_GTRA
  { fprintf (stderr, "GTRA \n"); }
  | K_GTRU
  { fprintf (stderr, "GTRU \n"); }
  | K_IF
  { fprintf (stderr, "IF \n"); }
  | K_INCR
  { fprintf (stderr, "INCR \n"); }
  | K_INCRA
  { fprintf (stderr, "INCRA \n"); }
  | K_INCRU
  { fprintf (stderr, "INCRU \n"); }
  | K_INITIAL
  { fprintf (stderr, "INITIAL \n"); }
  | K_INRANGE
  { fprintf (stderr, "INRANGE \n"); }
  | K_IOPAGE
  { fprintf (stderr, "IOPAGE \n"); }
  | K_KEYWORDMACRO
  { fprintf (stderr, "KEYWORDMACRO \n"); }
  | K_LABEL
  { fprintf (stderr, "LABEL \n"); }
  | K_LEAVE
  { fprintf (stderr, "LEAVE \n"); }
  | K_LEQ
  { fprintf (stderr, "LEQ \n"); }
  | K_LEQA
  { fprintf (stderr, "LEQA \n"); }
  | K_LEQU
  { fprintf (stderr, "LEQU \n"); }
  | K_LIBRARY
  { fprintf (stderr, "LIBRARY \n"); }
  | K_LINKAGE
  { fprintf (stderr, "LINKAGE \n"); }
  | K_LITERAL
  { fprintf (stderr, "LITERAL \n"); }
  | K_LOCAL
  { fprintf (stderr, "LOCAL \n"); }
  | K_LONG
  { fprintf (stderr, "LONG \n"); }
  | K_LSS
  { fprintf (stderr, "LSS \n"); }
  | K_LSSA
  { fprintf (stderr, "LSSA \n"); }
  | K_LSSU
  { fprintf (stderr, "LSSU \n"); }
  | K_MACRO
  { fprintf (stderr, "MACRO \n"); }
  | K_MAP
  { fprintf (stderr, "MAP \n"); }
  | K_MOD
  { fprintf (stderr, "MOD \n"); }
  | K_MODULE
  { fprintf (stderr, "MODULE \n"); }
  | K_NEQ
  { fprintf (stderr, "NEQ \n"); }
  | K_NEQA
  { fprintf (stderr, "NEQA \n"); }
  | K_NEQU
  { fprintf (stderr, "NEQU \n"); }
  | K_NOT
  { fprintf (stderr, "NOT \n"); }
  | K_NOVALUE
  { fprintf (stderr, "NOVALUE \n"); }
  | K_OF
  { fprintf (stderr, "OF \n"); }
  | K_OR
  { fprintf (stderr, "OR \n"); }
  | K_OTHERWISE
  { fprintf (stderr, "OTHERWISE \n"); }
  | K_OUTRANGE
  { fprintf (stderr, "OUTRANGE \n"); }
  | K_OWN
  { fprintf (stderr, "OWN \n"); }
  | K_PLIT
  { fprintf (stderr, "PLIT \n"); }
  | K_PRESET
  { fprintf (stderr, "PRESET \n"); }
  | K_PSECT
  { fprintf (stderr, "PSECT \n"); }
  | K_RECORD
  { fprintf (stderr, "RECORD \n"); }
  | K_REF
  { fprintf (stderr, "REF \n"); }
  | K_REGISTER
  { fprintf (stderr, "REGISTER \n"); }
  | K_REP
  { fprintf (stderr, "REP \n"); }
  | K_REQUIRE
  { fprintf (stderr, "REQUIRE \n"); }
  | K_RETURN
  { fprintf (stderr, "RETURN \n"); }
  | K_ROUTINE
  { fprintf (stderr, "ROUTINE \n"); }
  | K_SELECT
  { fprintf (stderr, "SELECT \n"); }
  | K_SELECTA
  { fprintf (stderr, "SELECTA \n"); }
  | K_SELECTONE
  { fprintf (stderr, "SELECTONE \n"); }
  | K_SELECTONEA
  { fprintf (stderr, "SELECTONEA \n"); }
  | K_SELECTONEU
  { fprintf (stderr, "SELECTONEU \n"); }
  | K_SELECTU
  { fprintf (stderr, "SELECTU \n"); }
  | K_SET
  { fprintf (stderr, "SET \n"); }
  | K_SHOW
  { fprintf (stderr, "SHOW \n"); }
  | K_SIGNED
  { fprintf (stderr, "SIGNED \n"); }
  | K_STACKLOCAL
  { fprintf (stderr, "STACKLOCAL \n"); }
  | K_STRUCTURE
  { fprintf (stderr, "STRUCTURE \n"); }
  | K_SWITCHES
  { fprintf (stderr, "SWITCHES \n"); }
  | K_TES
  { fprintf (stderr, "TES \n"); }
  | K_THEN
  { fprintf (stderr, "THEN \n"); }
  | K_TO
  { fprintf (stderr, "TO \n"); }
  | K_UNDECLARE
  { fprintf (stderr, "UNDECLARE \n"); }
  | K_UNSIGNED
  { fprintf (stderr, "UNSIGNED \n"); }
  | K_UNTIL
  { fprintf (stderr, "UNTIL \n"); }
  | K_UPLIT
  { fprintf (stderr, "UPLIT \n"); }
  | K_VOLATILE
  { fprintf (stderr, "VOLATILE \n"); }
  | K_WEAK
  { fprintf (stderr, "WEAK \n"); }
  | K_WHILE
  { fprintf (stderr, "WHILE \n"); }
  | K_WITH
  { fprintf (stderr, "WITH \n"); }
  | K_WORD
  { fprintf (stderr, "WORD \n"); }
  | K_XOR
  { fprintf (stderr, "XOR \n"); }
  | P_ALLOCATION
  { fprintf (stderr, "%%ALLOCATION \n"); }
  | P_ASCIC
  { fprintf (stderr, "%%ASCIC \n"); }
  | P_ASCID
  { fprintf (stderr, "%%ASCID \n"); }
  | P_ASCII
  { fprintf (stderr, "%%ASCII \n"); }
  | P_ASCIZ
  { fprintf (stderr, "%%ASCIZ \n"); }
  | P_ASSIGN
  { fprintf (stderr, "%%ASSIGN \n"); }
  | P_B
  { fprintf (stderr, "%%B \n"); }
  | P_BLISS
  { fprintf (stderr, "%%BLISS \n"); }
  | P_BLISS16
  { fprintf (stderr, "%%BLISS16 \n"); }
  | P_BLISS32
  { fprintf (stderr, "%%BLISS32 \n"); }
  | P_BLISS36
  { fprintf (stderr, "%%BLISS36 \n"); }
  | P_BPADDR
  { fprintf (stderr, "%%BPADDR \n"); }
  | P_BPUNIT
  { fprintf (stderr, "%%BPUNIT \n"); }
  | P_BPVAL
  { fprintf (stderr, "%%BPVAL \n"); }
  | P_C
  { fprintf (stderr, "%%C \n"); }
  | P_CHAR
  { fprintf (stderr, "%%CHAR \n"); }
  | P_CHARCOUNT
  { fprintf (stderr, "%%CHARCOUNT \n"); }
  | P_CHESC
  { fprintf (stderr, "%%CHESC \n"); }
  | P_COUNT
  { fprintf (stderr, "%%COUNT \n"); }
  | P_CTCE
  { fprintf (stderr, "%%CTCE \n"); }
  | P_D
  { fprintf (stderr, "%%D \n"); }
  | P_DECIMAL
  { fprintf (stderr, "%%DECIMAL \n"); }
  | P_DECLARED
  { fprintf (stderr, "%%DECLARED \n"); }
  | P_E
  { fprintf (stderr, "%%E \n"); }
  | P_ELSE
  { fprintf (stderr, "%%ELSE \n"); }
  | P_ERROR
  { fprintf (stderr, "%%ERROR \n"); }
  | P_ERRORMACRO
  { fprintf (stderr, "%%ERRORMACRO \n"); }
  | P_EXACTSTRING
  { fprintf (stderr, "%%EXACTSTRING \n"); }
  | P_EXITITERATION
  { fprintf (stderr, "%%EXITITERATION \n"); }
  | P_EXITMACRO
  { fprintf (stderr, "%%EXITMACRO \n"); }
  | P_EXPAND
  { fprintf (stderr, "%%EXPAND \n"); }
  | P_EXPLODE
  { fprintf (stderr, "%%EXPLODE \n"); }
  | P_FI
  { fprintf (stderr, "%%FI \n"); }
  | P_FIELDEXPAND
  { fprintf (stderr, "%%FIELDEXPAND \n"); }
  | P_G
  { fprintf (stderr, "%%G \n"); }
  | P_H
  { fprintf (stderr, "%%H \n"); }
  | P_IDENTICAL
  { fprintf (stderr, "%%IDENTICAL \n"); }
  | P_IF
  { fprintf (stderr, "%%IF \n"); }
  | P_INFORM
  { fprintf (stderr, "%%INFORM \n"); }
  | P_ISSTRING
  { fprintf (stderr, "%%ISSTRING \n"); }
  | P_LENGTH
  { fprintf (stderr, "%%LENGTH \n"); }
  | P_LTCE
  { fprintf (stderr, "%%LTCE \n"); }
  | P_MESSAGE
  { fprintf (stderr, "%%MESSAGE \n"); }
  | P_NAME
  { fprintf (stderr, "%%NAME \n"); }
  | P_NBITS
  { fprintf (stderr, "%%NBITS \n"); }
  | P_NBITSU
  { fprintf (stderr, "%%NBITSU \n"); }
  | P_NULL
  { fprintf (stderr, "%%NULL \n"); }
  | P_NUMBER
  { fprintf (stderr, "%%NUMBER \n"); }
  | P_O
  { fprintf (stderr, "%%O \n"); }
  | P_P
  { fprintf (stderr, "%%P \n"); }
  | P_PRINT
  { fprintf (stderr, "%%PRINT \n"); }
  | P_QUOTE
  { fprintf (stderr, "%%QUOTE \n"); }
  | P_QUOTENAME
  { fprintf (stderr, "%%QUOTENAME \n"); }
  | P_RAD50_10
  { fprintf (stderr, "%%RAD50_10 \n"); }
  | P_RAD50_11
  { fprintf (stderr, "%%RAD50_11 \n"); }
  | P_REF
  { fprintf (stderr, "%%REF \n"); }
  | P_REMAINING
  { fprintf (stderr, "%%REMAINING \n"); }
  | P_REMOVE
  { fprintf (stderr, "%%REMOVE \n"); }
  | P_REQUIRE
  { fprintf (stderr, "%%REQUIRE \n"); }
  | P_SBTTL
  { fprintf (stderr, "%%SBTTL \n"); }
  | P_SIXBIT
  { fprintf (stderr, "%%SIXBIT \n"); }
  | P_SIZE
  { fprintf (stderr, "%%SIZE \n"); }
  | P_STRING
  { fprintf (stderr, "%%STRING \n"); }
  | P_SWITCHES
  { fprintf (stderr, "%%SWITCHES \n"); }
  | P_THE
  { fprintf (stderr, "%%THE \n"); }
  | P_TITLE
  { fprintf (stderr, "%%TITLE \n"); }
  | P_UNQUOTE
  { fprintf (stderr, "%%UNQUOTE \n"); }
  | P_UPVAL
  { fprintf (stderr, "%%UPVAL \n"); }
  | P_VARIANT
  { fprintf (stderr, "%%VARIANT \n"); }
  | P_WARN
  { fprintf (stderr, "%%WARN \n"); }
  | P_X
  { fprintf (stderr, "%%X \n"); }
  ;
		*/

pushlevel:  /* empty */
{ pushlevel (0);
 clear_last_expr ();
 add_scope_stmt (/*begin_p=*/1, /*partial_p=*/0);
}
;

poplevel:  /* empty */
{ 
  $$ = add_scope_stmt (/*begin_p=*/0, /*partial_p=*/0);
 }
;

%%

#define NOYTABH
//#include "lex.yy.c"

/*
support
functions
here
*/



yyerror (char *s)
{
  if (s)fprintf( stderr,"\n\n%s\n",s); 
  fprintf (stderr, "Nu b;lev det fel %d\n",linenumb);
}

tree  creatnode (nodeAttr_t  type, tree left,
							 tree     new)
{
#if 0
  tree  tmp_node = (tree) xmalloc (sizeof (ignode));

  if (yydebug) fprintf(stderr, "creating %d\n",type);
  tmp_node->type = type;
  tmp_node->left = left;
  tmp_node->middle = 0;
  tmp_node->next = new;
  tmp_node->id = 0;
  tmp_node->value = 0;
  return tmp_node;
#endif
}

tree creatid (char *id) 
{
#if 0
  tree  tmp_node = (tree) xmalloc (sizeof (ignode));

  if (id) if (yydebug) fprintf(stderr, "creating %s\n",id);
  tmp_node->type = tname;
  tmp_node->left = 0;
  tmp_node->middle = 0;
  tmp_node->next = 0;
  tmp_node->id = id;
  tmp_node->value = 0;
  return tmp_node;
#endif
}

tree creatvalue (int value) 
{
#if 0
  tree  tmp_node = (tree) xmalloc (sizeof (ignode));

  if (yydebug) fprintf(stderr, "creating %d\n",value);
  tmp_node->type = tvalue;
  tmp_node->left = 0;
  tmp_node->middle = 0;
  tmp_node->next = 0;
  tmp_node->id = 0;
  tmp_node->value = value;
  return tmp_node;
#endif
}

/* Konstruer en l|vnode som kun inneholder en tallverdi. */
int numbFors=0;

tree search(tree ig, int type) {
#if 0
  tree fin;
  void dosearch(tree ig, int type) {
	 if (fin) return;
	 if (!ig) { fin=1; return 0; }
	 if (ig->type==type) { fin=ig; }
	 if (ig->left) search(ig->left,type);
	 if (ig->middle) search(ig->middle,type);
	 if (ig->next) search(ig->next,type);
  };
  fin=0;
  dosearch(ig,type);
  if (fin==1) fin=0;
  return fin;
#endif
}

char * gettype(tree ig) {
#if 0
  tree sign, isvoid, def;
  char *sign_s=0,*isvoid_s=0,*def_s=0,*tmp;
  tmp=(char *)xmalloc( 80);
  tmp[0]=32;
  tmp[1]=0;
  isvoid=search(ig,novalue_attribute);
  if (isvoid) { strcat(tmp,"void "); goto endgettype; }
  sign=search(ig,extension_attribute);
  def=search(ig,allocation_unit);
  if (sign && sign->value==K_SIGNED)  ; else strcat(tmp, "unsigned "); 
  if (def) 
	 switch (def->value) {
	 case K_LONG:
		strcat(tmp,"long ");
		break;
	 case K_WORD:
		strcat(tmp,"short ");
		break;
	 case K_BYTE:
		strcat(tmp,"short "); /*should be char nonfin*/
		break;
	 default:
		strcat(tmp,"<<<1>>>");
		break;
	 } else strcat(tmp, " int "); 
endgettype:
  return xstrdup(tmp);
#endif
}

/*#define fprinto(x) fprintf(stdout,x)
#define fprinte(x) fprintf(stderr,x)
#define fprinte(x,y) fprintf(stderr,x,y)*/

extern int p;
extern int c;

/*
void fprinte(int p,...) {
va_list argptr ;
va_start(argptr,
if (p) fprintf(stderr,argptr);
va_end(argptr);
}
void fprinto(int c,...) {
va_list argptr ;
if (c) fprintf(stdout,argptr);
}
*/

FILE * myout=stderr;
#if 0
extern FILE * f;
extern FILE * myout;

void  generatecode (tree np) {
  if (np) {
	 switch (np->type) {
	 case module:
					
		generatecode(np->left);
		generatecode(np->next);
					
		break;
	 case module_head:
				 
		generatecode(np->next);
		break;
	 case opt_mhargs:
				 
		generatecode(np->next);
				 
		break;
	 case ms_list:
		generatecode(np->next);
				 
		generatecode(np->left);
		break;
	 case module_body:
		generatecode(np->next);
		break;

	 case block_body:
		generatecode(np->left);
		generatecode(np->middle);
		if (np->next) fprintf(f, "return ");
		generatecode(np->next);
		break;
	 case return_expression:
		fprintf(f, "return "); 
		generatecode(np->next);
		break;
	 case unlabeled_block:
		fprintf(f, "{\n");
		generatecode(np->next);
		fprintf(f, "\n}\n");
		break;
	 case on_off_switch:
				 
		break;
	 case special_switch:
					
		generatecode(np->left);
		break;
	 case own_declaration:
					
		generatecode(np->next);
					
		break;

	 case access_formal_list:
	 case allocation_formal_list:
	 case attached_label_list:
	 case attribute_list:
	 case block_action_list:
	 case case_line_list:
	 case decl_list:
	 case declaration_list:
	 case external_item_list:
	 case external_reg_item_list:
	 case external_routine_item_list:
	 case forward_item_list:
	 case forward_routine_attribute_list:
	 case global_item_list:
	 case global_reg_item_list:
	 case list_option_list:
	 case map_declaration_attribute_list:
	 case own_attribute_list:
	 case own_declaration_list:
	 case own_item_list:
	 case quoted_string_list:
	 case register_item_list:
	 case routine_attribute_list:
	 case select_label_list:
	 case select_line_list:
	 case structure_definition_list:
		generatecode(np->next);
		fprintf(f, " ");
		generatecode(np->left);
		break;
	 case access_actual_list:
	 case actual_parameter_list:
	 case alloc_actual_list:
	 case bind_data_item_list:
	 case bind_routine_item_list:
	 case case_label_list:
	 case char_par_list:
	 case external_literal_item_list:
	 case field_stuff_list:
	 case formal_item_list:
	 case forward_routine_item_list:
	 case global_routine_definition_list:
	 case initial_item_list:
	 case input_parameter_location_list:
	 case io_actual_parameter_list:
	 case keyword_pair_list:
	 case language_name_list:
	 case linkage_definition_list:
	 case literal_item_list:
	 case local_item_list:
	 case map_item_list:
	 case mode_spec_list:
	 case output_parameter_location_list:
	 case plit_item_list:
	 case positional_macro_definition_list:
	 case psect_item_list:
	 case routine_definition_list:
	 case string_par_list:
	 case tname_list:
		generatecode(np->next);
		if(np->next) fprintf(f, " , ");
		generatecode(np->left);
		break;


	 case require_declaration:
		fprintf(f, "#include< %s.h >\n",np->id);
		break;
	 case library_declaration:
		fprintf(f, "#include< %s.h >\n",np->id);
		break;
	 case local_item:
					
		if (np->next) {
		  generatecode(np->next);
					
		} else {
		  fprintf(f, "\nint ");
		}
		fprintf(f,"*%s ;\n",np->id);
		break;
	 case formal_item:
	 case own_item:
					
		/*					if (np->next) {*/
		{ char *tmp;
		tmp=gettype(np->next);
		fprintf(f, " %s ",tmp);
		}
	 /*					generatecode(np->next);
					
							} else {
							fprintf(f, "\nstatic int ");
							}*/
	 /* default is 8 bytes on 64bit, 4bytes on 32bit
		 for both VAX and AXP:
		 long is 4 bytes
		 word is 2 bytes
		 byte is 2 bytes -> even/word alignment?
		 */
	 fprintf(f,"*%s ;\n",np->id);
	 break;
	 case ordinary_routine_declaration:
		/*					if (ig_main) fprintf(f,"main(int argc, char **argv){");*/
		generatecode(np->next);
		/*					if (ig_main) fprintf(f,"}");*/
					
		break;
	 case routine_definition:
		{char *tmp; tmp=gettype(np->middle);
		fprintf(f, " %s ",tmp);}
	 fprintf(f,np->id); 
	 /*					searchvalue(np->middle,)*/
	 fprintf(f, " ( ");
	 generatecode(np->left);
	 fprintf(f, " ) ");
	 generatecode(np->next);
	 break;
	 case io_list:
		generatecode(np->left);
		if (np->next) fprintf(f, " , ");
		generatecode(np->next);
		break;
	 case executable_function:
		fprintf(f, "\nef");
		generatecode(np->left);
		fprintf(f, " ( ");
		generatecode(np->next);
		fprintf(f, " ) ");
		break;
	 case ordinary_routine_call:
		fprintf(f, "\n%s ( ",np->id);
		generatecode(np->next);
		fprintf(f, " ) ");
		break;
	 case general_routine_call:
		break;
	 case global_io_list2:
	 case io_list3:
	 case io_list2:
		generatecode(np->next);
		generatecode(np->left);
		break;
	 case block_body2:
		generatecode(np->left);
		generatecode(np->next);
		break;
	 case decimal_literal:
		if (np->id) fprintf(f,"%c",np->id);
		fprintf(f,"%d",np->value);
		generatecode(np->left);
		break;
		break;

	 case fetch_expression:
		fprintf(f, " * ");
		generatecode(np->next);
		break;

	 case assign_expression:
		break;

	 case infix_expression:
		generatecode(np->left);
		fprintf(f, " %s ",np->id);
		generatecode(np->next);
		break;


	 case pot_expression:
	 case opexp1:
	 case opexp3:
	 case opexp5:
	 case opexp6:
	 case opexp7:
	 case opexp8:
	 case opexp9:
		generatecode(np->left);
		fprintf(f, " %s ",np->id);
		generatecode(np->next);
		break;
	 case opexp4:
		fprintf(f, " ! ");
		generatecode(np->next);
		break;
	 case opexp2:
		generatecode(np->left);
		switch (np->value) {
		case K_OR:
		  fprintf(f, " or "); 
		  break;
		case K_XOR:
		  fprintf(f, " xor "); 
		  break;
		case K_AND:
		  fprintf(f, " and "); 
		  break;
		case K_EQV:
		  fprintf(f, " eqv "); 
		  break;
		case K_EQL:
		  fprintf(f, " eql "); 
		  break;
		case K_EQLA:
		  fprintf(f, " eqla "); 
		  break;
		case K_EQLU:
		  fprintf(f, " eqlu "); 
		  break;
		case K_NEQ:
		  fprintf(f, " neq "); 
		  break;
		case K_NEQA:
		  fprintf(f, " neqa "); 
		  break;
		case K_NEQU:
		  fprintf(f, " nequ "); 
		  break;
		case K_LSS:
		  fprintf(f, " lss "); 
		  break;
		case K_LSSA:
		  fprintf(f, " lssa "); 
		  break;
		case K_LSSU:
		  fprintf(f, " lssu "); 
		  break;
		case K_LEQ:
		  fprintf(f, " leq "); 
		  break;
		case K_LEQA:
		  fprintf(f, " leqa "); 
		  break;
		case K_LEQU:
		  fprintf(f, " lequ "); 
		  break;
		case K_GTR:
		  fprintf(f, " gtr "); 
		  break;
		case K_GTRA:
		  fprintf(f, " gtra "); 
		  break;
		case K_GTRU:
		  fprintf(f, " gtru "); 
		  break;
		case K_GEQ:
		  fprintf(f, " geq "); 
		  break;
		case K_GEQA:
		  fprintf(f, " geqa "); 
		  break;
		case K_GEQU:
		  fprintf(f, " gequ "); 
		  break;
		default:
		  fprintf(f, "nonimp4 %d",np->value);
		  break;
		}
		generatecode(np->next);
		break;


	 case block_action:
		generatecode(np->next);
		fprintf(f, ";\n");
		break;
					
	 case tname:
		fprintf(f, "%s", np->id); 
		break;
	 case tvalue:
		fprintf(f, "%d", np->value); 
					
		break;
	 default:
					
		generatecode(np->next);
		generatecode(np->left);
		break;
	 }
  }
};
#endif

#define uplevel prevtop=cur_top_table;newscosym();prev=cur_sym_table;
#define downlevel cur_sym_table=prev;cur_top_table=prevtop;end_cur();next_sym_table=&(cur_sym_table->next);

#if 0
void  parsetree (tree np) {
  symrec *this,*prev,*scope,*prevtop; void * new;
  if (np) {
	 switch (np->type) {
	 case module:
		fprintf(myout,"\nmodule\n");
		parsetree(np->left);
		fprintf(myout," = ");
		parsetree(np->next);
		fprintf(myout,"\neludom\n");
		break;
	 case module_head:
		fprintf(myout," %s ",np->id);
		parsetree(np->next);
		break;
	 case opt_mhargs:
		fprintf(myout," ( ");
		parsetree(np->next);
		fprintf(myout," ) ");
		break;
	 case ms_list:
		parsetree(np->next);
		fprintf(myout," , ");
		parsetree(np->left);
		break;
	 case module_body:
		fprintf(myout,"\nbegin \n");
		parsetree(np->next);
		fprintf(myout,"\nend \n");
		break;
	 case on_off_switch:
		fprintf(myout," %s",np->id);
		break;
	 case special_switch:
		fprintf(myout," %s = ",np->id);
		parsetree(np->left);
		break;
	 case name_list:
		parsetree(np->left);
		fprintf(myout, " , ");
		parsetree(np->next);
		break;
	 case require_declaration:
		fprintf(myout, "\nrequire %s ;\n",np->id);
		break;
	 case library_declaration:
		fprintf(myout, "\nlibrary %s ;\n",np->id);
		break;
	 case own_declaration:
		fprintf(myout, "\nown ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;

	 case access_formal_list:
	 case allocation_formal_list:
	 case attached_label_list:
	 case attribute_list:
	 case block_action_list:
	 case case_line_list:
	 case decl_list:
	 case declaration_list:
	 case external_item_list:
	 case external_reg_item_list:
	 case external_routine_item_list:
	 case forward_item_list:
	 case forward_routine_attribute_list:
	 case global_item_list:
	 case global_reg_item_list:
	 case list_option_list:
	 case map_declaration_attribute_list:
	 case own_attribute_list:
	 case own_declaration_list:
	 case quoted_string_list:
	 case register_item_list:
	 case routine_attribute_list:
	 case select_label_list:
	 case select_line_list:
	 case structure_definition_list:
		parsetree(np->next);
		fprintf(myout, " ");
		parsetree(np->left);
		break;
	 case access_actual_list:
	 case actual_parameter_list:
	 case alloc_actual_list:
	 case bind_data_item_list:
	 case bind_routine_item_list:
	 case case_label_list:
	 case char_par_list:
	 case external_literal_item_list:
	 case field_stuff_list:
	 case formal_item_list:
	 case forward_routine_item_list:
	 case global_routine_definition_list:
	 case initial_item_list:
	 case input_parameter_location_list:
	 case io_actual_parameter_list:
	 case keyword_pair_list:
	 case language_name_list:
	 case linkage_definition_list:
	 case literal_item_list:
	 case local_item_list:
	 case map_item_list:
	 case mode_spec_list:
	 case output_parameter_location_list:
	 case own_item_list:
	 case plit_item_list:
	 case positional_macro_definition_list:
	 case psect_item_list:
	 case routine_definition_list:
	 case string_par_list:
	 case tname_list:
		parsetree(np->next);
		if(np->next) fprintf(myout, " , ");
		parsetree(np->left);
		break;
	 case bind_routine_item:
	 case external_item:
	 case external_literal_item:
	 case external_reg_item:
	 case external_routine_item:
	 case formal_item:
	 case forward_item:
	 case forward_routine_item:
	 case global_item:
	 case global_reg_item:
	 case initial_item:
	 case map_item:
	 case own_item:
	 case plit_item:
	 case preset_item:
	 case register_item:
		this=putsym(np->id,VAR);
		fprintf(myout, "\n\t%s ", np->id);
		if (np->next) fprintf(myout, " : "); 
		parsetree(np->next);
		break;
	 case literal_item:
		putsym(np->id,VAR);
		fprintf(myout, "\n\t%s = ",np->id); 
		parsetree(np->left);
		if (np->next) fprintf(myout, " : "); 
		parsetree(np->next);
		break;
			  
	 case initial_group:
		if (np->left) fprintf(myout, "\nrep "); 
		parsetree(np->left);
		if (np->left) fprintf(myout, " of ");
		parsetree(np->middle);
		fprintf(myout, "("); 
		parsetree(np->next);
		fprintf(myout, ")"); 
		break;
	 case plit_group:
		if (np->left) fprintf(myout, "\nrep "); 
		parsetree(np->left);
		if (np->left) fprintf(myout, " of ");
		parsetree(np->next);
		break;
	 case plit2:
		if (np->value=K_PLIT) fprintf (myout, "\nplit "); 
		else fprintf (myout, "\nuplit "); 
		break;
	 case plit3:
		parsetree(np->left);
		parsetree(np->next);
		break;
	 case plit:
		parsetree(np->left);
		parsetree(np->middle);
		fprintf(myout, " ( ");
		parsetree(np->next);
		fprintf(myout, " ) ");
		break;
	 case forward_routine_declaration:
		fprintf(myout, "\nforward routine ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case routine_declaration:
	 case ordinary_routine_declaration:
		fprintf(myout, "\nroutine ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case global_routine_declaration:
		fprintf(myout, "\nglobal routine ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case external_routine_declaration:
		fprintf(myout, "\nexternal routine ");
		parsetree(np->next);
		break;
		/*			case ordinary_routine_definition:
					break;*/
	 case global_routine_definition:
	 case routine_definition:
		putsym(np->id,ROU);
		fprintf(myout, " %s ",np->id);
		uplevel;
		parsetree(np->left);
		if (np->middle) fprintf(myout, " : "); 
		parsetree(np->middle);
		fprintf(myout, " = ");
		parsetree(np->next);
		downlevel;
		break;
	 case io_list:
		fprintf(myout, " ( ");
		parsetree(np->left);
		if (np->next) fprintf(myout, " : ");
		parsetree(np->next);
		fprintf(myout, " ) ");
		break;
	 case global_declaration:
		fprintf(myout, "\nglobal ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case external_declaration:
		fprintf(myout, "\nexternal ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case forward_declaration:
		fprintf(myout, "\nforward ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case linkage_declaration:
		fprintf(myout, "\nlinkage ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case local_declaration:
		fprintf(myout, "\nlocal ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case stackglocal_declaration:
		fprintf(myout, "\nstackglocal ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case register_declaration:
		fprintf(myout, "\nregister ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case global_register_declaration:
		fprintf(myout, "\nglobal register ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case external_register_declaration:
		fprintf(myout, "\nexternal register");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case literal_declaration:
		fprintf(myout, "\nliteral ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case external_literal_declaration:
		fprintf(myout, "\nexternal literal ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case map_declaration:
		fprintf(myout, "\nmap ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case structure_declaration:
		fprintf(myout, "\nstructure ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case undeclare_declaration:
		fprintf(myout, "\nundeclare ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case enable_declaration:
		fprintf(myout, "\nenable %s",np->id);
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case bind_data_declaration:
		fprintf(myout, "\nbind ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
	 case bind_data_item:
		fprintf(myout, "%s =",np->id);
		parsetree(np->left);
		fprintf(myout, ":");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
					
	 case positional_macro_declaration:
		fprintf(myout, "\nmacro ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
					
	 case keyword_macro_declaration:
		fprintf(myout, "\nkeywordmacro ");
		parsetree(np->next);
		fprintf(myout, ";");
		break;
					
	 case linkage_definition:
		fprintf(myout, "\n\t%s = ",np->id);
		parsetree(np->left);
		break;
			  
	 case conditional_macro_definition:
		fprintf(myout, "\n\t %s ",np->id);
		if (np->left) fprintf(myout, " ( ");
		parsetree(np->left);
		if (np->left) fprintf(myout, " ) ");
		fprintf(myout, " [ ] = ");
		parsetree(np->next);
		fprintf(myout, " %%");
		break;
	 case iterative_macro_definition:
		fprintf(myout, "\n\t %s ",np->id);
		if (np->left) fprintf(myout, " ( ");
		parsetree(np->left);
		if (np->left) fprintf(myout, " ) ");
		fprintf(myout, " [ ");
		parsetree(np->middle);
		if (np->left) fprintf(myout, " ) ");
		fprintf(myout, " ] = ");
		parsetree(np->next);
		fprintf(myout, " %%");
		break;
	 case iterative_macro_definition2:
		parsetree(np->left);
		if (np->left) fprintf(myout, " ) ");
		fprintf(myout, " ] = ");
		parsetree(np->next);
		break;
	 case keyword_macro_definition:
		fprintf(myout, "\n\t %s ",np->id);
		if (np->left) fprintf(myout, " ( ");
		parsetree(np->left);
		if (np->left) fprintf(myout, " ) ");
		fprintf(myout, " = ");
		parsetree(np->next);
		fprintf(myout, " %%");
		break;
	 case positional_macro_definition:
		break;
	 case simple_macro_definition:
		fprintf(myout, "\n\t %s ",np->id);
		if (np->left) fprintf(myout, " ( ");
		parsetree(np->left);
		if (np->left) fprintf(myout, " ) ");
		fprintf(myout, " = ");
		parsetree(np->next);
		fprintf(myout, " %%");
		break;
	 case keyword_pair:
		fprintf(myout, "\n\t %s ",np->id);
		if(np->next) fprintf(myout, " = ");
		parsetree(np->next);
		break;
	 case structure_definition:
		break;
	 case ordinary_routine_call:
		fprintf(myout, "\n%s ( ",np->id);
		parsetree(np->next); 
		fprintf(myout, " ) ");
		break;
	 case general_routine_call:
		fprintf(myout, "\n%s ( ",np->id);
		parsetree(np->left); 
		fprintf(myout, " , ");
		parsetree(np->next); 
		fprintf(myout, " ) ");
		break;
	 case io_list3:
	 case io_list2:
		parsetree(np->left);
		if (np->next) fprintf(myout, " : ");
		parsetree(np->next);
		break;
	 case routine_attribute:
					
		break;
	 case allocation_unit:
		switch (np->value) {
		case K_LONG:
		  fprintf(myout, "long");
		  break;
		case K_BYTE:
		  fprintf(myout, "byte");
		  break;
		case K_WORD:
		  fprintf(myout, "word");
		  break;
		default:
		  fprintf(myout, "nonimp6 %d",np->value);/*
																 { tree p=(tree)np->value; fprintf(myout, " %d",p->value); }*/
		  break;
		}
		break;
	 case extension_attribute:
		if (np->value=K_SIGNED) fprintf(myout, " signed "); 
		else fprintf(myout, " unsigned ");
		break;
	 case structure_attribute:
		if (np->value) fprintf(myout, " ref ");
		fprintf(myout, "%s ",np->id);
		if (np->next) fprintf(myout, " [ "); 
		parsetree(np->next);
		if (np->next) fprintf(myout, " ] "); 
		break;
	 case field_attribute:
		fprintf(myout, "field ");
		break;
	 case addressing_mode_attribute:
		fprintf(myout, "addressing_mode ( ");
		parsetree(np->next);
		fprintf(myout, " ) ");
		break;
	 case alignment_attribute:
		fprintf(myout, "align ");
		parsetree(np->next);
		break;
	 case initial_attribute:
		fprintf(myout, " initial ( ");
		parsetree(np->next);
		fprintf(myout, " ) ");
		break;
	 case preset_attribute:
		fprintf(myout, " preset");
		break;
	 case psect_attribute:
		fprintf(myout, " psect");
		break;
	 case volatile_attribute:
		fprintf(myout, " volatile");
		break;
	 case novalue_attribute:
		fprintf(myout, " novalue");
		break;
	 case linkage_attribute:
		fprintf(myout, "%s",np->id);
		break;
	 case range_attribute:
		if (np->value=K_SIGNED) fprintf(myout, " signed ("); 
		else fprintf(myout, " unsigned (");
		parsetree(np->next);
		fprintf(myout,")");
		break;
	 case addressing_mode:
		fprintf(myout,"addressing_mode (");
		parsetree(np->next);
		fprintf(myout,")"); 
		break;
	 case pre_tested_loop:
		if (np->value)
		  fprintf(myout, "\nwhile ");
		else fprintf(myout, "\nuntil ");
		parsetree(np->left);
		fprintf(myout, " do "); 
		parsetree(np->next);
		break;
	 case post_tested_loop:
		fprintf(myout, " do "); 
		parsetree(np->left);
		if (np->value)
		  fprintf(myout, "\nwhile ");
		else fprintf(myout, "\nuntil ");
		parsetree(np->next);
		break;
	 case conditional_expression:
		fprintf(myout, "\nif ");
		parsetree(np->left);
		fprintf(myout, " then ");
		parsetree(np->middle);
		if (np->next) fprintf(myout, " else ");
		parsetree(np->next);
		break;
	 case conditional_expression2:
		parsetree(np->left);
		if (np->next) fprintf(myout, " else ");
		parsetree(np->next);
		break;
	 case return_expression:
		fprintf(myout, "return "); 
		parsetree(np->next);
		break;
	 case unlabeled_block:
		if (np->value)
		  fprintf(myout, "\nbegin ");
		else
		  fprintf(myout, " ( ");
		parsetree(np->next);
		if (np->value)
		  fprintf(myout, "\nend ");
		else
		  fprintf(myout, " ) ");
		break;
	 case block_body:
		/*					prev=cur_sym_table;
							prevtop=cur_top_table;
							newscosym();
							next_sym_table=&(cur_sym_table->level);
							cur_sym_table=cur_sym_table->level;*/
		uplevel;
		parsetree(np->left);
		parsetree(np->middle);
		parsetree(np->next);
		/*					cur_sym_table=prev;
							cur_top_table=prevtop;
							next_sym_table=&(cur_sym_table->level);*/
		downlevel;
		/*										end_cur();*/
		break;
	 case block_body2:
		parsetree(np->left);
		parsetree(np->middle);
		parsetree(np->next);
		break;
	 case integer_literal:
		fprintf ( myout, "%%%c",np->value);
		goto skipit;
		switch (np->value) {
		case P_B: 
		  fprintf(myout, "%c b",'%');
		  break;
		case P_O: 
		  fprintf(myout, "%s o","%");
		  break;
		case P_DECIMAL: 
		  fprintf(myout, "%cd",'%');
		  break;
		case P_X: 
		  fprintf(myout, "%cx",'%');
		  break;
		default:
		  fprintf(myout, "nonimp5 %d",np->value);
		  break;
		}
	 skipit:
		fprintf(myout, "%s",np->id);
		break;

	 case decimal_literal:
		if (np->id) fprintf(myout,"%c",np->id);
		fprintf(myout,"%d",np->value);
		parsetree(np->left);
		break;
	 case fetch_expression:
		fprintf(myout, ".");
		parsetree(np->next);
		break;
	 case assign_expression:
		parsetree(np->left);
		fprintf(myout, " = ");
		parsetree(np->next);
		break;
	 case infix_expression:
		parsetree(np->left);
		fprintf(myout, " %s ",np->id);
		parsetree(np->next);
		break;
	 case block_action:
		parsetree(np->next);
		fprintf(myout, ";\n");
		break;
	 case executable_function:
		parsetree(np->left);
		fprintf(myout, "(");
		parsetree(np->next);
		fprintf(myout, ")");
		break;
	 case cond_handling_function_name:
		switch (np->value) {
		case K_SIGNAL:
		  fprintf(myout, "signal");
		  break;
		case K_STOP:
		  fprintf(myout, "stop");
		  break;
		case K_SETUNWIND:
		  fprintf(myout, "setunwind");
		  break;
		default:
		  fprintf(myout, "nonimp1 %d",np->value);
		  break;
		}
		break;
	 case pot_expression:
	 case opexp1:
	 case opexp3:
	 case opexp5:
	 case opexp6:
	 case opexp7:
	 case opexp8:
	 case opexp9:
		parsetree(np->left);
		fprintf(myout, " %s ",np->id);
		parsetree(np->next);
		break;
	 case opexp4:
		fprintf(myout, " not ");
		parsetree(np->next);
		break;
	 case opexp2:
		parsetree(np->left);
		switch (np->value) {
		case K_OR:
		  fprintf(myout, " or "); 
		  break;
		case K_XOR:
		  fprintf(myout, " xor "); 
		  break;
		case K_AND:
		  fprintf(myout, " and "); 
		  break;
		case K_EQV:
		  fprintf(myout, " eqv "); 
		  break;
		case K_EQL:
		  fprintf(myout, " eql "); 
		  break;
		case K_EQLA:
		  fprintf(myout, " eqla "); 
		  break;
		case K_EQLU:
		  fprintf(myout, " eqlu "); 
		  break;
		case K_NEQ:
		  fprintf(myout, " neq "); 
		  break;
		case K_NEQA:
		  fprintf(myout, " neqa "); 
		  break;
		case K_NEQU:
		  fprintf(myout, " nequ "); 
		  break;
		case K_LSS:
		  fprintf(myout, " lss "); 
		  break;
		case K_LSSA:
		  fprintf(myout, " lssa "); 
		  break;
		case K_LSSU:
		  fprintf(myout, " lssu "); 
		  break;
		case K_LEQ:
		  fprintf(myout, " leq "); 
		  break;
		case K_LEQA:
		  fprintf(myout, " leqa "); 
		  break;
		case K_LEQU:
		  fprintf(myout, " lequ "); 
		  break;
		case K_GTR:
		  fprintf(myout, " gtr "); 
		  break;
		case K_GTRA:
		  fprintf(myout, " gtra "); 
		  break;
		case K_GTRU:
		  fprintf(myout, " gtru "); 
		  break;
		case K_GEQ:
		  fprintf(myout, " geq "); 
		  break;
		case K_GEQA:
		  fprintf(myout, " geqa "); 
		  break;
		case K_GEQU:
		  fprintf(myout, " gequ "); 
		  break;
		default:
		  fprintf(myout, "nonimp4 %d",np->value);
		  break;
		}
		parsetree(np->next);
		break;
	 case b_main:
		fprintf(myout, "\nmain = %s",np->id);
		break;
	 case prversion:
		fprintf(myout, "\nversion = %s",np->id);
		break;
	 case ident:
		fprintf(myout, "\nident = %s",np->id);
		break;
	 case sw_addressing_mode:
		fprintf(myout,"addressing_mode(");
		parsetree(np->next);
		fprintf(myout,")");
		break;
	 case addr_external:
		fprintf(myout,"external = ");
		parsetree(np->next);
		break;
	 case addr_non_external:
		fprintf(myout,"nonexternal = ");
		parsetree(np->next);
		break;
	 case mode_32:
		switch (np->value) {
		case U_ABSOLUTE:
		  fprintf(myout, "absolute");
		  break;
		case U_GENERAL:
		  fprintf(myout, "general");
		  break;
		case U_LONG_RELATIVE:
		  fprintf(myout, "long_relative");
		  break;
		case U_WORD_RELATIVE:
		  fprintf(myout, "word_relative");
		  break;
		default:
		  fprintf(myout, "nonimp3 %d",np->value);
		  break;
		}
		break;
	 case string_type:
		switch (np->value) {
		case P_ASCII:
		  fprintf(myout, "%%ascii");
		  break;
		case P_ASCIZ:
		  fprintf(myout, "%%asciz");
		  break;
		case P_ASCIC:
		  fprintf(myout, "%%ascic");
		  break;
		case P_ASCID:
		  fprintf(myout, "%%ascid");
		  break;
		case P_RAD50_11:
		  fprintf(myout, "%%rad50_11");
		  break;
		case P_RAD50_10:
		  fprintf(myout, "%%rad50_10");
		  break;
		case P_SIXBIT:
		  fprintf(myout, "%%sixbit");
		  break;
		case P_P:
		  fprintf(myout, "%%p");
		  break;
		default:
		  fprintf(myout, "nonimp2 %d",np->value);
		  break;
		}
		break;
	 case string_literal:
		parsetree(np->left);
		parsetree(np->next);
		break;
	 case p_chesc:
		fprintf(myout, "%%chesc");
		break;
	 case p_char:
		fprintf(myout, "%%char");
		fprintf(myout, "(");
		parsetree(np->next);
		fprintf(myout, ")");
		break;
	 case p_string:
		fprintf(myout, "%%string");
		fprintf(myout, "(");
		parsetree(np->next);
		fprintf(myout, ")");
		break;
	 case tname:
		fprintf(myout, " %s",np->id);
		break;
	 case tvalue:
		fprintf(myout, " %d",np->value);
		break;
	 default:
		fprintf(myout,"\nunknown %d\n\n",np->type);
		parsetree(np->next);
		parsetree(np->left);
		break;
	 }
  }
};
#endif

void end_cur(void) { }
void newscosym(void) { }

symrec *cur_sym_table;

int bli_lex(void) {
  return z1zlex();
}

int bli_lex_not_now(void) 
{
  struct bli_token_struct* prev;
  prev=current_token;
  
  if (current_token==NULL) 
    {
      current_token=first_available_token;
    } 
  else 
    {
      current_token=current_token->bli_token_next;
    }
  
  if (!current_token) 
    {
//      bli_lval=NULL;
      return 0;
    }
  
  current_token->bli_token_prev=prev;
  
  //  bli_lval.type_node_p=current_token;
  return current_token->pfx.type.tok_type;
}

extern FILE *z1zin;
void 
parse_init (struct bli_token_struct * first_token, uint32 parser_trace_flag) 
{
  z1zin=fopen(input_filename,"r");
  current_token=NULL;
  first_available_token=first_token;
#ifdef YYDEBUG
  bli_debug=parser_trace_flag;    
#else
  if (parser_trace_flag) 
    {
#if 0
            assert(0 && "lll; parser trace cannot be enabled - blicprs.y not compiled 
with YYDEBUG set");
#endif
      fprintf(stderr, "lll; parser trace cannot be enabled - blicprs.y not compiled with YYDEBUG set");
    }
#endif

  init_reswords();

  ggc_add_tree_root (&declspec_stack, 1);
  ggc_add_tree_root (&current_declspecs, 1);
  ggc_add_tree_root (&prefix_attributes, 1);
  ggc_add_tree_root (&all_prefix_attributes, 1);
  
}

symrec *
putsym (sym_name,sym_type)
          char *sym_name;
          int sym_type;
{
}

symrec *sym_table = (symrec *)0;
symrec *cur_sym_table = (symrec *)0;
symrec *cur_top_table= (symrec*) 0;
symrec **next_sym_table;

void
dump_token (FILE * f , const uchar *prefix, struct bli_token_struct *t)
{
//fprintf(stderr, "%x\n", yylval);
  uint32 char_ix;
  BLI_ASSERT (t);
  BLI_ASSERT (max_token_nbr >= t->pfx.type.tok_type);
  fprintf (f,"%stoken type = %d lit=%s len = %d line = %d char = %d file nbr = %d chars = ", 
          prefix,
          t->pfx.type.tok_type,
          token_lit_ptrs[t->pfx.type.tok_type],
          t->string_details->length,
          t->bli_token_lineno,
          t->bli_token_charno,
          t->bli_token_fileno);
  for (char_ix = 0; char_ix < t->string_details->length; char_ix++) 
    {
      fprintf (f,"%c",t->string_details->string[char_ix]);
    }
  for (char_ix = 0; char_ix < t->string_details->length; char_ix++) 
    {
      fprintf (f," %2.2x",t->string_details->string[char_ix]);
    }
  if (t->string_details->string_upper->string)
    {
      fprintf (f," uc = ");
      for (char_ix = 0; char_ix < t->string_details->length; char_ix++) 
        {
          fprintf (f, "%c",t->string_details->string_upper->string[char_ix]);
        }
    }
}

void yyerror3 (char *s)
{
  if (s)fprintf( stderr,"\n\n%s\n",s); 
  fprintf (stderr, "Nu b;lev det fel %d\n",linenumb);
}

void yy2error (char *s)
{
  if (s)fprintf( stderr,"\n\n%s\n",s); 
  fprintf (stderr, "Nu b;lev det fel %d\n",linenumb);
}

void
ggc_mark_if_gcable (const void * a  ATTRIBUTE_UNUSED)
{
}

void
maybe_apply_pragma_weak(int a) {
}

tree
maybe_apply_renaming_pragma(tree decl, tree asmname) {
  return asmname;
}

void
c_parse_init(void) {
}

void
c_common_insert_default_attributes2 (decl)
     tree decl;
{
  tree name = DECL_NAME (decl);

#if 0
  if (!c_attrs_initialized)
    c_init_attributes ();
#endif

#if 0
#define DEF_ATTR_NULL_TREE(ENUM) /* Nothing needed after initialization.  */
#define DEF_ATTR_INT(ENUM, VALUE)
#define DEF_ATTR_IDENT(ENUM, STRING)
#define DEF_ATTR_TREE_LIST(ENUM, PURPOSE, VALUE, CHAIN)
#define DEF_FN_ATTR(NAME, ATTRS, PREDICATE)                     \
  if ((PREDICATE) && name == built_in_attributes[(int) NAME])   \
    decl_attributes (&decl, built_in_attributes[(int) ATTRS],   \
                     ATTR_FLAG_BUILT_IN);
#include "builtin-attrs.def"
#undef DEF_ATTR_NULL_TREE
#undef DEF_ATTR_INT
#undef DEF_ATTR_IDENT
#undef DEF_ATTR_TREE_LIST
#undef DEF_FN_ATTR
#endif
}

int
cpp_handle_option (pfile, argc, argv, ignore)
     cpp_reader *pfile;
     int argc;
     char **argv;
     int ignore;
{
}

int
defer_fn2 (fn)
     tree fn;
{
  //  VARRAY_PUSH_TREE (deferred_fns, fn);

  return 1;
}

void
set_Wformat (setting)
     int setting;
{
  warn_format = setting;
  warn_format_y2k = setting;
  warn_format_extra_args = setting;
  if (setting != 1)
    {
      warn_format_nonliteral = setting;
      warn_format_security = setting;
    }
}

void
gen_aux_info_record (fndecl, is_definition, is_implicit, is_prototyped)
     tree fndecl;
     int is_definition;
     int is_implicit;
     int is_prototyped;
{
#if 0
  if (flag_gen_aux_info)
    {
      static int compiled_from_record = 0;

      /* Each output .X file must have a header line.  Write one now if we
         have not yet done so.  */

      if (! compiled_from_record++)
        {
          /* The first line tells which directory file names are relative to.
             Currently, -aux-info works only for files in the working
             directory, so just use a `.' as a placeholder for now.  */
          fprintf (aux_info_file, "/* compiled from: . */\n");
        }

      /* Write the actual line of auxiliary info.  */

      fprintf (aux_info_file, "/* %s:%d:%c%c */ %s;",
               DECL_SOURCE_FILE (fndecl),
               DECL_SOURCE_LINE (fndecl),
               (is_implicit) ? 'I' : (is_prototyped) ? 'N' : 'O',
               (is_definition) ? 'F' : 'C',
               gen_decl (fndecl, is_definition, ansi));

      /* If this is an explicit function declaration, we need to also write
         out an old-style (i.e. K&R) function header, just in case the user
         wants to run unprotoize.  */

      if (is_definition)
        {
          fprintf (aux_info_file, " /*%s %s*/",
                   gen_formal_list_for_func_def (fndecl, k_and_r_names),
                   gen_formal_list_for_func_def (fndecl, k_and_r_decls));
        }

      fprintf (aux_info_file, "\n");
    }
#endif
}

void
cpp_finish_options (pfile)
     cpp_reader *pfile;
{
}

cpp_reader *
cpp_create_reader (lang)
     enum c_lang lang;
{
}

void
cpp_post_options (pfile)
     cpp_reader *pfile;
{
}

void
cpp_error VPARAMS ((cpp_reader * pfile, const char *msgid, ...))
{
}

unsigned int
cpp_errors (pfile)
     cpp_reader *pfile;
{
  //return pfile->errors;
}

tree
handle_format_attribute (node, name, args, flags, no_add_attrs)
     tree *node;
     tree name ATTRIBUTE_UNUSED;
     tree args;
     int flags;
     bool *no_add_attrs;
{
}

tree
handle_format_arg_attribute (node, name, args, flags, no_add_attrs)
     tree *node;
     tree name ATTRIBUTE_UNUSED;
     tree args;
     int flags;
     bool *no_add_attrs;
{
}

const char *
init_c_lex (filename)
     const char *filename;
{
}

void
init_pragma ()
{
}

void
cpp_finish (pfile)
     cpp_reader *pfile;
{
}
