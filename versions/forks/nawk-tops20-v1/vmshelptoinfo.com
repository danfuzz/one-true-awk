$ if p1 .eqs. "" then -
	inquire p1 "Input VMS .HLP file"
$ nawk :== $sys$oprroot:[beebe.unix.nawk]nawk.exe
$	dev := 'f$parse(p1,,,"device")'
$	dir := 'f$parse(p1,,,"directory")'
$	nam := 'f$parse(p1,,,"name")'
$	ext := 'f$parse(p1,,,"type")'
$ define/user sys$output 'dev''dir''nam'
$ nawk "-f" vmshelptoinfo.awk 'dev''dir''nam''ext'
