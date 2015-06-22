YFLAGS = -d
CFLAGS = -g 

OFILES = b.o main.o parse.o proctab.o tran.o lib.o run.o 

SOURCE = awk.h awk.g.y awk.lx.l b.c main.c maketab.c parse.c lib.c run.c tran.c

a.out:	awk.g.o awk.lx.o $(OFILES) $(ALLOC) awk.h
	cc $(CFLAGS) awk.g.o awk.lx.o $(OFILES) $(ALLOC)  -lm

$(OFILES) awk.lx.o:	awk.h prevy.tab.h 

awk.g.o:	awk.h

y.tab.h:	awk.g.o

prevy.tab.h:	y.tab.h
	-cmp -s y.tab.h prevy.tab.h || (cp y.tab.h prevy.tab.h; echo change maketab)

proctab.c:	maketab
	./maketab >proctab.c

maketab:	prevy.tab.h maketab.c
	cc maketab.c -o maketab

prcan:	NEW $(SOURCE) makefile
	@prcan $?
	@touch prcan

bundle:
	@bundle NEW $(SOURCE) makefile awk.1 compat.awk

backup:	$(SOURCE) makefile NEW
	push panther $? awk
	touch backup

bowell:	$(SOURCE) makefile NEW
	push bowell $? /usr/src/cmd/awk
	touch bowell

profile:
	make 'CFLAGS=-p'

lcomp:
	make 'CC=lcomp' 'CFLAGS='

lprint:
	echo 1>&2 "piping into can"
	@lprint -P "pr -l90 -w120" | can -fCW.8 -l90

clean:
	rm -f a.out *.o t.* *temp* *.out *junk* y.tab.* prevy.tab.h maketab proctab.c

install:	a.out
	cp a.out /usr/bin/awk
	strip /usr/bin/awk
