/*
 * simulator.h
 *
 * Implementation of the simulator engine.
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
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
	virtual void handle(Event*) {};
};

/* start time (secs) */
#define	SCHED_START	0.0
#define DEFAULT_RELAXTIME 1.0

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
	void dispatch(Event*);	// execute an event
	void cancel(Event*);	// cancel event
	void insert(Event*);	// schedule event
	Event* lookup(int uid);	// look for event
	Event* deque();		// next event (removes from q)
	
	inline double clock() const {			// Event start time
		return (clock_);
	}
	
	inline double pseudoStartTime() {		// start time
		return pseudoStartTime_;
	}
	
	/*
	inline double pseudoCurrentTime() {
		return ((double) ::clock()/CLOCKS_PER_SEC - pseudoStartTime());
	}
	*/
	
	inline double pseudoCurrentTime() {
		struct timespec time;
		double timef, startTime, integral, fractional;
		startTime = pseudoStartTime();
		fractional = modf(startTime, &integral);
		fractional = fractional * 1e+9;
		clock_gettime(CLOCK_MONOTONIC, &time);
		timef = time.tv_sec - integral;
		timef += (time.tv_nsec - fractional) / 1e+9;
		return timef;
	}
	
	/*
	inline void delay(double sec) {
		double end_time = pseudoCurrentTime() + sec;	// Till when the delay should persist
		while(pseudoCurrentTime() < end_time);
	}
	*/
	
	inline void delay(double sec) {
		double fractional, integral;
		struct timespec time1;
		time1.tv_sec = (long)sec;
		fractional = modf(sec, &integral);
		time1.tv_nsec = (long)(fractional * 10e9);
		if(nanosleep(&time1, NULL) < 0) {
			perror("nanosleep");
			exit(1);
		}
	}
	
	void reset();
	
	Node* node(int i) { if(i < nnodes_) return (nodes_[i]); else return NULL; }
	Node** nodePtr(int i) { if(i < nnodes_) return (nodes_+i); else return NULL; }
	Node* baseStation() {return bs_; }
	int nClusters() { return nclusters_; }
	void createTopology(); //Connect all the nodes according to their distances
	
	double energyGapThreshold() { return energyGapThreshold_; }
	
	void killNode(Node*);	// Remove node from the network
	
	int eventCount() { return eventCount_; }
	int totalEvents() { return totalEvents_; }
	int nnodes() { return nnodes_; }
	
	double relaxPeriodTime_;
	
private:
	FILE *configFile_; //Input config file
	int nnodes_, ndead_nodes;
	int nclusters_;	//No of clusters
	char proto[128]; //Protocol name
	Node **nodes_; //Array of node pointer objects ([index] == id_)
	Node *bs_;	//Base station node
	
	// Protocol specific parameters
	double energyGapThreshold_;
	
	// Scheduling properties
	Event *eventList_, *tail_;
	unsigned int eventCount_, totalEvents_;
	
	double clock_; // Event start time
	// This is discrete. For getting the exact continuous time use the 'pseudoCurrentTime_' variable and related.
	
	// This is the time taken as a start point when the program actually reaches the run() method so that
	// the time duration occupied by the initialization processes are neglected.
	// The overall simulation time will offset with reference to this time, which means this time is actually = 0(pseudo)
	double pseudoStartTime_;

	//clock_t systemClock_;	// Process's clock time
	static Simulator* instance_;
	
};



// A function to be start point of a thread
void* threadify(void *);
#endif
