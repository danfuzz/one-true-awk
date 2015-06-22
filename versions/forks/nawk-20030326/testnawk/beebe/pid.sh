#! /bin/sh
AWK=${AWK-../a.out}
echo $$ > _pid.in
echo $1 >> _pid.in
exec $AWK -f pid.awk _pid.in 2>/dev/null
