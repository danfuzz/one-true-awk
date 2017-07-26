%token	FIRSTTOKEN	/* must be first */
%token	FATAL
%token	PROGRAM PASTAT PASTAT2 XBEGIN XEND
%token	NL
%token	LT LE GT GE EQ NE APPEND
%token	MATCH NOTMATCH
%token	FINAL DOT ALL CCL NCCL CHAR OR STAR QUEST PLUS
%token	ADD MINUS MULT DIVIDE MOD
%token	ASSIGN ADDEQ SUBEQ MULTEQ DIVEQ MODEQ POWEQ
%token	PRINT PRINTF
%token	DELETE
%token	IF ELSE WHILE FOR IN INTEST NEXT EXIT BREAK CONTINUE

%right	ASGNOP
%left	BOR
%left	AND
%nonassoc RELOP MATCHOP
%left	CAT
%left	'+' '-'
%left	'*' '/' '%'
%left	NOT UMINUS
%right	POWER
%left	POSTINCR PREINCR POSTDECR PREDECR INCR DECR
%left	FIELD INDIRECT SUB GSUB SUBSTR LSUBSTR INDEX CALL RETURN FUNC BLTIN
%left	SPRINTF GETLINE CLOSE
%token	STRING NUMBER REGEXPR VAR ARG VARNF ARRAY SPLIT

%token	LASTTOKEN	/* must be last */

%{
#include "awk.h"
yywrap() { return(1); }
#ifndef	DEBUG
#	define	PUTS(x)
#endif
Node	*beginloc = 0, *endloc = 0;
int	infunc	= 0;	/* = 1 if in arglist or body of func */
char	*curfname = 0;
Node	*arglist = 0;	/* list of args for current function */
%}
%%

program:
	  pa_stats { if (errorflag==0)
			winner = (Node *)stat3(PROGRAM, beginloc, $1, endloc); }
	| error    { yyclearin; bracecheck(); yyerror("bailing out"); }
	;

andNL:
	  AND | andNL NL
	;

begin:
	  XBEGIN '{' stmtlist '}'	{ beginloc = (Node*) linkum(beginloc,(Node *) $3); }
	;

borNL:
	  BOR | borNL NL
	;

commaNL:
	  ',' | commaNL NL
	;

compound_conditional:
	  conditional borNL conditional	{ $$ = op2(BOR, $1, $3); }
	| conditional andNL conditional	{ $$ = op2(AND, $1, $3); }
	| NOT conditional		{ $$ = op1(NOT, $2); }
	| '(' compound_conditional ')'	{ $$ = $2; }
	;

compound_pattern:
	  pattern borNL pattern		{ $$ = op2(BOR, $1, $3); }
	| pattern andNL pattern		{ $$ = op2(AND, $1, $3); }
	| NOT pattern			{ $$ = op1(NOT, $2); }
	| '(' compound_pattern ')'	{ $$ = $2; }
	;

conditional:
	  expr		{ $$ = op2(NE,$1,valtonode(lookup("$zero&null",symtab),CCON)); }
	| compound_conditional	 
	;

cond_expr:
	  rel_expr
	| lex_expr
	| in_expr
	;

easy_expr:
	  term
	| easy_expr term %prec CAT	{ $$ = op2(CAT, $1, $2); }
	| var ASGNOP easy_expr		{ $$ = op2($2, $1, $3); }
	;

elseNL:
	  ELSE | elseNL NL
	;

end:
	  XEND '{' stmtlist '}'		{ endloc = (Node*) linkum(endloc,(Node *) $3); }
	;

expr:
	  easy_expr
	| getpipe
	| cond_expr
	;

exprlist:
	  expr
	| exprlist commaNL expr	{ $$ = linkum($1, $3); }
	;

field:
	  FIELD			{ $$ = valtonode($1, CFLD); }
	| INDIRECT term 	{ $$ = op1(INDIRECT, $2); }
	;

for:
	  FOR '(' simple_stmt ';' conditional ';' simple_stmt rparenNL stmt
		{ $$ = stat4(FOR, $3, $5, $7, $9); }
	| FOR '(' simple_stmt ';'  ';' simple_stmt rparenNL stmt
		{ $$ = stat4(FOR, $3, 0, $6, $8); }
	| FOR '(' varname IN varname rparenNL stmt
		{ $$ = stat3(IN, $3, makearr($5), $7); }
	;

func:
	  FUNC funcname '(' varlist rparenNL {infunc++;} '{' stmtlist '}'
		{ infunc--; curfname=0; defn($2, $4, $8); }
	;

funcname:
	  VAR	{ setfname($1); }
	| CALL	{ setfname($1); }
	;

getpipe:
	  easy_expr '|' GETLINE var	{ $$ = op3(GETLINE, $4, $2, $1); }
	| easy_expr '|' GETLINE		{ $$ = op3(GETLINE, 0, $2, $1); }
	;

if:
	  IF '(' conditional rparenNL	{ $$ = $3; }
	;

in_expr:
	  expr IN varname	{ $$ = op2(INTEST, $1, makearr($3)); }
	| '(' in_expr ')'	{ $$ = $2; }
	;

lex_expr:
	  expr MATCHOP reg_expr	{ $$ = op3($2, 0, $1, makedfa(reparse($3), 0)); }
	| expr MATCHOP expr
		{ if (((Node*)$3)->ntype == NVALUE && ((Cell*)((Node*)$3)->narg[0])->csub == CCON)
			$$ = op3($2, 0, $1, makedfa(reparse( ((Cell*)((Node*)$3)->narg[0])->sval)), 0);
		  else
			$$ = op3($2, 1, $1, $3); }
	| '(' lex_expr ')'	{ $$ = $2; }
	;

opt_plist:
	  /* nothing */		{ $$ = rectonode(); }
	| plist
	;

pa_stat:
	  pattern			{ $$ = stat2(PASTAT, $1, stat2(PRINT, rectonode(), 0)); }
	| pattern '{' stmtlist '}'	{ $$ = stat2(PASTAT, $1, $3); }
	| pattern ',' pattern		{ $$ = pa2stat($1, $3, stat2(PRINT, rectonode(), 0)); }
	| pattern ',' pattern '{' stmtlist '}'	{ $$ = pa2stat($1, $3, $5); }
	| '{' stmtlist '}'		{ $$ = stat2(PASTAT, 0, $2); }
	;

pa_stats:
	  /* nothing */		{ $$ = (int)0; }
	| pa_stats NL
	| pa_stats pa_stat	{ $$ = linkum($1, $2); }
	| pa_stats begin
	| pa_stats end
	| pa_stats func
	;


pattern:
	  reg_expr	 { $$ = op3(MATCH, 0, rectonode(), makedfa(reparse($1), 0)); }
	| rel_expr
	| lex_expr
	| in_expr
	| compound_pattern
	;

plist:
	  easy_expr
	| plist commaNL easy_expr	{ $$ = linkum($1, $3); }
	| '(' plist ')'		{ $$ = $2; }
	;

redir:
	  GT | APPEND | LT | '|'
	;

reg_expr:
	  '/' {startreg();} REGEXPR '/'		{ $$ = $3; }
	;

rel_expr:
	  expr relop expr	{ $$ = op2($2, $1, $3); }
	| '(' rel_expr ')'	{ $$ = $2; }
	;

relop:
	  EQ | NE | LT | LE | GT | GE
	;

rparenNL:
	  ')' | rparenNL NL
	;

simple_stmt:
	  PRINT opt_plist redir expr	{ $$ = stat3($1, $2, $3, $4); }
	| PRINT opt_plist		{ $$ = stat3($1, $2, 0, 0); }
	| PRINTF plist redir expr	{ $$ = stat3($1, $2, $3, $4); }
	| PRINTF plist			{ $$ = stat3($1, $2, 0, 0); }
	| DELETE varname '[' expr ']'	{ $$ = stat2(DELETE, makearr($2), $4); }
	| expr				{ $$ = exptostat($1); }
	| /* nothing */			{ $$ = (int)0; }
	| error		{ yyclearin; yyerror("illegal statement"); }
	;

st:
	  NL | ';'
	;

stmt:
	  simple_stmt st
	| RETURN st		{ $$ = stat1(RETURN, 0); }
	| RETURN expr st	{ $$ = stat1(RETURN, $2); }
	| NEXT st		{ $$ = stat1(NEXT, 0); }
	| EXIT st		{ $$ = stat1(EXIT, 0); }
	| EXIT expr st		{ $$ = stat1(EXIT, $2); }
	| BREAK st		{ $$ = stat1(BREAK, 0); }
	| CONTINUE st		{ $$ = stat1(CONTINUE, 0); }
	| '{' stmtlist '}'	{ $$ = $2; }
	| if stmt		{ $$ = stat3(IF, $1, $2, 0); }
	| if stmt elseNL stmt	{ $$ = stat3(IF, $1, $2, $4); }
	| while stmt		{ $$ = stat2(WHILE, $1, $2); }
	| for
	| varname st		{ yyerror("no-effect statement"); $$ = 0; }
	;

stmtlist:
	  /* nothing */		{ $$ = (int) 0; }
	| stmtlist NL
	| stmtlist ';'
	| stmtlist stmt		{ $$ = linkum($1, $2); }
	;

subop:
	  SUB | GSUB
	;

term:
	  var
	| CALL '(' ')'			{ $$ = op2(CALL, valtonode($1,CVAR), 0); }
	| CALL '(' exprlist ')'		{ $$ = op2(CALL, valtonode($1,CVAR), $3); }
	| NUMBER			{ $$ = valtonode($1, CCON); }
	| STRING	 		{ $$ = valtonode($1, CCON); }
	| CLOSE easy_expr		{ $$ = op1(CLOSE, $2); }
	| GETLINE var LT easy_expr	{ $$ = op3(GETLINE, $2, $3, $4); }
	| GETLINE LT easy_expr		{ $$ = op3(GETLINE, 0, $2, $3); }
	| GETLINE var			{ $$ = op3(GETLINE, $2, 0, 0); }
	| GETLINE			{ $$ = op3(GETLINE, 0, 0, 0); }
	| BLTIN				{ $$ = op2(BLTIN, $1, rectonode()); }
	| BLTIN '(' ')'			{ $$ = op2(BLTIN, $1, rectonode()); }
	| BLTIN '(' exprlist ')'	{ $$ = op2(BLTIN, $1, $3); }
	| SPRINTF '(' plist ')'		{ $$ = op1($1, $3); }
	| SUBSTR '(' expr commaNL expr commaNL expr ')' { $$ = op3(SUBSTR, $3, $5, $7); }
	| SUBSTR '(' expr commaNL expr ')'		{ $$ = op3(SUBSTR, $3, $5, 0); }
	| subop '(' reg_expr commaNL expr ')'		{ $$ = op4($1, 0, makedfa(reparse($3), 1), $5, rectonode()); }
	| subop '(' expr commaNL expr ')'		{ $$ = op4($1, 1, $3, $5, rectonode()); }
	| subop '(' reg_expr commaNL expr commaNL expr ')' { $$ = op4($1, 0, makedfa(reparse($3), 1), $5, $7); }
	| subop '(' expr commaNL expr commaNL expr ')'	{ $$ = op4($1, 1, $3, $5, $7); }
	| SPLIT '(' expr commaNL varname commaNL expr ')' { $$ = op3(SPLIT, $3, makearr($5), $7); }
	| SPLIT '(' expr commaNL varname ')'		{ $$ = op3(SPLIT, $3, makearr($5), 0); }
	| INDEX '(' expr commaNL expr ')'		{ $$ = op2(INDEX, $3, $5); }
	| '(' expr ')'		{ $$ = $2; }
	| term '+' term		{ $$ = op2(ADD, $1, $3); }
	| term '-' term		{ $$ = op2(MINUS, $1, $3); }
	| term '*' term		{ $$ = op2(MULT, $1, $3); }
	| term '/' term		{ $$ = op2(DIVIDE, $1, $3); }
	| term '%' term		{ $$ = op2(MOD, $1, $3); }
	| term POWER term	{ $$ = op2(POWER, $1, $3); }
	| '-' term %prec UMINUS	{ $$ = op1(UMINUS, $2); }
	| '+' term %prec UMINUS	{ $$ = $2; }
	| INCR var		{ $$ = op1(PREINCR, $2); }
	| DECR var		{ $$ = op1(PREDECR, $2); }
	| var INCR		{ $$ = op1(POSTINCR, $1); }
	| var DECR		{ $$ = op1(POSTDECR, $1); }
	;

var:
	  varname
	| varname '[' expr ']'	{ $$ = op2(ARRAY, makearr($1), $3); }
	| field
	;

varlist:
	  /* nothing */		{ arglist = (Node *)($$ = 0); }
	| VAR			{ $$ = valtonode($1,CVAR); arglist = (Node*) $$; }
	| varlist commaNL VAR	{ $$ = linkum($1,valtonode($3,CVAR)); arglist = (Node*) $$; }
	;

varname:
	  VAR			{ $$ = valtonode($1, CVAR); }
	| ARG 			{ $$ = op1(ARG, $1); }
	| VARNF			{ $$ = op1(VARNF, $1); }
	;


while:
	  WHILE '(' conditional rparenNL	{ $$ = $3; }
	;

%%

setfname(p)
	Cell *p;
{
	extern int bracecnt, brackcnt, parencnt;

	curfname = p->nval;
	/*bracecnt = brackcnt = parencnt = 0;*/
}
