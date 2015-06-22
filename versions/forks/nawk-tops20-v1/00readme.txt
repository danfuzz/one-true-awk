! <BEEBE.UNIX.NAWK>--README.TXT.3, 24-Jan-87 15:42:39, Edit by BEEBE

                          Changes for TOPS-20

[24-Jan-87]
	* renamed awk.g.* awkg.*
	* renamed awk.lx.* awkly.*
	* renamed *y.tab.h *ytab.h
	* applied "sed -e s/y.tab.h/ytab.h/ F >F.-1" for all files F
	* applied "sed -e s/awk.g/awkg/ F >F.-1" for all files F
	* applied "sed -e s/awk.lx/awklx/ F >F.-1" for all files F
	* copied prevytab.h into ytab.h
	* applied "sed -e s/bracecnt/brccount/g F >F.-1" for all files F
	  to eliminate 6-char collision with bracecheck()
	* renamed proctab.c to procta.c because of 6-char object
	  filename length restriction
	* renamed rstartloc to rstloc to avoid conflict with
	  RSTART due to 6-character limit and case
	  independence of externals in TOPS-20
	* renamed rlengthloc to rlnloc for same reasons
	* changed array to arrayx with compile-time
	  preprocessor switch to avoid conflict with array
	  object in macro assembler
