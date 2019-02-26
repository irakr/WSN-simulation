#!/usr/bin/awk -f

# Script to record energy drop of a node 'node' with time

BEGIN {
	last = 0
}
{
	# every second
	if($1-1 > last) {
		printf("%d %lf\n", $1, $2)
		last = $1
	}
}
END {
	printf("%d %lf\n", $1, $2)
	#avgEnergyDrop = avgEnergyDrop / count
	#printf("%d %lf\n", node, avgEnergyDrop)
}
