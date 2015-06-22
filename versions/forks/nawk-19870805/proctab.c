#include "awk.h"
#include "y.tab.h"

Cell *nullproc();
extern Cell *program();
extern Cell *boolop();
extern Cell *relop();
extern Cell *array();
extern Cell *indirect();
extern Cell *substr();
extern Cell *sub();
extern Cell *gsub();
extern Cell *sindex();
extern Cell *asprintf();
extern Cell *arith();
extern Cell *incrdecr();
extern Cell *cat();
extern Cell *pastat();
extern Cell *dopa2();
extern Cell *matchop();
extern Cell *intest();
extern Cell *aprintf();
extern Cell *print();
extern Cell *closefile();
extern Cell *delete();
extern Cell *split();
extern Cell *assign();
extern Cell *condexpr();
extern Cell *ifstat();
extern Cell *whilestat();
extern Cell *forstat();
extern Cell *dostat();
extern Cell *instat();
extern Cell *jump();
extern Cell *bltin();
extern Cell *call();
extern Cell *arg();
extern Cell *getnf();
extern Cell *getline();
static uchar *printname[93] = {
	(uchar *) "FIRSTTOKEN",	/* 257 */
	(uchar *) "FATAL",	/* 258 */
	(uchar *) "PROGRAM",	/* 259 */
	(uchar *) "PASTAT",	/* 260 */
	(uchar *) "PASTAT2",	/* 261 */
	(uchar *) "XBEGIN",	/* 262 */
	(uchar *) "XEND",	/* 263 */
	(uchar *) "NL",	/* 264 */
	(uchar *) "ARRAY",	/* 265 */
	(uchar *) "MATCH",	/* 266 */
	(uchar *) "NOTMATCH",	/* 267 */
	(uchar *) "MATCHOP",	/* 268 */
	(uchar *) "FINAL",	/* 269 */
	(uchar *) "DOT",	/* 270 */
	(uchar *) "ALL",	/* 271 */
	(uchar *) "CCL",	/* 272 */
	(uchar *) "NCCL",	/* 273 */
	(uchar *) "CHAR",	/* 274 */
	(uchar *) "OR",	/* 275 */
	(uchar *) "STAR",	/* 276 */
	(uchar *) "QUEST",	/* 277 */
	(uchar *) "PLUS",	/* 278 */
	(uchar *) "AND",	/* 279 */
	(uchar *) "BOR",	/* 280 */
	(uchar *) "APPEND",	/* 281 */
	(uchar *) "EQ",	/* 282 */
	(uchar *) "GE",	/* 283 */
	(uchar *) "GT",	/* 284 */
	(uchar *) "LE",	/* 285 */
	(uchar *) "LT",	/* 286 */
	(uchar *) "NE",	/* 287 */
	(uchar *) "IN",	/* 288 */
	(uchar *) "ARG",	/* 289 */
	(uchar *) "BLTIN",	/* 290 */
	(uchar *) "BREAK",	/* 291 */
	(uchar *) "CLOSE",	/* 292 */
	(uchar *) "CONTINUE",	/* 293 */
	(uchar *) "DELETE",	/* 294 */
	(uchar *) "DO",	/* 295 */
	(uchar *) "EXIT",	/* 296 */
	(uchar *) "FOR",	/* 297 */
	(uchar *) "FUNC",	/* 298 */
	(uchar *) "SUB",	/* 299 */
	(uchar *) "GSUB",	/* 300 */
	(uchar *) "IF",	/* 301 */
	(uchar *) "INDEX",	/* 302 */
	(uchar *) "LSUBSTR",	/* 303 */
	(uchar *) "MATCHFCN",	/* 304 */
	(uchar *) "NEXT",	/* 305 */
	(uchar *) "ADD",	/* 306 */
	(uchar *) "MINUS",	/* 307 */
	(uchar *) "MULT",	/* 308 */
	(uchar *) "DIVIDE",	/* 309 */
	(uchar *) "MOD",	/* 310 */
	(uchar *) "ASSIGN",	/* 311 */
	(uchar *) "ASGNOP",	/* 312 */
	(uchar *) "ADDEQ",	/* 313 */
	(uchar *) "SUBEQ",	/* 314 */
	(uchar *) "MULTEQ",	/* 315 */
	(uchar *) "DIVEQ",	/* 316 */
	(uchar *) "MODEQ",	/* 317 */
	(uchar *) "POWEQ",	/* 318 */
	(uchar *) "PRINT",	/* 319 */
	(uchar *) "PRINTF",	/* 320 */
	(uchar *) "SPRINTF",	/* 321 */
	(uchar *) "ELSE",	/* 322 */
	(uchar *) "INTEST",	/* 323 */
	(uchar *) "CONDEXPR",	/* 324 */
	(uchar *) "POSTINCR",	/* 325 */
	(uchar *) "PREINCR",	/* 326 */
	(uchar *) "POSTDECR",	/* 327 */
	(uchar *) "PREDECR",	/* 328 */
	(uchar *) "VAR",	/* 329 */
	(uchar *) "IVAR",	/* 330 */
	(uchar *) "VARNF",	/* 331 */
	(uchar *) "CALL",	/* 332 */
	(uchar *) "NUMBER",	/* 333 */
	(uchar *) "STRING",	/* 334 */
	(uchar *) "FIELD",	/* 335 */
	(uchar *) "REGEXPR",	/* 336 */
	(uchar *) "GETLINE",	/* 337 */
	(uchar *) "RETURN",	/* 338 */
	(uchar *) "SPLIT",	/* 339 */
	(uchar *) "SUBSTR",	/* 340 */
	(uchar *) "WHILE",	/* 341 */
	(uchar *) "CAT",	/* 342 */
	(uchar *) "NOT",	/* 343 */
	(uchar *) "UMINUS",	/* 344 */
	(uchar *) "POWER",	/* 345 */
	(uchar *) "DECR",	/* 346 */
	(uchar *) "INCR",	/* 347 */
	(uchar *) "INDIRECT",	/* 348 */
	(uchar *) "LASTTOKEN",	/* 349 */
};


Cell *(*proctab[93])() = {
	nullproc,	/* FIRSTTOKEN */
	nullproc,	/* FATAL */
	program,	/* PROGRAM */
	pastat,	/* PASTAT */
	dopa2,	/* PASTAT2 */
	nullproc,	/* XBEGIN */
	nullproc,	/* XEND */
	nullproc,	/* NL */
	array,	/* ARRAY */
	matchop,	/* MATCH */
	matchop,	/* NOTMATCH */
	nullproc,	/* MATCHOP */
	nullproc,	/* FINAL */
	nullproc,	/* DOT */
	nullproc,	/* ALL */
	nullproc,	/* CCL */
	nullproc,	/* NCCL */
	nullproc,	/* CHAR */
	nullproc,	/* OR */
	nullproc,	/* STAR */
	nullproc,	/* QUEST */
	nullproc,	/* PLUS */
	boolop,	/* AND */
	boolop,	/* BOR */
	nullproc,	/* APPEND */
	relop,	/* EQ */
	relop,	/* GE */
	relop,	/* GT */
	relop,	/* LE */
	relop,	/* LT */
	relop,	/* NE */
	instat,	/* IN */
	arg,	/* ARG */
	bltin,	/* BLTIN */
	jump,	/* BREAK */
	closefile,	/* CLOSE */
	jump,	/* CONTINUE */
	delete,	/* DELETE */
	dostat,	/* DO */
	jump,	/* EXIT */
	forstat,	/* FOR */
	nullproc,	/* FUNC */
	sub,	/* SUB */
	gsub,	/* GSUB */
	ifstat,	/* IF */
	sindex,	/* INDEX */
	nullproc,	/* LSUBSTR */
	matchop,	/* MATCHFCN */
	jump,	/* NEXT */
	arith,	/* ADD */
	arith,	/* MINUS */
	arith,	/* MULT */
	arith,	/* DIVIDE */
	arith,	/* MOD */
	assign,	/* ASSIGN */
	nullproc,	/* ASGNOP */
	assign,	/* ADDEQ */
	assign,	/* SUBEQ */
	assign,	/* MULTEQ */
	assign,	/* DIVEQ */
	assign,	/* MODEQ */
	assign,	/* POWEQ */
	print,	/* PRINT */
	aprintf,	/* PRINTF */
	asprintf,	/* SPRINTF */
	nullproc,	/* ELSE */
	intest,	/* INTEST */
	condexpr,	/* CONDEXPR */
	incrdecr,	/* POSTINCR */
	incrdecr,	/* PREINCR */
	incrdecr,	/* POSTDECR */
	incrdecr,	/* PREDECR */
	nullproc,	/* VAR */
	nullproc,	/* IVAR */
	getnf,	/* VARNF */
	call,	/* CALL */
	nullproc,	/* NUMBER */
	nullproc,	/* STRING */
	nullproc,	/* FIELD */
	nullproc,	/* REGEXPR */
	getline,	/* GETLINE */
	jump,	/* RETURN */
	split,	/* SPLIT */
	substr,	/* SUBSTR */
	whilestat,	/* WHILE */
	cat,	/* CAT */
	boolop,	/* NOT */
	arith,	/* UMINUS */
	arith,	/* POWER */
	nullproc,	/* DECR */
	nullproc,	/* INCR */
	indirect,	/* INDIRECT */
	nullproc,	/* LASTTOKEN */
};

uchar *tokname(n)
{
	static uchar buf[100];

	if (n < FIRSTTOKEN || n > LASTTOKEN) {
		sprintf(buf, "token %d", n);
		return buf;
	}
	return printname[n-257];
}
