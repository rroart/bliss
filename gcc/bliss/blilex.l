%x macro

%{

  /* File lexlex.l
   * The Unknown Scandinavian was here
   * 9 Nov 1990
   *
   * Taken over by
   * Roar Thronæs <roart@nvg.ntnu.no>
   * 7-MAY-2000
   *
   *
   * Lexically analyzing BLISS using lex
   * lex.yy.c is #included in parse.y
   */

#include "config.h"
#include <stdio.h>

#undef IN_GCC

#include "system.h"
#include "coretypes.h"
#include "tree.h"

#include "parse.h"

#define my_isblank(c) ((c) == ' ' || (c) == '\t')
#define my_isblank2(c) ((c) == ' ' || (c) == '\t' || (c) == 0)
#define my_isletter(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || (c)=='$' || (c)=='_')
#define my_isend(c) ((c) == '\n' || (c) == 0)

  void error(),warning(),inform();
  tree parse_this (char * this);
  tree parse_this_n (char * this, int n);
  tree parse_this_range (char * this, char * end);
  char * my_strcasestr(const char *, const char *);
  char * my_strstrend(const char *, const char *);
  char * my_strstrendbal(const char *, const char *, char , int);

  extern int yydebug;
  extern int yy2debug;
  extern int quiet_flag;
  int opexpcnt=0;

  char * s;

  YYSTYPE yylval;
  unsigned int intern_hash_size=0;
  unsigned int estimated_token_count=0;

  int cond_iter_macro_count = 0;
  int cur_act_par = 0;
  int pname_string=0;
  char pname_out[256];
  int pname_len;
  char * use_yytext;
  int * use_yyleng;
  void pname_macro(char *, int);

  char *p_title;
  int undefmode=0;
  int anymode=0;
  int tnamemode=0;
  int macromode=0;
  int if_mode=0;
  int then_mode=0;
  int skip_mode=0;
  int base_skip_mode=0;
  int do_skip=0;
  int se2=0;
  int last_p=0;
  int one_lexeme=0;
  int balanced_lexemes=0;
  int do_quote=0;

  int blipre_lineno=1;
  int blipre_charno=1;
  int blipre_debug_line=0; 

  int last_eq=0;

#define LEXINIT_INVALID 3
  int which_lex=LEXINIT_INVALID;

  struct bli_token_struct * first_token=NULL;

#define DSC$K_DTYPE_T 14
#define DSC$K_CLASS_S 1

  struct dsc$descriptor {
    unsigned short dsc$w_length;   
    unsigned char dsc$b_dtype;    
    unsigned char dsc$b_class;    
    void          *dsc$a_pointer; 
  };

  struct mymacro {
    struct mymacro * next;
    char * name;
    int type;
    tree param;
    tree param2;
    tree body;
  };

  static int keyword (char *);
  static int ukeyword (char *);
  static void downcase (char *);
  void push_macro(struct dsc$descriptor *);
  void push_req_stack(char *);
  void pop_req_stack(void);
  void push_cond_stack(int);
  void pop_cond_stack(void);
  void * find_macro(struct mymacro *,char *);
  void yy2error (const char *);
  int make_macro_string(struct dsc$descriptor * dsc, struct mymacro *, tree);
  int isupper(int);
  int islower(int);
  int tolower(int);
  int bli_init_reswords (void);
  int yylex ( void );
  int something_that_uses_yyunput(void);
  int add_tree_string(tree, const char *, int, int);
  void strip_par(char *);

#define MAX_INCLUDE_DEPTH 40
  YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
  int include_stack_state[MAX_INCLUDE_DEPTH];
  int include_stack_start[MAX_INCLUDE_DEPTH];
  int include_stack_cond[MAX_INCLUDE_DEPTH];
  int include_stack_act_par[MAX_INCLUDE_DEPTH];
  signed int include_stack_ptr = 0;
#define MAX_COND_DEPTH 40
  int cond_stack[MAX_COND_DEPTH];
  int cond_stack_ptr = 0;
#define MAX_MACRO_DEPTH 40
  tree macro_stack[MAX_MACRO_DEPTH];
  char macro_stack_char[MAX_MACRO_DEPTH];
  int macro_stack_ptr=0;
#define MAX_NESTED_MACRO_DEPTH 10
  void* nested_macro_stack_mac[MAX_MACRO_DEPTH];
  tree nested_macro_stack_act[MAX_MACRO_DEPTH];
  int nested_macro_stack_ptr=0;

 enum{SIMP_MACRO,COND_MACRO,ITER_MACRO,KEYW_MACRO};

  int expr_parse = 0;

  int do_builtin = 0;

  char macrostart=0;
  struct mymacro * macr=0;
  tree macroact=0;
  char macrocomma=1;
  tree remaining=0;

  int longest_string=0;
  int add_memcpy=0;

  char * opened_req[100];
  char opened_req_ptr=0;

  int 
	 is_opened(char * file)
	 {
		downcase(file);
		char i;
		for(i=0;i<opened_req_ptr;i++) {
		  if (0==strcmp(file,opened_req[i]))
			 return 1;
		}
		return 0;
	 }

  void
    push_macro(struct dsc$descriptor *dsc)
    {
      char * empty="";
      char * new=dsc->dsc$a_pointer;
      if (new==0)
        new=empty;
		strip_par(new);
      include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
      include_stack_state[include_stack_ptr] = 0; 
      include_stack_start[include_stack_ptr] = YY_START; 
      include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
      include_stack_act_par[include_stack_ptr] = cur_act_par; 
      include_stack_ptr++;

      yy_switch_to_buffer(yy_scan_bytes(new, dsc->dsc$w_length));
    }

  void
    push_nested_macro()
    {
      nested_macro_stack_mac[nested_macro_stack_ptr] = macr;
      nested_macro_stack_act[nested_macro_stack_ptr] = macroact;
		nested_macro_stack_ptr++;
    }

  void
    pop_nested_macro()
    {
		nested_macro_stack_ptr--;
      macr = nested_macro_stack_mac[nested_macro_stack_ptr];
      macroact = nested_macro_stack_act[nested_macro_stack_ptr];
    }

  void
    push_req_stack(char *new)
    {
      include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
      include_stack_state[include_stack_ptr] = 0;
      include_stack_start[include_stack_ptr] = YY_START;
      include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
      include_stack_act_par[include_stack_ptr] = 0; 
      include_stack_ptr++;
      downcase(new);
      opened_req[opened_req_ptr++]=xstrdup(new);
      yyin = fopen( new, "r" );
     
      yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
    }

  void
    pop_req_stack(void)
    {
      if ( --include_stack_ptr < 0 )
	{
	  if (yy2debug) inform("\nempty include stack\n");
	}
      else
	{
	  yy_delete_buffer( YY_CURRENT_BUFFER );
	  yy_switch_to_buffer(include_stack[include_stack_ptr] );
	  BEGIN(include_stack_start[include_stack_ptr]);
	}
    }

  void
    push_cond_stack(int new)
    {
      cond_stack[cond_stack_ptr++] = /*base_*/skip_mode;
    }

  void
    pop_cond_stack(void)
    {
      if ( --cond_stack_ptr < 0 )
		  {
			 if (yy2debug) inform("\nempty cond stack\n");
		  }
      else
		  {
			 /*base_*/skip_mode=cond_stack[cond_stack_ptr];
		  }
	 }

  int
    get_cond_stack(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return cond_stack[cond_stack_ptr-1];
    }

  int
    get_prev_cond_stack(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return cond_stack[cond_stack_ptr-2];
    }

  void
    push_macro_stack(tree t, char *c)
    {
      macro_stack[macro_stack_ptr] = t;
      macro_stack_char[macro_stack_ptr++] = c;
    }

  tree
    pop_macro_stack(void)
    {
      if ( --macro_stack_ptr < 0 )
		  {
			 if (yy2debug) inform("\nempty cond stack\n");
		  }
      else
		  {
			 return macro_stack[cond_stack_ptr];
		  }
	 }

  char
    get_macro_stack_char(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return macro_stack_char[macro_stack_ptr-1];
    }

%}

%option never-interactive

idigit		[0-9a-f]
digit		[0-9]
letter		[A-Za-z_$]
string		'([^'\n\f]|'')*'
illstring	'([^']|'')*'
name		{letter}({letter}|{digit})*
delim   "="|":"|","|"/"|"."|"<"|"["|"("|"-"|"*"|"+"|"^"|">"|"]"|")"|";"
others	  \"|"#"|"&"|"?"|"@"|\\|"`"|"{"|"|"|"}"|"~"
any        {digit}|{letter}|{delim}|"%"
space      [ \t]
/*optarg "+"|"-"
hexdigit [0-9a-f]
*/

%%

  // executed last?
  if (last_eq) {
	 if (*yytext!='=' && !my_isblank2(*yytext))
      last_eq=0;
  }

  if (se2) {
    se2=0;
    return P_SOFTERROR2;
  }

\n            input_line++;
 { }
\r { }
 { }

{space}+          { }

{delim}				{
  if (skip_mode && if_mode == 0) {
	 yyless(yyleng);
    break;
  }

  if (macromode==3 && one_lexeme) {
    // was this balancing defined in the grammar?
	 // check this. more to balance?
	 if (strchr("<[",*yytext))
		balanced_lexemes++;
	 if (strchr(">]",*yytext))
		balanced_lexemes--;
  }

  if (macromode==3 && one_lexeme && balanced_lexemes==0 && (*yytext==',' || *yytext==')')) {
	 change_macromode();
  }

  if (macromode) { yylval.type_node_p = build_string(strlen(yytext),yytext); return LEXEME; }

  if (last_p && *yytext==')') {
    int last=last_p;
    yyless(yyleng);
    last_p=0;
#if 0
    switch (last) {
	 case P_ALLOCATION:
		{
		  if (!quiet_flag) printf("xxx %s\n",IDENTIFIER_POINTER((tree)mylast()));
		}
		return P_SOFTERROR4;
		break;
    }
    return 777;
#endif
  }

  if (*yytext=='=')
	 last_eq=1;

  if (*yytext=='+' || *yytext=='-') {
	 // workaround because shift/reduce etc with plain chars
#if 0
	 char * text = yytext - 1;
	 while (my_isblank(*text))
      text--;

	 if (*text=='=') {}
#endif
#if 0
	 if (last_eq) {
		last_eq=0;
		if (*yytext=='+')
		  return UPLUS;
		else
		  return UMINUS;
	 }
#else
	 if (opexpcnt==0) {
		if (*yytext=='+')
		  return UPLUS;
		else
		  return UMINUS;
	 }
#endif
  }

  if (*yytext=='=' || *yytext=='(' || *yytext=='[' || *yytext==',')
    opexpcnt=0; // for unary op workaround
  else {
#if 0
    opexpcnt++;
#endif
  }

  return *yytext;
}

{digit}+			{
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  if (macromode) { yylval.type_node_p = build_string(strlen(yytext),yytext); return LEXEME; }
  s=strdup(yytext);
  //fprintf(stderr, "TDIG %s\n",s); fflush(stderr); 
  //yylval.type_node_p=copy_node(get_identifier(s));
  //yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
  yylval.type_node_p=build_int_2(atoi(yytext),0);
  opexpcnt++; // for unary op workaround
  return T_DIGITS;
}

{name}				{
 do_name:
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }

  int key=0;

  downcase (yytext);
  if (do_quote) {
    do_quote=0;
    if (macromode)
      goto out_do_quote;
#if 1
    yylval.type_node_p = get_identifier(yytext);
#else
    yylval.type_node_p = maybe_get_identifier(yytext);
    if (yylval.type_node_p==0)
      yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
#endif
    return T_NAME;
  out_do_quote:
    {}
  }
  if (macromode) { yylval.type_node_p = build_string(strlen(yytext),yytext); return LEXEME; }
  if (tnamemode) goto onlytname;
  if (undefmode) { key=ukeyword(yytext); /*undefmode=0;*/ }
  if (0==strcmp("ch$allocation",yytext))
    key = N_CH_ALLOCATION; // temp workaround
  if (key<=0) key = keyword (yytext);
  if (key == -1) 
    {
      struct mymacro * m=(void*)find_macro(0,yytext);
      if (m) { 
  	     //if (m->param==0) goto ifall;
	macr=m;
	     BEGIN(macro);
	     continue;
      }
#if 0
      if (m) {
	     push_macro(make_macro_string(m));
	     continue;                   /* not obvious at all... */
      }
#endif
    onlytname:
      s=strdup(yytext);
#if 1
      yylval.type_node_p=get_identifier(yytext);
      //yylval.type_node_p=build_string(strlen(yytext),yytext);
#else
      yylval.type_node_p = maybe_get_identifier(yytext);
      if (yylval.type_node_p==0)
	yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
#endif
      tree label = is_label (maybe_get_identifier(yytext));
      if (m==0 && label) {
	//yylval.type_node_p=get_identifier(s);
	return T_LABELNAME; // workaround, temp
      }
      tree field = find_field (s);
      opexpcnt++; // for unary op workaround
      if (m==0 && field) return T_FIELDNAME; // workaround, temp
      if (m==0) return T_NAME;
    }
  /*yylval.type_str=strdup(yytext);*/
  yylval.type_node_p=get_identifier(yytext);
  //yylval.type_node_p=build_string(strlen(yytext),yytext);
  opexpcnt=0; // for unary op workaround
  return key;
}

"%"{name}			{
 p_name:
  {}
  int my_yyleng=0;
  signed int key = -3;
  downcase (yytext);
  key = keyword (yytext);
  int key_len = strlen(yytext);

  if (macromode) {
    if (do_quote) {
      do_quote=0;
      goto do_ret_lex;
    } 
    if (key==P_QUOTE)
      goto p_quote;
  do_ret_lex:
    yylval.type_node_p = build_string(strlen(yytext),yytext);
    return LEXEME;
  }

#if 0
  if (key==-1)
	 {
		fprintf(stderr, "key %x\n",key);
		fflush(stderr);
	 }
#endif

  do_skip=0;
  if (skip_mode && if_mode == 0 && key!=P_IF && key!=P_THEN && key != P_ELSE && key != P_FI) {
#if 0
    yyless(yyleng);
    break;
#else
	 do_skip=1;
#endif
  }

  if (key == -1) {
    goto do_name;
  }

  int retval = 0;
  char * my_yytext=yytext+strlen(yytext);
  char * my_end;
  yyless(strlen(yytext));

  switch (key) {
  case P_IF:
	 {
#if 0
		if_mode = 1;
		extern int yyrec;
		yyrec=1;
		//yyless(yyleng);
		return START_CTCE;
#endif
		my_end=my_strcasestr(my_yytext, "%then");
		int my_yyleng=5+my_end-my_yytext;
		char myline[255];
		memset(myline, 0, 255);
		if (!quiet_flag) printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
		memcpy(myline,my_yytext,my_end-my_yytext);
		if (!quiet_flag) printf("my2 %s\n",myline);
#if 0
		myline+=3;
		myline[strlen(myline)-5]=0;
#endif
		//fprintf(stderr,"myline%smyline\n",myline); fflush(stderr);
		//yytext=my_yytext+=my_yyleng;
		int len=my_end-yytext+5;
		yyless(len);
		//yyless(my_yyleng);
		if (!quiet_flag) printf("skipp %x %x %x\n",base_skip_mode,skip_mode,cond_stack_ptr);
		if (skip_mode==0) {
		  parse_this(myline);
		  push_cond_stack(0);
		  skip_mode=get_cond_stack()|integer_zerop(fold(parser_build_binary_op(BIT_AND_EXPR,mylast(),build_int_2(1,0))));
		  push_cond_stack(0);
		} else {
		  push_cond_stack(0);
		  skip_mode=1;
		  push_cond_stack(0);
		}
		goto myout;
	 }
	 break;

  case P_ELSE: {
	 then_mode = 0;
	 skip_mode=get_prev_cond_stack()|!get_cond_stack();
	 //fprintf(stderr,"pel %s\n",yytext); fflush(stderr); 
	 yyless(5/*yyleng*/);
	 //fprintf(stderr,"pel2 %s\n",yytext); fflush(stderr);
	 key = -1;
  }
	 break;

  case P_FI: { /*skip_mode = 0;*/ pop_cond_stack(); pop_cond_stack(); /*yyless(yyleng);*/ yyless(3); key=-1; }
	 break;

  case P_QUOTE: {
  p_quote:
#if 0
    if (do_quote && macromode) {
      do_quote=0;
      yylval.type_node_p = build_string(strlen(yytext),yytext);
      return LEXEME;
    }
#endif
    yyless(6/*yyleng*/);
    do_quote=1;
    key=-1;
  }
	 break;

  case P_EXPAND: { /* quote_mode_next = 1; */ yyless(7/*yyleng*/); key=-1; }
	 break;

  case P_REMAINING: { 
	 /* quote_mode_next = 1; */
	 yyless(key_len);
	 if (do_skip)
		goto do_break;
	 key=-1;
	 struct dsc$descriptor dsc;
	 print_remain(&dsc, remaining);
	 push_macro(&dsc);
  }
	 break;

  case P_ALLOCATION: {
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);
	 int len=my_end-yytext+1;
	 yyless(len);
	 while(!my_isletter(*my_yytext))
	   my_yytext++;
	 while(!my_isletter(*my_end))
	   my_end--;
	 my_end++;
	 int my_yyleng=1+my_end-my_yytext;
	 char myline[255];
	 if (!quiet_flag) printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
	 memcpy(myline,my_yytext,my_end-my_yytext);
	 myline[my_end-my_yytext]=0;
	 //yyless(my_yyleng);
	 //  yytext=my_yytext+=my_yyleng;

	 if (do_skip)
		goto do_break;

#if 0
	 tree t=get_identifier(myline);
	 tree cell__ = maybe_get_identifier(add_underscore(myline, 2));
#endif
	 myline[my_end-my_yytext]='_';
	 myline[1+my_end-my_yytext]='_';
	 myline[2+my_end-my_yytext]=0;
	 if (!quiet_flag) printf("my %s\n",myline);
	 tree t=maybe_get_identifier(myline);
	 if (t)
	   t=mytag(t);
	 if (t) {
	   //	   tree size=my_copy_tree(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))))));
	   tree size=TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TYPE_FIELDS(t))));
	   size=fold(size);

	   int val=TREE_INT_CST_LOW(size);
	   sprintf(myline,"%d",val);
	   yylval.type_node_p=build_int_2(val,0);
	   if(!quiet_flag)printf("all %x\n",val);
	 } else {
	   myline[1]=0;
	   myline[0]='4';
	   yylval.type_node_p=build_int_2(4,0);
	 }
	 //yylval.type_node_p=copy_node(get_identifier(myline));
	 //yylval.type_node_p=copy_node(build_string(strlen(myline),myline));
	 pname_macro(myline,strlen(myline));
	 retval=T_DIGITS;
	 goto out_data;
#if 0
	 char * str = IDENTIFIER_POINTER($4);
	 if (strcmp(str,"own")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"global")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"forward")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"local")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"stacklocal")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"register")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"global register")==0)
		$$ = build_int_2(4,0);
#endif
  }
	 break;

  case P_ASSIGN: {
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);
	 int len=my_end-yytext+1;
	 yyless(len);
  
	 if (do_skip)
		goto do_break;
  
	 while (*my_end!=')')
		my_end--;

	 char * ct = my_yytext;
	 while (!my_isletter(*ct))
		ct++;

	 char * ct_end = ct;
	 while (!my_isblank(*ct_end) && *ct_end!=',') // or my_isletter
		ct_end++;

	 int my_yyleng=1+my_end-my_yytext;
	 char myline[255];
	 memset(myline, 0, 255);
	 memcpy(myline,ct,ct_end-ct);

	 char * second = ct_end + 1;
	 while (my_isblank(*second) || *second==',')
		second++;
	 char what[255];
	 memset(what, 0, 255);
	 memcpy(what, second, my_end - second);

	 //printf("setcti %s %s %d\n",myline,what,input_location.line);

	 parse_this(what);

	 set_cti(get_identifier(strdup(myline)), fold(mylast()));

	 goto myout;
	 break;
  }

  case P_BLISS: {
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);
	 int my_yyleng=1+my_end-my_yytext;
	 *my_end=0;
	 int is32v=my_strcasestr(my_yytext,"32v");
	 int is32=strstr(my_yytext,"32"); // shortcut
	 if (is32v)
		is32=0;
	 *my_end=')';
	 char myline[255];
	 if (!quiet_flag) printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
	 memcpy(myline,my_yytext,my_end-my_yytext);
	 myline[my_end-my_yytext]=0;
	 //yyless(my_yyleng);
	 //  yytext=my_yytext+=my_yyleng;
	 int len=my_end-yytext+1;
	 yyless(len);
	 myline[1]=0;
	 if (is32)
	   is32=1;
	 if (is32)
		myline[0]='1';
	 else
		myline[0]='0';
	 //printf("is32 %x\n",is32);
	 //yylval.type_node_p=copy_node(get_identifier(myline));
	 //yylval.type_node_p=copy_node(build_string(strlen(myline),myline));
	 yylval.type_node_p=build_int_2(is32,0);
	 pname_macro(myline,strlen(myline));
	 retval=T_DIGITS;
	 goto out_data;
#if 0
	 char * str = IDENTIFIER_POINTER($4);
	 if (strcmp(str,"own")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"global")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"forward")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"local")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"stacklocal")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"register")==0)
		$$ = build_int_2(4,0);
	 if (strcmp(str,"global register")==0)
		$$ = build_int_2(4,0);
#endif
  }
	 break;

  case P_CHAR: {
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);
	 int len=my_end-yytext+1;
	 yyless(len);
  
	 if (do_skip)
		goto do_break;
  
	 char myret[255];
	 memset(myret, 0, 255);
	 myret[0]=39;
	 int myretp=1;
	 char myline[255];

	 char * par = my_yytext+1;
	 char * par_end;

  char_again:
	 par_end = par;
	 while (*par_end!=',' && par_end<my_end)
		par_end++;

	 memset(myline, 0, 255);
	 memcpy(myline,par,par_end-par);

	 parse_this(myline);
	 if (!quiet_flag) printf("myretp %s\n",myline);

	 myret[myretp++]=TREE_INT_CST_LOW(fold(mylast()));

	 if (*par_end==',') {
		par = par_end + 1;
		goto char_again;
	 }

	 myret[myretp]=39;
	 char * s = xmalloc(myretp+2); // check. more of these
	 memcpy(s, myret, myretp+1); // check. more of these
	 // was	 char * s = strndup(myret,myretp+1); // check. more of these
	 if (!quiet_flag) printf("myret %s\n",s);
	 //yylval.type_node_p=copy_node(get_identifier(s));
	 yylval.type_node_p=copy_node(build_string(myretp+1,s)); // check. more
	 TREE_TYPE(yylval.type_node_p)=give_char_array_type_node(); // check. more?
	 pname_macro(s,myretp+1); // check. more like this
	 retval = T_STRING;

	 goto out_data;
	 break;
  }
	 break;

  case P_CHARCOUNT: { 
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);

	 while (*my_end!=')')
		my_end--;

	 int len=my_end-yytext+1;
	 yyless(len);
  
	 if (do_skip)
		goto do_break;
  
	 int myret = 0;
	 char myline[255];

	 char * par = my_yytext+1;
	 char * par_end;

  charcount_again:
	 par_end = par;
	 while (*par_end!=',' && par_end<my_end)
		par_end++;

	 memset(myline, 0, 255);
	 memcpy(myline,par,par_end-par);

	 parse_this(myline);
	 if (!quiet_flag) printf("myretp %s\n",myline);

	 tree last=fold(mylast());
	 if (TREE_CODE(last)==IDENTIFIER_NODE)
	   myret+=strlen(IDENTIFIER_POINTER(last))-2;
	 if (TREE_CODE(last)==STRING_CST)
	   myret+=strlen(TREE_STRING_POINTER(last))-2;

	 if (*par_end==',') {
		par = par_end + 1;
		goto charcount_again;
	 }

	 memset(myline, 0, 255);
	 sprintf(myline,"%d",myret);
	 char * s = strdup(myline);
	 if (!quiet_flag) printf("myret %s\n",s);
	 //yylval.type_node_p=copy_node(get_identifier(s));
	 //yylval.type_node_p=copy_node(build_string(strlen(s),s));
	 yylval.type_node_p=build_int_2(myret,0);
	 pname_macro(s, strlen(s));
	 retval = T_DIGITS;

	 goto out_data;
	 break;
  }
	 break;

  case P_CTCE: { 
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);

	 while (*my_end!=')')
		my_end--;

	 int len=my_end-yytext+1;
	 yyless(len);
  
	 if (do_skip)
		goto do_break;
  
	 int tmp;
	 int myret = 1;
	 char myline[255];

	 char * par = my_yytext+1;
	 char * par_end;

  ctce_again:
	 par_end = par;
	 while (*par_end!=',' && par_end<my_end)
		par_end++;

	 memset(myline, 0, 255);
	 memcpy(myline,par,par_end-par);

	 if (!quiet_flag) printf("myretp %s\n",myline);
	 parse_this(myline);

	 tmp=0;
	 if (TREE_TYPE(TREE_OPERAND(fold(mylast()),0))==INTEGER_CST)
		tmp=1;
	 myret*=tmp;

	 if (*par_end==',') {
		par = par_end + 1;
		goto ctce_again;
	 }

	 memset(myline, 0, 255);
	 sprintf(myline,"%d",myret);
	 char * s = strdup(myline);
	 if (!quiet_flag) printf("myret %s\n",s);
	 //yylval.type_node_p=copy_node(get_identifier(s));
	 //yylval.type_node_p=copy_node(build_string(strlen(s),s));
	 yylval.type_node_p=build_int_2(myret,0);
	 pname_macro(s,strlen(s));
	 retval = T_DIGITS;

	 goto out_data;
	 break;
  }
	 break;

  case P_EXPLODE: { 
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);

	 while (*my_end!=')')
		my_end--;

	 int len=my_end-yytext+1;
	 yyless(len);
  
	 if (do_skip)
		goto do_break;
  
	 int myret = 0;
	 int mylinep = 0;
	 char myline[255];
	 memset(myline, 0, 255);

	 char * par = my_yytext+1;
	 char * par_end;

  explode_again:
	 par_end = par;
	 while (*par_end!=',' && par_end<my_end)
		par_end++;

	 char * quote_start = par;
	 char * quote_end = par_end;

	 while (*quote_start!=39)
		quote_start++;
	 quote_start++;
	 while (*quote_end!=39)
		quote_end--;
	 quote_end--;

	 memcpy(myline+mylinep,quote_start,1+quote_end-quote_start);
	 mylinep+=1+quote_end-quote_start;

	 if (*par_end==',') {
		par = par_end + 1;
		goto explode_again;
	 }

	 char myline2[255];
	 memset(myline2, 0, 255);
	 int myline2p=0;

	 int i;
	 for (i=0;i<mylinep;i++) {
		myline2[myline2p++]=39;
		myline2[myline2p++]=myline[i];
		myline2[myline2p++]=39;
		if (i<(mylinep-1))
		  myline2[myline2p++]=',';
	 }

	 include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
	 include_stack_state[include_stack_ptr] = 0;
	 include_stack_start[include_stack_ptr] = YY_START;
	 include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
	 include_stack_act_par[include_stack_ptr] = cur_act_par; 
	 yy_switch_to_buffer(yy_scan_string(myline2));
	 if (!quiet_flag) printf("myexp %s\n",myline2);

	 goto myout;
	 break;
  }
	 break;

  case P_ERROR:
  case P_WARN:
  case P_INFORM:
  case P_PRINT:
  case P_MESSAGE:
  case P_ERRORMACRO:
  case P_NAME:
  case P_STRING: { 
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);

	 while (*my_end!=')')
		my_end--;

	 int len=my_end-yytext+1;
	 yyless(len);

	 if (do_skip)
		goto do_break;
  
	 char myret[255];
	 int myretp=0;
	 memset(myret, 0, 255);
	 if (key!=P_NAME) {
		myret[0]=39;
		myretp=1;
	 }
	 char myline[255];

	 char * par = my_yytext+1;
	 char * par_end;

  string_again:
	 par_end = par;
#if 0
	 while (*par_end!=',' && par_end<my_end)
		par_end++;
#endif
	 par_end=my_strstrendbal(par,my_end,',',1);

	 memset(myline, 0, 255);
	 memcpy(myline,par,par_end-par);

	 strip_par(myline);
	 if (!quiet_flag) printf("myretsp %s\n",myline);
	 tree t=parse_this(myline);
	 t=fold(t);
	 int v=TREE_INT_CST_LOW(t);
	 char * c = &v;
	 int i;
	 if (!quiet_flag) printf("tc %x\n",TREE_CODE(t));

	 if (TREE_CODE(t)!=INTEGER_CST)
		goto no_int;

	 if (TREE_LANG_FLAG_0(t)) {
		while (*c) {
		  myret[myretp++]=*c++;
		}
		goto str_out;
	 }

	 if (TREE_LANG_FLAG_1(t)) {
		i=*c++;
		while (i--) {
		  myret[myretp++]=*c++;
		}
		goto str_out;
	 }

	 if (TREE_LANG_FLAG_2(t)) {
		if (!quiet_flag) printf("ascid stuff not yet implemented\n");
	 }

	 sprintf(&myret[myretp],"%d",v);
	 myretp+=strlen(&myret[myretp]);

	 goto str_out;

  no_int:

	 {}

	 char * start=0;
	 if (TREE_CODE(t)==IDENTIFIER_NODE) start=IDENTIFIER_POINTER(t);
	 if (TREE_CODE(t)==STRING_CST) start=TREE_STRING_POINTER(t);

	 if (key==P_NAME && (TREE_CODE(t)==IDENTIFIER_NODE || TREE_CODE(t)==STRING_CST) && *start!=39) {
		int len;
		if (TREE_CODE(t)==IDENTIFIER_NODE)
		  len=IDENTIFIER_LENGTH(t);
		else
		  len=TREE_STRING_LENGTH(t);
		memcpy(&myret[myretp],start,len);
		myretp+=len;
		goto str_out;
	 }

	 start++;
	 char * end = strchr(start,39);

	 if (end) {
	   memcpy(&myret[myretp],start,end-start);
	   myretp+=strlen(&myret[myretp]);
	 }

  str_out:

	 if (*par_end==',') {
		par = par_end + 1;
		goto string_again;
	 }

	 if (key!=P_NAME)
		myret[myretp]=39;

	 char * s = strdup(myret);
	 if (!quiet_flag) printf("myrets %s\n",s);
	 //yylval.type_node_p=copy_node(get_identifier(s));
	 yylval.type_node_p=copy_node(build_string(strlen(s),s));

	 void (*fn)()=0;
	 char * str;

	 switch (key) {
	 case P_ERROR:
		fn=error;
		str="ERR";
		break;
	 case P_WARN:
		fn=warning;
		str="WARN";
		break;
	 case P_INFORM:
		fn=inform;
		str="INFORM";
		break;
	 case P_PRINT:
		fn=inform;
		str="";
		break;
	 case P_MESSAGE:
		fn=inform;
		str="MSG";
		break;
	 case P_ERRORMACRO:
		fn=inform;
		str="ERRORMACRO";
		break;
	 case P_NAME:
	   yylval.type_node_p=get_identifier(s);
	   pname_macro(s,strlen(s));
		retval=T_NAME;
		goto out_data;
		break;
	 case P_STRING:
	   pname_macro(s,strlen(s));
		retval=T_STRING;
		goto out_data;
		break;
	 }
	 fn("BLS-%s-%s\n",str,s);
	 goto myout;
	 break;
  }
	 break;

  case P_EXACTSTRING: { 
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);

	 while (*my_end!=')')
		my_end--;

	 int len=my_end-yytext+1;
	 yyless(len);
  
	 if (do_skip)
		goto do_break;
  
	 char * p1 = my_yytext + 1;
	 while (my_isblank(*p1))
		p1++;

	 char * p1_end = p1;
	 while (*p1_end!=',')
		p1_end++;

	 char * p2 = p1_end + 1;
	 while (my_isblank(*p2))
		p2++;

	 char * p2_end = p2;
	 while (*p2_end!=',')
		p2_end++;

	 tree n = parse_this_range(p1, p1_end);
	 n=fold(n);
	 int number=TREE_INT_CST_LOW(n);

	 tree f = parse_this_range(p2, p2_end);
	 f=fold(f);
	 int fill=TREE_INT_CST_LOW(f);

	 //printf("exa %x %x %x\n",number,fill,f);

	 char myret[255];
	 memset(myret, 0, 255);
	 myret[0]=39;
	 int myretp=1;
	 char myline[255];

	 char * par = p2_end+1;
	 char * par_end;

  ex_string_again:
	 par_end = par;
	 while (*par_end!=',' && par_end<my_end)
		par_end++;

	 memset(myline, 0, 255);
	 memcpy(myline,par,par_end-par);

	 if (!quiet_flag) printf("myretsp %s\n",myline);
	 parse_this(myline);

	 tree t=fold(mylast());
	 int v=TREE_INT_CST_LOW(t);
	 char * c = &v;
	 int i;
	 if (!quiet_flag) printf("tc %x\n",TREE_CODE(t));

	 if (TREE_CODE(t)!=INTEGER_CST)
		goto ex_no_int;

	 if (TREE_LANG_FLAG_0(t)) {
		while (*c) {
		  myret[myretp++]=*c++;
		}
		goto ex_str_out;
	 }

	 if (TREE_LANG_FLAG_1(t)) {
		i=*c++;
		while (i--) {
		  myret[myretp++]=*c++;
		}
		goto ex_str_out;
	 }

	 if (TREE_LANG_FLAG_2(t)) {
		if (!quiet_flag) printf("ascid stuff not yet implemented\n");
	 }

	 sprintf(&myret[myretp],"%d",v);
	 myretp+=strlen(&myret[myretp]);

	 goto ex_str_out;

  ex_no_int:

	 {}

	 char * start = IDENTIFIER_POINTER(t) + 1;
	 char * end = strchr(start,39);

	 memcpy(&myret[myretp],start,end-start);
	 myretp+=strlen(&myret[myretp]);

  ex_str_out:

	 if (*par_end==',') {
		par = par_end + 1;
		goto ex_string_again;
	 }

	 myret[myretp]=39;
	 myretp--;

	 if (number<myretp) {
		myret[number+1]=39;
		myret[number+2]=0;
	 }
	 if (number>myretp) {
		memset(myret+myretp+1, fill, number-myretp);
		myret[number+1]=39;
		myret[number+2]=0;
	 }

	 char * s = strdup(myret);
	 if (!quiet_flag) printf("myretexs %s\n",s);
	 //yylval.type_node_p=copy_node(get_identifier(s));
	 yylval.type_node_p=copy_node(build_string(strlen(s),s));
	 pname_macro(s,strlen(s));

	 retval=T_STRING;
	 goto out_data;
	 break;
  }
	 break;

  case P_REMOVE: { 
	 my_yytext=strchr(my_yytext,'(');
	 my_end=my_strstrendbal(my_yytext, 0, 0, 1);

	 while (*my_end!=')')
		my_end--;

#if 0
	 int len=my_end-yytext+1;
	 yyless(len);
#else
	 //  yyless(7);
	 yytext[6]=' ';
	 yyless(6);
#endif

	 if (do_skip)
		goto do_break;
  
#if 1
	 *my_yytext=' ';
	 *my_end=' ';
#endif

	 my_yytext++;
	 my_end--;
  
	 while (my_isblank(*my_yytext))
		my_yytext++;

	 while (my_isblank(*my_end))
		my_end--;

	 int myret = 0;
	 char myline[255];
	 memset(myline, 0, 255);

	 char end_char;
	 char start_char=*my_yytext;

	 switch (start_char) {
    case '(':
      end_char=')';
      break;
    case '<':
      end_char='>';
      break;
    case '[':
      end_char=']';
      break;
    default:
      goto no_remove;
	 }

	 //  my_yytext;
	 char * tmp_end = strchr(my_yytext+1,end_char);

	 if (tmp_end!=my_end)
		goto no_remove;

	 *my_yytext=' ';
	 *my_end=' ';

  no_remove:
#if 0
	 memcpy(myline,my_yytext,my_end-my_yytext);
	 char * s = strdup(myline);
#endif
	 if (!quiet_flag) printf("myrem %s\n",my_yytext);
#if 0
	 yylval.type_node_p=copy_node(get_identifier(s));
	 retval = T_DIGITS;
#endif
	 goto myout;
	 goto out_data;
	 break;
  }
	 break;

  case P_SBTTL:
  case P_TITLE:
	 {
		my_yytext=strchr(my_yytext,39);
		my_end=strchr(my_yytext+1, 39);
		int my_yyleng=1+my_end-my_yytext;
		int len=my_end-yytext+1;
		yyless(len);
		goto myout;
	 }
	 break;

  case P_DECLARED:
	 {
		my_yytext=strchr(my_yytext,'(');
		my_end=my_strstrendbal(my_yytext, 0, 0, 1);

		while (*my_end!=')')
		  my_end--;

		int len=my_end-yytext+1;
		yyless(len);

		if (do_skip)
		  goto do_break;
  
		my_yytext++;
		while (my_isblank(*my_yytext))
		  my_yytext++;

		my_end--;
		while (my_isblank(*my_end))
		  my_end--;

		int myret = 0;
		char myline[255];
		memset(myline, 0, 255);
		memcpy(myline,my_yytext,1+my_end-my_yytext);

#if 1
		if (!quiet_flag) printf("decl0 %s\n",myline);
		tree n = parse_this(myline);
		char * myline2=0;
		if (TREE_CODE(n)==IDENTIFIER_NODE) myline2=IDENTIFIER_POINTER(n);
		if (TREE_CODE(n)==STRING_CST) myline2=TREE_STRING_POINTER(n);
#else
		char * quote=my_strcasestr(myline,"%quote");
		char * myline2=myline;
		if (quote) {
		  myline2+=6;
		  while (my_isblank(*myline2))
			 myline2++;
		}
#endif
		downcase(myline2);
		tree is_there = maybe_get_identifier(myline2);
		if (is_there==0)
		  find_macro(0,myline2);
		if (is_there)
		  is_there=is_something(is_there);
		if (is_there==0)
		  is_there = find_field(myline2);
		if (!quiet_flag) printf("decl %s %x\n",myline2,is_there);

		myline[1]=0;
		if (is_there)
		  is_there=1;
		if (is_there)
		  myline[0]='1';
		else
		  myline[0]='0';
		//yylval.type_node_p=copy_node(get_identifier(myline));
		//yylval.type_node_p=copy_node(build_string(strlen(myline),myline));
		yylval.type_node_p=build_int_2(is_there,0);
		pname_macro(myline,strlen(myline));
		retval=T_DIGITS;
		goto out_data;

		break;
	 }
	 break;

  case P_NULL:
	 {
		my_yytext=strchr(my_yytext,'(');
		my_end=my_strstrendbal(my_yytext, 0, 0, 1);

		while (*my_end!=')')
		  my_end--;

		int len=my_end-yytext+1;
		yyless(len);
  
		if (do_skip)
		  goto do_break;
  
		char myline[255];
		int myret=0;

		char * par = my_yytext+1;
		char * par_end;

	 null_again:
		par_end = par;
#if 0
		while (*par_end!=',' && par_end<my_end)
		  par_end++;
#endif
		par_end=my_strstrendbal(par,my_end,',',1);

		memset(myline, 0, 255);
		memcpy(myline,par,par_end-par);

		strip_par(myline);
		if (!quiet_flag) printf("null parse this %s\n",myline);
		tree t=parse_this(myline);
		if (t) // check
		  myret++;
#if 0
		// why was this here?
		if (!is_on_remaining(myline),macroact)
		  myret++;
#endif

		if (*par_end==',') {
		  par = par_end + 1;
		  goto null_again;
		}

		if (myret==0)
		  myline[0]='1';
		else
		  myline[0]='0';
		myline[1]=0;
		if (myret==0)
		  myret=1;
		else
		  myret=0;

		char * s = strdup(myline);
		if (!quiet_flag) printf("null %s\n",s);
		//yylval.type_node_p=copy_node(get_identifier(s));
		//yylval.type_node_p=copy_node(build_string(strlen(s),s));
		yylval.type_node_p=build_int_2(myret,0);
		pname_macro(s,strlen(s));

		retval=T_DIGITS;
		goto out_data;
		break;
	 }
	 break;

  case P_IDENTICAL:
	 {
		my_yytext=strchr(my_yytext,'(');
		my_end=my_strstrendbal(my_yytext, 0, 0, 1);

		while (*my_end!=')')
		  my_end--;

		int len=my_end-yytext+1;
		yyless(len);
  
		if (do_skip)
		  goto do_break;
  
		char * p1 = my_yytext + 1;
		while (my_isblank(*p1))
		  p1++;

		char * p1_end = p1;
		while (*p1_end!=',')
		  p1_end++;

		char * p2 = p1_end + 1;
		while (my_isblank(*p2))
		  p2++;

		char * p2_end = my_strstrendbal(p2, my_end, 0, 1);
		if (*p2_end!=')')
		  p2_end++; // bad workaround?

		char myline1[255];
		char myline2[255];
		memset(myline1,0,255);
		memset(myline2,0,255);
		memcpy(myline1,p1,p1_end-p1);
		memcpy(myline2,p2,p2_end-p2);

		char myline[255];

		if (!quiet_flag) printf("IDENT %s %s\n",myline1,myline2);
#if 0
		tree t1=parse_this(myline1);
		tree t2=parse_this(myline2);
		if (!quiet_flag) printf("IDENT %x %x\n",t1,t2);
#endif
		if (0==strcmp(myline1,myline2))
		  myline[0]='1';
		else
		  myline[0]='0';
		myline[1]=0;

		char * s = strdup(myline);
		if (!quiet_flag) printf("ident %s\n",s);
		//yylval.type_node_p=copy_node(get_identifier(s));
		//yylval.type_node_p=copy_node(build_string(strlen(s),s));
		yylval.type_node_p=build_int_2(myline[0]-48,0);
		pname_macro(s,strlen(s));

		retval=T_DIGITS;
		goto out_data;
		break;
	 }
	 break;

  case P_ISSTRING:
	 {
		my_yytext=strchr(my_yytext,'(');
		my_end=my_strstrendbal(my_yytext, 0, 0, 1);

		while (*my_end!=')')
		  my_end--;

		int len=my_end-yytext+1;
		yyless(len);
  
		if (do_skip)
		  goto do_break;
  
		char myline[255];
		int myret=0;

		char * par = my_yytext+1;
		char * par_end;

	 isstring_again:
		par_end = par;
		while (*par_end!=',' && par_end<my_end)
		  par_end++;

		tree t = parse_this_range(par, par_end);

		t = fold(t);

		if (*par_end==',') {
		  par = par_end + 1;
		  goto isstring_again;
		}

		if (TREE_CODE(t)!=IDENTIFIER_NODE)
		  myret=1;
		else {
		  char *l=IDENTIFIER_POINTER(t);
		  if (l[0]!=39 || l[strlen(l)-1]!=39)
			 myret=1;
		}

		if (myret==0)
		  myline[0]='1';
		else
		  myline[0]='0';
		myline[1]=0;

		char * s = strdup(myline);
		if (!quiet_flag) printf("isstr %s\n",s);
		//yylval.type_node_p=copy_node(get_identifier(s));
		//yylval.type_node_p=copy_node(build_string(strlen(s),s));
		yylval.type_node_p=build_int_2(myline[0]-48,0);
		pname_macro(s,strlen(s));

		retval=T_DIGITS;
		goto out_data;
		break;
	 }
	 break;

  case P_FIELDEXPAND:
	 {
		my_yytext=strchr(my_yytext,'(');
		my_end=my_strstrendbal(my_yytext, 0, 0, 1);

		while (*my_end!=')')
		  my_end--;

		int len=my_end-yytext+1;
		yyless(len);
  
		if (do_skip)
		  goto do_break;
  
		char * p1 = my_yytext + 1;
		while (my_isblank(*p1))
		  p1++;

		char * p1_end = p1;
		while (my_isletter(*p1_end))
		  p1_end++;

		char * p2 = p1_end;
		while (my_isblank(*p2))
		  p2++;

		char * p2_end = p2;

		if (*p2!=',') {
		  p2 = 0;
		  goto no_p2;
		}

		p2++;
		while (my_isblank(*p2))
		  p2++;

		p2_end = p2;
		while (isdigit(*p2_end)) // fix/change later
		  p2_end++;

	 no_p2:
		{}

		char myline1[255];
		char myline2[255];
		memset(myline1,0,255);
		memset(myline2,0,255);
		memcpy(myline1,p1,p1_end-p1);
		int v=0;
		if (p2) {
		  memcpy(myline2,p2,p2_end-p2);
		  tree t=fold(parse_this_range(p2,p2_end));
		  v=TREE_INT_CST_LOW(t);
		}

		tree exp = find_field(myline1);

		tree new;

		if (p2==0)
		  new = my_copy_tree(exp);
		else {
		  v++;
		  for (new=exp;v;v--,new=TREE_CHAIN(new)) ;
		  new=copy_node(new);
		}    

		char * p = print_tree(new);

		expr_parse++;
		include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
		include_stack_state[include_stack_ptr] = 0;
		include_stack_start[include_stack_ptr] = YY_START;
		include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
		include_stack_act_par[include_stack_ptr] = cur_act_par; 
		include_stack_ptr++;
		yy_switch_to_buffer(yy_scan_string(p));

		goto myout;
		break;
	 }
	 break;

  case P_LENGTH:
	 {
		yyless(key_len);

		if (do_skip)
		  goto do_break;
  
		tree t=macroact;
		int len=0;
		for (;t;t=TREE_CHAIN(t),len++) ;
		len = cur_act_par;

		char myline[255];
		memset(myline,0,255);
		sprintf(myline,"%d",len);
		char * s = strdup(myline);
		if (!quiet_flag) printf("len %s\n",s);
		//yylval.type_node_p=copy_node(get_identifier(s));
		//yylval.type_node_p=copy_node(build_string(strlen(s),s));
		yylval.type_node_p=build_int_2(len,0);
		pname_macro(s,strlen(s));

		retval=T_DIGITS;
		goto out_data;
		break;
	 }
	 break;

  case P_NUMBER:
	 {
		my_yytext=strchr(my_yytext,'(');
		my_end=my_strstrendbal(my_yytext, 0, 0, 1);

		while (*my_end!=')')
		  my_end--;

		int len=my_end-yytext+1;
		yyless(len);

		if (do_skip)
		  goto do_break;
  
		my_yytext++;
		my_end--;

		tree t = parse_this_range(my_yytext,my_end);
		t=fold(t);
		int v=0;

		if (TREE_CODE(t)==IDENTIFIER_NODE) {
		  if (TREE_TYPE(t)) {
		    t=TREE_TYPE(t);
		    goto number_int;
		  }
		  char * s = IDENTIFIER_POINTER(t);
		  int len=strlen(s);
		  if (s[0]==39 && s[len-1]==39) {
			 char * new = xstrdup(s+1);
			 new[len-2]=0;
			 v=atoi(new);
		  } else {
			 if (!quiet_flag) printf("not yet implemnted number\n");
		  }
		} else {
		number_int:
		  v=TREE_INT_CST_LOW(t);
		}
  
		char myline[255];
		memset(myline,0,255);
		sprintf(myline,"%d",v);
		char * s = strdup(myline);
		if (!quiet_flag) printf("number %s\n",s);
		//yylval.type_node_p=copy_node(get_identifier(s));
		//yylval.type_node_p=copy_node(build_string(strlen(s),s));
		yylval.type_node_p=build_int_2(v,0);
		pname_macro(s,strlen(s));

		retval=T_DIGITS;
		goto out_data;
		break;
	 }
	 break;

  case P_COUNT:
	 {
		yyless(key_len);

		if (do_skip)
		  goto do_break;
  
		char myline[255];
		memset(myline,0,255);
		sprintf(myline,"%d",cond_iter_macro_count);
		char * s = strdup(myline);
		//yylval.type_node_p=copy_node(get_identifier(s));
		//yylval.type_node_p=copy_node(build_string(strlen(s),s));
		yylval.type_node_p=build_int_2(cond_iter_macro_count,0);
		pname_macro(s,strlen(s));

		cond_iter_macro_count=0; // reset it here because cannot in cond itself?
		retval=T_DIGITS;
		goto out_data;
		break;
	 }
	 break;

  case P_EXITITERATION:
    // might suffice for a while? (until I get code that use it, at least)
  case P_EXITMACRO:
	 {
		yyless(key_len);

		if (do_skip)
		  goto do_break;

		if (!quiet_flag) printf("exitm\n");

		goto eof;
		break;
	 }
	 break;

  case P_X:
  case P_O:
  case P_B:
    {
      yyless(key_len);
      
      if (do_skip)
	goto do_break;

      break;
    }
    break;

  case P_SWITCHES:
    {
      // not really implemented
      my_yytext=strchr(my_yytext,'(');
      my_end=my_strstrendbal(my_yytext, 0, 0, 1);

      while (*my_end!=')')
	my_end--;

      int len=my_end-yytext+1;
      yyless(len);
      
      if (do_skip)
	goto do_break;

      char myline[255];
      myline[0]='0';
      myline[1]=0;

      char * s = strdup(myline);
      //yylval.type_node_p=copy_node(get_identifier(s));
      //yylval.type_node_p=copy_node(build_string(strlen(s),s));
      yylval.type_node_p=build_int_2(0,0);
      pname_macro(s,strlen(s));

      retval=T_DIGITS;
      goto out_data;
      break;
    }
    break;
  
  case P_LTCE:
  case P_VARIANT:
  case P_UNQUOTE:
  case P_SIZE:
  case P_NBITS:
  case P_NBITSU:
  case P_QUOTENAME:
	 {
		if (!quiet_flag) printf("key %x not yet implemented\n", key);
	 }
  default: {
#if 0
	 printf("DEF %x %x %x %s\n",key,strlen(yytext),keyw_len(key),yytext);
	 yyless(strlen(yytext));
	 yyless(keyw_len(key));
	 printf("DEF %x %x %x %s\n",key,strlen(yytext),keyw_len(key),yytext);
#endif
	 yyless(key_len);
	 break;
  }
	 break;
  }

  if (key>=0)
    return key;
#if 0
  //if (key!=((int)-1))
  {
	 printf("key %x\n",key);
  }
#endif
  //  break;

  goto myout;

 out_data:
  if (macromode) { return LEXEME; }
  if (pname_string) {
    pname_string=0;
    use_yytext=pname_out;
    use_yyleng=pname_len;
    goto pname_back;
  }
  opexpcnt++;
  return retval;

 less_and_break:
  yyless(my_yyleng);
 do_break:
  break;

 myout:
  {}
  if (pname_string)
    pname_string=0;
}

"%"				{
  if (do_quote && macromode) {
    do_quote=0;
    yylval.type_node_p = build_string(strlen(yytext),yytext);
    return LEXEME;
  }
  if (macromode) macromode=0;
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  yylval.type_str = strdup(yytext);
  return '%';
}

{string}			{
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  struct mymacro * m;
  /* store that string w/o '' */
  if (macromode) { yylval.type_node_p = build_string(strlen(yytext),yytext); return LEXEME; }
  m=(void*)find_macro(0,yytext);
  if (m) { 
    macr=m;
    BEGIN(macro);
    exit(42);
  }

#if 0
  if (m) {
    push_macro(make_macro_string(m));
    continue;                   /* not obvious at all...*/
  }
#endif
  s=strdup(yytext);
  //yylval.type_node_p=copy_node(get_identifier(s));
  yylval.type_node_p=copy_node(build_string(strlen(s),s));
  return T_STRING;
}

'\0' {
  warning("Even more illegal?\n");
  exit(9999);
  goto do_ill;
}

{illstring}			{
  // seems you can create illegal strings with \n etc in it
 do_ill:
  warning("Got illegal string\n");
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  struct mymacro * m;
  /* store that string w/o '' */
  if (macromode) { yylval.type_node_p = build_string(strlen(yytext),yytext); return LEXEME; }
  m=(void*)find_macro(0,yytext);
  if (m) { 
    macr=m;
    BEGIN(macro);
    exit(42);
  }

#if 0
  if (m) {
    push_macro(make_macro_string(m));
    continue;                   /* not obvious at all...*/
  }
#endif
  s=strdup(yytext);
  //yylval.type_node_p=copy_node(get_identifier(s));
  yylval.type_node_p=copy_node(build_string(strlen(s),s));
  return T_STRING;
}

![^\n\f]*[\n\f]			/* ignore trailing comment */ input_line++ ; 

"%("([^\)]|")"[^\%])*")%"	/* ignore embedded comment */  ;

"@" { if (yy2debug) inform("%%lexical gone?\n"); }

{others}			{
  yy2error ("not recognized");
}

<<EOF>> { 
 eof:
  if ( --include_stack_ptr < 0 )
    {
      if (yy2debug) inform("\nempty include stack\n");
      return YY_NULL;
    }
  else
    {
      yy_delete_buffer( YY_CURRENT_BUFFER );
      yy_switch_to_buffer(include_stack[include_stack_ptr] );
      if (include_stack_cond[include_stack_ptr] != cond_stack_ptr) {
	if (!quiet_flag)printf("resetting cond_stack_ptr %x %x\n", include_stack_cond[include_stack_ptr], cond_stack_ptr); 
#if 0
	sleep (1);
#endif
	cond_stack_ptr = include_stack_cond[include_stack_ptr]; 
      }
      if (include_stack_act_par[include_stack_ptr] != cur_act_par) {
	if (!quiet_flag)printf("setting cur_act_par %x %x\n", include_stack_act_par[include_stack_ptr], cur_act_par); 
	cur_act_par = include_stack_act_par[include_stack_ptr]; 
      }
      if (include_stack_state[include_stack_ptr])
	BEGIN(include_stack_start[include_stack_ptr]);
    }
#if 0
  if (expr_parse) {
	 expr_parse--;
	 return END_EXPR;
  }
#else
  if (include_stack_state[include_stack_ptr])
    return END_EXPR;
#endif
  if (do_builtin) {
	 do_builtin--;
	 return END_BUILTIN;
  }
} 

<macro>"%x"|"%X"|"%o"|"%O"|"%b"|"%B"|"%bpval"|"%BPVAL"|"%bpunit"|"%BPUNIT"|"%bpaddr"|%"BPADDR"|"%upval"|"%UPVAL"|"%ASCID"|"%ascid"|"%ASCIC"|"%ascic"|"%ASCIZ"|"%asciz" {
  goto m_name;
}

<macro>"%"{name} {
  downcase(yytext+1);
  if (0==strcmp("%else",yytext) || 0==strcmp("%if",yytext) || 0==strcmp("%then",yytext) || 0==strcmp("%fi",yytext)) {
    if (macrostart==0 && 0==strcmp("%if",yytext))
      goto do_this;
    goto do_p_name;
  }
  if (macrostart==0) {
  do_this:
    yyless(0);
    goto ifall;
  }
  pname_string=1;
 do_p_name:
  goto p_name;
}

<macro>{delim}|"%"				{
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  int newmacrocomma;
  if (*yytext=='[' || *yytext=='(' || *yytext=='<') {
	 if (macrostart==0)
		macrostart=*yytext;
  }

  if (*yytext=='[' || *yytext=='(' || *yytext=='<') {
	 push_macro_stack(macroact,*yytext);
	 if (macro_stack_ptr>1) {
	   if (macroact==0) {
	     macroact=chainon(macroact,build_string(strlen(yytext),yytext));
	     goto out;
	   }
	   tree t = tree_last(macroact);
	   if (macrocomma=='=' && macr->type==KEYW_MACRO) {
	     // was: add_tree_string(TREE_TYPE(t), yytext, 1, 1);
	     if (TREE_TYPE(tree_last(macroact))==0)
	       TREE_TYPE(tree_last(macroact))=build_string(strlen(yytext),yytext);
	     else
	       add_tree_string(TREE_TYPE(tree_last(macroact)), yytext, 1, 1);
	   } else {
	     add_tree_string(t, yytext, 1, 0);
	   }
	 }
	 goto out;
  }

  if (*yytext==']' || *yytext==')' || *yytext=='>') {
	 if (macro_stack_ptr==0)
		goto end_out;
	 if (macro_stack_ptr>1) {
		tree t = tree_last(macroact);
		if (macrocomma=='=' && macr->type==KEYW_MACRO)
		  add_tree_string(TREE_TYPE(t), yytext, 1, 1);
		else
		  add_tree_string(t, yytext, 1, 0);
		pop_macro_stack();
		goto out;
	 }
	 pop_macro_stack();
  }

  if (macrostart==0 && *yytext==',') {
	 yyless(0);
	 goto ifall;
  }

 end_out:
  if (macrostart==0) {
	 yyless(0);
	 goto ifall;
  }

  if (macrostart) {
    switch (*yytext) {
    case ']':
      goto ifall;
      break;
    case ')':
      goto ifall;
      break;
    case '>': 
      goto ifall;
      break;
	 case '=':
		if (macro_stack_ptr==1 && macr->type==KEYW_MACRO)
		  goto out;
	 default:
		if (macro_stack_ptr>1) {
		  tree t = tree_last(macroact);
		  if (macrocomma=='=' && macr->type==KEYW_MACRO)
		    add_tree_string(TREE_TYPE(t), yytext, strlen(yytext), 1);
		  else
		    add_tree_string(t, yytext, strlen(yytext), 0);
		} else {
		  if (*yytext==',')
		    if (macroact==0) {
		      macroact=chainon(macroact,build_string(0,""));
		      macroact=chainon(macroact,build_string(0,""));
#if 0
		      macroact=chainon(macroact,build_string(strlen(yytext),yytext));
		      sleep(1);
#endif
		    } else {
		      macroact=chainon(macroact,build_string(0,""));
		    }
		  else {
		    if (macroact==0) {
		      macroact=chainon(macroact,build_string(strlen(yytext),yytext));
		      goto out;
		    }
		    tree t = tree_last(macroact);
		    if (macrocomma=='=' && macr->type==KEYW_MACRO) {
		      if (TREE_TYPE(tree_last(macroact))==0)
			TREE_TYPE(tree_last(macroact))=build_string(strlen(yytext),yytext);
		      else
			add_tree_string(TREE_TYPE(tree_last(macroact)), yytext, strlen(yytext), 1);
		    } else {
		      add_tree_string(t, yytext, strlen(yytext), 1);
		    }
		  }
		}
    }
    goto out;
  ifall:
    {
      tree l = tree_last(macroact);
      if (l && TREE_CODE(l)==STRING_CST && 0==strcmp(TREE_STRING_POINTER(l),","))
	chainon(macroact,build_string(0,""));
      tree p = macr->param;
      tree a = macroact;
      while (a && p) {
        a = TREE_CHAIN(a);
        p = TREE_CHAIN(p);
      }
      if (p==0)
        remaining=a;
      struct dsc$descriptor dsc;
      int sts = make_macro_string(&dsc, macr, macroact);
      if ((sts&1)==0)
#if 0
	push_macro("%exitmacro");
#else
      do_eof();
#if 0
	goto eof;
#endif
#endif
	else
      push_macro(&dsc);
      for (cur_act_par = 0, a = macroact; a; a = TREE_CHAIN(a), cur_act_par++) ;
    }
	 // not yet? pop_nested_macro();
	 if (nested_macro_stack_ptr==0) {
		macrostart=0;
		free(macr);
		macr=0;
		macroact=0;
		macrocomma=1;
		BEGIN(0);
		continue;
	 }
  } // if (macrostart)...
 out:

  newmacrocomma=*yytext;

  if (macrocomma && newmacrocomma==',') {
	 //fprintf(stderr,"macroact chainon\n");
	 //chainon(macroact,tree_cons(0,0,0));
  }

  if ((newmacrocomma==',' && macro_stack_ptr == 1) || newmacrocomma=='=')
    macrocomma=newmacrocomma;

}

<macro>{name}|{string}|{digit}+				{
 m_name:
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  use_yytext=yytext;
  use_yyleng=strlen(yytext);
 pname_back:
  downcase (use_yytext);
  if (macrostart==0) {
    yyless(0);
    goto ifall;
  }
  struct mymacro * m=(void*)find_macro(0,use_yytext);
  if (m) {
#if 0
	 // not  yet?
	 push_nested_macro();
	 // which also must must handle the ordinary macro_stack etc
	 macr=m;
	 macroact=0;
	 continue;
#endif
	 // temp workaround if macros are simple enough?
	 if (m->param==0 && m->param2==0) {
	        struct dsc$descriptor dsc;
		int sts = make_macro_string(&dsc, m, 0);
		if ((sts&1)==0)
#if 0
		  push_macro("%exitmacro");
#else
		do_eof();
#if 0
		  goto eof;
#endif
#endif
		  else
		push_macro(&dsc);
		free(m);
		continue;
	 }
  }
  if (macrocomma==0) if (yy2debug) inform("\nmacrocomma missing %x\n",input_line);
  if (macrocomma=='=' && macr->type==KEYW_MACRO) {
    if (TREE_TYPE(tree_last(macroact))==0)
      TREE_TYPE(tree_last(macroact))=build_string(use_yyleng,use_yytext);
    else
      add_tree_string(TREE_TYPE(tree_last(macroact)), use_yytext, use_yyleng, 1);
    goto out_macro_comma;
  }
  if (macrocomma==',' && macro_stack_ptr==1) {
#if 0
	 // don't need this anymore?
	 macroact=chainon(macroact,build_string(use_yyleng,use_yytext));
#else
	 tree t = tree_last(macroact);
	 int space=0;
	 if (TREE_STRING_LENGTH(t))
	   space=1;
	 add_tree_string(t, use_yytext, use_yyleng, space);
#endif
  } else {
	 if (macroact==0) {
		macroact=chainon(macroact,build_string(use_yyleng,use_yytext));
		goto out_macro_comma;
	 }
	 tree t = tree_last(macroact);
	 add_tree_string(t, use_yytext, use_yyleng, 1);
  }
 out_macro_comma:
  {}
  //  macrocomma=1;
}

<macro>\n            input_line++;
<macro> { }
<macro>\r { }
<macro> { }

<macro>{space}+          { }

<macro>![^\n\f]*[\n\f]			/* ignore trailing comment */ input_line++ ; 

%%


struct dict {
  const char *d_text;
  int	d_num;
};

static void downcase (cp)
     char *cp;
{
  while (cp && *cp) {
    if (isupper (*cp))
      *cp = tolower (*cp);
    cp++;
  }
}

struct dict keywords[] = 
  {
#if 1
	 { "%allocation", P_ALLOCATION },	/* percent names */
#endif
	 { "%ascic", P_ASCIC },
	 { "%ascid", P_ASCID },
	 { "%ascii", P_ASCII },
	 { "%asciz", P_ASCIZ },
#if 1
	 { "%assign", P_ASSIGN },
#endif
	 { "%b", P_B },
#if 1
	 { "%bliss", P_BLISS },
#if 0
	 { "%bliss16", P_BLISS16 },
	 { "%bliss32", P_BLISS32 },
	 { "%bliss36", P_BLISS36 },
#endif
#if 0
	 { "%bpaddr", P_BPADDR },
	 { "%bpunit", P_BPUNIT },
	 { "%bpval", P_BPVAL },
#endif
#endif
	 { "%c", P_C },
#if 1
	 { "%char", P_CHAR },
	 { "%charcount", P_CHARCOUNT },
	 { "%chesc", P_CHESC },		/* Is this a keyword? */
	 { "%count", P_COUNT },
	 { "%ctce", P_CTCE },
#endif
	 { "%d", P_D },
	 { "%decimal", P_DECIMAL },
#if 1
	 { "%declared", P_DECLARED },
#endif
	 { "%e", P_E },
#if 1
	 { "%else", P_ELSE },
	 { "%error", P_ERROR },
	 { "%errormacro", P_ERRORMACRO },
	 { "%exactstring", P_EXACTSTRING },
	 { "%exititeration", P_EXITITERATION },
	 { "%exitmacro", P_EXITMACRO },
	 { "%expand", P_EXPAND },
	 { "%explode", P_EXPLODE },
	 { "%fi", P_FI },
	 { "%fieldexpand", P_FIELDEXPAND },
#endif
	 { "%g", P_G },
	 { "%h", P_H },
#if 1
	 { "%identical", P_IDENTICAL },
	 { "%if", P_IF },
	 { "%inform", P_INFORM },
	 { "%isstring", P_ISSTRING },
	 { "%length", P_LENGTH },
	 { "%ltce", P_LTCE },
	 { "%message", P_MESSAGE },
	 { "%name", P_NAME },
	 { "%nbits", P_NBITS },
	 { "%nbitsu", P_NBITSU },
	 { "%null", P_NULL },
	 { "%number", P_NUMBER },
#endif
	 { "%o", P_O },
	 { "%p", P_P },
#if 1
	 { "%print", P_PRINT },
	 { "%quote", P_QUOTE },
	 { "%quotename", P_QUOTENAME },
#endif
	 { "%rad50_10", P_RAD50_10 },
	 { "%rad50_11", P_RAD50_11 },
#if 1
	 { "%ref", P_REF },
	 { "%remaining", P_REMAINING },
	 { "%remove", P_REMOVE },
	 { "%require", P_REQUIRE },
	 { "%sbttl", P_SBTTL },
	 { "%sixbit", P_SIXBIT },
	 { "%size", P_SIZE },
	 { "%string", P_STRING },
	 { "%switches", P_SWITCHES },
	 { "%the", P_THE },
	 { "%then", P_THEN },
	 { "%title", P_TITLE },
	 { "%unquote", P_UNQUOTE },
#if 0
	 { "%upval", P_UPVAL },
#endif
	 { "%variant", P_VARIANT },
	 { "%warn", P_WARN },
#endif
	 { "%x", P_X },
	 { "addressing_mode", K_ADDRESSING_MODE }, /* Normal names */
	 { "align", K_ALIGN },
	 { "always", K_ALWAYS },
	 { "and", K_AND },
	 { "begin", K_BEGIN },
	 { "bind", K_BIND },
	 { "bit", K_BIT },
	 { "builtin", K_BUILTIN },
	 { "by", K_BY },
	 { "byte", K_BYTE },
	 { "case", K_CASE },
	 { "codecomment", K_CODECOMMENT },
	 { "compiletime", K_COMPILETIME },
	 { "decr", K_DECR },
	 { "decra", K_DECRA },
	 { "decru", K_DECRU },
	 { "do", K_DO },
	 { "else", K_ELSE },
	 { "eludom", K_ELUDOM },
	 { "enable", K_ENABLE },
	 { "end", K_END },
	 { "eql", K_EQL },
	 { "eqla", K_EQLA },
	 { "eqlu", K_EQLU },
	 { "eqv", K_EQV },
	 { "exitloop", K_EXITLOOP },
	 { "external", K_EXTERNAL },
	 { "field", K_FIELD },
	 { "forward", K_FORWARD },
	 { "from", K_FROM },
	 { "geq", K_GEQ },
	 { "geqa", K_GEQA },
	 { "gequ", K_GEQU },
	 { "global", K_GLOBAL },
	 { "gtr", K_GTR },
	 { "gtra", K_GTRA },
	 { "gtru", K_GTRU },
	 { "if", K_IF },
	 { "incr", K_INCR },
	 { "incra", K_INCRA },
	 { "incru", K_INCRU },
	 { "initial", K_INITIAL },
	 { "inrange", K_INRANGE },
	 { "iopage", K_IOPAGE },
	 { "keywordmacro", K_KEYWORDMACRO },
	 { "label", K_LABEL },
	 { "leave", K_LEAVE },
	 { "leq", K_LEQ },
	 { "leqa", K_LEQA },
	 { "lequ", K_LEQU },
	 { "library", K_LIBRARY },
	 { "linkage", K_LINKAGE },
	 { "literal", K_LITERAL },
	 { "local", K_LOCAL },
	 { "long", K_LONG },
	 { "lss", K_LSS },
	 { "lssa", K_LSSA },
	 { "lssu", K_LSSU },
	 { "macro", K_MACRO },
	 { "map", K_MAP },
	 { "mod", K_MOD },
	 { "module", K_MODULE },
	 { "neq", K_NEQ },
	 { "neqa", K_NEQA },
	 { "nequ", K_NEQU },
	 { "not", K_NOT },
	 { "novalue", K_NOVALUE },
	 { "of", K_OF },
	 { "or", K_OR },
	 { "otherwise", K_OTHERWISE },
	 { "outrange", K_OUTRANGE },
	 { "own", K_OWN },
	 { "plit", K_PLIT },
	 { "preset", K_PRESET },
	 { "psect", K_PSECT },
	 { "record", K_RECORD },
	 { "ref", K_REF },
	 { "register", K_REGISTER },
	 { "rep", K_REP },
	 { "require", K_REQUIRE },
	 { "return", K_RETURN },
	 { "routine", K_ROUTINE },
	 { "select", K_SELECT },
	 { "selecta", K_SELECTA },
	 { "selectone", K_SELECTONE },
	 { "selectonea", K_SELECTONEA },
	 { "selectoneu", K_SELECTONEU },
	 { "selectu", K_SELECTU },
	 { "set", K_SET },
	 { "setunwind", K_SETUNWIND },
	 { "show", K_SHOW },
	 { "signal", K_SIGNAL },
	 { "signed", K_SIGNED },
	 { "stacklocal", K_STACKLOCAL },
	 { "stop", K_STOP },
	 { "structure", K_STRUCTURE },
	 { "switches", K_SWITCHES },
	 { "tes", K_TES },
	 { "then", K_THEN },
	 { "to", K_TO },
	 { "undeclare", K_UNDECLARE },
	 { "unsigned", K_UNSIGNED },
	 { "until", K_UNTIL },
	 { "uplit", K_UPLIT },
	 { "volatile", K_VOLATILE },
	 { "weak", K_WEAK },
	 { "while", K_WHILE },
	 { "with", K_WITH },
	 { "word", K_WORD },
	 { "xor", K_XOR },
	 { NULL, -1 }
  };

int keyw_len(int key) {
  if (!quiet_flag) printf(" KEY %x %s\n",strlen(keywords[key-P_ALLOCATION].d_text),(keywords[key-P_ALLOCATION].d_text));
  return strlen(keywords[key-P_ALLOCATION].d_text);
}

static struct dict ukeywords[] = {
  { "absolute", U_ABSOLUTE },
  { "assembly", U_ASSEMBLY },
  { "binary", U_BINARY },
  { "bliss10_ots", U_BLISS10_OTS },
  { "bliss16", U_BLISS16 },
  { "bliss32", U_BLISS32 },
  { "bliss36", U_BLISS36 },
  { "bliss36c_ots", U_BLISS36C_OTS },
  { "call", U_CALL },
  { "clearstack", U_CLEARSTACK },
  { "code", U_CODE },
  { "commentary", U_COMMENTARY },
  { "concatenate", U_CONCATENATE },
  { "debug", U_DEBUG },
  { "eis", U_EIS },
  { "emt", U_EMT },
  { "entry", U_ENTRY },
  { "environment", U_ENVIRONMENT },
  { "errs", U_ERRS },
  { "execute", U_EXECUTE },
  { "expand", U_EXPAND },
  { "extended", U_EXTENDED },
  { "general", U_GENERAL },
  { "ident", U_IDENT },
  { "indirect", U_INDIRECT },
  { "interrupt", U_INTERRUPT },
  { "iot", U_IOT },
  { "jsb", U_JSB },
  { "jsr", U_JSR },
  { "jsys", U_JSYS },
  { "ka10", U_KA10 },
  { "ki10", U_KI10 },
  { "kl10", U_KL10 },
  { "ks10", U_KS10 },
  { "language", U_LANGUAGE },
  { "library", U_LIBRARY },
  { "linkage", U_LINKAGE },
  { "linkage_regs", U_LINKAGE_REGS },
  { "list", U_LIST },
  { "long_relative", U_LONG_RELATIVE },
  { "lsi11", U_LSI11 },
  { "lsi11", U_LSI11 },
  { "main", U_MAIN },
  { "noassembly", U_NOASSEMBLY },
  { "nobinary", U_NOBINARY },
  { "nocode", U_NOCODE },
  { "nocommentary", U_NOCOMMENTARY },
  { "nodebug", U_NODEBUG },
  { "nodefault", U_NODEFAULT },
  { "noeis", U_NOEIS },
  { "noerrs", U_NOERRS },
  { "noexecute", U_NOEXECUTE },
  { "noexpand", U_NOEXPAND },
  { "noindirect", U_NOINDIRECT },
  { "nolibrary", U_NOLIBRARY },
  { "nonexternal", U_NONEXTERNAL },
  { "noobject", U_NOOBJECT },
  { "nooptimize", U_NOOPTIMIZE },
  { "nopic", U_NOPIC },
  { "nopreserve", U_NOPRESERVE },
  { "noread", U_NOREAD },
  { "norequire", U_NOREQUIRE },
  { "nosafe", U_NOSAFE },
  { "noshare", U_NOSHARE },
  { "nosource", U_NOSOURCE },
  { "nosymbolic", U_NOSYMBOLIC },
  { "notrace", U_NOTRACE },
  { "notused", U_NOTUSED },
  { "nounames", U_NOUNAMES },
  { "nowrite", U_NOWRITE },
  { "nozip", U_NOZIP },
  { "object", U_OBJECT },
  { "odt", U_ODT },
  { "optimize", U_OPTIMIZE },
  { "optlevel", U_OPTLEVEL },
  { "origin", U_ORIGIN },
  { "ots", U_OTS },
  { "ots_linkage", U_OTS_LINKAGE },
  { "overlay", U_OVERLAY },
  { "pic", U_PIC },
  { "pic", U_PIC },
  { "portal", U_PORTAL },
  { "preserve", U_PRESERVE },
  { "ps_interrupt", U_PS_INTERRUPT },
  { "pushj", U_PUSHJ },
  { "read", U_READ },
  { "relative", U_RELATIVE },
  { "relocatable", U_RELOCATABLE },
  { "require", U_REQUIRE },
  { "rsx_ast", U_RSX_AST },
  { "rtt", U_RTT },
  { "safe", U_SAFE },
  { "share", U_SHARE },
  { "skip", U_SKIP },
  { "source", U_SOURCE },
  { "stack", U_STACK },
  { "standard", U_STANDARD },
  { "standard_ots", U_STANDARD_OTS },
  { "symbolic", U_SYMBOLIC },
  { "t11", U_T11 },
  { "t11", U_T11 },
  { "tops10", U_TOPS10 },
  { "tops20", U_TOPS20 },
  { "trace", U_TRACE },
  { "trap", U_TRAP },
  { "unames", U_UNAMES },
  { "valuecbit", U_VALUECBIT },
  { "version", U_VERSION },
  { "word_relative", U_WORD_RELATIVE },
  { "write", U_WRITE },
  { "zip", U_ZIP },
  /*  { "structure", U_STRUCTURE },*/
  { NULL, -1 }
};

#define NKEYW 180
#define NUKEYW 111

static struct dict prename[] = {
  { "abs", N_ABS },
//  { "bitvector", N_BITVECTOR },
  { "bliss", N_BLISS },
  { "bliss10", N_BLISS10 },
  { "bliss36c", N_BLISS36C },
//  { "block", N_BLOCK },
//  { "blockvector", N_BLOCKVECTOR },
  { "ch$a_rchar", N_CH_A_RCHAR },
  { "ch$a_wchar", N_CH_A_WCHAR },
  { "ch$allocation", N_CH_ALLOCATION },
  { "ch$compare", N_CH_COMPARE },
  { "ch$copy", N_CH_COPY },
  { "ch$diff", N_CH_DIFF },
  { "ch$eql", N_CH_EQL },
  { "ch$fail", N_CH_FAIL },
  { "ch$fill", N_CH_FILL },
  { "ch$find_ch", N_CH_FIND_CH },
  { "ch$find_not_ch", N_CH_FIND_NOT_CH },
  { "ch$find_sub", N_CH_FIND_SUB },
  { "ch$geq", N_CH_GEQ },
  { "ch$gtr", N_CH_GTR },
  { "ch$leq", N_CH_LEQ },
  { "ch$lss", N_CH_LSS },
  { "ch$move", N_CH_MOVE },
  { "ch$neq", N_CH_NEQ },
  { "ch$plus", N_CH_PLUS },
  { "ch$ptr", N_CH_PTR },
  { "ch$rchar", N_CH_RCHAR },
  { "ch$rchar_a", N_CH_RCHAR_A },
  { "ch$size", N_CH_SIZE },
  { "ch$translate", N_CH_TRANSLATE },
  { "ch$wchar", N_CH_WCHAR },
  { "ch$wchar", N_CH_WCHAR_A },
  { "fortran", N_FORTRAN },
  { "fortran_func", N_FORTRAN_FUNC },
  { "fortran_sub", N_FORTRAN_SUB },
  { "f10", N_F10 },
  { "max", N_MAX },
  { "maxa", N_MAXA },
  { "maxu", N_MAXU },
  { "min", N_MIN },
  { "mina", N_MINA },
  { "minu", N_MINU },
  { "setunwind", N_SETUNWIND },
  { "sign", N_SIGN },
  { "signal", N_SIGNAL },
  { "signal_stop", N_SIGNAL_STOP },
//  { "vector", N_VECTOR },
  { "$code$", N_CODE },
  { "$global$", N_GLOBAL },
  { "$high$", N_HIGH },
  { "$low$", N_LOW },
  { "$own$", N_OWN },
  { "$plit$", N_PLIT },
  { NULL, -1 }
};

static struct dict bname[] = {
  { "actualcount", B_ACTUALCOUNT },
  { "actualparameter", B_ACTUALPARAMETER },
  { "ap", B_AP },
  { "argptr", B_ARGPTR },
  { "fp", B_FP },
  { "nullparameter", B_NULLPARAMETER },
  { "pc", B_PC },
  { "r0", B_R0 },
  { "r1", B_R1 },
  { "r2", B_R2 },
  { "r3", B_R3 },
  { "r4", B_R4 },
  { "r5", B_R5 },
  { "r6", B_R6 },
  { "r7", B_R7 },
  { "r8", B_R8 },
  { "r9", B_R9 },
  { "r10", B_R10 },
  { "r11", B_R11 },
  { "sp", B_SP },
  { NULL, -1 }
};

static int keyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) inform("key %x %s\n",(unsigned int)cp,cp);

  first = 0;				/* inclusive */
  last = NKEYW;				/* exclusive */
  last = sizeof keywords / sizeof (struct dict) - 1;
  while (first < last)
    {
      half = (first + last) / 2;
      c = strcmp (cp, keywords[half].d_text);
      if (c == 0)
		  return keywords[half].d_num;
      else if (c < 0)
		  last = half;
      else
		  first = half + 1;
    }
  if (yy2debug) inform("keynot\n");
  return -1;				/* Not a keyword */
}

static int ukeyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) inform("ukey %x %s\n",(unsigned int)cp,cp);
  first = 0;				/* inclusive */
  last = NUKEYW;				/* exclusive */
  while (first < last)
    {
      half = (first + last) / 2;
      c = strcmp (cp, ukeywords[half].d_text);
      if (c == 0)
		  return ukeywords[half].d_num;
      else if (c < 0)
		  last = half;
      else
		  first = half + 1;
    }
  if (yy2debug) inform("ukeynot\n");
  return -1;				/* Not a keyword */
}

int
yywrap(void)
{
  return 1;
}

tree *bli_ridpointers;
#define N_reswords (sizeof keywords / sizeof (struct dict))
int
bli_init_reswords (void)
{
  unsigned int i;
  tree id;
  /* It is not necessary to register ridpointers as a GC root, because
     all the trees it points to are permanently interned in the
     get_identifier hash anyway.  */
  bli_ridpointers = (tree *) xcalloc ((int) REALLY_MAX, sizeof (tree));
  for (i = 0; i < N_reswords && keywords[i].d_num>-1; i++)
    {
      /* If a keyword is disabled, do not enter it into the table
         and so create a canonical spelling that isn't a keyword.  */

      id = get_identifier (keywords[i].d_text);
#if 0
      C_RID_CODE (id) = keywords[i].rid;
      C_IS_RESERVED_WORD (id) = 1;
      TREE_LANG_FLAG_0 (id) = 1;
      id->common.lang_flag_0 = 1;
#endif
      bli_ridpointers [(int) keywords[i].d_num] = id;
    }
  return 0;
}

int something_that_uses_yyunput(void) {
  void (*f)(int c, char *);
  f=yyunput;
  return 0;
}

tree parse_this (char * this) {
  if (!quiet_flag) printf("parse this %s\n",this);
  expr_parse++;
  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
  include_stack_state[include_stack_ptr] = 1;
  include_stack_start[include_stack_ptr] = YY_START;
  include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
  include_stack_act_par[include_stack_ptr] = cur_act_par; 
  include_stack_ptr++;
  yy_switch_to_buffer(yy_scan_string(this));
  BEGIN(0);
  //tree t = save_last_tree();
  yyparse();
  //restore_last_tree(t);
  return mylast();
}

tree parse_this_n (char * this, int n) {
  expr_parse++;
  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
  include_stack_state[include_stack_ptr] = 1;
  include_stack_start[include_stack_ptr] = YY_START;
  include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
  include_stack_act_par[include_stack_ptr] = cur_act_par; 
  include_stack_ptr++;
  yy_switch_to_buffer(yy_scan_bytes(this,n));
  BEGIN(0);
  yyparse();
  return mylast();
}

tree parse_this_range (char * this, char * end) {
  goto skip_me;
  if (yydebug) {
  skip_me: {}
    char myline[255];
    memset(myline, 0, 255);
    memcpy(myline,this,end-this);
    if (!quiet_flag) printf("parse_this_range %s\n",myline);
  }
  expr_parse++;
  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
  include_stack_state[include_stack_ptr] = 1;
  include_stack_start[include_stack_ptr] = YY_START;
  include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
  include_stack_act_par[include_stack_ptr] = cur_act_par; 
  include_stack_ptr++;
  yy_switch_to_buffer(yy_scan_bytes(this,end-this));
  BEGIN(0);
  yyparse();
  return mylast();
}

typedef char chartype;

char *
my_strcasestr (phaystack, pneedle)
     const char *phaystack;
     const char *pneedle;
{
  register const unsigned char *haystack, *needle;
  register chartype b, c;

  haystack = (const unsigned char *) phaystack;
  needle = (const unsigned char *) pneedle;

  b = tolower (*needle);
  if (b != '\0')
    {
      haystack--;                               /* possible ANSI violation */
      do
        {
          c = *++haystack;
          if (c == '\0')
            goto ret0;
        }
      while (tolower (c) != (int) b);

      c = tolower (*++needle);
      if (c == '\0')
        goto foundneedle;
      ++needle;
      goto jin;

      for (;;)
        {
          register chartype a;
          register const unsigned char *rhaystack, *rneedle;

          do
            {
              a = *++haystack;
              if (a == '\0')
                goto ret0;
              if (tolower (a) == (int) b)
                break;
              a = *++haystack;
              if (a == '\0')
                goto ret0;
				shloop:
              ;
            }
          while (tolower (a) != (int) b);

		  jin:      a = *++haystack;
          if (a == '\0')
            goto ret0;

          if (tolower (a) != (int) c)
            goto shloop;

          rhaystack = haystack-- + 1;
          rneedle = needle;
          a = tolower (*rneedle);

          if (tolower (*rhaystack) == (int) a)
            do
              {
                if (a == '\0')
                  goto foundneedle;
                ++rhaystack;
                a = tolower (*++needle);
                if (tolower (*rhaystack) != (int) a)
                  break;
                if (a == '\0')
                  goto foundneedle;
                ++rhaystack;
                a = tolower (*++needle);
              }
				while (tolower (*rhaystack) == (int) a);

          needle = rneedle;             /* took the register-poor approach */

          if (a == '\0')
            break;
        }
    }
 foundneedle:
  return (char*) haystack;
 ret0:
  return 0;
}

char * my_strstrend(const char * s, const char * t) {
  char * e = strchr(s, '\n');
  if (e)
	 return e;
  return s + strlen(s);
}

char * my_strstrendbal(const char * s, const char * end, char t, int bal) {
  char * e = end; //strchr(s, '\n');
    if (e==0)
	 e=s+strlen(s);
  if (bal) {
	 signed int mybalcnt = 0;
	 char * c = s;
	 for(;c!=e;c++) {
		if (*c=='(' || *c=='[' || *c=='<')
		  mybalcnt++;
		if (*c==')' || *c==']' || *c=='>') {
		  mybalcnt--;
		  if (mybalcnt<0)
			 return c-1; // check
		  if (mybalcnt==0 && t==0) // bad shortcut
			 return c;
		}
		if (t && t==*c && mybalcnt==0) // bad shortcut
		  return c;
	 }
  }
 get_out:
  return e;
}

change_macromode() {
  if (one_lexeme) {
	 macromode=0;
	 one_lexeme=0;
  }
}

int
add_tree_string(t,s,len,space)
	  tree t;
	  const char * s;
	  int len;
	  int space;
{
  // maybe a source for leakage
  int orig_len = TREE_STRING_LENGTH (t);
  char str[1024]; // check bound overflow later
  memcpy(str, TREE_STRING_POINTER(t), orig_len);
  if (space) {
    str[orig_len]=32;
    space=1;
  }
  memcpy(str+orig_len+space, s, len);
  str[orig_len+len+space]=0;
  TREE_STRING_POINTER (t) = ggc_alloc_string (str, orig_len+len+space);
  TREE_STRING_LENGTH (t) = orig_len+len+space;
  //  free(str); leak
  if (TREE_STRING_LENGTH (t)>longest_string)
    longest_string = TREE_STRING_LENGTH (t);
  add_memcpy+=orig_len;
}

void
strip_par (s)
	  char * s;
{
#if 0
  return;
#endif
  int end = strlen(s)-1;
  int start = 0;
  if (my_isblank(s[start]))
	 start++;
  if (my_isblank(s[end]))
	 end--;
  int alt_end = start+1;
  for (;alt_end<end;alt_end++)
    if (s[alt_end]==')')
      return;
  if (s[start]=='(' && s[end]==')')
	 s[start]=s[end]=' ';
}

void
pname_macro(src,len)
     char * src;
     int len;
{
  if (pname_string==0)
    return;
  if (len>250) {
    printf("LEN %x %s\n",len,src);
    exit(84);
  }
  memcpy(pname_out,src,len);
  pname_out[len]=0;
  pname_len=len;
}

check_lib(s) 
     char * s;
{
  // lib.req is really compiled lib and seems to imply starlet, too
  downcase(s);
  if (0==strcmp("lib",s)) {
    char * f="starlet.req";
    if (!is_opened(f))
      push_req_stack(f);
  }
}

do_eof()
{ 
 eof:
  if ( --include_stack_ptr < 0 )
    {
      if (yy2debug) inform("\nempty include stack\n");
      return YY_NULL;
    }
  else
    {
      yy_delete_buffer( YY_CURRENT_BUFFER );
      yy_switch_to_buffer(include_stack[include_stack_ptr] );
      if (include_stack_cond[include_stack_ptr] != cond_stack_ptr) {
	if (!quiet_flag)printf("resetting cond_stack_ptr %x %x\n", include_stack_cond[include_stack_ptr], cond_stack_ptr); 
#if 0
	sleep (1);
#endif
	cond_stack_ptr = include_stack_cond[include_stack_ptr]; 
      }
      if (include_stack_act_par[include_stack_ptr] != cur_act_par) {
	if (!quiet_flag)printf("setting cur_act_par %x %x\n", include_stack_act_par[include_stack_ptr], cur_act_par); 
	cur_act_par = include_stack_act_par[include_stack_ptr]; 
      }
      if (include_stack_state[include_stack_ptr])
	BEGIN(include_stack_start[include_stack_ptr]);
    }
#if 0
  if (expr_parse) {
	 expr_parse--;
	 return END_EXPR;
  }
#else
  if (include_stack_state[include_stack_ptr])
    return END_EXPR;
#endif
  if (do_builtin) {
	 do_builtin--;
	 return END_BUILTIN;
  }
} 
