			  New Awk on VAX VMS
			     [01-Oct-88]

Getting nawk running on VAX VMS was somewhat painful.  The port
subdirectory of the Unix masters created by MAKEPORT takes care of
name substitutions, and MAKEPORT enumerates the source changes that
must then be edited in by hand.

This VMS version includes changes to support Unix-style command line 
argument expansion, and Unix-style exit().

Build awk by

	gmake -f makefile.vms

Errors about multiply-defined puts() and delete() can be safely
ignored; private versions are used intentionally.
