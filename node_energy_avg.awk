#!/usr/bin/awk -f

# Script to record energy drop of a node 'node' with time

BEGIN {
	avgEnergy = 0
	count = 0
}
{
	for(i=2; i<=NF; i++) {
		avgEnergy += $i
		count++
	}
	avgEnergy = avgEnergy / count
	print $1, " ", avgEnergy
	avgEnergy = 0
	count = 0
}
END {
	#avgEnergyDrop = avgEnergyDrop / count
	#printf("%d %lf\n", node, avgEnergyDrop)
}
