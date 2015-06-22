for i
do
	echo "$i:"
	awk -f $i test.data >foo1 
	../a.out -f $i test.data >foo2 
	if cmp -s foo1 foo2
	then true
	else echo -n "$i:	BAD ..."
	fi
	diff -b foo1 foo2 | sed -e 's/^/	/' -e 10q
	# bprint -c ../a.out
done
