#define	DEBUG

#include "awk.h"
#include "ctype.h"
#include "stdio.h"
#include "y.tab.h"

extern Node *op2();
#define MAXLIN 256

#define type(v)	v->nobj
#define left(v)	v->narg[0]
#define right(v)	v->narg[1]
#define parent(v)	v->nnext

#define LEAF	case CCL: case NCCL: case CHAR: case DOT: case FINAL: case ALL:
#define UNARY	case STAR: case PLUS: case QUEST:

/* encoding in tree Nodes:
	leaf (CCL, NCCL, CHAR, DOT, FINAL, ALL): left is index, right contains value or pointer to value
	unary (STAR, PLUS, QUEST): left is child, right is null
	binary (CAT, OR): left and right are children
	parent contains pointer to parent
*/


char	chars[MAXLIN];
int	setvec[MAXLIN];
Node	*point[MAXLIN];

int	rtok;
int	rlxval;
char	*prestr;

int	setcnt;
static	int line;

char	*patbeg;
int	patlen;

fa *makedfa(p, anchor)	/* returns dfa for tree pointed to by p */
Node *p;	/* anchor = 1 for anchored matches, else 0 */
int anchor;
{
	Node *p1;
	fa *f;
	int i;
	fcell *pf;

	p1 = op2(CAT, op2(STAR, op2(ALL, (Node *) 0, (Node *) 0), (Node *) 0), p);
		/* put ALL STAR in front of reg.  exp. */
	p1 = op2(CAT, p1, op2(FINAL, (Node *) 0, (Node *) 0));
		/* put FINAL after reg.  exp. */

	line = 0;
	penter(p1);	/* enter parent pointers and leaf indices */
	if ((f = (fa *) Calloc (1, sizeof(fa) + (line-1)*sizeof(rrow))) == NULL)
		overflo("no room for fa");
	cfoll(f, p1);	/* set up follow sets */
	freetr(p1);
	f->accept = line-1;
/*
printf("retab %o:\n", f->re);
printf("	ltype	lval	lfollow\n");
for (i=0; i<line; i++) {
	printf("%d	%d	%c	%o\n", i, f->re[i].ltype, f->re[i].lval, f->re[i].lfollow);
	pf = f->re[i].lfollow;
	while (pf != 0) {
		printf("				%o: %d	%o\n", pf, pf->info, pf->link);
		pf = pf->link;
	}
}
*/
	f->initstat = makeinit(f, anchor);
	f->reset = 0;
	return f;
}

int makeinit(f, anchor)
fa *f;
int anchor;
{
	register i;
	fcell *pf;

	f->curstat = 2;
	f->out[2] = 0;
	pf = f->posns[2] = f->re[0].lfollow;
	while (pf != 0) {
		if (pf->info == f->accept) {
			f->out[2] = 1;
			break;
		}
		pf = pf->link;
	}
	for (i=0; i<NCHARS; i++)
		f->gototab[2][i] = 0;
	f->curstat = cgoto(f, 2, HAT);
	if (anchor) {
		f->posns[1] = 0;
		f->posns[0] = f->posns[2] = f->posns[2]->link;
		f->out[0] = f->out[2];
		if (f->curstat != 2)
			f->posns[f->curstat] = f->posns[f->curstat]->link;
	}
	return f->curstat;
}

penter(p)	/* set up parent pointers and leaf indices */
Node *p;
{
	switch(type(p)) {
		LEAF
			left(p) = (Node *) line;
			point[line++] = p;
			break;
		UNARY
			penter(left(p));
			parent(left(p)) = p;
			break;
		case CAT:
		case OR:
			penter(left(p));
			penter(right(p));
			parent(left(p)) = p;
			parent(right(p)) = p;
			break;
		default:
			error(FATAL, "unknown type %d in penter\n", type(p));
			break;
	}
}

freetr(p)	/* free parse tree and follow sets */
Node *p;
{
	switch(type(p)) {
		LEAF
			xfree(p);
			break;
		UNARY
			freetr(left(p));
			xfree(p);
			break;
		case CAT:
		case OR:
			freetr(left(p));
			freetr(right(p));
			xfree(p);
			break;
		default:
			error(FATAL, "unknown type %d in freetr", type(p));
			break;
	}
}

char *cclenter(p)
register char *p;
{
	register i, c;
	char *op;

	op = p;
	i = 0;
	while ((c = *p++) != 0) {
		if (c == '-' && i > 0 && chars[i-1] != 0) {
			if (*p != 0) {
				c = chars[i-1];
				while (c < *p) {
					if (i >= MAXLIN)
						overflo("character class too big");
					chars[i++] = ++c;
				}
				p++;
				continue;
			}
		}
		if (i >= MAXLIN)
			overflo("character class too big");
		chars[i++] = c;
	}
	chars[i++] = '\0';
	dprintf("cclenter: in = |%s|, out = |%s|\n", op, chars, NULL);
	xfree(op);
	return(tostring(chars));
}

overflo(s)
	char *s;
{
	error(FATAL, "regular expression too big: %s", s);
}

cfoll(f, v)		/* enter follow set of each leaf of vertex v into lfollow[leaf] */
fa *f;
register Node *v;
{
	register i;
	fcell **prev;
	fcell *p;

	switch(type(v)) {
		LEAF
			f->re[(int) left(v)].ltype = type(v);
			f->re[(int) left(v)].lval = (int) right(v);
			for (i=0; i<line; i++)
				setvec[i] = 0;
			follow(v);
			prev = &(f->re[(int) left(v)].lfollow);
			for (i=0; i<line; i++)
				if (setvec[i] == 1) {
					if ((p = (fcell *) Malloc(sizeof(struct fcell))) == NULL)
						overflo("follow set overflow");
					p->info = i;
					*prev = p;
					prev = &(p->link);
				}
			*prev = (fcell *) 0;
			break;
		UNARY
			cfoll(f,left(v));
			break;
		case CAT:
		case OR:
			cfoll(f,left(v));
			cfoll(f,right(v));
			break;
		default:
			error(FATAL, "unknown type %d in cfoll", type(v));
	}
}

first(p)			/* collects initially active leaves of p into setvec */
register Node *p;		/* returns 0 or 1 depending on whether p matches empty string */
{
	register b;

	switch(type(p)) {
		LEAF
			if (setvec[(int) left(p)] != 1) {
				setvec[(int) left(p)] = 1;
				setcnt++;
			}
			if (type(p) == CCL && (*(char *) right(p)) == '\0')
				return(0);		/* empty CCL */
			else return(1);
		case PLUS:
			if (first(left(p)) == 0) return(0);
			return(1);
		case STAR:
		case QUEST:
			first(left(p));
			return(0);
		case CAT:
			if (first(left(p)) == 0 && first(right(p)) == 0) return(0);
			return(1);
		case OR:
			b = first(right(p));
			if (first(left(p)) == 0 || b == 0) return(0);
			return(1);
	}
	error(FATAL, "unknown type %d in first\n", type(p));
	return(-1);
}

follow(v)
Node *v;		/* collects leaves that can follow v into setvec */
{
	Node *p;

	if (type(v) == FINAL)
		return;
	p = parent(v);
	switch (type(p)) {
		case STAR:
		case PLUS:	first(v);
				follow(p);
				return;

		case OR:
		case QUEST:	follow(p);
				return;

		case CAT:	if (v == left(p)) {	/* v is left child of p */
					if (first(right(p)) == 0) {
						follow(p);
						return;
					}
				}
				else		/* v is right child */
					follow(p);
				return;
	}
}

member(c, s)	/* is c in s? */
register char c, *s;
{
	while (*s)
		if (c == *s++)
			return(1);
	return(0);
}


match(f, p)
register fa *f;
register char *p;
{
	register s,ns;

	s = (f->reset)?makeinit(f,0):f->initstat;
	if (f->out[s])
		return(1);
	do {
		if (ns=f->gototab[s][*p])
			s=ns;
		else
			s=cgoto(f,s,*p);
		if (f->out[s])
			return(1);
	} while(*p++ != 0);
	return(0);
}

pmatch(f, p)
register fa *f;
register char *p;
{
	register s, ns;
	register char *q;
	extern char *patbeg;
	extern int patlen;
	int i;

	s = (f->reset)?makeinit(f,1):f->initstat;
	patlen = -1;
	do {
		q = p;
		do {
/*
fcell *pp;
printf("pmatch: p = %o, *p = %c, q = %o, *q = %c\n", p, *p, q, *q);
printf("state %d: ", s);
pp = f->posns[s];
while (pp != 0) {
	printf(" %d", pp->info);
	pp = pp->link;
}
printf("	out = %d\n", f->out[s]);
*/
			if (f->out[s])		/* final state */
				patlen = q-p;
/*
printf("	g(%d, %c) = ", s, *q);
*/
			if (ns=f->gototab[s][*q])
				s=ns;
			else
				s=cgoto(f,s,*q);
/*
printf("%d\n", s);
*/
			if (s==1)	/* no transition */
				if (patlen >= 0) {
					patbeg = p;
					return(1);
				}
				else
					goto nextin;	/* no match */
		} while (*q++ != 0);
		if (f->out[s])
			patlen	= q-p;
		if (patlen >=0 ) {
			patbeg = p;
			return(1);
		}
	nextin:
		s = 2;
		if (f->reset) {
			s = f->initstat = f->curstat = 2;
			f->posns[2] = f->posns[0];
			f->out[2] = f->out[0];
			for (i=0; i<NCHARS; i++)
				f->gototab[2][i] = 0;
		}
	} while (*p++ != 0);
	return (0);
}

nematch(f, p)
register fa *f;
register char *p;
{
	register s, ns;
	register char *q;
	extern char *patbeg;
	extern int patlen;
	int i;

	s = (f->reset)?makeinit(f,1):f->initstat;
	patlen = -1;
	while (*p) {
		q = p;
		do {
			if (f->out[s])		/* final state */
				patlen = q-p;
			if (ns=f->gototab[s][*q])
				s=ns;
			else
				s=cgoto(f,s,*q);
			if (s==1)	/* no transition */
				if (patlen > 0) {
					patbeg = p;
					return(1);
				}
				else
					goto nnextin;	/* no nonempty match */
		} while (*q++ != 0);
		if (f->out[s])
			patlen	= q-p;
		if (patlen >0 ) {
			patbeg = p;
			return(1);
		}
	nnextin:
		s = 2;
		if (f->reset) {
			s = f->initstat = f->curstat = 2;
			f->posns[2] = f->posns[0];
			f->out[2] = f->out[0];
			for (i=0; i<NCHARS; i++)
				f->gototab[2][i] = 0;
		}
	p++;
	}
	return (0);
}

Node *regexp(), *primary(), *concat(), *alt(), *unary();

Node *reparse(p)
char *p;
{
	/* parses regular expression pointed to by p */
	/* uses relex() to scan regular expression */
	Node *np;

	dprintf("reparse <%s>\n", p);
	prestr = p;		/* prestr points to string to be parsed */
	rtok = relex();
	if (rtok == '\0')
		error(FATAL, "empty regular expression");
	np = regexp();
	if (rtok == '\0') return(np);
	else
		error(FATAL, "syntax error in regular expression");
}
Node *regexp(){
	return (alt(concat(primary())));
}
Node *primary(){
	Node *np;
	switch(rtok){
	case CHAR:
		np = op2(CHAR, (Node *) 0, rlxval);
		rtok = relex();
		return (unary(np));
	case ALL:
		rtok = relex();
		return (unary(op2(ALL, (Node *) 0, (Node *) 0)));
	case DOT:
		rtok = relex();
		return (unary(op2(DOT, (Node *) 0, (Node *) 0)));
	case CCL:
		np = op2(CCL, (Node *) 0, cclenter(rlxval));
		rtok = relex();
		return (unary(np));
	case NCCL:
		np = op2(NCCL, (Node *) 0, cclenter(rlxval));
		rtok = relex();
		return (unary(np));
	case '^':
		rtok = relex();
		return (unary(op2(CHAR, (Node *) 0, HAT)));
	case '$':
		rtok = relex();
		return (unary(op2(CHAR, (Node *) 0, (Node *) 0)));
	case '(':
		rtok = relex();
		if (rtok == ')') {	/* special pleading for () */
			rtok = relex();
			return unary(op2(CCL, (Node *) 0, tostring("")));
		}
		np = regexp();
		if (rtok==')') {
			rtok = relex();
			return (unary(np));
		}
		else
			error(FATAL, "syntax error in regular expression");
	}
}
Node *concat(np)
Node *np;
{
	switch(rtok){
	case CHAR: case DOT: case ALL: case CCL: case NCCL: case '$': case '(':
		return (concat(op2(CAT, np, primary())));
	default:
		return (np);
	}
}
Node *alt(np)
Node *np;
{
	if (rtok == OR) {
		rtok = relex();
		return (alt(op2(OR, np, concat(primary()))));
	}
	return (np);
}
Node *unary(np)
Node *np;
{
	switch(rtok){
	case STAR:
		rtok = relex();
		return (unary(op2(STAR, np, (Node *) 0)));
	case PLUS:
		rtok = relex();
		return (unary(op2(PLUS, np, (Node *) 0)));
	case QUEST:
		rtok = relex();
		return (unary(op2(QUEST, np, (Node *) 0)));
	default:
		return (np);
	}
}

relex()		/* lexical analyzer for reparse */
{
	extern int rlxval;
	register int c;
	char cbuf[150];
	int clen, cflag;
	switch (c = *prestr++) {
		case '|': return OR;
		case '*': return STAR;
		case '+': return PLUS;
		case '?': return QUEST;
		case '.': return DOT;
		case '\0': return '\0';
		case '^':
		case '$':
		case '(':
		case ')':
			return c;
		case '\\':
			if ((c = *prestr++) == 't')
				c = '\t';
			else if (c == 'n')
				c = '\n';
			else if (c == 'f')
				c = '\f';
			else if (c == 'r')
				c = '\r';
			else if (c == 'b')
				c = '\b';
			else if (c == '\\')
				c = '\\';
			else if (isdigit(c)) {
				int n = c - '0';
				if (isdigit(*prestr)) {
					n = 8 * n + *prestr++ - '0';
					if (isdigit(*prestr))
						n = 8 * n + *prestr++ - '0';
				}
				c = n;
			} /* else it's now in c */
			rlxval = c;
			return CHAR;
		default:
			rlxval = c;
			return CHAR;
		case '[': 
			clen = 0;
			if (*prestr == '^') {
				cflag = 1;
				prestr++;
			}
			else
				cflag = 0;
			for (;;) {
				if ((c = *prestr++) == '\\') {
					if ((c = *prestr++) == 't')
						cbuf[clen++] = '\t';
					else if (c == 'n')
						cbuf[clen++] = '\n';
					else if (c == 'f')
						cbuf[clen++] = '\f';
					else if (c == 'r')
						cbuf[clen++] = '\r';
					else if (c == 'b')
						cbuf[clen++] = '\b';
					else if (c == '\\')
						cbuf[clen++] = '\\';
					else if (isdigit(c)) {
						int n = c - '0';
						if (isdigit(*prestr)) {
							n = 8 * n + *prestr++ - '0';
							if (isdigit(*prestr))
								n = 8 * n + *prestr++ - '0';
						}
						cbuf[clen++] = n;
					} else
						cbuf[clen++] = c;
				} else if (c == ']') {
					cbuf[clen] = 0;
					rlxval = (int) tostring(cbuf);
					if (cflag == 0)
						return CCL;
					else
						return NCCL;
				} else if (c == '\n') {
					error(FATAL, "newline in character class");
				} else if (c == '\0') {
					error(FATAL, "non-terminated character class");
				} else
					cbuf[clen++] = c;
			}
	}
}


int cgoto(f, s, c)
fa *f;
int s;
char c;
{
	register int i, j, k;
	register fcell *p, *q;
	fcell *listbeg;
	fcell **prev;
	int curvec[MAXLIN];
	int fline;

	fline = f->accept;
	for (i=0; i<=fline; i++)
		curvec[i] = 0;
	/* compute positions of state s into curvec */
	p = f->posns[s];
	while (p != 0) {
		curvec[p->info] = 1;
		p = p->link;
	}
	for (i=0; i<=fline; i++)
		setvec[i] = 0;
	/* compute positions of gototab[s,c] into setvec */
	for (i=0; i<=fline; i++)
		if (curvec[i])
			if ((k = f->re[i].ltype) != FINAL) {
				if (k == CHAR && c == f->re[i].lval
				 || k == DOT && c != 0 && c != HAT
				 || k == ALL && c != 0
				 || k == CCL && member(c, (char *) f->re[i].lval)
				 || k == NCCL && !member(c, (char *) f->re[i].lval) && c != 0) {
					p = f->re[i].lfollow;
					while (p != 0) {
						setvec[p->info] = 1;
						p = p->link;
					}
				}
			}
	/* determine if setvec is a previous state */
	prev = &listbeg;
	for (i=0; i<=fline; i++) {
		if (setvec[i]) {
			if ((p = (fcell *) Malloc(sizeof(struct fcell))) == NULL)
				overflo("out of space in cgoto");
			p->info = i;
			*prev = p;
			prev = &p->link;
		}
	}
	*prev = (fcell *) 0;
	for (i=1; i<= f->curstat; i++) {
		p = f->posns[i];
		q = listbeg;
		while (p != 0) {
			if ((p->info != q->info) || (q == 0))
				goto different;
			p = p->link;
			q = q->link;
		}
		if (q != 0)
			goto different;
		/* setvec is state i */
		f->gototab[s][c] = i;
/*	printf("g[%d][%c] = %d\n", s, c, i);	*/
	p = listbeg;
	while (p != 0) {
		q = p->link;
		Free(p);
		p = q;
	}
		return i;
	different:;
	}
	/* setvec is notin current set of states */
	if (f->curstat >= NSTATES-1) {
		f->curstat = 2;
		f->reset = 1;
	}
	else
		++(f->curstat);
	for (i=0; i<NCHARS; i++)
		f->gototab[f->curstat][i] = 0;
	f->posns[f->curstat] = listbeg;
	f->gototab[s][c] = f->curstat;
/*	printf("g[%d, %c] = %d\n", s, c, f->curstat);	*/
	if (setvec[fline])
		f->out[f->curstat] = 1;
	else
		f->out[f->curstat] = 0;
	return f->curstat;
}

freefa(f)
struct fa *f;
{
	register fcell *p, *q;
	register int i;

	/* free posns */
	for (i=0; i < NSTATES; i++) {
		p = f->posns[i];
		while (p != 0) {
			q = p->link;
			Free(p);
			p = q;
		}
	}
	/* free re */
	for (i=0; i<line; i++) {
		p = f->re[i].lfollow;
		while (p != 0) {
			q = p->link;
			Free(p);
			p = q;
		}
	}
	Free(f);
}
