%{
  /*
	*
	* Taken over by
	* Roar Thron�s <roart@nvg.ntnu.no>
	* 7-MAY-2000
	*
*/

  // TODO: IF_STMT etc in bli-common

int yy2debug=0;
//extern int yydebug=0;

#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYLEX yylex()
 extern int yylex();
 extern int yyerror (char *s);
#define LVAL_ADDR(x) (fold(build_indirect_ref (input_location, convert (integer_ptr_type_node, (x)), RO_UNARY_STAR)))
#define RVAL_ADDR(x) (build_unary_op(input_location, ADDR_EXPR, (x), 0))
#define UN_RVAL_ADDR(x) (fold(build_indirect_ref (input_location, convert (integer_ptr_type_node, (x)), RO_UNARY_STAR)))

int turn_off_addr_expr = 0;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 extern int quiet_flag;
 extern int undefmode;
 extern int tnamemode;
 extern int macromode;

#include "config.h"

#include "system.h"
#include "coretypes.h"

#include "tree.h"
#include "tree-inline.h"

#if 0
#include "bliss-tree.h"
#endif

#include "c/c-tree.h"

 //#undef IN_GCC
#if 1
#include "ggc.h"
#endif
 //#define IN_GCC

#include "bliss.h"

 #  define PARAMS(args) args
static const char *input_filename;
extern int   yyparse (void);
  extern void inform (location_t, const char *, ...);
  extern tree get_identifier (const char *);
  extern tree maybe_get_identifier (const char *);
  extern tree fold (tree);
  extern void reset_symbol(tree t);
  extern void c_pushtag (location_t loc, tree name, tree type);
  extern int is_opened(char * file);
  extern void check_lib(char * s);
  extern void push_req_stack(char *neww);
  extern int s_str_attr(tree t);
  extern void layout_decl(tree, unsigned int);
  extern int is_str_attr(tree t);
  extern int bli_finish_function();
  extern tree c_start_case_2 (tree, tree);
  extern tree build_inrange ();
  extern tree build_outrange ();
  extern void   bli_init_reswords();
  extern int set_cti(tree, tree);
  extern int check_little_endian();
  extern void push_parm_decl_init(tree, tree);
  extern int register_field(char * s, tree t);

  // bli-decl.c
  extern void bli_pushtag (location_t loc, tree name, tree type);
  extern void bli_pop_scope();
  extern struct c_arg_info * bli_get_parm_info (bool ellipsis, tree expr);

  static short *malloced_yyss;
 static void *malloced_yyvs;

 static int compstmt_count;

 static tree last_expr = 0;

 static int lastotherwise = 0;
 static int icc=0 ;
 static int gsc=0 ;

 int opt_minimum_actuals = 0;

 tree default_global_section = 0;
 tree default_global_align = 0;
 
 tree default_own_section = 0;
 tree default_own_align = 0;

 int yyrec = 0;

 //extern tree  build_modify_expr (tree, enum tree_code, tree);

#define symbtabinfo
  typedef int nodeAttr_t;

  tree  creatnode (nodeAttr_t, tree, tree);
  tree  creatid (char *);
  tree  creatvalue (int);
  tree  ForExpand(char *, int, int, tree);
//  int      yyerror (char *);
  //  int input_location.line=1;

void 
bli_error2 PARAMS ((char * msg));

int 
bli_lex PARAMS((void));

int 
bli_parse PARAMS((void));

 void get_builtin(void);
 void add_builtin(void);
 void predef_literal(char * name, int value);
 extern int node_type_to_int(tree t);

 tree igroot;

struct structure {
  struct structure * next;
  tree elem;
};

struct structure * mystructs = 0;

 struct mymacro * macros = 0;
 
 struct mymacro amacro;

 enum ps { p_none, p_ascii, p_asciz, p_ascic, p_ascid, p_rad50_11, p_rad50_10, p_sixbit, p_p };

 struct dsc_dollar_descriptor commadsc = {
   dsc_dollar_w_length : 1,
   dsc_dollar_a_pointer : ","
 };

 struct dsc_dollar_descriptor semidsc = {
   dsc_dollar_w_length : 1,
   dsc_dollar_a_pointer : ";"
 };

 int default_punct = ',';

extern FILE *yyin;

// wasint yyparse()
void
  bli_common_parse_file()
{
  //z1zin=fopen(input_filename,"r");

#if YYDEBUG != 0
  //yydebug = set_yydebug;
#else
  warning ("YYDEBUG not defined");
#endif

  get_builtin();

  yyin=fopen(main_input_filename,"r");

  // need this way or it will not execute
  // if return yyparse(), it just disappeared
  int parse = yyparse();
  printf("Parsing complete.\n");
  return parse;
}

 extern tree current_function_decl; 
 extern struct function *cfun; 
 struct function *acfun = 0;
 tree afun = 0;
 int cfun_zero = 0;
#if 1
 tree c_cont_label = 0;
#endif

 struct cntrl_stack {
   tree c_break_label;
   tree block_value;
   tree myselect;
   int unsign;
 };

 struct cntrl_stack cntrls[16];
 int cntrli = 0;

 static int filestackno = 0;

 struct myfilestackstruct {
   unsigned long yyin;
   unsigned long myinput_filename;
 };

 struct myfilestackstruct myfilestack[128]; 

 static void pushfilestack() {
   myfilestack[filestackno].yyin=yyin;
   myfilestack[filestackno].myinput_filename=input_filename;
   filestackno++;
 }

 static void popfilestack() {
   filestackno--;
   yyin=myfilestack[filestackno].yyin;
   input_filename=myfilestack[filestackno].myinput_filename;
 }

 static int bitstackno = 0;
 static char bitstack[20]="oooooooooooooooooooo"; // f=fetch a=assign o=other

 static int setbitcontext(char c) {
   bitstack[bitstackno]=c;
 }

 static char getbitcontext() {
   return bitstack[bitstackno];
 }

 static void pushbitstack() {
   bitstackno++;
 }

 static void popbitstack() {
   bitstackno--;
 }

 enum{SIMP_MACRO,COND_MACRO,ITER_MACRO,KEYW_MACRO};

 int longest_macro=0;
 int str1_memcpy=0;
 int str1_memcpys=0;
 int fields=0;
 int fielditers=0;

%}

%start mystart

%union {
  int type_int;
  char     *type_str;
  tree  type_node_p;
  int type_node_code;
  location_t location;
  enum tree_code code;
  struct c_expr expr;
}

/* test */

%nonassoc K_IF
%nonassoc K_ELSE

/*%token_table yacc*/
%token <type_node_p> T_DIGITS
%token <type_node_p> T_NAME T_STRING T_IDIGITS LEXEME M_NAME T_LABELNAME
%token <type_node_p> P_SOFTERROR P_SOFTERROR2 P_SOFTERROR3 P_SOFTERROR4
%token <type_node_p> START_CTCE START_LEX START_EXPR END_EXPR END_BUILTIN
%token <type_node_p> T_FIELDNAME T_LINKAGENAME

/*%light uplus uminus*/
%right <type_node_code> '='
%left <type_node_code> K_EQV K_XOR
%left <type_node_code> K_OR
%left <type_node_code> K_AND
%right <type_node_code> K_NOT
%left <type_node_code> K_EQL  K_EQLA  K_EQLU  K_NEQ  K_NEQA  K_NEQU K_LSS  K_LSSA  K_LSSU  K_LEQ  K_LEQA  K_LEQU K_GTR  K_GTRA  K_GTRU  K_GEQ  K_GEQA  K_GEQU
%left  <type_node_code> '+' '-'
%left <type_node_code> K_MOD '*' '/'
%left <type_node_code> '^'
%right <type_node_code> UMINUS UPLUS
%right <type_node_code> UNARY
%right <type_node_code> '.'
/*%token apo ''' yacc*/
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
%token K_SIGNAL K_STOP K_SETUNWIND K_EXTERNAL_NAME K_QUAD
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
%token P_REQUIRE P_SBTTL P_SIXBIT P_SIZE P_STRING P_SWITCHES P_THE P_THEN
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
%token N_ABS N_BITVECTOR N_BLISS N_BLISS10 N_BLISS36C
%token N_FORTRAN N_FORTRAN_FUNC N_FORTRAN_SUB N_F10
%token N_MAX N_MAXA N_MAXU N_MIN N_MINA N_MINU 
%token N_SETUNWIND N_SIGN N_SIGNAL N_SIGNAL_STOP N_VECTOR
%token N_CODE N_GLOBAL N_HIGH N_LOW N_OWN N_PLIT
%token N_CH_A_RCHAR N_CH_A_WCHAR N_CH_ALLOCATION N_CH_COMPARE N_CH_COPY
%token N_CH_DIFF N_CH_EQL N_CH_FAIL N_CH_FILL N_CH_FIND_CH N_CH_FIND_NOT_CH
%token N_CH_FIND_SUB N_CH_GEQ N_CH_GTR N_CH_LEQ N_CH_LSS N_CH_MOVE N_CH_NEQ
%token N_CH_PLUS N_CH_PTR N_CH_RCHAR N_CH_RCHAR_A N_CH_SIZE N_CH_TRANSLATE
%token N_CH_WCHAR N_CH_WCHAR_A

/* built-in */
%token B_ACTUALCOUNT B_ACTUALPARAMETER B_AP B_ARGPTR B_FP B_NULLPARAMETER B_PC
%token B_R0 B_R1 B_R2 B_R3 B_R4 B_R5 B_R6 B_R7 B_R8 B_R9 B_R10 B_R11 B_SP

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
%type <type_node_p> linkage_definition_list linkage_definition
%type <type_int> U_BLISS16 U_BLISS32 U_BLISS36 
%type <type_node_p> infix_operator
/*%type <type_node_p> executable_expression control_expression*/
%type <type_node_p> input_parameter_location_list input_parameter_location
%type <type_node_p>  output_parameter_location_list output_parameter_location
%type <type_node_p> linkage_type maybe_forward_routine_attribute_list
%type <type_node_p> routine_attributes global_routine_attributes
%type <type_node_p> control_expression language_list list_option_list
%type <type_node_p> numeric_literal decimal_literal character_code_literal
%type <type_node_p> float_literal integer_literal language_name_list
%type <type_node_p> single_precision_float_literal 
%type <type_node_p> double_precision_float_literal extended_exponent_double_precision_float_literal
%type <type_node_p> extended_exponent_extended_precision_float_literal mantissa
%type <type_node_p> string_literal plit3 plit_item_list
%type <type_node_p> forward_routine_attribute_list plit_group allocation_unit
%type <type_node_p> plit2 
%type <type_int> string_type 
%type <type_node_p> ctce replicator plit_expression linkage_time_constant_expression
%type <type_node_p> plit  plit_item
%type <type_int> U_SOURCE  U_NOSOURCE  U_REQUIRE  U_NOREQUIRE
%type <type_int> U_EXPAND  U_NOEXPAND  U_TRACE  U_NOTRACE
%type <type_int> U_LIBRARY  U_NOLIBRARY  U_OBJECT  U_NOOBJECT
%type <type_int> U_ASSEMBLY  U_NOASSEMBLY  U_SYMBOLIC  U_NOSYMBOLIC
%type <type_int> U_BINARY  U_NOBINARY  U_COMMENTARY  U_NOCOMMENTARY
%type <type_node_p>  plit_string attached_label_list list_option language_name
%type <type_node_p> block labeled_block attached_label
%type <type_node_p> block_body block_action block_value structure_reference
%type <type_node_p> ordinary_structure_reference default_structure_reference
%type <type_node_p> general_structure_reference segment_name access_actual
%type <type_node_p> access_part field_set_name field_name attribute
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
%type <type_node_code> op7 
/*
%type <type_node_code> K_AND K_OR K_NOT K_EQV K_XOR
%type <type_node_code> K_EQL K_EQLA K_EQLU K_NEQ K_NEQA K_NEQU K_LSS K_LSSA K_LSSU
%type <type_node_code> K_LEQ K_LEQA K_LEQU K_GTR K_GTRA K_GTRU K_GEQ K_GEQA K_GEQU
*/
%type <type_node_p> linkage_attribute tname_list tname_list2 char_par_list char_par
%type <type_node_p> range_attribute novalue_attribute /*pot_expression*/
/* %type <type_node_p> opexp1 opexp2 opexp3 opexp4 opexp5 opexp6 opexp7 opexp8*/
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
%type <type_node_p> register_declaration global_register_declaration
%type <type_node_p> external_register_declaration
%type <type_node_p> map_declaration
%type <type_node_p> own_item global_item
%type <type_node_p> external_item forward_item
%type <type_node_p> local_item local_name
%type <type_node_p> register_item
%type <type_node_p> global_reg_item 
%type <type_node_p> external_reg_item external_reg_item_list
%type <type_node_p> map_item map_item_list segment_expression
%type <type_node_p> allocation_name access_formal structure_body structure_size
%type <type_node_p> executable_function routine_call codecomment
%type <type_node_p> field_reference integer_digit_list
%type <type_node_p> integer_digit digits field_stuff_list
%type <type_node_p> opt_sign psect_name declaration_list
%type <type_node_p> block_action_list dsr1
%type <type_node_p> structure_definition structure_definition_list builtin_name
%type <type_node_p>  global_item_list external_item_list
%type <type_node_p> machine_specific_function linkage_function
%type <type_node_p> forward_item_list local_item_list register_item_list
%type <type_node_p> global_reg_item_list alloc_actual_list
%type <type_node_p> alloc_actual sign_ext_flag maybe_sign_ext_flag
%type <type_node_p> initial_item_list ctce_access_actual_list preset_item_list
%type <type_node_p> initial_string own_item_list 
%type <type_node_p> attribute_list 
%type <type_node_p> ordinary_routine_call general_routine_call
%type <type_node_p> routine_designator actual_parameter_list
%type <type_node_p> linkage_name
%type <type_node_p> actual_parameter routine_address
%type <type_node_p> field_selector quoted_string_list maybe_field_selector
%type <type_node_p> fetch_expression /*prefix_expression*/
%type <type_node_p> infix_expression assign_expression op_exp op_exp1
%type <type_node_p> op_exp2 op_exp3 op_exp4 op_exp5 op_exp6 op_exp7 op_exp8
%type <type_node_p> op_exp9 op_exp10 op_exp11 op_exp12
%type <type_node_p> b32_psect_attribute psect_attribute
%type <type_int> U_READ U_NOREAD U_SHARE U_NOSHARE U_NOPIC K_LOCAL 
%type <type_node_p> standard_function_name
%type <type_int> K_SETUNWIND  K_SIGNAL K_STOP U_OVERLAY  U_CONCATENATE U_VECTOR
%type <type_int>  K_REF U_CODE U_NODEFAULT U_WRITE U_NOWRITE U_EXECUTE U_NOEXECUTE
%type <type_node_p> conditional_expression  case_expression
%type <type_node_p> select_expression loop_expression case_label_list 
%type <type_node_p>  select_type
%type <type_int>  K_OTHERWISE  K_ALWAYS
%type <type_node_p> case_line case_line_list select_line select_line_list
%type <type_node_p> select_label select_label_list case_action select_action
%type <type_node_p> indexed_loop_expression tested_loop_expression
%type <type_node_p> leave_expression exitloop_expression case_label
%type <type_node_p> ltce
%type <type_node_p>  pre_tested_loop post_tested_loop
%type <type_int> K_TO K_INRANGE K_OUTRANGE indexed_loop_type
%type <type_int> K_INCR K_INCRA K_INCRU K_DECR K_DECRA K_DECRU
%type <type_int> K_SELECT K_SELECTA K_SELECTU K_WHILE K_DO K_UNTIL
%type <type_int> K_SELECTONE K_SELECTONEA K_SELECTONEU
%type <type_int> K_LEAVE K_WITH  K_EXITLOOP K_RETURN
%type <type_node_p> executable_function_name
%type <type_node_p> linkage_function_name character_handling_function_name
%type <type_node_p> machine_specific_function_name select_index
%type <type_node_p> position_exp size_exp 
%type <type_node_p> exit_expression return_expression exp
%type <type_int> K_PLIT K_UPLIT K_LONG K_WORD K_BYTE P_CTCE P_LTCE K_QUAD
%type <type_int> K_SIGNED  K_UNSIGNED K_OWN K_GLOBAL K_VOLATILE
%type <type_int> K_FORWARD K_STACKLOCAL  K_REGISTER
%type <type_int> K_ROUTINE
%type <type_node_p> allocation_default allocation_formal
%type <type_node_p> routine_definition_list io_list2 formal_item_list
%type <type_node_p> routine_attribute_list
%type <type_node_p> formal_item routine_definition io_list
%type <type_node_p> formal_attribute_list io_list3 global_io_list2
%type <type_node_p> map_declaration_attribute routine_attribute
%type <type_node_p> forward_routine_attribute
%type <type_node_p> global_routine_attribute  ext_routine_attribute
%type <type_node_p> global_routine_definition_list
%type <type_node_p> global_routine_definition global_routine_attribute_list
%type <type_node_p> forward_routine_item_list
%type <type_node_p> external_routine_item_list
%type <type_node_p> ext_routine_attribute_list forward_routine_item
%type <type_node_p> map_declaration_attribute_list
%type <type_node_p> external_routine_item
%type <type_node_p>  maybe_declaration_list maybe_block_action_list maybe_block_value
%type <type_node_p> access_formal_list  allocation_formal_list
%type <type_node_p> compile_time_constant_expression
%type <type_int>  K_STRUCTURE K_FIELD K_ALIGN K_INITIAL
%type <type_int> K_PSECT K_NOVALUE K_LINKAGE 
%type <type_int> K_ADDRESSING_MODE K_WEAK K_PRESET
%type <type_int> P_B P_O P_DECIMAL P_X
%type <type_node_p> bind_data_attribute positional_macro_declaration
%type <type_node_p> keyword_macro_declaration
%type <type_node_p> bind_data_attribute_list bind_data_declaration
%type <type_node_p> bind_data_item bind_data_item_list bind_routine_attribute
%type <type_node_p> bind_routine_attribute_list bind_routine_declaration
%type <type_node_p> bind_routine_item bind_routine_item_list
%type <type_node_p> external_literal_declaration
%type <type_node_p> external_literal_item external_literal_item_list
%type <type_node_p> literal_attribute literal_attribute_list
%type <type_node_p>  literal_declaration literal_item literal_item_list
%type <type_node_p> positional_macro_definition mode_16 mode_32
%type <type_node_p> keyword_macro_definition simple_macro_definition
%type <type_node_p> conditional_macro_definition iterative_macro_definition
%type <type_node_p> default_actual
%type <type_node_p> keyword_macro_definition_list
%type <type_node_p> keyword_pair keyword_pair_list lexeme_list macro_body
%type <type_node_p> positional_macro_definition_list
%type <type_int> U_ABSOLUTE U_RELATIVE U_GENERAL U_LONG_RELATIVE U_WORD_RELATIVE
%type <type_node_p> macro_call positional_macro_call keyword_macro_call macro_name
%type <type_node_p> macro_actuals macro_actual_parameter_list macro_actual_parameter
%type <type_node_p> keyword_assignments keyword_assignment keyword_formal_name
%type <type_node_p> maybe_local_attribute_list local_attribute local_attribute_list
%type <type_node_p> by_exp from_exp to_exp k_while_or_until
%type <type_node_p> if_then compile_time_item compile_time_name compile_time_value
%type <type_node_p> field_definition field_component_list field_component
%type <type_node_p> maybe_bind_data_attribute_list
%type <type_node_p> own_name bind_data_name formal_name routine_name
%type <type_node_p> global_name register_name literal_name label_name
%type <type_node_p> data_name_value maybe_bind_routine_attribute_list
%type <type_node_p> bind_routine_item_rest label_name_list
%type <type_node_p> maybe_global_attribute_list global_attribute_list
%type <type_node_p> maybe_external_attribute_list external_attribute_list
%type <type_node_p> external_attribute external_name
%type <type_node_p> switch_item_list switch_item on_off_switch_item 
%type <type_node_p> special_switch_item
%type <type_node_p> expr_list maybe_ref maybe_alloc_actual_list structure_name
%type <type_node_p> built_in_name_list built_in_name select_action_with_end
%type <type_node_p> map_name macro_formal_name macro_formal_name_list
%type <type_node_p> maybe_ext_routine_attribute_list forward_name
%type <type_node_p> external_name_attribute
/*%type <type_node_p> test tok*/
%type <location> save_location

%{
#define SAVE_WARN_FLAGS()                       \
        size_int (pedantic                      \
                  | (warn_pointer_arith << 1)   \
                  | (warn_traditional << 2))

#define RESTORE_WARN_FLAGS(tval)                \
  do {                                          \
    unsigned int val = tree_to_uhwi (tval);           \
    pedantic = val & 1;                         \
    warn_pointer_arith = (val >> 1) & 1;        \
    warn_traditional = (val >> 2) & 1;          \
  } while (0)

  %}

%%

/**** 1.0 MODULES ***************************************************/

save_location:
{ if (yychar == YYEMPTY)
  yychar = YYLEX;
 $$ = input_location; }
;

mystart: module
{ 
};
/*module_body*/
module		: K_MODULE module_head '=' 
{
#if 0
  last_expr_filename=input_filename;
#endif
  push_file_scope ();
  // TODO add_builtin();
}
save_location
 start_block K_ELUDOM
{
  $$=$6;
  if (1) inform (input_location, "\n%%BLS-I-PARSED-OK-That's a module alright\n");
#if 0
  while (! global_bindings_p ())
    bli_pop_scope();
#endif
  finish_fname_decls ();
  // TODO finish_file ();
  pop_file_scope ();
}
/*
|
expression END_EXPR { last_expr = $1; YYACCEPT; }
*/
|
maybe_declaration_list END_EXPR { last_expr = tree_cons(0, $1, 0); YYACCEPT; }
|
T_NAME END_EXPR 
{
  last_expr = tree_cons (0, $1, 0);
#if 1
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && TREE_CODE(TREE_TYPE($1))==INTEGER_CST) {
    last_expr=tree_cons(0, TREE_TYPE($1), 0);
  }
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && TREE_CODE(TREE_TYPE($1))==IDENTIFIER_NODE) {
    last_expr=tree_cons(0, get_identifier(IDENTIFIER_POINTER(TREE_TYPE($1))), 0);
  }
#endif
  YYACCEPT; 
}
|
expr_list END_EXPR 
{
  // not quite finished? to catch parse(%remaining)
#if 0
  tree d1=$1;
  d1=TREE_VALUE(d1);
  last_expr = d1;
  if (TREE_CODE(d1)==IDENTIFIER_NODE && TREE_TYPE(d1) && TREE_CODE(TREE_TYPE(d1))==INTEGER_CST) {
    last_expr=TREE_TYPE(d1);
  }
  if (TREE_CODE(d1)==IDENTIFIER_NODE && TREE_TYPE(d1) && TREE_CODE(TREE_TYPE(d1))==IDENTIFIER_NODE) {
    last_expr=get_identifier(TREE_TYPE(d1));
  }
#endif
  last_expr = $1;
  YYACCEPT; 
}
|
T_FIELDNAME END_EXPR 
{
  last_expr = tree_cons(0, $1, 0);
  YYACCEPT; 
}
|
allocation_unit END_EXPR 
{
  if ($1==char_type_node)
    last_expr = build_string(strlen("byte"),"byte");
  if ($1==short_integer_type_node)
    last_expr = build_string(strlen("word"),"word");
  if ($1==integer_type_node)
    last_expr = build_string(strlen("long"),"long");
  if ($1==long_integer_type_node) // 64-bit
    last_expr = build_string(strlen("quad"),"quad");
  last_expr = tree_cons (0, last_expr, 0);
  YYACCEPT; 
}
|
',' END_EXPR 
{
  last_expr = tree_cons(0, 0, 0);
  YYACCEPT; 
}
;

expr_list:
expr_list ',' expression
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
expr_list ','
|
expression
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

module_head	: T_NAME
opt_mhargs
{ /* set the default switches */
  /*ig_name=(char *)$1;*/
  /*   $$->id=$1; */
  char * c= (void *) $1;
  $$=build_string(strlen(c),c);
}
;

opt_mhargs	: /* empty 1 */ { $$=0; }
| '(' ms_list ')' { $$ = $2; }
/*| '(' ms_list ')' { fprintf(stderr,"err %x \n",$2); fflush(stderr); }*/
  		;

ms_list		: { undefmode=1; } module_switch { undefmode=0; $$ = $2; }
| ms_list ',' { undefmode=1; } module_switch { undefmode=0;  }
;

module_body: /* empty 2 */ { $$=0; }
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

on_off_switch:
onoffmodes { $$ = 0; }
;

onoffmodes:
CODE		/* default */
| NOCODE
| DEBUG
| NODEBUG		/* default */
| ERRS			/* default */
| NOERRS
| U_OPTIMIZE		/* default */
| U_NOOPTIMIZE
| U_UNAMES
| U_NOUNAMES		/* default */
| SAFE			/* default */
| NOSAFE
| U_ZIP
| U_NOZIP			/* default */
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
|
U_MAIN '=' T_NAME  
{
  add_macro(IDENTIFIER_POINTER($3),SIMP_MACRO,0,0,build_string(4,"main"));
}
| U_OPTLEVEL '=' T_DIGITS  { $$ = 0; }
| U_VERSION '=' T_STRING  { $$ = 0; }

/*| P_TITLE T_STRING {  $$->id=$2; nonfin}*/
/*
  |T_NAME "equal" T_NAME
  {
   $$->id=$1;
  if (yydebug) inform ("swit %x %x %x\n",$$,$1,$3);
  switch (ukeyword( $1 )) {
  case U_MAIN:
  ig_main  = $3;
  if (yydebug) inform ("main= %d %s \n",switch_main,ig_main);
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
list_option_list: list_option_list ',' list_option 
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

tname_list:
tname_list ',' T_NAME
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
T_NAME
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

tname_list2: tname_list2  T_NAME 
|T_NAME 
;
/*
  name_list	: T_NAME
  {
  int i=ukeyword( $1 );
  
  fprintf(stdout, "$1 %s\n",$1);
  if (i>0)
  
  else {
  
  $$->id=(char *) $1;
  }
  }
  | name_list ',' T_NAME
  {
  int i=ukeyword( $3 );
  
  if (yydebug) inform ("zzz $1 %s\n",$1);
  if (i>0)
  
  else {
  
  $$->id=(char *) $1;
  }
  }
  |*/ /* empty 3 */
/*			{
			$$ = NULL;
			}
			;
		*/

lowlevel: T_DIGITS 
| T_NAME 
;

routine_designator:
primary  
/*| executable_function */
/* placing here temp workaround because of reduce reduce*/
;

/**** 2.0 EXPRESSIONS ***********************************************/
expression: 
primary
| operator_expression
| executable_function
| control_expression
;

primary: 
numeric_literal
| string_literal
| plit 
| T_NAME { 
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && TREE_CODE(TREE_TYPE($1))==INTEGER_CST) {
#if 0
    printf("\n\n\n\nCOMP %x %x\n\n\n\n",$1,TREE_TYPE($1),TREE_CODE(TREE_TYPE($1)));
    sleep(5);
#endif
    $$=TREE_TYPE($1);
    goto myout;
  }
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && (TREE_CODE(TREE_TYPE($1))==IDENTIFIER_NODE || (TREE_CONSTANT(TREE_TYPE($1)) && TREE_CODE(TREE_TYPE($1))!=INTEGER_TYPE))) {
    if (TREE_CODE(TREE_TYPE($1))==STRING_CST) {
      $$=convert_string_literal_to_integer(TREE_TYPE($1));
      goto myout;
    }
    if (TREE_CODE(TREE_TYPE($1))==IDENTIFIER_NODE)
      $$=get_identifier(IDENTIFIER_POINTER(TREE_TYPE($1)));
    else
      $$=TREE_TYPE($1);
    goto myout;
  }
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  $$ = build_external_ref (input_location, $1, yychar == '(', 0);
  if (TREE_LANG_FLAG_0($1) || TREE_LANG_FLAG_0($$)) {
         //fprintf(stderr, "CON %s\n",IDENTIFIER_POINTER($1));
         $$ = strip_literal($$);
	 goto myout;
  }
  if (yychar == '(')
    goto myout;
  if (!turn_off_addr_expr)
	 $$ = RVAL_ADDR($$);
 myout:
  {}
}
| block
| structure_reference 
| routine_call  { 
// $$ = c_expand_expr_stmt ($1);
 }
| field_reference 
| codecomment 
/* | macro_call error { yyerrok; } */
;

numeric_literal: 
decimal_literal  { /* TREE_TYPE($1)=integer_type_node;*/ }
| integer_literal 
| character_code_literal 
| float_literal  
;

decimal_literal: 
T_DIGITS
;

opt_sign: { $$=0; }
|'+' { $$ = 0; }
|'-' { $$ = 0; }
;

integer_literal: 
P_B T_STRING  { 
  tree t;
  t = build_int_cst (long_integer_type_node, 1+strtoul(TREE_STRING_POINTER($2),0,2)); // 64-bit
  $$ = t;
}
| P_O T_STRING  {
  tree t;
  t = build_int_cst (long_integer_type_node, 1+strtoul(TREE_STRING_POINTER($2),0,8)); // 64-bit
  $$ = t;
}
| P_DECIMAL T_STRING  {
  tree t;
  t = build_int_cst (long_integer_type_node, 1+strtoul(TREE_STRING_POINTER($2),0,10)); // 64-bit
  $$ = t;
}
| P_X T_STRING  {
  tree t;
  t = build_int_cst (long_integer_type_node, strtoul(1+TREE_STRING_POINTER($2),0,16)); // 64-bit
  $$ = t;
}
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

character_code_literal: P_C T_STRING { 
  tree t;
  t = build_int_cst (long_integer_type_node, *(char*)(1+TREE_STRING_POINTER($2))); // 64-bit
  $$ = t;
}
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

// not quite right implemented with regard to all types
string_literal:  string_type T_STRING2 {
  enum ps type = $1;
  char * str=TREE_STRING_POINTER($2);
  int len=strlen(str)-2;
  str++;
  switch (type) {
    // check 64-bit
  case p_ascii:
	 if (len>4) error("string longer than 4");
    // check 64-bit
  case p_asciz:
	 {
	 if (len>3) error("string longer than 3");
	 tree t;
	 int myint = 0;
	 char *c = (void *) &myint;
	 int i;
	 for (i=0;i<len;i++)
		c[i]=str[i];
	 t = build_int_cst (long_integer_type_node, myint); // 64-bit
	 TREE_LANG_FLAG_0(t)=1;
	 $$ = t;
	 }
	 break;
    // 64-bit
  case p_ascic:
	 {
	 if (len>3) error("string longer than 3");
	 //	 $$ = add_counted_string($2, len);
	 tree t;
	 int myint = 0;
	 char * c = (void *) &myint;
	 int i;
	 c[0]=len;
	 for (i=0;i<len;i++)
		c[i+1]=str[i];
	 t = build_int_cst (long_integer_type_node, myint); // 64-bit
	 TREE_LANG_FLAG_1(t)=1;
	 $$ = t;
	 }
	 break;
    // 64-bit
  case p_ascid:
	 // long_long_integer_type_node
	 {
	 struct dsc_dollar_descriptor dsc = {
	 dsc_dollar_w_length : len,
		dsc_dollar_b_dtype : DSC_DOLLAR_K_DTYPE_T,
		dsc_dollar_b_class : DSC_DOLLAR_K_CLASS_S
	 };
	 //	 volatile long * vec;
	 long firstlong;
	 tree cell_decl;
	 tree cell;
	 tree init;
	 tree decl_p;
	 tree type = char_array_type_node /*integer_type_node*/; // TODO
	 tree t = tree_cons (NULL_TREE, type, NULL_TREE); 
	 // check. superflous? found when 64-biting
	 tree high;
	 // check. superflous? found when 64-biting
	 tree string;
	 char mystr[1024];
	 int new_len=1+((len+3)&1021);
	 memset(mystr+new_len-4, 0, 5);
	 memcpy(mystr, str, len);
	 int cpu_le=check_little_endian();

	 tree addr;
	 if (len) {
	   string = build_string(new_len, mystr);
#if 0
	 else
	   string = build_string(1, "");
#endif
#if 0
	 tree decl = build_decl (CONST_DECL, 0, string_type_node);
	 DECL_INITIAL(decl)=string;
	 pushdecl(decl);
#else
	 TREE_TYPE(string)=string_type_node;
	 char s2[32];
	 sprintf(s2,"_dsc_a_pointer%d",icc);
#if 0
	 tree d2 = build_nt(ARRAY_REF,get_identifier(s2),build_int_cst (long_integer_type_node, new_len/*+1*/)); // 64-bit
	 tree decl = bli_start_decl(d2, char_type_node, 1, 0);
#else
	 struct c_declarator * id_declarator = build_id_declarator (get_identifier (s2));
	 struct c_declarator * array_declarator = build_array_declarator (input_location, build_int_cst (long_integer_type_node, new_len/*+1*/), 0, 0, 0); // 64-bit
	 struct c_declarator * d1 = set_array_declarator_inner (array_declarator, id_declarator);
	 tree decl = start_decl (array_declarator, build_specs (char_type_node), 1, 0, 1, 0);
	 TREE_STATIC(decl)=1;
#endif
#if 0
	 if (len<4) {
	   int tmp_len=len+1;
	   tree index_type = c_common_signed_type (sizetype);
	   tree itype = fold (convert (index_type, build_int_cst (long_integer_type_node, 5))); 
	   itype = build_index_type (itype);
	   TREE_TYPE(decl) = build_array_type (char_type_node, itype);
	 }
#endif
	 start_init(decl,NULL,global_bindings_p(), 0, 0);
	 finish_init();

	 tree stringlist = tree_cons (build_int_cst (integer_type_node, 0), string, 0);
	 tree constructor2 = build_constructor_from_list (char_array_type_node /*TREE_TYPE(decl)*//*integer_type_node int_array_type_node*/,stringlist); // TODO
	 TREE_CONSTANT(constructor2)=1;
	 init=constructor2;

	 finish_decl (decl, input_location, string, NULL_TREE, 0);
#endif

	 TREE_TYPE(string) = /*string*/char_array_type_node; // or char_array?
#if 0
	 tree addr = build_unary_op (input_location, ADDR_EXPR, string, 0);
#else
	 addr = build_unary_op (input_location, ADDR_EXPR, decl, 0);
#endif
	 } else {
	   addr=build_int_cst (long_integer_type_node, 0); // later: init this to be start of dsc
	 }
	 // check addr = convert (integer_type_node, addr);

#if 0
	 addr = build_int_cst (long_integer_type_node, 42); // 64-bit
	 TREE_TYPE (addr) = widest_integer_literal_type_node;
	 addr = convert (integer_type_node, addr);
#endif

	 //	 vec=&dsc;
	 firstlong=*(int *)&dsc; // check. 64-bit

	 // 64-bit
#ifdef __i386__
	 tree last = tree_cons(build_int_cst (long_integer_type_node, 1),addr,0); // 64-bit
	 tree nextlast = tree_cons(build_int_cst (long_integer_type_node, 0),build_int_cst (long_integer_type_node, firstlong), last); // 64-bit
#else
	 tree last = tree_cons(build_int_cst (long_integer_type_node, 3), build_int_cst (long_integer_type_node, 0) ,0);
	 tree nextlast = tree_cons(build_int_cst (long_integer_type_node, 2), addr,last); // check. not quite 64-bit
	 tree nextnextlast = tree_cons(build_int_cst (long_integer_type_node, 1), build_int_cst (long_integer_type_node, 0), nextlast);
	 tree nextnextnextlast = tree_cons(build_int_cst (long_integer_type_node, 0), build_int_cst (long_integer_type_node, firstlong), nextnextlast);
#endif

#if 0
	 tree decl2 = build_decl (CONST_DECL, 0, string_type_node);
	 DECL_INITIAL(decl2)=constructor;
	 pushdecl(decl2);
#endif
	 char s[16];
	 sprintf(s,"_dsc%d",icc);
	 icc++;
#if 0	 
	 type=long_integer_type_node; // 64-bit
	 tree size=build_int_cst (long_integer_type_node, 2); // 64-bit
	 tree index_type = c_common_signed_type (sizetype);
	 tree itype=build_int_cst (long_integer_type_node, 1); // 64-bit
	 itype = build_index_type (itype);
	 type = build_array_type (type, itype);
	 type = int_array_type_node;
#endif
#if 0
#ifdef __i386__
	 // check. 64-bit
	 tree d = build_nt(ARRAY_REF,get_identifier(s),build_int_cst (long_integer_type_node, 2)); // 64-bit
#else
	 tree d = build_nt(ARRAY_REF,get_identifier(s),build_int_cst (long_integer_type_node, 4)); // 64-bit
#endif
	 tree decl2 = bli_start_decl(d, type, 1, 0);
#else
#ifdef __i386__
	 // check. 64-bit
	 tree the_len = build_int_cst (long_integer_type_node, 2); // 64-bit
#else
	 tree the_len = build_int_cst (long_integer_type_node, 4); // 64-bit
#endif
	  struct c_declarator * id_declarator = build_id_declarator (get_identifier (s));
	  struct c_declarator * array_declarator = build_array_declarator (input_location, the_len, 0, 0, 0);
	 struct c_declarator * d1 = set_array_declarator_inner (array_declarator, id_declarator);
	 tree decl2 = start_decl(array_declarator, build_specs (long_integer_type_node/*type*/), 1, 0, 1, 0);
	 TREE_STATIC(decl2)=1;
#endif
#if 0
	 DECL_SIZE_UNIT(decl2)=build_int_cst (long_integer_type_node, 8);	
	 DECL_SIZE(decl2)=build_int_cst (long_integer_type_node, 64); // 64-bit
	 DECL_ALIGN(decl2)=1;
#endif
	 start_init(decl2,NULL,global_bindings_p(), 0, 0);
	 finish_init();

	 // 64-bit
#ifdef __i386__
	 tree constructor = build_constructor_from_list (/*int_array_type_node*/ TREE_TYPE(decl2)/*integer_type_node int_array_type_node*/,nextlast);
#else
	 tree constructor = build_constructor_from_list (/*int_array_type_node*/ TREE_TYPE(decl2)/*integer_type_node int_array_type_node*/,nextnextnextlast);
#endif
	 TREE_CONSTANT(constructor)=1;
	 init=constructor;

	 finish_decl (decl2, input_location, init, NULL_TREE, 0);

	 //	 fprintf(stderr, "vec %x %x %x %x %x\n",firstlong,sizeof( long),vec[0],vec[1],len);
	
	 $$ = build_unary_op (input_location, ADDR_EXPR, decl2, 0);
	 TREE_CONSTANT($$)=1;
	 TREE_LANG_FLAG_2($$)=1;

	 }
	 break;
  case p_rad50_11:
  case p_rad50_10:
  case p_sixbit:
  case p_p:
	 break;
  default:
	 break;
  }
}
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

string_type:  P_ASCII  { $$ = p_ascii; }
| P_ASCIZ   { $$ = p_asciz; }
| P_ASCIC   { $$ = p_ascic; }
| P_ASCID   { $$ = p_ascid; }
| P_RAD50_11  { $$ = p_rad50_11; }
| P_RAD50_10   { $$ = p_rad50_10; }
| P_SIXBIT   { $$ = p_sixbit; }
| P_P   { $$ = p_p; }
;

plit2:
K_PLIT  { $$ = 1; }
|K_UPLIT  { $$ = 0; }
;

plit3: /* empty 4 */ { $$=0; }
| allocation_unit 
| psect_allocation
| psect_allocation allocation_unit
;

plit: plit2 plit3 '(' plit_item_list ')' 
{
  int counted = $1; // == K_PLIT;
  long page[1024];
  int offset = 0;
  int size = sizeof(long); // 64-bit
  char * start = (void *) page;
  char * cur = start;
  memset(page, 0, 4096);
  if ($2)
    size=node_type_to_int($2);
  if (counted)
    cur += sizeof(long); // 64-bit
  tree t = handle_initial(0, build_nt(INIT_ATTR, $4), 0, 0, size);

  static int myplit=0;
  char pl[256];
  sprintf(pl,"plit%x",myplit);
  tree cell=get_identifier(pl);
  myplit++;

  tree init = t; //handle_initial(0, $4, 0, 0, size);
  tree mysize = TREE_TYPE(init);

  tree c_attr = 0;

  tree cell_decl_p = bli_start_decl (cell, mysize, 1, c_attr);
  TREE_STATIC(cell_decl_p)=1;
  TREE_PUBLIC(cell_decl_p)=0;

  start_init(cell_decl_p,NULL,1, 0, 0);
  finish_init();

  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);

  $$ = build_unary_op (input_location, ADDR_EXPR, cell_decl_p, 0);
}
;

plit_item_list: plit_item_list ',' plit_item { $$ = chainon($1, $3); }
|plit_item 
;

psect_allocation: K_PSECT '(' T_NAME ')' { $$ = 0; }
;

psect_name: T_NAME 
;

plit_item: 
plit_group  
{
  $$ = tree_cons($1, 0, 0);
}
|
plit_expression 
{
  $$ = tree_cons($1, 0, 0);
}
|
plit_string 
{
  $$ = tree_cons($1, 0, 0);
}
;

plit_group: 
allocation_unit '(' plit_item_list ')' /* not easy to interpret p_group */
{
  $$ = build_nt(PLIT_GROUP, build_int_cst (long_integer_type_node, 1), $1, $3); // 64-bit
}
|
K_REP replicator K_OF '(' plit_item_list ')' 
{
  $$ = build_nt(PLIT_GROUP, $2, long_integer_type_node, $5); // 64-bit
}
|
K_REP replicator K_OF allocation_unit '(' plit_item_list ')' 
{
  $$ = build_nt(PLIT_GROUP, $2, $4, $6);
}
;

allocation_unit:  K_LONG   { $$ = integer_type_node; }
| K_WORD   { $$ = short_integer_type_node; }
| K_BYTE  { $$ = char_type_node; }
| K_QUAD
{
  $$ = long_long_integer_type_node; // 64-bit
#ifdef __i386__
  warning(0, "%%BLS32-W-TEXT, Quad is not a standard 32-bit thingie.");
#endif
}
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

labeled_block:
attached_label_list
{
  tree cur_last=tree_last(cur_stmt_list);
  tree decl;
  tree name=$1;
  decl = lookup_label(name);
#if 0
  //  decl = build_decl (LABEL_DECL, name, TREE_TYPE (integer_type_node));
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
#endif

#if 0
  tree label_decl = decl;
  tree node = build (LABELED_BLOCK_EXPR, NULL_TREE, label_decl, NULL_TREE);
  TREE_SIDE_EFFECTS (node) = 1;
  TREE_TYPE(node)=void_type_node;
  LABELED_BLOCK_LABEL(node)=label_decl; // superfluous?
#endif
#if 0
  TREE_OPERAND(node, 1)= cur_last; // temp storage
  //$$=node;
#endif
#if 0
  //$$=c_expand_expr_stmt($$);
#endif
}
unlabeled_block
{
  tree name=$1;
  tree decl = lookup_label(name);
  add_stmt (build1 (LABEL_EXPR, void_type_node, decl));
#if 0
  tree body = tree_last(cur_stmt_list);
  tree es=$<type_node_p>2;
  tree node=TREE_OPERAND(es, 0);
  tree scope=TREE_OPERAND(node, 1);
  TREE_TYPE(body)=integer_type_node;
  // body = c_expand_expr_stmt(body);
  // body = EXPR_STMT_EXPR(body);
  TREE_TYPE(body)=integer_type_node;
  //RECHAIN_STMTS(node, LABELED_BLOCK_BODY(node));
  LABELED_BLOCK_BODY(node)=body;//$2;
  TREE_CHAIN(TREE_CHAIN(scope))=TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(scope)));
  //TREE_CHAIN(scope)=TREE_CHAIN(TREE_CHAIN(scope));
  //tree_last(cur_stmt_list) = node;
  TREE_SIDE_EFFECTS (body) = 1;
  //c_expand_expr_stmt(node);
#endif

#if 0
  tree n = build1 (STMT_EXPR, last_expr_type, node);
  TREE_TYPE(n)=void_type_node;
  TREE_SIDE_EFFECTS (n) = 1;
  c_expand_expr_stmt(n);
#endif

#if 0
  //tree_last(cur_stmt_list) = node;
  $$ = node;
  $$ = 0;
  $$ = body;
  //c_expand_expr_stmt($$);
  //$$ = 0;
  //  add_stmt($$);
#endif
  $$ = $3;
}
;

label_name:
T_LABELNAME
{ /* was: T_NAME, but it caused some grammar problems, so therefore
     this workaround */
}
;

attached_label: 
label_name ':'
;

attached_label_list: attached_label_list attached_label 
| attached_label 
;

unlabeled_block_start: K_BEGIN { 
#ifndef c99
  compstmt_count++;
  $$=c_begin_compound_stmt (1);
#endif
}
;

unlabeled_block_start2: '(' { 
#ifndef c99
  if (cfun==0) {
#if 0
	 cfun_zero=1;
	 allocate_struct_function();
#endif
  }
  if (cfun) {
    compstmt_count++;
    $$=c_begin_compound_stmt (1);
  }
#endif
}
;

unlabeled_block: unlabeled_block_start
pushlevel block_body K_END poplevel
{ 
#ifndef c99
#if 0
  $$=bli_pop_scope();
#endif
#endif
 // last_expr_type = NULL_TREE; // check
  add_stmt (c_end_compound_stmt (input_location, $1, true));
  $$ = $3;
}
| unlabeled_block_start2 
pushlevel block_body ')' poplevel
{ 
#ifndef c99
  if (cfun==0)
	 goto no_cfun;
#if 0
$$=bli_pop_scope();
#endif
 // last_expr_type = NULL_TREE; // check
 $$=$1;
 no_cfun:
 $$=$3;
#else 
 $$=0;
#endif
 if (cfun)
   $$ = add_stmt (c_end_compound_stmt (input_location, $1, true));
 $$ = $3;
}
;

start_block:
K_BEGIN block_body K_END 
{
  $$=$2;
} 
|
'(' block_body ')'
{
  $$=$2;
} 
;


block_body: 
maybe_declaration_list 
{
  default_punct = ',';
}
/* { $$ = bli_begin_compound_stmt (); } */
maybe_block_action_list 
maybe_block_value 
{
  $$=$4;
}
;
/*
maybe_declaration_list: { $$=0; }
| declaration_list 
;
*/
maybe_declaration_list:  { $$=NULL_TREE; }
|
maybe_declaration_list
{
  default_punct = ',';
}
 declaration {
  /*  */
  // check reason for the if-else part
  default_punct = ';';
#if 0
  if ($3 && !chain_member($3, $1))
    $$ = chainon($1, $3);
  else
    $$ = $1;
#else
  $$ = 0;
#endif
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

maybe_block_action_list:   { $$=0; } |  
maybe_block_action_list block_action { 
#if 0
  $$=chainon($1, $2); 
#endif
}
;

block_action_list: block_action_list block_action { $$ = chainon ($1, $2); }
|block_action 
{
  // check if need more elaborate list chainon
}
;
/*The block_body must not be null.*/

block_action: expression ';' { 
  /*bli_add_stmt ($1);*/
  if ($1)
    add_stmt ($1);
#if 0
  if ($1) 
    if (TREE_CODE($1)<SIZEOF_EXPR)
      $$=c_expand_expr_stmt($1);
#endif
}
;

maybe_block_value: { $$=build_int_cst (/*long_*/integer_type_node, 0); /* check */ }
|block_value
{
#if 0
  if ($1) 
    if (TREE_CODE($1)<SIZEOF_EXPR)
		c_expand_expr_stmt($1);
  $$=$1; 
#endif

  $$ = $1;
}
;

block_value: expression 
;

structure_reference:
  /*  ordinary_structure_reference 
  | default_structure_reference nit ye
  | general_structure_reference */
T_NAME '[' access_actual_list maybe_alloc_actual_list ']' 
{ 
  tree tag = mytag($1);
  if (tag && is_str_attr(tag)) {
    char s2[32];
    sprintf(s2,"_gsc%d",gsc++);
    tree d2 = get_identifier(s2);
    tree cell__ = get_identifier(add_underscore(d2, 2));

    tree st = handle_structure_attribute($1, $4, 0);
    tree segname = TREE_VALUE($3);
    tree extref=segname; // could be RVAL_ADDR(build_external_ref (segname, 0, input_location));
    tree params = TREE_CHAIN($3);
    //  tree type = $1;
    tree size = handle_structure(d2, st, 0);
    // rest from ordinary str ref, from body =
    tree type = xref_tag(STRUCTURE_ATTR,cell__);
    tree body = my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(type))))));
    tree access = my_copy_tree(TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(type))));
    chainon (tree_last(params), build_tree_list(NULL_TREE, extref)); 
    my_substitute(body,access,params);
    $$=body;
  } else {
    tree cell__ = get_identifier(add_underscore($1, 2));
    //tree t=build_external_ref (input_location.$1, 0, 0);
    tree extref = build_external_ref (input_location, $1, 0, 0);
    if (TREE_LANG_FLAG_3(extref)==0 && TREE_LANG_FLAG_0(extref)==0)
      extref=RVAL_ADDR(extref);
#if 0
    tree extref=RVAL_ADDR(build_external_ref ($1, 0, input_location));
#endif
    //tree params = chainon(copy_node(extref), $3);
    //if (TREE_CHAIN(er)) fprintf(stdout, "\npanic %x\n",input_location.line);
    tree params = $3;
    tree type = xref_tag(STRUCTURE_ATTR,cell__);
    tree body = my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(type))))));
    tree access = my_copy_tree(TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(type))));
    chainon (tree_last($3), build_tree_list(NULL_TREE, extref)); 
    my_substitute(body,access,params);
    $$=body;
  }
}
;

ordinary_structure_reference:
segment_name '[' access_actual_list ']' {
  tree cell__ = get_identifier(add_underscore($1, 2));
  //tree t=build_external_ref ($1, 0, input_location);
  if (TREE_LANG_FLAG_3(extref)==0 && TREE_LANG_FLAG_0(extref)==0)
    extref=RVAL_ADDR(extref);
#if 0
  tree extref=RVAL_ADDR(build_external_ref ($1, 0, input_location));
#endif
  //tree params = chainon(copy_node(extref), $3);
  //if (TREE_CHAIN(er)) fprintf(stdout, "\npanic %x\n",input_location.line);
  tree params = $3;
  tree type = xref_tag(STRUCTURE_ATTR,cell__);
  tree body = my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(type))))));
  tree access = my_copy_tree(TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(type))));
  chainon (tree_last($3), build_tree_list(NULL_TREE, extref)); 
  my_substitute(body,access,params);
  $$=body;
  //tree decl=build_array_declarator (t->exp.operands[3], NULL_TREE, 0, 0) ;
  //decl->exp.operands[2]=t;
  //tree type=integer_type_node;
  //$$ = set_array_declarator_type (decl, type, 1);
  //$$ = $3;
}
;

access_actual_list:
access_actual_list ',' access_actual
{
  //$$ = chainon($1,$3);
  chainon ($1, $3/* was: build_tree_list (NULL_TREE, $3)*/);
}
|
access_actual 
{
  // was:  $$ = build_tree_list (NULL_TREE, $1);
}
;

segment_name:
T_NAME
;

access_actual:
{
  // was: $$=0;
  $$ = build_tree_list (NULL_TREE, 0);
}
|
expression  
{
  $$ = build_tree_list (NULL_TREE, convert (long_integer_type_node, $1)); // 64-bit
  // convert primarily to have (bind?) byte in a strucure ref
}
|
T_FIELDNAME 
{
  /* was: field_name, but it caused some grammar problems, so therefore
     this workaround */
  tree t = find_field(IDENTIFIER_POINTER($1));
  if (t==0) {
    $$ = 0; // something is wrong
  } else {
    $$ = my_copy_tree(t);
  }
}
;

access_part:  
segment_expression ',' access_actual_list 
{ 
  chainon($1, $3);
}
|
segment_expression  
;

segment_expression:
exp 
{ 
  $$ = build_tree_list (NULL_TREE, $1);
}
;

/*field_name      Note: See field_attribute, Section 4.1*/

field_set_name: T_NAME 
;

field_name: T_NAME 
;

dsr1:
primary  
| executable_function 
;

default_structure_reference: 
address '[' access_actual_list ']' 
;

structure_name:
T_NAME
{ 
  // was: T_NAME but got grammar problems
}
;

general_structure_reference:
structure_name '[' access_part ';' alloc_actual_list ']' 
{ 
  char s2[32];
  sprintf(s2,"_gsc%d",gsc++);
  tree d2 = get_identifier(s2);
  tree cell__ = get_identifier(add_underscore(d2, 2));

  tree st = handle_structure_attribute($1, $5, 0);
  tree segname = TREE_VALUE($3);
  tree extref=segname; // could be RVAL_ADDR(build_external_ref (segname, 0, input_location));
  tree params = TREE_CHAIN($3);
  //  tree type = $1;
  tree size = handle_structure(d2, st, 0);
  // rest from ordinary str ref, from body =
  tree type = xref_tag(STRUCTURE_ATTR,cell__);
  tree body = my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(type))))));
  tree access = my_copy_tree(TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(type))));
  chainon (tree_last(params), build_tree_list(NULL_TREE, extref)); 
  my_substitute(body,access,params);
  $$=body;
}
|
structure_name '[' access_part ']' 
{
  tree st = handle_structure_attribute($1, 0, 0);
}
;

alloc_actual_list: alloc_actual_list ',' alloc_actual
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
alloc_actual 
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

routine_call: ordinary_routine_call  
/* not yet| general_routine_call */
;

ordinary_routine_call:
routine_designator /*T_NAME*/ '(' io_list3 ')' { 
   // was:  shouldbe routine_designator. %prec '.' ?
  tree ref;
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  if (TREE_CODE($1)==IDENTIFIER_NODE)
    ref = build_external_ref (input_location, $1, 1, 0);
  else
    ref=$1;

  if (TREE_CODE(ref)!=FUNCTION_DECL) {
    tree decl = create_temp_var();

    tree t = set_temp_var(decl, ref);
    // why does this work, and not with a simple default_function_type?
    TREE_TYPE(decl) = build_pointer_type(default_function_type);
#if 0
    // check
    c_expand_expr_stmt(t);
#else
    add_stmt(t);
#endif
    ref = decl;
  }
  //    ref= build_decl( FUNCTION_DECL, build_unary_op (input_location, ADDR_EXPR, ref, 0), integer_type_node);
  int count=0;
  tree tmp;
  for (tmp=$3; tmp; tmp=TREE_CHAIN(tmp), count++) ;
  if (TREE_LANG_FLAG_5(ref)) {
    $3 = chainon(build_tree_list (NULL_TREE, build_int_cst (long_integer_type_node, count)), $3); // 64-bit
  } else {
    if (!quiet_flag) fprintf(stderr, "f %x %x\n", count,opt_minimum_actuals);
    for(;count<opt_minimum_actuals;count++)
      chainon ($3, build_tree_list(0, build_int_cst (long_integer_type_node, 0))); // 64-bit
  }
  $$ = build_function_call (input_location, ref, $3); 
}
/*nonfin should be routine_designator, but it did not work*/
;

io_list3: { $$=0; }
| io_actual_parameter_list  
| io_actual_parameter_list ';' io_actual_parameter_list  
| ';' io_actual_parameter_list  { $$ = 0; }
;

actual_parameter_list:
actual_parameter_list ',' actual_parameter 
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
actual_parameter 
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

io_actual_parameter_list:
io_actual_parameter_list ',' io_actual_parameter
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
io_actual_parameter
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

io_actual_parameter:
{
  $$=build_int_cst (long_integer_type_node, 0); // was 0 // 64-bit
}
|
expression
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
/*address*/ /*makes stuff loop*/
address maybe_field_selector 
/*address '<' position_exp ',' size_exp '>'*/
{
  tree t, op0=0, op1=0, op2=0; 
  tree pos = convert (bitsizetype, TREE_OPERAND($2, 0)); // check was u-
  tree size = TREE_OPERAND($2, 1);
  tree ext = TREE_OPERAND($2, 2);
  unsigned int unsign = 1;
  if (ext==0 || (ext && TREE_CONSTANT(ext))) {
    unsigned int sign=0;
    if (ext)
      sign = tree_to_uhwi (ext);
    unsign=!sign;
    if (unsign)
      unsign = 1;
  }
  char context=getbitcontext();
  /* if (context=='o') */ {
    // 32 hardcoded
    tree f = build_int_cst (long_integer_type_node, 5); // (32==2^5)
    t = /*parser_*/build_binary_op(input_location, RSHIFT_EXPR, pos, f, 1); // check -parser
    op0 = /*parser_*/build_binary_op (input_location, PLUS_EXPR, $1, t, 1); // check -parser
  }
  /*if (context=='f') */ {
    tree d=$1;
    t=build3 (BIT_FIELD_REF, c_common_type_for_mode(TYPE_MODE (long_integer_type_node),1), d, size, pos); // 64-bit
#if 0
    // check
    TREE_TYPE(TREE_OPERAND(t, 2)) = bitsizetype; // check. was u-
#endif
    TYPE_UNSIGNED(t)=unsign;
    t=stabilize_reference(t);
    TYPE_UNSIGNED(t)=unsign;
    op1 = t; 
  }
  /* if (context=='o') */ {
    tree d=$1;
    t=build3 (BIT_FIELD_REF, c_common_type_for_mode(TYPE_MODE (long_integer_type_node),1), d, size, pos); // 64-bit
#if 0
    // check
    TREE_TYPE(TREE_OPERAND(t, 2)) = bitsizetype; // check. was u-
#endif
    TYPE_UNSIGNED(t)=unsign;
    t=stabilize_reference(t);
    TYPE_UNSIGNED(t)=unsign;
   op2 = t;
  }
  if (context=='a') {
    if (yydebug) inform(input_location, "not implemented yet");
  }
  $$ = build_nt (BIT_FIELD_REFS, op0, op1, op2);
  if (op0) TREE_TYPE($$)=TREE_TYPE(op0);
}
;

address: 
primary  
| executable_function 
;

maybe_field_selector:
/* could not be empty, it caused loop */
/*|*/
field_selector
;

field_selector:
'<' position_exp ',' size_exp maybe_sign_ext_flag '>'
{
  $$ = build_nt(FIELD_SELECTOR, $2, $4, $5);
}
;

maybe_sign_ext_flag:
{ $$ = build_int_cst (long_integer_type_node, 0); } // 64-bit
|
',' sign_ext_flag 
{ $$ = $2; }
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

macro_call:
positional_macro_call
/*|keyword_macro_call*/
;

positional_macro_call:
macro_name
{
  struct mymacro * m = find_macro(0,IDENTIFIER_POINTER($1));
  push_macro(make_macro_string(m,0));
}
| macro_name '[' { macromode=2; } macro_actuals ']'
| macro_name '(' { macromode=2; } macro_actuals ')'
{
  struct mymacro * m = find_macro(0,IDENTIFIER_POINTER($1));
  push_macro(make_macro_string(m,$4));
}
| macro_name '<' { macromode=2; } macro_actuals '>'
;

macro_actuals:
{ $$ = 0; }
| macro_actual_parameter_list
;

macro_actual_parameter_list:
macro_actual_parameter_list ',' macro_actual_parameter { $$ = chainon($1,$3); }
|macro_actual_parameter
;

keyword_macro_call:
 macro_name '[' keyword_assignments ']'
| macro_name '(' keyword_assignments  ')'
| macro_name '<' keyword_assignments  '>'
;

keyword_assignments:
keyword_assignments ',' keyword_assignment 
| keyword_assignment ; 

keyword_assignment:
keyword_formal_name '=' macro_actual_parameter
;

macro_actual_parameter: { $$=0; }
| LEXEME;
  
macro_name: M_NAME;

keyword_formal_name:
T_NAME
;

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

myassign: opexp9 '=' opexp9 { $<type_node_p>$=0; };

opexp99:
{
  fprintf (stdout, "here I am 2\n");

} 
opexp9
{
  $<type_node_p>$=$2;
}
;

operator_expression:
{ pushbitstack();
 setbitcontext('f');
} 
'.' opexp9 %prec '.'  { 
  if (TREE_CODE($3)==BIT_FIELD_REFS) {
    $$ = TREE_OPERAND ($3, 1);

    tree tmp = convert(integer_ptr_type_node, TREE_OPERAND($$, 0));
#if 0
    if (TREE_CODE(tmp)==NOP_EXPR)
      tmp=TREE_OPERAND(tmp,0);
#endif
    TREE_OPERAND($$, 0) = build_indirect_ref (input_location,tmp, RO_UNARY_STAR); 

    if (TREE_OPERAND($$, 1)) TREE_OPERAND($$, 1)=fold(TREE_OPERAND($$, 1));
    if (TREE_OPERAND($$, 2)) TREE_OPERAND($$, 2)=fold(TREE_OPERAND($$, 2));
#if 0
    // check
    TREE_TYPE(TREE_OPERAND($$, 2)) = bitsizetype; // check. was u-
#endif

#if 0
    tree tmp = convert(integer_ptr_type_node, $$);
    if (TREE_CODE(tmp)==NOP_EXPR)
      tmp=TREE_OPERAND(tmp,0);
    $$ = build_indirect_ref (tmp, RO_UNARY_STAR); 
#endif
    $$ = fold($$);
  } else {
    tree tmp = convert(integer_ptr_type_node, $3);
    if (TREE_CODE(tmp)==NOP_EXPR)
      tmp=TREE_OPERAND(tmp,0);
    $$ = build_indirect_ref (input_location, tmp, RO_UNARY_STAR); 
    $$ = fold($$);
  }
  setbitcontext('o');
  popbitstack();
}
| UPLUS opexp9 %prec UNARY { $$ = build_unary_op (input_location, CONVERT_EXPR, $2, 0); }
| UMINUS opexp9 %prec UNARY { $$ = build_unary_op (input_location, NEGATE_EXPR, $2, 0); }
/*
| '+' opexp9 %prec UNARY { $$ = build_unary_op (input_location, CONVERT_EXPR, $2, 0); }
| '-' opexp9 %prec UNARY { $$ = build_unary_op (input_location, NEGATE_EXPR, $2, 0); }
*/
| opexp9 '^' opexp9 { 
  tree d3 = fold ( $3 );
  //  if (tree_int_cst_sgn (d3) < 0) 
  if (!tree_expr_nonnegative_p(d3)) {
    $$ = /*parser_*/build_binary_op (input_location, RSHIFT_EXPR, $1, build_unary_op(input_location,  NEGATE_EXPR, d3, 0), 1); // check -parser
    if (yydebug) inform(input_location, "\n%%BLS-I-NOTHING rshift %x\n");
  } else {
    $$ = /*parser_*/build_binary_op (input_location, LSHIFT_EXPR, $1, d3, 1); // check -parser
    if (yydebug) inform(input_location,"\n%%BLS-I-NOTHING lshift %x\n");
  }
}
| opexp9 K_MOD opexp9 { $$ = /*parser_*/build_binary_op (input_location, TRUNC_MOD_EXPR, $1, $3, 1); }
| opexp9 '*' opexp9 { $$ = /*parser_*/build_binary_op (input_location, MULT_EXPR, $1, convert(long_integer_type_node, $3), 1); } // 64-bit
| opexp9 '/' opexp9 { $$ = /*parser_*/build_binary_op (input_location, TRUNC_DIV_EXPR, $1, $3, 1); }
|
opexp9 '+' opexp9
{
  tree left = $1;
  tree right = $3;
  if (POINTER_TYPE_CHECK(left))
    left = convert (long_integer_type_node, left); // 64-bit
  if (POINTER_TYPE_CHECK(right))
    right = convert (long_integer_type_node, right); // 64-bit
  $$ = /*parser_*/build_binary_op (input_location, PLUS_EXPR, left, right, 1);
}
|
opexp9 '-' opexp9
{
  tree left = $1;
  tree right = $3;
  // check. removed?
#if 0
#if 0
  if (POINTER_TYPE_CHECK(left))
    left = convert (long_integer_type_node, left); // 64-bit
  if (POINTER_TYPE_CHECK(right))
    right = convert (long_integer_type_node, right); // 64-bit
#else
  if (POINTER_TYPE_CHECK(left))
    left = convert (integer_type_node, left);
  if (POINTER_TYPE_CHECK(right))
    right = convert (integer_type_node, right);
#endif
#endif
  $$ = /*parser_*/build_binary_op (input_location, MINUS_EXPR, left, right, 1); 
}
|
opexp9 infix_operator opexp9
{
  tree left = $1;
  tree right = $3;
  tree myop = $2;
  int op = TREE_OPERAND(myop, 0);
  int unsign = TREE_OPERAND(myop, 1);
  if (unsign) {
    left = c_cast_expr(input_location, build_type_name (unsigned_type_node), left); // check
    right = c_cast_expr(input_location, build_type_name (unsigned_type_node), right); //check
  } else {
    left = c_cast_expr(input_location, build_type_name (long_integer_type_node), left); // 64-bit
    right = c_cast_expr(input_location, build_type_name (long_integer_type_node), right); // 64-bit
}
  $$ = /*parser_*/build_binary_op (input_location, op, left, right, 1);
 }
| K_NOT opexp9 %prec K_NOT { $$ = build_unary_op (input_location, BIT_NOT_EXPR, convert(long_integer_type_node, $2), 0); /* 64-bit */ }
| opexp9 K_AND opexp9 { $$ = /*parser_*/build_binary_op (input_location, BIT_AND_EXPR, $1, $3, 1); }
| opexp9 K_OR opexp9 { $$ = /*parser_*/build_binary_op (input_location, BIT_IOR_EXPR, convert(long_integer_type_node,$1), convert(long_integer_type_node,$3), 1); /* temp fix */ /* 64-bit */ }
| opexp9 K_EQV opexp9 { $$ = build_unary_op(input_location, BIT_NOT_EXPR,/*parser_*/build_binary_op (input_location, BIT_XOR_EXPR, $1, $3, 1),0); }
| opexp9 K_XOR  opexp9 { $$ = /*parser_*/build_binary_op (input_location, BIT_XOR_EXPR, $1, $3, 1); }
|
opexp9 '=' opexp9 { 
  tree t=$1;
  tree b=TREE_OPERAND (t, 2);
  if (TREE_CODE(t) == BIT_FIELD_REFS && b && TREE_CODE(b) == BIT_FIELD_REF) {
    tree op0=TREE_OPERAND(b, 0);
    if (TREE_OPERAND(b, 1)) TREE_OPERAND(b, 1)=fold(TREE_OPERAND(b, 1));
    if (TREE_OPERAND(b, 2)) TREE_OPERAND(b, 2)=fold(TREE_OPERAND(b, 2));
#if 0
    // check
    TREE_TYPE(TREE_OPERAND(b, 2)) = bitsizetype; // check. was u-
#endif
    tree tt=bli_make_pointer_declarator(0,op0);
    TREE_TYPE(tt) = long_integer_type_node; // 64-bit
    TREE_OPERAND(b, 0)=tt;
    $$=build_modify_expr(input_location, b, 0, NOP_EXPR, input_location, $3, 0);
  } else {
    tree type = TREE_TYPE(t);
    tree ptype;
    tree conv;
    if (TREE_CODE(type)==POINTER_TYPE) {
      conv = t;
    } else {
      ptype = build_pointer_type(type);
      conv = convert (ptype, (t));
    }
    tree tmp=fold(build_indirect_ref (input_location, conv, RO_UNARY_STAR));
#if 1
    if (TREE_CODE(tmp)==FUNCTION_DECL)
      tmp =  build_indirect_ref(input_location, convert(integer_ptr_type_node, build_unary_op (input_location, ADDR_EXPR, tmp, 1)), RO_UNARY_STAR) ;
    if (TREE_CODE($3)==FUNCTION_DECL) // check
      $3 =  build_indirect_ref(input_location, convert(integer_ptr_type_node, build_unary_op (input_location, ADDR_EXPR, $3, 1)), RO_UNARY_STAR) ;
#endif
    if (TREE_CODE($3)==STRING_CST)
      $3 = convert_string_literal_to_integer($3);
    $$=build_modify_expr(input_location, tmp, 0, NOP_EXPR, input_location, $3, 0);
  }
}
;

opexp9:
primary
|
operator_expression 
|
executable_function 
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
K_EQL   { $$ = build_nt (MYSIGN, EQ_EXPR, 0); }
| K_EQLA   { $$ = build_nt (MYSIGN, EQ_EXPR, 1); }
| K_EQLU  { $$ = build_nt (MYSIGN, EQ_EXPR, 1); }
| K_NEQ   { $$ = build_nt (MYSIGN, NE_EXPR, 0); }
| K_NEQA   { $$ = build_nt (MYSIGN, NE_EXPR, 1); }
| K_NEQU   { $$ = build_nt (MYSIGN, NE_EXPR, 1); }
| K_LSS   { $$ = build_nt (MYSIGN, LT_EXPR, 0); }
| K_LSSA   { $$ = build_nt (MYSIGN, LT_EXPR, 1); }
| K_LSSU   { $$ = build_nt (MYSIGN, LT_EXPR, 1); }
| K_LEQ  { $$ = build_nt (MYSIGN, LE_EXPR, 0); }
| K_LEQA   { $$ = build_nt (MYSIGN, LE_EXPR, 1); }
| K_LEQU   { $$ = build_nt (MYSIGN, LE_EXPR, 1); }
| K_GTR   { $$ = build_nt (MYSIGN, GT_EXPR, 0); }
| K_GTRA   { $$ = build_nt (MYSIGN, GT_EXPR, 1); }
| K_GTRU   { $$ = build_nt (MYSIGN, GT_EXPR, 1); }
| K_GEQ   { $$ = build_nt (MYSIGN, GE_EXPR, 0); }
| K_GEQA  { $$ = build_nt (MYSIGN, GE_EXPR, 1); }
| K_GEQU   { $$ = build_nt (MYSIGN, GE_EXPR, 1); }
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
op_exp12:  op_exp11 /*'='*/ '@' op_exp12  
| op_exp11  
;
/*
  operator_expression: op_exp12 
  ;
*/
executable_function:
executable_function_name '('  actual_parameter_list  ')'
{
  // TODO check redo
  //if (IDENTIFIER_POINTER($1)==P_REF)
  //  goto do_pref;
  if (TREE_CODE($1)==IDENTIFIER_NODE && 0==strcmp("ch$allocation",IDENTIFIER_POINTER($1))) {
    // temp workaround to ctce if par is ctce 
    if (TREE_CHAIN($3))
      goto no_ch_ctce;
    tree val = TREE_VALUE($3);
    if (TREE_CODE(val)!=INTEGER_CST)
      goto no_ch_ctce;
    int v=TREE_INT_CST_LOW(val);
    $$ = build_int_cst (long_integer_type_node, (31+v*8)>>5); // 64-bit
    if (!quiet_flag)
      inform("ctce ch$all %x\n",(31+v*8)>>5);
    goto out_exec_func;
  }

  if (TREE_CODE($1)==IDENTIFIER_NODE && 0==strcmp("nullparameter",IDENTIFIER_POINTER($1))) {
    tree decl = current_function_decl;
    tree arg = DECL_ARGUMENTS(decl);
    tree val = DECL_NAME(TREE_OPERAND(TREE_VALUE($3),0));
    int count=1;
    tree tmp;
    for (tmp=arg; tmp && DECL_NAME(tmp) != val; tmp=TREE_CHAIN(tmp), count++) ;
    int counted = TREE_LANG_FLAG_5(current_function_decl);
    tree arg2 = arg;
    $3 = chainon (build_tree_list(0, arg2),$3);
    $3 = chainon(build_tree_list (0, build_int_cst (long_integer_type_node, count)), $3); // 64-bit
    $3 = chainon(build_tree_list (0, build_int_cst (long_integer_type_node, counted)), $3); // 64-bit
  }
 no_ch_ctce:
  {
  // copy from routine call?
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  tree ref = build_external_ref (input_location, $1, 1, 0);
  if (0==strcmp("signal",IDENTIFIER_POINTER($1)))
    SET_DECL_ASSEMBLER_NAME(ref, get_identifier("bli_signal"));
  $$ = build_function_call (input_location, ref, $3); 
  goto out_exec_func;
  }
 do_pref:
  // not quite according to 5.2.2.3
#if 0
  // not do this yet
  if (TREE_READONLY(TREE_VALUE($3)) || TREE_CONSTANT(TREE_VALUE($3))) 
    $$ = build_unary_op(input_location, ADDR_EXPR, TREE_VALUE($3), 0); // cut this sometime?
  else {
    tree decl = create_temp_var();
    tree value = TREE_VALUE($3);
    tree t = value;
    
    t = set_temp_var(decl, value);
#if 0
    c_expand_expr_stmt(t);
#endif
    $$ = build_unary_op(input_location, ADDR_EXPR, decl, 0);
  }
#else
  $$ = build_unary_op(input_location, ADDR_EXPR, TREE_VALUE($3), 0);
#endif

 out_exec_func:
}
|executable_function_name '(' ')' 
{
  // copy from routine call?
  tree ref;
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  ref = build_external_ref (input_location, $1, 1, 0);
  $$ = build_function_call (input_location, ref, 0); 
}
;

executable_function_named:
standard_function_name
/* standard_function_name  */
| 
linkage_function_name 
| character_handling_function_name 
/*
| machine_specific_function_name */
| cond_handling_function_name 
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

standard_function_name: /*T_NAME also min max */
P_REF
{
  //$$ = P_REF;
}
;

character_handling_function_name: 
N_CH_ALLOCATION
{ /* was: T_NAME */ }
;
machine_specific_function_name: T_NAME 
;
cond_handling_function_name:
K_SIGNAL
| K_STOP
| K_SETUNWIND 
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

if_then:
K_IF
{
  $<type_node_p>$ = c_begin_compound_stmt (1);
}
exp
{
  compstmt_count++;
  $<type_node_p>$=c_begin_compound_stmt (1);
}
K_THEN
pushlevel exp poplevel
{
  if ($7)
    add_stmt ($7);

#if 0
  $$=bli_pop_scope();
#endif
  $3 = c_common_truthvalue_conversion (input_location, build_binary_op(input_location, BIT_AND_EXPR,convert(integer_type_node,$3),build_int_cst (long_integer_type_node, 1), 1)); // 64-bit
  $$ = c_end_compound_stmt (input_location, $<type_node_p>4, 1);
  $$ = build_tree_list ($$, build_tree_list ($<type_node_p>2, $3));
}
;

conditional_expression: 
if_then
K_ELSE
{
  compstmt_count++;
  $<type_node_p>$=c_begin_compound_stmt (1);
}
pushlevel exp poplevel
{
  if ($5)
    add_stmt ($5);

  $5 = c_end_compound_stmt (input_location, $<type_node_p>3, 1);
#if 0
  $$=bli_pop_scope();
#endif

  tree block = TREE_PURPOSE(TREE_VALUE($1));
  tree body1 = TREE_PURPOSE($1);
  tree cond = TREE_VALUE(TREE_VALUE($1));
  tree body2 = $5;
  c_finish_if_stmt (input_location, cond, body1, body2); // check last
  add_stmt (c_end_compound_stmt (input_location, block, 1));

  $$ = 0;  //TODO
}
|
if_then %prec K_IF
{
  tree nop = build_int_cst (long_integer_type_node, 0); // 64-bit

  tree block = TREE_PURPOSE(TREE_VALUE($1));
  tree body1 = TREE_PURPOSE($1);
  tree cond = TREE_VALUE(TREE_VALUE($1));
  c_finish_if_stmt (input_location, cond, body1, nop /*0*/); // check last
  add_stmt (c_end_compound_stmt (input_location, block, 1));

  $$ = 0; // TODO
}
;

exp:
expression 
;

case_expression:
K_CASE exp K_FROM
{
  cntrli++;
  cntrls[cntrli].c_break_label = create_artificial_label (input_location);
  cntrls[cntrli].block_value = create_tmp_var (long_integer_type_node, "loopval");
  $<type_node_p>$ = c_start_switch (input_location, input_location, $2, 0);
  // c_in_case_stmt++; // check
}
ctce K_TO ctce K_OF
{
  c_start_case_2 ($5, $7);
#if 0
  c_break_label = create_artificial_label (input_location);
#endif
  $<type_node_p>$=c_begin_compound_stmt (1);
}
K_SET case_line_list K_TES
{
  $$ = c_end_compound_stmt (input_location, $<type_node_p>9, 1);
  //c_break_label = 0;
  //tree nop = build_int_cst (long_integer_type_node, 0); // 64-bit
  c_finish_switch ($$, 0 /*nop*/ /*0*/); // check
  // c_in_case_stmt--; // check
  add_stmt (build1 (LABEL_EXPR, void_type_node, cntrls[cntrli].c_break_label));
  $$ = cntrls[cntrli].block_value;
  cntrli--;
}
;

case_line_list:
case_line_list
{ 
  yyrec = 1;
  // handle no ; before tes
}
case_line 
|
{ 
  yyrec = 1;
  // handle no ; before tes
}
case_line 
;

case_line:
'[' case_label_list ']' ':' case_action ';'
{
  tree decl = cntrls[cntrli].block_value;
  $5 = build2 (MODIFY_EXPR, TREE_TYPE (decl), decl, $5);
  add_stmt ($5);
  if (cntrls[cntrli].c_break_label)
    add_stmt (build1 (GOTO_EXPR, void_type_node, cntrls[cntrli].c_break_label));
}
|
'[' case_label_list ']' ':' case_action error
{
  yyerrok;
  // another deviation. handle no ';' before tes
  tree decl = cntrls[cntrli].block_value;
  $5 = build2 (MODIFY_EXPR, TREE_TYPE (decl), decl, $5);
  add_stmt ($5);
  if (cntrls[cntrli].c_break_label)
    add_stmt (build1 (GOTO_EXPR, void_type_node, cntrls[cntrli].c_break_label));
}
;

case_label_list:
case_label_list ','  case_label 
{
  $$ = chainon($1, $3);
}
|
case_label 
;

case_label:
ctce
{
  $$ = do_case (input_location, $1, 0, 0);
}
|
ctce K_TO ctce 
{
  $$ = do_case (input_location, $1, $3, 0);
}
|
K_INRANGE
{
  $$ = build_inrange ();
}
|
K_OUTRANGE
{
  $$ = build_outrange (); // check. this is working?
}
;

case_action:
expression 
|
error
;

select_expression:
select_type select_index
{
  cntrli++;
#if 1
  // selectone problem
  cntrls[cntrli].c_break_label = create_artificial_label (input_location);
#endif
  cntrls[cntrli].block_value = create_tmp_var (long_integer_type_node, "loopval");
  cntrls[cntrli].myselect = fold($2);
  tree d1 = $1;
  cntrls[cntrli].unsign = TREE_OPERAND(d1, 1);
  if (cntrls[cntrli].unsign)
    cntrls[cntrli].myselect = c_cast_expr (input_location, build_type_name (unsigned_type_node), cntrls[cntrli].myselect); // check
  else
    cntrls[cntrli].myselect = c_cast_expr (input_location, build_type_name (long_integer_type_node), cntrls[cntrli].myselect); // 64-bit
#if 0
  tree label = build_decl (LABEL_DECL, NULL_TREE, NULL_TREE);
  tree decl=label;
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
#else
#if 0
  // not yet select instead of selectone
  tree type = TREE_OPERAND(d1, 0);
  if (strstr(IDENTIFIER_POINTER(type),"one"))
    cntrls[cntrli].c_break_label = create_artificial_label (input_location);
  else
    cntrls[cntrli].c_break_label = 0;
#endif
#endif
#if 0
  // not yet
  tree label_decl = decl;
  tree node = build (LABELED_BLOCK_EXPR, NULL_TREE, label_decl, NULL_TREE);
  TREE_SIDE_EFFECTS (node) = 1;
  TREE_TYPE(node)=void_type_node;
  LABELED_BLOCK_LABEL(node)=label_decl; // superfluous?
  // not yet? TREE_OPERAND(node, 1)= cur_last; // temp storage
  $<type_node_p>$=node;
#endif
#if 0
  // not now
  $<type_node_p>$ = c_start_case ($2);
  c_in_case_stmt++;
#endif
}
K_OF K_SET select_line_list K_TES
{
#if 0
  // not now
  c_finish_case ();
  c_in_case_stmt--;
  $$=0; 
  //  $$=$<type_node_p>3;
#endif
  $$ = 0;
#if 0
  $$=$<type_node_p>3;
  $$=c_expand_expr_stmt($$);
#endif
  // simplest right now?
#if 0
  C_DECLARED_LABEL_FLAG (mylabel) = 1;
  add_decl_stmt (mylabel);
#else
#if 0
  tree label = declare_label (mylabel);
  C_DECLARED_LABEL_FLAG (label) = 1;
  add_stmt (build_stmt (DECL_EXPR, label));
#endif
#endif
  if (cntrls[cntrli].c_break_label)
    add_stmt (build1 (LABEL_EXPR, void_type_node, cntrls[cntrli].c_break_label));
  $$ = cntrls[cntrli].block_value;
  cntrli--;
}
;

select_type: 
K_SELECT { $$ = build_nt (MYSIGN, $1, 0); }
| K_SELECTA { $$ = build_nt (MYSIGN, $1, 1); }
| K_SELECTU { $$ = build_nt (MYSIGN, $1, 1); }
| K_SELECTONE { $$ = build_nt (MYSIGN, $1, 0); }
| K_SELECTONEA { $$ = build_nt (MYSIGN, $1, 1); }
| K_SELECTONEU { $$ = build_nt (MYSIGN, $1, 1); }
;

select_index:
expression 
;

select_line_list:
select_line_list
{ 
  //yyrec = 1;
  // handle no ; before tes
}
select_line 
{
  // check. must connect in some other way, later
  // do not. last in list got chained to itself
  //$$ = chainon($1, $2);
}
|
{ 
  //yyrec = 1;
  // handle no ; before tes
}
select_line 
;

error_or_semi:
error
{ yyerrok; }
|
';'
;

select_line:
'[' 
{
  $<type_node_p>$ = c_begin_compound_stmt (1);
}
select_label_list ']' ':'
{
  tree t = 0;
  tree cond = $3;
  if (cond==0) {
    t=build_int_cst (long_integer_type_node, 1); // 64-bit
    goto no_t;
  }
  t = TREE_VALUE(cond);
  for (cond=TREE_CHAIN(cond);cond;cond=TREE_CHAIN(cond)) {
    t = /*parser_*/build_binary_op(input_location, BIT_IOR_EXPR,t,TREE_VALUE(cond), 1);
  } 
  t = c_common_truthvalue_conversion (input_location, t);
 no_t:

#if 0
  // not now
  //chainon ($2, build_stmt (EXPR_STMT, $5));
  add_stmt (build_stmt (EXPR_STMT, $<type_node_p>5));
  add_stmt (build_break_stmt ()); // selectone always
#endif
  yyrec = 1;
  $<type_node_p>$ = t;
}
mydummy
{
  $<type_node_p>$ = c_begin_compound_stmt (1);
}
select_action_with_end
{
  tree decl = cntrls[cntrli].block_value;
  $9 = build2 (MODIFY_EXPR, TREE_TYPE (decl), decl, $9);
  add_stmt ($9);
  if (!lastotherwise && cntrls[cntrli].c_break_label)
    add_stmt ( build1 (GOTO_EXPR, void_type_node, cntrls[cntrli].c_break_label));
  tree body1 = c_end_compound_stmt (input_location, $<type_node_p>8, 1);
  // only if one?

  tree nop = build_int_cst (long_integer_type_node, 0); // 64-bit

  tree block = $<type_node_p>2;
  tree cond = $<type_node_p>6;
  c_finish_if_stmt (input_location, cond, body1, nop /*0*/); // check last
  add_stmt (c_end_compound_stmt (input_location, block, 1));
  $$ = 0;
}
;

select_label_list:
select_label_list ',' select_label 
{
  $$ = chainon($1, $3);
}
|
select_label 
;

select_label:
exp  
{
  lastotherwise = 0;
#if 0
  $$ = do_case ($1, 0);
#endif
  tree e1 = $1;
  if (TREE_CODE(e1)==IDENTIFIER_NODE) {
    char *s=IDENTIFIER_POINTER(e1);
    e1=build_int_cst (long_integer_type_node, s[1]); // 64-bit
  }
  if (TREE_CODE(e1)==STRING_CST) {
    char *s=TREE_STRING_POINTER(e1);
    e1=build_int_cst (long_integer_type_node, s[1]); // 64-bit
  }
  if (cntrls[cntrli].unsign)
    e1 = c_cast_expr (input_location, build_type_name (unsigned_type_node), e1); // check
  else
    e1 = c_cast_expr (input_location, build_type_name (long_integer_type_node), e1); // 64-bit
		 
  tree t = /*parser_*/build_binary_op(input_location, EQ_EXPR,cntrls[cntrli].myselect,e1,1);
  $$ = tree_cons (0, t, 0);
}
|
exp K_TO exp
{
  lastotherwise = 0;
#if 0
  // can not be variable. for future?
  $$ = do_case ($1, $3);
#endif
  tree e1 = $1;
  if (TREE_CODE(e1)==IDENTIFIER_NODE) {
    char *s=IDENTIFIER_POINTER(e1);
    e1=build_int_cst(long_integer_type_node, s[1]); // 64-bit
  }
  if (TREE_CODE(e1)==STRING_CST) {
    char *s=TREE_STRING_POINTER(e1);
    e1=build_int_cst (long_integer_type_node, s[1]); // 64-bit
  }
  tree e2 = $3;
  if (TREE_CODE(e2)==IDENTIFIER_NODE) {
    char *s=IDENTIFIER_POINTER(e2);
    e2=build_int_cst(long_integer_type_node, s[1]); // 64-bit
  }
  if (TREE_CODE(e2)==STRING_CST) {
    char *s=TREE_STRING_POINTER(e2);
    e2=build_int_cst (long_integer_type_node, s[1]); // 64-bit
  }
  tree t1=/*parser_*/build_binary_op(input_location, GE_EXPR,cntrls[cntrli].myselect,e1,1);
  tree t2=/*parser_*/build_binary_op(input_location, LE_EXPR,cntrls[cntrli].myselect,e2,1);
  tree t = /*parser_*/build_binary_op(input_location, BIT_AND_EXPR, t1, t2, 1); 
  $$ = tree_cons (0, t, 0);
}  
|
K_OTHERWISE
{
  {
    // get error in select_line_list
    yyrec=1;
    // workaround to handle some accepted grammar contrary to specs
    // [OTHERWISE:] TES;
    lastotherwise = 1;
  } 
#if 0
  $$ = do_case (0, 0);
#endif
  $$ = 0;
}
|
K_ALWAYS
{
  // not yet
  $$ = 0;
}
;

select_action:
expression
;

select_action_with_end:
select_action error_or_semi
|
error {
  yyerrok;
  // handle empty otherwise
  $$ = build_int_cst (long_integer_type_node, 0); // 64-bit
}
;

loop_expression:
indexed_loop_expression  
|
tested_loop_expression  
;

by_exp:
{
  $$ = build_int_cst (long_integer_type_node, 1); // 64-bit
}
|
K_BY exp
{
  $$ = $2;
}
;

from_exp:
{
  $$ = build_int_cst (long_integer_type_node, 0); // 64-bit
}
|
K_FROM exp
{
  $$ = $2;
}
;

to_exp:
{
  $$ = build_int_cst (long_integer_type_node, 0); // 64-bit
}
|
K_TO exp
{
  $$ = $2;
}
;

indexed_loop_expression:
indexed_loop_type
{
  cntrli++;
  $<type_node_p>$ = c_begin_compound_stmt (1);
  cntrls[cntrli].c_break_label = create_artificial_label (input_location);
  //exitblock_value = create_temp_var();
  cntrls[cntrli].block_value = create_tmp_var (long_integer_type_node, "loopval");
  //exitblock_value = create_tmp_var_raw (long_integer_type_node, 0);
}
 T_NAME
from_exp  
{
  // $5
  tree decl = lookup_name ($3); // check. only originial others were I_TAG
  if (!decl) {
  // bad, but temp workaround
  tree tmp = bli_start_decl($3, long_integer_type_node, 1, 0); // 64-bit
  finish_decl(tmp, input_location, 0, 0, 0);
  }
  tree count = build_external_ref(input_location, $3, 1, 0);
  tree init = build_modify_expr (input_location, count, 0, NOP_EXPR, input_location, $4, 0);
  add_stmt(init);
}
to_exp   by_exp  K_DO 
{
  $<type_node_p>$ = c_begin_compound_stmt (1);
}
exp 
{
  tree block, cond, incr, body;
  tree count = build_external_ref(input_location, $3, 1, 0);
  tree check = /*parser_*/build_binary_op (input_location, LE_EXPR, count, $6, 1);
  block = $<type_node_p>2;
  tree block_value = $10;
  cond = c_common_truthvalue_conversion (input_location, check);

  tree incr_count=$7;
  if (incr_count==0) {
    tree f = build_int_cst (long_integer_type_node, 1); // 64-bit
    incr_count=f;
  }
  incr = build_modify_expr (input_location, count, 0, PLUS_EXPR, input_location, incr_count, 0);

  body = c_end_compound_stmt (input_location, $<type_node_p>9, 1);
  c_cont_label = 0;
  // TODO c_finish_loop (input_location, cond, incr, body, cntrls[cntrli].c_break_label, c_cont_label, true, cntrls[cntrli].block_value, block_value);
  add_stmt (c_end_compound_stmt (input_location, block, 1));

  $$ = build_int_cst(integer_type_node, 0);
  $$ = cntrls[cntrli].block_value;
  cntrli--;
}
;

indexed_loop_type:
 K_INCR | K_INCRA | K_INCRU  | K_DECR | K_DECRA | K_DECRU 
;

tested_loop_expression:
pre_tested_loop  
| post_tested_loop  
;

pre_tested_loop:  
k_while_or_until 
{
  cntrli++;
  $<type_node_p>$ = c_begin_compound_stmt (1);
  cntrls[cntrli].c_break_label = create_artificial_label (input_location);
  cntrls[cntrli].block_value = create_tmp_var (long_integer_type_node, "loopval");
}
exp
{
  $<type_node_p>$ = $3; // check
  tree t=$<type_node_p>$;
  t=/*parser_*/build_binary_op(input_location, BIT_AND_EXPR,t,build_int_cst (long_integer_type_node, 1), 1); // 64-bit
  t = c_common_truthvalue_conversion ( input_location, t ); 
  if ($1)
	 t = build_unary_op (input_location, TRUTH_NOT_EXPR, t, 0);
  $<type_node_p>$ = c_common_truthvalue_conversion (input_location, t);
}
K_DO
{
  $<type_node_p>$ = c_begin_compound_stmt (1);
}
exp
{
  tree block, cond, body;
  $$ = c_end_compound_stmt (input_location, $<type_node_p>6, 1);
  cond = $<type_node_p>4;
  block = $<type_node_p>2;
  tree block_value = $7;
  body = $$;
  c_cont_label = 0;
  // TODO c_finish_loop (input_location, cond, NULL, body, cntrls[cntrli].c_break_label, c_cont_label, true, cntrls[cntrli].block_value, block_value);
  add_stmt (c_end_compound_stmt (input_location, block, 1));
  $$ = cntrls[cntrli].block_value;
  cntrli--;
 } 
/*| K_UNTIL  exp K_DO exp { $$ = 0; }*/
;

k_while_or_until:
K_WHILE { $$ = 0; }
|
K_UNTIL { $$ = 1; }
;

mydummy:

post_tested_loop:
K_DO 
{
  cntrli++;
  $<type_node_p>$ = c_begin_compound_stmt (1);
  cntrls[cntrli].c_break_label = create_artificial_label (input_location);
  cntrls[cntrli].block_value = create_tmp_var (long_integer_type_node, "loopval");
}
mydummy
{
  // $<type_node_p>$ = add_stmt (build_stmt (DO_STMT, NULL_TREE, NULL_TREE));
  //  DO_COND ($<type_node_p>$) = error_mark_node; // see c-parse.y for why
  $<type_node_p>$ = c_begin_compound_stmt (1);
}
exp
{
  $<type_node_p>$ = c_end_compound_stmt (input_location, $<type_node_p>4, 1);
}
k_while_or_until
{
}
exp 
{ 
  tree block, cond, body;
  tree block_value = $5;
  tree t=$9;
  t=/*parser_*/build_binary_op(input_location, BIT_AND_EXPR,t,build_int_cst (long_integer_type_node, 1), 1); // 64-bit
  if ($<type_node_p>6)
	 t = build_unary_op (input_location, TRUTH_NOT_EXPR, t, 0);
  cond = t;
  block = $<type_node_p>2;
  body = $<type_node_p>6;
  c_cont_label = 0;
  // TODO c_finish_loop (input_location, cond, NULL, body, cntrls[cntrli].c_break_label, c_cont_label, false, cntrls[cntrli].block_value, block_value);
  add_stmt (c_end_compound_stmt (input_location, block, 1));
  $$=0;
  //DO_COND ($$) = c_common_truthvalue_conversion ($6);
  //  c_expand_expr_stmt($$);
  $$ = cntrls[cntrli].block_value;
  cntrli--;
}
/*
|
K_DO 
{
//  $<type_node_p>$ = add_stmt (build_stmt (DO_STMT, NULL_TREE, NULL_TREE));
//  DO_COND ($<type_node_p>$ = error_mark_node); // see c-parse.y for why
  $$ = c_begin_compound_stmt (1);
}
exp K_UNTIL
{
  $$ = $<type_node_p>2;
}
exp 
{ 
  $$ = $<type_node_p>5;
  $6 = build_unary_op (input_location, BIT_NOT_EXPR, $6, 0);
  DO_COND ($$) = c_common_truthvalue_conversion ($6);
}*/
;


exit_expression:
leave_expression  
|
exitloop_expression  
;

leave_expression: 
K_LEAVE T_LABELNAME K_WITH exp 
{
  // as said earlier, was T_NAME, but workaround
  tree decl;
  tree name=$2;
  decl = lookup_label(name);
  add_stmt ($4);
  add_stmt (build1 (GOTO_EXPR, void_type_node, decl));
  $$ = 0;
}
|
K_LEAVE T_LABELNAME  
{
  tree decl;
  tree name=$2;
  decl = lookup_label(name);
  add_stmt (build1 (GOTO_EXPR, void_type_node, decl));
  $$ = 0;
}
;

exitloop_expression: 
K_EXITLOOP exp {
  tree decl = cntrls[cntrli].block_value;
  tree value = $2;
  tree t = value;
  
  if (!(TREE_CODE(t)==VAR_DECL && DECL_ARTIFICIAL (decl))) 
    t = build2 (MODIFY_EXPR, TREE_TYPE (decl), decl, t);
  add_stmt (t);

  t = build1 (GOTO_EXPR, void_type_node, cntrls[cntrli].c_break_label);
  add_stmt (t);
  $$=0;
}
|
K_EXITLOOP
{
  tree t = build1 (GOTO_EXPR, void_type_node, cntrls[cntrli].c_break_label);
  add_stmt (t);
  $$=0;
}
;

return_expression: 
/*K_RETURN  exp { $$ = c_finish_return (input_location, build_compound_expr($2), NULL_TREE); $$=0; }*/
/*K_RETURN  exp { $$ = c_finish_return (input_location, c_expand_expr_stmt($2), NULL_TREE); $$=0; }*/
K_RETURN exp
{
  $$ = c_finish_return (input_location, $2, NULL_TREE);
  $$ = build_int_cst (long_integer_type_node, 0);
}
|
K_RETURN
{
  $$ = c_finish_return (input_location, NULL_TREE, NULL_TREE);
  $$ = build_int_cst (long_integer_type_node, 0);
}
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
|
';'
{
  $$ = 0;
  // this is not according to official specs
}
;

attribute_list:
attribute_list attribute
{ 
  $$ = tree_cons (NULL_TREE, $2, $1); 
}
|
attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

attribute:  allocation_unit { $$ = build_nt (ALLOC_ATTR, $1); } 
| extension_attribute { $$ = build_nt (EXTENSION_ATTR, $1); } 
| structure_attribute  
| field_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
| alignment_attribute
| initial_attribute 
| preset_attribute 
| { undefmode=1; } psect_allocation{ undefmode=0; $$=$2; }
| volatile_attribute
| novalue_attribute 
| linkage_attribute  
| range_attribute 
| { undefmode=1; if (yydebug) inform(input_location, "undefmode\n\n\n\n"); } addressing_mode_attribute { undefmode=0; $$=$2; } 
| weak_attribute  
|
external_name_attribute
;
/*
  allocation_unit:  K_LONG | K_WORD | K_BYTE 
  ;
*/
/*extension_attribute:  K_SIGNED | K_UNSIGNED 
;*/

maybe_ref:
{ $$ = 0; }
|
K_REF
;

maybe_alloc_actual_list:
{
  $$ = 0;
}
|
';' alloc_actual_list
{
  $$ = $2;
}
;

structure_attribute:
  K_REF T_NAME '[' alloc_actual_list ']'
{
  // for all 4: was T_NAME, but got grammar problems
  $$ = handle_structure_attribute($2, $4, 1);
  TREE_LANG_FLAG_0($$)=1;
}
|
  K_REF T_NAME
{
  $$ = handle_structure_attribute($2, 0, 1);
  TREE_LANG_FLAG_0($$)=1;
}
|
  T_NAME '[' alloc_actual_list ']'
{ 
  $$ = handle_structure_attribute($1, $3, 0);
}
|
  T_NAME
{
  $$ = handle_structure_attribute($1, 0, 0);
}
;

extension_attribute:
K_SIGNED
|
K_UNSIGNED
;
/*
  field_attribute: K_FIELD 
  ;
*/
alignment_attribute:
K_ALIGN '(' compile_time_constant_expression ')'
{
  $$ = build_nt (ALIGNMENT_ATTR, $3);
}
;

/*
  preset_attribute: K_PRESET 
  ;
*/
/*
psect_allocation: K_PSECT 
;
*/

volatile_attribute:
K_VOLATILE
{ 
  $$ = build_nt(VOLATILE_ATTR);
}
;

novalue_attribute:
K_NOVALUE
{ 
  $$ = build_nt(NOVALUE_ATTR);
}
;
linkage_attribute: /*T_NAME*/
T_LINKAGENAME
{
  $$ = build_nt(LINKAGE_ATTR, $1);
}
;

range_attribute: K_SIGNED compile_time_constant_expression { $$ = 0; }
|K_UNSIGNED compile_time_constant_expression { $$ = 0; }
;

ctce: compile_time_constant_expression 
;

ltce: linkage_time_constant_expression 
;
linkage_time_constant_expression:expression { /*nonfin*/ }
;

compile_time_constant_expression: expression { /*nonfin*/ }
;

addressing_mode_attribute: 
/*K_ADDRESSING_MODE { undefmode=1; } '(' mode_16 ')' 
  |*/
{
  undefmode=1;
  if (!quiet_flag)inform(input_location, "undefmode\n");
}
K_ADDRESSING_MODE '('  mode_32 ')'
{
  undefmode=0;
}
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


alloc_actual:
{
  $$=build_int_cst (long_integer_type_node, 0); // 64-bit
}
| ctce 
|
allocation_unit 
{
  int i=0;
  if ($1==char_type_node)
    i=8;
  if ($1==short_integer_type_node)
    i=16;
  if ($1==integer_type_node)
    i=32;
  if ($1==long_integer_type_node)
    i=64;
  i=i>>3;
  if (i)
    $$=build_int_cst (long_integer_type_node, i); // 64-bit
}
|extension_attribute  { $$ = 0; }
;

field_attribute: K_FIELD '(' field_stuff_list ')' { $$ = 0; }
;

field_stuff_list: field_stuff_list ','  field_name 
| field_stuff_list ','  field_set_name
| field_set_name 
| field_name 
;

initial_attribute:
K_INITIAL '(' initial_item_list ')'
{
  $$ = build_nt(INIT_ATTR, $3);
}
;

initial_item_list:
initial_item_list ',' initial_item 
{
  chainon($1, $3);
}
|
initial_item 
;

initial_item: 
initial_group  
{
  $$ = tree_cons($1, 0, 0);
}
|
initial_expression 
{
  $$ = tree_cons($1, 0, 0);
}
|
initial_string 
{
  $$ = tree_cons($1, 0, 0);
}
;

initial_group: 
allocation_unit '(' initial_item_list ')' /* not easy to interpret i_group */
{
  $$ = build_nt(INITIAL_GROUP, build_int_cst (long_integer_type_node, 1), $1, $3); // 64-bit
}
|
K_REP replicator K_OF '(' initial_item_list ')'
{
  $$ = build_nt(INITIAL_GROUP, $2, long_integer_type_node, $5); // 64-bit
}
|
K_REP replicator K_OF allocation_unit '(' initial_item_list ')'
{
  $$ = build_nt(INITIAL_GROUP, $2, $4, $6);
}
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

preset_item_list:
preset_item_list ',' preset_item
{
  chainon($1, $3);
}
|
preset_item
;

preset_attribute:
K_PRESET '(' preset_item_list ')' 
{
  $$ = build_nt (PRESET_ATTR, $3);
}
;

preset_item:
'[' ctce_access_actual_list ']' '=' preset_value
{
  $$ = tree_cons($5, $2, 0);
}
;

ctce_access_actual_list:
ctce_access_actual_list ',' ctce_access_actual
{
  $$ = chainon($1, tree_cons(0, $3, 0));
  //$$= tree_cons (NULL_TREE, $3, $1);

}
|
ctce_access_actual
{
  $$= tree_cons (NULL_TREE, $1, 0);
}
;

ctce_access_actual: 
ctce 
|
field_name 
;

preset_value:
expression 
;

psect_allocation: K_PSECT '(' T_NAME ')' { $$ = 0; }
;
/*
  volatile_attribute: K_VOLATILE
  ;
*/

data_declaration:  own_declaration
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
| own_item
;

own_name:
T_NAME
;

own_item:
own_name maybe_own_attribute_list
{
  tree cell, init, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree myattr = $2;
  
  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize = type;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    size=handle_structure(cell, st_attr, 1);
  }

  init = find_init_attr(myattr);

  tree orig_init = init;

  if (init)
	 init = TREE_PURPOSE(TREE_OPERAND(init, 0));

  int do_init = 0;
  if (init)
	 do_init=1;

  tree pres2 = find_tree_code(myattr, PRESET_ATTR); 
  if (pres2) {
    do_init=1;
  }

  if (st_attr) {
    mysize = build_our_record(fold(size));
  }
  tree cell2 = 0;
  if (TREE_LANG_FLAG_4(cell)) {
    cell_decl_p=identifier_global_value (cell);
    cell2 = get_identifier(add_underscore(DECL_ASSEMBLER_NAME(cell_decl_p), 1));
#if 0
    SET_DECL_ASSEMBLER_NAME(cell_decl_p,cell2);
#endif
    reset_symbol (cell);
#if 0
    TREE_LANG_FLAG_4(cell)=0;
    $$=0;
#endif
  }
  tree myname = 0;
  if (identifier_global_value(cell))
    myname = DECL_ASSEMBLER_NAME(identifier_global_value(cell));
  tree c_attr = default_own_section;
  tree c_align = get_alignment(myattr);
  if (c_align == 0)
    c_align = default_own_align;
  c_attr = chainon(c_attr, c_align);
  cell_decl_p = bli_start_decl (cell, mysize, do_init, c_attr);
#if 0
  if (default_own_section)
    DECL_SECTION_NAME(cell_decl_p)=default_own_section;
#endif
  if (myname)
    SET_DECL_ASSEMBLER_NAME(cell_decl_p,myname);
  if (TREE_LANG_FLAG_4(cell)) {
    //tree cell2 = get_identifier(add_underscore(DECL_ASSEMBLER_NAME(cell_decl_p), 1));
    SET_DECL_ASSEMBLER_NAME(cell_decl_p,cell2);
#if 0
    reset_symbol (cell) = 0;
#endif
    TREE_LANG_FLAG_4(cell)=0;
    $$=0;
  }
  TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?
  TREE_PUBLIC(cell_decl_p)=0;

  start_init(cell_decl_p,NULL,global_bindings_p(), 0, 0);
  finish_init();

  tree pres = find_tree_code(myattr, PRESET_ATTR);
  if (pres) {
    init = handle_preset($1, pres, 0, mysize);
  } else {
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      TREE_TYPE(cell_decl_p)=build_pointer_type(TREE_TYPE(cell_decl_p));
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      TREE_LANG_FLAG_3(cell_decl_p)=1;
  }

  if (orig_init && st_attr)
    init = handle_initial(0, orig_init, 0, mysize, 4);

  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);
}
;

maybe_own_attribute_list: { $$ = 0; }
| ':' own_attribute_list { $$ = $2; }
;

own_attribute_list:
own_attribute_list own_attribute
{ 
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
own_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

own_attribute:
allocation_unit { $$ = build_nt (ALLOC_ATTR, $1); } 
|extension_attribute { $$ = build_nt (EXTENSION_ATTR, $1); } 
|structure_attribute 
|field_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
|alignment_attribute
|initial_attribute 
|preset_attribute 
|psect_allocation 
|volatile_attribute
;

global_declaration: K_GLOBAL global_item_list  ';' { $$ = 0; }
;

global_item_list:  global_item_list ',' global_item 
|global_item 
;

global_name:
T_NAME;

maybe_global_attribute_list:
{ 
  $$ = 0;
}
|
':' global_attribute_list
{
  $$ = $2;
}
;

global_attribute_list:
attribute_list
;

global_item:
global_name maybe_global_attribute_list 
{
  // only differs in start_init?
  tree cell, init, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree myattr = $2;

  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize = type; // check long_integer_type_node; // 64-bit

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    size=handle_structure(cell, st_attr, 1);
  }

  init = find_init_attr(myattr);

  tree orig_init = init;

  if (init)
	 init = TREE_PURPOSE(TREE_OPERAND(init, 0));

  int do_init = 0;
  if (init)
	 do_init=1;

  tree pres2 = find_tree_code(myattr, PRESET_ATTR); 
  if (pres2) {
    do_init=1;
  }

  if (st_attr) {
    mysize = build_our_record(fold(size));
  }
  tree c_attr = default_global_section;
  tree c_align = get_alignment(myattr);
  if (c_align == 0)
    c_align = default_global_align;
  c_attr = chainon(c_attr, c_align);
  cell_decl_p = bli_start_decl (cell, mysize, do_init, c_attr);
#if 0
  if (default_global_section)
    DECL_SECTION_NAME(cell_decl_p)=default_global_section;
#endif

  TREE_STATIC(cell_decl_p)=1;

  start_init(cell_decl_p,NULL,1, 0, 0);
  finish_init();

  tree pres = find_tree_code(myattr, PRESET_ATTR);
  if (pres) {
    init = handle_preset($1, pres, 0, mysize);
  } else {
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      TREE_TYPE(cell_decl_p)=build_pointer_type(TREE_TYPE(cell_decl_p));
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      TREE_LANG_FLAG_3(cell_decl_p)=1;
  }

  if (orig_init && st_attr)
    init = handle_initial(0, orig_init, 0, mysize, 4);

  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);
  set_external_name(cell_decl_p, myattr);
}
;

external_declaration: K_EXTERNAL external_item_list ';' { $$ = 0; }
;

external_item_list: external_item_list ',' external_item 
|external_item 
;

external_name:
T_NAME
;

maybe_external_attribute_list:
{
  $$ = 0;
}
|
':' external_attribute_list
{
  $$ = $2;
}
;

external_attribute_list:
external_attribute_list external_attribute
{ 
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
external_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

external_attribute:
attribute
{
  // temp
}
;

external_item:
external_name maybe_external_attribute_list 
{
  tree cell, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree myattr = $2;
  
  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize = type;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    /*size=*/handle_structure(cell, st_attr, 1);
#if 0
    cell=bli_make_pointer_declarator(0,cell); // check?
#endif
  }

  cell_decl_p = bli_start_decl (cell, mysize, 0, 0);

  tree decl = cell_decl_p;
  int extern_ref = 1; 

  if (global_bindings_p())
  {
    TREE_PUBLIC (decl) = 1;
    TREE_STATIC (decl) = !extern_ref; // 0 
  } else {
    TREE_STATIC (decl) = 0;
    TREE_PUBLIC (decl) = extern_ref; // 1
  }

  DECL_EXTERNAL (cell_decl_p) = 1; // differs from bind here
  //TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

#if 0
  // not like bind here
  start_init(cell_decl_p,NULL,global_bindings_p());
  finish_init();
  finish_decl (cell_decl_p, 0, NULL_TREE);
  TREE_LANG_FLAG_0($1)=1;
#else
  finish_decl (cell_decl_p, input_location, 0, NULL_TREE, 0);
#endif
  set_external_name(cell_decl_p, myattr);
}
;

forward_declaration: K_FORWARD forward_item_list ';'  { $$ = 0; }
;

forward_name:
T_NAME
;

forward_item:
forward_name maybe_local_attribute_list 
{
  tree cell, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree myattr = $2;
  
  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize = type;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    /*size=*/handle_structure(cell, st_attr, 1);
#if 0
    cell=bli_make_pointer_declarator(0,cell); // check?
#endif
  }

  cell_decl_p = bli_start_decl (cell, mysize, 0, 0);

  tree decl = cell_decl_p;
  int extern_ref = 1; 

  if (global_bindings_p())
  {
    TREE_PUBLIC (decl) = 1;
    TREE_STATIC (decl) = !extern_ref; // 0 
  } else {
    TREE_STATIC (decl) = 0;
    TREE_PUBLIC (decl) = extern_ref; // 1
  }

  DECL_EXTERNAL (cell_decl_p) = 1; // differs from bind here
  //TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

#if 0
  // not like bind here
  start_init(cell_decl_p,NULL,global_bindings_p());
  finish_init();
  finish_decl (cell_decl_p, 0, NULL_TREE);
  TREE_LANG_FLAG_0($1)=1;
#else
  finish_decl (cell_decl_p, input_location, 0, NULL_TREE, 0);
#endif
}
;

forward_item_list: forward_item_list forward_item 
|forward_item 
;

local_declaration: K_LOCAL local_item_list ';'  { $$ = 0; }
;

local_item_list: local_item_list ',' local_item 
| local_item 
;

maybe_local_attribute_list: { $$ = 0; }
| ':' local_attribute_list { $$ = $2; }
;

local_attribute_list:
local_attribute_list local_attribute
{ 
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
local_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

local_attribute: own_attribute /* temporary */
;

local_name:
T_NAME
;

local_item:
local_name maybe_local_attribute_list
{
  tree cell, init, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  //  tree type = long_integer_type_node; // 64-bit
  tree myattr = $2;

  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize = type;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    size=handle_structure(cell, st_attr, 1);
  }

  init = find_init_attr(myattr);

  tree orig_init = init;

  if (init)
	 init = TREE_PURPOSE(TREE_OPERAND(init, 0));

  int do_init = 0;
  if (init)
	 do_init=1;

  tree pres2 = find_tree_code(myattr, PRESET_ATTR); 
  if (pres2) {
    do_init=1;//diff here
  }

  if (st_attr) {
    mysize = build_our_record(fold(size));
  }
  tree c_attr = 0;
  c_attr = chainon(c_attr, get_alignment(myattr));
#if 1
  tree pres = find_tree_code(myattr, PRESET_ATTR);
  if (pres == 0) {
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      mysize = build_pointer_type (mysize);
  }
#endif
  cell_decl_p = bli_start_decl (cell, mysize, do_init, c_attr);

  start_init(cell_decl_p,NULL,global_bindings_p(), 0, 0);
  finish_init();

#if 0
  tree pres = find_tree_code(myattr, PRESET_ATTR);
#endif
  if (pres) {
    init = handle_preset($1, pres, 0, mysize);
  } else {
#if 0
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      TREE_TYPE(cell_decl_p)=build_pointer_type(TREE_TYPE(cell_decl_p));
#endif
    if (st_attr && TREE_LANG_FLAG_0(st_attr))
      TREE_LANG_FLAG_3(cell_decl_p)=1;
  }

  if (orig_init && st_attr)
    init = handle_initial(0, orig_init, 0, mysize, 4);

  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);
  TREE_THIS_VOLATILE(cell_decl_p)=find_volatile_attr(myattr)!=0;
}
;

stackglocal_declaration: K_STACKLOCAL local_item_list ';' { $$ = 0; }
;

register_declaration: K_REGISTER register_item_list ';' { $$ = 0; }
;

register_item_list: register_item_list register_item 
|register_item 
;

register_name:
T_NAME
;

register_item: register_name '=' ctce ':' attribute_list 
;

global_register_declaration:
K_GLOBAL K_REGISTER global_reg_item_list ';' { $$ = 0; }
;

global_reg_item_list: global_reg_item_list global_reg_item 
|global_reg_item 
;

global_reg_item: register_name '=' ctce attribute_list 
;

external_register_declaration:
K_EXTERNAL K_REGISTER external_reg_item_list ';' { $$ = 0; }
;

external_reg_item_list:external_reg_item_list external_reg_item 
|external_reg_item 
;

external_reg_item: register_name '=' ctce ':' attribute_list 
;

map_declaration: K_MAP map_item_list ';' { $$ = 0; }
;

map_item_list: map_item_list ',' map_item 
|map_item 
;

map_name:
T_NAME
;

map_item:
map_name ':' attribute_list 
{
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree myattr = $3;
  
  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize=tree_cons(0, type, 0);

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    //    /*size=*/handle_structure(cell, st_attr, 1);
    size=handle_structure(cell, st_attr, 1);
  }

  cell_decl_p = build_external_ref (input_location, $1, yychar == '(', 0);

  if (type)
    /*TREE_TYPE*/(TREE_TYPE(cell_decl_p))=type;

}
;

structure_declaration:
  K_STRUCTURE structure_definition_list ';' { $$ = 0; }
;

structure_definition_list: structure_definition_list ',' structure_definition 
|structure_definition 
;

structure_definition:
T_NAME '['
{
  // $3

  tree int_tree, parm_tree, x;

  turn_off_addr_expr = 1;

  push_scope();
  declare_parm_level();
  struct c_parm * decl = build_parm_decl($1, long_integer_type_node); // 64-bit
  push_parm_decl(decl, 0); 
}
  access_formal_list  ';' 
{
  // $6

  tree accessid = get_identifier(add_underscore($1, 2));
  //tree accessls = $4;
   struct c_arg_info * arg_info2 = bli_get_parm_info(1, 0);
  tree accessfn = build_nt (CALL_EXPR, accessid, arg_info2, NULL_TREE);

  //push_parm_decl($<type_node_p>3);
  //mydeclares($4);
  //begin_stmt_tree(&$<type_node_p>$);
  //$<type_node_p>6=accessfn;

#if 0
  push_function_context ();
#endif
  accessfn = accessid; 
  struct c_arg_info * arg_info = bli_get_parm_info(1, 0);
#if 0
  push_function_context ();
#endif
  bli_start_function (long_integer_type_node, accessfn, arg_info, 0); // 64-bit
  store_parm_decls();
#if 0
  void * ret = bli_get_parm_info(1, 0);
  store_parm_decls_from (ret);
#endif
  afun=current_function_decl;

  $<type_node_p>$ = current_function_decl;
  push_scope();
  push_scope();
  declare_parm_level();
}
allocation_formal_list ']' '='
{
  // $10

  tree allocform_list = $7;
  tree allocid = get_identifier(add_underscore($1, 1));
  tree allocfn = build_nt (CALL_EXPR, allocid, allocform_list, NULL_TREE);

  //mydeclares($7);

  acfun=cfun;
  allocfn = allocid;
  struct c_arg_info * arg_info = bli_get_parm_info(1, 0);
#if 0
  push_scope();
#endif
#if 0
  push_function_context ();
#endif
  bli_start_function (long_integer_type_node, allocfn, arg_info, 0); // 64-bit
  store_parm_decls();
#if 0
  void * ret = bli_get_parm_info(1, 0);
  store_parm_decls_from (ret);
#endif

  //begin_stmt_tree(&$<type_node_p>$);
  //$<type_node_p>12=allocfn;

  $<type_node_p>$=allocfn;
  $<type_node_p>$ = current_function_decl;
}
structure_size
{
  // $12

  tree accessfn = $<type_node_p>6;

  tree decl = current_function_decl;
  add_stmt($11);
#if 1
  bli_finish_function (); 
#else
  finish_function (input_location);
  pop_function_context ();
#endif
  add_stmt (build_stmt (input_location, DECL_EXPR, decl));

  current_function_decl=afun;
  cfun=acfun;
  //begin_stmt_tree (&tr);
#if 0
  //
  begin_stmt_tree (&DECL_SAVED_TREE (current_function_decl));
#endif

  $<type_node_p>$=accessfn;
}
structure_body
{
  //$14

  tree comp;  
  tree tmp;
  tree accesstmp;
  tree body;
  tree size;
  tree alloc;
  tree access;
  tree comp2;

  //DECL_SAVED_TREE (current_function_decl)=$13;
#if 0
  bli_pop_scope();
#endif
  add_stmt($13);
#if 0
  chainon(DECL_ARGUMENTS(current_function_decl), DECL_ARGUMENTS($<type_node_p>10));
#endif
#if 1
bli_finish_function ();
#else
  finish_function (input_location);
  pop_function_context ();
#endif

  bli_pop_scope();
  bli_pop_scope();
  bli_pop_scope();

  tree d7 = DECL_ARGUMENTS($<type_node_p>10);
  tree d4 = DECL_ARGUMENTS($<type_node_p>6 /*12*/);

//  $$ = finish_struct ($<type_node_p>3, comp, NULL);

  accesstmp = first_to_last (d4); // check no

  body = tree_cons(0, $13, 0);
  size = tree_cons(0, $11, body);
  alloc = tree_cons(0, d7, size);
  access = tree_cons(0, /*$4*/accesstmp, alloc);
  comp2 = tree_cons(0, 0, access);

  $$ = start_structure (STRUCTURE_TYPE, $1);
  $$ = finish_structure ($$, comp2, 0, 0, 0, 0, 0); 

  //$$ = build_nt (STRUCTURE_DECL, $1, $4, $7, $11, $13);
  //add_struct(&mystructs,$$); 

  turn_off_addr_expr = 0;
}
;

allocation_formal_list:
allocation_formal_list ',' allocation_formal 
{ 
//$$ = tree_cons (NULL_TREE, $1, $3);
  /*$$ = bli_get_parm_info (1);*/
  //$$=chainon ($1, $3); 
}
|allocation_formal { 
  /*$$ = bli_get_parm_info (1);*/
//$$=$1; 
 }
;

allocation_formal:
allocation_name { 
  struct c_parm * decl = build_parm_decl ($1, long_integer_type_node); 
  push_parm_decl(decl, 0);
}
|allocation_name '=' allocation_default { 
  //$$ = tree_cons ($3, $1, NULL_TREE); 
#if 1
  //tree t = tree_cons (0, build_specs(long_integer_type_node), 0);   
  struct c_declspecs * declspecs = build_specs(long_integer_type_node); // 64-bit
  tree d = tree_cons ((void *) declspecs, (void *) build_id_declarator($1), 0);
  //struct c_parm * decl = tree_cons (d, 0, 0);
#else
  struct c_parm * decl = build_parm_decl ($1, long_integer_type_node); // 64-bit
#endif
  push_parm_decl_init(d, $3);
}
;

allocation_default: exp 
;

structure_size: { $$ = 0; /* empty */ }
|'[' expression ']' { $$=$2; }
;

structure_body:
  expression;

access_formal_list: access_formal_list ',' access_formal { 
  /*  $$ = bli_get_parm_info (1);*/
  //$$=chainon($1,$3); 
}
| access_formal 
{
  /*  $$ = bli_get_parm_info (1);*/
  //$$=$1;
}
;

access_formal: T_NAME {
//tree d = start_decl ($1, current_declspecs, 0,
//  chainon (NULL_TREE, all_prefix_attributes));
//finish_decl (d, 0, NULL_TREE);
  struct c_parm * decl = build_parm_decl($1, long_integer_type_node); // 64-bit
  push_parm_decl(decl, 0); 
}
;

allocation_name: T_NAME
;

field_declaration: K_FIELD 
field_something_definition_list ';'
{
  $$ = 0; 
}
;

field_something_definition_list:
field_something_definition_list ',' field_something_definition
|
field_something_definition
;

field_something_definition:
field_set_definition
|
field_definition

field_set_definition:
field_set_name '=' K_SET field_definition_list K_TES
;

field_definition_list:
field_definition_list ',' field_definition
|
field_definition
;

field_definition:
field_name '=' '[' field_component_list ']'
{
  register_field(IDENTIFIER_POINTER($1), $4);
}
;

field_component_list:
field_component_list ',' field_component
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
field_component
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

field_component:
ctce;

routine_declaration:
ordinary_routine_declaration 
|global_routine_declaration 
|forward_routine_declaration 
|external_routine_declaration 
;
ordinary_routine_declaration: 
K_ROUTINE routine_definition_list ';'
{ 
  $$ = $2;
}
;

routine_definition_list:
routine_definition_list ',' routine_definition 
|
routine_definition 
;

routine_definition: routine_name
{
}
io_list routine_attributes 
{
  // mark this as static?
  tree io_list = $3;
  tree myattr = $4;
  int do_counted_arg = is_counted(myattr);
  int is_void = find_novalue_attr(myattr);
  tree mytype = long_integer_type_node; // 64-bit
  if (is_void)
    mytype = void_type_node;
  tree fn;
#if 0
  if (io_list==0) io_list=build_tree_list (NULL_TREE, NULL_TREE);
#else
  if (io_list==0) {
#if 0
    // temp workaround to have ap with no formals
    tree d1 = get_identifier("__mydummy_for_ap__");
    tree type = long_integer_type_node; // 64-bit
    tree int_tree = tree_cons (NULL_TREE, type, NULL_TREE);
    tree point_int = tree_cons (int_tree, d1, 0);
    io_list = tree_cons (point_int, 0, 0);
#else
#if 1
    push_scope();
    // check: needs mark_forward_parm_decls (); maybe?
    declare_parm_level ();
    tree d1 = get_identifier("__mydummy_for_ap__");
    tree type = long_integer_type_node; // 64-bit
    tree int_tree = tree_cons (NULL_TREE, type, NULL_TREE);
    struct c_parm * decl = build_parm_decl(d1, long_integer_type_node); // 64-bit
    push_parm_decl (decl, 0);
    struct c_arg_info * arg_info = bli_get_parm_info(1, 0);
    bli_pop_scope();
#else
    tree type = long_integer_type_node;
    tree d1 = get_identifier("__mydummy_for_ap__");
    io_list = build_decl (PARM_DECL, d1, type);
#endif
#endif
  }
#endif
  struct c_arg_info * arg_info = build_arg_info();
  if (io_list != 0) arg_info = bli_get_parm_info(1, 0);
  // check. why CALL_EXPR?
  fn = build_nt (FUNCTION_DECL, $1, arg_info, NULL_TREE);
  fn = $1;
  bli_start_function (mytype, fn, arg_info, 0);
  if (0==strcmp("main", IDENTIFIER_POINTER($1))) {
    DECL_EXTERNAL (current_function_decl) = 0;
    TREE_PUBLIC (current_function_decl) = 1;
  } else {
    TREE_PUBLIC (current_function_decl) = 0;
  }
  TREE_LANG_FLAG_5(current_function_decl)=do_counted_arg;
#if 1
  store_parm_decls ();
#endif
}
'=' save_location exp 
{ 
  tree block_value = $8; 
#if 0
  if (block_value) $$ = c_finish_return (input_location, build_compound_expr(build_tree_list(NULL_TREE,$8),0), NULL_TREE); $$=0; // check
#else
  /*if (block_value) $$ = c_finish_return (input_location, $8, NULL_TREE);*/ $$=0; // check
  if (block_value) $$ = c_finish_return (input_location, $8, NULL_TREE); $$=0; // check
#endif
  if ($8)
    add_stmt ($8);
  finish_function (input_location); 
}
;

routine_attributes: { $$=0; }
| ':' routine_attribute_list { $$=$2; }
;

io_list2: 
io_list ':' routine_attribute_list 
|io_list 
;

io_list: { $$=0; }
| {
  push_scope();
  // check: needs mark_forward_parm_decls (); maybe?
  declare_parm_level ();
}
'(' formal_item_list ')'
{ 
  $$ = $3;
  struct c_arg_info * arg_info = bli_get_parm_info(1, 0);
  bli_pop_scope();
}
/*  |'(' formal_item_list ';' formal_item_list ')' 
    |'(' ';' formal_item_list ')' */
;

formal_item_list:
/* empty 6 */ { $$ = 0 ; } 
|formal_item_list ','  formal_item { /*$$ = bli_get_parm_info (1);*/ }
|formal_item { /*$$ = bli_get_parm_info (1);*/ }
;

formal_name:
T_NAME
;

formal_item: /*T_NAME ':' formal_attribute_list 
|*/
formal_name
{ 
  tree type = long_integer_type_node; // 64-bit
  tree int_tree = tree_cons (NULL_TREE, type, NULL_TREE);
  tree point_tree = bli_make_pointer_declarator (0, $1);
  point_tree = $1;

  struct c_parm * decl = build_parm_decl($1, long_integer_type_node); // 64-bit
  push_parm_decl (decl, 0);
}
;

routine_attribute_list:
routine_attribute_list routine_attribute 
{
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
routine_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
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
allocation_unit { $$ = build_nt (ALLOC_ATTR, $1); }
|extension_attribute { $$ = build_nt (EXTENSION_ATTR, $1); } 
|structure_attribute 
|field_attribute 
|volatile_attribute
;
routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
|weak_attribute 
;

global_routine_declaration:
K_GLOBAL K_ROUTINE global_routine_definition_list ';'
{ $$ = $3; }
;

global_routine_definition_list: global_routine_definition_list ',' global_routine_definition 
|global_routine_definition 
;

global_routine_definition:
routine_name
{
}
io_list global_routine_attributes 
{
  tree io_list = $3;
  tree myattr = $4;
  int do_counted_arg = is_counted(myattr);
  int is_void = find_novalue_attr(myattr);
  tree mytype = long_integer_type_node; // 64-bit
  if (is_void)
    mytype = void_type_node;
  tree fn;
#if 0
  if (io_list==0) io_list=build_tree_list (NULL_TREE, NULL_TREE);
#else
  if (io_list==0) {
#if 0
    // temp workaround to have ap with no formals
    tree d1 = get_identifier("__mydummy_for_ap__");
    tree type = long_integer_type_node; // 64-bit
    tree int_tree = tree_cons (NULL_TREE, type, NULL_TREE);
    tree point_tree = bli_make_pointer_declarator (0, d1);
    tree point_int = tree_cons (int_tree, point_tree, 0);
    io_list = tree_cons (point_int, 0, 0);
#else
#if 1
    push_scope();
    // check: needs mark_forward_parm_decls (); maybe?
    declare_parm_level ();
    tree d1 = get_identifier("__mydummy_for_ap__");
    tree type = long_integer_type_node; // 64-bit
    tree int_tree = tree_cons (NULL_TREE, type, NULL_TREE);
    struct c_parm * decl = build_parm_decl(d1, long_integer_type_node); // 64-bit
    push_parm_decl (decl, 0);
    struct c_arg_info * arg_info = bli_get_parm_info(1, 0);
    bli_pop_scope();
#else
    tree type = long_integer_type_node;
    tree d1 = get_identifier("__mydummy_for_ap__");
    io_list = build_decl (PARM_DECL, d1, type);
#endif
#endif
  }
#endif
    struct c_arg_info * arg_info = bli_get_parm_info(1, 0);
  fn = build_nt (CALL_EXPR, $1, arg_info, NULL_TREE);
  fn = $1;
  bli_start_function (mytype, fn, arg_info, 0);
  DECL_EXTERNAL (current_function_decl) = 0;
  TREE_PUBLIC (current_function_decl) = 1;
  TREE_LANG_FLAG_5(current_function_decl)=do_counted_arg;
  set_external_name(current_function_decl, myattr);
#if 1
  store_parm_decls ();
#endif
}
'=' save_location exp 
{ 
  tree block_value = $8; 
#if 0
  if (block_value) $$ = c_finish_return (input_location, build_compound_expr(build_tree_list(NULL_TREE,$8),0), NULL_TREE); $$=0; // check
#else
  if (block_value) $$ = c_finish_return (input_location, $8, NULL_TREE); $$=0; // check
#endif
  finish_function (input_location); 
}
;

global_routine_attributes:
{
  $$=0;
}
|
':' global_routine_attribute_list
{
  $$=$2;
}
;


global_io_list2: 
io_list ':'  global_routine_attribute_list 
|io_list  
;

global_routine_attribute_list:
{
  $$=0;
}
|
global_routine_attribute_list global_routine_attribute 
{
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
global_routine_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

global_routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
|weak_attribute 
|
external_name_attribute
;

forward_routine_declaration:
K_FORWARD K_ROUTINE forward_routine_item_list ';' { $$ = 0; }
;

forward_routine_item_list: forward_routine_item_list ',' forward_routine_item 
|forward_routine_item  
;

routine_name:
T_NAME
;

forward_routine_item: 
routine_name maybe_forward_routine_attribute_list 
/*|T_NAME  */
{
  tree myattr = $2;
  tree mytype = default_function_type;
  if (find_novalue_attr(myattr))
    mytype = build_function_type (void_type_node, 0);
  tree decl = build_decl(input_location, FUNCTION_DECL, $1, mytype);
  DECL_EXTERNAL (decl) = 1;
  decl = pushdecl (decl);
  $$ = 0;
}
;

maybe_forward_routine_attribute_list: { $$=0; }
|':' forward_routine_attribute_list { $$=$2; }
;
forward_routine_attribute_list:
forward_routine_attribute_list forward_routine_attribute 
{
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
forward_routine_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

forward_routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
;
external_routine_declaration:
K_EXTERNAL K_ROUTINE external_routine_item_list ';' { $$ = 0; }
;

external_routine_item_list: external_routine_item_list ',' external_routine_item 
|external_routine_item 
;

maybe_ext_routine_attribute_list:
{
  $$ = 0;
}
|
':' ext_routine_attribute_list
{
  $$ = $2;
}
;

external_routine_item:
routine_name maybe_ext_routine_attribute_list 
{ 
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  tree myattr = $2;
  $$ = build_external_ref (input_location, $1, 1, 0);
  if (is_counted(myattr))
    TREE_LANG_FLAG_5($$)=1;
  set_external_name($$, myattr);
}
;

ext_routine_attribute_list:
ext_routine_attribute_list ext_routine_attribute 
{
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
ext_routine_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

ext_routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
|weak_attribute 
|
external_name_attribute
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
compiletime_declaration:
K_COMPILETIME compile_time_item_list ';'
{ $$ = 0; }
;

macro_declaration: 
positional_macro_declaration 
|keyword_macro_declaration 
;
require_declaration: K_REQUIRE T_STRING ';'
{
  //push_srcloc($2,0);
  //pushfilestack();
  char *neww=xstrdup(TREE_STRING_POINTER($2)+1);
  neww[strlen(neww)-1]=0;
  char * colon = strchr(neww,':');
  if (colon)
	 neww=colon+1;
  char * brack = strchr(neww,']');
  if (brack)
	 neww=brack+1;
  struct dsc_dollar_descriptor dsc; 
  my_strcat(&dsc, strlen(neww), neww, 4, ".req", 0);
  char * new2 = dsc.dsc_dollar_a_pointer;
  if (!is_opened(new2))
	 push_req_stack(new2);

  //  pop_srcloc();
  $$ = 0;
}
;

library_declaration: K_LIBRARY T_STRING ';'
{
  // approx. doing the same as require, since we have no librarian utility
  //push_srcloc($2,0);
  //pushfilestack();
  char *neww=xstrdup(TREE_STRING_POINTER($2)+1);
  neww[strlen(neww)-1]=0;
  char * colon = strchr(neww,':');
  if (colon)
	 neww=colon+1;
  char * brack = strchr(neww,']');
  if (brack)
	 neww=brack+1;
  check_lib(neww);
  struct dsc_dollar_descriptor dsc; 
  my_strcat(&dsc, strlen(neww), neww, 4, ".req", 0);
  char * new2 = dsc.dsc_dollar_a_pointer;
  if (!is_opened(new2))
	 push_req_stack(new2);

  //  pop_srcloc();
  $$ = 0;
}
;

psect_declaration: K_PSECT psect_item_list ';' { $$ = 0; }
;

switches_declaration:
K_SWITCHES
{
  undefmode=1;
}
switch_item_list ';'
{
  undefmode=0;
  $$ = 0;
}
;

switch_item_list:
switch_item_list ',' switch_item
|
switch_item
;

switch_item:
on_off_switch_item
|
special_switch_item
;

special_switch_item:
common_switch
{
  // temp
}
;

on_off_switch_item:
onoffmodes
;

label_name_list:
label_name_list ',' T_NAME
|
T_NAME
{
  /*add_stmt*/ (lookup_label($1));
}
;

label_declaration:
K_LABEL label_name_list ';'
{
  $$ = 0;
}
;

built_in_name:
T_NAME
{
  if (0==strcmp(IDENTIFIER_POINTER($1),"ap")) {
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree type = long_integer_type_node; // 64-bit
  
  cell=$1;

  cell_decl_p = bli_start_decl (cell, long_integer_type_node, 1, 0); // 64-bit

  start_init(cell_decl_p,NULL,global_bindings_p(), 0, 0);
  finish_init();

  // also needs a dummy parameter __mydummy_for_ap__ if no formals
  init = build_unary_op (input_location, ADDR_EXPR, DECL_ARGUMENTS(current_function_decl), 1);
  if (TREE_LANG_FLAG_5(current_function_decl)==0)
    init = /*parser_*/build_binary_op (input_location, MINUS_EXPR, init, build_int_cst (long_integer_type_node, sizeof(long)), 1); // 64-bit check. fix to be sizeof(long) later when pointer arithmetic is fixed 
  
  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);
  } else {
#if 0
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree type = long_integer_type_node; // 64-bit
  
  cell=$1;

  cell_decl_p = bli_start_decl (cell, long_integer_type_node, 0, 0); // 64-bit

  tree decl = cell_decl_p;
  int extern_ref = 1; 

  if (global_bindings_p())
  {
    TREE_PUBLIC (decl) = 1;
    TREE_STATIC (decl) = !extern_ref; // 0 
  } else {
    TREE_STATIC (decl) = 0;
    TREE_PUBLIC (decl) = extern_ref; // 1
  }

  DECL_EXTERNAL (cell_decl_p) = 1; // differs from bind here
  //TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

  finish_decl (cell_decl_p, input_location, 0, NULL_TREE, 0);
#endif
  }
}
|
B_NULLPARAMETER
;

built_in_name_list:
built_in_name_list ',' built_in_name
|
built_in_name
;

builtin_declaration:
K_BUILTIN built_in_name_list ';'
{ $$ = 0; }
;

undeclare_declaration:
K_UNDECLARE tname_list ';'
{ 
  $$ = 0;
  tree tmp = $2;
  for(;tmp;tmp=TREE_CHAIN(tmp))
    TREE_LANG_FLAG_4(TREE_VALUE(tmp)) = 1;
#if 0
  if (TREE_READONLY(t)==0)
    reset_symbol (t) = 0;
#endif
}
;

literal_declaration: K_LITERAL literal_item_list ';' { $$ = $2; }
| K_GLOBAL K_LITERAL literal_item_list { $$ = 0; }
;

literal_item_list: literal_item_list ',' literal_item 
| literal_item 
;

literal_name:
T_NAME
;

literal_item: literal_name '=' compile_time_constant_expression ':' literal_attribute_list 
|literal_name '=' compile_time_constant_expression
{
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize =  long_integer_type_node; // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree type = long_integer_type_node; // 64-bit
  
  cell=$1;
  TREE_TYPE(cell)=long_integer_type_node; // 64-bit

  init=$3;
  if (TREE_LANG_FLAG_4(cell)) {
    cell_decl_p=identifier_global_value (cell);
    TREE_LANG_FLAG_4(cell)=0;
    $$=0;
  } else
  cell_decl_p = bli_start_decl (cell, mysize, 1, 0);
  TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?
  TREE_READONLY(cell_decl_p)=1;
  TREE_PUBLIC(cell_decl_p)=0;
  start_init(cell_decl_p,NULL,global_bindings_p(), 0, 0);  finish_init();
  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);
  TREE_LANG_FLAG_0($1)=1;
}
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

external_literal_item: literal_name ':' literal_attribute_list 
|
literal_name  
{
  tree cell_decl_p = bli_start_decl ($1, long_integer_type_node, 0, 0); // 64-bit

  tree decl = cell_decl_p;
  int extern_ref = 1; 

  if (global_bindings_p())
  {
    TREE_PUBLIC (decl) = 1;
    TREE_STATIC (decl) = !extern_ref; // 0 
  } else {
    TREE_STATIC (decl) = 0;
    TREE_PUBLIC (decl) = extern_ref; // 1
  }

  DECL_EXTERNAL(decl)=1;
  finish_decl (cell_decl_p, input_location, 0, NULL_TREE, 0);
  TREE_LANG_FLAG_0($1)=1;
}
;

bind_data_declaration: K_BIND bind_data_item_list ';'  { $$ = 0; }
| K_GLOBAL K_BIND bind_data_item_list { $$ = 0; }
;

bind_data_item_list: bind_data_item_list ',' bind_data_item 
| bind_data_item 
;

bind_data_item_0: T_NAME '=' expression 
;

bind_data_name:
T_NAME
;

data_name_value:
expression
;

bind_data_item:
bind_data_name '=' data_name_value maybe_bind_data_attribute_list
{
  tree cell, init, t, cell_decl_p;
  tree mysize=tree_cons(0,long_integer_type_node,0); // 64-bit
  tree size=tree_cons(0,long_integer_type_node,0); // 64-bit

  tree myattr = $4;
  
  tree type = find_alloc_attr(myattr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,myattr);
  mysize = type;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  int pointer = 0;
  if (st_attr) {
    //    /*size=*/handle_structure(cell, st_attr, 1);
    size=handle_structure(cell, st_attr, 1);
  } else {
    pointer = 1;
    //    cell = bli_make_pointer_declarator(0, cell);
    //mysize = build1 (INDIRECT_REF, mysize, 0);
  }

  init = $3;

  if (TREE_CODE(init)==BIT_FIELD_REFS) {
    init=TREE_OPERAND(init, 2);
    init=TREE_OPERAND(init, 0);
    my_fold_p(&init);
  }
#if 0
  TREE_CONSTANT(init)=1;
#endif

  cell_decl_p = bli_start_decl_2 (cell, mysize, 1, 0, pointer);
  if (global_bindings_p() /*current_function_decl == 0*/)
    TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

  start_init(cell_decl_p,NULL,global_bindings_p(), 0, 0);  finish_init();
  finish_decl (cell_decl_p, input_location, init, NULL_TREE, 0);
  TREE_LANG_FLAG_0(cell_decl_p)=1; // was $1
}
{
/*
 bind_data_item_0 ':' bind_data_attribute_list 
| bind_data_item_0 %prec '='
*/
}
;

maybe_bind_data_attribute_list: { $$ = 0; }
| ':' bind_data_attribute_list { $$ = $2; }
;

bind_data_attribute_list:
bind_data_attribute_list bind_data_attribute 
{
  $$= tree_cons (NULL_TREE, $2, $1);
}
|
bind_data_attribute 
{ 
  $$ = tree_cons (NULL_TREE, $1, 0); 
}
;

bind_data_attribute:
allocation_unit  { $$ = build_nt (ALLOC_ATTR, $1); } 
|extension_attribute { $$ = build_nt (EXTENSION_ATTR, $1); } 
|structure_attribute 
|field_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
|volatile_attribute
|weak_attribute 
;

bind_routine_declaration: K_BIND K_ROUTINE bind_routine_item_list ';'  { $$ = 0; }
| K_GLOBAL K_BIND K_ROUTINE bind_routine_item_list { $$ = 0; }
;

bind_routine_item_list: bind_routine_item_list ',' bind_routine_item 
| bind_routine_item 
;

bind_routine_name:
T_NAME
;

expression_test:
expression
;

routine_name_value:
expression_test
;

maybe_bind_routine_attribute_list: { $$ = 0; }
| ':' bind_routine_attribute_list { $$ = $2; }
;

bind_routine_item_rest:
routine_name_value maybe_bind_routine_attribute_list 
;

bind_routine_item: bind_routine_name '@' bind_routine_item_rest
{
  /*
|
bind_routine_name '=' routine_name_value */
  }
;

bind_routine_attribute_list:
bind_routine_attribute_list bind_routine_attribute 
|bind_routine_attribute 
;

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
;

simple_macro_definition: 
T_NAME '(' tname_list ')'  '='
{
  macromode=1;
}
macro_body '%' 
{ 
  add_macro(IDENTIFIER_POINTER($1),SIMP_MACRO,$3,0,$7);
}
| T_NAME  '=' { macromode=1; } macro_body '%'
{ 
  add_macro(IDENTIFIER_POINTER($1),SIMP_MACRO,0,0,$4);
}
;
conditional_macro_definition:
T_NAME '(' tname_list ')'  '[' ']' '=' { macromode=1; } macro_body '%' 
{ 
  add_macro(IDENTIFIER_POINTER($1),COND_MACRO,$3,0,$9);
}
|
T_NAME '[' ']' '=' { macromode=1; } macro_body '%'
{ 
  add_macro(IDENTIFIER_POINTER($1),COND_MACRO,0,0,$6);
}
;

iterative_macro_definition:
T_NAME '(' tname_list ')' '[' tname_list ']' '=' { macromode=1; } macro_body '%' 
{ 
  add_macro(IDENTIFIER_POINTER($1),ITER_MACRO,$3,$6,$10);
}
|
T_NAME '[' tname_list ']' '=' { macromode=1; } macro_body '%'
{ 
  add_macro(IDENTIFIER_POINTER($1),ITER_MACRO,0,$3,$7);
}
;

default_actual: { $$=0; }
| lexeme_list 
;

macro_body: { $$=0; }
| { tnamemode=0; macromode=1; } lexeme_list { tnamemode=0; macromode=0; $$=$2; }
;
/*| '<' { anymode=1; } lexeme_list { anymode=0; } '>' { $$=$3; }*/


lexeme_list: lexeme_list LEXEME { 
  //$$ = strcat(xstrdup($1), xstrdup($2));
  $$ = chainon ($1, $2);
} 
|LEXEME
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
T_NAME '('
{
  push_scope ();
}
keyword_pair_list
{
  bli_pop_scope ();
}
 ')' '=' { macromode=1; } macro_body '%' 
{ 
  add_macro(IDENTIFIER_POINTER($1),KEYW_MACRO,$4,0,$9);
}
|
T_NAME '=' { macromode=1; } macro_body '%' 
{ 
  // not according to grammar, but starlet.req got it like this at least once
  add_macro(IDENTIFIER_POINTER($1),KEYW_MACRO,0,0,$4);
}
;

keyword_pair_list:
keyword_pair_list ',' keyword_pair 
{
  $$ = chainon($1, $3);
}
|
keyword_pair 
;

keyword_pair:
keyword_formal_name '=' 
{
  extern int one_lexeme;
  extern int macromode;
  macromode=3;
  one_lexeme = 1;
}
default_actual 
{
  $$ = build_tree_list($4, $1);
  //$$ = build_nt(KEYWORD_PAIR, $1, $4);
}
|
keyword_formal_name
{
  $$ = build_tree_list(0, $1);
  //$$ = build_nt(KEYWORD_PAIR, $1, 0);
}
;

macro_formal_name_list:
macro_formal_name_list ',' macro_formal_name
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
|
macro_formal_name
{
  $$ = build_tree_list (NULL_TREE, $1);
}
;

macro_formal_name:
T_NAME
;

linkage_definition_list: linkage_definition_list ',' linkage_definition 
|linkage_definition 
;
linkage_definition:
linkage_name '='
{
  undefmode = 1;
}
linkage_type
{
  undefmode = 0;
  add_linkage(IDENTIFIER_POINTER($1), $4);
}
linkage_type_stuff maybe_linkage_option
;

linkage_type:
U_CALL
|
U_JSB
|
U_INTERRUPT
;

linkage_type_stuff:
|
'(' maybe_input_parameter_location_list maybe_output_parameter_location_list ')'
;

maybe_input_parameter_location_list:
|
input_parameter_location_list
;

maybe_output_parameter_location_list:
|
';' output_parameter_location_list
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
K_REGISTER '=' T_DIGITS { $$ = 0; }
;

maybe_linkage_option:
|
':'
{ 
  undefmode=1;
}
linkage_option_list
{
  undefmode=0;
}
;

linkage_option_list:
linkage_option_list linkage_option
|
linkage_option
;

linkage_option:
K_GLOBAL '(' global_register_segment_list ')'
|
U_PRESERVE '(' register_number_list ')'
{
  // another one not according to specs
}
|
U_NOPRESERVE '(' register_number_list ')'
|
U_NOTUSED '(' register_number_list ')'
;

global_register_segment_list:
global_register_segment_list ',' global_register_segment
|
global_register_segment
;

global_register_segment:
global_register_name '=' register_number
;

global_register_name:
T_NAME
;

register_number_list:
register_number_list ',' register_number
|
register_number
;

register_number: ctce;

psect_item_list: psect_item_list ',' psect_item 
|psect_item 
;

psect_attribute_list:
psect_attribute_list ',' psect_attribute 
{
  $$ = tree_cons (NULL_TREE, $3, $1);
}
|
psect_attribute
{
  $$ = tree_cons (NULL_TREE, $1, 0);
}
;

psect_item:
storage_class '=' T_NAME
{ 
  undefmode=1;
}
'(' psect_attribute_list ')'  
{ 
  undefmode=0;
  if ($1==K_OWN) {
#if 1
    tree val1 = tree_cons(0, build_string(IDENTIFIER_LENGTH($3),IDENTIFIER_POINTER($3)), 0);
    tree val2 = tree_cons(get_identifier("section"), val1, 0);
    default_own_section=val2;
#else
    default_own_section=$3;
    default_own_align=get_alignment($6);
#endif
  }
  if ($1==K_GLOBAL) {
#if 1
    tree val1 = tree_cons(0, build_string(IDENTIFIER_LENGTH($3),IDENTIFIER_POINTER($3)), 0);
    tree val2 = tree_cons(get_identifier("section"), val1, 0);
    default_global_section=val2;
    default_global_align=get_alignment($6);
#else
    default_global_section=$3;
#endif
  }
  $$ = $3;
}
|
storage_class '=' T_NAME
{
  if ($1==K_OWN && 0==strcmp("$own",IDENTIFIER_POINTER($3))) {
    default_own_section=0;
    default_own_align=0;
  }
  if ($1==K_GLOBAL && 0==strcmp("$global$",IDENTIFIER_POINTER($3)))
    default_global_section=0;
    default_global_align=0;
  $$ = $3;
}
;

storage_class:
K_OWN
{ 
  $$ = K_OWN;
} 
|
K_GLOBAL 
{ 
  $$ = K_GLOBAL;
}
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
|addressing_mode_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
;

external_name_attribute:
K_EXTERNAL_NAME '(' T_STRING ')'
{
  $$ = build_nt(EXTERNAL_NAME_ATTR, $3);
}
;

/**** 5.0 LEXICAL PROCESSING FACILITIES *****************************/

compile_time_item_list: compile_time_item_list ',' compile_time_item
| compile_time_item
;

compile_time_item:
compile_time_name '=' compile_time_value
{
  if (!quiet_flag) printf("k_com %s %x\n",IDENTIFIER_POINTER($1),$3);
  $$ = set_cti($1, $3);
}
;

compile_time_name:
T_NAME
;

compile_time_value:
compile_time_constant_expression
;

/**** 7.0 MACHINE SPECIFIC NAMES ************************************/

builtin_name: T_NAME  
| machine_specific_function  
| linkage_function 
;

machine_specific_function: T_NAME
;

linkage_function: /* was: T_NAME */
B_NULLPARAMETER
;

linkage_function_name: /* was: T_NAME */
B_NULLPARAMETER
;

pushlevel:  /* empty 7 */
{
  if (cfun==0)
	 goto skip_pushlevel;
  tree scope;
#if 0
  push_scope();
#endif
#if 0
  // check gone
  clear_last_expr ();
  scope = add_scope_stmt (/*begin_p=*/1, /*partial_p=*/0);
#endif
  //  if (TREE_TYPE(TREE_PURPOSE(scope))==0) TREE_TYPE(TREE_PURPOSE(scope))=long_integer_type_node; // 64-bit
 skip_pushlevel:
}
;

poplevel:  /* empty 8 */
{ 
  if (cfun==0)
	 goto skip_poplevel;
#if 0
  // check
  $$ = add_scope_stmt (/*begin_p=*/0, /*partial_p=*/0);
#endif
 skip_poplevel:
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
  if (yyrec) {
	 yyrec=0;
	 return;
  }
  if (s) error("\n\n%s\n",s); 
  error(input_location, "Nu b;lev det fel\n");
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
            assert(0 && "lll; parser trace cannot be enabled - blicprs.y not compiled with YYDEBUG set");
#endif
      fprintf(stderr, "lll; parser trace cannot be enabled - blicprs.y not compiled with YYDEBUG set");
    }
#endif

  bli_init_reswords();

}

void yy2error (const char *s)
{
  if (s) error("\n\n%s\n",s); 
  error(input_location, "Nu b;lev det fel %d\n");
}

void
maybe_apply_pragma_weak(int a) {
}

/*
tree
maybe_apply_renaming_pragma(tree decl, tree asmname) {
  return asmname;
}
*/

static void init_reswords ();

void
c_parse_init(void) {
  init_reswords ();

parse_init();
}

int
cpp_handle_option (cpp_reader * pfile, int argc, char ** argv)
{
}

void
gen_aux_info_record (tree fndecl, int is_definition, int is_implicit, int is_prototyped)
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
init_c_lex ()
{
}

/*
void
init_pragma ()
{
}
*/

void
cpp_add_dependency_target (cpp_reader *pfile, const char *target, int quote)
{
}

void
cpp_preprocess_file (cpp_reader *pfile, const char *in_fname, FILE *out_stream)
{
}

tree
lookup_interface (tree arg ATTRIBUTE_UNUSED)
{
  return 0;
}

tree
is_class_name (tree arg ATTRIBUTE_UNUSED)
{
  return 0;
}

/*
tree
objc_is_object_ptr (tree arg ATTRIBUTE_UNUSED)
{
  return 0;
}
*/

tree
lookup_objc_ivar (tree arg ATTRIBUTE_UNUSED)
{
  return 0;
}

/*
void
objc_check_decl (tree decl ATTRIBUTE_UNUSED)
{
}
*/

int
objc_comptypes (tree lhs ATTRIBUTE_UNUSED, tree rhs ATTRIBUTE_UNUSED,
                int reflexive ATTRIBUTE_UNUSED)
{ 
  return -1;
}

/*
tree
objc_message_selector (void)
{ 
  return 0;
}
*/

#if 0
/* Add PATH to the include chain CHAIN. PATH must be malloc-ed and
   NUL-terminated.  */
void
add_path (char *path, int chain, int cxx_aware)
{
  struct cpp_dir *p;

#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
  /* Convert all backslashes to slashes.  The native CRT stat()
     function does not recognise a directory that ends in a backslash
     (unless it is a drive root dir, such "c:\").  Forward slashes,
     trailing or otherwise, cause no problems for stat().  */
  char* c;
  for (c = path; *c; c++)
    if (*c == '//') *c = '/';
#endif

  p = xmalloc (sizeof (struct cpp_dir));
  p->next = NULL;
  p->name = path;
  if (chain == SYSTEM || chain == AFTER)
    p->sysp = 1 + !cxx_aware;
  else
    p->sysp = 0;

  if (tails[chain])
    tails[chain]->next = p;
  else
    heads[chain] = p;
  tails[chain] = p;
}

/* Used by C front ends, which really should move to using
   cpp_token_as_text.  */
const char *
cpp_type2name (enum cpp_ttype type)
{
  return (const char *) token_spellings[type].name;
}
#endif

void
add_struct(struct structure ** s,tree elem) {
  struct structure * t=xmalloc(sizeof(*t));
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

/* The reserved keyword table.  */
struct resword
{
  const char *word;
  ENUM_BITFIELD(rid) rid : 16;
  unsigned int disable   : 16;
};

//extern int flag_no_asm;

/* Disable mask.  Keywords are disabled if (reswords[i].disable & mask) is
   _true_.  */
#define D_TRAD  0x01    /* not in traditional C */
#define D_C89   0x02    /* not in C89 */
#define D_EXT   0x04    /* GCC extension */
#define D_EXT89 0x08    /* GCC extension incorporated in C99 */
#define D_OBJC  0x10    /* Objective C only */

static const struct resword reswords[] =
{
#if 0
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
#endif
  { "int",              RID_INT,        0 },
  { "long",             RID_LONG,       0 },
#if 0
  { "register",         RID_REGISTER,   0 },
  { "restrict",         RID_RESTRICT,   D_TRAD|D_C89 },
  { "return",           RID_RETURN,     0 },
#endif
  { "short",            RID_SHORT,      0 },
#if 0
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
#endif
};
#define N_reswords (sizeof reswords / sizeof (struct resword))

static void
init_reswords ()
{
  unsigned int i;
  tree id;
  int mask = (flag_isoc99 ? 0 : D_C89)
              | (flag_no_asm ? (flag_isoc99 ? D_EXT : D_EXT|D_EXT89) : 0);

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
      // TODO C_RID_CODE (id) = reswords[i].rid;
      C_IS_RESERVED_WORD (id) = 1;
      ridpointers [(int) reswords[i].rid] = id;
    }
}

static tree
mydeclares (tree t) {
  tree tmp;
  if (!t) return 0;
  for(tmp=t;tmp;tmp=TREE_CHAIN(tmp)) {
    struct c_parm * decl = build_parm_decl (TREE_VALUE(tmp), long_integer_type_node); // 64-bit
    push_parm_decl(decl, 0);
    bli_get_parm_info(1, 0);
  }
  return 0;
}
tree
my_copy  (tree * tp, int * walk_subtrees, void * data) 
{
#if 0
  tree b=TREE_OPERAND (*tp, 1);
  if (TREE_CODE(*tp) == NON_LVALUE_EXPR && b && TREE_CODE(b) == BIT_FIELD_REF) {
    walk_tree (&TREE_OPERAND (*tp, 1), my_copy, 0, 0);
  }
#endif
  copy_tree_r (tp, walk_subtrees, NULL);

  /* Keep iterating.  */
  return NULL_TREE;

}

tree
my_copy_tree (tree node)
{
  tree ret = node;
  walk_tree (&ret, my_copy, 0, 0);
  //  ret = copy_node(node);
  //  if (TREE_CHAIN(node))
  //    TREE_CHAIN(ret)=my_copy_tree(TREE_CHAIN(node));
  return ret;
}

tree
my_substitute_fn  (tree * tp, int * walk_subtrees, void * data) 
{
  long *l=data;
  tree old=l[0];
  tree neww=l[1];
  //  copy_tree_r (tp, walk_subtrees, NULL);
  int i;
  if (DECL_P(*tp)) {
	 if (l[1]==-1)
		goto other_minus;
	 for(/*old=TREE_PURPOSE(old)*/;old && neww;old=TREE_CHAIN(old),neww=TREE_CHAIN(neww)) {
      if (DECL_NAME(*tp)==DECL_NAME(old)) {
		  *tp=TREE_VALUE(neww);
		  STRIP_NOPS(*tp); // check
		  goto subst_out;
      }
    }
	 return NULL_TREE;
  other_minus:
	 for(/*old=TREE_PURPOSE(old)*/;old;old=TREE_CHAIN(old)) {
      if (DECL_NAME(*tp)==DECL_NAME(old)) {
	// TODO if (TREE_CODE(((tree)DECL_LANG_SPECIFIC(old)))==INTEGER_CST)
	// TODO    *tp=copy_node(((tree)DECL_LANG_SPECIFIC(old)));
#if 0
		  // check
			 *tp=copy_node(DECL_RESULT(old));
#endif
		  goto subst_out;
      }
    }
	 return NULL_TREE;
  }
 subst_out:
  /* Keep iterating.  */
  return NULL_TREE;

}

void
my_substitute (tree mytree, tree old, tree neww)
{
  long l[2];
  l[0]=old;
  l[1]=neww;
  walk_tree (&mytree, my_substitute_fn, l, 0);
}

tree
my_do_parm(tree t)
{
  if (t==0) return 0;
  return tree_cons(0, build_decl(input_location, STRUCTURE_DECL, t, 0)  , my_do_parm(TREE_CHAIN(t)));
}

tree
my_fold_fn  (tree * tp, int * walk_subtrees, void * data) 
{
  *tp=fold(*tp);

  /* Keep iterating.  */
  return NULL_TREE;

}

void
my_fold (tree mytree)
{
  walk_tree (&mytree, my_fold_fn, 0, 0);
}

void
my_fold_p (tree * mytree)
{
  walk_tree (mytree, my_fold_fn, 0, 0);
}

tree
first_to_last(tree mytree)
{
  tree first=mytree;
  tree second=TREE_CHAIN(first);
  TREE_CHAIN(first)=0;
  chainon(tree_last(second),first);
  return second;
}

tree
parm_first_to_last(tree mytree)
{
  tree first=mytree;
  TREE_PURPOSE(first)=first_to_last(TREE_PURPOSE(first));
  return first;
}

tree
parm_first_to_last_new(struct c_arg_info * myargs)
{
  tree first=myargs->parms;
  myargs->parms = first_to_last(first);
  return first;
}

void
add_macro(char * name, int type, tree param, tree param2, tree body) {
  tree i = get_identifier(name);
  tree t = build_nt(MACRO_DEF, type, param, param2, body);
  bli_pushtag(input_location, i, t);
}

void *
find_macro(struct mymacro * s,char * name) {
  tree id=maybe_get_identifier(name);

  if (id==0)
    return 0;

  tree tag2 = get_label_binding (id);

  if (tag2==0)
    return 0;

  tree tag = tag2;

  if (tag==0)
    return 0;

  if (tag==0 || TREE_CODE (tag) != MACRO_DEF)
    return 0;

  tree t = tag;
  struct mymacro * m=xmalloc(sizeof(struct mymacro));
  m->type=TREE_OPERAND(t,0);
  m->param=TREE_OPERAND(t,1);
  m->param2=TREE_OPERAND(t,2);
  m->body=TREE_OPERAND(t,3);
  return m;
}

int
make_macro_string(struct dsc_dollar_descriptor * dsc, struct mymacro * m, tree r)
{
  extern int cond_iter_macro_count;
  struct dsc_dollar_descriptor s;
  s.dsc_dollar_a_pointer=0;
  s.dsc_dollar_w_length=0;
  tree b = m->body;
  tree p = m->param;
  tree t;
  int is_quote=0;
  if (m->type!=SIMP_MACRO && m->type!=KEYW_MACRO)
    goto other_macro;
  for(t=b;t;t=TREE_CHAIN(t)) {
    tree old,neww;
    struct dsc_dollar_descriptor l;
    l.dsc_dollar_a_pointer = TREE_STRING_POINTER(t);
    l.dsc_dollar_w_length = TREE_STRING_LENGTH(t);
    if (TREE_CODE(t)==IDENTIFIER_NODE) {
      l.dsc_dollar_a_pointer=IDENTIFIER_POINTER(t); // workaround in case %remaining IDENTIFIER
      l.dsc_dollar_w_length=IDENTIFIER_LENGTH(t);
    }
    // check. not quite sure about doing quoting here
    if (is_quote) {
      is_quote=0;
      goto subst_out;
    }
    is_quote=(0==strcasecmp(l.dsc_dollar_a_pointer,"%quote"));
    if (is_quote)
      continue;
    if (m->type==KEYW_MACRO)
      goto keyw_macro;
    for(old=p,neww=r;old && neww;old=TREE_CHAIN(old),neww=TREE_CHAIN(neww)) {
      if (0==strcmp(l.dsc_dollar_a_pointer,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
	l.dsc_dollar_a_pointer=TREE_STRING_POINTER(neww);
	l.dsc_dollar_w_length=TREE_STRING_LENGTH(neww);
	goto subst_out;
      }
    }
    for(;old;old=TREE_CHAIN(old)) {
      if (0==strcmp(l.dsc_dollar_a_pointer,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
	l.dsc_dollar_a_pointer=0;
	l.dsc_dollar_w_length=0;
	goto subst_out;
      }
    }
    goto subst_out;
  keyw_macro:
    for(neww=r;neww;neww=TREE_CHAIN(neww)) {
      //fprintf(stderr,"IDD3 %x %x %x\n",l,TREE_TYPE(neww),TREE_STRING_POINTER(neww));
      if (0==strcmp(l.dsc_dollar_a_pointer,TREE_STRING_POINTER(neww))) {
	if (TREE_TYPE(neww)) {
	  l.dsc_dollar_a_pointer=TREE_STRING_POINTER(TREE_TYPE(neww));
	  l.dsc_dollar_w_length=TREE_STRING_LENGTH(TREE_TYPE(neww));
	}
	goto subst_out;
      }
    }
    for(old=p;old;old=TREE_CHAIN(old)) {
      //printf("IDD4 %x %x %x\n",l,TREE_VALUE(old),TREE_PURPOSE(old));
      if (0==strcmp(l.dsc_dollar_a_pointer,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
	if (TREE_PURPOSE(old)) {
	  l.dsc_dollar_a_pointer=TREE_STRING_POINTER(TREE_PURPOSE(old));
	  l.dsc_dollar_w_length=TREE_STRING_LENGTH(TREE_PURPOSE(old));
	  goto subst_out;
	} else {
	  l.dsc_dollar_a_pointer=" ";
	  l.dsc_dollar_w_length=1;
	  goto subst_out;
	}
      }
    }
  subst_out:
    my_strcat_gen(&s,&s,&l,1);
  }
  
  if (yydebug) inform (input_location, "\n%%BLS-I-NOTHING line macro expanded to %s\n", s.dsc_dollar_a_pointer);

  dsc->dsc_dollar_a_pointer=s.dsc_dollar_a_pointer;
  dsc->dsc_dollar_w_length=s.dsc_dollar_w_length;
  if  (dsc->dsc_dollar_w_length>longest_macro)
    longest_macro = dsc->dsc_dollar_w_length;
  return 1;
 other_macro:
  tree tmp=p;
  if (m->type!=COND_MACRO) {
  cond_iter_macro_count=0;
  tree iter=m->param2;
  tree neww=r;
  for(;neww && tmp;neww=TREE_CHAIN(neww),tmp=TREE_CHAIN(tmp)) ;
  extern tree remaining;
  tmp=iter;
  for(;remaining && tmp;remaining=TREE_CHAIN(remaining),tmp=TREE_CHAIN(tmp)) ;
  while (neww) {
	 tree par=iter;
	 //printf("neww %x\n",neww);
	 tree t;
	 for(t=b;t;t=TREE_CHAIN(t)) {
		tree new2=neww;
		tree par2=par;
		struct dsc_dollar_descriptor l;
		l.dsc_dollar_a_pointer = TREE_STRING_POINTER(t);
		l.dsc_dollar_w_length = TREE_STRING_LENGTH(t);
		if (0==strcmp(l.dsc_dollar_a_pointer,"%remaining")) {
		  print_remain(&l,remaining);
		  if (l.dsc_dollar_a_pointer==0)
		    continue;
		  goto subst_out2;
		}
		if (0==strcmp(l.dsc_dollar_a_pointer,"%count")) {
		  char loc[10];
		  sprintf(loc,"%d",cond_iter_macro_count);
		  l.dsc_dollar_a_pointer=xstrdup(loc); // leak
		  l.dsc_dollar_w_length=strlen(l.dsc_dollar_a_pointer);
		  goto subst_out2;
		}
		for(;new2 && par2;new2=TREE_CHAIN(new2),par2=TREE_CHAIN(par2)) {
#if 0
		  // check. not quite sure about doing quoting here
		  if (is_quote) {
			 is_quote=0;
			 goto subst_out2;
		  }
		  is_quote=(0==strcasecmp(l.dsc_dollar_a_pointer,"%quote"));
		  if (is_quote)
			 continue;
#endif
		  if (0==strcmp(l.dsc_dollar_a_pointer,IDENTIFIER_POINTER(TREE_VALUE(par2)))) {
			 l.dsc_dollar_a_pointer=TREE_STRING_POINTER(new2);
			 l.dsc_dollar_w_length=TREE_STRING_LENGTH(new2);
			 goto subst_out2;
		  }
		}
		tree old3, new3;
		for(old3=p,new3=r;old3 && new3;old3=TREE_CHAIN(old3),new3=TREE_CHAIN(new3)) {
		  if (0==strcmp(l.dsc_dollar_a_pointer,IDENTIFIER_POINTER(TREE_VALUE(old3)))) {
			 //fprintf(stderr,"CMP3 %s %s %s %x %x\n",l,IDENTIFIER_POINTER(TREE_VALUE(old3)),TREE_STRING_POINTER(new3),TREE_STRING_POINTER(new3),new3);
			 l.dsc_dollar_a_pointer=TREE_STRING_POINTER(new3);
			 l.dsc_dollar_w_length=TREE_STRING_LENGTH(new3);
			 goto subst_out2;
		  }
		}
	 subst_out2:
		my_strcat_gen(&s, &s, &l, 1);
	 } // end outer for
	 for(tmp=iter;neww && tmp;neww=TREE_CHAIN(neww),tmp=TREE_CHAIN(tmp)) ;
	 for(tmp=iter;remaining && tmp;remaining=TREE_CHAIN(remaining),tmp=TREE_CHAIN(tmp)) ;
	 struct dsc_dollar_descriptor * dsc = &commadsc;
	 if (default_punct==';')
	   dsc = &semidsc;
	 if (neww)
		my_strcat_gen(&s, &s, dsc, 0); // remember to fix punctuation later
	 cond_iter_macro_count++;
  } // end while
  if (!quiet_flag) printf("ITER %x\n",s.dsc_dollar_a_pointer);
  if (!quiet_flag) printf("ITER %s\n",s.dsc_dollar_a_pointer);
  cond_iter_macro_count=0; // reset it here because cannot in cond itself?
  dsc->dsc_dollar_a_pointer=s.dsc_dollar_a_pointer;
  dsc->dsc_dollar_w_length=s.dsc_dollar_w_length;
  if  (dsc->dsc_dollar_w_length>longest_macro)
    longest_macro = dsc->dsc_dollar_w_length;
  return 1;
  } else { // cond_macro == m->type
  // this seems to be done just like a simple macro?
  // except for using cond_iter_macro_count? but might use it wrongly?
  // and 16.3.3.3 1 b and c: too few actuals and 0 eql actuals eql formals
  {}
  extern int cond_iter_macro_count;
  int actual_cnt;
  int formal_cnt;
  for (actual_cnt=0, tmp=r; tmp; actual_cnt++, tmp=TREE_CHAIN(tmp)) ;
  for (formal_cnt=0, tmp=m->param; tmp; formal_cnt++, tmp=TREE_CHAIN(tmp)) ;
#if 1
  if (actual_cnt < formal_cnt) // 16.3.3.3 1 b
    return 0;
  if (actual_cnt==0 && formal_cnt==0) // 16.3.3.3 1 c
    return 0;
#endif
  //cond_iter_macro_count=0; // cannot do this here?
  for(t=b;t;t=TREE_CHAIN(t)) {
    tree old,neww;
    struct dsc_dollar_descriptor l;
    l.dsc_dollar_a_pointer = TREE_STRING_POINTER(t);
    l.dsc_dollar_w_length = TREE_STRING_LENGTH(t);
    if (TREE_CODE(t)==IDENTIFIER_NODE) {
      l.dsc_dollar_a_pointer=IDENTIFIER_POINTER(t); // workaround in case %remaining IDENTIFIER
      l.dsc_dollar_w_length=IDENTIFIER_LENGTH(t);
    }
    // check. not quite sure about doing quoting here
    if (is_quote) {
      is_quote=0;
      goto subst_out3;
    }
    is_quote=(0==strcasecmp(l.dsc_dollar_a_pointer,"%quote"));
    if (is_quote)
      continue;
    for(old=p,neww=r;old && neww;old=TREE_CHAIN(old),neww=TREE_CHAIN(neww)) {
      if (0==strcmp(l.dsc_dollar_a_pointer,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
	l.dsc_dollar_a_pointer=TREE_STRING_POINTER(neww);
	l.dsc_dollar_w_length=TREE_STRING_LENGTH(neww);
	goto subst_out3;
      }
    }
    goto subst_out3;
  subst_out3:
    my_strcat_gen(&s, &s, &l, 1);
  }
  }
  
  if (yydebug) inform (input_location, "\n%%BLS-I-NOTHING %x cond line macro expanded to %s\n",s.dsc_dollar_a_pointer);

  cond_iter_macro_count++;

  dsc->dsc_dollar_a_pointer=s.dsc_dollar_a_pointer;
  dsc->dsc_dollar_w_length=s.dsc_dollar_w_length;
  if  (dsc->dsc_dollar_w_length>longest_macro)
    longest_macro = dsc->dsc_dollar_w_length;
  return 1;
}

int
print_remain(struct dsc_dollar_descriptor * dsc, tree r)
{
  struct dsc_dollar_descriptor s;
  s.dsc_dollar_a_pointer=0;
  s.dsc_dollar_w_length=0;
  tree t;
  for(t=r;t;t=TREE_CHAIN(t)) {
    tree old,neww;
    struct dsc_dollar_descriptor l;
    l.dsc_dollar_a_pointer = TREE_STRING_POINTER(t);
    l.dsc_dollar_w_length = TREE_STRING_LENGTH(t);
    my_strcat_gen(&s,&s,&l,0);
    if (TREE_CHAIN(t))
      my_strcat_gen(&s,&s,&commadsc,0);
  }
  
  //if (yydebug) inform ("\n%%BLS-I-NOTHING %x line macro expanded to %s\n",input_location.line,s);

  dsc->dsc_dollar_a_pointer=s.dsc_dollar_a_pointer;
  dsc->dsc_dollar_w_length=s.dsc_dollar_w_length;
  if (!quiet_flag) printf("pr %x %s\n",s.dsc_dollar_w_length,s.dsc_dollar_a_pointer);
  return 1;
}

char *
add_counted_string(tree t, int n)
{
    char * ss=xmalloc(IDENTIFIER_LENGTH(t)+1);
    strcpy(1+ss,IDENTIFIER_POINTER(t));
    ss[IDENTIFIER_LENGTH(t)+1]=0;
	 ss[0]=n;
    return ss;
}

char *
add_underscore(tree t, int n)
{
    char * ss=xmalloc(IDENTIFIER_LENGTH(t)+n+1);
    strcpy(ss,IDENTIFIER_POINTER(t));
    ss[IDENTIFIER_LENGTH(t)+n]=0;
    while (n--)
      ss[IDENTIFIER_LENGTH(t)+n]='_';
    return ss;
}

char *
add_percent(tree t, int n)
{
    char * ss=xmalloc(IDENTIFIER_LENGTH(t)+n);
    strcpy(ss,IDENTIFIER_POINTER(t));
    ss[IDENTIFIER_LENGTH(t)+n]=0;
    while (n--)
      ss[IDENTIFIER_LENGTH(t)+n]='_';
    return ss;
}

/* This is not the ideal place to put these, but we have to get them out
   of c-lex.c because cp/lex.c has its own versions.  */

/* Parse the file.  */
void
c_parse_file (void)
{
  yyparse ();
  /* In case there were missing closebraces, get us back to the global
     binding level.  */
  while (! global_bindings_p ())
    bli_pop_scope();
  /* __FUNCTION__ is defined at file scope ("").  This
     call may not be necessary as my tests indicate it
     still works without it.  */
  finish_fname_decls ();

  if (malloced_yyss)
    {
      free (malloced_yyss);
      free (malloced_yyvs);
      malloced_yyss = 0;
    }
}

set_cti(tree id, tree val) {
  tree newid=id;//add_percent(id,1);
#if 0
  if (TREE_CODE(val)!=INTEGER_CST) {
    error("int\n");
  }
#endif
  if (TREE_CODE(val)==IDENTIFIER_NODE) {
    // workaround to catch predefined literals
    //printf("SETCTI %s\n",IDENTIFIER_POINTER(val));
    char myline[255];
    memset (myline, 0, 255);
    sprintf(myline, "(1 * %s)",IDENTIFIER_POINTER(val));
    val=parse_this(myline);
    val=TREE_VALUE(val);
  }
  if (TREE_CODE(val)==NON_LVALUE_EXPR)
    val=TREE_OPERAND(val, 0);
  TREE_TYPE(newid)=val; // bad hack?
  return newid;
}

int find_skip_mode_not(tree t) {
  switch (TREE_CODE(t)) {
  case IDENTIFIER_NODE:
	 if (strcmp("0",IDENTIFIER_POINTER(t)))
		return 1;
	 break;
  default:
	 break;
  }
  return 0;
}

tree mylast() {
  return last_expr;
}

/* A single C token after string literal concatenation and conversion
   of preprocessing tokens to tokens.  */
typedef struct c_token GTY (())
{
#if 0
  /* The kind of token.  */
  ENUM_BITFIELD (cpp_ttype) type : 8;
  /* If this token is a CPP_NAME, this value indicates whether also
     declared as some kind of type.  Otherwise, it is C_ID_NONE.  */
  ENUM_BITFIELD (c_id_kind) id_kind : 8;
  /* If this token is a keyword, this value indicates which keyword.
     Otherwise, this value is RID_MAX.  */
  ENUM_BITFIELD (rid) keyword : 8;
  /* True if this token is from a system header.  */
  BOOL_BITFIELD in_system_header : 1;
  /* The value associated with this token, if any.  */
#endif
  tree value;
  /* The location at which this token was found.  */
  location_t location;
} c_token;

/* A parser structure recording information about the state and
   context of parsing.  Includes lexer information with up to two
   tokens of look-ahead; more are not needed for C.  */
typedef struct c_parser GTY(())
{
  /* The look-ahead tokens.  */
  c_token tokens[2];
  /* How many look-ahead tokens are available (0, 1 or 2).  */
  short tokens_avail;
  /* True if a syntax error is being recovered from; false otherwise.
     c_parser_error sets this flag.  It should clear this flag when
     enough tokens have been consumed to recover from the error.  */
  BOOL_BITFIELD error : 1;
} c_parser;

/* The actual parser and external interface.  ??? Does this need to be
   garbage-collected?  */

static GTY (()) c_parser *the_parser;

//#include "gt-c-parser.h"

const struct ggc_root_tab gt_ggc_r_gt_c_parse_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_pragma_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_c_parser_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_pch_rs_gt_c_family_c_cppbuiltin_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_family_c_cppbuiltin_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_family_c_pragma_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_family_c_format_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_family_c_cppbuiltin_[] = {
  LAST_GGC_ROOT_TAB
};

int check_little_endian() {
  short sh = 0x1234;
  char * c = (void *) &sh;
  return (*c==0x34);
}

char * bliss_builtin = "MACRO %BLISS16[] = % , %BLISS36[] = % , %BLISS32[] = %REMAINING % ; ";

void get_builtin(void) {
  // change from SIMP_MACRO later
  add_macro("%bliss16",SIMP_MACRO,0,0,0);
  add_macro("%bliss36",SIMP_MACRO,0,0,0);
  add_macro("%bliss32",SIMP_MACRO,0,0,get_identifier("%remaining"));

  // 64-bit
#if 0 // TODO
#ifdef __x86_64__
  // TODO
  predef_literal("%bpval",64);
  predef_literal("%bpunit",8);
  predef_literal("%bpaddr",64);
  predef_literal("%upval",8);
#else
  predef_literal("%bpval",32);
  predef_literal("%bpunit",8);
  predef_literal("%bpaddr",32);
  predef_literal("%upval",4);
#endif
#endif
  
#if 0
  do_builtin=1;

  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(bliss_builtin));

  yyparse();

  do_builtin=0;
#endif
}

// See language reference 11.10

// 64-bit
#ifdef __x86_64__
char * bliss_builtin_struct_1 = "structure vector[i; n, unit=8, ext=0] = [n*unit] (vector + i*unit)<0,8*unit,ext>;";
#else
char * bliss_builtin_struct_1 = "structure vector[i; n, unit=4, ext=0] = [n*unit] (vector + i*unit)<0,8*unit,ext>;";
#endif

//char * bliss_builtin_struct_1 = "structure vector[i; n, unit=%upval, ext=0] = [n*unit] (vector+i*unit)<0,%bpunit*unit,ext>;";

char * bliss_builtin_struct_2 = "structure bitvector[i; n] = [(n+7)/8] bitvector<i,1>;";

#ifdef __x86_64__
char * bliss_builtin_struct_3 = "structure block[o, p, s, e; bs, unit=8] = [bs*unit] (block+o*unit)<p,s,e>;";

char * bliss_builtin_struct_4 = "structure blockvector[i, o, p, s, e; n, bs, unit=8] = [n*bs*unit] (blockvector+(i*bs+o)*unit)<p,s,e>;";
#else
char * bliss_builtin_struct_3 = "structure block[o, p, s, e; bs, unit=4] = [bs*unit] (block+o*unit)<p,s,e>;";

char * bliss_builtin_struct_4 = "structure blockvector[i, o, p, s, e; n, bs, unit=4] = [n*bs*unit] (blockvector+(i*bs+o)*unit)<p,s,e>;";
#endif

// was bblock. starlet lets $$block be block_byte
char * bliss_builtin_struct_5 = "structure block_byte [o, p, s, e; n] = [n] (block_byte + o) <p, s, e>;";

char * bliss_builtin_linkage_1 = "linkage bliss = jsb;";

char * bliss_builtin_linkage_2 = "linkage call = call;";

#ifdef __x86_64__
char * bliss_builtin_barrier = "own barrier;"; // temp workaround
#endif

void add_builtin(void) {
  parse_this(bliss_builtin_struct_1);
  parse_this(bliss_builtin_struct_2);
  parse_this(bliss_builtin_struct_3);
  parse_this(bliss_builtin_struct_4);
  parse_this(bliss_builtin_struct_5);
  parse_this(bliss_builtin_linkage_1);
  parse_this(bliss_builtin_linkage_2);
#ifdef __x86_64
  parse_this(bliss_builtin_barrier); // temp workaround
#endif

  if (check_little_endian()) // had to do some of my own extensions
    parse_this("compiletime $cpu_le = 1;");
  else
    parse_this("compiletime $cpu_le = 0;");
}

int
is_on_remaining(char * c, tree r)
{
  char * s=0;
  tree t;
  for(t=r;t;t=TREE_CHAIN(t)) {
    char * l = IDENTIFIER_POINTER(t);
	 if (0==strcmp(c,l))
		return 1;
  }
  return 0;
}

struct field_struct {
  struct field_struct * next;
  tree t;
  char * name;
};

struct field_struct * field_root=0;

register_field(char * s, tree t) {
  struct field_struct * f = xmalloc(sizeof(struct field_struct));
  f->name=xstrdup(s);
  f->t=t;
  f->next=field_root;
  field_root=f;
  fields++;
}

tree
find_field(char * s) {
  struct field_struct * t = field_root;
  for (;t;t=t->next,fielditers++) {
	 if (0==strcmp(t->name,s))
		return t->t;
  }
  return 0;
}

char *
print_tree(tree r)
{
  struct dsc_dollar_descriptor s;
  s.dsc_dollar_a_pointer=0;
  s.dsc_dollar_w_length=0;
  tree t;
  for(t=r;t;t=TREE_CHAIN(t)) {
    tree old,neww;
#if 0
    char * l = IDENTIFIER_POINTER(t);
#endif
    char l[256];
    sprintf(l,"%d",TREE_INT_CST_LOW(TREE_VALUE(t)));
    if (s.dsc_dollar_a_pointer==0) {
      s.dsc_dollar_a_pointer=xstrdup(l);
      s.dsc_dollar_w_length=strlen(s.dsc_dollar_a_pointer);
    } else {
      my_strcat(&s,s.dsc_dollar_w_length,s.dsc_dollar_a_pointer,strlen(l),l,0);  
      my_strcat(&s,s.dsc_dollar_w_length,s.dsc_dollar_a_pointer,1,",",0);
    }
    char * c=s.dsc_dollar_a_pointer;
    c[strlen(c)]=0;
  }
  
  //if (yydebug) inform ("\n%%BLS-I-NOTHING %x line macro expanded to %s\n",input_location.line,s);

  return s.dsc_dollar_a_pointer;
}

void
predef_literal(char * name, int value)
{
  tree d1 = get_identifier(name);
  tree d3 = build_int_cst (long_integer_type_node, value); // 64-bit
  // TODO
  build_enumerator(input_location, input_location, 0, d1, d3); // later?
  TREE_LANG_FLAG_0(d1)=1;
}

tree
find_tree_code(tree t, int c)
{
  for(;t;t=TREE_CHAIN(t))
	 if (TREE_CODE(TREE_VALUE(t))==c)
		return TREE_VALUE(t);
  return 0;
}

tree
find_init_attr(tree t)
{
  return find_tree_code(t, INIT_ATTR);
}

tree
find_structure_attr(tree t)
{
  return find_tree_code(t, STRUCTURE_STUFF);
}

tree
find_alloc_attr(tree t)
{
  return find_tree_code(t, ALLOC_ATTR);
}

tree
find_extension_attr(tree t)
{
  return find_tree_code(t, EXTENSION_ATTR);
}

tree
find_linkage_attr(tree t)
{
  return find_tree_code(t, LINKAGE_ATTR);
}

tree
find_novalue_attr(tree t)
{
  return find_tree_code(t, NOVALUE_ATTR);
}

tree
find_volatile_attr(tree t)
{
  return find_tree_code(t, VOLATILE_ATTR);
}

tree
find_external_name_attr(tree t)
{
  return find_tree_code(t, EXTERNAL_NAME_ATTR);
}

tree
find_alignment_attr(tree t)
{
  return find_tree_code(t, ALIGNMENT_ATTR);
}

tree
get_alignment(tree t)
{
  t = find_alignment_attr(t);
  if (t==0)
    return 0;
  t = TREE_OPERAND(t,0);
  int i = TREE_INT_CST_LOW(t);
  tree val1 = tree_cons(0, build_int_cst (long_integer_type_node, 1<<i), 0); // 64-bit
  tree val2 = tree_cons(get_identifier("__aligned__"), val1, 0);
  return val2;
}

void
set_external_name(tree decl, tree attr)
{
  tree t = find_external_name_attr(attr);
  if (t==0)
    return;
  tree str = TREE_OPERAND(t, 0);
  char c[255];
  memcpy(c, TREE_STRING_POINTER(str)+1, TREE_STRING_LENGTH(str)-2);
  c[TREE_STRING_LENGTH(str)-2]=0;
  tree name=get_identifier(c);
  SET_DECL_ASSEMBLER_NAME(decl, name);
}

int
unsigned_attr(tree t)
{
  t = find_extension_attr(t);
#if 0
  return !(t && TREE_OPERAND(t,0)==K_SIGNED);
#else
  return !(t && 0==strcmp("signed",IDENTIFIER_POINTER(TREE_OPERAND(t,0))));
#endif
}

tree
conv_unsign(tree t)
{
  if (t==long_integer_type_node) // 64-bit
    return long_unsigned_type_node;
  if (t==long_long_integer_type_node) // 64-bit
    return long_long_unsigned_type_node;
  if (t==integer_type_node)
    return unsigned_type_node;
  if (t==short_integer_type_node)
    return short_unsigned_type_node;
  if (t==char_type_node) //check
    return unsigned_char_type_node;
  return 0;
}

tree
sign_convert(tree t, tree attr) 
{
  if (!unsigned_attr(attr))
    return t;
  return conv_unsign(t);
}

tree
is_label(tree t)
{
  if (t==0)
    return t;
  tree c = get_label_binding(t);
  if (c==0)
    return c;
  // TODO tree decl = binding_decl(c);
  if (1 /* TODO TREE_CODE(decl)==LABEL_DECL*/)
  return 0; // TODOdecl;
  return 0;
}

tree
is_symbol(tree t)
{
  if (t==0)
    return t;
  return identifier_global_value(t);
}

tree
is_tag(tree t)
{
  if (t==0)
    return t;
  return my_lookup_tag(t);
}

tree
is_something(tree t)
{
  if (t==0)
    return t;
  return t ; // TODO identifier_global_value(t)||my_lookup_tag(t)||get_label_binding(t);
}

int
my_strcat(struct dsc_dollar_descriptor * dsc, int len1, const char * str1, int len2, const char * str2, int space)
{
  if (space)
	 space=1;
  char * str = xmalloc(len1+len2+1+space);
  memcpy(str,str1,len1);
  if (space)
	 str[len1]=32;
  memcpy(str+len1+space,str2,len2);
  str[len1+len2+space]=0;
  dsc->dsc_dollar_a_pointer=str;
  dsc->dsc_dollar_w_length=len1+len2+space;
  return 1;
}

int
my_strcat_gen(
	      struct dsc_dollar_descriptor * dsc,
	      struct dsc_dollar_descriptor * dsc1,
	      struct dsc_dollar_descriptor * dsc2,
	      int space)
{
  int len1=dsc1->dsc_dollar_w_length;
  int len2=dsc2->dsc_dollar_w_length;
  char * str1=dsc1->dsc_dollar_a_pointer;
  char * str2=dsc2->dsc_dollar_a_pointer;
  if (str1==0) {
    dsc->dsc_dollar_a_pointer=xmalloc(len2+1);
    memcpy(dsc->dsc_dollar_a_pointer, str2, len2);
    dsc->dsc_dollar_w_length=len2;
    return 1;
  }
  if (space)
	 space=1;
  char * str = xmalloc(len1+len2+1+space);
  memcpy(str,str1,len1);
  str1_memcpy+=len1;
  str1_memcpys++;
  if (space)
	 str[len1]=32;
  memcpy(str+len1+space,str2,len2);
  str[len1+len2+space]=0;
  free(str1);
  dsc->dsc_dollar_a_pointer=str;
  dsc->dsc_dollar_w_length=len1+len2+space;
  return 1;
}

tree
save_last_tree() {
  int t = tree_last(cur_stmt_list);
  tree tr = tree_last(cur_stmt_list);
  TREE_CHAIN(tr) = build_nt (EXPR_STMT, void_node);
  return t;
}

void
restore_last_tree(tree t)
{
  tree tr = tree_last(cur_stmt_list);
  TREE_CHAIN(tr) = t;
}

tree build_our_record(tree size)
{
  my_fold_p(&size);
  if (TREE_CODE(size)==NON_LVALUE_EXPR)
    size=TREE_OPERAND(size,0);
  tree rt = make_node(RECORD_TYPE);
  TREE_TYPE(rt)=long_integer_type_node; // 64-bit
  TYPE_SIZE_UNIT(rt)=size;
  TYPE_SIZE(rt)=size;
  //TYPE_MODE(rt)=BLKmode;
  return rt;
}

tree
convert_field_ref_to_decl(tree ref, tree value)
{
  tree t = ref;
  tree field=build_decl (input_location, FIELD_DECL, 0/*cell_decl_p*/, long_integer_type_node); // 64-bit
  SET_DECL_C_BIT_FIELD(field);
  if (TREE_CODE(value)!=ADDR_EXPR)
    DECL_BIT_FIELD(field)=1;
  if (TREE_CODE(value)==PARM_DECL)
    value=RVAL_ADDR(value);
#if 0
  // check
  TREE_TYPE(TREE_OPERAND(t,2))=bitsizetype;
#endif
  DECL_FIELD_BIT_OFFSET(field)=TREE_OPERAND(t,2);
  DECL_FIELD_OFFSET(field)=convert(sizetype, fold(TREE_OPERAND(t,0)));//build_int_cst (long_integer_type_node, 0); // 64-bit
  DECL_SIZE_UNIT(field)=build_int_cst (long_integer_type_node, sizeof(long)); // 64-bit
  DECL_SIZE(field)=fold(TREE_OPERAND(t,1));//build_int_cst (long_integer_type_node, 8); // 64-bit
  my_fold(DECL_FIELD_OFFSET(field));
  my_fold(DECL_FIELD_OFFSET(field));
  my_fold(DECL_FIELD_BIT_OFFSET(field));
  my_fold_p(&DECL_FIELD_OFFSET(field)); // check. temp fix. better?
  my_fold_p(&DECL_FIELD_BIT_OFFSET(field)); // check. temp fix. better?
  my_fold(DECL_SIZE(field));
  STRIP_TYPE_NOPS(DECL_FIELD_OFFSET(field));
  STRIP_TYPE_NOPS(DECL_FIELD_BIT_OFFSET(field));
  STRIP_TYPE_NOPS(DECL_SIZE(field));
  if (!quiet_flag) printf("off %x %x %x\n",TREE_INT_CST_LOW(DECL_FIELD_OFFSET(field)), TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(field)),TREE_INT_CST_LOW(DECL_SIZE(field)));
  SET_DECL_OFFSET_ALIGN(field, 8);
  return field;
}

tree
handle_preset(tree name, tree pres, tree cell_decl_p, tree mytype)
{
  tree init;
  tree constructor_elements=0;
  tree attr=TREE_OPERAND(pres,0);
  for (;attr;attr=TREE_CHAIN(attr)) {
    tree d3=TREE_VALUE(attr);
    tree v=TREE_PURPOSE(attr);
    tree dd;
    // from ordinary_structure_reference:
    tree cell__ = get_identifier(add_underscore(name, 2));
    tree extref=RVAL_ADDR(build_external_ref (input_location, name, 0, 0));
    tree params = d3;
    tree type = xref_tag(STRUCTURE_ATTR,cell__);
    tree body = my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(type))))));
    tree access = my_copy_tree(TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(type))));
    //chainon (tree_last(d3), build_tree_list(NULL_TREE, extref)); 
    chainon (tree_last(d3), build_tree_list(NULL_TREE, build_int_cst (long_integer_type_node, 0))); 
    my_substitute(body,access,params);
    //my_substitute_parmz(body,access,params);// extra to zero parm
    dd=body;

    // differs from local from now on
    tree t;
    //t=LVAL_ADDR(t);
    t=TREE_OPERAND(dd,2);
    tree value=v;
    tree field=convert_field_ref_to_decl(t,value);
    if (TREE_CODE(value)==VAR_DECL) {
      tree i = DECL_INITIAL(value);
      if (TREE_CODE(i)==INTEGER_CST)
	value = i;
    }
#if 0
    // check. look later upon synching with sister routine
    if (TREE_CODE(value)!=INTEGER_CST)
      DECL_BIT_FIELD(field)=0;
#endif
    tree tmp_node, tmp_prev, tmp_field;
    int bit1 = TREE_INT_CST_LOW(DECL_FIELD_OFFSET(field));
    if (TREE_CODE(DECL_FIELD_OFFSET(field))==NON_LVALUE_EXPR)
      bit1 = TREE_INT_CST_LOW(TREE_OPERAND(DECL_FIELD_OFFSET(field),0));
    int bit2 = TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(field));
    if (TREE_CODE(DECL_FIELD_BIT_OFFSET(field))==NON_LVALUE_EXPR)
      bit2 = TREE_INT_CST_LOW(TREE_OPERAND(DECL_FIELD_BIT_OFFSET(field),0));
    int bit = bit1*32+bit2; // check. 64-bit
    for (tmp_node = constructor_elements, tmp_prev = 0; tmp_node; tmp_prev = tmp_node, tmp_node = TREE_CHAIN(tmp_node)) {
      tmp_field = TREE_PURPOSE(tmp_node);
      //      int tmp_bit = TREE_INT_CST_LOW(DECL_FIELD_OFFSET(tmp_field))*32+TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(tmp_field));
      int tmp_bit1 = TREE_INT_CST_LOW(DECL_FIELD_OFFSET(tmp_field));
      if (TREE_CODE(DECL_FIELD_OFFSET(tmp_field))==NON_LVALUE_EXPR)
	tmp_bit1 = TREE_INT_CST_LOW(TREE_OPERAND(DECL_FIELD_OFFSET(tmp_field),0));
      int tmp_bit2 = TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(tmp_field));
      if (TREE_CODE(DECL_FIELD_BIT_OFFSET(tmp_field))==NON_LVALUE_EXPR)
	tmp_bit2 = TREE_INT_CST_LOW(TREE_OPERAND(DECL_FIELD_BIT_OFFSET(tmp_field),0));
      int tmp_bit = tmp_bit1*32+tmp_bit2;
      if (tmp_bit>bit)
	break;
    }
    tree neww = tree_cons(field, value, 0);
    if (tmp_prev) {
      TREE_CHAIN(tmp_prev) = neww;
    }
    if (tmp_node) {
      TREE_CHAIN(neww) = tmp_node;
    }
    if (constructor_elements==0)
      constructor_elements = neww;
  }
  //    tree mytype=copy_node(long_integer_type_node); // 64-bit
  //    TREE_CODE (mytype) = RECORD_TYPE;
  tree tmp_node;
  for (tmp_node = constructor_elements; tmp_node; tmp_node = TREE_CHAIN(tmp_node)) {
    tree tmp_field = TREE_PURPOSE(tmp_node);
    int tmp_bit1 = TREE_INT_CST_LOW(DECL_FIELD_OFFSET(tmp_field));
    if (TREE_CODE(DECL_FIELD_OFFSET(tmp_field))==NON_LVALUE_EXPR)
      tmp_bit1 = TREE_INT_CST_LOW(TREE_OPERAND(DECL_FIELD_OFFSET(tmp_field),0));
    int tmp_bit2 = TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(tmp_field));
    if (TREE_CODE(DECL_FIELD_BIT_OFFSET(tmp_field))==NON_LVALUE_EXPR)
      tmp_bit2 = TREE_INT_CST_LOW(TREE_OPERAND(DECL_FIELD_BIT_OFFSET(tmp_field),0));
    if (!quiet_flag)printf ("F %d %d\n",tmp_bit1, tmp_bit2);
    //    printf ("F %d %d\n",TREE_INT_CST_LOW(DECL_FIELD_OFFSET(tmp_field)), TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(tmp_field)));
  }
  tree constructor = build_constructor_from_list (mytype,constructor_elements);
  TREE_CONSTANT(constructor)=1;
  TREE_ADDRESSABLE(constructor)=1;
  init=constructor;
  //init=field;
  return init;
}

tree
build_field_decl(int offset, int size)
{
  tree field=build_decl(input_location, FIELD_DECL, 0, long_integer_type_node); // 64-bit
  SET_DECL_C_BIT_FIELD(field);
  DECL_BIT_FIELD(field)=1;

  DECL_FIELD_BIT_OFFSET(field)=build_int_cst (bitsizetype /*long_integer_type_node*/, 0); // 64-bit

  DECL_FIELD_OFFSET(field)=build_int_cst (sizetype /*bitsizetype*/ /*long_integer_type_node*/, offset); // 64-bit

  DECL_SIZE_UNIT(field)=build_int_cst (long_integer_type_node, size); // 64-bit
  DECL_SIZE(field)=build_int_cst (sizetype /*long_integer_type_node*/, size<<3); // 64-bit
  return field;
}

int
  node_type_to_int(tree t)
{
  // 64-bit
  // also look at size and size_unit
  if (t==long_long_integer_type_node)
    return 8;
  // 64-bit
  if (t==long_integer_type_node)
#ifdef __x86_64__
    return 8;
#else
    return 4;
#endif
  if (t==integer_type_node)
    return 4;
  if (t==short_integer_type_node)
    return 2;
  if (t==char_type_node)
    return 1;
#ifdef __x86_64__
  return 8;
#else
  return 4;
#endif
}

tree
handle_initial_inner(tree * constructor_elements, tree attr, int * offset, int size)
{
  for (;attr;attr=TREE_CHAIN(attr)) {
    tree v=TREE_PURPOSE(attr);

    // differs from local from now on
    tree value = v;
    int rep=1;
    switch (TREE_CODE(value)) {
    case STRING_CST:
      {
	rep=TREE_STRING_LENGTH(value)-2;
	char * c=TREE_STRING_POINTER(value)+1;
	for(;rep;rep--, c++, *offset+=1 /* not size, it seems*/) {
	  tree field=build_field_decl(*offset,1/*size*/);
	  value=build_int_cst (long_integer_type_node, *c); // 64-bit
	  *constructor_elements = chainon (*constructor_elements, tree_cons (field, value, 0));
	}
	rep=TREE_STRING_LENGTH(value)-2;
	*offset+=size-1;
	*offset&=~(size-1);
      }
      break;
    case PLIT_GROUP:
    case INITIAL_GROUP:
      {
	rep=TREE_INT_CST_LOW(TREE_OPERAND(value,0));
	size=node_type_to_int(TREE_OPERAND(value,1));
	value=TREE_OPERAND(value,2);
	for(;rep;rep--)
	  handle_initial_inner(constructor_elements, value, offset, size);
      }
      break;
    default:
      {
	tree field=build_field_decl(*offset,size);
	if (TREE_CODE(value)==VAR_DECL) {
	  tree i = DECL_INITIAL(value);
	  if (TREE_CODE(i)==INTEGER_CST)
	    value = i;
	}
#if 1
	if (TREE_CODE(value)!=INTEGER_CST)
	  DECL_BIT_FIELD(field)=0;
#endif
	for(;rep;rep--, *offset+=size) 
	  *constructor_elements = chainon (*constructor_elements, tree_cons (field, value, 0));
      }
    }
  }
  return 0;
}

tree
handle_initial(tree name, tree pres, tree cell_decl_p, tree mytype, int size)
{
  tree init;
  tree constructor_elements=0;
  tree attr=TREE_OPERAND(pres,0);
  int offset=0;

  handle_initial_inner(&constructor_elements, attr, &offset, size);

  if (mytype==0)
    mytype = build_our_record(build_int_cst (long_integer_type_node, offset+size)); // 64-bit
  tree constructor = build_constructor_from_list (mytype,constructor_elements);
  TREE_CONSTANT(constructor)=1;
  init=constructor;
  //init=field;
  return init;
}

tree
handle_structure(tree name, tree st_attr, int alloc)
{
  tree decl, astruct, cell__; 
  tree mysize;
  
  cell__=get_identifier(add_underscore(name,2));
  astruct = start_structure (STRUCTURE_ATTR, cell__);
  st_attr = finish_structure (astruct, TREE_OPERAND(st_attr, 0) , 0,0,0,0,0); 
  tree size=TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(st_attr))));
  if (alloc==0)
    return 0;
  return size;
}

tree
create_temp_var()
{
  tree type = long_integer_type_node; // 64-bit

  // next something based on cp build_local_temp
  tree slot = build_decl (input_location, VAR_DECL, NULL_TREE, type);
  DECL_ARTIFICIAL (slot) = 1;
  DECL_CONTEXT (slot) = current_function_decl;
  TREE_USED (slot) = 1;
  layout_decl (slot, 0);
  return slot; 
}

tree
set_temp_var(tree decl, tree value)
{
  // next something based on cp build_target_expr

  if (value==0)
    value=build_int_cst (long_integer_type_node, 0); // workaround to avoid decl 0 and crash
  tree t = build4 (TARGET_EXPR, TREE_TYPE (decl), decl, value, 0, NULL_TREE);
  TREE_SIDE_EFFECTS (t) = 1;
  return t;
}

tree
handle_structure_attribute(tree name, tree alloc_actual_list, int ref)
{
  // fix later: ref
  //$$ = build_nt(STRUCTURE_ATTR,$1,$3); 
  //tree t=find_struct(mystructs,$1);
  tree t=xref_tag(STRUCTURE_TYPE, name);
  tree size=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))))));
  tree body=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t)))))));
  tree alloc=TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))));
  tree type, body_t, size_t, access_t, comp2, access;
  if (alloc_actual_list) {
    size=tree_cons(0,size,0); // temp workaround for maybe changing root node
    my_substitute(size,alloc,alloc_actual_list);
    my_substitute(body,alloc,alloc_actual_list);
    my_substitute(size,alloc,-1);
    my_substitute(body,alloc,-1);
    size=TREE_VALUE(size);
  } else {
    my_substitute(body,alloc,-1);
  }
  my_fold(size);
  if (ref)
    size=build_int_cst (long_integer_type_node, sizeof(long)); // 64-bit
  //tree decl=build_array_declarator (size, NULL_TREE, 0, 0) ; // 4x too big?
  ////decl->exp.operands[2]=t;
  type=char_type_node;
  //$$ = set_array_declarator_type (decl, type, 0);
  //TREE_TYPE(decl)=STRUCTURE_DECL;
  access=TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(t)));
  //$$ = tree;

  body_t=tree_cons(0,body,0);
  size_t=tree_cons(0,size,body_t);
  access_t=tree_cons(0,access,size_t);
  comp2=tree_cons(0,0,access_t);

  return build_nt (STRUCTURE_STUFF, comp2, comp2);
}

/*
tree
xref_struct (char * name)
{
  //tree id = name;
#if 0
  tree id = maybe_get_identifier (name);
  if (id==0)
    return 0;
#endif
  return my_lookup_tag (name);
}
*/

tree
mytag (tree t)
{
  if (t)
    return my_lookup_tag(t);
  else
    return 0;
}

tree
give_char_array_type_node()
{
  return char_array_type_node;
}

void
bli_print_statistics()
{
  extern int longest_string;
  extern int add_memcpy;
  fprintf(stderr, "\nBliss specific stats:\n");
  fprintf(stderr, "Longest produced macro was %d bytes\n", longest_macro);
  fprintf(stderr, "Make_macro_string unnecessary memcpy %d bytes\n", str1_memcpy);
  fprintf(stderr, "Make_macro_string unnecessary memcpys %d\n", str1_memcpys);
  fprintf(stderr, "Longest added string was %d bytes\n", longest_string);
  fprintf(stderr, "Add_string unnecessary memcpy %d bytes\n", add_memcpy);
  fprintf(stderr, "Fields %d\n", fields);
  fprintf(stderr, "Field iters %d\n", fielditers);
}

int
is_str_attr(tree t)
{
  return TREE_CODE(t)==STRUCTURE_TYPE;
}

tree
strip_literal(tree t)
{
  if (TREE_CODE(t)==VAR_DECL) {
    tree i = DECL_INITIAL(t);
    if (i && TREE_CODE(i)==INTEGER_CST)
      t = i;
  }
  return t;
}

void
add_linkage(char * name, tree type) {
  tree i = get_identifier(name);
  tree t = build_nt(LINKAGE, type);
  c_pushtag(input_location, i, t);
}

tree
find_linkage(char * name) {
  tree id=maybe_get_identifier(name);

  if (id==0)
    return 0;

#if 0
  tree * tag2 = my_lookup_tag (id);
#else
  tree tag2 = get_label_binding (id);
#endif

  if (tag2==0)
    return 0;

  tree tag = tag2;

  if (tag==0)
    return 0;

  if (tag==0 || TREE_CODE (tag) != LINKAGE)
    return 0;

  return tag;
}

int
is_counted(tree t)
{
  t = find_linkage_attr(t);
  if (t==0)
    return 0;
#if 0
  t = find_linkage(IDENTIFIER_POINTER(TREE_VALUE(t)));
#else
  t = find_linkage(IDENTIFIER_POINTER(TYPE_ATTRIBUTES(t))); // check
#endif
  return 0==strcmp("call",IDENTIFIER_POINTER(TREE_OPERAND(t,0)));
}

tree
build_external(tree id, tree attr)
{
  tree decl;
  tree size;
  tree id_ = 0;

  tree type = find_alloc_attr(attr);
  if (type==0)
    type = long_integer_type_node; // 64-bit
  else
    type=TREE_OPERAND(type,0);
  type=sign_convert(type,attr);
  size = type;

  tree st_attr = find_structure_attr(attr);
  if (st_attr) {
    handle_structure(id, st_attr, 1);
  }

  if (TREE_LANG_FLAG_4(id)) {
    tree decl = identifier_global_value (id);
    id_ = get_identifier(add_underscore(DECL_ASSEMBLER_NAME(decl), 1));
    reset_symbol (id);
  }
  decl = bli_start_decl (id, size, 0, 0);
  if (TREE_LANG_FLAG_4(id)) {
    SET_DECL_ASSEMBLER_NAME(decl, id_);
    TREE_LANG_FLAG_4(id)=0;
  }

  TREE_PUBLIC (decl) = 1;
  TREE_STATIC (decl) = 0;

  DECL_EXTERNAL (decl) = 1; // differs from bind here
  finish_decl (decl, input_location, 0, NULL_TREE, 0);
  return decl;
}

tree
convert_string_literal_to_integer(tree t)
{
  long val = 0;
  if (TREE_CODE(t)==STRING_CST) {
    if (TREE_STRING_LENGTH(t)>6)
      warning(0, "%%BLS32-W-TEXT, String literal too long for use outside a PLIT");
    else
      memcpy(&val,TREE_STRING_POINTER(t)+1,TREE_STRING_LENGTH(t)-2);
  }
  return build_int_cst (long_integer_type_node, val); // 64-bit
}

tree
get_value(tree t)
{
  if (TREE_CODE(t)==IDENTIFIER_NODE) {
    tree s = is_symbol(t);
    if (s && TREE_CODE(s)==VAR_DECL && TREE_READONLY(s) && DECL_INITIAL(s))
      return DECL_INITIAL(s);
  }
  return t;
}

struct c_parm *
build_parm_decl(tree id, tree type) {
  struct c_declspecs * specs = build_null_declspecs();
  struct c_typespec typespec;
  typespec.kind = ctsk_tagdef; // TODO
  typespec.spec = type;
  specs = declspecs_add_type(input_location, specs, typespec);
  return build_c_parm(specs, 0, build_id_declarator(id), input_location);
}

tree
bli_start_decl(tree declarator, tree type, bool initialized, tree attributes)
{
  struct c_declarator * c_declarator = build_id_declarator (declarator);
  struct c_declspecs * c_declspecs = build_specs(type);
  return start_decl (c_declarator, c_declspecs, initialized, attributes, 1, 0);
}

tree
bli_start_decl_2(tree declarator, tree type, bool initialized, tree attributes, int pointer)
{
  struct c_declarator * c_declarator = build_id_declarator (declarator);
  struct c_declspecs * c_declspecs = build_specs(type);
  c_declarator = make_pointer_declarator (0, c_declarator);
  return start_decl (c_declarator, c_declspecs, initialized, attributes, 1, 0);
}

struct c_declspecs *
build_specs(tree type)
{
  struct c_declspecs * specs = build_null_declspecs();
  struct c_typespec typespec;
  typespec.kind = ctsk_tagdef; // TODO
  typespec.spec = type;
  specs = declspecs_add_type(input_location, specs, typespec);
  return specs;
}

int
bli_start_function(tree type, tree declarator, struct c_arg_info * args, tree attributes)
{
  struct c_declarator * c_declarator = build_id_declarator (declarator);
  c_declarator = build_function_declarator (args, c_declarator);
  struct c_declspecs * c_declspecs = build_specs(type);
  return start_function (c_declspecs, c_declarator, attributes);
}

int
bli_finish_function() {
  cfun = NULL;
  current_function_decl = NULL;
}

tree
bli_make_pointer_declarator (tree type_quals_attrs, tree target)
{
  tree quals, attrs;
  tree itarget = target;
#if 0
  split_specs_attrs (type_quals_attrs, &quals, &attrs);
  if (attrs != NULL_TREE)
    itarget = tree_cons (attrs, target, NULL_TREE);
#endif
  //  return build1 (INDIRECT_REF, long_integer_type_node, itarget);
  return build1 (INDIRECT_REF, 0 /*quals*/, itarget);
}

/*
tree
bli_make_pointer_declarator_not(tree type, tree id)
{
#if 0
  if (type == NULL)
    type = long_integer_type_node; // 64-bit
#endif
  struct c_declarator * c = make_pointer_declarator (build_specs (type), build_id_declarator (id));
  return c->declarator;
}
*/

struct c_type_name *
build_type_name(tree t)
{
  struct c_declspecs *specs = build_specs (t);
  struct c_declarator *declarator = build_id_declarator (0);
  struct c_type_name *ret;
  ret = XOBNEW (&parser_obstack, struct c_type_name);
  ret->specs = specs;
  ret->declarator = declarator;
  return ret;
}
