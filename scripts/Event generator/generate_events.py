# generate_events.py
#
########################################################################################################
# This script is used to generate a a long list of events that is very tedious to write by hand		   #
########################################################################################################
#
#
# argv[1] = output filename
# argv[2] = initial simulation time
# argv[3] = uniform time interval between each event
# argv[4] = number of events to be generated


import sys
import random

filename = sys.argv[1]
f = open(filename, 'w')

# 'SENSE_EVENT:N27:1.0'

eventType = 'SENSE_EVENT'
eventCount = int(sys.argv[4])

# Anyone will be choosed at a time randomly
#nodeList = [27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39]
#nodeList = [43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63]
#nodeList = [63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93]
#nodeList = [27]
#nodeList = [43]
nodeList = [63]

node = str()
time = int(sys.argv[2])
timeStep = float(sys.argv[3])
line = str()

for i in range(0, eventCount):
	node = 'N' + str(random.choice(nodeList))
	line = eventType + ':' + node + ':' + str(time) + '\n'
	print line
	f.write(line)
	time = time + timeStep
