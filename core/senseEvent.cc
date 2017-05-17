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
	printf("SENSE_EVENT being handled at %lf\n", (double)::clock()/CLOCKS_PER_SEC);
	fflush(stdout);
	
	// Get some data to the node
	Simulator& simulator = Simulator::instance();
	Node *n = simulator.node(((SenseEvent*)e)->atNode_);
	n->eventData(SOME_SENSOR_DATA);
	
	// Forward sensed data to its CH TODO
	n->send(n->clusterHead());
	
	// Let CH forward data to its next hop
	routeToBs(n->clusterHead());	//TODO... instead use routeToBs()
	
	// Produce trace data TODO
	
	// Call event handler of the receiving node TODO
	
}

// Route packet from startnode to BS
int SenseEventHandler :: routeToBs(Node *startNode) {
	if(startNode->nodeType() == BS) {
		printf("[REACHED_BS]: Packet reached BS successfully\n\n");
		fflush(stdout);
		return 0;
	}
	startNode->forwardData();
	return routeToBs(startNode->nextHop());
}
