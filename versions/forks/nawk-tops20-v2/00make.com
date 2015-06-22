$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") awkg.c
$ copy ytab.h prevytab.h
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") awklx.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") b.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") main.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") parse.c
$ cc maketab.c
$ link /exec=maketab.exe maketab.obj,plt:unixclib.olb/lib,-
	sys$library:vaxcrtl.olb/lib
$ delete proctab.c.*
$ maketab :== $'f$environment("DEFAULT")'maketab.exe
$ define/user sys$output proctab.c
$ maketab
$ define/user sys$output proctab.c-new
$ sed -e "/%NONAME-W-NOMSG, Message number 00000000/d" proctab.c
$ rename proctab.c-new proctab.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") proctab.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") tran.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") lib.c
$ cc /debug /nooptimize /define=("popen=fopen","pclose=fclose") run.c
$ link /exec=nawk.exe  awkg.obj, awklx.obj, b.obj,main.obj,parse.obj,-
	proctab.obj,tran.obj,lib.obj,run.obj , plt:unixclib.olb/lib,-
	sys$library:vaxcrtl.olb/lib
