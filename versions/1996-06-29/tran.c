/****************************************************************
Copyright (C) AT&T and Lucent Technologies 1996
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the names of AT&T or Lucent Technologies
or any of their entities not be used in advertising or publicity
pertaining to distribution of the software without specific,
written prior permission.

AT&T AND LUCENT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL AT&T OR LUCENT OR ANY OF THEIR
ENTITIES BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
USE OR PERFORMANCE OF THIS SOFTWARE.
****************************************************************/

#define	DEBUG
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "awk.h"
#include "ytab.h"

#define	FULLTAB	2	/* rehash when table gets this x full */
#define	GROWTAB 4	/* grow table by this factor */

Array	*symtab;	/* main symbol table */

char	**FS;		/* initial field sep */
char	**RS;		/* initial record sep */
char	**OFS;		/* output field sep */
char	**ORS;		/* output record sep */
char	**OFMT;		/* output format for numbers */
char	**CONVFMT;	/* format for conversions in getsval */
Awkfloat *NF;		/* number of fields in current record */
Awkfloat *NR;		/* number of current record */
Awkfloat *FNR;		/* number of current record in current file */
char	**FILENAME;	/* current filename argument */
Awkfloat *ARGC;		/* number of arguments from command line */
char	**SUBSEP;	/* subscript separator for a[i,j,k]; default \034 */
Awkfloat *RSTART;	/* start of re matched with ~; origin 1 (!) */
Awkfloat *RLENGTH;	/* length of same */

Cell	*recloc;	/* location of record */
Cell	*nrloc;		/* NR */
Cell	*nfloc;		/* NF */
Cell	*fnrloc;	/* FNR */
Array	*ARGVtab;	/* symbol table containing ARGV[...] */
Array	*ENVtab;	/* symbol table containing ENVIRON[...] */
Cell	*rstartloc;	/* RSTART */
Cell	*rlengthloc;	/* RLENGTH */
Cell	*symtabloc;	/* SYMTAB */

Cell	*nullloc;	/* a guaranteed empty cell */
Node	*nullnode;	/* zero&null, converted into a node for comparisons */

extern Cell *fldtab;

void syminit(void)	/* initialize symbol table with builtin vars */
{
	setsymtab("0", "0", 0.0, NUM|STR|CON|DONTFREE, symtab);
	/* this is used for if(x)... tests: */
	nullloc = setsymtab("$zero&null", "", 0.0, NUM|STR|CON|DONTFREE, symtab);
	nullnode = valtonode(nullloc, CCON);

	/* recloc = setsymtab("$0", record, 0.0, REC|STR|DONTFREE, symtab); */
	/* has been done elsewhere */
	recloc = &fldtab[0];
	FS = &setsymtab("FS", " ", 0.0, STR|DONTFREE, symtab)->sval;
	RS = &setsymtab("RS", "\n", 0.0, STR|DONTFREE, symtab)->sval;
	OFS = &setsymtab("OFS", " ", 0.0, STR|DONTFREE, symtab)->sval;
	ORS = &setsymtab("ORS", "\n", 0.0, STR|DONTFREE, symtab)->sval;
	OFMT = &setsymtab("OFMT", "%.6g", 0.0, STR|DONTFREE, symtab)->sval;
	CONVFMT = &setsymtab("CONVFMT", "%.6g", 0.0, STR|DONTFREE, symtab)->sval;
	FILENAME = &setsymtab("FILENAME", "", 0.0, STR|DONTFREE, symtab)->sval;
	nfloc = setsymtab("NF", "", 0.0, NUM, symtab);
	NF = &nfloc->fval;
	nrloc = setsymtab("NR", "", 0.0, NUM, symtab);
	NR = &nrloc->fval;
	fnrloc = setsymtab("FNR", "", 0.0, NUM, symtab);
	FNR = &fnrloc->fval;
	SUBSEP = &setsymtab("SUBSEP", "\034", 0.0, STR|DONTFREE, symtab)->sval;
	rstartloc = setsymtab("RSTART", "", 0.0, NUM, symtab);
	RSTART = &rstartloc->fval;
	rlengthloc = setsymtab("RLENGTH", "", 0.0, NUM, symtab);
	RLENGTH = &rlengthloc->fval;
	symtabloc = setsymtab("SYMTAB", "", 0.0, ARR, symtab);
	symtabloc->sval = (char *) symtab;
}

void arginit(int ac, char *av[])	/* set up ARGV and ARGC */
{
	Cell *cp;
	int i;
	char temp[5];

	ARGC = &setsymtab("ARGC", "", (Awkfloat) ac, NUM, symtab)->fval;
	cp = setsymtab("ARGV", "", 0.0, ARR, symtab);
	ARGVtab = makesymtab(NSYMTAB);	/* could be (int) ARGC as well */
	cp->sval = (char *) ARGVtab;
	for (i = 0; i < ac; i++) {
		sprintf((char *)temp, "%d", i);
		if (isnumber(*av))
			setsymtab(temp, *av, atof(*av), STR|NUM, ARGVtab);
		else
			setsymtab(temp, *av, 0.0, STR, ARGVtab);
		av++;
	}
}

void envinit(char **envp)	/* set up ENVIRON variable */
{
	Cell *cp;
	char *p;

	cp = setsymtab("ENVIRON", "", 0.0, ARR, symtab);
	ENVtab = makesymtab(NSYMTAB);
	cp->sval = (char *) ENVtab;
	for ( ; *envp; envp++) {
		if ((p = (char *) strchr((char *) *envp, '=')) == NULL)
			continue;
		*p++ = 0;	/* split into two strings at = */
		if (isnumber(p))
			setsymtab(*envp, p, atof(p), STR|NUM, ENVtab);
		else
			setsymtab(*envp, p, 0.0, STR, ENVtab);
		p[-1] = '=';	/* restore in case env is passed down to a shell */
	}
}

Array *makesymtab(int n)	/* make a new symbol table */
{
	Array *ap;
	Cell **tp;

	ap = (Array *) malloc(sizeof(Array));
	tp = (Cell **) calloc(n, sizeof(Cell *));
	if (ap == NULL || tp == NULL)
		ERROR "out of space in makesymtab" FATAL;
	ap->nelem = 0;
	ap->size = n;
	ap->tab = tp;
	return(ap);
}

void freesymtab(Cell *ap)	/* free a symbol table */
{
	Cell *cp, *temp;
	Array *tp;
	int i;

	if (!isarr(ap))
		return;
	tp = (Array *) ap->sval;
	if (tp == NULL)
		return;
	for (i = 0; i < tp->size; i++) {
		for (cp = tp->tab[i]; cp != NULL; cp = temp) {
			xfree(cp->nval);
			if (freeable(cp))
				xfree(cp->sval);
			temp = cp->cnext;	/* avoids freeing then using */
			free((char *) cp); 
		}
		tp->tab[i] = 0;
	}
	free((char *) (tp->tab));
	free((char *) tp);
}

void freeelem(Cell *ap, char *s)	/* free elem s from ap (i.e., ap["s"] */
{
	Array *tp;
	Cell *p, *prev = NULL;
	int h;
	
	tp = (Array *) ap->sval;
	h = hash(s, tp->size);
	for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
		if (strcmp((char *) s, (char *) p->nval) == 0) {
			if (prev == NULL)	/* 1st one */
				tp->tab[h] = p->cnext;
			else			/* middle somewhere */
				prev->cnext = p->cnext;
			if (freeable(p))
				xfree(p->sval);
			free(p->nval);
			free((char *) p);
			tp->nelem--;
			return;
		}
}

Cell *setsymtab(char *n, char *s, Awkfloat f, unsigned t, Array *tp)
{
	int h;
	Cell *p;

	if (n != NULL && (p = lookup(n, tp)) != NULL) {
		dprintf( ("setsymtab found %p: n=%s s=\"%s\" f=%g t=%o\n",
			p, p->nval, p->sval, p->fval, p->tval) );
		return(p);
	}
	p = (Cell *) malloc(sizeof(Cell));
	if (p == NULL)
		ERROR "out of space for symbol table at %s", n FATAL;
	p->nval = tostring(n);
	p->sval = s ? tostring(s) : tostring("");
	p->fval = f;
	p->tval = t;
	p->csub = CUNK;
	p->ctype = OCELL;
	tp->nelem++;
	if (tp->nelem > FULLTAB * tp->size)
		rehash(tp);
	h = hash(n, tp->size);
	p->cnext = tp->tab[h];
	tp->tab[h] = p;
	dprintf( ("setsymtab set %p: n=%s s=\"%s\" f=%g t=%o\n",
		p, p->nval, p->sval, p->fval, p->tval) );
	return(p);
}

int hash(char *s, int n)	/* form hash value for string s */
{
	unsigned hashval;

	for (hashval = 0; *s != '\0'; s++)
		hashval = (*s + 31 * hashval);
	return hashval % n;
}

void rehash(Array *tp)	/* rehash items in small table into big one */
{
	int i, nh, nsz;
	Cell *cp, *op, **np;

	nsz = GROWTAB * tp->size;
	np = (Cell **) calloc(nsz, sizeof(Cell *));
	if (np == NULL)		/* can't do it, but can keep running. */
		return;		/* someone else will run out later. */
	for (i = 0; i < tp->size; i++) {
		for (cp = tp->tab[i]; cp; cp = op) {
			op = cp->cnext;
			nh = hash(cp->nval, nsz);
			cp->cnext = np[nh];
			np[nh] = cp;
		}
	}
	free((char *) (tp->tab));
	tp->tab = np;
	tp->size = nsz;
}

Cell *lookup(char *s, Array *tp)	/* look for s in tp */
{
	Cell *p, *prev = NULL;
	int h;

	h = hash(s, tp->size);
	for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
		if (strcmp((char *) s, (char *) p->nval) == 0)
			return(p);	/* found it */
	return(NULL);			/* not found */
}

Awkfloat setfval(Cell *vp, Awkfloat f)	/* set float val of a Cell */
{
	if ((vp->tval & (NUM | STR)) == 0) 
		funnyvar(vp, "assign to");
	if (vp->tval & FLD) {
		donerec = 0;	/* mark $0 invalid */
		if (vp-fldtab > *NF)
			newfld(vp-fldtab);
		dprintf( ("setting field %d to %g\n", vp-fldtab, f) );
	} else if (vp->tval & REC) {
		donefld = 0;	/* mark $1... invalid */
		donerec = 1;
	}
	vp->tval &= ~STR;	/* mark string invalid */
	vp->tval |= NUM;	/* mark number ok */
	dprintf( ("setfval %p: %s = %g, t=%o\n", vp, vp->nval, f, vp->tval) );
	return vp->fval = f;
}

void funnyvar(Cell *vp, char *rw)
{
	if (vp->tval & ARR)
		ERROR "can't %s %s; it's an array name.", rw, vp->nval FATAL;
	if (vp->tval & FCN)
		ERROR "can't %s %s; it's a function.", rw, vp->nval FATAL;
	ERROR "funny variable %p: n=%s s=\"%s\" f=%g t=%o",
		vp, vp->nval, vp->sval, vp->fval, vp->tval WARNING;
}

char *setsval(Cell *vp, char *s)	/* set string val of a Cell */
{
	char *t;

	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "assign to");
	if (vp->tval & FLD) {
		donerec = 0;	/* mark $0 invalid */
		if (vp-fldtab > *NF)
			newfld(vp-fldtab);
		dprintf( ("setting field %d to %s (%p)\n", vp-fldtab, s, s) );
	} else if (vp->tval & REC) {
		donefld = 0;	/* mark $1... invalid */
		donerec = 1;
	}
	t = tostring(s);	/* in case it's self-assign */
	vp->tval &= ~NUM;
	vp->tval |= STR;
	if (freeable(vp))
		xfree(vp->sval);
	vp->tval &= ~DONTFREE;
	dprintf( ("setsval %p: %s = \"%s (%p)\", t=%o\n", vp, vp->nval, t,t, vp->tval) );
	return(vp->sval = t);
}

Awkfloat getfval(Cell *vp)	/* get float val of a Cell */
{
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
	dprintf( ("getfval %p: %s = %g, t=%o\n", vp, vp->nval, vp->fval, vp->tval) );
	return(vp->fval);
}

char *getsval(Cell *vp)	/* get string val of a Cell */
{
	char s[100];
	double dtemp;

	if ((vp->tval & (NUM | STR)) == 0)
		funnyvar(vp, "read value of");
	if ((vp->tval & FLD) && donefld == 0)
		fldbld();
	else if ((vp->tval & REC) && donerec == 0)
		recbld();
	if ((vp->tval & STR) == 0) {
		if (!(vp->tval&DONTFREE))
			xfree(vp->sval);
		if (modf(vp->fval, &dtemp) == 0)	/* it's integral */
			sprintf((char *)s, "%.20g", vp->fval);
		else
			sprintf((char *)s, (char *)*CONVFMT, vp->fval);
		vp->sval = tostring(s);
		vp->tval &= ~DONTFREE;
		vp->tval |= STR;
	}
	dprintf( ("getsval %p: %s = \"%s (%p)\", t=%o\n", vp, vp->nval, vp->sval, vp->sval, vp->tval) );
	return(vp->sval);
}

char *tostring(char *s)	/* make a copy of string s */
{
	char *p;

	p = (char *) malloc(strlen((char *) s)+1);
	if (p == NULL)
		ERROR "out of space in tostring on %s", s FATAL;
	strcpy((char *) p, (char *) s);
	return(p);
}

char *qstring(char *s, int delim)	/* collect string up to next delim */
{
	int c, n;

	for (caddreset(gs); (c = *s) != delim; s++) {
		if (c == '\n')
			ERROR "newline in string %.10s...", gs->cbuf SYNTAX;
		else if (c != '\\')
			cadd(gs, c);
		else	/* \something */	
			switch (c = *++s) {
			case '\\':	cadd(gs, '\\'); break;
			case 'n':	cadd(gs, '\n'); break;
			case 't':	cadd(gs, '\t'); break;
			case 'b':	cadd(gs, '\b'); break;
			case 'f':	cadd(gs, '\f'); break;
			case 'r':	cadd(gs, '\r'); break;
			default:
				if (!isdigit(c)) {
					cadd(gs, c);
					break;
				}
				n = c - '0';
				if (isdigit(s[1])) {
					n = 8 * n + *++s - '0';
					if (isdigit(s[1]))
						n = 8 * n + *++s - '0';
				}
				cadd(gs, n);
				break;
			}
	}
	cadd(gs, 0);
	return gs->cbuf;
}
