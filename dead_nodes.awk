#!/usr/bin/awk -f

# Script to record energy drop of a node 'node' with time

BEGIN {
	deadNodes = 0
	last = 0
}
{
	if($1 == "X") {
		deadNodes++
	}
	
	# every second
	if($2-1 > last) {
		printf("%lf %d\n", $2, deadNodes)
		last = $2
	}
}
END {
	printf("%lf %d\n", $2, deadNodes)
}
