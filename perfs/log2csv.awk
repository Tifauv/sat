#!/bin/awk -f

BEGIN {
	#OFS=";"
}
/^c Solution to cnf file data/ { test = $6 }
/^c Took/ { time = $3 }
/^s / {
	print test, time; 
}
