/*
 * simulator.h
 *
 * Implementation of the simulator engine.
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "trace.h"


class Handler;

class Event {
public:
	Event *next_;
	Event *prev_;
	Handler* handler_;	/* handler to call when event ready */
	double time_;		/* time at which event is ready */
	static int uids_;
	int uid_;	/* unique ID for an event in the event list */
	Event() : time_(0) {
		uid_ = uids_++;
	}
};

/*
 * The base class for all event handlers.  When an event's scheduled
 * time arrives, it is passed to handle which must consume it.
 * i.e., if it needs to be freed it, it must be freed by the handler.
 */
class Handler {
 public:
 	Handler() {}
	~Handler () {}
	virtual void handle(Event*) = 0;
};

/* start time (secs) */
#define	SCHED_START	0.0

// This is the overall simulation environment
class Simulator {
public:
	Simulator();
	~Simulator() {
		if(configFile_)
			fclose(configFile_);
		delete[] nodes_;
	}
	
	void init(char*);
	void run();
	
	// Event management functions
	static Simulator& instance() {
		return (*instance_);		// general access to scheduler
	}
	void schedule(Event*, Handler*,  double delay);	// sched later event
	void cancel(Event*);	// cancel event
	void insert(Event*);	// schedule event
	Event* lookup(int uid);	// look for event
	Event* deque();		// next event (removes from q)
	
	inline double clock() const {			// simulator virtual time
		return (clock_);
	}
	
	inline double start() {		// start time
		return SCHED_START;
	}
	void reset();
private:
	FILE *configFile_; //Input config file
	int nnodes_, pktSize_;
	char proto[128]; //Protocol name
	Node *nodes_; //Array of node objects
	
	// Scheduling properties
	Event *eventList_, *tail_;
	void dispatch(Event*);	// execute an event
	void dispatch(Event*, double);	// exec event, set clock_
	double clock_; // Current simulation time
	int halted_;
	static Simulator* instance_;
	static int uid_; // The UID of the event that it is currently pointing to.
	
};

#endif
