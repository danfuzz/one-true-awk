#! /bin/sh
../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"cat"}'

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"cat"}'|cat

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");close("/dev/stdout");print "2nd"|"cat"}'|cat

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"cat";close("cat")}'|cat

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"cat";close("cat")}'|cat

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"cat";close("cat")}'|cat

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"sort"}'|cat

../a.out 'BEGIN{print "1st";fflush("/dev/stdout");print "2nd"|"sort";close("sort")}'|cat
