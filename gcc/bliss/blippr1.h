#ifndef YYSTYPE
#define YYSTYPE int
#endif

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	PREC_LESS_LPAREN	257
#define	PREC_LESS_PLUS_MINUS	258
#define	LPAREN	259
#define	NEWLINE	260
#define	BLANKS	261
#define	SEMICOLON_SPACE	262
#define	SEMICOLON_NOSPACE	263
#define	COMMA_SPACE	264
#define	COMMA_NOSPACE	265
#define	COMMENT	266
#define	NON_COMMENT	267
#define	COMMENT_ENTRY_PARA	268
#define	PERIOD_SPACE	269
#define	PERIOD_NOSPACE	270
#define	STRING	271
#define	STRING_UNTERM	272
//#define	UMINUS	273
//#define	UPLUS	274
#define	ASTER_ASTER	275
#define	ASTER	276
#define	SLASH	277
#define	PLUS	278
#define	MINUS	279
#define	EQUAL	280
#define	RPAREN	281
#define	COLON	282
#define	GE	283
#define	LE	284
#define	GT	285
#define	LT	286
#define	PSEUDOTEXTDELIM	287
#define	NUMBER	288
#define	NUMBERORNAMEORPIC	289
#define	NUMBERORNAMEORPIC_IDENTIFIER_BASE	290
#define	NUMBERORNAMEORPIC_REF_MOD_ONLY	291
#define	NUMBERORNAMEORPIC_ARRAY_REF_MOD	292
#define	ERROR_UNRECOGNISED_BLI_CHARACTER	293
#define	AND_CHAR	294
#define	COPY	295
#define	OF	296
#define	IN	297
#define	BY	298
#define	REPLACING	299
#define	REPLACE	300
#define	OFF	301
#define	FUNCTION	302
#define	EOF_symbol	303


extern YYSTYPE yy2lval;
