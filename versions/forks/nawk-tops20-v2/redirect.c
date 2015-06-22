/*
 * getredirection() is intended to aid in porting C programs
 * to VMS (Vax-11 C) which does not support '>' and '<'
 * I/O redirection.  With suitable modification, it may
 * useful for other portability problems as well.
 *
 * Modified, 24-Jan-86 by Jerry Leichter
 *	When creating a new output file, force the maximum record size to
 *	512; otherwise, it ends up as 0 (though the C I/O system won't write
 *	a record longer than 512 bytes anyway) which will cause problems if
 *	the file is later opened for APPEND - if the maximum record size is
 *	0, C will use the length of the longest record written to the file
 *	for its buffer!
 */

#ifdef	vms
#include	<stdio.h>
#include	<errno.h>

int
getredirection(argc, argv)
int		argc;
char		**argv;
/*
 * Process vms redirection arg's.  Exit if any error is seen.
 * If getredirection() processes an argument, it is erased
 * from the vector.  getredirection() returns a new argc value.
 *
 * Warning: do not try to simplify the code for vms.  The code
 * presupposes that getredirection() is called before any data is
 * read from stdin or written to stdout.
 *
 * Normal usage is as follows:
 *
 *	main(argc, argv)
 *	int		argc;
 *	char		*argv[];
 *	{
 *		argc = getredirection(argc, argv);
 *	}
 */
{
	register char		*ap;	/* Argument pointer	*/
	int			i;	/* argv[] index		*/
	int			j;	/* Output index		*/
	int			file;	/* File_descriptor 	*/

	for (j = i = 1; i < argc; i++) {   /* Do all arguments	*/
	    switch (*(ap = argv[i])) {
	    case '<':			/* <file		*/
		if (freopen(++ap, "r", stdin) == NULL) {
		    perror(ap);		/* Can't find file	*/
		    exit(errno);	/* Is a fatal error	*/
		}
		break;

	    case '>':			/* >file or >>file	*/
		if (*++ap == '>') {	/* >>file		*/
		    /*
		     * If the file exists, and is writable by us,
		     * call freopen to append to the file (using the
		     * file's current attributes).  Otherwise, create
		     * a new file with "vanilla" attributes as if
		     * the argument was given as ">filename".
		     * access(name, 2) is TRUE if we can write on
		     * the specified file.
		     */
		    if (access(++ap, 2) == 0) {
			if (freopen(ap, "a", stdout) != NULL)
			    break;	/* Exit case statement	*/
			perror(ap);	/* Error, can't append	*/
			exit(errno);	/* After access test	*/
		    }			/* If file accessable	*/
		}
		/*
		 * On vms, we want to create the file using "standard"
		 * record attributes.  create(...) creates the file
		 * using the caller's default protection mask and
		 * "variable length, implied carriage return"
		 * attributes. dup2() associates the file with stdout.
		 */
		if ((file = creat(ap, 0, "rat=cr", "rfm=var", "mrs=512"))
			== -1
		 || dup2(file, fileno(stdout)) == -1) {
		    perror(ap);		/* Can't create file	*/
		    exit(errno);	/* is a fatal error	*/
		}			/* If '>' creation	*/
		break;			/* Exit case test	*/

	    default:
		argv[j++] = ap;		/* Not a redirector	*/
		break;			/* Exit case test	*/
	    }
	}				/* For all arguments	*/
	argv[j] = NULL;			/* Terminate argv[]	*/
	return (j);			/* Return new argc	*/
}
#else
getredirection(argc, argv)
int		argc;
char		*argv[];
/*
 * Dummy routine.
 */
{
	return (argv[0], argc);
}
#endif
