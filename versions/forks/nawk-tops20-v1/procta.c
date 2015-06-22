#include "awk.h"
#include "ytab.h"

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
	(uchar *) "FINAL",	/* 268 */
	(uchar *) "DOT",	/* 269 */
	(uchar *) "ALL",	/* 270 */
	(uchar *) "CCL",	/* 271 */
	(uchar *) "NCCL",	/* 272 */
	(uchar *) "CHAR",	/* 273 */
	(uchar *) "OR",	/* 274 */
	(uchar *) "STAR",	/* 275 */
	(uchar *) "QUEST",	/* 276 */
	(uchar *) "PLUS",	/* 277 */
	(uchar *) "ADD",	/* 278 */
	(uchar *) "MINUS",	/* 279 */
	(uchar *) "MULT",	/* 280 */
	(uchar *) "DIVIDE",	/* 281 */
	(uchar *) "MOD",	/* 282 */
	(uchar *) "ASSIGN",	/* 283 */
	(uchar *) "ADDEQ",	/* 284 */
	(uchar *) "SUBEQ",	/* 285 */
	(uchar *) "MULTEQ",	/* 286 */
	(uchar *) "DIVEQ",	/* 287 */
	(uchar *) "MODEQ",	/* 288 */
	(uchar *) "POWEQ",	/* 289 */
	(uchar *) "ELSE",	/* 290 */
	(uchar *) "INTEST",	/* 291 */
	(uchar *) "CONDEXPR",	/* 292 */
	(uchar *) "POSTINCR",	/* 293 */
	(uchar *) "PREINCR",	/* 294 */
	(uchar *) "POSTDECR",	/* 295 */
	(uchar *) "PREDECR",	/* 296 */
	(uchar *) "ASGNOP",	/* 297 */
	(uchar *) "BOR",	/* 298 */
	(uchar *) "AND",	/* 299 */
	(uchar *) "GETLINE",	/* 300 */
	(uchar *) "APPEND",	/* 301 */
	(uchar *) "EQ",	/* 302 */
	(uchar *) "GE",	/* 303 */
	(uchar *) "GT",	/* 304 */
	(uchar *) "LE",	/* 305 */
	(uchar *) "LT",	/* 306 */
	(uchar *) "NE",	/* 307 */
	(uchar *) "MATCHOP",	/* 308 */
	(uchar *) "IN",	/* 309 */
	(uchar *) "ARG",	/* 310 */
	(uchar *) "BLTIN",	/* 311 */
	(uchar *) "BREAK",	/* 312 */
	(uchar *) "CALL",	/* 313 */
	(uchar *) "CLOSE",	/* 314 */
	(uchar *) "CONTINUE",	/* 315 */
	(uchar *) "DELETE",	/* 316 */
	(uchar *) "DO",	/* 317 */
	(uchar *) "EXIT",	/* 318 */
	(uchar *) "FOR",	/* 319 */
	(uchar *) "FIELD",	/* 320 */
	(uchar *) "FUNC",	/* 321 */
	(uchar *) "GSUB",	/* 322 */
	(uchar *) "IF",	/* 323 */
	(uchar *) "INDEX",	/* 324 */
	(uchar *) "LSUBSTR",	/* 325 */
	(uchar *) "MATCHFCN",	/* 326 */
	(uchar *) "NEXT",	/* 327 */
	(uchar *) "NUMBER",	/* 328 */
	(uchar *) "PRINT",	/* 329 */
	(uchar *) "PRINTF",	/* 330 */
	(uchar *) "RETURN",	/* 331 */
	(uchar *) "SPLIT",	/* 332 */
	(uchar *) "SPRINTF",	/* 333 */
	(uchar *) "STRING",	/* 334 */
	(uchar *) "SUB",	/* 335 */
	(uchar *) "SUBSTR",	/* 336 */
	(uchar *) "REGEXPR",	/* 337 */
	(uchar *) "VAR",	/* 338 */
	(uchar *) "VARNF",	/* 339 */
	(uchar *) "IVAR",	/* 340 */
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
	arith,	/* ADD */
	arith,	/* MINUS */
	arith,	/* MULT */
	arith,	/* DIVIDE */
	arith,	/* MOD */
	assign,	/* ASSIGN */
	assign,	/* ADDEQ */
	assign,	/* SUBEQ */
	assign,	/* MULTEQ */
	assign,	/* DIVEQ */
	assign,	/* MODEQ */
	assign,	/* POWEQ */
	nullproc,	/* ELSE */
	intest,	/* INTEST */
	condexpr,	/* CONDEXPR */
	incrdecr,	/* POSTINCR */
	incrdecr,	/* PREINCR */
	incrdecr,	/* POSTDECR */
	incrdecr,	/* PREDECR */
	nullproc,	/* ASGNOP */
	boolop,	/* BOR */
	boolop,	/* AND */
	getline,	/* GETLINE */
	nullproc,	/* APPEND */
	relop,	/* EQ */
	relop,	/* GE */
	relop,	/* GT */
	relop,	/* LE */
	relop,	/* LT */
	relop,	/* NE */
	nullproc,	/* MATCHOP */
	instat,	/* IN */
	arg,	/* ARG */
	bltin,	/* BLTIN */
	jump,	/* BREAK */
	call,	/* CALL */
	closefile,	/* CLOSE */
	jump,	/* CONTINUE */
	delete,	/* DELETE */
	dostat,	/* DO */
	jump,	/* EXIT */
	forstat,	/* FOR */
	nullproc,	/* FIELD */
	nullproc,	/* FUNC */
	gsub,	/* GSUB */
	ifstat,	/* IF */
	sindex,	/* INDEX */
	nullproc,	/* LSUBSTR */
	matchop,	/* MATCHFCN */
	jump,	/* NEXT */
	nullproc,	/* NUMBER */
	print,	/* PRINT */
	aprintf,	/* PRINTF */
	jump,	/* RETURN */
	split,	/* SPLIT */
	asprintf,	/* SPRINTF */
	nullproc,	/* STRING */
	sub,	/* SUB */
	substr,	/* SUBSTR */
	nullproc,	/* REGEXPR */
	nullproc,	/* VAR */
	getnf,	/* VARNF */
	nullproc,	/* IVAR */
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
