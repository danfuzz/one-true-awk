$ !=====================================================================
$ ! Makefile for files in directory [BEEBE.UNIX.NAWK]
$ !
$ ! Usage:
$ ! 	@00make
$ !	@00make /debug
$ ! [26-Jan-87]
$ !=====================================================================
$ on control_y then -
	goto done
$ if p1 .nes. "" then p1 = "/debug/nooptimize"
$ write sys$output "P1 = ''p1'"
$ ccom * "''p1'"
$ if p1 .nes. "" then p1 = "/debug"
$ clib = "lib:[plot79.tex.dvi]cc.opt/option"
$ if p1 .nes. "" then clib = "sys$library:vaxcrtl.olb/lib"
$ link /nomap/exec=nawk.exe -
	'p1' -
	main.obj, b.obj, lexyy.obj, lib.obj, parse.obj, procta.obj, -
	run.obj, tran.obj, ytab.obj,  -
	lib:[plot79]unixclib.olb/library, -
	'clib'
$ done:

