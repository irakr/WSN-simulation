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
nodeList = [27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39]

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
