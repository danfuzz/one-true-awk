			  New Awk on VAX VMS
			     [15-Dec-87]

Getting nawk running on VAX VMS is somewhat painful.  The port
subdirectory of the Unix masters created by MAKEPORT takes care of
name substitutions, and MAKEPORT enumerates the source changes that
must then be edited in by hand.

Once the indicated edits have been made, start the build by

	define /user sys$output foo.com
	make -f makefile.vms -n

then run foo.com down to the point that maketab has made proctab.c.
Edit proctab.c to remove the stupid exit() output that VMS puts there
in the last line.  Then resume foo.com.  Errors about multiply-defined
puts() and delete() can be safely ignored; private versions are used
intentionally.
