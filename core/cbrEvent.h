/*
 * cbrEvent.h
 *
 * An event that constantly generates data in a cluster and forward it towards BS.
 */

#ifndef CBR_EVENT_H_
#define CBR_EVENT_H_

#include "senseEvent.h"

class CBREvent : public SenseEvent {
public:
	CBREvent() : SenseEvent() {}
	double startTime_, stopTime_;
};

class CBREventHandler : public SenseEventHandler {
public:
	virtual void handle(Event*);	
};

#endif
