/*
 *  symbtab.h
 *	- en primitiv symboltabell + feilh}ndtermingsfunksjoner
 * 	  (utvidelse av symboltabellen fra oblig. |ving i Programmeringsspr}k
 *         til } h}ndtere globale og lokale navn)
 */

#ifndef SYMBTAB2_H
#define SYMBTAB2_H


#define InternalErrorMACRO  InternalError (__FILE__, __LINE__);

/* Maksimal lenge for et identifikatornavn */
#define MAX_ID_LEN	32

/* Maksimal lenge for en adresse */
#define MAX_ADDR_LEN	32

/* Maskimalt antall innslag i symboltabellen */
#define MAX_NUMB_IDS	1024


void  BindLocalVarOrParam (const char  *id, const char*  addr);
/* Binder lokalt variable eller parameter 'id' til 'addr'.
   Avslutter med feilmelding hvis det finnes en lokal variabel eller
   parameter ved navn 'id' allerede. */

void  BindGlobalVar (const char  *id, const char*  addr);
/* Binder globalt 'id' til 'addr'.
   Avslutter med feilmelding hvis 'id' allerede er bundet. */

char*  LocateId (const char  *id);
/* Returnerer adressen til 'id'.  Sjekker f|rst blant lokale variable og
   parametre, s} globale variable.  Avslutter med feilmelding hvis 'id'
   ikke er bundet hverkel lokalt eller globalt. */

void  UnbindLocalVarsOrParams (void);
/* Fjerner alle lokale varable. */

int   SemanticError (const char  *str1, const char  *str2);
/* Avslutter med meldig om semantisk feil.
   Utskrift: "Semantic error: 'str1' 'str2'" */

int   InternalError (const char*  filename, const int  code);
/* Avslutter med melding om intern feil.
   Utskrift: "INTERNAL ERROR; Code: 'code'"
   (Skriv f eks ut linjenummeret der det skar seg vha makroen __LINE__.) */

     /* Data type for links in the chain of symbols.      */
enum { enumempty, newscope, FNCT, VAR, ROU, sin };

struct symrec
{
  char *name;  /* name of symbol                     */
  int type;    /* type of symbol: either VAR or FNCT */
  char *id;
  union {
	 double var;           /* value of a VAR          */
	 double (*fnctptr)();  /* value of a FNCT         */
  } value;
  struct symrec *level;
  struct symrec *back;
  struct symrec *top;
  struct symrec *next;    /* link field              */
};

typedef struct symrec symrec;

     /* The symbol table: a chain of `struct symrec'.     */
extern symrec *sym_table;
extern symrec *cur_sym_table;
extern symrec **next_sym_table;
extern symrec *cur_top_table;
symrec *putsym ();
symrec *getsym ();
void newscosym();
void end_cur();
void printsymtab(symrec *, int);

#endif
