%x macro p_paren

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
#define my_isletter(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || (c)=='$' || (c)=='_')

  void error(),warning(),inform();
  tree parse_this (char * this);
  tree parse_this_n (char * this, int n);
  tree parse_this_range (char * this, char * end);


  extern int yydebug;
  extern int yy2debug;

  char * s;

  YYSTYPE yylval;
  unsigned int intern_hash_size=0;
  unsigned int estimated_token_count=0;

  char *p_title;
  int undefmode=0;
  int anymode=0;
  int tnamemode=0;
  int macromode=0;
  int if_mode=0;
  int then_mode=0;
  int skip_mode=0;
  int se2=0;
  int last_p=0;

  int blipre_lineno=1;
  int blipre_charno=1;
  int blipre_debug_line=0; 

#define LEXINIT_INVALID 3
  int which_lex=LEXINIT_INVALID;

  struct bli_token_struct * first_token=NULL;

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
  void push_macro(char *);
  void push_req_stack(char *);
  void pop_req_stack(void);
  void push_cond_stack(int);
  void pop_cond_stack(void);
  void * find_macro(struct mymacro *,char *);
  void yy2error (const char *);
  char * make_macro_string(struct mymacro *, tree);
  int isupper(int);
  int islower(int);
  int tolower(int);
  int bli_init_reswords (void);
  int yylex ( void );
  int something_that_uses_yyunput(void);

#define MAX_INCLUDE_DEPTH 10
  YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
  int include_stack_ptr = 0;
#define MAX_COND_DEPTH 10
  int cond_stack[MAX_COND_DEPTH];
  int cond_stack_ptr = 0;

  int expr_parse = 0;

  int do_builtin = 0;

  char macrostart=0;
  struct mymacro * macr=0;
  tree macroact=0;
  char macrocomma=1;
  tree remaining=0;

  void
    push_macro(char *new)
    {
      char * empty="";
      if (new==0)
        new=empty;
      include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
 
      yy_switch_to_buffer(yy_scan_string(new));
    }

  void
    push_req_stack(char *new)
    {
      include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
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
	}
    }

  void
    push_cond_stack(int new)
    {
      cond_stack[cond_stack_ptr++] = skip_mode;
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
      skip_mode=cond_stack[cond_stack_ptr];
    }
 }

  int
    get_cond_stack(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return cond_stack[cond_stack_ptr-1];
    }

%}

%option never-interactive

idigit		[0-9a-f]
digit		[0-9]
letter		[A-Za-z_$]
string		'([^'\n\f]|'')*'
name		{letter}({letter}|{digit})*
delim   "="|":"|","|"/"|"."|"<"|"["|"("|"-"|"*"|"+"|"^"|">"|"]"|")"|";"
others	  \"|"#"|"&"|"?"|"@"|\\|"`"|"{"|"|"|"}"|"~"
any        {digit}|{letter}|{delim}|"%"
space      [ \t]
/*optarg "+"|"-"
hexdigit [0-9a-f]
*/

%%

  if (se2) {
    se2=0;
    return P_SOFTERROR2;
  }

\n            input_line++;

{space}+          { }

{delim}				{
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }

  if (macromode) { yylval.type_node_p = copy_node(get_identifier(strdup(yytext))); return LEXEME; }

  if (last_p && *yytext==')') {
    int last=last_p;
    yyless(yyleng);
    last_p=0;
#if 0
    switch (last) {
      case P_ALLOCATION:
        {
          fprintf(stderr, "xxx %s\n",IDENTIFIER_POINTER((tree)mylast()));
        }
        return P_SOFTERROR4;
        break;
    }
    return 777;
#endif
  }

  return *yytext;
}

{digit}+			{
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  if (macromode) { yylval.type_node_p = copy_node(get_identifier(strdup(yytext))); return LEXEME; }
  s=strdup(yytext);
  //fprintf(stderr, "TDIG %s\n",s); fflush(stderr); 
  yylval.type_node_p=copy_node(get_identifier(s));
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
  if (macromode) { yylval.type_node_p = copy_node(get_identifier(strdup(yytext))); return LEXEME; }
  if (tnamemode) goto onlytname;
  if (undefmode) { key=ukeyword(yytext); /*undefmode=0;*/ }
  if (key<=0) key = keyword (yytext);
  if (key == -1) 
    {
      struct mymacro * m=(void*)find_macro(0,yytext);
      macr=m;
      if (m) { 
  	     //if (m->param==0) goto ifall;
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
      yylval.type_node_p=get_identifier(s);
      if (m==0) return T_NAME;
    }
  /*yylval.type_str=strdup(yytext);*/
  yylval.type_node_p=get_identifier(yytext);
  return key;
}

"%"{name}			{
  /* store that string w/o '' */
  if (macromode) { yylval.type_node_p = copy_node(get_identifier(strdup(yytext))); return LEXEME; }

  signed int key = -3;
  downcase (yytext);
  key = keyword (yytext);
  int key_len = strlen(yytext);
#if 0
if (key==-1)
{
fprintf(stderr, "key %x\n",key);
fflush(stderr);
}
#endif

  if (skip_mode && if_mode == 0 && key!=P_IF && key!=P_THEN && key != P_ELSE && key != P_FI) {
    yyless(yyleng);
    break;
  }

  if (key == -1) {
    goto do_name;
  }

  int retval = 0;
  char * my_yytext=yytext+strlen(yytext);
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
  char * my_end=strstr(my_yytext, "%THEN");
  int my_yyleng=5+my_end-my_yytext;
  char myline[255];
  memset(myline, 0, 255);
  printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
  memcpy(myline,my_yytext,my_end-my_yytext);
#if 0
  myline+=3;
  myline[strlen(myline)-5]=0;
#endif
  //fprintf(stderr,"myline%smyline\n",myline); fflush(stderr);
  //yytext=my_yytext+=my_yyleng;
  int len=my_end-yytext+5;
  yyless(len);
  //yyless(my_yyleng);
  printf("skipp %x\n",skip_mode);
  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(myline));
  yyparse();
  push_cond_stack(0);
  skip_mode=get_cond_stack()|integer_zerop(fold(mylast()));
  goto myout;
}
break;

case P_THEN: { if_mode = 0; then_mode = 1;
//printf("then nod %x\n" , yylval.type_node_p);
tree f=fold(yylval.type_node_p);
printf("f %x %x\n" , f, mylast());
push_cond_stack(0);
skip_mode=get_cond_stack()|integer_zerop(fold(mylast()));
printf("skip %x\n",skip_mode);
//yyless(yyleng);
se2=1;
return P_SOFTERROR3;
break;
}
break;

case P_ELSE: {
  then_mode = 0;
  skip_mode=get_cond_stack()|!skip_mode;
//fprintf(stderr,"pel %s\n",yytext); fflush(stderr); 
  yyless(5/*yyleng*/);
//fprintf(stderr,"pel2 %s\n",yytext); fflush(stderr);
  key = -1;
}
break;

case P_FI: { /*skip_mode = 0;*/ pop_cond_stack(); /*yyless(yyleng);*/ yyless(3); key=-1; }
break;

case P_QUOTE: { /* quote_mode_next = 1; */ yyless(yyleng); }
break;

case P_EXPAND: { /* quote_mode_next = 1; */ yyless(yyleng); }
break;

case P_REMAINING: { /* quote_mode_next = 1; */ yyless(yyleng); key=-1; push_macro(print_remain(remaining)); }
break;

case P_ALLOCATION: {
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, ')');
  int my_yyleng=1+my_end-my_yytext;
  char myline[255];
  printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
  memcpy(myline,my_yytext,my_end-my_yytext);
  myline[my_end-my_yytext]=0;
  //yyless(my_yyleng);
//  yytext=my_yytext+=my_yyleng;
  int len=my_end-yytext+1;
  yyless(len);
  myline[1]=0;
  myline[0]='4';
  yylval.type_node_p=copy_node(get_identifier(myline));
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
#if 0
  last_p=key;
  yyless(yyleng);
  BEGIN(p_paren);
  extern int yyrec;
  yyrec=1;
  return START_LEX;
#endif
}
break;

case P_ASSIGN: {
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');
  int len=my_end-yytext+1;
  yyless(len);
  
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
  char what[255];
  memset(what, 0, 255);
  memcpy(what, second, my_end - second);

  printf("setcti %s %s\n",myline,what);

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(what));
  yyparse();

  set_cti(get_identifier(strdup(myline)), fold(mylast()));

goto myout;
break;
}

case P_BLISS: {
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, ')');
  int my_yyleng=1+my_end-my_yytext;
  *my_end=0;
  int is32=strstr(my_yytext,"32"); // shortcut
  *my_end=')';
  char myline[255];
  printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
  memcpy(myline,my_yytext,my_end-my_yytext);
  myline[my_end-my_yytext]=0;
  //yyless(my_yyleng);
//  yytext=my_yytext+=my_yyleng;
  int len=my_end-yytext+1;
  yyless(len);
  myline[1]=0;
  if (is32)
    myline[0]='1';
  else
    myline[0]='0';
  //printf("is32 %x\n",is32);
  yylval.type_node_p=copy_node(get_identifier(myline));
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
#if 0
  last_p=key;
  yyless(yyleng);
  BEGIN(p_paren);
  extern int yyrec;
  yyrec=1;
  return START_LEX;
#endif
}
break;

case P_CHAR: {
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, ')');
  int len=my_end-yytext+1;
  yyless(len);
  
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

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(myline));
  printf("myretp %s\n",myline);
  yyparse();

  myret[myretp++]=TREE_INT_CST_LOW(fold(mylast()));

  if (*par_end==',') {
    par = par_end + 1;
    goto char_again;
  }

  myret[myretp]=39;
  char * s = strdup(myret);
  printf("myret %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));
  retval = T_STRING;

goto out_data;
break;
}
break;

case P_CHARCOUNT: { 
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(myline));
  printf("myretp %s\n",myline);
  yyparse();

  myret+=strlen(IDENTIFIER_POINTER(fold(mylast())))-2;

  if (*par_end==',') {
    par = par_end + 1;
    goto charcount_again;
  }

  memset(myline, 0, 255);
  sprintf(myline,"%d",myret);
  char * s = strdup(myline);
  printf("myret %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));
  retval = T_DIGITS;

goto out_data;
break;
}
break;

case P_CTCE: { 
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(myline));
  printf("myretp %s\n",myline);
  yyparse();

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
  printf("myret %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));
  retval = T_DIGITS;

goto out_data;
break;
}
break;

case P_EXPLODE: { 
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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
  yy_switch_to_buffer(yy_scan_string(myline2));
  printf("myexp %s\n",myline2);

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
case P_STRING: { 
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
  char myret[255];
  memset(myret, 0, 255);
  myret[0]=39;
  int myretp=1;
  char myline[255];

  char * par = my_yytext+1;
  char * par_end;

 string_again:
  par_end = par;
  while (*par_end!=',' && par_end<my_end)
    par_end++;

  memset(myline, 0, 255);
  memcpy(myline,par,par_end-par);

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(myline));
  printf("myretsp %s\n",myline);
  yyparse();

  tree t=fold(mylast());
  int v=TREE_INT_CST_LOW(t);
  char * c = &v;
  int i;
  printf("tc %x\n",TREE_CODE(t));

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
    printf("ascid stuff not yet implemented\n");
  }

  sprintf(&myret[myretp],"%d",v);
  myretp+=strlen(&myret[myretp]);

  goto str_out;

 no_int:

  {}

  char * start = IDENTIFIER_POINTER(t) + 1;
  char * end = strchr(start,39);

  memcpy(&myret[myretp],start,end-start);
  myretp+=strlen(&myret[myretp]);

  str_out:

  if (*par_end==',') {
    par = par_end + 1;
    goto string_again;
  }

  myret[myretp]=39;

  char * s = strdup(myret);
  printf("myrets %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

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
case P_STRING: { 
  retval=T_STRING;
  goto out_data;
  }
}
fn("BLS-%s-%s\n",str,s);
goto myout;
break;
}
break;

case P_EXACTSTRING: { 
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(myline));
  printf("myretsp %s\n",myline);
  yyparse();

  tree t=fold(mylast());
  int v=TREE_INT_CST_LOW(t);
  char * c = &v;
  int i;
  printf("tc %x\n",TREE_CODE(t));

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
    printf("ascid stuff not yet implemented\n");
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
  printf("myretexs %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

  retval=T_STRING;
  goto out_data;
break;
}
break;

case P_REMOVE: { 
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

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

  *my_yytext=' ';
  *my_end=' ';

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
  printf("myrem %s\n",yytext);
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
  char * my_end=strchr(my_yytext+1, 39);
  int my_yyleng=1+my_end-my_yytext;
  int len=my_end-yytext+1;
  yyless(len);
  goto myout;
}
break;

case P_DECLARED:
{
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);

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
  printf("decl %sX\n",myline);

  tree is_there = maybe_get_identifier(myline);

  myline[1]=0;
  if (is_there)
    myline[0]='1';
  else
    myline[0]='0';
  yylval.type_node_p=copy_node(get_identifier(myline));
  retval=T_DIGITS;
  goto out_data;

break;
}
break;

case P_NULL:
{
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
  char myline[255];
  int myret=0;

  char * par = my_yytext+1;
  char * par_end;

 null_again:
  par_end = par;
  while (*par_end!=',' && par_end<my_end)
    par_end++;

  memset(myline, 0, 255);
  memcpy(myline,par,par_end-par);

  if (!is_on_remaining(myline),macroact)
    myret++;

  if (*par_end==',') {
    par = par_end + 1;
    goto null_again;
  }

  if (myret==0)
    myline[0]='1';
  else
    myline[0]='0';
  myline[1]=0;

  char * s = strdup(myline);
  printf("null %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

  retval=T_DIGITS;
  goto out_data;
break;
}
break;

case P_IDENTICAL:
{
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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

  char myline1[255];
  char myline2[255];
  memset(myline1,0,255);
  memset(myline2,0,255);
  memcpy(myline1,p1,p1_end-p1);
  memcpy(myline2,p2,p2_end-p2);

  char myline[255];

  if (0==strcmp(myline1,myline2))
    myline[0]='1';
  else
    myline[0]='0';
  myline[1]=0;

  char * s = strdup(myline);
  printf("ident %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

  retval=T_DIGITS;
  goto out_data;
break;
}
break;

case P_ISSTRING:
{
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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
  printf("isstr %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

  retval=T_DIGITS;
  goto out_data;
break;
}
break;

case P_FIELDEXPAND:
{
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);
  
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

  while (my_isletter(*p2_end))
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

  if (p2)
    new = my_copy_tree(exp);
  else {
    v++;
    for (new=exp;v;v--,new=TREE_CHAIN(new)) ;
    new=copy_node(new);
  }    

  char * p = print_tree(new);

  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(p));

  goto myout;
break;
}
break;

case P_LENGTH:
{
yyless(key_len);
tree t=macroact;
int len=0;
for (;t;t=TREE_CHAIN(t),len++) ;

char myline[255];
memset(myline,0,255);
sprintf(myline,"%d",len);
  char * s = strdup(myline);
  printf("len %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

  retval=T_DIGITS;
  goto out_data;
break;
}
break;

case P_NUMBER:
{
  my_yytext=strchr(my_yytext,'(');
  char * my_end=strchr(my_yytext, '\n');

  while (*my_end!=')')
     my_end--;

  int len=my_end-yytext+1;
  yyless(len);

  my_yytext++;
  my_end++;

  tree t = parse_this_range(my_yytext,my_end);
  t=fold(t);
  int v=0;

  if (TREE_CODE(t)==IDENTIFIER_NODE) {
    char * s = IDENTIFIER_POINTER(t);
    int len=strlen(s);
    if (s[0]==39 && s[len-1]==39) {
      char * new = xstrdup(s+1);
      new[len-2]=0;
      v=atoi(new);
    } else {
printf("not yet implemnted number\n");
    }
  } else {
    v=TREE_INT_CST_LOW(t);
  }
  
char myline[255];
memset(myline,0,255);
sprintf(myline,"%d",v);
  char * s = strdup(myline);
  printf("number %s\n",s);
  yylval.type_node_p=copy_node(get_identifier(s));

  retval=T_DIGITS;
  goto out_data;
break;
}
break;

case P_COUNT:
case P_EXITITERATION:
case P_EXITMACRO:
case P_LTCE:
case P_VARIANT:
case P_UNQUOTE:
case P_SIZE:
case P_SWITCHES:
case P_NBITS:
case P_NBITSU:
case P_NAME:
case P_QUOTENAME:
{
printf("key %x not yet implemented\n", key);
}
default: {
#if 0
fprintf(stderr,"DEF %x %x %x %s\n",key,strlen(yytext),keyw_len(key),yytext);
fflush(stderr);
yyless(strlen(yytext));
yyless(keyw_len(key));
fprintf(stderr,"DEF %x %x %x %s\n",key,strlen(yytext),keyw_len(key),yytext);
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
fprintf(stderr, "key %x\n",key);
fflush(stderr);
}
#endif
//  break;

goto myout;

out_data:
  if (macromode) return LEXEME;
  return retval;

myout:
{}
}

"%"				{
  if (macromode) macromode=0;
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
  if (macromode) { yylval.type_node_p = copy_node(get_identifier(strdup(yytext))); return LEXEME; }
  m=(void*)find_macro(0,yytext);
  macr=m;
  if (m) BEGIN(macro);
#if 0
  if (m) {
    push_macro(make_macro_string(m));
    continue;                   /* not obvious at all...*/
  }
#endif
  s=strdup(yytext);
  yylval.type_node_p=copy_node(get_identifier(s));
  return T_STRING;
}

![^\n\f]*[\n\f]			/* ignore trailing comment */ input_line++ ; 

"%("([^\)]|")"[^\%])*")%"	/* ignore embedded comment */  ;

"@" { if (yy2debug) inform("%%lexical gone?\n"); }

{others}			{
  yy2error ("not recognized");
}

<<EOF>> { 
  if ( --include_stack_ptr < 0 )
    {
      if (yy2debug) inform("\nempty include stack\n");
      return YY_NULL;
    }
  else
    {
      yy_delete_buffer( YY_CURRENT_BUFFER );
      yy_switch_to_buffer(include_stack[include_stack_ptr] );
    }
    if (expr_parse) {
      expr_parse--;
      return END_EXPR;
    }
    if (do_builtin) {
      do_builtin--;
      return END_BUILTIN;
    }
} 

<macro>{delim}				{
  int newmacrocomma;
  if (*yytext=='[' || *yytext=='(' || *yytext=='<') { if (macrostart==0) macrostart=*yytext; }

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
    }
    goto out;
  ifall:
    {
      tree p = macr->param;
      tree a = macroact;
      while (a && p) {
        a = TREE_CHAIN(a);
        p = TREE_CHAIN(p);
      }
      if (p==0)
        remaining=a;
    }
    push_macro(make_macro_string(macr,macroact));
    macrostart=0;
    macr=0;
    macroact=0;
    macrocomma=1;
    BEGIN(0);
    continue;
  }
 out:

  newmacrocomma=*yytext==',';

  if (macrocomma && newmacrocomma) chainon(macroact,tree_cons(0,0,0));

  macrocomma=newmacrocomma;

}

<macro>{name}|{digit}+				{
  downcase (yytext);
  if (macrocomma==0) if (yy2debug) inform("\nmacrocomma missing %x\n",input_line);
  macroact=chainon(macroact,copy_node(get_identifier(strdup(yytext))));
  macrocomma=1;
}

<p_paren>"(" {
  yyless(yyleng);
  BEGIN(0);
  //se2=1;
  //return P_SOFTERROR3;
  continue;
}

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
   fprintf(stderr," KEY %x %s\n",strlen(keywords[key-P_ALLOCATION].d_text),(keywords[key-P_ALLOCATION].d_text));
   fflush(stderr);
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
#define NUKEYW 110

static int keyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) inform("key %x %s\n",(unsigned int)cp,cp);

  first = 0;				/* inclusive */
  last = NKEYW;				/* exclusive */
  last = sizeof keywords / sizeof (struct dict) - 2;
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
  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_string(this));
  yyparse();
  return mylast();
}

tree parse_this_n (char * this, int n) {
  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_bytes(this,n));
  yyparse();
  return mylast();
}

tree parse_this_range (char * this, char * end) {
  if (yydebug) {
    char myline[255];
    memset(myline, 0, 255);
    memcpy(myline,this,end-this);
    printf("parse_this_range %s\n",myline);
  }
  expr_parse++;
  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_scan_bytes(this,end-this));
  yyparse();
  return mylast();
}

