/*
 * scheduler.h
 *
 * Implementation of the scheduler, events and event handlers.
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

class Handler;

class Event {
public:
	Event* next_;		/* event list */
	Event* prev_;
	Handler* handler_;	/* handler to call when event ready */
	double time_;		/* time at which event is ready */
	int uid_;	/* unique ID for an event */
	Event() : time_(0), uid_(0) {}
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
	void handle(Event* event);
};

#define	SCHED_START	0.0	/* start time (secs) */

class Scheduler {
public:
	Scheduler();
	~Scheduler();
	static Scheduler& instance() {
		return (*instance_);		// general access to scheduler
	}
	void schedule(Handler*, Event*, double delay);	// sched later event
	void run();			// execute the simulator
	void cancel(Event*);	// cancel event
	void insert(Event*);	// schedule event
	Event* lookup(int uid);	// look for event
	Event* deque();		// next event (removes from q)
	const Event* head();	// next event (not removed from q)
	double clock() const {			// simulator virtual time
		return (clock_);
	}
	void sync();
	double start() {		// start time
		return SCHED_START;
	}
	void reset();
private:
	void dispatch(Event*);	// execute an event
	void dispatch(Event*, double);	// exec event, set clock_
	double clock_; // Current simulation time
	int halted_;
	static Scheduler* instance_;
	static int uid_; // The UID of the event that it is currently pointing to.
};

#endif
