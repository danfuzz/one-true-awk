#define	DEBUG
#include <stdio.h>
#include <ctype.h>
#include "awk.h"
#include "ytab.h"

#define	FULLTAB	2	/* rehash when table gets this x full */
#define	GROWTAB 4	/* grow table by this factor */

Array	*symtab;	/* main symbol table */

uchar	**FS;		/* initial field sep */
uchar	**RS;		/* initial record sep */
uchar	**OFS;		/* output field sep */
uchar	**ORS;		/* output record sep */
uchar	**OFMT;		/* output format for numbers*/
Awkfloat *NF;		/* number of fields in current record */
Awkfloat *NR;		/* number of current record */
Awkfloat *FNR;		/* number of current record in current file */
uchar	**FILENAME;	/* current filename argument */
Awkfloat *ARGC;		/* number of arguments from command line */
uchar	**SUBSEP;	/* subscript separator for a[i,j,k]; default \034 */
Awkfloat *RSTART;	/* start of re matched with ~; origin 1 (!) */
Awkfloat *RLENGTH;	/* length of same */

Cell	*recloc;	/* location of record */
Cell	*nrloc;		/* NR */
Cell	*nfloc;		/* NF */
Cell	*fnrloc;	/* FNR */
Array	*ARGVtab;	/* symbol table containing ARGV[...] */
Cell	*rstloc;	/* RSTART */
Cell	*rlnloc;	/* RLENGTH */
Cell	*symtblloc;	/* SYMTAB */

Cell	*nullloc;
Node	*nullnode;	/* zero&null, converted into a node for comparisons */

extern Node *valtonode();
extern Cell fldtab[];

syminit()
{
	symtab = makesymtab(NSYMTAB);
	setsymtab("0", "0", 0.0, NUM|STR|CON, symtab);
	/* this is used for if(x)... tests: */
	nullloc = setsymtab("$zero&null", "", 0.0, NUM|STR|CON, symtab);
	nullnode = valtonode(nullloc, CCON);
	/* recloc = setsymtab("$0", record, 0.0, REC|STR|DONTFREE, symtab); */
	recloc = &fldtab[0];
	FS = &setsymtab("FS", " ", 0.0, STR, symtab)->sval;
	RS = &setsymtab("RS", "\n", 0.0, STR, symtab)->sval;
	OFS = &setsymtab("OFS", " ", 0.0, STR, symtab)->sval;
	ORS = &setsymtab("ORS", "\n", 0.0, STR, symtab)->sval;
	OFMT = &setsymtab("OFMT", "%.6g", 0.0, STR, symtab)->sval;
	FILENAME = &setsymtab("FILENAME", "", 0.0, STR, symtab)->sval;
	nfloc = setsymtab("NF", "", 0.0, NUM, symtab);
	NF = &nfloc->fval;
	nrloc = setsymtab("NR", "", 0.0, NUM, symtab);
	NR = &nrloc->fval;
	fnrloc = setsymtab("FNR", "", 0.0, NUM, symtab);
	FNR = &fnrloc->fval;
	SUBSEP = &setsymtab("SUBSEP", "\034", 0.0, STR, symtab)->sval;
	rstloc = setsymtab("RSTART", "", 0.0, NUM, symtab);
	RSTART = &rstloc->fval;
	rlnloc = setsymtab("RLENGTH", "", 0.0, NUM, symtab);
	RLENGTH = &rlnloc->fval;
	symtblloc = setsymtab("SYMTAB", "", 0.0, ARR, symtab);
	symtblloc->sval = (uchar *) symtab;
}

arginit(ac, av)
	int ac;
	uchar *av[];
{
	Cell *cp;
	Array *makesymtab();
	int i;
	uchar temp[5];

	ARGC = &setsymtab("ARGC", "", (Awkfloat) ac, NUM, symtab)->fval;
	cp = setsymtab("ARGV", "", 0.0, ARR, symtab);
	ARGVtab = makesymtab(NSYMTAB);	/* could be (int) ARGC as well */
	cp->sval = (uchar *) ARGVtab;
	for (i = 0; i < ac; i++) {
		sprintf(temp, "%d", i);
		if (isnumber(*av))
			setsymtab(temp, *av, atof(*av), STR|NUM, ARGVtab);
		else
			setsymtab(temp, *av, 0.0, STR, ARGVtab);
		av++;
	}
}

Array *makesymtab(n)
	int n;
{
	Array *ap;
	Cell **tp;

	ap = (Array *) Malloc(sizeof(Array));
	tp = (Cell **) Calloc(n, sizeof(Cell *));
	if (ap == NULL || tp == NULL)
		error(FATAL, "out of space in makesymtab");
	ap->nelem = 0;
	ap->size = n;
	ap->tab = tp;
	return(ap);
}

freesymtab(ap)	/* free symbol table */
	Cell *ap;
{
	Cell *cp;
	Array *tp;
	int i;

	if (!isarr(ap))
		return;
	tp = (Array *) ap->sval;
	if (tp == NULL)
		return;
	for (i = 0; i < tp->size; i++) {
		for (cp = tp->tab[i]; cp != NULL; cp = cp->cnext) {
			xfree(cp->nval);
			xfree(cp->sval);
			Free(cp);
		}
	}
	Free(tp->tab);
	Free(tp);
}

freeelem(ap, s)		/* free elem s from ap (i.e., ap["s"] */
	Cell *ap;
	uchar *s;
{
	Array *tp;
	Cell *p, *prev = NULL;
	int h;
	
	tp = (Array *) ap->sval;
	h = hash(s, tp->size);
	for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
		if (strcmp(s, p->nval) == 0) {
			if (prev == NULL)	/* 1st one */
				tp->tab[h] = p->cnext;
			else			/* middle somewhere */
				prev->cnext = p->cnext;
			if (freeable(p))
				xfree(p->sval);
			Free(p->nval);
			Free(p);
			tp->nelem--;
			return;
		}
}

Cell *setsymtab(n, s, f, t, tp)
	uchar *n, *s;
	Awkfloat f;
	unsigned t;
	Array *tp;
{
	register h;
	register Cell *p;
	Cell *lookup();

	if (n != NULL && (p = lookup(n, tp)) != NULL) {
		dprintf("setsymtab found %o: n=%s", p, p->nval, NULL);
		dprintf(" s=\"%s\" f=%g t=%o\n", p->sval, p->fval, p->tval);
		return(p);
	}
	p = (Cell *) Malloc(sizeof(Cell));
	if (p == NULL)
		error(FATAL, "symbol table overflow at %s", n);
	p->nval = tostring(n);
	p->sval = s ? tostring(s) : tostring("");
	p->fval = f;
	p->tval = t;
	tp->nelem++;
	if (tp->nelem > FULLTAB * tp->size)
		rehash(tp);
	h = hash(n, tp->size);
	p->cnext = tp->tab[h];
	tp->tab[h] = p;
	dprintf("setsymtab set %o: n=%s", p, p->nval, NULL);
	dprintf(" s=\"%s\" f=%g t=%o\n", p->sval, p->fval, p->tval);
	return(p);
}

hash(s, n)	/* form hash value for string s */
	register uchar *s;
	int n;
{
	register unsigned hashval;

	for (hashval = 0; *s != '\0'; s++)
		hashval = (*s + 31 * hashval);
	return hashval % n;
}

rehash(tp)	/* rehash items in small table into big one */
	Array *tp;
{
	int i, nh, nsz;
	Cell *cp, *op, **np;

	nsz = GROWTAB * tp->size;
	np = (Cell **) Calloc(nsz, sizeof(Cell *));
	if (np == NULL)
		error(FATAL, "out of space in rehash");
	for (i = 0; i < tp->size; i++) {
		for (cp = tp->tab[i]; cp; cp = op) {
			op = cp->cnext;
			nh = hash(cp->nval, nsz);
			cp->cnext = np[nh];
			np[nh] = cp;
		}
	}
	free(tp->tab);
	tp->tab = np;
	tp->size = nsz;
}

Cell *lookup(s, tp)	/* look for s in tp */
	register uchar *s;
	Array *tp;
{
	register Cell *p, *prev = NULL;
	int h;

	h = hash(s, tp->size);
	for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
		if (strcmp(s, p->nval) == 0)
			return(p);	/* found it */
	return(NULL);			/* not found */
}

Awkfloat setfval(vp, f)
	register Cell *vp;
	Awkfloat f;
{
	extern Cell fldtab[];

	if ((vp->tval & (NUM | STR)) == 0) 
		funnyvar(vp, "assign to");
	if (vp->tval & FLD) {
		donerec = 0;	/* mark $0 invalid */
		if (vp-fldtab > *NF)
			newfld(vp-fldtab);
		dprintf("setting field %d to %g\n", vp-fldtab, f);
	} else if (vp->tval & REC) {
		donefld = 0;	/* mark $1... invalid */
		donerec = 1;
	}
	vp->tval &= ~STR;	/* mark string invalid */
	vp->tval |= NUM;	/* mark number ok */
	dprintf("setfval %o: %s = %g, t=%o\n", vp, vp->nval, f, vp->tval);
	return vp->fval = f;
}

funnyvar(vp, rw)
	Cell *vp;
	char *rw;
{
	if (vp->tval & ARR)
		error(FATAL, "can't %s %s; it's an array name.", rw, vp->nval);
	if (vp->tval & FCN)
		error(FATAL, "can't %s %s; it's a function.", rw, vp->nval);
	error(FATAL, "funny variable %o: n=%s s=\"%s\" f=%g t=%o",
		vp, vp->nval, vp->sval, vp->fval, vp->tval);
}

uchar *setsval(vp, s)
register Cell *vp;
uchar *s;
{
	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "assign to");
	if (vp->tval & FLD) {
		donerec = 0;	/* mark $0 invalid */
		if (vp-fldtab > *NF)
			newfld(vp-fldtab);
		dprintf("setting field %d to %s\n", vp-fldtab, s);
	} else if (vp->tval & REC) {
		donefld = 0;	/* mark $1... invalid */
		donerec = 1;
	}
	vp->tval &= ~NUM;
	vp->tval |= STR;
	if (!(vp->tval&DONTFREE))
		xfree(vp->sval);
	vp->tval &= ~DONTFREE;
	dprintf("setsval %o: %s = \"%s\", t=%o\n", vp, vp->nval, s, vp->tval);
	return(vp->sval = tostring(s));
}

Awkfloat r_getfval(vp)
register Cell *vp;
{
	/* if (vp->tval & ARR)
		error(FATAL, "illegal reference to array %s", vp->nval);
		return 0.0; */
	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "read value of");
	if ((vp->tval & FLD) && donefld == 0)
		fldbld();
	else if ((vp->tval & REC) && donerec == 0)
		recbld();
	if (!isnum(vp)) {	/* not a number */
		vp->fval = atof(vp->sval);	/* best guess */
		if (isnumber(vp->sval) && !(vp->tval&CON))
			vp->tval |= NUM;	/* make NUM only sparingly */
	}
	dprintf("getfval %o: %s = %g, t=%o\n", vp, vp->nval, vp->fval, vp->tval);
	return(vp->fval);
}

uchar *r_getsval(vp)
register Cell *vp;
{
	uchar s[100];

	/* if (vp->tval & ARR)
		error(FATAL, "illegal reference to array %s", vp->nval);
		return ""; */
	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "read value of");
	if ((vp->tval & FLD) && donefld == 0)
		fldbld();
	else if ((vp->tval & REC) && donerec == 0)
		recbld();
	if ((vp->tval & STR) == 0) {
		if (!(vp->tval&DONTFREE))
			xfree(vp->sval);
		if ((long)vp->fval == vp->fval)
			sprintf(s, "%.20g", vp->fval);
		else
			sprintf(s, *OFMT, vp->fval);
		vp->sval = tostring(s);
		vp->tval &= ~DONTFREE;
		vp->tval |= STR;
	}
	dprintf("getsval %o: %s = \"%s\", t=%o\n", vp, vp->nval, vp->sval, vp->tval);
	return(vp->sval);
}

uchar *tostring(s)
register uchar *s;
{
	register uchar *p;

	p = Malloc(strlen(s)+1);
	if (p == NULL)
		error(FATAL, "out of space in tostring on %s", s);
	strcpy(p, s);
	return(p);
}

uchar *qstring(s, delim)	/* collect string up to delim */
	uchar *s;
	int delim;
{
	uchar *q;
	int c, n;

	for (q = cbuf; (c = *s) != delim; s++) {
		if (q >= cbuf + CBUFLEN - 1)
			yyerror("string %.10s... too long", cbuf);
		else if (c == '\n')
			yyerror("newline in string %.10s...", cbuf);
		else if (c != '\\')
			*q++ = c;
		else	/* \something */	
			switch (c = *++s) {
			case '\\':	*q++ = '\\'; break;
			case 'n':	*q++ = '\n'; break;
			case 't':	*q++ = '\t'; break;
			case 'b':	*q++ = '\b'; break;
			case 'f':	*q++ = '\f'; break;
			case 'r':	*q++ = '\r'; break;
			default:
				if (!isdigit(c)) {
					*q++ = c;
					break;
				}
				n = c - '0';
				if (isdigit(s[1])) {
					n = 8 * n + *++s - '0';
					if (isdigit(s[1]))
						n = 8 * n + *++s - '0';
				}
				*q++ = n;
				break;
			}
	}
	*q = '\0';
	return cbuf;
}

#ifdef MYALLOC

long	stamp;

uchar *Malloc(n)
	int n;
{
	uchar *p;

	p = (uchar *) malloc(n);
	fprintf(stderr, "%6d a %d %d\n", ++stamp, p, n);
	return p;
}

uchar *Calloc(n, sz)
	int n, sz;
{
	uchar *p;

	p = (uchar *) calloc(n, sz);
	fprintf(stderr, "%6d a %d %d (%d %d)\n", ++stamp, p, n*sz, n, sz);
	return p;
}

Free(p)
	uchar *p;
{
	fprintf(stderr, "%6d f %d\n", ++stamp, p);
	free(p);
}

#endif
