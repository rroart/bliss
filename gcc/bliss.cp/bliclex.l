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

//#define YYSTYPE struct bli_token_struct

extern int yydebug;
extern int yy2debug;

char * s;

//#define do_build_string(c) do { c=strdup(yytext); yylval=build_string(strlen(c),c); } while(0);

#if 0
static inline char *strdup(const char *s1) {
return strdup(s1);
}
#endif

//#include "blir_temp_config.h"
#include "config.h"
#include <stdio.h>
//#include <ctype.h>
//#include "gansidecl.h"

#undef IN_GCC

#include "system.h"
#include "tree.h"

//#include "blidebug.h"
//#include "blicc1.h"
//#include "ignorance.h"
//#include "bliclex.h"
//#include "bliumem.h"
//#include "bliumsg.h"
#include "blicsyt.h"
//#include "blicprs.h"

#include "parse.h"

//#define yylval bli_lval
//#define YYSTYPE int
//#define YYSTYPE struct token_struct
YYSTYPE yylval;
//static YYSTYPE yy2lval;
static unsigned char *upper_work = NULL;
static unsigned int upper_work_length = 0;
unsigned int intern_hash_size=0;
unsigned int estimated_token_count=0;
//struct intern_hash_entry ** GTY ((length ("intern_hash_size"))) intern_hash_table = NULL;

char *p_title;
  int linenumb;
  int undefmode=0;
  int anymode=0;
  int tnamemode=0;

#ifndef NOYTABH
//#include "blippr1.h"
#endif
//#include <string.h>
//#include <strings.h>
//#include <ctype.h>
//#include "blicc1.h"

int blipre_lineno=1;
int blipre_charno=1;
int blipre_debug_line=0; 

static struct file_struct * save_file;
#define LEXINIT_INVALID 3
static int blipre_lexinit=LEXINIT_INVALID;
int which_lex=LEXINIT_INVALID;
static int next_blicc1_lineno=1;
static int next_blicc1_charno=1;
static int next_blicc1_debug_line=0;
static int first_time_flag=1;
static int next_char_offset=0;

struct bli_token_struct * first_token=NULL;

//#define yylval yy2lval
//extern YYSTYPE yy2lval;

  static int keyword ();
  static int ukeyword ();
  static void downcase ();

static char * save_filename=NULL;

/* file length in bytes */
static int save_buffer_length=0;

/* file buffer */
static char * save_buffer=NULL;

%}

idigit		[0-9a-f]
digit		[0-9]
letter		[A-Za-z_$]
string		'([^'\n\f]|'')*'
name		{letter}({letter}|{digit})*
delim   "="|":"|","|"/"|"."|"<"|"["|"("|"-"|"*"|"+"|"^"|">"|"]"|")"|";"
others	  \"|"#"|"&"|"?"|"@"|\\|"`"|"{"|"|"|"}"|"~"
any        {digit}|{letter}|{delim}|"%"
/*optarg "+"|"-"
hexdigit [0-9a-f]
*/

%%
   
{any}+ { if (anymode) { 

if (yy2debug) fprintf(stderr, "\nanymode %s\n",yytext); 
//yylval.type_str = strdup(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
return T_NAME;
} else REJECT;  }

\n            linenumb++,lineno++;

{delim}				{
  /*yylval.type_str = strdup(yytext);*/
  return *yytext;
}

{digit}+			{
//  yylval.type_int = atoi(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
  return T_DIGITS;
}

"%TITLE".{string} {  p_title=strdup(yytext); if (yy2debug) fprintf(stderr,"Title: %s\n\n",p_title); yyless(yyleng); }

"%SBTTL".{string} { yyless(yyleng); }

"%TITLE"{string} { p_title=strdup(yytext); if (yy2debug) fprintf(stderr,"Title: %s\n\n",p_title); yyless(yyleng); }

{name}				{
  int key=0;

  downcase (yytext);
  if (tnamemode) goto onlytname;
  if (undefmode) { key=ukeyword(yytext); /*undefmode=0;*/ }
  if (key<=0) key = keyword (yytext);
  if (key == -1) 
	 {
	 onlytname:
//		yylval.type_str=strdup(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
		return T_NAME;
	 }
  /*yylval.type_str=strdup(yytext);*/
  yylval.type_node_p=get_identifier(yytext);
  return key;
}

"%"{name}			{
  int key;

  downcase (yytext);
  key = keyword (yytext);
  if (key == -1)
	 yy2error ("no %");
  switch (key) {
  case P_TITLE:
	 break;
  }
  return key;
}

"%"				{
  yylval.type_str = strdup(yytext);
  return '%';
}

{string}			{
  /* store that string w/o '' */
//  yylval.type_str = strdup(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
  return T_STRING;
}

![^\n\f]*[\n\f]			/* ignore trailing comment */  ;

"%("([^\)]|")"[^\%])*")%"	/* ignore embedded comment */  ;

"@" { if (yy2debug) fprintf(stderr,"%%lexical gone?\n"); }

{others}			{
  yy2error ("not recognized");
}


%%


struct dict {
  char *d_text;
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
  { "%allocation", P_ALLOCATION },	/* percent names */
  { "%ascic", P_ASCIC },
  { "%ascid", P_ASCID },
  { "%ascii", P_ASCII },
  { "%asciz", P_ASCIZ },
  { "%assign", P_ASSIGN },
  { "%b", P_B },
  { "%bliss", P_BLISS },
  { "%bliss16", P_BLISS16 },
  { "%bliss32", P_BLISS32 },
  { "%bliss36", P_BLISS36 },
  { "%bpaddr", P_BPADDR },
  { "%bpunit", P_BPUNIT },
  { "%bpval", P_BPVAL },
  { "%c", P_C },
  { "%char", P_CHAR },
  { "%charcount", P_CHARCOUNT },
  { "%chesc", P_CHESC },		/* Is this a keyword? */
  { "%count", P_COUNT },
  { "%ctce", P_CTCE },
  { "%d", P_D },
  { "%decimal", P_DECIMAL },
  { "%declared", P_DECLARED },
  { "%e", P_E },
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
  { "%g", P_G },
  { "%h", P_H },
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
  { "%o", P_O },
  { "%p", P_P },
  { "%print", P_PRINT },
  { "%quote", P_QUOTE },
  { "%quotename", P_QUOTENAME },
  { "%rad50_10", P_RAD50_10 },
  { "%rad50_11", P_RAD50_11 },
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
  { "%title", P_TITLE },
  { "%unquote", P_UNQUOTE },
  { "%upval", P_UPVAL },
  { "%variant", P_VARIANT },
  { "%warn", P_WARN },
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

  if (yy2debug) fprintf(stderr, "key %x %s\n",cp,cp);

  first = 0;				/* inclusive */
  last = NKEYW;				/* exclusive */
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
  if (yy2debug) fprintf(stderr, "keynot\n");
  return -1;				/* Not a keyword */
}

static int ukeyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) fprintf(stderr, "ukey %x %s\n",cp,cp);
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
  if (yy2debug) fprintf(stderr, "ukeynot\n");
  return -1;				/* Not a keyword */
}

/*
{idigit}+			{
//yylval.type_str=strdup(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
				  return T_IDIGITS;
				}
*/
/*
{any}+ {
if (anymode) {
anymode=0;
if (yy2debug) fprintf(stderr, "anymode\n");
//yylval.type_str=strdup(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
				     return T_NAME;
   }
else REJECT;
}
*/
/*
  {others} {
  if (yy2debug) fprintf(stderr, "anymode\n"); 
//  yylval.type_str = strdup(yytext);
s=strdup(yytext);
yylval.type_node_p=get_identifier(s);
  return T_NAME;
  }

*/

#if 0
void 
yylexinit(char * filename, char * buffer, int length) 
{
  save_filename=filename;  
  save_buffer=buffer;
  save_buffer_length=length;
  first_time_flag=1;
  next_char_offset=0;
  next_blicc1_lineno=1;
  next_blicc1_charno=1;
  next_blicc1_debug_line=0;
}
#endif

void 
lexinit PARAMS((int lexinit_value, struct file_struct * file_ptr)) 
//lexinit2 PARAMS((char * filename, char * buffer, int length)) 
{
   save_file=file_ptr;
   blipre_lexinit=lexinit_value;   
   which_lex=lexinit_value;   
}

void 
yylexinit(unsigned char * filename, unsigned char * buffer, unsigned length) 
{
  save_filename=filename;  
  save_buffer=buffer;
  save_buffer_length=length;
  first_time_flag=1;
  next_char_offset=0;
  next_blicc1_lineno=1;
  next_blicc1_charno=1;
  next_blicc1_debug_line=0;
}

struct bli_token_struct *
initial_lexical_analysis(unsigned int * token_count)
{
  
  int my_token_count = 0;
  struct bli_token_struct *last_token = NULL;
  struct bli_token_struct *new_token = NULL;

  /* Init interned string storage.  */
  
  intern_string_init ();

  /* Call flex and link in the tokens.  */
  
  while (yylex ())
    {
      my_token_count ++;
      new_token = malloc (sizeof (struct bli_token_struct));
//      memcpy (new_token, ((struct bli_token_struct *)(&yylval)), sizeof (struct bli_token_struct));
//      new_token->type_int=yylval.type_int;
//      BLI_ASSERT (new_token->string_details);

      if (first_token)
        {
        //  BLI_ASSERT (last_token);
          last_token->bli_token_next = new_token;
          new_token->bli_token_prev = last_token;
        //  BLI_ASSERT (new_token->bli_token_prev);
        //  BLI_ASSERT (new_token->bli_token_prev != new_token);
        //  BLI_ASSERT (new_token->bli_token_prev->bli_token_next == new_token);
        }
      else
        first_token = new_token;

      last_token = new_token;
    }

  upper_work = NULL;
  upper_work_length = 0;

  * token_count = my_token_count;
  
  /* Wrap interned string storage.  */
  
  intern_string_wrap ();
  
  return first_token;

}

int yywrap (void) { }
int
z1zwrap(void)
{
}

void
intern_string_init ()
{
  estimated_token_count = save_buffer_length / 50;  /* One unique token per line should be enough.  */
  if (estimated_token_count < 1009) /* Prime numbers.  */
    intern_hash_size = 1009;
  else
    if (estimated_token_count < 10007)
      intern_hash_size = 10007;
  else
    intern_hash_size = 100003;

  //intern_hash_table = BLI_ALLOC (intern_hash_size * sizeof (struct inter_hash_entry *));
}
    

/* Wrap up string interning.  */

void
intern_string_wrap ()
{
  //intern_hash_table = NULL;
}

tree *bli_ridpointers;
#define N_reswords (sizeof keywords / sizeof (struct dict))
bli_init_reswords ()
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
      //if (reswords[i].disable & mask)
      //  continue;

      id = get_identifier (keywords[i].d_text);
      // C_RID_CODE (id) = keywords[i].rid;
      //C_IS_RESERVED_WORD (id) = 1;
      //TREE_LANG_FLAG_0 (id) = 1;
      //id->common.lang_flag_0 = 1;
      bli_ridpointers [(int) keywords[i].d_num] = id;
    }
}
