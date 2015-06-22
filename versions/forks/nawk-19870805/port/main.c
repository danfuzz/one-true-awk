#define DEBUG
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "awk.h"
#include "ytab.h"

int	dbg	= 0;
uchar	*cmdname;	/* gets argv[0] for error messages */
extern	FILE *yyin;	/* lex input file */
uchar	*lexprog;	/* points to program argument if it exists */
extern	int errorflag;	/* non-zero if any syntax errors; set by yyerror */
int	compile_time = 1;	/* 0 when machine starts.  for error printing */


main(argc, argv)
	int argc;
	uchar *argv[];
{
	uchar *fs = NULL;
	extern int fpecatch();

	cmdname = argv[0];
	if (argc == 1)
		error(FATAL, "Usage: %s [-f source | 'cmds'] [files]", cmdname);
	yyin = NULL;
	while (argc > 1 && argv[1][0] == '-' && argv[1][1] != '\0') {
		switch (argv[1][1]) {
		case 'f':	/* next argument is program filename */
			argc--;
			argv++;
			if ((yyin = fopen(argv[1], "r")) == NULL)
				error(FATAL, "can't open file %s", argv[1]);
			break;
		case 'F':	/* set field separator */
			if (argv[1][2] != 0) {	/* arg is -Fsomething */
				if (argv[1][2] == 't' && argv[1][3] == 0)	/* wart: t=>\t */
					fs = (uchar *) "\t";
				else
					fs = &argv[1][2];
			} else {	/* it's -F (space) something */
				argc--;
				argv++;
				if (argv[1][0] == 't' && argv[1][1] == 0)
					fs = (uchar *) "\t";
				else
					fs = &argv[1][0];
			}
			break;
		case 'd':
			dbg = 1;
			break;
		}
		argc--;
		argv++;
	}
	if (yyin == NULL) {	/* no -f; first argument is program */
		dprintf("program = |%s|\n", argv[1]);
		lexprog = argv[1];
		argc--;
		argv++;
	}
	syminit();
	while (argc > 1) {	/* do leading "name=val" */
		if (!isclvar(argv[1]))
			break;
		setclvar(argv[1]);
		argc--;
		argv++;

	}
	argv[0] = cmdname;	/* put prog name at front of arglist */
	signal(SIGFPE, fpecatch);
	dprintf("argc=%d, argv[0]=%s\n", argc, argv[0]);
	arginit(argc, argv);
	yyparse();
	dprintf("errorflag=%d\n", errorflag, NULL, NULL);
	if (fs)
		*FS = tostring(qstring(fs, '\0'));
	*FILENAME = argv[1];	/* initial file name */
	if (argc == 1)	/* no filenames; use stdin */
		initgetrec();
	if (errorflag == 0) {
		compile_time = 0;
		run(winner);
	} else
		bracecheck();
	exit(errorflag);
}
