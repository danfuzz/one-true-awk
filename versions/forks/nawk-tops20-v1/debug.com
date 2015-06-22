set module/all
step
set break run
go
set brea %line 1294 when ((n1 == 5220) && (n2 == 1))
go
set mode screen
set break %line 487 when (a->nobj == 265) do (e a->nobj,a->narg,proc)
go
! 5 times now
go
go
go
go
go
! Now single step

set break %line 1113 do (e/az p,fmt);
show break
