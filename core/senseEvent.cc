/*
 * senseEvent.h
 * 
 * Event handler for a 'SENSE_EVENT' event.
 */

#include "senseEvent.h"

void SenseEventHandler :: handle(Event *e) {
	// Enqueue following internal events like- data fowarding to CH, data-aggregation by CH, etc., to the event list
	//TODO...
	printf("SENSE_EVENT handled at %lf\n", Simulator::instance().clock());
	
	
}
