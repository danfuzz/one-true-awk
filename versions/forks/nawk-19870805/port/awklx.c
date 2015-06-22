# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
# define A 2
# define str 4
# define sc 6
# define reg 8
# define comment 10
#include	"awk.h"
#include	"ytab.h"

#undef	input	/* defeat lex */
#undef	unput

extern YYSTYPE	yylval;
extern int	infunc;

int	lineno	= 1;
int	brccount = 0;
int	brackcnt  = 0;
int	parencnt = 0;
#define DEBUG
#ifdef	DEBUG
#	define	RET(x)	{if(dbg)printf("lex %s [%s]\n", tokname(x), yytext); return(x); }
#else
#	define	RET(x)	return(x)
#endif

#define	CADD	cbuf[clen++] = yytext[0]; \
		if (clen >= CBUFLEN-1) { \
			yyerror("string/reg expr %.10s... too long", cbuf); \
			BEGIN A; \
		}

uchar	cbuf[CBUFLEN];
uchar	*s;
int	clen, cflag;
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
switch (yybgin-yysvec-1) {	/* witchcraft */
	case 0:
		BEGIN A;
		break;
	case sc:
		BEGIN A;
		RET('}');
	}
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	{ lineno++; RET(NL); }
break;
case 2:
	{ lineno++; RET(NL); }
break;
case 3:
{ ; }
break;
case 4:
	{ RET(';'); }
break;
case 5:
{ lineno++; }
break;
case 6:
{ RET(XBEGIN); }
break;
case 7:
	{ RET(XEND); }
break;
case 8:
{ if (infunc) yyerror("illegal nested function"); RET(FUNC); }
break;
case 9:
{ if (!infunc) yyerror("return not in function"); RET(RETURN); }
break;
case 10:
	{ RET(AND); }
break;
case 11:
	{ RET(BOR); }
break;
case 12:
	{ RET(NOT); }
break;
case 13:
	{ yylval.i = NE; RET(NE); }
break;
case 14:
	{ yylval.i = MATCH; RET(MATCHOP); }
break;
case 15:
	{ yylval.i = NOTMATCH; RET(MATCHOP); }
break;
case 16:
	{ yylval.i = LT; RET(LT); }
break;
case 17:
	{ yylval.i = LE; RET(LE); }
break;
case 18:
	{ yylval.i = EQ; RET(EQ); }
break;
case 19:
	{ yylval.i = GE; RET(GE); }
break;
case 20:
	{ yylval.i = GT; RET(GT); }
break;
case 21:
	{ yylval.i = APPEND; RET(APPEND); }
break;
case 22:
	{ yylval.i = INCR; RET(INCR); }
break;
case 23:
	{ yylval.i = DECR; RET(DECR); }
break;
case 24:
	{ yylval.i = ADDEQ; RET(ASGNOP); }
break;
case 25:
	{ yylval.i = SUBEQ; RET(ASGNOP); }
break;
case 26:
	{ yylval.i = MULTEQ; RET(ASGNOP); }
break;
case 27:
	{ yylval.i = DIVEQ; RET(ASGNOP); }
break;
case 28:
	{ yylval.i = MODEQ; RET(ASGNOP); }
break;
case 29:
	{ yylval.i = POWEQ; RET(ASGNOP); }
break;
case 30:
{ yylval.i = POWEQ; RET(ASGNOP); }
break;
case 31:
	{ yylval.i = ASSIGN; RET(ASGNOP); }
break;
case 32:
	{ RET(POWER); }
break;
case 33:
	{ RET(POWER); }
break;
case 34:
{ yylval.cp = fieldadr(atoi(yytext+1)); RET(FIELD); }
break;
case 35:
{ unputstr("(NF)"); return(INDIRECT); }
break;
case 36:
{ int c, n;
		  c = input(); unput(c);
		  if (c == '(' || c == '[' || infunc && (n=isarg(yytext+1)) >= 0) {
			unputstr(yytext+1);
			return(INDIRECT);
		  } else {
			yylval.cp = setsymtab(yytext+1,"",0.0,STR|NUM,symtab);
			RET(IVAR);
		  }
		}
break;
case 37:
	{ RET(INDIRECT); }
break;
case 38:
	{ yylval.cp = setsymtab(yytext, "", 0.0, NUM, symtab); RET(VARNF); }
break;
case 39:
{
		  yylval.cp = setsymtab(yytext, tostring(yytext), atof(yytext), CON|NUM, symtab);
		  RET(NUMBER); }
break;
case 40:
{ RET(WHILE); }
break;
case 41:
	{ RET(FOR); }
break;
case 42:
	{ RET(DO); }
break;
case 43:
	{ RET(IF); }
break;
case 44:
	{ RET(ELSE); }
break;
case 45:
	{ RET(NEXT); }
break;
case 46:
	{ RET(EXIT); }
break;
case 47:
{ RET(BREAK); }
break;
case 48:
{ RET(CONTINUE); }
break;
case 49:
{ yylval.i = PRINT; RET(PRINT); }
break;
case 50:
{ yylval.i = PRINTF; RET(PRINTF); }
break;
case 51:
{ yylval.i = SPRINTF; RET(SPRINTF); }
break;
case 52:
{ yylval.i = SPLIT; RET(SPLIT); }
break;
case 53:
{ RET(SUBSTR); }
break;
case 54:
	{ yylval.i = SUB; RET(SUB); }
break;
case 55:
	{ yylval.i = GSUB; RET(GSUB); }
break;
case 56:
{ RET(INDEX); }
break;
case 57:
{ RET(MATCHFCN); }
break;
case 58:
	{ RET(IN); }
break;
case 59:
{ RET(GETLINE); }
break;
case 60:
{ RET(CLOSE); }
break;
case 61:
{ RET(DELETE); }
break;
case 62:
{ yylval.i = FLENGTH; RET(BLTIN); }
break;
case 63:
	{ yylval.i = FLOG; RET(BLTIN); }
break;
case 64:
	{ yylval.i = FINT; RET(BLTIN); }
break;
case 65:
	{ yylval.i = FEXP; RET(BLTIN); }
break;
case 66:
	{ yylval.i = FSQRT; RET(BLTIN); }
break;
case 67:
	{ yylval.i = FSIN; RET(BLTIN); }
break;
case 68:
	{ yylval.i = FCOS; RET(BLTIN); }
break;
case 69:
{ yylval.i = FATAN; RET(BLTIN); }
break;
case 70:
{ yylval.i = FSYSTEM; RET(BLTIN); }
break;
case 71:
	{ yylval.i = FRAND; RET(BLTIN); }
break;
case 72:
{ yylval.i = FSRAND; RET(BLTIN); }
break;
case 73:
{ int n, c;
		  c = input(); unput(c);	/* look for '(' */
		  if (c != '(' && infunc && (n=isarg(yytext)) >= 0) {
			yylval.i = n;
			RET(ARG);
		  } else {
			yylval.cp = setsymtab(yytext,"",0.0,STR|NUM,symtab);
			if (c == '(') {
				RET(CALL);
			} else {
				RET(VAR);
			}
		  }
		}
break;
case 74:
	{ BEGIN str; clen = 0; }
break;
case 75:
	{ if (--brccount < 0) yyerror("extra }"); BEGIN sc; RET(';'); }
break;
case 76:
	{ if (--brackcnt < 0) yyerror("extra ]"); RET(']'); }
break;
case 77:
	{ if (--parencnt < 0) yyerror("extra )"); RET(')'); }
break;
case 78:
	{ if (yytext[0] == '{') brccount++;
		  else if (yytext[0] == '[') brackcnt++;
		  else if (yytext[0] == '(') parencnt++;
		  RET(yylval.i = yytext[0]); /* everything else */ }
break;
case 79:
{ cbuf[clen++] = '\\'; cbuf[clen++] = yytext[1]; }
break;
case 80:
	{ yyerror("newline in regular expression %.10s...", cbuf); lineno++; BEGIN A; }
break;
case 81:
{ BEGIN A;
		  cbuf[clen] = 0;
		  yylval.s = tostring(cbuf);
		  unput('/');
		  RET(REGEXPR); }
break;
case 82:
	{ CADD; }
break;
case 83:
	{ BEGIN A;
		  cbuf[clen] = 0; s = tostring(cbuf);
		  cbuf[clen] = ' '; cbuf[++clen] = 0;
		  yylval.cp = setsymtab(cbuf, s, 0.0, CON|STR, symtab);
		  RET(STRING); }
break;
case 84:
	{ yyerror("newline in string %.10s...", cbuf); lineno++; BEGIN A; }
break;
case 85:
{ cbuf[clen++] = '"'; }
break;
case 86:
{ cbuf[clen++] = '\n'; }
break;
case 87:
{ cbuf[clen++] = '\t'; }
break;
case 88:
{ cbuf[clen++] = '\f'; }
break;
case 89:
{ cbuf[clen++] = '\r'; }
break;
case 90:
{ cbuf[clen++] = '\b'; }
break;
case 91:
{ cbuf[clen++] = '\\'; }
break;
case 92:
{ int n;
		  sscanf(yytext+1, "%o", &n); cbuf[clen++] = n; }
break;
case 93:
{ cbuf[clen++] = yytext[1]; }
break;
case 94:
	{ CADD; }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

startreg()
{
	BEGIN reg;
	clen = 0;
}

/* input() and unput() are transcriptions of the standard lex
   macros for input and output with additions for error message
   printing.  God help us all if someone changes how lex works.
*/

uchar	ebuf[300];
uchar	*ep = ebuf;

input()
{
	register c;
	extern uchar *lexprog;

	if (yysptr > yysbuf)
		c = U(*--yysptr);
	else if (yyin == NULL)
		c = *lexprog++;
	else
		c = getc(yyin);
	if (c == '\n')
		yylineno++;
	else if (c == EOF)
		c = 0;
	if (ep >= ebuf + sizeof ebuf)
		ep = ebuf;
	return *ep++ = c;
}

unput(c)
{
	yytchar = c;
	if (yytchar == '\n')
		yylineno--;
	*yysptr++ = yytchar;
	if (--ep < ebuf)
		ep = ebuf + sizeof(ebuf) - 1;
}


unputstr(s)
	char *s;
{
	int i;

	for (i = strlen(s)-1; i >= 0; i--)
		unput(s[i]);
}
int yyvstop[] = {
0,

78,
0,

3,
78,
0,

1,
0,

12,
78,
0,

74,
78,
0,

2,
78,
0,

37,
78,
0,

78,
0,

78,
0,

77,
78,
0,

78,
0,

78,
0,

78,
0,

78,
0,

78,
0,

39,
78,
0,

4,
78,
0,

16,
78,
0,

31,
78,
0,

20,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

78,
0,

76,
78,
0,

33,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

73,
78,
0,

78,
0,

75,
78,
0,

14,
78,
0,

94,
0,

84,
0,

83,
94,
0,

94,
0,

82,
0,

80,
0,

81,
82,
0,

82,
0,

3,
0,

13,
0,

15,
0,

2,
0,

34,
0,

36,
0,

36,
0,

28,
0,

10,
0,

32,
0,

26,
0,

22,
0,

24,
0,

23,
0,

25,
0,

39,
0,

27,
0,

39,
0,

39,
0,

17,
0,

18,
0,

19,
0,

21,
0,

73,
0,

73,
0,

73,
0,

38,
73,
0,

5,
0,

29,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

42,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

43,
73,
0,

58,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

11,
0,

93,
0,

85,
93,
0,

92,
93,
0,

91,
93,
0,

90,
93,
0,

88,
93,
0,

86,
93,
0,

89,
93,
0,

87,
93,
0,

79,
0,

35,
36,
0,

30,
0,

39,
0,

73,
0,

7,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

68,
73,
0,

73,
0,

73,
0,

73,
0,

65,
73,
0,

41,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

64,
73,
0,

73,
0,

63,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

67,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

54,
73,
0,

73,
0,

73,
0,

92,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

44,
73,
0,

46,
73,
0,

8,
73,
0,

73,
0,

55,
73,
0,

73,
0,

73,
0,

73,
0,

45,
73,
0,

73,
0,

71,
73,
0,

73,
0,

73,
0,

73,
0,

66,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

92,
0,

6,
73,
0,

69,
73,
0,

47,
73,
0,

60,
73,
0,

73,
0,

73,
0,

73,
0,

73,
0,

56,
73,
0,

73,
0,

57,
73,
0,

49,
73,
0,

73,
0,

52,
73,
0,

73,
0,

72,
73,
0,

73,
0,

73,
0,

40,
73,
0,

73,
0,

61,
73,
0,

73,
0,

73,
0,

62,
73,
0,

50,
73,
0,

9,
73,
0,

73,
0,

53,
73,
0,

70,
73,
0,

73,
0,

73,
0,

59,
73,
0,

51,
73,
0,

48,
73,
0,

8,
73,
0,
0};
# define YYTYPE int
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	3,13,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	3,14,	3,15,	
18,69,	0,0,	0,0,	0,0,	
0,0,	5,58,	14,66,	37,94,	
18,69,	18,0,	0,0,	0,0,	
0,0,	5,58,	5,59,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	3,16,	3,17,	
3,18,	3,19,	3,20,	3,21,	
0,0,	14,66,	3,22,	3,23,	
3,24,	21,74,	3,25,	3,26,	
3,27,	3,28,	5,60,	0,0,	
23,75,	0,0,	6,60,	24,77,	
25,79,	69,0,	0,0,	18,69,	
3,29,	3,30,	3,31,	3,32,	
5,58,	0,0,	3,33,	3,34,	
20,73,	10,64,	3,35,	23,76,	
25,80,	24,78,	27,82,	9,62,	
18,69,	30,86,	16,67,	3,36,	
31,87,	5,58,	34,91,	9,62,	
9,63,	32,88,	32,89,	35,92,	
36,93,	39,95,	0,0,	0,0,	
0,0,	3,37,	3,38,	3,39,	
0,0,	0,0,	3,40,	3,41,	
3,42,	3,43,	3,44,	3,45,	
3,46,	49,112,	3,47,	75,136,	
5,61,	3,48,	3,49,	3,50,	
6,61,	3,51,	10,65,	3,52,	
3,53,	50,113,	54,123,	91,139,	
3,54,	9,64,	9,62,	40,96,	
41,97,	3,55,	3,56,	3,57,	
4,16,	4,17,	4,18,	4,19,	
4,20,	4,21,	51,114,	55,124,	
4,22,	4,23,	4,24,	9,62,	
4,25,	4,26,	4,27,	16,68,	
26,81,	26,81,	26,81,	26,81,	
26,81,	26,81,	26,81,	26,81,	
26,81,	26,81,	4,29,	4,30,	
4,31,	4,32,	43,100,	42,98,	
44,102,	4,34,	42,99,	45,104,	
4,35,	46,106,	9,65,	47,108,	
43,101,	45,105,	48,110,	92,140,	
44,103,	4,36,	52,115,	47,109,	
81,85,	96,141,	52,116,	46,107,	
48,111,	97,142,	98,143,	100,146,	
102,147,	99,144,	104,150,	4,37,	
4,38,	4,39,	99,145,	105,151,	
4,40,	4,41,	4,42,	4,43,	
4,44,	4,45,	4,46,	103,148,	
4,47,	106,152,	107,153,	4,48,	
4,49,	4,50,	103,149,	4,51,	
81,85,	4,52,	4,53,	110,156,	
111,157,	112,158,	4,54,	113,159,	
109,154,	114,160,	115,161,	4,55,	
4,56,	4,57,	19,70,	19,70,	
19,70,	19,70,	19,70,	19,70,	
19,70,	19,70,	19,70,	19,70,	
109,155,	116,162,	117,163,	119,166,	
120,167,	121,168,	122,169,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,72,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	123,170,	139,172,	141,173,	
142,174,	19,71,	143,175,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	19,71,	19,71,	19,71,	
19,71,	28,83,	144,176,	28,84,	
28,84,	28,84,	28,84,	28,84,	
28,84,	28,84,	28,84,	28,84,	
28,84,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	118,164,	
28,85,	146,177,	147,178,	148,179,	
151,180,	118,165,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
28,85,	152,181,	153,182,	154,183,	
33,90,	156,184,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
33,90,	33,90,	33,90,	33,90,	
53,117,	158,185,	61,125,	159,186,	
160,187,	65,134,	161,188,	53,118,	
53,119,	53,120,	61,125,	61,0,	
53,121,	65,134,	65,0,	162,189,	
53,122,	70,70,	70,70,	70,70,	
70,70,	70,70,	70,70,	70,70,	
70,70,	70,70,	70,70,	164,190,	
165,191,	166,192,	167,193,	168,194,	
169,195,	170,196,	172,198,	61,126,	
173,199,	71,71,	71,71,	71,71,	
71,71,	71,71,	71,71,	71,71,	
71,71,	71,71,	71,71,	174,200,	
175,201,	61,127,	176,202,	177,203,	
65,134,	72,71,	72,71,	72,71,	
72,71,	72,71,	72,71,	72,71,	
72,71,	72,71,	72,71,	180,204,	
181,205,	183,206,	61,125,	71,71,	
184,207,	65,134,	185,208,	187,209,	
189,210,	190,211,	191,212,	72,135,	
193,213,	194,214,	195,215,	196,216,	
202,217,	203,218,	204,219,	72,71,	
205,220,	207,221,	209,222,	210,223,	
212,224,	214,225,	215,226,	217,227,	
219,228,	61,128,	220,229,	224,230,	
227,231,	228,232,	0,0,	61,129,	
0,0,	0,0,	0,0,	61,130,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	61,131,	
0,0,	0,0,	0,0,	61,132,	
0,0,	61,133,	83,83,	83,83,	
83,83,	83,83,	83,83,	83,83,	
83,83,	83,83,	83,83,	83,83,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	85,137,	
0,0,	85,137,	0,0,	83,85,	
85,138,	85,138,	85,138,	85,138,	
85,138,	85,138,	85,138,	85,138,	
85,138,	85,138,	127,171,	127,171,	
127,171,	127,171,	127,171,	127,171,	
127,171,	127,171,	127,171,	127,171,	
135,71,	135,71,	135,71,	135,71,	
135,71,	135,71,	135,71,	135,71,	
135,71,	135,71,	0,0,	83,85,	
137,138,	137,138,	137,138,	137,138,	
137,138,	137,138,	137,138,	137,138,	
137,138,	137,138,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	135,71,	171,197,	
171,197,	171,197,	171,197,	171,197,	
171,197,	171,197,	171,197,	171,197,	
171,197,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+-1,	0,		0,	
yycrank+-95,	yysvec+3,	0,	
yycrank+-16,	0,		0,	
yycrank+-20,	yysvec+5,	0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+-74,	0,		0,	
yycrank+-22,	yysvec+9,	0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		yyvstop+1,
yycrank+9,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+17,	0,		yyvstop+8,
yycrank+0,	0,		yyvstop+11,
yycrank+-11,	0,		yyvstop+14,
yycrank+174,	0,		yyvstop+17,
yycrank+7,	0,		yyvstop+20,
yycrank+7,	0,		yyvstop+22,
yycrank+0,	0,		yyvstop+24,
yycrank+10,	0,		yyvstop+27,
yycrank+12,	0,		yyvstop+29,
yycrank+11,	0,		yyvstop+31,
yycrank+96,	0,		yyvstop+33,
yycrank+13,	0,		yyvstop+35,
yycrank+251,	0,		yyvstop+37,
yycrank+0,	0,		yyvstop+40,
yycrank+16,	0,		yyvstop+43,
yycrank+19,	0,		yyvstop+46,
yycrank+24,	0,		yyvstop+49,
yycrank+261,	0,		yyvstop+52,
yycrank+13,	yysvec+33,	yyvstop+55,
yycrank+9,	yysvec+33,	yyvstop+58,
yycrank+18,	yysvec+33,	yyvstop+61,
yycrank+9,	0,		yyvstop+64,
yycrank+0,	0,		yyvstop+66,
yycrank+28,	0,		yyvstop+69,
yycrank+7,	yysvec+33,	yyvstop+72,
yycrank+10,	yysvec+33,	yyvstop+75,
yycrank+51,	yysvec+33,	yyvstop+78,
yycrank+57,	yysvec+33,	yyvstop+81,
yycrank+52,	yysvec+33,	yyvstop+84,
yycrank+52,	yysvec+33,	yyvstop+87,
yycrank+64,	yysvec+33,	yyvstop+90,
yycrank+65,	yysvec+33,	yyvstop+93,
yycrank+69,	yysvec+33,	yyvstop+96,
yycrank+8,	yysvec+33,	yyvstop+99,
yycrank+16,	yysvec+33,	yyvstop+102,
yycrank+20,	yysvec+33,	yyvstop+105,
yycrank+77,	yysvec+33,	yyvstop+108,
yycrank+279,	yysvec+33,	yyvstop+111,
yycrank+14,	yysvec+33,	yyvstop+114,
yycrank+11,	0,		yyvstop+117,
yycrank+0,	0,		yyvstop+119,
yycrank+0,	0,		yyvstop+122,
yycrank+0,	0,		yyvstop+125,
yycrank+0,	0,		yyvstop+127,
yycrank+0,	0,		yyvstop+129,
yycrank+-385,	0,		yyvstop+132,
yycrank+0,	0,		yyvstop+134,
yycrank+0,	0,		yyvstop+136,
yycrank+0,	0,		yyvstop+138,
yycrank+-388,	0,		yyvstop+141,
yycrank+0,	yysvec+14,	yyvstop+143,
yycrank+0,	0,		yyvstop+145,
yycrank+0,	0,		yyvstop+147,
yycrank+-47,	yysvec+18,	yyvstop+149,
yycrank+353,	0,		yyvstop+151,
yycrank+373,	yysvec+19,	yyvstop+153,
yycrank+389,	yysvec+19,	yyvstop+155,
yycrank+0,	0,		yyvstop+157,
yycrank+0,	0,		yyvstop+159,
yycrank+46,	0,		yyvstop+161,
yycrank+0,	0,		yyvstop+163,
yycrank+0,	0,		yyvstop+165,
yycrank+0,	0,		yyvstop+167,
yycrank+0,	0,		yyvstop+169,
yycrank+0,	0,		yyvstop+171,
yycrank+107,	yysvec+26,	yyvstop+173,
yycrank+0,	0,		yyvstop+175,
yycrank+454,	0,		yyvstop+177,
yycrank+0,	yysvec+28,	yyvstop+179,
yycrank+476,	0,		0,	
yycrank+0,	0,		yyvstop+181,
yycrank+0,	0,		yyvstop+183,
yycrank+0,	0,		yyvstop+185,
yycrank+0,	0,		yyvstop+187,
yycrank+0,	yysvec+33,	yyvstop+189,
yycrank+48,	yysvec+33,	yyvstop+191,
yycrank+103,	yysvec+33,	yyvstop+193,
yycrank+0,	yysvec+33,	yyvstop+195,
yycrank+0,	0,		yyvstop+198,
yycrank+0,	0,		yyvstop+200,
yycrank+80,	yysvec+33,	yyvstop+202,
yycrank+80,	yysvec+33,	yyvstop+204,
yycrank+71,	yysvec+33,	yyvstop+206,
yycrank+75,	yysvec+33,	yyvstop+208,
yycrank+75,	yysvec+33,	yyvstop+210,
yycrank+0,	yysvec+33,	yyvstop+212,
yycrank+69,	yysvec+33,	yyvstop+215,
yycrank+94,	yysvec+33,	yyvstop+217,
yycrank+72,	yysvec+33,	yyvstop+219,
yycrank+81,	yysvec+33,	yyvstop+221,
yycrank+85,	yysvec+33,	yyvstop+223,
yycrank+85,	yysvec+33,	yyvstop+225,
yycrank+0,	yysvec+33,	yyvstop+227,
yycrank+116,	yysvec+33,	yyvstop+230,
yycrank+101,	yysvec+33,	yyvstop+233,
yycrank+109,	yysvec+33,	yyvstop+235,
yycrank+97,	yysvec+33,	yyvstop+237,
yycrank+95,	yysvec+33,	yyvstop+239,
yycrank+112,	yysvec+33,	yyvstop+241,
yycrank+108,	yysvec+33,	yyvstop+243,
yycrank+117,	yysvec+33,	yyvstop+245,
yycrank+124,	yysvec+33,	yyvstop+247,
yycrank+211,	yysvec+33,	yyvstop+249,
yycrank+121,	yysvec+33,	yyvstop+251,
yycrank+139,	yysvec+33,	yyvstop+253,
yycrank+139,	yysvec+33,	yyvstop+255,
yycrank+123,	yysvec+33,	yyvstop+257,
yycrank+160,	yysvec+33,	yyvstop+259,
yycrank+0,	0,		yyvstop+261,
yycrank+0,	0,		yyvstop+263,
yycrank+0,	0,		yyvstop+265,
yycrank+486,	0,		yyvstop+268,
yycrank+0,	0,		yyvstop+271,
yycrank+0,	0,		yyvstop+274,
yycrank+0,	0,		yyvstop+277,
yycrank+0,	0,		yyvstop+280,
yycrank+0,	0,		yyvstop+283,
yycrank+0,	0,		yyvstop+286,
yycrank+0,	0,		yyvstop+289,
yycrank+496,	yysvec+19,	yyvstop+291,
yycrank+0,	0,		yyvstop+294,
yycrank+508,	0,		0,	
yycrank+0,	yysvec+137,	yyvstop+296,
yycrank+193,	yysvec+33,	yyvstop+298,
yycrank+0,	yysvec+33,	yyvstop+300,
yycrank+157,	yysvec+33,	yyvstop+303,
yycrank+171,	yysvec+33,	yyvstop+305,
yycrank+155,	yysvec+33,	yyvstop+307,
yycrank+182,	yysvec+33,	yyvstop+309,
yycrank+0,	yysvec+33,	yyvstop+311,
yycrank+220,	yysvec+33,	yyvstop+314,
yycrank+221,	yysvec+33,	yyvstop+316,
yycrank+207,	yysvec+33,	yyvstop+318,
yycrank+0,	yysvec+33,	yyvstop+320,
yycrank+0,	yysvec+33,	yyvstop+323,
yycrank+225,	yysvec+33,	yyvstop+326,
yycrank+245,	yysvec+33,	yyvstop+328,
yycrank+256,	yysvec+33,	yyvstop+330,
yycrank+254,	yysvec+33,	yyvstop+332,
yycrank+0,	yysvec+33,	yyvstop+334,
yycrank+254,	yysvec+33,	yyvstop+337,
yycrank+0,	yysvec+33,	yyvstop+339,
yycrank+286,	yysvec+33,	yyvstop+342,
yycrank+271,	yysvec+33,	yyvstop+344,
yycrank+278,	yysvec+33,	yyvstop+346,
yycrank+290,	yysvec+33,	yyvstop+348,
yycrank+282,	yysvec+33,	yyvstop+350,
yycrank+0,	yysvec+33,	yyvstop+352,
yycrank+306,	yysvec+33,	yyvstop+355,
yycrank+307,	yysvec+33,	yyvstop+357,
yycrank+297,	yysvec+33,	yyvstop+359,
yycrank+304,	yysvec+33,	yyvstop+361,
yycrank+300,	yysvec+33,	yyvstop+363,
yycrank+300,	yysvec+33,	yyvstop+366,
yycrank+309,	yysvec+33,	yyvstop+368,
yycrank+527,	0,		yyvstop+370,
yycrank+340,	yysvec+33,	yyvstop+372,
yycrank+370,	yysvec+33,	yyvstop+374,
yycrank+324,	yysvec+33,	yyvstop+376,
yycrank+331,	yysvec+33,	yyvstop+378,
yycrank+329,	yysvec+33,	yyvstop+380,
yycrank+319,	yysvec+33,	yyvstop+382,
yycrank+0,	yysvec+33,	yyvstop+384,
yycrank+0,	yysvec+33,	yyvstop+387,
yycrank+331,	yysvec+33,	yyvstop+390,
yycrank+343,	yysvec+33,	yyvstop+393,
yycrank+0,	yysvec+33,	yyvstop+395,
yycrank+329,	yysvec+33,	yyvstop+398,
yycrank+336,	yysvec+33,	yyvstop+400,
yycrank+350,	yysvec+33,	yyvstop+402,
yycrank+0,	yysvec+33,	yyvstop+404,
yycrank+339,	yysvec+33,	yyvstop+407,
yycrank+0,	yysvec+33,	yyvstop+409,
yycrank+342,	yysvec+33,	yyvstop+412,
yycrank+341,	yysvec+33,	yyvstop+414,
yycrank+348,	yysvec+33,	yyvstop+416,
yycrank+0,	yysvec+33,	yyvstop+418,
yycrank+360,	yysvec+33,	yyvstop+421,
yycrank+345,	yysvec+33,	yyvstop+423,
yycrank+361,	yysvec+33,	yyvstop+425,
yycrank+362,	yysvec+33,	yyvstop+427,
yycrank+0,	0,		yyvstop+429,
yycrank+0,	yysvec+33,	yyvstop+431,
yycrank+0,	yysvec+33,	yyvstop+434,
yycrank+0,	yysvec+33,	yyvstop+437,
yycrank+0,	yysvec+33,	yyvstop+440,
yycrank+354,	yysvec+33,	yyvstop+443,
yycrank+364,	yysvec+33,	yyvstop+445,
yycrank+361,	yysvec+33,	yyvstop+447,
yycrank+358,	yysvec+33,	yyvstop+449,
yycrank+0,	yysvec+33,	yyvstop+451,
yycrank+365,	yysvec+33,	yyvstop+454,
yycrank+0,	yysvec+33,	yyvstop+456,
yycrank+368,	yysvec+33,	yyvstop+459,
yycrank+361,	yysvec+33,	yyvstop+462,
yycrank+0,	yysvec+33,	yyvstop+464,
yycrank+356,	yysvec+33,	yyvstop+467,
yycrank+0,	yysvec+33,	yyvstop+469,
yycrank+359,	yysvec+33,	yyvstop+472,
yycrank+365,	yysvec+33,	yyvstop+474,
yycrank+0,	yysvec+33,	yyvstop+476,
yycrank+358,	yysvec+33,	yyvstop+479,
yycrank+0,	yysvec+33,	yyvstop+481,
yycrank+365,	yysvec+33,	yyvstop+484,
yycrank+377,	yysvec+33,	yyvstop+486,
yycrank+0,	yysvec+33,	yyvstop+488,
yycrank+0,	yysvec+33,	yyvstop+491,
yycrank+0,	yysvec+33,	yyvstop+494,
yycrank+377,	yysvec+33,	yyvstop+497,
yycrank+0,	yysvec+33,	yyvstop+499,
yycrank+0,	yysvec+33,	yyvstop+502,
yycrank+379,	yysvec+33,	yyvstop+505,
yycrank+371,	yysvec+33,	yyvstop+507,
yycrank+0,	yysvec+33,	yyvstop+509,
yycrank+0,	yysvec+33,	yyvstop+512,
yycrank+0,	yysvec+33,	yyvstop+515,
yycrank+0,	yysvec+33,	yyvstop+518,
0,	0,	0};
struct yywork *yytop = yycrank+584;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'A' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.1 86/07/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
