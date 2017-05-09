/*
 * simulator.cc
 *
 * Implementation of the simulator engine.
 */
 
#include "simulator.h"
#include "wrappers.h"
#include "senseEvent.h"
#include <string.h>

Simulator* Simulator :: instance_;
int Event :: uids_ = 1;

Simulator :: Simulator() {
	configFile_ = NULL;
	nodes_ = NULL;
	nnodes_ = 0;
	pktSize_ = 0;
	strcpy(proto, "");
	eventList_ = NULL;
	instance_ = this;
}

/* Extracts simulation and network configuration data from the input file and initializes
 * the environment.
 */
void Simulator :: init(char *config_file) {
	
	configFile_ = fopen(config_file, "r");
	if(!configFile_) {
		fprintf(stderr, "[ERROR]: Could not open simulation file '%s'\n", config_file);
		exit(1);
	}
	
	char line[512] = "", *temp;
	while(!feof(configFile_)) {
		Fgets(line, 512, configFile_);
		if((*line == '\0') || (*line == '#'))	//Ignore comment and blank lines
			continue;
		if(strcmp(line, "END") == 0)
			break;
		//Read the configuration attribute type and values
		temp = strtok(line, "=");
		if(strcmp(temp, "Protocol") == 0) {	// Protocol Name
			//Get the value
			temp = strtok(NULL, "=");
			strcpy(proto, temp);
		}
		else if(strcmp(temp, "Nodes") == 0) {	// Node count
			//Get the value
			temp = strtok(NULL, "=");
			nnodes_ = atoi(temp);
			nodes_ = new Node[nnodes_];
		}
		else if(strcmp(temp, "Attributes") == 0) {	// Node attributes(Location coordinates, energy, etc...)
			for(int i = 0; i < nnodes_; i++) {	// There must be nnodes_ lines that defines the node attributes from this point on.
				Fgets(line, 512, configFile_);
				//Coordinates...Process still pending
				char coord[64]="";
				temp = strtok(line, ":");
				strcpy(coord, temp);
				
				//Transmission Range value
				temp = strtok(NULL, ":");
				nodes_[i].transmissionRange(atof(temp));
				
				// Energy level
				temp = strtok(NULL, ":");
				nodes_[i].energy(atof(temp));
				
				//Packet size
				temp = strtok(NULL, ":");
				pktSize_ = atoi(temp);
				
				//TODO...Anymore attribute then add here
				
				//Extract coordinates
				nodes_[i].location().x = atoi(strtok(coord, ","));
				nodes_[i].location().y = atoi(strtok(NULL, ","));
				nodes_[i].location().z = atoi(strtok(NULL, ","));
			}
		}
		else if(strcmp(temp, "Events") == 0) {	//Event scheduling
			while(strcmp(Fgets(line, 512, configFile_), "") != 0) {
				if(line[0] == '#')
					continue;
				// Event type
				temp = strtok(line, ":");
				if(strcmp(temp, "SENSE_EVENT") == 0) {
					SenseEvent *e = new SenseEvent();
					e->handler_ = new SenseEventHandler();
					e->atNode_ = atoi(strtok(NULL, ":") + 1);
					e->time_ = atof(strtok(NULL, ":"));
					insert(e);
				}
				else {
					fprintf(stderr, "[WARNING]: Unknown Event Type : '%s'\n", temp);
				}
			}
		}
		else if(strcmp(temp, "Tracefile") == 0) {	// Trace file
			(new Trace(strtok(NULL, "=")));
			
		}
		else {
			// Unknown configuration
			fprintf(stderr, "[WARNING]: Unknown configuration attribute : '%s'\n", temp);
		}
	}
}

// Insert/Enqueue event to the event list
void Simulator :: insert(Event *e) {
	Event *ptr = eventList_;
	if(ptr == NULL) {
		ptr = e; //First event
		ptr->prev_ = NULL;
		eventList_ = ptr;
		tail_ = ptr;
	}
	else {
		while(ptr->next_ != NULL)
			ptr = ptr->next_;
		ptr->next_ = e;
		e->prev_ = ptr;
		ptr = ptr->next_;
		tail_ = ptr;
	}
	ptr->next_ = NULL;
}

// Remove/Dequeue event from the event list
Event* Simulator :: deque() {
	Event *ptr = eventList_;
	if(!ptr)
		return NULL;
	eventList_ = eventList_->next_;
	if(eventList_)
		eventList_->prev_ = NULL;
	return ptr;
}

// Execute an event...use  dequeue()  here in the caller
void Simulator :: dispatch(Event *e) {
	clock_ = e->time_;
	e->handler_->handle(e);
	delete e;
}


//
//
//	Run simulator
//
//
void Simulator :: run() {
	Event *e;
	while((e=deque())) {
		dispatch(e);
	}
	
	/* TESTING NODES
	Packet *p = new Packet();
	p->sourceId_ = 0;
	p->destId_ = 1;
	strcpy(p->payload, "HELLO");
	for(int i=0; i<1024; i++) {
		nodes_[0].enqueuePkt(p);
		nodes_[0].send(&nodes_[1]);
	}
	*/
}
