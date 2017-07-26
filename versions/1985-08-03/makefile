# This makefile is wrong -- it doesn't properly
# recompile everything when a new token is added
# to awk.g.y.
# Watch out!

YFLAGS = -d
CFLAGS = -g 

OFILES = awk.g.o awk.lx.o b.o main.o tran.o lib.o run.o parse.o proctab.o

SOURCE = awk.h awk.g.y awk.lx.l b.c main.c \
	maketab.c parse.c lib.c run.c tran.c

a.out:	$(OFILES) $(ALLOC) awk.h
	cc $(CFLAGS) $(OFILES) $(ALLOC)  -lm

$(OFILES):	prevy.tab.h awk.h

y.tab.h:	awk.g.o

prevy.tab.h:	y.tab.h awk.g.o
	-cmp -s y.tab.h prevy.tab.h || (cp y.tab.h prevy.tab.h; echo change maketab)

proctab.c:	maketab
	./maketab >proctab.c

maketab:	prevy.tab.h maketab.c
	cc maketab.c -o maketab

install:	a.out
	cp a.out /usr/bin/awk
	strip /usr/bin/awk

profile:	awk.g.o $(OFILES) mon.o
	cc -p $(LOAD) awk.g.o $(OFILES) mon.o -lm

prcan:	NEW $(SOURCE) makefile
	@prcan $?
	@touch prcan

bundle:
	@bundle NEW $(SOURCE) makefile

backup:	$(SOURCE) makefile NEW
	push panther $? awk
	touch backup

bowell:	$(SOURCE) makefile NEW
	push bowell $? /src/cmd/awk
	touch bowell

lint:
	test -r y.tab.c || yacc -d awk.g.y
	test -r lex.yy.c || lex awk.lx.l
	lint -pvcu y.tab.c lex.yy.c b.c main.c proctab.c parse.c lib.c run.c tran.c

lcomp:
	-rm [b-z]*.o
	lcomp b.c main.c tran.c run.c lib.c parse.c proctab.c *.o -lm

lprint:
	echo 1>&2 "piping into can"
	@lprint -P "pr -l90 -w120" | can -fCW.8 -l90

clean:
	rm -f a.out *.o t.* *temp* *.out *junk* y.tab.* prevy.tab.h maketab proctab.c
