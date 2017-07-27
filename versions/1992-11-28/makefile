# Copyright (c) 1989 AT&T
# 	All Rights Reserved
# 
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.
# 
# The copyright notice above does not evidence any
# actual or intended publication of such source code.

YFLAGS = -d
CFLAGS = -g -N -I/usr/include/lcc -A
CC = lcc

OFILES = b.o main.o parse.o proctab.o tran.o lib.o run.o 

SOURCE = awk.h proto.h awk.g.y awk.lx.l b.c main.c maketab.c parse.c lib.c run.c tran.c
CSOURCE = y.tab.c lex.yy.c b.c main.c proctab.c parse.c lib.c run.c tran.c

a.out:	awk.g.o awk.lx.o $(OFILES) $(ALLOC) awk.h
	$(CC) $(CFLAGS) awk.g.o awk.lx.o $(OFILES) $(ALLOC)  -lm

$(OFILES) awk.lx.o:	awk.h prevy.tab.h proto.h

awk.g.o:	awk.h proto.h

prevy.tab.h:	y.tab.h
	-cmp -s y.tab.h prevy.tab.h || (cp y.tab.h prevy.tab.h; echo change maketab)

proctab.c:	maketab
	./maketab >proctab.c

maketab:	prevy.tab.h maketab.c
	$(CC) $(CFLAGS) maketab.c -o maketab

bundle:
	@bundle README FIXES $(SOURCE) makefile awk.1 ansi2knr.c deansify.awk 

bowell:	$(SOURCE) makefile NEW FIXES awktest.a
	push bowell $? /usr/src/cmd/awk
	touch bowell

profile:
	make 'CFLAGS=-p'

install:	a.out
	cp a.out /usr/bin/awk
	strip /usr/bin/awk

clean:
	rm -f a.out *.o t.* *temp* *.out *junk* y.tab.* lex.yy.c prevy.tab.h maketab proctab.c foo* glop*
