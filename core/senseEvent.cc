/*
 * senseEvent.h
 * 
 * Event handler for a 'SENSE_EVENT' event.
 */

#include "senseEvent.h"
#include <string.h>
#include <time.h>

#define SOME_SENSOR_DATA	"[EVENT_DATA]: Some event occured!"


void SenseEventHandler :: handle(Event *e) {
	// Execute following internal events like- data fowarding to CH, data-aggregation by CH, etc., to the event list
	//TODO...
	Simulator& sim = Simulator::instance();
	printf("SENSE_EVENT at time %lf\n", sim.clock());
	fflush(stdout);
	
	// Get some data to the node
	Node *n = sim.node(((SenseEvent*)e)->atNode_);
	n->eventData(SOME_SENSOR_DATA);
	
	// Forward sensed data to its CH
	n->send(n->clusterHead());
	
	// Let CH forward data to its next hop
	if(routeToBs(n->clusterHead()) == -1)
		fprintf(stderr, "[INFO]: Node(%d) failed to route data to BS\n", n->id());
	
}

// Route packet from startnode to BS
int SenseEventHandler :: routeToBs(Node *startNode) {
	if(startNode->nodeType() == BS) {
		printf("[REACHED_BS]: Packet reached BS successfully\n\n");
		fflush(stdout);
		return 0;
	}
	if(startNode->forwardData() == -1) {
		return -1;
	}
	return routeToBs(startNode->nextHop());
}
