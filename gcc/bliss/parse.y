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

#define NEW_ROUTINE_CALL 1
#define NEW_POINTER 1
#define NEW_POINTER_NOT_YET 1

#ifdef NEW_POINTER
#define LVAL_ADDR(x) (fold(build_indirect_ref (convert (integer_ptr_type_node, (x)), "unary *")))
#define RVAL_ADDR(x) (build_unary_op(ADDR_EXPR, (x), 0))
#define UN_RVAL_ADDR(x) (fold(build_indirect_ref (convert (integer_ptr_type_node, (x)), "unary *")))
#else
#define LVAL_ADDR(x) (build_indirect_ref (convert (integer_ptr_type_node, (x)), "unary *"))
#define RVAL_ADDR(x) (x)
#define UN_RVAL_ADDR(x) (x)
#endif
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

#include "c-common.h"

#include "bliss-tree.h"

#include "c-tree.h"

 //#undef IN_GCC
#if 1
#include "ggc.h"
#endif
 //#define IN_GCC

 static short *malloced_yyss;
 static void *malloced_yyvs;

 static int compstmt_count;

 static tree last_expr = 0;

 static tree myselect = 0;
 static tree mylabel = 0;
 static tree selif = 0;
 static char icc=0 ;
 static tree ifthenelseval=0;

 int yyrec = 0;

extern tree  build_modify_expr (tree, enum tree_code, tree);

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

 tree igroot;

struct structure {
  struct structure * next;
  tree elem;
};

struct structure * mystructs = 0;

struct mymacro {
  struct mymacro * next;
  char * name;
  int type;
  tree param;
  tree param2;
  tree body;
};

 struct mymacro * macros = 0;
 
 struct mymacro amacro;

 enum ps { p_none, p_ascii, p_asciz, p_ascic, p_ascid, p_rad50_11, p_rad50_10, p_sixbit, p_p };

#define DSC$K_DTYPE_T 14
#define DSC$K_CLASS_S 1

 struct dsc$descriptor {
	unsigned short        dsc$w_length;   
	unsigned char dsc$b_dtype;    
	unsigned char dsc$b_class;    
	void          *dsc$a_pointer; 
 };
 // has not got descrip.h (yet)

extern FILE *yyin;

// wasint yyparse()
void
bli_common_parse_file(set_yydebug) 
  int set_yydebug ATTRIBUTE_UNUSED;
{
  //z1zin=fopen(input_filename,"r");

#if YYDEBUG != 0
  yydebug = set_yydebug;
#else
  warning ("YYDEBUG not defined");
#endif

  get_builtin();

  yyin=fopen(input_filename,"r");

  return yyparse();
}

 extern tree current_function_decl; 
 extern struct function *cfun; 
 struct function *acfun = 0;
 tree afun = 0;
 int cfun_zero = 0;

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

 tree my_copy_tree (tree);
 void my_substitute (tree, tree, tree);
 void my_fold (tree);
 tree parm_first_to_last (tree);
 enum{SIMP_MACRO,COND_MACRO,ITER_MACRO,KEYW_MACRO};
 void add_macro (char *,int,tree,tree,tree);
 char * add_counted_string (tree, int);
 char * add_underscore (tree, int);
 void * find_macro(struct mymacro * s,char * name);
 char * make_macro_string(struct mymacro * m, tree r);
 tree find_init_attr(tree t);
 tree find_structure_attr(tree);
 tree find_tree_code(tree, int);
 char * my_strcat(const char *, const char *, int);
 char * my_strcat_gen(const char *, const char *, int);
 tree temp_value(tree);
 tree handle_preset(tree, tree, tree, tree);
 tree convert_field_ref_to_decl(tree,tree);
 tree handle_structure(tree, tree, int);
 tree create_temp_var();
 tree set_temp_var(tree, tree);

%}

%start mystart

%union {
  int type_int;
  char     *type_str;
  tree  type_node_p;
  int type_node_code;
  location_t location;
  enum tree_code code;
}

/* test */

%nonassoc K_IF
%nonassoc K_ELSE

/*%token_table yacc*/
%token <type_node_p> T_DIGITS
%token <type_node_p> T_NAME T_STRING T_IDIGITS LEXEME M_NAME T_LABELNAME
%token <type_node_p> P_SOFTERROR P_SOFTERROR2 P_SOFTERROR3 P_SOFTERROR4
%token <type_node_p> START_CTCE START_LEX START_EXPR END_EXPR END_BUILTIN
%token <type_node_p> T_FIELDNAME

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
%type <type_int> infix_operator
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
%type <type_int>  select_type K_OTHERWISE  K_ALWAYS
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
%type <type_int> K_PLIT K_UPLIT K_LONG K_WORD K_BYTE P_CTCE P_LTCE
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
%type <type_node_p> expr_list
/*%type <type_node_p> test tok*/
%type <location> save_location

%{
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
  last_expr_filename=input_filename;
  add_builtin();
}
save_location
 start_block K_ELUDOM
{
  $$=$6;
  if (yydebug) inform ("\n%%BLS-I-PARSED-OK-That's a module alright\n");
  while (! global_bindings_p ())
    poplevel (0, 0, 0);
  finish_fname_decls ();
  finish_file ();
}
/*
|
expression END_EXPR { last_expr = $1; YYACCEPT; }
*/
|
maybe_declaration_list END_EXPR { last_expr = $1; YYACCEPT; }
|
T_NAME END_EXPR 
{
  last_expr = $1;
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && TREE_CODE(TREE_TYPE($1))==INTEGER_CST) {
    last_expr=TREE_TYPE($1);
  }
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && TREE_CODE(TREE_TYPE($1))==IDENTIFIER_NODE) {
    last_expr=get_identifier(TREE_TYPE($1));
  }
  YYACCEPT; 
}
|
expr_list END_EXPR 
{
  // not quite finished? to catch parse(%remaining)
  tree d1=$1;
  d1=TREE_VALUE(d1);
  last_expr = d1;
  if (TREE_CODE(d1)==IDENTIFIER_NODE && TREE_TYPE(d1) && TREE_CODE(TREE_TYPE(d1))==INTEGER_CST) {
    last_expr=TREE_TYPE(d1);
  }
  if (TREE_CODE(d1)==IDENTIFIER_NODE && TREE_TYPE(d1) && TREE_CODE(TREE_TYPE(d1))==IDENTIFIER_NODE) {
    last_expr=get_identifier(TREE_TYPE(d1));
  }
  YYACCEPT; 
}
;

expr_list:
expr_list ',' expression
{
  chainon ($1, build_tree_list (NULL_TREE, $3));
}
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
  char *c=$1;
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
| U_MAIN '=' T_NAME  { $$ = 0; }
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
  if (TREE_CODE($1)==IDENTIFIER_NODE && TREE_TYPE($1) && TREE_CODE(TREE_TYPE($1))==IDENTIFIER_NODE) {
    $$=get_identifier(TREE_TYPE($1));
    goto myout;
  }
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  $$ = build_external_ref ($1, yychar == '(');
  if (TREE_LANG_FLAG_0($1)) {
	 //fprintf(stderr, "CON\n");
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
T_DIGITS { 
  tree t=$1;
#if 0
  t = build_int_2(atoi(TREE_STRING_POINTER($1)),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
#endif
  $$ = t;
 }
;

opt_sign: { $$=0; }
|'+' { $$ = 0; }
|'-' { $$ = 0; }
;

integer_literal: 
P_B T_STRING  { 
  tree t;
  t = build_int_2(1+strtol(TREE_STRING_POINTER($2),0,2),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
  $$ = t;
}
| P_O T_STRING  {
  tree t;
  t = build_int_2(1+strtol(TREE_STRING_POINTER($2),0,8),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
  $$ = t;
}
| P_DECIMAL T_STRING  {
  tree t;
  t = build_int_2(1+strtol(TREE_STRING_POINTER($2),0,10),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
  $$ = t;
}
| P_X T_STRING  {
  tree t;
  t = build_int_2(strtol(1+TREE_STRING_POINTER($2),0,16),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
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
  t = build_int_2(*(char*)(1+TREE_STRING_POINTER($2)),0);
  TREE_TYPE (t) = widest_integer_literal_type_node;
  t = convert (integer_type_node, t);
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
  case p_ascii:
	 if (len>4) error("string longer than 4");
  case p_asciz:
	 {
	 if (len>3) error("string longer than 3");
	 tree t;
	 int myint = 0;
	 char *c = &myint;
	 int i;
	 for (i=0;i<len;i++)
		c[i]=str[i];
	 t = build_int_2(myint,0);
	 TREE_TYPE (t) = widest_integer_literal_type_node;
	 t = convert (integer_type_node, t);
	 TREE_LANG_FLAG_0(t)=1;
	 $$ = t;
	 }
	 break;
  case p_ascic:
	 {
	 if (len>3) error("string longer than 3");
	 //	 $$ = add_counted_string($2, len);
	 tree t;
	 int myint = 0;
	 char *c = &myint;
	 int i;
	 c[0]=len;
	 for (i=0;i<len;i++)
		c[i+1]=str[i];
	 t = build_int_2(myint,0);
	 TREE_TYPE (t) = widest_integer_literal_type_node;
	 t = convert (integer_type_node, t);
	 TREE_LANG_FLAG_1(t)=1;
	 $$ = t;
	 }
	 break;
  case p_ascid:
	 // long_long_integer_type_node
	 {
	 unsigned long long l;
	 struct dsc$descriptor dsc = {
		dsc$b_dtype : DSC$K_DTYPE_T,
		dsc$b_class : DSC$K_CLASS_S,
		dsc$w_length : len
	 };
	 //	 volatile long * vec;
	 long firstlong;
	 tree cell_decl_p;
	 tree cell_decl;
	 tree cell;
	 tree init;
	 tree decl_p;
	 tree type = integer_type_node;
	 tree t = tree_cons (NULL_TREE, type, NULL_TREE); 
	 tree first;
	 tree high;
	 tree shift;
	 tree string;
	 char mystr[1024];
	 int new_len=1+((len+3)&1021);
	 memset(mystr+new_len-4, 0, 5);
	 memcpy(mystr, str, len);
	 int cpu_le=check_little_endian();

	 if (len)
	   string = build_string(new_len, mystr);
	 else
	   goto skip_build_string_etc;
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
	 tree d2 = build_nt(ARRAY_REF,get_identifier(s2),build_int_2(new_len/*+1*/,0));
	 tree decl = start_decl(d2, tree_cons(0, char_type_node, 0), 1, 0);
#if 0
	 if (len<4) {
	   int tmp_len=len+1;
	   tree index_type = c_common_signed_type (sizetype);
	   tree itype = fold (convert (index_type, build_int_2(5, 0))); 
	   itype = build_index_type (itype);
	   TREE_TYPE(decl) = build_array_type (char_type_node, itype);
	 }
#endif
	 start_init(decl,NULL,global_bindings_p());
	 finish_init();

	 tree constructor2 = build_constructor(TREE_TYPE(decl)/*integer_type_node int_array_type_node*/,string);
	 TREE_CONSTANT(constructor2)=1;
	 init=constructor2;

	 finish_decl (decl, string, NULL_TREE);
#endif

	 TREE_TYPE(string) = /*string*/char_array_type_node; // or char_array?
#if 0
	 tree addr = build_unary_op (ADDR_EXPR, string, 0);
#else
	 tree addr = build_unary_op (ADDR_EXPR, decl, 0);
#endif
	 skip_build_string_etc:
	 if (len==0) {
	   addr=build_int_2(0,0); // later: init this to be start of dsc
	 }
	 addr = convert (integer_type_node, addr);

#if 0
	 addr = build_int_2(42, 0);
	 TREE_TYPE (addr) = widest_integer_literal_type_node;
	 addr = convert (integer_type_node, addr);
#endif

	 //	 vec=&dsc;
	 firstlong=*(long *)&dsc;

	 tree last = tree_cons(build_int_2(1,0),addr,0);
	 tree nextlast = tree_cons(build_int_2(0,0),build_int_2(firstlong,0), last);

#if 0
	 tree decl2 = build_decl (CONST_DECL, 0, string_type_node);
	 DECL_INITIAL(decl2)=constructor;
	 pushdecl(decl2);
#endif
	 char s[16];
	 sprintf(s,"_dsc%d",icc);
	 icc++;
#if 0	 
	 type=long_integer_type_node;
	 tree size=build_int_2(2,0);
	 tree index_type = c_common_signed_type (sizetype);
	 tree itype=build_int_2(1,0);
	 itype = build_index_type (itype);
	 type = build_array_type (type, itype);
	 type = int_array_type_node;
#endif
	 tree d = build_nt(ARRAY_REF,get_identifier(s),build_int_2(2,0));
	 tree decl2 = start_decl(d, tree_cons(0, type, 0), 1, 0);
#if 0
	 DECL_SIZE_UNIT(decl2)=build_int_2(8,0);	
	 DECL_SIZE(decl2)=build_int_2(64,0);
	 DECL_ALIGN(decl2)=1;
#endif
	 start_init(decl2,NULL,global_bindings_p());
	 finish_init();

	 tree constructor = build_constructor(TREE_TYPE(decl2)/*integer_type_node int_array_type_node*/,nextlast);
	 TREE_CONSTANT(constructor)=1;
	 init=constructor;

	 finish_decl (decl2, init, NULL_TREE);

	 //	 fprintf(stderr, "vec %x %x %x %x %x\n",firstlong,sizeof( long),vec[0],vec[1],len);
	 
	 first = build_int_2(firstlong, 0);
	 TREE_TYPE (first) = widest_integer_literal_type_node;
	 first = convert (long_long_integer_type_node, first);

	 TREE_TYPE (addr) = widest_integer_literal_type_node;
	 addr = convert (long_long_integer_type_node, addr);

	 shift = build_int_2(32, 0);
	 TREE_TYPE (shift) = widest_integer_literal_type_node;
	 shift = convert (integer_type_node, shift);

	 if (cpu_le)
	   addr = parser_build_binary_op (LSHIFT_EXPR, addr, shift);
	 else
	   first = parser_build_binary_op (LSHIFT_EXPR, first, shift);

	 tree longlong = parser_build_binary_op (BIT_IOR_EXPR, first, addr);

	 $$ = longlong;
	 $$ = build_unary_op (ADDR_EXPR, longlong, 0);
	 $$ = build_unary_op (ADDR_EXPR, decl2, 0);
	 TREE_CONSTANT($$)=1;
	 TREE_LANG_FLAG_2($$)=1;

#if 0

	 tree tstr=get_identifier(add_underscore(IDENTIFIER_POINTER($2),2));
    TREE_TYPE(tstr)=string_type_node;
	 cell_decl_p = start_decl (cell, current_declspecs, 0,
										chainon (NULL_TREE, all_prefix_attributes));
	 finish_decl (cell_decl_p, 0, NULL_TREE);

	 cell=get_identifier(add_underscore(IDENTIFIER_POINTER($2),1));
    TREE_TYPE(cell)=long_long_integer_type_node;
	 cell_decl_p = start_decl (cell, current_declspecs, 0,
										chainon (NULL_TREE, all_prefix_attributes));
	 finish_decl (cell_decl_p, 0, NULL_TREE);

	 decl_p = make_pointer_declarator(0,$2);
	 cell_decl = start_decl (decl_p, current_declspecs, 1,
									 chainon (NULL_TREE, all_prefix_attributes));
	 
	 start_init(cell_decl,NULL,global_bindings_p());
	 finish_init();
	 
	 init = build_unary_op (ADDR_EXPR, cell_decl_p/*build_external_ref (c, 0)*/, 0);
	 

	 finish_decl (cell_decl, init, NULL_TREE);
#endif
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
  long size=0;
  char * start = page;
  char * cur = start;
  memset(page, 0, 4096);
  if (counted)
	 cur+=4;
  tree t = $4;
  while (t) {
	 switch (TREE_CODE(t)) {
	 case IDENTIFIER_NODE:
		{
		  char * str=IDENTIFIER_POINTER(t);
		  int len=strlen(str)-2;
		  str++;
		  memcpy(cur, str, len);
		  if (len&3)
			 len+=(4-(len&3));
		  cur+=len;
		}
		break;
	 case INTEGER_CST:
		{
		  int low=TREE_INT_CST_LOW(t);
		  memcpy(cur, &low, 4);
		  cur+=4;
		}
		break;
	 default:
		break;
	 }
	 t = TREE_CHAIN(t);
  }
  if (counted) {
	 page[0]=((cur-start)>>2)-1;
  }

  tree string = build_string(cur-start, page);
  TREE_TYPE(string) = /*string*/char_array_type_node; // or char_array?
#if 0
  tree addr = string;
#else
  tree addr = build_unary_op (ADDR_EXPR, string, 0);
#endif
  addr = convert (integer_type_node, addr);
  $$ = addr;
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

labeled_block:
attached_label_list
{
  tree cur_last=last_tree;
  tree decl;
  tree name=$1;
  decl = lookup_label(name);
  //  decl = build_decl (LABEL_DECL, name, TREE_TYPE (integer_type_node));
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;

  tree label_decl = decl;
  tree node = build (LABELED_BLOCK_EXPR, NULL_TREE, label_decl, NULL_TREE);
  TREE_SIDE_EFFECTS (node) = 1;
  TREE_TYPE(node)=void_type_node;
  LABELED_BLOCK_LABEL(node)=label_decl; // superfluous?
  TREE_OPERAND(node, 1)= cur_last; // temp storage
  $$=node;
  $$=c_expand_expr_stmt($$);
}
unlabeled_block
{
  tree body = last_tree;
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
  //last_tree = node;
  TREE_SIDE_EFFECTS (body) = 1;
  //c_expand_expr_stmt(node);

#if 0
  tree n = build1 (STMT_EXPR, last_expr_type, node);
  TREE_TYPE(n)=void_type_node;
  TREE_SIDE_EFFECTS (n) = 1;
  c_expand_expr_stmt(n);
#endif

  //last_tree = node;
  $$ = node;
  $$ = 0;
  //c_expand_expr_stmt($$);
  //$$ = 0;
  //  add_stmt($$);
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
  $$=c_begin_compound_stmt ();
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
    $$=c_begin_compound_stmt ();
  }
#endif
}
;

unlabeled_block: unlabeled_block_start
pushlevel block_body K_END poplevel
{ 
#ifndef c99
  $$=poplevel (KEEP_MAYBE, 0, 0);
  SCOPE_STMT_BLOCK (TREE_PURPOSE ($5))
	 = SCOPE_STMT_BLOCK (TREE_VALUE ($5))
	 = $$;
 RECHAIN_STMTS ($1, COMPOUND_BODY ($1)); 
 last_expr_type = NULL_TREE;
 $$ = $3;
#if 0
 $$ = build_nt (BLOCK_BODY, $1, $3);
 if ($3) TREE_TYPE($$)=TREE_TYPE($3);
 else TREE_TYPE($$)=void_type_node;
#endif
#else
 $$=0;
#endif
}
| unlabeled_block_start2 
pushlevel block_body ')' poplevel
{ 
#ifndef c99
  if (cfun==0)
	 goto no_cfun;
$$=poplevel (KEEP_MAYBE, 0, 0);
 SCOPE_STMT_BLOCK (TREE_PURPOSE ($5))
   = SCOPE_STMT_BLOCK (TREE_VALUE ($5))
   = $$;
 RECHAIN_STMTS ($1, COMPOUND_BODY ($1)); 
 last_expr_type = NULL_TREE;
 $$=$1;
 no_cfun:
 $$=$3;
#else 
 $$=0;
#endif
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
/* { $$ = bli_begin_compound_stmt (); } */
maybe_block_action_list 
maybe_block_value 
{
  $$=$3;
}
;
/*
maybe_declaration_list: { $$=0; }
| declaration_list 
;
*/
maybe_declaration_list:  { $$=NULL_TREE; }
| maybe_declaration_list declaration {
  /*  */
  if ($2)
    $$ = chainon($1, $2);
  else
    $$ = $1;
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
  $$=chainon($1, $2); 
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
    if (TREE_CODE($1)<SIZEOF_EXPR)
      $$=c_expand_expr_stmt($1);
}
;

maybe_block_value: { $$=build_int_2(0,0); }
|block_value
{
#if 0
  if ($1) 
    if (TREE_CODE($1)<SIZEOF_EXPR)
		c_expand_expr_stmt($1);
  $$=$1; 
#endif

  goto no_cfun_block_value;
  if (cfun==0)
	 goto no_cfun_block_value;

  tree type = integer_type_node;

  // next something based on cp build_local_temp
  tree slot = build_decl (VAR_DECL, NULL_TREE, type);
  DECL_ARTIFICIAL (slot) = 1;
  DECL_CONTEXT (slot) = current_function_decl;
  layout_decl (slot, 0);

  tree decl = slot;
  tree value = $1;
  // next something based on cp build_target_expr

  if (value==0) value=build_int_2(0,0); // workaround to avoid decl 0 and crash
  //fprintf(stderr, "bu %x %x %x %x\n",TREE_TYPE (decl), decl, value,0);
  tree t = build (TARGET_EXPR, TREE_TYPE (decl), decl, value,
						0 /*cxx_maybe_build_cleanup (decl)*/, NULL_TREE);
  TREE_SIDE_EFFECTS (t) = 1;
  $$ = copy_node(t); //t;

  c_expand_expr_stmt(t);
  TREE_TYPE($$)=integer_type_node;
  goto block_value_out;
 no_cfun_block_value:
  $$ = $1;
 block_value_out:
}
;

block_value: expression 
;

structure_reference:
  ordinary_structure_reference 
  /*| default_structure_reference nit ye*/
| general_structure_reference 
;

ordinary_structure_reference:
segment_name '[' access_actual_list ']' {
  tree cell__ = get_identifier(add_underscore($1, 2));
  //tree t=build_external_ref ($1, 0);
  tree extref=RVAL_ADDR(build_external_ref ($1, 0));
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
  $$ = build_tree_list (NULL_TREE, $1);
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
|segment_expression  
;

segment_expression: exp 
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
;

general_structure_reference:
structure_name '[' access_part ';' alloc_actual_list ']' 
|structure_name '[' access_part ']' 
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
    ref = build_external_ref ($1, 1);
  else
    ref=$1;

  if (TREE_CODE(ref)!=FUNCTION_DECL) {
    tree decl = create_temp_var();
    TREE_TYPE(decl)=default_function_type;
    tree t = set_temp_var(decl, ref);
    c_expand_expr_stmt(t);
    ref = decl;
  }
  //    ref= build_decl( FUNCTION_DECL, build_unary_op (ADDR_EXPR, ref, 0), integer_type_node);
  $$ = build_function_call (ref, $3); 
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
$$=0;
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
  tree pos = TREE_OPERAND($2, 0);
  tree size = TREE_OPERAND($2, 1);
  char context=getbitcontext();
  /* if (context=='o') */ {
    // 32 hardcoded
    tree f = build_int_2(5,0); // (32==2^5)
    TREE_TYPE (f) = widest_integer_literal_type_node;
    f = convert (integer_type_node, f);
    t = parser_build_binary_op(RSHIFT_EXPR, pos, f);
    op0 = parser_build_binary_op (PLUS_EXPR, $1, t);
  }
  /*if (context=='f') */ {
    tree i, t;
    if (TREE_CODE(TREE_TYPE($1))==POINTER_TYPE) {
      i = build_indirect_ref ($1, "unary *");
    } else {
		if (yydebug) inform ("\n%%BLS32-I-NOTHING fetch arg not ptr (probably in structure?) %x\n",input_location.line);
      //i = build_indirect_ref (convert(build_pointer_type (integer_type_node), $1), "unary *");
      //i = build_indirect_ref ($1, "unary *");
      //goto fetch_end;
      i=$1;
    }
    //tree t=build (BIT_FIELD_REF, c_common_type_for_mode(TYPE_MODE (integer_type_node),1), i, size_int (size->int_cst.int_cst.low), bitsize_int (pos->int_cst.int_cst.low));
    t=build (BIT_FIELD_REF, c_common_type_for_mode(TYPE_MODE (integer_type_node),1), i, size, pos);
    TREE_TYPE(TREE_OPERAND(t, 2)) = ubitsizetype;
    t=stabilize_reference(t);
    op1 = t; 
  fetch_end:
	 {}
  }
  /* if (context=='o') */ {
    tree d=$1;
    if (TREE_CODE(TREE_TYPE(d))==POINTER_TYPE) {
#ifndef NEW_POINTER
      d = build_indirect_ref (d, "unary *");
#endif
    } else {
      if (yydebug) inform("\n%%BLS32-I-NOTHING not pointer for field ref? %x\n", input_location.line);
      //d = build_indirect_ref (convert(build_pointer_type (integer_type_node), d), "unary *");
      //d = build_indirect_ref (d, "unary *");
    }
    //tree t=build (BIT_FIELD_REF, c_common_type_for_mode(TYPE_MODE (integer_type_node),1), d, size_int (size->int_cst.int_cst.low), bitsize_int (pos->int_cst.int_cst.low));
    t=build (BIT_FIELD_REF, c_common_type_for_mode(TYPE_MODE (integer_type_node),1), d, size, pos);
    TREE_TYPE(TREE_OPERAND(t, 2)) = ubitsizetype;
    t=stabilize_reference(t);
   op2 = t;
  }
  if (context=='a') {
    if (yydebug) inform("not implemented yet");
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
{ $$ = 0; } 
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

keyword_formal_name: T_NAME;

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

myassign: opexp9 '=' opexp9 {$<type_node_p>$=0};

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
    if (TREE_OPERAND($$, 1)) TREE_OPERAND($$, 1)=fold(TREE_OPERAND($$, 1));
    if (TREE_OPERAND($$, 2)) TREE_OPERAND($$, 2)=fold(TREE_OPERAND($$, 2));
    TREE_TYPE(TREE_OPERAND($$, 2)) = ubitsizetype;
  } else {
    $$ = build_indirect_ref (convert(integer_ptr_type_node, $3), "unary *"); 
  }
  setbitcontext('o');
  popbitstack();
}
| UPLUS opexp9 %prec UNARY { $$ = build_unary_op (CONVERT_EXPR, $2, 0); }
| UMINUS opexp9 %prec UNARY { $$ = build_unary_op (NEGATE_EXPR, $2, 0); }
/*
| '+' opexp9 %prec UNARY { $$ = build_unary_op (CONVERT_EXPR, $2, 0); }
| '-' opexp9 %prec UNARY { $$ = build_unary_op (NEGATE_EXPR, $2, 0); }
*/
| opexp9 '^' opexp9 { 
  tree d3 = fold ( $3 );
  //  if (tree_int_cst_sgn (d3) < 0) 
  if (!tree_expr_nonnegative_p(d3)) {
    $$ = parser_build_binary_op (RSHIFT_EXPR, $1, build_unary_op( NEGATE_EXPR, d3, 0)); 
    if (yydebug) inform("\n%%BLS-I-NOTHING rshift %x\n", input_location.line);
  } else {
    $$ = parser_build_binary_op (LSHIFT_EXPR, $1, d3); 
    if (yydebug) inform("\n%%BLS-I-NOTHING lshift %x\n", input_location.line);
  }
}
| opexp9 K_MOD opexp9 { $$ = parser_build_binary_op (TRUNC_MOD_EXPR, $1, $3); }
| opexp9 '*' opexp9 { $$ = parser_build_binary_op (MULT_EXPR, $1, $3); }
| opexp9 '/' opexp9 { $$ = parser_build_binary_op (TRUNC_DIV_EXPR, $1, $3); }
| opexp9 '+' opexp9 { $$ = parser_build_binary_op (PLUS_EXPR, $1, $3); }
| opexp9 '-' opexp9 { $$ = parser_build_binary_op (MINUS_EXPR, $1, $3); }
| opexp9 infix_operator opexp9 {
  $$ = parser_build_binary_op ($2, $1, $3);
 }
| K_NOT opexp9 %prec K_NOT { $$ = build_unary_op (BIT_NOT_EXPR, $2, 0); }
| opexp9 K_AND opexp9 { $$ = parser_build_binary_op (BIT_AND_EXPR, $1, $3); }
|  opexp9 K_OR opexp9 { $$ = parser_build_binary_op (BIT_IOR_EXPR, $1, $3); }
| opexp9 K_EQV opexp9 { $$ = build_unary_op(BIT_NOT_EXPR,parser_build_binary_op (BIT_XOR_EXPR, $1, $3),0); }
| opexp9 K_XOR  opexp9 { $$ = parser_build_binary_op (BIT_XOR_EXPR, $1, $3); }
|
opexp9 '=' opexp9 { 
  tree t=$1;
  tree b=TREE_OPERAND (t, 2);
  if (TREE_CODE(t) == BIT_FIELD_REFS && b && TREE_CODE(b) == BIT_FIELD_REF) {
    tree newop0, op0=TREE_OPERAND(b, 0);
    if (TREE_OPERAND(b, 1)) TREE_OPERAND(b, 1)=fold(TREE_OPERAND(b, 1));
    if (TREE_OPERAND(b, 2)) TREE_OPERAND(b, 2)=fold(TREE_OPERAND(b, 2));
    TREE_TYPE(TREE_OPERAND(b, 2)) = ubitsizetype;
    if (TREE_CODE(op0)==INDIRECT_REF) {
      newop0=op0;
      op0=TREE_OPERAND(op0, 0);
#if 0
      if (TREE_CODE(op0)==PLUS_EXPR && TREE_CODE(TREE_TYPE(op0))==POINTER_TYPE) {
	fprintf(stdout, "\n\nxyz %x\n\n",input_location.line);
	TREE_TYPE(op0)==integer_type_node;
      }
#endif
    } else {
      tree tt=make_pointer_declarator(0,op0);
      TREE_TYPE(tt)=build_pointer_type(integer_type_node);
      TREE_TYPE(tt)=integer_type_node;
      //tree i = build_unary_op (ADDR_EXPR, op0, 1);
      //newop0=build_indirect_ref (convert(build_pointer_type (integer_type_node),op0), "unary *");
      //newop0=build_indirect_ref (i, "unary *");
      TREE_OPERAND(b, 0)=newop0;
      TREE_OPERAND(b, 0)=tt;
    }
    $$=build_modify_expr(b, NOP_EXPR, $3);
    goto bitend;
  }
  if (TREE_CODE(t) == INTEGER_CST || (TREE_CODE(t)==NON_LVALUE_EXPR && TREE_CODE(TREE_OPERAND(t, 0))==INTEGER_CST )) {
    t=make_pointer_declarator(0,$1);
    TREE_TYPE(t)=build_pointer_type(integer_type_node);
    $$=build_modify_expr(t, NOP_EXPR, $3); // check. no LVAL_ADDR?
  } else {
#if 0
    $$=build_modify_expr(build_indirect_ref (convert(integer_ptr_type_node, t), "unary *"), NOP_EXPR, $3);
#else
	 $$=build_modify_expr(LVAL_ADDR(t), NOP_EXPR, $3);
#endif
  }
 bitend:
#if 0
 setbitcontext('o');
 popbitstack();
{ pushbitstack();
 setbitcontext('a');
 $$=0;
} 
#endif
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
K_EQL   { $$ = EQ_EXPR; }
| K_EQLA   { $$ = EQ_EXPR; }
| K_EQLU  { $$ = EQ_EXPR; }
| K_NEQ   { $$ = NE_EXPR; }
| K_NEQA   { $$ = NE_EXPR; }
| K_NEQU   { $$ = NE_EXPR; }
| K_LSS   { $$ = LT_EXPR; }
| K_LSSA   { $$ = LT_EXPR; }
| K_LSSU   { $$ = LT_EXPR; }
| K_LEQ  { $$ = LE_EXPR; }
| K_LEQA   { $$ = LE_EXPR; }
| K_LEQU   { $$ = LE_EXPR; }
| K_GTR   { $$ = GT_EXPR; }
| K_GTRA   { $$ = GT_EXPR; }
| K_GTRU   { $$ = GT_EXPR; }
| K_GEQ   { $$ = GE_EXPR; }
| K_GEQA  { $$ = GE_EXPR; }
| K_GEQU   { $$ = GE_EXPR; }
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
  // check redo
  if ($1==P_REF)
    goto do_pref;
  // copy from routine call?
  void * ref;
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  ref = build_external_ref ($1, 1);
  $$ = build_function_call (ref, $3); 
  goto out_exec_func;

 do_pref:
  // not quite according to 5.2.2.3
  $$ = build_unary_op(ADDR_EXPR, TREE_VALUE($3), 0);

 out_exec_func:
}
|executable_function_name '(' ')' 
{
  // copy from routine call?
  void * ref;
  if (yychar == YYEMPTY)
    yychar = YYLEX;
  ref = build_external_ref ($1, 1);
  $$ = build_function_call (ref, 0); 
}
;

executable_function_named:
standard_function_name
/* standard_function_name  
| linkage_function_name 
| character_handling_function_name 
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
  $$ = P_REF;
}
;

character_handling_function_name: T_NAME 
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
  // next something based on cp build_local_temp
  tree decl = create_temp_var();
  ifthenelseval=decl;

  $<type_node_p>$ = c_begin_if_stmt ();
  TREE_TYPE ($<type_node_p>$) = integer_type_node;
}
exp
{
  tree t=$3;
  t=parser_build_binary_op(BIT_AND_EXPR,convert(integer_type_node,t),build_int_2(1,0));
  c_expand_start_cond (c_common_truthvalue_conversion (t), 
		       compstmt_count,$<type_node_p>2);
  compstmt_count++;
  $<type_node_p>$=c_begin_compound_stmt ();
}
K_THEN
pushlevel exp poplevel
{
  if ($7) {
    $7 = set_temp_var(ifthenelseval, $7);
  }

#if 1
  if ($7) /* $<type_node_p>$ =*/ c_expand_expr_stmt($7);
#endif
  $$=poplevel (KEEP_MAYBE, 0, 0);
  SCOPE_STMT_BLOCK (TREE_PURPOSE ($8))
    = SCOPE_STMT_BLOCK (TREE_VALUE ($8))
    = $$;
  RECHAIN_STMTS ($<type_node_p>4, COMPOUND_BODY ($<type_node_p>4)); 
  last_expr_type = NULL_TREE;
  //  $$=$3;

  tree if_stmt = $<type_node_p>2;
#if 0
  if ($7) /* $<type_node_p>$ =*/ c_expand_expr_stmt($7);
#endif
  //  RECHAIN_STMTS ($<type_node_p>4, COMPOUND_BODY ($<type_node_p>4));
  //temp_value(fold($6));
  c_finish_then();
  //THEN_CLAUSE (if_stmt) = $6;
  $$=$<type_node_p>2;
  //  $$=$6;
  $$ = ifthenelseval;
}
;

conditional_expression: 
if_then
K_ELSE
{
  c_expand_start_else();
  compstmt_count++;
  $$=c_begin_compound_stmt ();
}
pushlevel exp poplevel
{
  if ($5) {
    $5 = set_temp_var(ifthenelseval, $5);
  }

#if 1
  if ($5) $5 /*$<type_node_p>$*/ = c_expand_expr_stmt($5);
#endif
  $$=poplevel (KEEP_MAYBE, 0, 0);
  SCOPE_STMT_BLOCK (TREE_PURPOSE ($6))
    = SCOPE_STMT_BLOCK (TREE_VALUE ($6))
    = $$;
  RECHAIN_STMTS ($<type_node_p>3, COMPOUND_BODY ($<type_node_p>3)); 
  last_expr_type = NULL_TREE;
  //  $$=$3;

  tree if_stmt = $1;
  tree nop = build_int_2(0,0);

#if 0
  if ($5) $5 /*$<type_node_p>$*/ = c_expand_expr_stmt($5);
#endif
  //RECHAIN_STMTS ($<type_node_p>3, COMPOUND_BODY ($<type_node_p>3));

  //temp_value(fold($4));
  c_finish_else();
  c_expand_end_cond();

  //c_expand_expr_stmt(ifthenelseval);

  $$ = 0;
  $$ = $1; 
  $$ = ifthenelseval;

#if 0
  if (TREE_TYPE(THEN_CLAUSE(if_stmt))==0)
    TREE_TYPE(THEN_CLAUSE(if_stmt))=integer_type_node;
  if (TREE_TYPE(ELSE_CLAUSE(if_stmt))==0)
    TREE_TYPE(ELSE_CLAUSE(if_stmt))=integer_type_node;
#endif

#if 0
  $$ = build_conditional_expr (IF_COND(if_stmt), THEN_CLAUSE(if_stmt), ELSE_CLAUSE(if_stmt));
  //  $$ = add_stmt ( build_stmt (EXPR_STMT, $$));
  //TREE_TYPE ($$) = TREE_TYPE (TREE_OPERAND($$, 0));
  //  $$ = c_expand_expr_stmt ($$);

  TREE_SIDE_EFFECTS ($$) = 1;
  TREE_SIDE_EFFECTS (THEN_CLAUSE(if_stmt)) = 1;
  TREE_SIDE_EFFECTS (ELSE_CLAUSE(if_stmt)) = 1;

  THEN_CLAUSE(if_stmt) = 0;//nop;
  ELSE_CLAUSE(if_stmt) = 0;//nop;
#endif
}
|
if_then %prec K_IF
{
  tree if_stmt = $1;
  tree nop = build_int_2(0,0);

  c_expand_end_cond ();

  $$ = $1;
  //  $$ = 0;

#if 0
  if (TREE_TYPE(THEN_CLAUSE(if_stmt))==0)
    TREE_TYPE(THEN_CLAUSE(if_stmt))=integer_type_node;
#endif

#if 0
  $$ = build_conditional_expr (IF_COND(if_stmt), THEN_CLAUSE(if_stmt), build_int_2(0,0)/*save_expr($1)*/);
  //  $$ = c_expand_expr_stmt ($$);
  //TREE_TYPE ($$) = TREE_TYPE (TREE_OPERAND($$, 0));

  TREE_SIDE_EFFECTS ($$) = 1;
  TREE_SIDE_EFFECTS (THEN_CLAUSE(if_stmt)) = 1;
  //TREE_SIDE_EFFECTS (ELSE_CLAUSE(if_stmt)) = 1;

  THEN_CLAUSE(if_stmt) = 0;//nop;
  ELSE_CLAUSE(if_stmt) = 0;//nop;
#endif

}
;

exp:
expression 
;

case_expression:
K_CASE exp K_FROM
{
  $<type_node_p>$ = c_start_case ($2);
  c_in_case_stmt++;
}
ctce K_TO ctce K_OF
{
  c_start_case_2 ($5, $7);
}
K_SET case_line_list K_TES
{
  c_finish_case ();
  c_in_case_stmt--;
  $$=0; 
  //  $$=$<type_node_p>3;
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
  //chainon ($2, build_stmt (EXPR_STMT, $5));
  add_stmt (build_stmt (EXPR_STMT, $5));
  add_stmt (build_break_stmt ()); // selectone always
}
|
'[' case_label_list ']' ':' case_action error
{
  yyerrok;
  // another deviation. handle no ';' before tes
  //chainon ($2, build_stmt (EXPR_STMT, $5));
  add_stmt (build_stmt (EXPR_STMT, $5));
  add_stmt (build_break_stmt ()); // selectone always
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
  $$ = do_case ($1, 0);
}
|
ctce K_TO ctce 
{
  $$ = do_case ($1, $3);
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
;

select_expression:
select_type select_index
{
  myselect = fold($2);
  tree label = build_decl (LABEL_DECL, NULL_TREE, NULL_TREE);
  tree decl=label;
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
  mylabel = decl;
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
  C_DECLARED_LABEL_FLAG (mylabel) = 1;
  add_decl_stmt (mylabel)
}
;

select_type:  K_SELECT | K_SELECTA | K_SELECTU
| K_SELECTONE | K_SELECTONEA | K_SELECTONEU
{
  // note always behaves as selectone
} 
;

select_index:
expression 
;

select_line_list:
select_line_list select_line 
{
  // check. must connect in some other way, later
  // do not. last in list got chained to itself
  //$$ = chainon($1, $2);
}
|
select_line 
|
error {
  yyerrok;
  // handle empty otherwise
}
;

select_line:
'[' 
{
  $<type_node_p>$ = c_begin_if_stmt ();
  TREE_TYPE ($<type_node_p>$) = integer_type_node;
}
select_label_list ']' ':'
{
  tree t = 0;
  tree cond = $3;
  if (cond==0) {
    t=build_int_2(1,0);
    goto no_t;
  }
  t = TREE_VALUE(cond);
  for (cond=TREE_CHAIN(cond);cond;cond=TREE_CHAIN(cond)) {
    t = parser_build_binary_op(BIT_IOR_EXPR,t,TREE_VALUE(cond));
  } 
 no_t:
  c_expand_start_cond (c_common_truthvalue_conversion (t), 
			 compstmt_count,$<type_node_p>2);

#if 0
  // not now
  //chainon ($2, build_stmt (EXPR_STMT, $5));
  add_stmt (build_stmt (EXPR_STMT, $<type_node_p>5));
  add_stmt (build_break_stmt ()); // selectone always
#endif
}
select_action ';'
{
  tree if_stmt = $<type_node_p>2;
  if ($7) /* $<type_node_p>$ =*/ c_expand_expr_stmt($7);

  // only if one?
  tree decl=mylabel;
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
  tree newdecl=build(LABEL_EXPR, NULL_TREE, decl, NULL_TREE);

#if 0
  // not yet
  tree label_block_expr = newdecl;
  tree t = build (EXIT_BLOCK_EXPR, NULL_TREE, label_block_expr, NULL_TREE);
  TREE_TYPE(t)=void_type_node;
  TREE_SIDE_EFFECTS (t) = 1;
  add_stmt(t);
#endif
  add_stmt (build_stmt (GOTO_STMT, mylabel));

  c_finish_then();
  c_expand_end_cond();
  //THEN_CLAUSE (if_stmt) = $7;
  $$=$<type_node_p>2;
  //  $$=$6;
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
#if 0
  $$ = do_case ($1, 0);
#endif
  tree e1 = $1;
  if (TREE_CODE(e1)==IDENTIFIER_NODE) {
    char *s=IDENTIFIER_POINTER(e1);
    e1=build_int_2(s[1],0);
  }
  if (TREE_CODE(e1)==STRING_CST) {
    char *s=TREE_STRING_POINTER(e1);
    e1=build_int_2(s[1],0);
  }
  tree t = parser_build_binary_op(EQ_EXPR,myselect,e1);
  $$ = tree_cons (0, t, 0);
}
|
exp K_TO exp
{
#if 0
  // can not be variable. for future?
  $$ = do_case ($1, $3);
#endif
  tree e1 = $1;
  if (TREE_CODE(e1)==IDENTIFIER_NODE) {
    char *s=IDENTIFIER_POINTER(e1);
    e1=build_int_2(s[1],0);
  }
  if (TREE_CODE(e1)==STRING_CST) {
    char *s=TREE_STRING_POINTER(e1);
    e1=build_int_2(s[1],0);
  }
  tree e2 = $3;
  if (TREE_CODE(e2)==IDENTIFIER_NODE) {
    char *s=IDENTIFIER_POINTER(e2);
    e2=build_int_2(s[1],0);
  }
  if (TREE_CODE(e2)==STRING_CST) {
    char *s=TREE_STRING_POINTER(e2);
    e2=build_int_2(s[1],0);
  }
  tree t1=parser_build_binary_op(GE_EXPR,myselect,e1);
  tree t2=parser_build_binary_op(LE_EXPR,myselect,e2);
  tree t = parser_build_binary_op(BIT_AND_EXPR, t1, t2); 
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

loop_expression:
indexed_loop_expression  
|
tested_loop_expression  
;

by_exp:
{
  $$ = build_int_2(1,0);
}
|
K_BY exp
{
  $$ = $2;
}
;

from_exp:
{
  $$ = build_int_2(0,0);
}
|
K_FROM exp
{
  $$ = $2;
}
;

to_exp:
{
  $$ = build_int_2(0,0);
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
  $<type_node_p>$ = build_stmt (FOR_STMT, NULL_TREE, NULL_TREE,
			  NULL_TREE, NULL_TREE);
  add_stmt ($<type_node_p>$);  
}
 T_NAME
from_exp  
{
  // $5
  tree decl = lookup_name ($3);
  if (decl)
	 goto skip_decl;
  // bad, but temp workaround
  tree tmp = start_decl($3, 0, 0, 0);
  finish_decl(tmp, 0, 0);
 skip_decl:
  {}
  tree count = build_external_ref($3,1);
  tree init = build_modify_expr (count, NOP_EXPR, $4);
  c_expand_expr_stmt(init);
  RECHAIN_STMTS ($<type_node_p>2, FOR_INIT_STMT ($<type_node_p>2));
}
to_exp   by_exp  K_DO exp 
{
  tree count = build_external_ref($3,1);
  tree check = parser_build_binary_op (LE_EXPR, count, $6);
  FOR_COND ($<type_node_p>2) = c_common_truthvalue_conversion (check);

  tree incr_count=$7;
  if (incr_count==0) {
	 tree f = build_int_2(1,0);
	 TREE_TYPE (f) = widest_integer_literal_type_node;
    f = convert (integer_type_node, f);
	 incr_count=f;
  }
  tree incr = build_modify_expr (count, PLUS_EXPR, incr_count);
  FOR_EXPR ($<type_node_p>2) = incr;

  tree type = integer_type_node;

  tree decl = create_temp_var();
  tree value = $9;
  tree t = value;

  if (!(TREE_CODE(t)==VAR_DECL && DECL_ARTIFICIAL (decl))) 
    t = set_temp_var(decl, value);

  c_expand_expr_stmt(t);
  RECHAIN_STMTS ($<type_node_p>2, FOR_BODY ($<type_node_p>2));
  $$=decl;
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
{ $<type_node_p>$ = c_begin_while_stmt(); }
exp
{
  tree t=$3;
  t=parser_build_binary_op(BIT_AND_EXPR,t,build_int_2(1,0));
  t = c_common_truthvalue_conversion ( t ); 
  if ($1)
	 t = build_unary_op (TRUTH_NOT_EXPR, t, 0);
 c_finish_while_stmt_cond (c_common_truthvalue_conversion ( t ),
			   $<type_node_p>2);
 $<type_node_p>$ = add_stmt ($<type_node_p>2); }
 K_DO exp  {
  RECHAIN_STMTS ($<type_node_p>4, WHILE_BODY ($<type_node_p>4));
  $$=0;
 } 
/*| K_UNTIL  exp K_DO exp { $$ = 0; }*/
;

k_while_or_until:
K_WHILE { $$ = 0; }
|K_UNTIL { $$ = 1; }
;

post_tested_loop:
K_DO 
{
  $<type_node_p>$ = add_stmt (build_stmt (DO_STMT, NULL_TREE, NULL_TREE));
  DO_COND ($<type_node_p>$) = error_mark_node; // see c-parse.y for why
}
exp k_while_or_until
{
  tree t=$3;
  t=parser_build_binary_op(BIT_AND_EXPR,t,build_int_2(1,0));
  c_expand_expr_stmt(t);
  $$ = $<type_node_p>2;
  RECHAIN_STMTS ($$, DO_BODY ($$));
}
exp 
{ 
  $$ = $<type_node_p>5;
  if ($4)
	 $6 = build_unary_op (TRUTH_NOT_EXPR, $6, 0);
  DO_COND ($$) = c_common_truthvalue_conversion ($6);
  //  c_expand_expr_stmt($$);
}/*
|
K_DO 
{
  $<type_node_p>$ = add_stmt (build_stmt (DO_STMT, NULL_TREE, NULL_TREE));
  DO_COND ($<type_node_p>$ = error_mark_node); // see c-parse.y for why
}
exp K_UNTIL
{
  $$ = $<type_node_p>2;
  RECHAIN_STMTS ($$, DO_BODY ($$));
}
exp 
{ 
  $$ = $<type_node_p>5;
  $6 = build_unary_op (BIT_NOT_EXPR, $6, 0);
  DO_COND ($$) = c_common_truthvalue_conversion ($6);
}*/
;


exit_expression:  leave_expression  
| exitloop_expression  
;
leave_expression: 
K_LEAVE T_LABELNAME K_WITH exp 
{
  // as said earlier, was T_NAME, but workaround
  tree decl;
  tree name=$2;
  decl = lookup_label(name);
  //  decl = build_decl (LABEL_DECL, name, TREE_TYPE (integer_type_node));
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
  tree newdecl=build(LABEL_EXPR, NULL_TREE, decl, NULL_TREE);

  tree label_block_expr = newdecl;
  tree t = build (EXIT_BLOCK_EXPR, NULL_TREE, label_block_expr, NULL_TREE);
  TREE_TYPE(t)=void_type_node;
  TREE_SIDE_EFFECTS (t) = 1;
  EXIT_BLOCK_RETURN(t)=c_expand_expr_stmt($4);
  $$=t;
}
|K_LEAVE T_LABELNAME  
{
  tree decl;
  tree name=$2;
  decl = lookup_label(name);
	 // build_decl (LABEL_DECL, name, TREE_TYPE (integer_type_node));
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
  tree newdecl=build(LABEL_EXPR, NULL_TREE, decl, NULL_TREE);

  tree label_block_expr = newdecl;
  tree t = build (EXIT_BLOCK_EXPR, NULL_TREE, label_block_expr, NULL_TREE);
  TREE_TYPE(t)=void_type_node;
  TREE_SIDE_EFFECTS (t) = 1;

  $$=t;

#if 0
  tree decl;
  tree name=$2;
  decl = build_decl (LABEL_DECL, name, TREE_TYPE (integer_type_node));
  DECL_CONTEXT (decl) = current_function_decl;
  DECL_IGNORED_P (decl) = 1;
#endif
}
;
exitloop_expression: 
K_EXITLOOP  exp  { $$=0; }
|K_EXITLOOP
{
  tree t = build_int_2 (0, 0);
  tree truetree = parser_build_binary_op (EQ_EXPR, t, t);
  $$ = build (EXIT_EXPR, void_type_node, truetree);
}
;

return_expression: 
/*K_RETURN  exp { $$ = c_expand_return (build_compound_expr($2)); $$=0; }*/
/*K_RETURN  exp { $$ = c_expand_return (c_expand_expr_stmt($2)); $$=0; }*/
K_RETURN exp { $$ = c_expand_return ($2); $$=0; }
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

attribute:  allocation_unit { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
| extension_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
| structure_attribute  
| field_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
| alignment_attribute  
| initial_attribute 
| preset_attribute 
| { undefmode=1; } psect_allocation{ undefmode=0; $$=$2; }
| volatile_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }  
| novalue_attribute 
| linkage_attribute  
| range_attribute 
| { undefmode=1; if (yydebug) inform("undefmode\n\n\n\n"); } addressing_mode_attribute { undefmode=0; $$=$2; } 
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
  //tree t=find_struct(mystructs,$1);
  tree t=xref_tag(STRUCTURE_TYPE,$1);
  tree size=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))))));
  tree body=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t)))))));
  tree alloc=TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))));
  tree type, body_t, size_t, access_t, comp2, access;
  size=tree_cons(0,size,0); // temp workaround for maybe changing root node
  my_substitute(size,alloc,$3);
  my_substitute(body,alloc,$3);
  my_substitute(size,alloc,-1);
  my_substitute(body,alloc,-1);
  size=TREE_VALUE(size);

  my_fold(size);
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

  $$ = build_nt (STRUCTURE_STUFF, comp2, comp2);

}
|   T_NAME  
{ 
  //$$ = build_nt(STRUCTURE_ATTR,$1,$3); 
  //tree t=find_struct(mystructs,$1);
  tree t=xref_tag(STRUCTURE_TYPE,$1);
  tree size=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))))));
  tree body=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t)))))));
  tree alloc=TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))));
  tree type, body_t, size_t, access_t, comp2, access;
  //my_substitute(size,alloc,$3);
  //my_substitute(body,alloc,$3);
  my_substitute(body,alloc,-1);
  my_fold(size);
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

  $$ = build_nt (STRUCTURE_STUFF, comp2, comp2);
}
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
linkage_time_constant_expression:expression { /*nonfin*/ }
;

compile_time_constant_expression: expression { /*nonfin*/ }
;

addressing_mode_attribute: 
/*K_ADDRESSING_MODE { undefmode=1; } '(' mode_16 ')' 
  |*/{ undefmode=1; fprintf(stdout, "undefmode\n\n\n\n"); } K_ADDRESSING_MODE '('  mode_32 ')' {  undefmode=0; }
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
  $$=build_int_2(0,0);
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
  if (i)
    $$=build_int_2(i,0);
}
|extension_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }  
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
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,integer_type_node,0);
  tree size=tree_cons(0,integer_type_node,0);

  tree type = integer_type_node;
  tree myattr = $2;
  
  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    size=handle_structure(cell, st_attr, 1);
  }

  init = find_init_attr(myattr);
  if (init)
	 init = TREE_OPERAND(init, 0);

  int do_init = 0;
  if (init)
	 do_init=1;

  tree pres2 = find_tree_code(myattr, PRESET_ATTR); 
  if (pres2) {
    do_init=1;
  }

  cell_decl_p = start_decl (cell, mysize, do_init, 0);
  TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

  start_init(cell_decl_p,NULL,global_bindings_p());
  finish_init();

  tree pres = find_tree_code(myattr, PRESET_ATTR);
  if (pres) {
    init = handle_preset($1, pres, cell_decl_p, fold(size));
  } else {
    if (st_attr)
      TREE_TYPE(cell_decl_p)=build_our_record(fold(size));
  }

  finish_decl (cell_decl_p, init, NULL_TREE);
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
allocation_unit { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
|extension_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }
|structure_attribute 
|field_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
|alignment_attribute 
|initial_attribute 
|preset_attribute 
|psect_allocation 
|volatile_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }  
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
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,integer_type_node,0);
  tree size=tree_cons(0,integer_type_node,0);

  tree type = integer_type_node;
  tree myattr = $2;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    size=handle_structure(cell, st_attr, 1);
  }

  init = find_init_attr(myattr);
  if (init)
	 init = TREE_OPERAND(init, 0);

  int do_init = 0;
  if (init)
	 do_init=1;

  tree pres2 = find_tree_code(myattr, PRESET_ATTR); 
  if (pres2) {
    do_init=1;
  }

  cell_decl_p = start_decl (cell, mysize, do_init, 0);

  start_init(cell_decl_p,NULL,1);
  finish_init();

  tree pres = find_tree_code(myattr, PRESET_ATTR);
  if (pres) {
    init = handle_preset($1, pres, cell_decl_p, fold(size));
  } else {
    if (st_attr)
      TREE_TYPE(cell_decl_p)=build_our_record(fold(size));
  }

  finish_decl (cell_decl_p, init, NULL_TREE);
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
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,integer_type_node,0);
  tree size=tree_cons(0,integer_type_node,0);

  tree type = integer_type_node;
  tree myattr = $2;
  
  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    /*size=*/handle_structure(cell, st_attr, 1);
    cell=make_pointer_declarator(0,cell); // check?
  }

  cell_decl_p = start_decl (cell, tree_cons(0, integer_type_node, 0), 0, 0);
  DECL_EXTERNAL (cell_decl_p) = 1; // differs from bind here
  //TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

#if 0
  // not like bind here
  start_init(cell_decl_p,NULL,global_bindings_p());
  finish_init();
  finish_decl (cell_decl_p, 0, NULL_TREE);
  TREE_LANG_FLAG_0($1)=1;
#else
  finish_decl (cell_decl_p, 0, NULL_TREE);
#endif
}
;

forward_declaration: K_FORWARD forward_item_list ';'  { $$ = 0; }
;

forward_name:
T_NAME
;

forward_item: forward_name ':' attribute_list 
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
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,integer_type_node,0);
  tree size=tree_cons(0,integer_type_node,0);

  tree type = integer_type_node;
  tree myattr = $2;

  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    size=handle_structure(cell, st_attr, 1);
  }

  init = find_init_attr(myattr);
  if (init)
	 init = TREE_OPERAND(init, 0);

  int do_init = 0;
  if (init)
	 do_init=1;

  tree pres2 = find_tree_code(myattr, PRESET_ATTR); 
  if (pres2) {
    do_init=1;//diff here
  }

  cell_decl_p = start_decl (cell, mysize, do_init, 0);

  start_init(cell_decl_p,NULL,global_bindings_p());
  finish_init();

  tree pres = find_tree_code(myattr, PRESET_ATTR);
  if (pres) {
    init = handle_preset($1, pres, cell_decl_p, fold(size));
  } else {
    if (st_attr)
      TREE_TYPE(cell_decl_p)=build_our_record(fold(size));
  }

  finish_decl (cell_decl_p, init, NULL_TREE);
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

map_item: map_name ':' attribute_list 
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

#ifdef NEW_POINTER
  turn_off_addr_expr = 1;
#endif

  pushlevel(0);
#if 1
  int_tree = tree_cons(0,integer_type_node,0);
  parm_tree = tree_cons(int_tree, $1, 0); 
  $$ = tree_cons (parm_tree, 0, 0);
  push_parm_decl($$); 
#if 0
  x = get_parm_info (1);
  TREE_ADDRESSABLE (TREE_PURPOSE(x)) = 1;
#endif
#endif
}
  access_formal_list  ';' 
{
  // $6

  tree accessid = get_identifier(add_underscore($1, 2));
  tree accessls = $4;
  tree accessfn = build_nt (CALL_EXPR, accessid, accessls, NULL_TREE);

  //push_parm_decl($<type_node_p>3);
  //mydeclares($4);
  //begin_stmt_tree(&$<type_node_p>$);
  //$<type_node_p>6=accessfn;

  start_function (tree_cons(0, integer_type_node, 0), accessfn, 0);
  store_parm_decls ();
  afun=current_function_decl;

  $$=accessfn;
  pushlevel(0);
  pushlevel(0);

}
allocation_formal_list ']' '='
{
  // $10

  tree allocform_list = $7;
  tree allocid = get_identifier(add_underscore($1, 1));
  tree allocfn = build_nt (CALL_EXPR, allocid, allocform_list, NULL_TREE);

  //mydeclares($7);

  acfun=cfun;
  pushlevel(0);
  start_function (tree_cons(0, integer_type_node, 0), allocfn, 0);
  store_parm_decls ();

  //begin_stmt_tree(&$<type_node_p>$);
  //$<type_node_p>12=allocfn;

  $$=allocfn;
}
structure_size
{
  // $12

  tree accessfn = $<type_node_p>6;

  finish_function (); 

  current_function_decl=afun;
  cfun=acfun;
  //begin_stmt_tree (&tr);
  begin_stmt_tree (&DECL_SAVED_TREE (current_function_decl));

  $$=accessfn;
}
structure_body
{
  //$14

  tree comp;  
  tree allocer;
  tree accesser;
  tree prev;
  tree tmp;
  tree accesstmp;
  tree body;
  tree size;
  tree alloc;
  tree access;
  tree comp2;

  //DECL_SAVED_TREE (current_function_decl)=$13;
  poplevel(0,0,0);
  finish_function (); 
  poplevel(0,0,0);
  poplevel(0,0,0);
  poplevel(0,0,0);

#if 1
  allocer = grokfield ($<type_node_p>10, 0, 0);
  accesser = grokfield ($<type_node_p>12, 0, 0);
#else
  allocer = $<type_node_p>10;
  accesser = $<type_node_p>12;
#endif

  //$$ = finish_structure ( $<type_node_p>3, 0, $4, $7 ,$11, $13, 0);

  comp=allocer;
  chainon(allocer,accesser);

  prev=accesser;

  if (!$7) goto none;
  for(tmp=$7->list.purpose;tmp;tmp=TREE_CHAIN(tmp)) {
    tree decl_name = DECL_NAME(tmp);
#if 1
    tree grok = grokfield (decl_name, 0, 0);;
#else
    tree grok = decl_name;
#endif

    chainon(prev, grok);
    prev=grok;
  }

  none:

//  $$ = finish_struct ($<type_node_p>3, comp, NULL);

  $$=0;

  accesstmp = parm_first_to_last($4);

  body = tree_cons(0, $13, 0);
  size = tree_cons(0, $11, body);
  alloc = tree_cons(0, $7, size);
  access = tree_cons(0, /*$4*/accesstmp, alloc);
  comp2 = tree_cons(0, 0, access);

  $$ = start_structure (STRUCTURE_TYPE, $1);
  $$ = finish_structure ($$, comp2, 0, 0, 0, 0, 0); 

  //$$ = build_nt (STRUCTURE_DECL, $1, $4, $7, $11, $13);
  //add_struct(&mystructs,$$); 
#ifdef NEW_POINTER
  turn_off_addr_expr = 0;
#endif
}
;

allocation_formal_list:
allocation_formal_list ',' allocation_formal 
{ 
//$$ = tree_cons (NULL_TREE, $1, $3);
  $$ = get_parm_info (1);
  //$$=chainon ($1, $3); 
}
|allocation_formal { 
$$ = get_parm_info (1);
//$$=$1; 
 }
;

allocation_formal:
allocation_name { 
#if 1
  tree int_tree = tree_cons (0, integer_type_node, 0);   
  tree parm_decl = tree_cons (int_tree, $1, 0);
 $$ = tree_cons (parm_decl, 0, 0);
 push_parm_decl($$);
#else
  tree e=tree_cons(0,integer_type_node,0);
  tree f=tree_cons(e,$1,0); 
  $$ = tree_cons (f,0,0);
#endif
}
|allocation_name '=' allocation_default { 
  //$$ = tree_cons ($3, $1, NULL_TREE); 
  tree t = tree_cons (0, integer_type_node, 0);   
  tree d = tree_cons (t, $1, 0);
 $$ = tree_cons (d, 0, 0);
 push_parm_decl_init($$, $3);
}
;

allocation_default: exp 
;

structure_size: { $$ = 0; /* empty */ }
|'[' expression ']' { $$=$2; }

/*
structure_size: { $$ = 0; }
|
{
  //  $<type_node_p>1=c_begin_compound_stmt ();
  compstmt_count++;
  $$=c_begin_compound_stmt ();
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
*/

structure_body:
  expression;

/*
structure_body: 
{
  // $<type_node_p>1 = c_begin_compound_stmt;
  compstmt_count++;
  $$ = c_begin_compound_stmt();
}
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
;
*/

access_formal_list: access_formal_list ',' access_formal { 
  $$ = get_parm_info (1);
  //$$=chainon($1,$3); 
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
  tree int_tree = tree_cons(0, integer_type_node, 0);
  tree parm_tree = tree_cons(int_tree, $1, 0); 
  $$ = tree_cons (parm_tree, 0, 0);
  push_parm_decl($$); 
#else
  tree e=tree_cons(0,integer_type_node,0);
  tree f=tree_cons(e,$1,0); 
  $$ = tree_cons (f,0,0);
#endif
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
io_list 
{
  // mark this as static?
  void * io_list = $3;
  void * fn;
  if (io_list==0) io_list=build_tree_list (NULL_TREE, NULL_TREE);
  // check. why CALL_EXPR?
  fn = build_nt (CALL_EXPR, $1, io_list, NULL_TREE);
  start_function (tree_cons(0, integer_type_node, 0), fn, 0);
  store_parm_decls ();
}
routine_attributes '=' save_location exp 
{ 
  tree block_value = $8; 
  if (block_value) $$ = c_expand_return (build_compound_expr(build_tree_list(NULL_TREE,$8))); $$=0;
  finish_function (); 
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
  pushlevel (0);
  // check: needs mark_forward_parm_decls (); maybe?
  declare_parm_level ();
}
'(' formal_item_list ')'
{ 
  poplevel (0, 0, 0);
  $$ = $3;
}
/*  |'(' formal_item_list ';' formal_item_list ')' 
    |'(' ';' formal_item_list ')' */
;

formal_item_list:
/* empty 6 */ { $$ = 0 ; } 
|formal_item_list ','  formal_item { $$ = get_parm_info (1); }
|formal_item { $$ = get_parm_info (1);  }
;

formal_name:
T_NAME
;

formal_item: /*T_NAME ':' formal_attribute_list 
|*/
formal_name
{ 
  tree type = integer_type_node;
  tree int_tree = tree_cons (NULL_TREE, type, NULL_TREE);
  tree point_tree = make_pointer_declarator (0, $1);

  tree point_int = tree_cons (int_tree, point_tree, 0);

  $$ = tree_cons (point_int, 0, 0);

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
|volatile_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }  
;
routine_attribute: novalue_attribute 
|linkage_attribute 
|psect_allocation 
|addressing_mode_attribute { $$ = 0; }
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
io_list 
{
  void * io_list = $3;
  void * fn;
  if (io_list==0) io_list=build_tree_list (NULL_TREE, NULL_TREE);
  fn = build_nt (CALL_EXPR, $1, io_list, NULL_TREE);
  start_function (tree_cons(0, integer_type_node, 0), fn, 0);
  store_parm_decls ();
}
global_routine_attributes '=' save_location exp 
{ 
  tree block_value = $8; 
  if (block_value) $$ = c_expand_return (build_compound_expr(build_tree_list(NULL_TREE,$8))); $$=0;
  finish_function (); 
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

routine_name:
T_NAME
;

forward_routine_item: 
routine_name maybe_forward_routine_attribute_list 
/*|T_NAME  */
{
  tree decl = build_decl (FUNCTION_DECL, $1, default_function_type);
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

maybe_ext_routine_attribute_list:
|
':' ext_routine_attribute_list
;

external_routine_item: routine_name maybe_ext_routine_attribute_list 
{ 
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
  char *new=xstrdup(TREE_STRING_POINTER($2)+1);
  new[strlen(new)-1]=0;
  char * colon = strchr(new,':');
  if (colon)
	 new=colon+1;
  char * brack = strchr(new,']');
  if (brack)
	 new=brack+1;
  char * new2 = my_strcat(new,".req",0);
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
  char *new=xstrdup(TREE_STRING_POINTER($2)+1);
  new[strlen(new)-1]=0;
  char * colon = strchr(new,':');
  if (colon)
	 new=colon+1;
  char * brack = strchr(new,']');
  if (brack)
	 new=brack+1;
  char * new2 = my_strcat(new,".req",0);
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
  lookup_label($1);
}
;

label_declaration:
K_LABEL label_name_list ';'
{
  $$ = 0;
}
;

builtin_declaration: K_BUILTIN tname_list ';' { $$ = 0; }
 ;
undeclare_declaration: K_UNDECLARE tname_list ';' { $$ = 0; }
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
  tree mysize=tree_cons(0,integer_type_node,0);
  tree size=tree_cons(0,integer_type_node,0);

  tree type = integer_type_node;
  
  cell=$1;
  TREE_TYPE(cell)=integer_type_node;

#if 0
  cell_decl_p = start_decl (cell, current_declspecs, 0, 0);
  TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?
  //printf("xxx %x\n",d);
  finish_decl (cell_decl_p, 0, NULL_TREE);
#else
  $$ = build_enumerator($1, $3);
  TREE_LANG_FLAG_0($1)=1;
#endif
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
  tree decl = build_decl (CONST_DECL, $1, integer_type_node);
  DECL_EXTERNAL(decl)=1;
  DECL_INITIAL(decl)=build_int_2(0,0); // workaround. should not be.
  pushdecl(decl);
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
  tree cell, decl_p , cell_decl, init, t, cell_decl_p;
  tree mysize=tree_cons(0,integer_type_node,0);
  tree size=tree_cons(0,integer_type_node,0);

  tree type = integer_type_node;
  tree myattr = $4;
  
  cell=$1;

  tree st_attr = find_structure_attr(myattr);
  if (st_attr) {
    /*size=*/handle_structure(cell, st_attr, 1);
    size=handle_structure(cell, st_attr, 1);
  }

  init = $3;

  if (TREE_CODE(init)==BIT_FIELD_REFS) {
    init=TREE_OPERAND(init, 2);
    init=TREE_OPERAND(init, 0);
    my_fold(init);
  }
  TREE_CONSTANT(init)=1;

  cell_decl_p = start_decl (cell, tree_cons(0, integer_type_node, 0), 1, 0);
  //TREE_STATIC(cell_decl_p)=1; // same as local, except for STATIC?

  start_init(cell_decl_p,NULL,global_bindings_p());
  finish_init();
  finish_decl (cell_decl_p, init, NULL_TREE);
  TREE_LANG_FLAG_0($1)=1;
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
allocation_unit  { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
|extension_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }  
|structure_attribute 
|field_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); } 
|volatile_attribute { $$ = tree_cons(NULL_TREE, $1, NULL_TREE); }  
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
T_NAME '(' tname_list ')'  '=' { macromode=1; } macro_body '%' 
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
T_NAME '(' keyword_pair_list ')' '=' { macromode=1; } macro_body '%' 
{ 
  add_macro(IDENTIFIER_POINTER($1),KEYW_MACRO,$3,0,$7);
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
T_NAME '=' 
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
T_NAME 
{
  $$ = build_tree_list(0, $1);
  //$$ = build_nt(KEYWORD_PAIR, $1, 0);
}
;

linkage_definition_list: linkage_definition_list ',' linkage_definition 
|linkage_definition 
;
linkage_definition: linkage_name '=' linkage_type linkage_type_stuff maybe_linkage_option
;

linkage_type: U_CALL  { $$ = 0; }
| T_NAME 
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
'(' K_REGISTER '=' T_DIGITS ')' { $$ = 0; }
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

psect_attribute_list: psect_attribute_list ',' psect_attribute 
|psect_attribute { $$ = 0; }
;

psect_item:
storage_class '=' T_NAME
{ 
  undefmode=1;
}
'(' psect_attribute_list ')'  
{ 
  undefmode=0;
  $$ = $3;
}
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

compile_time_item_list: compile_time_item_list ',' compile_time_item
| compile_time_item
;

compile_time_item:
compile_time_name '=' compile_time_value
{
  if (!quiet_flag) printf("k_com %s %d %x\n",IDENTIFIER_POINTER($1),input_location.line,$3);
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

linkage_function: T_NAME
;

linkage_function_name: T_NAME 
;

pushlevel:  /* empty 7 */
{
  if (cfun==0)
	 goto skip_pushlevel;
  tree scope;
  pushlevel (0);
  clear_last_expr ();
  scope = add_scope_stmt (/*begin_p=*/1, /*partial_p=*/0);
  //  if (TREE_TYPE(TREE_PURPOSE(scope))==0) TREE_TYPE(TREE_PURPOSE(scope))=integer_type_node;
 skip_pushlevel:
}
;

poplevel:  /* empty 8 */
{ 
  if (cfun==0)
	 goto skip_poplevel;
  $$ = add_scope_stmt (/*begin_p=*/0, /*partial_p=*/0);
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
  error("Nu b;lev det fel %d\n",input_location.line);
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
  error("Nu b;lev det fel %d\n",input_location.line);
}

void
maybe_apply_pragma_weak(int a) {
}

tree
maybe_apply_renaming_pragma(tree decl, tree asmname) {
  return asmname;
}

static void init_reswords ();

void
c_parse_init(void) {
  init_reswords ();

parse_init();
}

int
cpp_handle_option (pfile, argc, argv)
     cpp_reader *pfile;
     int argc;
     char **argv;
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
cpp_create_reader (lang, ht)
     enum c_lang lang; struct ht * ht;
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

void
cpp_assert (pfile, str)
     cpp_reader *pfile;
     const char *str;
{
}

void
init_c_lex ()
{
}

void
init_pragma ()
{
}

int
cpp_finish (pfile,deps_stream)
     cpp_reader *pfile;
     FILE *deps_stream;
{
}

void
cpp_define (pfile, str)
     cpp_reader *pfile;
     const char *str;
{
}

/* The options structure.  */
cpp_options *
cpp_get_options (pfile)
     cpp_reader *pfile;
{
#if 0
  return &pfile->opts;
#endif
}

/* The callbacks structure.  */
cpp_callbacks *
cpp_get_callbacks (pfile)
     cpp_reader *pfile;
{
#if 0
  return &pfile->cb;
#endif
}

void
cpp_add_dependency_target (pfile, target, quote)
     cpp_reader *pfile;
     const char *target;
     int quote;
{
}

void
cpp_set_lang (pfile, lang)
     cpp_reader *pfile;
     enum c_lang lang;
{
#if 0
  const struct lang_flags *l = &lang_defaults[(int) lang];

  CPP_OPTION (pfile, lang) = lang;

  CPP_OPTION (pfile, c99)                = l->c99;
  CPP_OPTION (pfile, cplusplus)          = l->cplusplus;
  CPP_OPTION (pfile, extended_numbers)   = l->extended_numbers;
  CPP_OPTION (pfile, std)                = l->std;
  CPP_OPTION (pfile, trigraphs)          = l->std;
  CPP_OPTION (pfile, dollars_in_ident)   = l->dollars_in_ident;
  CPP_OPTION (pfile, cplusplus_comments) = l->cplusplus_comments;
  CPP_OPTION (pfile, digraphs)           = l->digraphs;
#endif
}

void
cpp_preprocess_file (pfile, in_fname, out_stream)
     cpp_reader *pfile;
     const char *in_fname;
     FILE *out_stream;
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

tree
objc_is_object_ptr (tree arg ATTRIBUTE_UNUSED)
{
  return 0;
}

tree
lookup_objc_ivar (tree arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
objc_check_decl (tree decl ATTRIBUTE_UNUSED)
{
}
   
int
objc_comptypes (tree lhs ATTRIBUTE_UNUSED, tree rhs ATTRIBUTE_UNUSED,
                int reflexive ATTRIBUTE_UNUSED)
{ 
  return -1;
}

tree
objc_message_selector (void)
{ 
  return 0;
}

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
    if (*c == '\\') *c = '/';
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

#if 0
/* Pass an object-like macro and a value to define it to.  The third
   parameter says whether or not to turn the value into a string
   constant.  */
void
builtin_define_with_value (const char *macro, const char *expansion, int is_str)
{
  char *buf;
  size_t mlen = strlen (macro);
  size_t elen = strlen (expansion);
  size_t extra = 2;  /* space for an = and a NUL */

  if (is_str)
    extra += 2;  /* space for two quote marks */

  buf = alloca (mlen + elen + extra);
  if (is_str)
    sprintf (buf, "%s=\"%s\"", macro, expansion);
  else
    sprintf (buf, "%s=%s", macro, expansion);

  cpp_define (parse_in, buf);
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
      C_RID_CODE (id) = reswords[i].rid;
      C_IS_RESERVED_WORD (id) = 1;
      ridpointers [(int) reswords[i].rid] = id;
    }
}

static tree
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

tree
my_copy  (tp, walk_subtrees, data) 
     tree *tp;
     int *walk_subtrees;
     void *data;
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
my_copy_tree (node)
     tree node;
{
  tree ret = node;
  walk_tree (&ret, my_copy, 0, 0);
  //  ret = copy_node(node);
  //  if (TREE_CHAIN(node))
  //    TREE_CHAIN(ret)=my_copy_tree(TREE_CHAIN(node));
  return ret;
}

tree
my_substitute_fn  (tp, walk_subtrees, data) 
     tree *tp;
     int *walk_subtrees;
     void *data;
{
  long *l=data;
  tree old=l[0];
  tree new=l[1];
  //  copy_tree_r (tp, walk_subtrees, NULL);
  int i;
  if (DECL_P(*tp)) {
	 if (l[1]==-1)
		goto other_minus;
    for(old=TREE_PURPOSE(old);old && new;old=TREE_CHAIN(old),new=TREE_CHAIN(new)) {
      if (DECL_NAME(*tp)==DECL_NAME(old)) {
		  *tp=TREE_VALUE(new);
		  goto subst_out;
      }
    }
	 return NULL_TREE;
  other_minus:
    for(old=TREE_PURPOSE(old);old;old=TREE_CHAIN(old)) {
      if (DECL_NAME(*tp)==DECL_NAME(old)) {
		  if (TREE_CODE(DECL_RESULT(old))==INTEGER_CST)
			 *tp=copy_node(DECL_RESULT(old));
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
my_substitute (mytree, old, new)
  tree mytree, old, new;
{
  long l[2];
  l[0]=old;
  l[1]=new;
  walk_tree (&mytree, my_substitute_fn, l, 0);
}

tree
my_do_parm(t)
     tree t;
{
  if (t==0) return 0;
  return tree_cons(0, build_decl (STRUCTURE_DECL, t, 0)  , my_do_parm(TREE_CHAIN(t)));
}

tree
my_fold_fn  (tp, walk_subtrees, data) 
     tree *tp;
     int *walk_subtrees;
     void *data;
{
  *tp=fold(*tp);

  /* Keep iterating.  */
  return NULL_TREE;

}

void
my_fold (mytree)
  tree mytree;
{
  walk_tree (&mytree, my_fold_fn, 0, 0);
}

tree
first_to_last(mytree)
  tree mytree;
{
  tree first=mytree;
  tree second=TREE_CHAIN(first);
  TREE_CHAIN(first)=0;
  chainon(tree_last(second),first);
  return second;
}

tree
parm_first_to_last(mytree)
  tree mytree;
{
  tree first=mytree;
  TREE_PURPOSE(first)=first_to_last(TREE_PURPOSE(first));
  return first;
}

void
add_macro(char * name, int type, tree param, tree param2, tree body) {
  tree i = get_identifier(name);
  tree t = build_nt(MACRO_DEF, type, param, param2, body);
  pushtag(i, t);
}

void *
find_macro(struct mymacro * s,char * name) {
  tree id=maybe_get_identifier(name);

  if (id==0)
    return 0;

  tree tag = IDENTIFIER_TAG_VALUE (id);

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

char *
make_macro_string(m,r)
     struct mymacro * m;
     tree r;
{
  char * s=0;
  tree b = m->body;
  tree p = m->param;
  tree t;
  int is_quote=0;
  if (m->type!=SIMP_MACRO && m->type!=KEYW_MACRO)
	 goto other_macro;
  for(t=b;t;t=TREE_CHAIN(t)) {
    tree old,new;
    char * l = TREE_STRING_POINTER(t);
	 if (TREE_CODE(t)==IDENTIFIER_NODE)
		l=IDENTIFIER_POINTER(t); // workaround in case %remaining IDENTIFIER
	 // check. not quite sure about doing quoting here
	 if (is_quote) {
		is_quote=0;
		goto subst_out;
	 }
	 is_quote=(0==strcasecmp(l,"%quote"));
	 if (is_quote)
		continue;
	 if (m->type==KEYW_MACRO)
		goto keyw_macro;
    for(old=p,new=r;old && new;old=TREE_CHAIN(old),new=TREE_CHAIN(new)) {
      if (0==strcmp(l,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
		  l=TREE_STRING_POINTER(new);
		  goto subst_out;
      }
    }
	 goto subst_out;
  keyw_macro:
    for(new=r;new;new=TREE_CHAIN(new)) {
		//fprintf(stderr,"IDD3 %x %x %x\n",l,TREE_TYPE(new),TREE_STRING_POINTER(new));
      if (0==strcmp(l,TREE_STRING_POINTER(new))) {
		  if (TREE_TYPE(new))
			 l=TREE_STRING_POINTER(TREE_TYPE(new));
		  goto subst_out;
      }
    }
    for(old=p;old;old=TREE_CHAIN(old)) {
		//printf("IDD4 %x %x %x\n",l,TREE_VALUE(old),TREE_PURPOSE(old));
      if (0==strcmp(l,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
	if (TREE_PURPOSE(old)) {
	  l=TREE_STRING_POINTER(TREE_PURPOSE(old));
	  goto subst_out;
	} else {
	  l=" ";
	  goto subst_out;
	}
      }
    }
  subst_out:
    s=my_strcat_gen(s,l,1);
  }
  
  if (yydebug) inform ("\n%%BLS-I-NOTHING %x line macro expanded to %s\n",input_location.line,s);

  return s;
 other_macro:
  if (m->type==COND_MACRO)
	 goto cond_macro;
  extern int cond_iter_macro_count;
  cond_iter_macro_count=0;
  tree iter=m->param2;
  tree new=r;
  tree tmp=p;
  for(;new && tmp;new=TREE_CHAIN(new),tmp=TREE_CHAIN(tmp)) ;
  while (new) {
	 tree par=iter;
	 //printf("new %x\n",new);
	 tree t;
	 for(t=b;t;t=TREE_CHAIN(t)) {
		char * l = TREE_STRING_POINTER(t);
		tree new2=new;
		tree par2=par;
		for(;new2 && par2;new2=TREE_CHAIN(new2),par2=TREE_CHAIN(par2)) {
#if 0
		  // check. not quite sure about doing quoting here
		  if (is_quote) {
			 is_quote=0;
			 goto subst_out2;
		  }
		  is_quote=(0==strcasecmp(l,"%quote"));
		  if (is_quote)
			 continue;
#endif
		  if (0==strcmp(l,IDENTIFIER_POINTER(TREE_VALUE(par2)))) {
			 l=TREE_STRING_POINTER(new2);
			 goto subst_out2;
		  }
		}
		tree old3, new3;
		for(old3=p,new3=r;old3 && new3;old3=TREE_CHAIN(old3),new3=TREE_CHAIN(new3)) {
		  if (0==strcmp(l,IDENTIFIER_POINTER(TREE_VALUE(old3)))) {
			 //fprintf(stderr,"CMP3 %s %s %s %x %x\n",l,IDENTIFIER_POINTER(TREE_VALUE(old3)),TREE_STRING_POINTER(new3),TREE_STRING_POINTER(new3),new3);
			 l=TREE_STRING_POINTER(new3);
			 goto subst_out2;
		  }
		}
	 subst_out2:
		s = my_strcat_gen(s,l,1);
	 } // end outer for
	 for(tmp=iter;new && tmp;new=TREE_CHAIN(new),tmp=TREE_CHAIN(tmp)) ;
	 if (new)
		s = my_strcat(s,",",0); // remember to fix punctuation later
	 cond_iter_macro_count++;
  } // end while
  if (!quiet_flag) printf("ITER %x\n",s);
  if (!quiet_flag) printf("ITER %s\n",s);
  cond_iter_macro_count=0; // reset it here because cannot in cond itself?
  return s;
 cond_macro:
  // this seems to be done just like a simple macro?
  // except for using cond_iter_macro_count? but might use it wrongly?
  {}
  extern int cond_iter_macro_count;
  //cond_iter_macro_count=0; // cannot do this here?
  for(t=b;t;t=TREE_CHAIN(t)) {
    tree old,new;
    char * l = TREE_STRING_POINTER(t);
	 if (TREE_CODE(t)==IDENTIFIER_NODE)
		l=IDENTIFIER_POINTER(t); // workaround in case %remaining IDENTIFIER
	 // check. not quite sure about doing quoting here
	 if (is_quote) {
		is_quote=0;
		goto subst_out3;
	 }
	 is_quote=(0==strcasecmp(l,"%quote"));
	 if (is_quote)
		continue;
    for(old=p,new=r;old && new;old=TREE_CHAIN(old),new=TREE_CHAIN(new)) {
      if (0==strcmp(l,IDENTIFIER_POINTER(TREE_VALUE(old)))) {
		  l=TREE_STRING_POINTER(new);
		  goto subst_out3;
      }
    }
	 goto subst_out3;
  subst_out3:
	 s = my_strcat_gen(s,l,1);
  }
  
  if (yydebug) inform ("\n%%BLS-I-NOTHING %x cond line macro expanded to %s\n",input_location.line,s);

  cond_iter_macro_count++;

  return s;
}

char *
print_remain(r)
     tree r;
{
  char * s=0;
  tree t;
  for(t=r;t;t=TREE_CHAIN(t)) {
    tree old,new;
    char * l = TREE_STRING_POINTER(t);
    s=my_strcat_gen(s,l,0);
    if (TREE_CHAIN(t))
      s=my_strcat_gen(s,",",0);
  }
  
  //if (yydebug) inform ("\n%%BLS-I-NOTHING %x line macro expanded to %s\n",input_location.line,s);

  return s;
}

char *
add_counted_string(t,n)
     tree t;
     int n;
{
    char * ss=xmalloc(IDENTIFIER_LENGTH(t)+1);
    strcpy(1+ss,IDENTIFIER_POINTER(t));
    ss[IDENTIFIER_LENGTH(t)+1]=0;
	 ss[0]=n;
    return ss;
}

char *
add_underscore(t,n)
     tree t;
     int n;
{
    char * ss=xmalloc(IDENTIFIER_LENGTH(t)+n+1);
    strcpy(ss,IDENTIFIER_POINTER(t));
    ss[IDENTIFIER_LENGTH(t)+n]=0;
    while (n--)
      ss[IDENTIFIER_LENGTH(t)+n]='_';
    return ss;
}

char *
add_percent(t,n)
     tree t;
     int n;
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
    poplevel (0, 0, 0);
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

int mylast() {
  return last_expr;
}

#if 0
#include "gt-c-parse.h"
#endif

const struct ggc_root_tab gt_ggc_r_gt_c_parse_h[] = {
  LAST_GGC_ROOT_TAB
};

const struct ggc_root_tab gt_ggc_r_gt_c_pragma_h[] = {
  LAST_GGC_ROOT_TAB
};

int check_little_endian() {
  short sh = 0x1234;
  char * c = &sh;
  return (*c==0x34);
}

char * bliss_builtin = "MACRO %BLISS16[] = % , %BLISS36[] = % , %BLISS32[] = %REMAINING % ; ";

void get_builtin(void) {
  // change from SIMP_MACRO later
  add_macro("%bliss16",SIMP_MACRO,0,0,0);
  add_macro("%bliss36",SIMP_MACRO,0,0,0);
  add_macro("%bliss32",SIMP_MACRO,0,0,get_identifier("%remaining"));

  predef_literal("%bpval",32);
  predef_literal("%bpunit",8);
  predef_literal("%bpaddr",32);
  predef_literal("%upval",4);

#if 0
  do_builtin=1;

  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(bliss_builtin));

  yyparse();

  do_builtin=0;
#endif
}

// See language reference 11.10

char * bliss_builtin_struct_1 = "structure vector[i; n, unit=4, ext=0] = [n*unit] (vector + i*unit)<0,8*unit,ext>;";

//char * bliss_builtin_struct_1 = "structure vector[i; n, unit=%upval, ext=0] = [n*unit] (vector+i*unit)<0,%bpunit*unit,ext>;";

char * bliss_builtin_struct_2 = "structure bitvector[i; n] = [(n+7)/8] bitvector<i,1>;";

char * bliss_builtin_struct_3 = "structure block[o, p, s, e; bs, unit=4] = [bs*unit] (block+o*unit)<p,s,e>;";

char * bliss_builtin_struct_4 = "structure blockvector[i, o, p, s, e; n, bs, unit=4] = [n*bs*unit] (blockvector+(i*bs+o)*unit)<p,s,e>;";

// was bblock. starlet lets $$block be block_byte
char * bliss_builtin_struct_5 = "structure block_byte [o, p, s, e; n] = [n] (block_byte + o) <p, s, e>;";

void add_builtin(void) {
  parse_this(bliss_builtin_struct_1);
  parse_this(bliss_builtin_struct_2);
  parse_this(bliss_builtin_struct_3);
  parse_this(bliss_builtin_struct_4);
  parse_this(bliss_builtin_struct_5);

  if (check_little_endian()) // had to do some of my own extensions
    parse_this("compiletime $cpu_le = 1;");
  else
    parse_this("compiletime $cpu_le = 0;");
}

int
is_on_remaining(c,r)
     char * c;
     tree r;
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
}

find_field(char * s) {
  struct field_struct * t = field_root;
  for (;t;t=t->next) {
	 if (0==strcmp(t->name,s))
		return t->t;
  }
  return 0;
}

char *
print_tree(r)
     tree r;
{
  char * s=0;
  tree t;
  for(t=r;t;t=TREE_CHAIN(t)) {
    tree old,new;
    char * l = IDENTIFIER_POINTER(t);
    if (s==0) s=xstrdup(l);
    else {
      s=my_strcat(s,l,0);  
      s=my_strcat(s,",",0);
    }
	 s[strlen(s)-1]=0;
  }
  
  //if (yydebug) inform ("\n%%BLS-I-NOTHING %x line macro expanded to %s\n",input_location.line,s);

  return s;
}

void
predef_literal(name, value)
	  char * name;
	  int value;
{
  tree d1 = get_identifier(name);
  tree d3 = build_int_2(value,0);
  build_enumerator(d1, d3);
  TREE_LANG_FLAG_0(d1)=1;
}

tree
find_tree_code(t,c)
	  tree t;
	  int c;
{
  for(;t;t=TREE_CHAIN(t))
	 if (TREE_CODE(TREE_VALUE(t))==c)
		return TREE_VALUE(t);
  return 0;
}

tree
find_init_attr(t)
	  tree t;
{
  return find_tree_code(t, INIT_ATTR);
}

tree
find_structure_attr(t)
	  tree t;
{
  return find_tree_code(t, STRUCTURE_STUFF);
}

tree
is_label(t)
     tree t;
{
  if (t==0)
    return t;
  return IDENTIFIER_LABEL_VALUE(t);
}

tree
is_symbol(t)
     tree t;
{
  if (t==0)
    return t;
  return IDENTIFIER_SYMBOL_VALUE(t);
}

tree
is_tag(t)
     tree t;
{
  if (t==0)
    return t;
  return IDENTIFIER_TAG_VALUE(t);
}

tree
is_something(t)
     tree t;
{
  if (t==0)
    return t;
  return IDENTIFIER_SYMBOL_VALUE(t)||IDENTIFIER_TAG_VALUE(t)||IDENTIFIER_LABEL_VALUE(t);
}

char *
my_strcat(str1, str2, space)
	  const char * str1;
	  const char * str2;
	  int space;
{
  if (space)
	 space=1;
  int len1=strlen(str1);
  int len2=strlen(str2);
  char * str = xmalloc(len1+len2+1+space);
  memcpy(str,str1,len1);
  if (space)
	 str[len1]=32;
  memcpy(str+len1+space,str2,len2);
  str[len1+len2+space]=0;
  return str;
}

char *
my_strcat_gen(str1, str2, space)
	  const char * str1;
	  const char * str2;
	  int space;
{
  if (str1==0)
	 return xstrdup(str2);
  if (space)
	 space=1;
  int len1=strlen(str1);
  int len2=strlen(str2);
  char * str = xmalloc(len1+len2+1+space);
  memcpy(str,str1,len1);
  if (space)
	 str[len1]=32;
  memcpy(str+len1+space,str2,len2);
  str[len1+len2+space]=0;
  free(str1);
  return str;
}

tree
save_last_tree() {
  int t = last_tree;
  last_tree = build_nt (EXPR_STMT, void_zero_node);
  return t;
}

void
restore_last_tree(t)
	  tree t;
{
  last_tree = t;
}

tree
temp_value(d1)
     tree d1;
{
  tree dd;
  tree type = integer_type_node;

  // next something based on cp build_local_temp
  tree slot = build_decl (VAR_DECL, NULL_TREE, type);
  DECL_ARTIFICIAL (slot) = 1;
  DECL_CONTEXT (slot) = current_function_decl;
  layout_decl (slot, 0);

  tree decl = slot;
  tree value = d1;
  // next something based on cp build_target_expr

  if (value==0) value=build_int_2(0,0); // workaround to avoid decl 0 and crash
  //fprintf(stderr, "bu %x %x %x %x\n",TREE_TYPE (decl), decl, value,0);
  tree t = set_temp_var(decl, value);
  dd = copy_node(t); //t;

  c_expand_expr_stmt(t);
  TREE_TYPE(dd)=integer_type_node;
  return dd;
}

tree build_our_record(size)
     tree size;
{
  tree rt = make_node(RECORD_TYPE);
  TREE_TYPE(rt)=integer_type_node;
  TYPE_SIZE_UNIT(rt)=integer_type_node;
  TYPE_SIZE_UNIT(rt)=size;
  TYPE_MODE(rt)=SImode;
  return rt;
}

tree
convert_field_ref_to_decl(ref, value)
     tree ref;
     tree value;
{
  tree t = ref;
  tree field=build_decl (FIELD_DECL, 0/*cell_decl_p*/, integer_type_node);
  SET_DECL_C_BIT_FIELD(field);
  if (TREE_CODE(value)!=ADDR_EXPR)
    DECL_BIT_FIELD(field)=1;
  if (TREE_CODE(value)==PARM_DECL)
    value=RVAL_ADDR(value);
  TREE_TYPE(TREE_OPERAND(t,2))=bitsizetype;
  DECL_FIELD_BIT_OFFSET(field)=TREE_OPERAND(t,2);
  DECL_FIELD_OFFSET(field)=fold(TREE_OPERAND(t,0));//build_int_2(0,0);
  my_fold(DECL_FIELD_OFFSET(field));
  DECL_SIZE_UNIT(field)=build_int_2(4,0);
  DECL_SIZE(field)=fold(TREE_OPERAND(t,1));//build_int_2(8,0);
  if (!quiet_flag) printf("off %x %x %x\n",TREE_INT_CST_LOW(DECL_FIELD_OFFSET(field)), TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(field)),TREE_INT_CST(DECL_SIZE(field)));
  TREE_TYPE(DECL_FIELD_OFFSET(field)) = sizetype;
  return field;
}

tree
handle_preset(name, pres, cell_decl_p, size)
     tree name;
     tree pres;
     tree cell_decl_p;
     tree size;
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
    tree extref=RVAL_ADDR(build_external_ref (name, 0));
    tree params = d3;
    tree type = xref_tag(STRUCTURE_ATTR,cell__);
    tree body = my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(type))))));
    tree access = my_copy_tree(TREE_VALUE(TREE_CHAIN(TYPE_FIELDS(type))));
    //chainon (tree_last(d3), build_tree_list(NULL_TREE, extref)); 
    chainon (tree_last(d3), build_tree_list(NULL_TREE, build_int_2(0,0))); 
    my_substitute(body,access,params);
    //my_substitute_parmz(body,access,params);// extra to zero parm
    dd=body;

#if 0    
    tree d1=dd;
    d3=v;
    //    next from opexp9 '=' opexp9
    tree t=d1;
    tree b=TREE_OPERAND (t, 2);
    if (TREE_CODE(t) == BIT_FIELD_REFS && b && TREE_CODE(b) == BIT_FIELD_REF) {
      tree newop0, op0=TREE_OPERAND(b, 0);
      if (TREE_OPERAND(b, 1)) TREE_OPERAND(b, 1)=fold(TREE_OPERAND(b, 1));
      if (TREE_OPERAND(b, 2)) TREE_OPERAND(b, 2)=fold(TREE_OPERAND(b, 2));
      TREE_TYPE(TREE_OPERAND(b, 2)) = ubitsizetype;
      if (TREE_CODE(op0)==INDIRECT_REF) {
	newop0=op0;
	op0=TREE_OPERAND(op0, 0);
#if 0
	if (TREE_CODE(op0)==PLUS_EXPR && TREE_CODE(TREE_TYPE(op0))==POINTER_TYPE) {
	  fprintf(stdout, "\n\nxyz %x\n\n",input_location.line);
	  TREE_TYPE(op0)==integer_type_node;
	}
#endif
      } else {
	tree tt=make_pointer_declarator(0,op0);
	TREE_TYPE(tt)=build_pointer_type(integer_type_node);
	TREE_TYPE(tt)=integer_type_node;
	//tree i = build_unary_op (ADDR_EXPR, op0, 1);
	//newop0=build_indirect_ref (convert(build_pointer_type (integer_type_node),op0), "unary *");
	//newop0=build_indirect_ref (i, "unary *");
	TREE_OPERAND(b, 0)=newop0;
	TREE_OPERAND(b, 0)=tt;
      }
      dd=build_modify_expr(b, NOP_EXPR, d3);
      goto gbitend2;
    }
    if (TREE_CODE(t) == INTEGER_CST || (TREE_CODE(t)==NON_LVALUE_EXPR && TREE_CODE(TREE_OPERAND(t, 0))==INTEGER_CST )) {
      t=make_pointer_declarator(0,d1);
      TREE_TYPE(t)=build_pointer_type(integer_type_node);
      dd=build_modify_expr(t, NOP_EXPR, d3); // check. no LVAL_ADDR?
    } else {
      dd=build_modify_expr(LVAL_ADDR(t), NOP_EXPR, d3);
    }
  gbitend2:
#endif
#if 0
    $$=c_expand_expr_stmt(dd);
#endif
    // differs from local from now on
    tree t;
    //t=LVAL_ADDR(t);
    t=TREE_OPERAND(dd,2);
    tree value=v;
    tree field=convert_field_ref_to_decl(t,value);
#if 0
    constructor_elements
      = tree_cons (field, value, constructor_elements);
#endif
    constructor_elements = chainon (constructor_elements, tree_cons (field, value, 0));
  }
  //    tree mytype=copy_node(integer_type_node);
  //    TREE_CODE (mytype) = RECORD_TYPE;
  tree mytype=build_our_record(size);
  TREE_TYPE(cell_decl_p)=mytype;
  tree constructor = build_constructor(mytype,constructor_elements);
  TREE_CONSTANT(constructor)=1;
  init=constructor;
  //init=field;
  return init;
}

tree
handle_structure(name, st_attr, alloc)
     tree name;
     tree st_attr;
     int alloc;
{
  tree decl, astruct, cell__; 
  tree mysize;
  
  cell__=get_identifier(add_underscore(name,2));
  astruct = start_structure (STRUCTURE_ATTR, cell__);
  st_attr = finish_structure (astruct, TREE_OPERAND(st_attr, 0) , 0,0,0,0); 
  tree size=TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(st_attr))));
  if (alloc==0)
    return 0;
  decl=build_array_declarator (fold(size), NULL_TREE, 0, 0) ; // 4x too big?
  tree byte=build_int_2(8,0);
  TREE_TYPE (byte) = widest_integer_literal_type_node;
  byte = convert (integer_type_node, byte);
  tree newsize = parser_build_binary_op (MULT_EXPR, byte, fold(size));
  tree newint=copy_node(integer_type_node);
  newsize=fold(newsize);
  if (TREE_CODE(newsize)==NON_LVALUE_EXPR)
    newsize=TREE_OPERAND(newsize, 0);
  TYPE_SIZE(newint)=newsize;
  TYPE_SIZE_UNIT(newint)=integer_type_node;
  TYPE_SIZE_UNIT(newint)=TYPE_SIZE(TYPE_SIZE_UNIT(newint));
  //	 TREE_TYPE(cell_)=newint;
  return size;
}

tree
create_temp_var()
{
  tree type = integer_type_node;

  // next something based on cp build_local_temp
  tree slot = build_decl (VAR_DECL, NULL_TREE, type);
  DECL_ARTIFICIAL (slot) = 1;
  DECL_CONTEXT (slot) = current_function_decl;
  layout_decl (slot, 0);
  return slot; 
}

tree
set_temp_var(decl, value)
     tree decl;
     tree value;
{
  // next something based on cp build_target_expr

  if (value==0)
    value=build_int_2(0,0); // workaround to avoid decl 0 and crash
  tree t = build (TARGET_EXPR, TREE_TYPE (decl), decl, value, 0, NULL_TREE);
  TREE_SIDE_EFFECTS (t) = 1;
  return t;
}
