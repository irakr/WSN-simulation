/*
 * cbrEvent.cc
 */

#include "cbrEvent.h"
#include <string.h>
#include <time.h>

#define SOME_SENSOR_DATA	"[CBR]: HELLO WORLD!"

void CBREventHandler :: handle(Event *e) {
	Simulator& sim = Simulator::instance();
	printf("CBR_EVENT started at %lf\n", sim.clock());
	fflush(stdout);
	
	// Get some data to the node
	Node *n = sim.node(((CBREvent*)e)->atNode_);
	double stopTime = ((CBREvent*)e)->stopTime_;
	double currTime = sim.pseudoCurrentTime();
	while(currTime < stopTime) {
		printf("\t\t\t%lf\n", sim.pseudoCurrentTime());
		n->eventData(SOME_SENSOR_DATA);
		
		// Forward sensed data to its CH
		n->send(n->clusterHead());
		
		// Let CH forward data to its next hop
		if(routeToBs(n->clusterHead()) == -1)
			fprintf(stderr, "[INFO]: Node(%d) failed to route data to BS\n", n->id());
		
		currTime = sim.pseudoCurrentTime();
	}
}
