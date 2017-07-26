#define DEBUG
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "awk.h"
#include "y.tab.h"

int	dbg	= 0;
int	svargc;
char	**svargv;
char	*cmdname;	/* gets argv[0] for error messages */
extern	FILE *yyin;	/* lex input file */
char	*lexprog;	/* points to program argument if it exists */
extern	int errorflag;	/* non-zero if any syntax errors; set by yyerror */
int	compile_time = 1;	/* 0 when machine starts.  for error printing */

main(argc, argv)
	int argc;
	char *argv[];
{
	char *progfile = NULL, *progarg = NULL, *fs = NULL, *freezename = NULL;
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
			progfile = argv[1];
			break;
		case 'F':	/* set field separator */
			if (argv[1][2] != 0) {	/* arg is -Fsomething */
				if (argv[1][2] == 't' && argv[1][3] == 0)	/* special case for tab */
					fs = "\t";
				else
					fs = &argv[1][2];
			} else {	/* it's -F (space) something */
				argc--;
				argv++;
				if (argv[1][0] == 't' && argv[1][1] == 0)
					fs = "\t";
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
		progarg = lexprog = argv[1];
		argc--;
		argv++;
	}
	if (argc == 1) {	/* no filenames; use stdin */
		argv[0] = tostring("-");
		argc++;
		argv--;
	}
	argv[0] = cmdname;	/* put prog name at front of arglist */
	svargc = argc;
	svargv = argv;
	dprintf("svargc=%d, svargv[0]=%s\n", svargc, svargv[0]);
	syminit(svargc, svargv);
	if (fs)
		*FS = tostring(fs);
	*FILENAME = svargv[1];	/* initial file name */
	signal(SIGFPE, fpecatch);
	yyparse();
	dprintf("errorflag=%d\n", errorflag, NULL, NULL);
	if (errorflag == 0) {
		compile_time = 0;
		run(winner);
	} else
		bracecheck();
	exit(errorflag);
}
