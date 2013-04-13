#ifndef lint
static char sccsid[] = "@(#)token.c	4.2 8/11/83";
#endif

#include "awk.h"
struct tok
{	char *tnm;
	int yval;
} tok[]	= {
"FIRSTTOKEN", 257,
"FINAL", 258,
"FATAL", 259,
"LT", 260,
"LE", 261,
"GT", 262,
"GE", 263,
"EQ", 264,
"NE", 265,
"MATCH", 266,
"NOTMATCH", 267,
"APPEND", 268,
"ADD", 269,
"MINUS", 270,
"MULT", 271,
"DIVIDE", 272,
"MOD", 273,
"UMINUS", 274,
"ASSIGN", 275,
"ADDEQ", 276,
"SUBEQ", 277,
"MULTEQ", 278,
"DIVEQ", 279,
"MODEQ", 280,
"JUMP", 281,
"XBEGIN", 282,
"XEND", 283,
"NL", 284,
"PRINT", 285,
"PRINTF", 286,
"SPRINTF", 287,
"SPLIT", 288,
"IF", 289,
"ELSE", 290,
"WHILE", 291,
"FOR", 292,
"IN", 293,
"NEXT", 294,
"EXIT", 295,
"BREAK", 296,
"CONTINUE", 297,
"PROGRAM", 298,
"PASTAT", 299,
"PASTAT2", 300,
"ASGNOP", 301,
"BOR", 302,
"AND", 303,
"NOT", 304,
"NUMBER", 305,
"VAR", 306,
"ARRAY", 307,
"FNCN", 308,
"SUBSTR", 309,
"LSUBSTR", 310,
"INDEX", 311,
"GETLINE", 312,
"RELOP", 313,
"MATCHOP", 314,
"OR", 315,
"STRING", 316,
"DOT", 317,
"CCL", 318,
"NCCL", 319,
"CHAR", 320,
"CAT", 321,
"STAR", 322,
"PLUS", 323,
"QUEST", 324,
"POSTINCR", 325,
"PREINCR", 326,
"POSTDECR", 327,
"PREDECR", 328,
"INCR", 329,
"DECR", 330,
"FIELD", 331,
"INDIRECT", 332,
"LASTTOKEN", 333,
};
ptoken(n)
{
	if(n<128) printf("lex: %c\n",n);
	else	if(n<=256) printf("lex:? %o\n",n);
	else	if(n<LASTTOKEN) printf("lex: %s\n",tok[n-257].tnm);
	else	printf("lex:? %o\n",n);
	return;
}

char *tokname(n)
{
	if (n<=256 || n >= LASTTOKEN)
		n = 257;
	return(tok[n-257].tnm);
}
