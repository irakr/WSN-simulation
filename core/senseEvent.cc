/*
 * senseEvent.h
 * 
 * Event handler for a 'SENSE_EVENT' event.
 */

#include "senseEvent.h"
#include <string.h>

#define SOME_SENSOR_DATA	"[EVENT_DATA]: Some event occured!!!"

void SenseEventHandler :: handle(Event *e) {
	// Execute following internal events like- data fowarding to CH, data-aggregation by CH, etc., to the event list
	//TODO...
	printf("SENSE_EVENT being handled at %lf\n", Simulator::instance().clock());
	
	// Get some data to the node
	Simulator& simulator = Simulator::instance();
	Node *n = simulator.node(((SenseEvent*)e)->atNode_);
	n->eventData(SOME_SENSOR_DATA);
	
	// Forward sensed data to its CH TODO
	n->send(n->clusterHead());
	
	
	// Produce trace data TODO
	
	// Call event handler of the receiving node TODO
	
}
