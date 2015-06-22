AWK		= env LC_COLLATE=C ../a.out
AWK		= ../a.out

CMP		= cmp

OKAY		= okaynawk

RM		= /bin/rm -f

SHELL		= /bin/sh

### Eliminate any GNU grep flags (e.g., options that color matching strings)
TESTENV		= env GREP_OPTIONS=

TESTFILES       = $(TESTFILES_OTHER) $(TESTFILES_P) $(TESTFILES_T1) \
		  $(TESTFILES_T2) $(TESTFILES_TT)

TESTFILES_OTHER	= chem.awk funstack.awk

TESTFILES_P     = p.1 p.2 p.3 p.4 p.5 p.6 p.7 p.8 p.9 p.10 p.11 p.12 p.13 \
		  p.14 p.15 p.16 p.17 p.18 p.19 p.20 p.21 p.22 p.23 p.24 p.25 \
		  p.26 p.27 p.28 p.29 p.30 p.31 p.32 p.33 p.34 p.35 p.36 p.37 \
		  p.38 p.39 p.40 p.41 p.42 p.43 p.44 p.45 p.46 p.47 p.48 p.49 \
		  p.50 p.51 p.52 p.5a

TESTFILES_T1    = t.0 t.0a t.0b t.0c t.1 t.1.x t.1a t.1b t.2 t.2.x t.3 t.3.x \
		  t.3.y t.3a t.3b t.3c t.3d t.4 t.4.x t.4.y t.5.x t.5.y t.6 \
		  t.6.x t.6a t.6b t.6c t.8.x t.8.y t.NF t.a t.addops t.aeiou \
		  t.aeiouy t.arith t.array t.array1 t.array2 t.assert t.avg \
		  t.b.x t.be t.beginexit t.beginnext t.break t.break1 \
		  t.break2 t.break3 t.bug1 t.builtins t.cat t.cat1 t.cat2 \
		  t.cmp t.coerce t.coerce2 t.comment t.comment1 t.concat \
		  t.cond t.contin t.count t.cum t.d.x t.delete t.delete1 \
		  t.delete2 t.delete3 t.do t.do1 t.e t.else t.exit t.exit1 \
		  t.f t.f.x t.f0 t.f1 t.f2 t.f3 t.f4 t.for t.for1 t.for2 \
		  t.for3 t.format4 t.fun t.fun0 t.fun1 t.fun2 t.fun3 t.fun4 \
		  t.fun5 t.getline t.getval t.gsub t.gsub1 t.gsub3 t.gsub4 \
		  t.i.x t.if t.in t.in1 t.in2 t.in3 t.incr t.incr2 t.incr3 \
		  t.index t.intest t.intest2 t.j.x t.longstr t.makef t.match \
		  t.match1 t.max t.mod t.monotone t.nameval t.next t.not \
		  t.null0 t.ofmt t.ofs t.ors t.ors1 t.pat t.pipe t.pp t.pp1 \
		  t.pp2 t.printf t.printf2 t.quote t.randk t.re1 t.re1a t.re2 \
		  t.re3 t.re4 t.re5 t.re7 t.reFS t.rec t.redir t.reg t.roff \
		  t.sep t.seqno t.set0 t.set0a t.set0b t.set1 t.set2 t.set3 \
		  t.split t.split2 t.split3 t.split4 t.split8 t.split9 \
		  t.split9a t.stately t.strcmp t.strcmp1 t.strnum t.sub \
		  t.sub1 t.sub2 t.sub3 t.substr t.substr1 t.time t.vf t.vf1 \
		  t.vf2 t.vf3 t.x

### NB: Test T.beebe is omitted, since it is the gawk testsuite handled
### in ../testgawk
TESTFILES_T2    = T.-f-f T.argv T.builtin T.chem T.close T.clv T.delete \
		  T.errmsg T.expr T.exprconv T.flags T.func T.gawk T.getline \
		  T.latin1 T.leak T.lilly T.main T.misc T.nextfile T.overflow \
		  T.re T.recache T.redir T.rsfs T.split T.sub T.system

TESTFILES_TT    = tt.01 tt.02 tt.02a tt.03 tt.03a tt.04 tt.05 tt.06 tt.07 \
		  tt.08 tt.09 tt.10 tt.10a tt.11 tt.12 tt.13 tt.13a tt.14 \
		  tt.big

all:

check:		clean-test
	@-test -f /bin/echo && ln -sf /bin/echo .
	@-test -f /usr/bin/echo && ln -sf /usr/bin/echo .
	@echo
	@echo Testing awk = '"'${AWK}'"'
	@echo
	@-nerr=0 ; \
	nfile=0 ; \
	ntest=0 ; \
	maxfiles=`echo $(TESTFILES) | wc -w` ; \
	for f in $(TESTFILES) ; \
	do \
		nfile=`expr $$nfile + 1` ; \
		ntest=`expr $$ntest + 2` ; \
		case $$f in \
		chem*) datafile="eth.p lsd.p lsd1.p morphine.p penicil.p res.p" ;; \
		funstack*) datafile=funstack.in ;; \
		p.*)  datafile=test.countries ;; \
		t.*)  datafile=test.data ;; \
		T.*)  datafile= ;; \
		tt.*) datafile=test-tt.data ;; \
		*)    datafile= ;; \
		esac ; \
		test -f $(OKAY)/$$f.err || touch $(OKAY)/$$f.err ; \
		test -f $(OKAY)/$$f.out || touch $(OKAY)/$$f.out ; \
		case $$f in \
		T.*)  $(TESTENV) awk="${AWK}" ./$$f $$datafile > $$f.out 2> $$f.err ;; \
		*)    $(AWK) -f $$f $$datafile > $$f.out 2> $$f.err ;; \
		esac ; \
		status="PASS" ; \
		if $(CMP) -s $(OKAY)/$$f.err $$f.err ; \
		then \
			$(RM) $$f.err ; \
		else \
			status="FAIL" ; \
			nerr=`expr $$nerr + 1` ; \
		fi ; \
		if $(CMP) -s $(OKAY)/$$f.out $$f.out ; \
		then \
			$(RM) $$f.out ; \
		else \
			status="FAIL" ; \
			nerr=`expr $$nerr + 1` ; \
		fi ; \
		test $$status = FAIL && echo ; \
		printf "%s: %3d/%3d: %s\n" $$status $$nfile $$maxfiles $$f ; \
		test $$status = FAIL && echo ; \
	done ; \
	echo ; \
	echo ; \
	test $$nerr -eq 0 && echo SUCCESS: All $$ntest tests passed ; \
	test $$nerr -gt 0 && echo FAILURE: $$nerr failures in $$ntest tests ; \
	echo ; \
	echo

clean:	clean-test
	-$(RM) a.out core \#* *~

clean-test:
	@-$(RM) _* *.err *.out foo* glop glop1 glop2 out2 out3 ./echo

distclean: mostlyclean

maintainer-clean: distclean

mostlyclean:	clean
