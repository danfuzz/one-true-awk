YFLAGS = -d
YACCRM=-rm
TESTDIR = .
FRC =
INS = /etc/install -n /usr/bin
IFLAG = -i
CFLAGS = -O
FFLAG =
LDFLAGS = -s
FILES = awk.lx.o b.o main.o token.o tran.o lib.o run.o parse.o proctab.o 

all:	awk

awk:	$(FILES) awk.g.o
	$(CC) $(LDFLAGS) $(IFLAG) $(FFLAG) awk.g.o $(FILES) -lm -o $(TESTDIR)/awk

$(FILES):   awk.h awk.def $(FRC)
token.c:    awk.h $(FRC)
	    ed - <tokenscript
proctab.c:  awk.h proc.c token.c $(FRC)
	    cc -o proc proc.c token.c
	   -./proc > proctab.c
awk.g.o:    awk.def awk.g.c $(FRC)
	$(CC) $(CFLAGS) -c awk.g.c

awk.g.h awk.g.c: awk.g.y $(FRC)
	$(YACC) $(YFLAGS) awk.g.y
	mv y.tab.h awk.g.h
	mv y.tab.c awk.g.c

awk.h:    awk.g.h $(FRC)
	    -cmp -s awk.g.h awk.h || cp awk.g.h awk.h

install:	all
	    $(INS) $(TESTDIR)/awk

clean:
	    -rm -f *.o temp* core proc proctab.c
		$(YACCRM) -f awk.lx.c awk.g.c awk.g.h

clobber:    clean
	    -rm -f $(TESTDIR)/awk

awk.lx.o: awk.lx.c $(FRC)
	$(CC) $(CFLAGS) -c awk.lx.c

awk.lx.c: awk.lx.l $(FRC)
	$(LEX) $(LFLAGS) awk.lx.l
	mv lex.yy.c awk.lx.c

FRC:
