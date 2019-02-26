#!/usr/bin/awk -f

# Script to record energy drop of a node 'node' with time

BEGIN {
	energyDrop = 0
	last = 0
	node = 0
}
{
	if($3 == node) {
		#energyDrop += (totalEnergy - $5)
		printf("%lf %lf\n", $2, $5)
	}
	else if($4 == node) {
		#energyDrop += (totalEnergy - $6)
		printf("%lf %lf\n", $2, $6)
	}
	
	#Every second take average
	#if($2-1 < last) {
	#	if($3 == node) {
	#		energyDrop += (totalEnergy - $5)		
	#		count++
	#	}
	#	else if($4 == node) {
	#		energyDrop += (totalEnergy - $6)
	#		count++
	#	}
	#	
	#	avgEnergyDrop += (avgEnergyDrop + energyDrop)
	#	energyDrop = 0
	#}
}
END {
	#avgEnergyDrop = avgEnergyDrop / count
	#printf("%d %lf\n", node, avgEnergyDrop)
}
