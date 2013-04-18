# /****************************************************************
# Copyright (C) AT&T and Lucent Technologies 1996
# All Rights Reserved
# 
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby
# granted, provided that the above copyright notice appear in all
# copies and that both that the copyright notice and this
# permission notice and warranty disclaimer appear in supporting
# documentation, and that the names of AT&T or Lucent Technologies
# or any of their entities not be used in advertising or publicity
# pertaining to distribution of the software without specific,
# written prior permission.
# 
# AT&T AND LUCENT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
# SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS. IN NO EVENT SHALL AT&T OR LUCENT OR ANY OF THEIR
# ENTITIES BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
# USE OR PERFORMANCE OF THIS SOFTWARE.
# ****************************************************************/

CFLAGS = -g
CFLAGS = -O
CC = purify cc
CC = gcc -Wall
CC = cc

YACC = bison -y
YACC = yacc
YFLAGS = -d

OFILES = b.o main.o parse.o proctab.o tran.o lib.o run.o

SOURCE = awk.h ytab.c ytab.h lexyy.c proto.h awkgram.y awklex.l b.c main.c maketab.c parse.c lib.c run.c tran.c

a.out:	ytab.o $(OFILES) lexyy.o
	$(CC) $(CFLAGS) ytab.o lexyy.o $(OFILES) $(ALLOC)  -lm

$(OFILES) lexyy.o:	awk.h prevytab.h proto.h

ytab.o:	awk.h proto.h awkgram.y
	$(YACC) $(YFLAGS) awkgram.y
	mv y.tab.c ytab.c
	mv y.tab.h ytab.h
	$(CC) $(CFLAGS) -c ytab.c
	cmp -s ytab.h prevytab.h || (cp ytab.h prevytab.h; echo change maketab)

lexyy.c:	awklex.l
	lex awklex.l
	mv lex.yy.c lexyy.c

proctab.c:	maketab
	./maketab >proctab.c

maketab:	prevytab.h maketab.c
	$(CC) $(CFLAGS) maketab.c -o maketab

bundle:
	@cp lexyy.c lexyyc.bak
	@cp ytab.h ytabh.bak
	@cp ytab.c ytabc.bak
	@bundle README FIXES $(SOURCE) lexyyc.bak ytab[ch].bak makefile awk.1

install:	a.out
	cp a.out /usr/bin/awk
	strip /usr/bin/awk

clean:
	rm -f a.out *.o temp* junk* *ytab.[ch] lexyy.c maketab proctab.c foo* glop*
