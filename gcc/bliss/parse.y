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

#include "ignorance.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 extern int undefmode;
 extern int tnamemode;
 extern int linenumb;

#define malloc xmalloc

#include "config.h"

#undef IN_GCC
#include "ggc.h"
#define IN_GCC

#define malloc xmalloc

#include "tree.h"

#include "blilex.h"

#include "blicsyt.h"

#include "c-common.h"

#include "bliss-tree.h"
#include "c-tree.h"

 static int compstmt_count;

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

struct structure {
  struct structure * next;
  tree elem;
};

struct structure * mystructs = 0;

extern FILE *yyin;

int yyparse() {
  //z1zin=fopen(input_filename,"r");
  yyin=fopen(input_filename,"r");

  return yyparse_2();
}


#define yyparse yyparse_2
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
%type <type_node_p> unlabeled_block_start unlabeled_block_start2 
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
%type <type_node_p> routine_attributes
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
%type <type_node_p> weak_attribute own_attribute maybe_own_attribute_list
%type <type_node_p> address lexical_actual_parameter own_attribute_list
%type <type_node_p> initial_item initial_group initial_expression
%type <type_node_p>  preset_item ctce_access_actual io_actual_parameter_list io_actual_parameter
%type <type_node_p> preset_value psect_item_list psect_item
%type <type_int> storage_class addressing_mode_attribute 
%type <type_node_p> volatile_attribute psect_attribute_list
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
%type <type_node_p> op_exp9 op_exp10 op_exp11 op_exp12
%type <type_node_p> b32_psect_attribute psect_attribute
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
};
/*module_body*/
module		: K_MODULE module_head '=' 
{
  last_expr_filename=input_filename;
}
save_filename save_lineno
 start_block K_ELUDOM
{
  $$=$7;
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
| '(' ms_list ')' { $$ = $2; }
/*| '(' ms_list ')' { fprintf(stderr,"err %x \n",$2); fflush(stderr); }*/
  		;

ms_list		: { undefmode=1; } module_switch { undefmode=0; $$ = $2; }
| ms_list ',' { undefmode=1; } module_switch { undefmode=0;  }
;

module_body: /* empty */ { $$=0; }
| K_BEGIN decl_list K_END { $$ = $2; }

| '(' decl_list ')' { $$ = $2; }

;

decl_list	: declaration 
| decl_list declaration 
;

/* 1.1 Module Switches */

module_switch	: on_off_switch 
| special_switch 
;

on_off_switch: onoffmodes { $$ = 0; }
;

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
common_switch 
/*|bliss_16_switch */
|bliss_32_switch 
/*|bliss_36_switch */
;

bliss_16_switch:
K_ADDRESSING_MODE '(' mode_16 ')'  { $$ = 0; }
/*|U_ENVIRONMENT '(' environ_16_option ')'  */
;

environ_16_option:
U_EIS  { $$ = 0; }
| U_NOEIS  { $$ = 0; }
| U_LSI11  { $$ = 0; }
| U_T11  { $$ = 0; }
|	 U_PIC  { $$ = 0; }
| U_ODT  { $$ = 0; }
;

bliss_32_switch:
K_ADDRESSING_MODE '(' mode_spec_list ')'  { $$ = 0; }
;

mode_spec_list: mode_spec_list ',' mode_spec 
| mode_spec 
;

mode_spec:
K_EXTERNAL '='  mode_32  { $$ = 0; }
| U_NONEXTERNAL '='  mode_32  { $$ = 0; }
;

bliss_36_switch: /*nonfin*/ P_ASCIZ { $$=0; }
;

common_switch	:  U_IDENT '=' T_STRING  { $$ = 0; }
| U_LANGUAGE '(' language_list ')'  { $$ = 0; }
| U_LINKAGE '(' T_NAME ')'  { $$ = 0; }
| U_LIST '(' list_option_list ')'  { $$ = 0; }
/*| K_STRUCTURE '(' structure_attribute ')'  { $$ = 0; }*/
| U_MAIN '=' T_NAME  { $$ = 0; }
| U_OPTLEVEL '=' T_DIGITS  { $$ = 0; }
| U_VERSION '=' T_STRING  { $$ = 0; }

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
|"common" { $$ = 0; }
| language_name_list 
;
language_name_list: language_name_list ',' language_name 
|language_name 
;
language_name:  U_BLISS16  { $$ = 0; }
| U_BLISS32   { $$ = 0; }
| U_BLISS36   { $$ = 0; }
;
list_option_list: list_option_list list_option 
|list_option 
;
list_option:  U_SOURCE    { $$ = 0; }
| U_NOSOURCE     { $$ = 0; }
| U_REQUIRE     { $$ = 0; }
| U_NOREQUIRE    { $$ = 0; }
| U_EXPAND     { $$ = 0; }
| U_NOEXPAND    { $$ = 0; }
| U_TRACE    { $$ = 0; }
| U_NOTRACE    { $$ = 0; }
| U_LIBRARY    { $$ = 0; }
| U_NOLIBRARY    { $$ = 0; }
| U_OBJECT    { $$ = 0; }
| U_NOOBJECT    { $$ = 0; }
| U_ASSEMBLY    { $$ = 0; }
| U_NOASSEMBLY    { $$ = 0; }
| U_SYMBOLIC    { $$ = 0; }
| U_NOSYMBOLIC    { $$ = 0; }
| U_BINARY    { $$ = 0; }
| U_NOBINARY    { $$ = 0; }
| U_COMMENTARY    { $$ = 0; }
| U_NOCOMMENTARY     { $$ = 0; }
;

tname_list: tname_list ',' T_NAME 
|T_NAME 
;

tname_list2: tname_list2  T_NAME 
|T_NAME 
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

lowlevel: T_DIGITS 
| T_NAME 
;

/**** 2.0 EXPRESSIONS ***********************************************/
expression: 
primary  
| operator_expression { $$ = $1; }
| executable_function  
| control_expression   
| p_stuff ;

p_stuff:
P_REMAINING  { $$ = 0; }

primary: 
numeric_literal  
| string_literal  
| plit 
| T_NAME { 
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  $$ = build_external_ref ($1, yychar == '(');
}
| block  
| structure_reference 
| routine_call  { /* $$ = c_expand_expr_stmt ($1);*/ }
| field_reference 
| codecomment 
;
numeric_literal: 
decimal_literal  { $$=$1; /* TREE_TYPE($1)=integer_type_node;*/ }
| integer_literal 
| character_code_literal 
| float_literal  
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
|'+' { $$ = 0; }
|'-' { $$ = 0; }
;

integer_literal: 
P_B T_STRING  { $$ = 0; }
| P_O T_STRING  { $$ = 0; }
| P_DECIMAL T_STRING  { $$ = 0; }
| P_X T_STRING  { $$ = 0; }
;                  
/*
  P_B apo opt_sign T_NAME apo 
  | P_O apo opt_sign T_NAME apo 
  | P_DECIMAL apo opt_sign T_NAME apo 
  | P_X apo opt_sign T_NAME apo 
  */
						
integer_digit_list:  
integer_digit_list integer_digit 
|integer_digit 
;

integer_digit:   digits  
| T_NAME  
;

character_code_literal: P_C T_STRING { $$ = 0; }
;

/*The quoted_character is defined in the LEXICAL DEFINITION part of this
text. Here, only one quoted_character must appear.*/

float_literal: 
single_precision_float_literal 
| double_precision_float_literal 
| extended_exponent_double_precision_float_literal 
| extended_exponent_extended_precision_float_literal 
;

single_precision_float_literal:
P_E " mantissa {E exponent | nothing } " { $$ = 0; }
;

double_precision_float_literal:
P_D " mantissa { D exponent | nothing } " { $$ = 0; }
;

extended_exponent_double_precision_float_literal:
P_G " mantissa { { G | Q } exponent | nothing } " { $$ = 0; }
;

extended_exponent_extended_precision_float_literal:
P_H " mantissa { Q exponent | nothing } " { $$ = 0; }
;

mantissa: opt_sign
digits | digits '.' | '.' digits { $$=$2; } | digits '.' digits 
;

digits: digits T_DIGITS 
| T_DIGITS {}
;

string_literal:  string_type T_STRING2 
| T_STRING2 
;

/*T_STRING2 is a new extension nonfin*/
T_STRING2:
T_STRING 
|P_STRING '(' string_par_list ')' { $$ = 0; }
|P_CHAR '(' char_par_list ')' { $$ = 0; }
|P_CHESC { $$ = 0; }
;

string_par_list:string_par_list ',' string_par 
|string_par 
;

string_par:
numeric_literal 
|string_literal 
;

char_par_list:char_par_list ',' char_par 
|char_par 
;

char_par: ctce 
;

string_type:  P_ASCII  { $$ = 0; }
| P_ASCIZ   { $$ = 0; }
| P_ASCIC   { $$ = 0; }
| P_ASCID   { $$ = 0; }
| P_RAD50_11  { $$ = 0; }
| P_RAD50_10   { $$ = 0; }
| P_SIXBIT   { $$ = 0; }
| P_P   { $$ = 0; }
;

plit2:
K_PLIT  { $$ = 0; }
|K_UPLIT  { $$ = 0; }
;

plit3: { $$=0; }
| allocation_unit 
| psect_allocation
| psect_allocation allocation_unit
;

plit: plit2 plit3 '(' plit_item_list ')' 
;

plit_item_list: plit_item_list ',' plit_item 
|plit_item 
;

psect_allocation: K_PSECT '(' T_NAME ')' { $$ = 0; }
;

psect_name: T_NAME 

plit_item: 
plit_group  
| plit_expression 
| plit_string 
;

plit_group: 
allocation_unit 
| K_REP replicator K_OF  { $$ = 0; }
| K_REP replicator K_OF allocation_unit  { $$ = 0; }
;

allocation_unit:  K_LONG   { $$ = integer_type_node; }
| K_WORD   { $$ = short_integer_type_node; }
| K_BYTE  { $$ = char_type_node; }
;

replicator: ctce 
;

plit_expression: ltce 
;

plit_string: string_literal 
;

block: 
labeled_block 
| unlabeled_block
;

labeled_block: attached_label_list unlabeled_block { $$=chainon($1,$2); }
;

attached_label_list: attached_label_list attached_label 
| attached_label 
;

attached_label: 
T_NAME ':' 
;

unlabeled_block_start: K_BEGIN { 
#ifndef c99
  $$=c_begin_compound_stmt ();
#endif
}
;

unlabeled_block_start2: '(' { 
#ifndef c99
  $$=c_begin_compound_stmt ();
#endif
}
;

unlabeled_block: unlabeled_block_start
pushlevel block_body K_END poplevel
{ 
#ifndef c99
$$=poplevel (kept_level_p (), 1, 0);
 SCOPE_STMT_BLOCK (TREE_PURPOSE ($5))
   = SCOPE_STMT_BLOCK (TREE_VALUE ($5))
   = $$;
 RECHAIN_STMTS ($1, COMPOUND_BODY ($1)); 
 last_expr_type = NULL_TREE;
 $$=$1;
#else
 $$=0;
#endif
}
| unlabeled_block_start2 
pushlevel block_body ')' poplevel
{ 
#ifndef c99
$$=poplevel (kept_level_p (), 1, 0);
 SCOPE_STMT_BLOCK (TREE_PURPOSE ($5))
   = SCOPE_STMT_BLOCK (TREE_VALUE ($5))
   = $$;
 RECHAIN_STMTS ($1, COMPOUND_BODY ($1)); 
 last_expr_type = NULL_TREE;
 $$=$1;
#else 
 $$=0;
#endif
}
;

start_block: K_BEGIN { 
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
/* */
{
  $$=$3;
}
;
/*
maybe_declaration_list: { $$=0; }
| declaration_list 
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
|declaration 
;
/*
  maybe_block_action_list: { $$=0; }
  |block_action_list  
  ;
*/
maybe_block_action_list: { $$=0; }
|maybe_block_action_list block_action { 
  $$=chainon($1, $2); 
}
;

block_action_list: block_action_list block_action { $$ = chainon ($1, $2); }
|block_action 
;
/*The block_body must not be null.*/

block_action: expression ';' { 
  /*bli_add_stmt ($1);*/
  if ($1) $$=c_expand_expr_stmt($1);
}
;

maybe_block_value: { $$=0; }
|block_value 
;

block_value: expression 
;

structure_reference:
  ordinary_structure_reference 
| default_structure_reference 
| general_structure_reference 
;

ordinary_structure_reference:
T_NAME '[' access_actual_list ']' {
  tree t=build_external_ref ($1, 0);
  tree decl=build_array_declarator (t->exp.operands[3], NULL_TREE, 0, 0) ;
  decl->exp.operands[2]=t;
  tree type=integer_type_node;
  $$ = set_array_declarator_type (decl, type, 1);
  $$ = $3;
}
;

access_actual_list: access_actual_list ',' access_actual { $$=chainon($1,$3); }
|access_actual 
;

segment_name: T_NAME 
;

access_actual: { $$=0; }
|expression  
| field_name  
;

access_part:  
segment_expression ',' access_actual_list 
|segment_expression  
;

segment_expression: exp 
;

/*field_name      Note: See field_attribute, Section 4.1*/

field_name: T_NAME 
;

field_set_name: T_NAME 
;

dsr1:
primary  
| executable_function 
;

default_structure_reference: dsr1
'[' access_actual_list ']' 
;

general_structure_reference:
T_NAME '[' access_part ';' alloc_actual_list ']' 
|T_NAME '[' access_part ']' 
;

alloc_actual_list: alloc_actual_list ',' alloc_actual { $$ = chainon($1,$3); }
|alloc_actual 
;

routine_call: ordinary_routine_call  
| general_routine_call 
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
primary  
/*| executable_function */
;

io_list3: { $$=0; }
| io_actual_parameter_list  
| io_actual_parameter_list ';' io_actual_parameter_list  
| ';' io_actual_parameter_list  { $$ = 0; }
;

actual_parameter_list: actual_parameter_list ',' actual_parameter 
|actual_parameter 
;

io_actual_parameter_list: io_actual_parameter_list ',' io_actual_parameter { chainon ($1, build_tree_list (NULL_TREE, $3)); }
|io_actual_parameter { $$ = build_tree_list (NULL_TREE, $1);  }
;

io_actual_parameter: { $$=0 }
|expression { $$ = build_unary_op(ADDR_EXPR, $1, 0);
/* $$ =build_nt (ADDR_EXPR, $1); */
} 
;

general_routine_call:
T_NAME '(' routine_address ',' io_list3 ')'
|T_NAME '(' routine_address ')'
;

linkage_name: T_NAME 
;


routine_address: expression 
;

field_reference: 
/*address*//*makes stuff loop*/
/*|address field_selector  */
/*|*/address '<' position_exp ',' size_exp '>'
{
  $$=build (BIT_FIELD_REF, type_for_mode(TYPE_MODE (integer_type_node),1), $1, size_int ($5->int_cst.int_cst.low), bitsize_int ($3->int_cst.int_cst.low));
  $$=stabilize_reference($$);
}
;

address: 
primary  
| executable_function 
;

field_selector: '<' position_exp ',' size_exp '>' { $$ = 0; }
| '<' position_exp ',' size_exp ',' sign_ext_flag '>' { $$ = 0; }
;

sign_ext_flag: ctce
;

codecomment: K_CODECOMMENT quoted_string_list ':' block{ $$ = 0; }

;

quoted_string_list: quoted_string_list T_STRING 
|T_STRING 
;
/*
  operator_expression:
  fetch_expression  
  | prefix_expression 
  | infix_expression  
  ;
*/
op_exp:
primary  
| operator_expression  
|executable_function 
;

fetch_expression:
'.' op_exp12 { $$ = 0; }
;

operator_expression_not: 
op_exp12 
;

operator_expression:
'.' opexp9  { /* $$=$2; */ $$ = build_indirect_ref ($2, "unary *"); }
/*| '+' opexp9 %prec UMINUS {  $$->id="+"; }
| '-' opexp9 %prec UPLUS {  $$->id="-"; } nonfin*/
| opexp9 '^' opexp9 
| opexp9 K_MOD opexp9 
| opexp9 '*' opexp9 { $$ = parser_build_binary_op (MULT_EXPR, $1, $3); }
| opexp9 '/' opexp9 
| opexp9 '+' opexp9 { $$ = parser_build_binary_op (PLUS_EXPR, $1, $3); }
| opexp9 '-' opexp9 { $$ = parser_build_binary_op (MINUS_EXPR, $1, $3); }
| opexp9 infix_operator opexp9 {/* $$ = parser_build_binary_op ($2, $1, $3);*/ }
| K_NOT opexp9 { $$ = 0; }
| opexp9 K_AND opexp9 
|  opexp9 K_OR opexp9 
| opexp9 K_EQV opexp9 
| opexp9 K_XOR  opexp9 
| opexp9 '=' opexp9 { 
  tree t=$1;
  if (TREE_CODE(t) == INTEGER_CST) {
    t=make_pointer_declarator(0,$1);
    TREE_TYPE(t)=build_pointer_type(integer_type_node);
    $$=build_modify_expr(t, NOP_EXPR, $3);
  } else {
    $$=build_modify_expr(build_indirect_ref (t, "unary *"), NOP_EXPR, $3);
  }
}
;

opexp9:
primary  
| operator_expression  { /*$$=c_expand_expr_stmt($1); abort(); */}
|executable_function 
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
K_EQL   { $$ = 0; }
| K_EQLA   { $$ = 0; }
| K_EQLU  { $$ = 0; }
| K_NEQ   { $$ = 0; }
| K_NEQA   { $$ = 0; }
| K_NEQU   { $$ = 0; }
| K_LSS   { $$ = 0; }
| K_LSSA   { $$ = 0; }
| K_LSSU   { $$ = 0; }
| K_LEQ  { $$ = 0; }
| K_LEQA   { $$ = 0; }
| K_LEQU   { $$ = 0; }
| K_GTR   { $$ = 0; }
| K_GTRA   { $$ = 0; }
| K_GTRU   { $$ = 0; }
| K_GEQ   { $$ = 0; }
| K_GEQA  { $$ = 0; }
| K_GEQU   { $$ = 0; }
/* | K_AND  
 | K_OR  
 | K_EQV  
 | K_XOR  */
;


assign_expression: K_IF K_RETURN K_IF
/* op_exp '=' op_exp */
{
}
;


op_exp1: op_exp 
;
op_exp2:  '.' op_exp2 { $$=$2; }
| op_exp  
;
op_exp3:/*  '+' op_exp3 { $$=$2; }
				| '-' op_exp3 { $$=$2; }
				|*/ op_exp2  
;
op_exp4:  op_exp4 '^' op_exp3  
| op_exp3  
;
op_exp5:  op_exp5 K_MOD op_exp4  
| op_exp5 '/' op_exp4 
| op_exp5 '*' op_exp4  
| op_exp4  
;
op_exp6:  op_exp6 '+' op_exp5  
| op_exp6 '-' op_exp5   
| op_exp5  
;
op_exp7:  op_exp7 op7 op_exp6  
| op_exp6  
;
op7:  K_EQL  
| K_EQLA  
| K_EQLU  
| K_NEQ 
| K_NEQA 
| K_NEQU 
| K_LSS 
| K_LSSA 
| K_LSSU 
| K_LEQ 
| K_LEQA 
| K_LEQU 
| K_GTR 
| K_GTRA 
| K_GTRU 
| K_GEQ 
| K_GEQA 
| K_GEQU  
;
op_exp8:  K_NOT op_exp8 { $$=$2; }
| op_exp7  
;
op_exp9:  op_exp9 K_AND op_exp8  
| op_exp8  
;
op_exp10:  op_exp10 K_OR op_exp9  
| op_exp9  
;
op_exp11:  op_exp11 K_EQV op_exp10 
| op_exp11 K_XOR op_exp10  
| op_exp10  
;
op_exp12:  op_exp11 '=' op_exp12  
| op_exp11  
;
/*
  operator_expression: op_exp12 
  ;
*/
executable_function:
executable_function_name '('  actual_parameter_list  ')' {
  /* */
 }
|executable_function_name '(' ')' 
;

executable_function_named:
/* standard_function_name  
| linkage_function_name 
| character_handling_function_name 
| machine_specific_function_name 
|*/ cond_handling_function_name 
;


executable_function_name:
T_NAME  
| '%' T_NAME { $$ = 0; }
|executable_function_named 
;

lexical_function: 
lexical_function_name '(' lexical_actual_parameter ')' 
| lexical_function_name expression 
| lexical_function_name  
;

lexical_actual_parameter: lexeme_list 
;

lexical_function_name:
P_REMAINING { $$=0; }
/* really %name nonfin*/
;

actual_parameter: expression 
;
standard_function_name: T_NAME 
;
character_handling_function_name: T_NAME 
;
machine_specific_function_name: T_NAME 
;
cond_handling_function_name:
K_SIGNAL { $$ = 0; }
| K_STOP { $$ = 0; }
| K_SETUNWIND  { $$ = 0; }
;

size_exp: exp 
;

position_exp: exp 
;

control_expression:  conditional_expression  
| case_expression 
| select_expression 
| loop_expression 
| exit_expression 
| return_expression  
;

conditional_expression: 
/* K_IF exp K_THEN exp  K_ELSE exp ';' 
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
  $$=0;
}
/*K_IF exp K_THEN exp  K_ELSE exp 
|K_IF exp K_THEN exp  */
;

exp: expression 
;

case_expression: K_CASE exp K_FROM ctce K_TO ctce K_OF { $$=$2; }
K_SET case_line_list K_TES { $$=$2; }
;

case_line_list: case_line_list case_line 
|case_line 
;

case_line: '[' case_label_list ']' ':' case_action ';' { $$=$2; }
;

case_label_list: case_label_list ','  case_label 
|case_label 
;

case_label: { $$=0; }
| ctce K_TO ctce 
| K_INRANGE { $$=0; }
| K_OUTRANGE { $$=0; }
;
case_action: expression 
;


select_expression: select_type select_index K_OF K_SET select_line_list K_TES
{ $$=$2; }
;
select_type:  K_SELECT | K_SELECTA | K_SELECTU
| K_SELECTONE | K_SELECTONEA | K_SELECTONEU 
;
select_index: expression 
;

select_line_list: select_line_list select_line 
|select_line 
;

select_line: '[' select_label_list ']' ':' select_action ';' { $$=$2; }
;
select_label_list: select_label_list select_label 
|select_label 
;

select_label:  exp  
| exp K_TO exp  
| K_OTHERWISE { $$ = 0; }
| K_ALWAYS { $$ = 0; }
;
select_action: expression 
;


loop_expression:  indexed_loop_expression  
| tested_loop_expression  
;
indexed_loop_expression:
indexed_loop_type
{
  $<type_node_p>$ = build_stmt (FOR_STMT, NULL_TREE, NULL_TREE,
			  NULL_TREE, NULL_TREE);
  add_stmt ($<type_node_p>$);  
}
 T_NAME
K_FROM exp   K_TO exp   K_BY exp  K_DO exp 
;
indexed_loop_type:
K_INCR | K_INCRA | K_INCRU  | K_DECR | K_DECRA | K_DECRU 
;
tested_loop_expression:
pre_tested_loop  
| post_tested_loop  
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
  $$=0;
 } 
| K_UNTIL  exp K_DO exp { $$ = 0; }
;
post_tested_loop:
K_DO exp K_WHILE exp  { $$ = 0; }
| K_DO exp K_UNTIL  exp { $$ = 0; }
;


exit_expression:  leave_expression  
| exitloop_expression  
;
leave_expression: 
K_LEAVE T_NAME K_WITH exp  { $$ = 0; }
|K_LEAVE T_NAME  { $$ = 0; }
;
exitloop_expression: 
K_EXITLOOP  exp  { $$=$2; }
|K_EXITLOOP    { $$=0; }
;

return_expression: 
K_RETURN  exp { $$ = c_expand_return (build_compound_expr($2)); $$=0; }
|K_RETURN { $$ = c_expand_return (NULL_TREE); $$=0; }
;
/**** 3.0 CONSTANT EXPRESSIONS **************************************/
/**** 4.0 DECLARATIONS **********************************************/
declaration: data_declaration 
| structure_declaration 
| field_declaration  
| routine_declaration 
| linkage_declaration  
| enable_declaration 
| bound_declaration  
| compiletime_declaration 
| macro_declaration  
| require_declaration 
| library_declaration  
| psect_declaration 
| switches_declaration  
| label_declaration 
| builtin_declaration  
| undeclare_declaration 
;

attribute_list:attribute_list attribute { 
$$ = tree_cons (NULL_TREE, $1, $2); 
}
|attribute 
;
attribute:  allocation_unit 
| extension_attribute 
| structure_attribute  
| field_attribute 
| alignment_attribute  
| initial_attribute 
| preset_attribute 
| { undefmode=1; } psect_allocation{ undefmode=0; $$=$2; }
| volatile_attribute 
| novalue_attribute 
| linkage_attribute  
| range_attribute 
| { undefmode=1; fprintf(stderr, "undefmode\n\n\n\n"); } addressing_mode_attribute { undefmode=0; $$=$2; } 
| weak_attribute  
;
/*
  allocation_unit:  K_LONG | K_WORD | K_BYTE 
  ;
*/
/*extension_attribute:  K_SIGNED | K_UNSIGNED 
;*/
structure_attribute:
  K_REF T_NAME '[' alloc_actual_list ']' { $$ = 0; }
|  K_REF T_NAME  { $$ = 0; }
|   T_NAME '[' alloc_actual_list ']' { 
  //$$ = build_nt(STRUCTURE_ATTR,$1,$3); 
  tree t=find_struct(mystructs,$1);
  tree decl=build_array_declarator (t->exp.operands[3], NULL_TREE, 0, 0) ;
  decl->exp.operands[2]=t;
  tree type=integer_type_node;
  $$ = set_array_declarator_type (decl, type, 1);
  TREE_TYPE(decl)=STRUCTURE_DECL;
}
|   T_NAME  
;

extension_attribute: K_SIGNED { $$ = 0; }
|K_UNSIGNED { $$ = 0; }
;
/*
  field_attribute: K_FIELD 
  ;
*/
alignment_attribute: K_ALIGN  compile_time_constant_expression { $$ = 0; }
;
/*
  preset_attribute: K_PRESET 
  ;
*/
/*
psect_allocation: K_PSECT 
;
*/

volatile_attribute: K_VOLATILE { $$ = 0; }
;

novalue_attribute: K_NOVALUE { $$ = 0; }
;
linkage_attribute: T_NAME 
;
range_attribute: K_SIGNED compile_time_constant_expression { $$ = 0; }
|K_UNSIGNED compile_time_constant_expression { $$ = 0; }
;

ctce: compile_time_constant_expression 
;

ltce: linkage_time_constant_expression 
;
linkage_time_constant_expression:expression { $$=$1; /*nonfin*/ }
;

compile_time_constant_expression: expression { $$=$1; /*nonfin*/ }
;

addressing_mode_attribute: 
/*K_ADDRESSING_MODE { undefmode=1; } '(' mode_16 ')' 
  |*/{ undefmode=1; fprintf(stderr, "undefmode\n\n\n\n"); } K_ADDRESSING_MODE '('  mode_32 ')' {  undefmode=0; }
;

mode_16:
  U_ABSOLUTE { $$ = 0; }
|U_RELATIVE { $$ = 0; }
;

mode_32:
U_ABSOLUTE { $$ = 0; }
|U_GENERAL { $$ = 0; }
|U_LONG_RELATIVE { $$ = 0; }
|U_WORD_RELATIVE { $$ = 0; }
;

weak_attribute: K_WEAK { $$ = 0; }
;

/*
alloc_actual_list: alloc_actual_list alloc_actual|alloc_actual;
*/


alloc_actual:  { $$=(int) 0; }
| ctce 
|allocation_unit 
|extension_attribute 
;

field_attribute: K_FIELD field_stuff_list { $$ = 0; }
;

field_stuff_list: field_stuff_list ','  field_name 
| field_stuff_list ','  field_set_name
| field_set_name 
| field_name 
;

initial_attribute: K_INITIAL '(' initial_item_list ')' { $$ = 0; }
;

initial_item_list: initial_item_list ',' initial_item 
|initial_item 
;

initial_item: 
initial_group  
| initial_expression 
| initial_string 
;

initial_group: 
allocation_unit '(' initial_item_list ')' 
| K_REP replicator K_OF '(' initial_item_list ')'  { $$ = 0; }
| K_REP replicator K_OF allocation_unit '(' initial_item_list ')'  { $$ = 0; }
/*| '(' initial_item_list ')' */
;

/*
replicator: ctce 
;
*/

initial_expression: expression 
;

/*
An initial_expression must be an ltce for OWN and GLOBAL declarations.
*/

initial_string: string_literal 
;

preset_item_list: preset_item_list ',' preset_item|preset_item;

preset_attribute: K_PRESET '(' preset_item_list ')' { $$ = 0; }
;

preset_item:  '[' ctce_access_actual_list ']' '=' preset_value { $$ = 0; }
;

ctce_access_actual_list: ctce_access_actual_list ',' ctce_access_actual|ctce_access_actual;

ctce_access_actual: 
ctce 
| field_name 
;

preset_value: expression 
;

psect_allocation: K_PSECT '(' T_NAME ')' { $$ = 0; }
;
/*
  volatile_attribute: K_VOLATILE
  ;
*/

data_declaration:  own_declaration {
  $$ = $1;
}
| global_declaration 
| external_declaration  
| forward_declaration 
| local_declaration  
| stackglocal_declaration 
| register_declaration 
| global_register_declaration 
| external_register_declaration 
| map_declaration  
;

/*
own_declaration_list: own_declaration_list own_declaration 
|own_declaration;
*/

own_declaration: K_OWN own_item_list ';' {
  $$ = $2;
}
;

own_item_list: own_item_list ',' own_item
| declspecs_ts setspecs own_item
;

own_item: T_NAME maybe_own_attribute_list setspecs { //maybe... is a declspecs
  tree c, p , d, i, t;
  char * s;
  int e,f;

  tree type = $2;

  if ($2==0) type = integer_type_node;

  t = tree_cons (NULL_TREE, type, NULL_TREE); 
  TREE_STATIC ($$) = 0;

  s=malloc($1->identifier.id.len+2);
  strcpy(s,$1->identifier.id.str);
  s[$1->identifier.id.len]='_';
  s[$1->identifier.id.len+1]=0;
  c=get_identifier(s);
  TREE_TYPE(c)=integer_type_node;

  d = start_decl (c, current_declspecs, 0,
                       chainon (NULL_TREE, all_prefix_attributes));
  finish_decl (d, 0, NULL_TREE);
  p = make_pointer_declarator(0,$1);
  d = start_decl (p, current_declspecs, 1,
                       chainon (NULL_TREE, all_prefix_attributes));

  start_init(d,NULL,global_bindings_p());
  finish_init();
  i = build_unary_op (ADDR_EXPR, build_external_ref (c, 0), 0);

  finish_decl (d, i, NULL_TREE);
}
;

maybe_own_attribute_list: { $$ = 0; }
| ':' own_attribute_list { $$ = $2; }
;

own_attribute_list:
own_attribute_list own_attribute { 
  $$= tree_cons (NULL_TREE, $2, $1);
}
|own_attribute 
;

own_attribute:
allocation_unit { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
|extension_attribute 
|structure_attribute 
|field_attribute 
|alignment_attribute 
|initial_attribute 
|preset_attribute 
|psect_allocation 
|volatile_attribute 
;

global_declaration: K_GLOBAL global_item_list  ';' { $$ = 0; }
;

global_item_list:  global_item_list global_item 
|global_item 
;

global_item: T_NAME ':' attribute_list 
;

external_declaration: K_EXTERNAL external_item_list ';' { $$ = 0; }
;

external_item_list: external_item_list external_item 
|external_item 
;

external_item: T_NAME ':' attribute_list 
;

forward_declaration: K_FORWARD forward_item_list ';'  { $$ = 0; }
;

forward_item: T_NAME ':' attribute_list 
;

forward_item_list: forward_item_list forward_item 
|forward_item 
;

local_declaration: K_LOCAL local_item_list ';'  { $$ = 0; }
;

local_item_list: local_item_list ',' local_item 
|local_item 
;

local_item: 
T_NAME ':' attribute_list 
|T_NAME 
;

stackglocal_declaration: K_STACKLOCAL local_item_list ';' { $$ = 0; }
;

register_declaration: K_REGISTER register_item_list ';' { $$ = 0; }
;

register_item_list: register_item_list register_item 
|register_item 
;

register_item: T_NAME '=' ctce ':' attribute_list 
;

global_register_declaration:
K_GLOBAL K_REGISTER global_reg_item_list ';' { $$ = 0; }
;

global_reg_item_list: global_reg_item_list global_reg_item 
|global_reg_item 
;

global_reg_item: T_NAME '=' ctce attribute_list 
;

external_register_declaration:
K_EXTERNAL K_REGISTER external_reg_item_list ';' { $$ = 0; }
;

external_reg_item_list:external_reg_item_list external_reg_item 
|external_reg_item 
;

external_reg_item: T_NAME '=' ctce ':' attribute_list 
;

map_declaration: K_MAP map_item_list ';' { $$ = 0; }
;

map_item_list: map_item_list ',' map_item 
|map_item 
;

map_item: T_NAME ':' attribute_list 
;

structure_declaration:
  K_STRUCTURE structure_definition_list ';' { $$ = 0; }
;

structure_definition_list: structure_definition_list ',' structure_definition 
|structure_definition 
;

structure_definition:
  declspecs_ts setspecs T_NAME '['
{
  //$5
  $$ = start_struct(RECORD_TYPE, $3);
}
  access_formal_list  ';' 
{
  //$8
}
allocation_formal_list ']' '='
{
  //$12
  tree allocfn;
  tree allocid;
  char *allocfnname=malloc($3->identifier.id.len+2);
  strcpy(allocfnname,$3->identifier.id.str);
  allocfnname[$3->identifier.id.len]='_';
  allocfnname[$3->identifier.id.len+1]=0;
  allocid=get_identifier(allocfnname);
  tree v = build_tree_list (NULL_TREE, NULL_TREE);
  //mydeclares($9);
  v = $9;
  allocfn = build_nt (CALL_EXPR, allocid, v, NULL_TREE);
  start_function (current_declspecs, allocfn, all_prefix_attributes);
  store_parm_decls ();
  //begin_stmt_tree(&$<type_node_p>$);
  $<type_node_p>12=allocfn;
}
structure_size
{
  //$14
  finish_function (0, 1); 
  POP_DECLSPEC_STACK;

  tree accessfn;
  tree accessid;
  char *accessfnname=malloc($3->identifier.id.len+2);
  strcpy(accessfnname,$3->identifier.id.str);
  accessfnname[$3->identifier.id.len]='_';
  accessfnname[$3->identifier.id.len+1]='_';
  accessfnname[$3->identifier.id.len+2]=0;
  accessid=get_identifier(accessfnname);

  PUSH_DECLSPEC_STACK;
  //mydeclares($6);
  tree v = $9;
  accessfn = build_nt (CALL_EXPR, accessid, v, NULL_TREE);
  start_function (current_declspecs, accessfn, all_prefix_attributes);
  store_parm_decls ();
  //begin_stmt_tree(&$<type_node_p>$);
  $<type_node_p>14=accessfn;
}
structure_body
{
  //$16

  finish_function (0, 1); 
  POP_DECLSPEC_STACK;

  tree comp;  
  tree alloc;
  tree access;

  alloc = grokfield (input_filename, lineno, $<type_node_p>12, 0, 0);
  access = grokfield (input_filename, lineno, $<type_node_p>14, 0, 0);

  //$$ = finish_structure ( $<type_node_p>5, 0, $6, $9 ,$13, $15, 0);

  comp=alloc;
  chainon(alloc,access);

  $$ = finish_struct ($<type_node_p>5, comp, NULL);

  //$$ = build_nt (STRUCTURE_DECL, $3, $6, $9, $13, $15);
  //add_struct(&mystructs,$$); 
}
;

allocation_formal_list: allocation_formal_list ',' allocation_formal 
{ 
  //$$=chainon ($1, $3); 
//$$ = tree_cons (NULL_TREE, $1, $3);
  $$ = get_parm_info (1);
}
|allocation_formal { 
  //$$=$1; 
$$ = get_parm_info (1);
 }
;

allocation_formal:
allocation_name { 
#if 1
  tree t = tree_cons (0, integer_type_node, 0);   
  tree d = tree_cons (t, $1, 0);
 $$ = tree_cons (d, 0, 0);
 push_parm_decl($$);
#else
  tree e=tree_cons(0,integer_type_node,0);
  tree f=tree_cons(e,$1,0); 
  $$ = tree_cons (f,0,0);
#endif
}
|allocation_name '=' allocation_default { $$ = tree_cons ($3, $1, NULL_TREE); }
;

allocation_default: exp 
;

structure_size: { $$ = 0; }
|
{
  $<type_node_p>1=c_begin_compound_stmt ();
}
pushlevel '[' expression ']' poplevel { 
  //$$ = $2; 
$$=poplevel (kept_level_p (), 1, 0);
 SCOPE_STMT_BLOCK (TREE_PURPOSE ($6))
   = SCOPE_STMT_BLOCK (TREE_VALUE ($6))
   = $$;
 RECHAIN_STMTS ($<type_node_p>1, COMPOUND_BODY ($<type_node_p>1)); 
 last_expr_type = NULL_TREE;
 $$=$<type_node_p>1;

}
;

structure_body: 
{ $<type_node_p>1 = c_begin_compound_stmt; }
pushlevel expression poplevel
{
$$=poplevel (kept_level_p (), 1, 0);
 SCOPE_STMT_BLOCK (TREE_PURPOSE ($4))
   = SCOPE_STMT_BLOCK (TREE_VALUE ($4))
   = $$;
 RECHAIN_STMTS ($<type_node_p>1, COMPOUND_BODY ($<type_node_p>1)); 
 last_expr_type = NULL_TREE;
 $$=$<type_node_p>1;
} 


access_formal_list: access_formal_list ',' access_formal { 
  //$$=chainon($1,$3); 
  $$ = get_parm_info (1);
}
| access_formal 
{
  $$ = get_parm_info (1);
  //$$=$1;
}
;

access_formal: T_NAME {
//tree d = start_decl ($1, current_declspecs, 0,
//  chainon (NULL_TREE, all_prefix_attributes));
//finish_decl (d, 0, NULL_TREE);
#if 1
  tree e=tree_cons(0,integer_type_node,0);
  tree f=tree_cons(e,$1,0); 
  $$ = tree_cons (f,0,0);
  push_parm_decl($$); 
#else
  tree e=tree_cons(0,integer_type_node,0);
  tree f=tree_cons(e,$1,0); 
  $$ = tree_cons (f,0,0);
#endif
}
;

allocation_name: T_NAME {
  $$ = $1;
}
;

field_declaration: K_FIELD { $$ = 0; }
 
;

routine_declaration:
ordinary_routine_declaration 
|global_routine_declaration 
|forward_routine_declaration 
|external_routine_declaration 
;
ordinary_routine_declaration: 
K_ROUTINE routine_definition_list ';' { 
  $$ = $2;
}
;

routine_definition_list: routine_definition_list ',' routine_definition 
|routine_definition 
;

routine_definition: declspecs_ts setspecs T_NAME 
{
}
io_list 
{
  void * xyz = $5;
  void * v = $5;
  if (v==0) v=build_tree_list (NULL_TREE, NULL_TREE);
  void * vo = build_nt (CALL_EXPR, $3, v, NULL_TREE);
  start_function (current_declspecs, vo, all_prefix_attributes);
  store_parm_decls ();
}
routine_attributes '=' save_filename save_lineno exp 
{ 
  finish_function (0, 1); 
  POP_DECLSPEC_STACK;
}
;

routine_attributes: { $$=0; }
| ':' routine_attribute_list { $$=$2; }
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
io_list ':' routine_attribute_list 
|io_list 
;

io_list: { $$=0; }
| {
  pushlevel (0);
  clear_parm_order ();
  declare_parm_level (1);
}
'(' formal_item_list ')'
{ 
  parmlist_tags_warning ();
  poplevel (0, 0, 0);
  $$ = $3;
}
/*  |'(' formal_item_list ';' formal_item_list ')' 
    |'(' ';' formal_item_list ')' */
;

formal_item_list:
/* empty */ { $$ = 0 ; } 
|formal_item_list ','  formal_item { $$ = get_parm_info (1); }
|formal_item { $$ = get_parm_info (1);  }
;

formal_item: /*T_NAME ':' formal_attribute_list 
|*/declspecs_ts setspecs  T_NAME { 

  tree type = integer_type_node;
  tree t = tree_cons (NULL_TREE, type, NULL_TREE);
  tree p = make_pointer_declarator (0, $3);

  tree e = tree_cons (t, p, 0);

  $$ = tree_cons (e, 0, chainon (NULL, all_prefix_attributes));




 POP_DECLSPEC_STACK;
 push_parm_decl ($$);

}
;
routine_attribute_list:
routine_attribute_list routine_attribute 
|routine_attribute 
;
formal_attribute_list:  map_declaration_attribute_list  
;
/*
  ?? nonfin
  formal_attribute_list: '(' map_declaration_attribute_list ')' { $$=$2; }
  ;
*/
map_declaration_attribute_list: map_declaration_attribute_list map_declaration_attribute 
|map_declaration_attribute 
;

map_declaration_attribute:
allocation_unit 
|extension_attribute 
|structure_attribute 
|field_attribute 
|volatile_attribute 
;
routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = 0; }
|weak_attribute 
;

global_routine_declaration:
K_GLOBAL K_ROUTINE global_routine_definition_list ';' { $$ = 0; }
;
global_routine_definition_list: global_routine_definition_list ',' global_routine_definition 
|global_routine_definition 
;
global_routine_definition: T_NAME io_list ':' global_routine_attribute_list '=' exp 
|T_NAME io_list '=' exp 
;

global_io_list2: 
io_list ':'  global_routine_attribute_list 
|io_list  
;

global_routine_attribute_list: { $$=0; }
|global_routine_attribute_list global_routine_attribute 
|global_routine_attribute 
;
global_routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = 0; }
|weak_attribute 
;

forward_routine_declaration:
K_FORWARD K_ROUTINE forward_routine_item_list ';' { $$ = 0; }
;

forward_routine_item_list: forward_routine_item_list ',' forward_routine_item 
|forward_routine_item  
;
forward_routine_item: 
T_NAME maybe_forward_routine_attribute_list 
/*|T_NAME  */
;
maybe_forward_routine_attribute_list: { $$=0; }
|':' forward_routine_attribute_list { $$=$2; }
;
forward_routine_attribute_list:
forward_routine_attribute_list forward_routine_attribute 
|forward_routine_attribute 
;
forward_routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = 0; }
;
external_routine_declaration:
K_EXTERNAL K_ROUTINE external_routine_item_list ';' { $$ = 0; }
;

external_routine_item_list: external_routine_item_list ',' external_routine_item 
|external_routine_item 
;
external_routine_item: T_NAME ':' ext_routine_attribute_list 
| T_NAME { 
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  $$ = build_external_ref ($1, 1);
}
;
ext_routine_attribute_list:
ext_routine_attribute_list ext_routine_attribute 
|ext_routine_attribute 
;
ext_routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = 0; }
|weak_attribute 
;
linkage_declaration: K_LINKAGE linkage_definition_list ';' { $$ = 0; }
;
enable_declaration: K_ENABLE T_NAME '(' tname_list ')' ';'  { $$ = 0; }
|K_ENABLE T_NAME ';'  { $$ = 0; }
;
bound_declaration: 
literal_declaration 
|external_literal_declaration 
|bind_data_declaration 
|bind_routine_declaration 
;
compiletime_declaration: K_COMPILETIME { $$ = 0; }
 ;
macro_declaration: 
positional_macro_declaration 
|keyword_macro_declaration 
;
require_declaration: K_REQUIRE T_STRING ';' { $$ = 0; }
 ;
library_declaration: K_LIBRARY T_STRING ';' { $$ = 0; }
 ;
psect_declaration: K_PSECT psect_item_list ';' { $$ = 0; }
 ;
switches_declaration: K_SWITCHES { $$ = 0; }
 ;
label_declaration: K_LABEL { $$ = 0; }
 ;
builtin_declaration: K_BUILTIN { $$ = 0; }
 ;
undeclare_declaration: K_UNDECLARE tname_list ';' { $$ = 0; }
 ;

literal_declaration: K_LITERAL literal_item_list ';' { $$ = 0; }
| K_GLOBAL K_LITERAL literal_item_list { $$ = 0; }
;

literal_item_list: literal_item_list ',' literal_item 
| literal_item 
;

literal_item: T_NAME '=' compile_time_constant_expression ':' literal_attribute_list 
|T_NAME '=' compile_time_constant_expression 
;

literal_attribute_list:
literal_attribute_list literal_attribute 
|literal_attribute 
;

literal_attribute: weak_attribute 
|range_attribute 
;
external_literal_declaration: K_EXTERNAL K_LITERAL external_literal_item_list ';' { $$ = 0; }
;

external_literal_item_list: external_literal_item_list ',' external_literal_item 
| external_literal_item 
;

external_literal_item: T_NAME  ':' literal_attribute_list 
|T_NAME  
;

bind_data_declaration: K_BIND bind_data_item_list ';'  { $$ = 0; }
| K_GLOBAL K_BIND bind_data_item_list { $$ = 0; }
;

bind_data_item_list: bind_data_item_list ',' bind_data_item 
| bind_data_item 
;

bind_data_item: T_NAME '=' expression ':' bind_data_attribute_list 
|T_NAME '=' expression 
;

bind_data_attribute_list:
bind_data_attribute_list bind_data_attribute 
|bind_data_attribute 

bind_data_attribute:
allocation_unit 
|extension_attribute 
|structure_attribute 
|field_attribute 
|volatile_attribute 
|weak_attribute 
;

bind_routine_declaration: K_BIND K_ROUTINE bind_routine_item_list ';'  { $$ = 0; }
| K_GLOBAL K_BIND K_ROUTINE bind_routine_item_list { $$ = 0; }
;

bind_routine_item_list: bind_routine_item_list ',' bind_routine_item 
| bind_routine_item 
;

bind_routine_item: T_NAME '=' expression ':' bind_routine_attribute_list 
|T_NAME '=' expression 
;

bind_routine_attribute_list:
bind_routine_attribute_list bind_routine_attribute 
|bind_routine_attribute 

bind_routine_attribute:
linkage_attribute 
|novalue_attribute 
|weak_attribute 
;

positional_macro_declaration: K_MACRO positional_macro_definition_list ';'  { $$ = 0; }
;

positional_macro_definition_list: positional_macro_definition_list ',' positional_macro_definition 
|positional_macro_definition 
;

positional_macro_definition:
simple_macro_definition 
|conditional_macro_definition 
|iterative_macro_definition 

simple_macro_definition: 
T_NAME '(' tname_list ')'  '=' macro_body '%' 
|T_NAME  '=' macro_body '%'
;
conditional_macro_definition:
T_NAME '(' tname_list ')'  '[' ']' '=' macro_body '%' 
|T_NAME '[' ']' '=' macro_body '%'
;
iterative_macro_definition:
T_NAME '(' tname_list ')' '[' tname_list ']' '=' macro_body '%' 
|T_NAME '[' tname_list ']' '=' macro_body '%'
;

default_actual: { $$=0; }
| lexeme_list 
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
keyword_macro_declaration: K_KEYWORDMACRO keyword_macro_definition_list ';'  { $$ = 0; }
;

keyword_macro_definition_list:
keyword_macro_definition_list ',' keyword_macro_definition 
|keyword_macro_definition 
;

keyword_macro_definition:
T_NAME '(' keyword_pair_list ')' '=' macro_body '%' 
;

keyword_pair_list: keyword_pair_list ',' keyword_pair 
|keyword_pair 
;

keyword_pair:
T_NAME '=' default_actual 
|T_NAME 
;

linkage_definition_list: linkage_definition_list ',' linkage_definition 
|linkage_definition 
;
linkage_definition: T_NAME '=' linkage_type 
;

linkage_type: U_CALL  { $$ = 0; }
| T_NAME 
;

input_parameter_location_list:input_parameter_location_list ',' input_parameter_location 
|input_parameter_location 
;
output_parameter_location_list:output_parameter_location_list ',' output_parameter_location 
|output_parameter_location 
;

input_parameter_location: U_STANDARD  { $$ = 0; }
|K_REGISTER '=' T_DIGITS  { $$ = 0; }
| { $$=0; }
;

output_parameter_location:
'(' K_REGISTER '=' T_DIGITS ')' { $$ = 0; }
;

psect_item_list: psect_item_list ',' psect_item 
|psect_item 
;

psect_attribute_list: psect_attribute_list ',' psect_attribute 
|psect_attribute { $$ = 0; }
;

psect_item:
storage_class '=' T_NAME '(' psect_attribute_list ')'  { $$ = $3; }
|storage_class '=' T_NAME { $$ = $3; }
;

storage_class:
K_OWN 
|K_GLOBAL 
|K_PLIT 
|U_CODE 
|U_NODEFAULT 
;

psect_attribute:
U_WRITE   { $$ = 0; }
| U_NOWRITE  { $$ = 0; }
|U_EXECUTE  { $$ = 0; }
| U_NOEXECUTE  { $$ = 0; }
|U_OVERLAY  { $$ = 0; }
| U_CONCATENATE  { $$ = 0; }
|  b32_psect_attribute 
;

b32_psect_attribute:
U_READ { $$ = 0; }
|U_NOREAD  { $$ = 0; }
|U_SHARE  { $$ = 0; }
|U_NOSHARE  { $$ = 0; }
|U_PIC  { $$ = 0; }
|U_NOPIC  { $$ = 0; }
|K_LOCAL  { $$ = 0; }
|K_GLOBAL  { $$ = 0; }
|U_VECTOR  { $$ = 0; }
|alignment_attribute 
|addressing_mode_attribute { $$ = 0; }
;


/**** 5.0 LEXICAL PROCESSING FACILITIES *****************************/
/**** 7.0 MACHINE SPECIFIC NAMES ************************************/

builtin_name: T_NAME  
| machine_specific_function  
| linkage_function 
;

machine_specific_function: T_NAME
;

linkage_function: T_NAME
;

linkage_function_name: T_NAME 
;

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

void 
parse_init ()
{
  //z1zin=fopen(input_filename,"r");
  //current_token=NULL;
  //first_available_token=first_token;
#ifdef YYDEBUG
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

  bli_init_reswords();

  ggc_add_tree_root (&declspec_stack, 1);
  ggc_add_tree_root (&current_declspecs, 1);
  ggc_add_tree_root (&prefix_attributes, 1);
  ggc_add_tree_root (&all_prefix_attributes, 1);
  
}

void yy2error (char *s)
{
  if (s)fprintf( stderr,"\n\n%s\n",s); 
  fprintf (stderr, "Nu b;lev det fel %d\n",linenumb);
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
  init_reswords ();

parse_init();
}

int
cpp_handle_option (pfile, argc, argv, ignore)
     cpp_reader *pfile;
     int argc;
     char **argv;
     int ignore;
{
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

unsigned int
cpp_errors (pfile)
     cpp_reader *pfile;
{
  //return pfile->errors;
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

/* This is actually the C default */

tree
make_pointer_declarator (type_quals_attrs, target)
     tree type_quals_attrs, target;
{
  tree quals, attrs;
  tree itarget = target;
  split_specs_attrs (type_quals_attrs, &quals, &attrs);
  if (attrs != NULL_TREE)
    itarget = tree_cons (attrs, target, NULL_TREE);
  return build1 (INDIRECT_REF, quals, itarget);
}

/* borrowed from cp */

#if 0
tree
make_reference_declarator (cv_qualifiers, target)
     tree cv_qualifiers, target;
{
  if (target)
    {
      if (TREE_CODE (target) == ADDR_EXPR)
        {
          error ("cannot declare references to references");
          return target;
        }
      if (TREE_CODE (target) == INDIRECT_REF)
        {
          error ("cannot declare pointers to references");
          return target;
        }
      if (TREE_CODE (target) == IDENTIFIER_NODE && ANON_AGGRNAME_P (target))
	error ("type name expected before `&'");
    }
  target = build_nt (ADDR_EXPR, target);
  TREE_TYPE (target) = cv_qualifiers;
  return target;
}
#endif

void
add_struct(struct structure ** s,tree elem) {
  struct structure * t=malloc(sizeof(*t));
  t->next=*s;
  t->elem=elem;
  *s=t;
}

tree
find_struct(struct structure * s,tree elem) {
  struct structure * t=s;
  while (t->elem && t->elem->exp.operands[0]!=elem) t=t->next;
  return t->elem;
}

void
c_set_yydebug (value)
     int value;
{
#if YYDEBUG != 0
  yydebug = value;
#else
  warning ("YYDEBUG not defined");
#endif
}

/* The reserved keyword table.  */
struct resword
{
  const char *word;
  ENUM_BITFIELD(rid) rid : 16;
  unsigned int disable   : 16;
};

extern int flag_no_asm;

/* Disable mask.  Keywords are disabled if (reswords[i].disable & mask) is
   _true_.  */
#define D_TRAD  0x01    /* not in traditional C */
#define D_C89   0x02    /* not in C89 */
#define D_EXT   0x04    /* GCC extension */
#define D_EXT89 0x08    /* GCC extension incorporated in C99 */
#define D_OBJC  0x10    /* Objective C only */

static const struct resword reswords[] =
{
  { "_Bool",            RID_BOOL,       0 },
  { "_Complex",         RID_COMPLEX,    0 },
  { "__FUNCTION__",     RID_FUNCTION_NAME, 0 },
  { "__PRETTY_FUNCTION__", RID_PRETTY_FUNCTION_NAME, 0 },
  { "__alignof",        RID_ALIGNOF,    0 },
  { "__alignof__",      RID_ALIGNOF,    0 },
  { "__asm",            RID_ASM,        0 },
  { "__asm__",          RID_ASM,        0 },
  { "__attribute",      RID_ATTRIBUTE,  0 },
  { "__attribute__",    RID_ATTRIBUTE,  0 },
  { "__bounded",        RID_BOUNDED,    0 },
  { "__bounded__",      RID_BOUNDED,    0 },
  { "__builtin_choose_expr", RID_CHOOSE_EXPR, 0 },
  { "__builtin_types_compatible_p", RID_TYPES_COMPATIBLE_P, 0 },
  { "__builtin_va_arg", RID_VA_ARG,     0 },
  { "__complex",        RID_COMPLEX,    0 },
  { "__complex__",      RID_COMPLEX,    0 },
  { "__const",          RID_CONST,      0 },
  { "__const__",        RID_CONST,      0 },
  { "__extension__",    RID_EXTENSION,  0 },
  { "__func__",         RID_C99_FUNCTION_NAME, 0 },
  { "__imag",           RID_IMAGPART,   0 },
  { "__imag__",         RID_IMAGPART,   0 },
  { "__inline",         RID_INLINE,     0 },
  { "__inline__",       RID_INLINE,     0 },
  { "__label__",        RID_LABEL,      0 },
  { "__ptrbase",        RID_PTRBASE,    0 },
  { "__ptrbase__",      RID_PTRBASE,    0 },
  { "__ptrextent",      RID_PTREXTENT,  0 },
  { "__ptrextent__",    RID_PTREXTENT,  0 },
  { "__ptrvalue",       RID_PTRVALUE,   0 },
  { "__ptrvalue__",     RID_PTRVALUE,   0 },
  { "__real",           RID_REALPART,   0 },
  { "__real__",         RID_REALPART,   0 },
  { "__restrict",       RID_RESTRICT,   0 },
  { "__restrict__",     RID_RESTRICT,   0 },
  { "__signed",         RID_SIGNED,     0 },
  { "__signed__",       RID_SIGNED,     0 },
  { "__typeof",         RID_TYPEOF,     0 },
  { "__typeof__",       RID_TYPEOF,     0 },
  { "__unbounded",      RID_UNBOUNDED,  0 },
  { "__unbounded__",    RID_UNBOUNDED,  0 },
  { "__volatile",       RID_VOLATILE,   0 },
  { "asm",              RID_ASM,        D_EXT },
  { "auto",             RID_AUTO,       0 },
  { "break",            RID_BREAK,      0 },
  { "case",             RID_CASE,       0 },
  { "char",             RID_CHAR,       0 },
  { "const",            RID_CONST,      D_TRAD },
  { "continue",         RID_CONTINUE,   0 },
  { "default",          RID_DEFAULT,    0 },
  { "do",               RID_DO,         0 },
  { "double",           RID_DOUBLE,     0 },
  { "else",             RID_ELSE,       0 },
  { "enum",             RID_ENUM,       0 },
  { "extern",           RID_EXTERN,     0 },
  { "float",            RID_FLOAT,      0 },
  { "for",              RID_FOR,        0 },
  { "goto",             RID_GOTO,       0 },
  { "if",               RID_IF,         0 },
  { "inline",           RID_INLINE,     D_TRAD|D_EXT89 },
  { "int",              RID_INT,        0 },
  { "long",             RID_LONG,       0 },
  { "register",         RID_REGISTER,   0 },
  { "restrict",         RID_RESTRICT,   D_TRAD|D_C89 },
  { "return",           RID_RETURN,     0 },
  { "short",            RID_SHORT,      0 },
  { "signed",           RID_SIGNED,     D_TRAD },
  { "sizeof",           RID_SIZEOF,     0 },
  { "static",           RID_STATIC,     0 },
  { "struct",           RID_STRUCT,     0 },
  { "switch",           RID_SWITCH,     0 },
  { "typedef",          RID_TYPEDEF,    0 },
  { "typeof",           RID_TYPEOF,     D_TRAD|D_EXT },
  { "union",            RID_UNION,      0 },
  { "unsigned",         RID_UNSIGNED,   0 },
  { "void",             RID_VOID,       0 },
  { "volatile",         RID_VOLATILE,   D_TRAD },
  { "while",            RID_WHILE,      0 },
};
#define N_reswords (sizeof reswords / sizeof (struct resword))

static void
init_reswords ()
{
  unsigned int i;
  tree id;
  int mask = (flag_isoc99 ? 0 : D_C89)
              | (flag_traditional ? D_TRAD : 0)
              | (flag_no_asm ? (flag_isoc99 ? D_EXT : D_EXT|D_EXT89) : 0);

  if (c_language != clk_objective_c)
     mask |= D_OBJC;

  /* It is not necessary to register ridpointers as a GC root, because
     all the trees it points to are permanently interned in the
     get_identifier hash anyway.  */
  ridpointers = (tree *) xcalloc ((int) RID_MAX, sizeof (tree));
  for (i = 0; i < N_reswords; i++)
    {
      /* If a keyword is disabled, do not enter it into the table
         and so create a canonical spelling that isn't a keyword.  */
      if (reswords[i].disable & mask)
        continue;

      id = get_identifier (reswords[i].word);
      C_RID_CODE (id) = reswords[i].rid;
      C_IS_RESERVED_WORD (id) = 1;
      ridpointers [(int) reswords[i].rid] = id;
    }
}

tree
mydeclares (tree t) {
  tree tmp;
  if (!t) return 0;
  for(tmp=t;tmp;tmp=TREE_CHAIN(tmp)) {
    tree c = tree_cons (0, integer_type_node, 0);   
    tree d = tree_cons (c, TREE_VALUE(tmp), 0);
    tree dd = tree_cons (d, 0, 0);
    push_parm_decl(dd);
    get_parm_info(1);
  }
  return 0;
}
