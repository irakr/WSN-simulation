/*
 * senseEvent.h
 * 
 * Event handler for a 'SENSE_EVENT' event.
 */

#ifndef SENSE_EVENT_H_
#define SENSE_EVENT_H_

#include "simulator.h"

class SenseEvent : public Event {
public:
	SenseEvent() : Event() {}
	int atNode_; // ID of node where the event was sensed
};

class SenseEventHandler : public Handler {
public:
	virtual void handle(Event*);
	int routeToBs(Node*);	// Route packet from node to BS
	
	
};

#endif
