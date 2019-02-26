/*
 * simulator.cc
 *
 * Implementation of the simulator engine.
 */
 
#include "simulator.h"
#include "wrappers.h"
#include "senseEvent.h"
#include "cbrEvent.h"
#include <pthread.h>
#include <string.h>


#define BS_QUEUE_SIZE	1048576		// 1MegaBytes

extern pthread_mutex_t simulatorData_mutex;

Simulator* Simulator :: instance_;
int Event :: uids_ = 0;

Simulator :: Simulator() {
	configFile_ = NULL;
	nodes_ = NULL;
	nnodes_ = 0;
	ndead_nodes = 0;
	strcpy(proto, "");
	eventList_ = NULL;
	nclusters_ = 0;
	eventCount_ = 0;
	relaxPeriodTime_ = DEFAULT_RELAXTIME;
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
			nodes_ = new Node*[nnodes_];
			for(int k=0; k<nnodes_; k++)
				nodes_[k] = new Node(nnodes_); //XXX
		}
		else if(strcmp(temp, "Attributes") == 0) {	// Node attributes(Location coordinates, energy, etc...)
			for(int i = 0; i < nnodes_; i++) {	// There must be nnodes_ lines that defines the node attributes from this point on.
				Fgets(line, 512, configFile_);
				
				// ID
				temp = strtok(line, ":");
				//printf("[Node %d]: ", atoi(temp));
				
				//Coordinates...Partial work is done below somewhere
				char coord[64]="";
				temp = strtok(NULL, ":");
				strcpy(coord, temp);
				
				//Transmission Range value
				temp = strtok(NULL, ":");
				nodes_[i]->transmissionRange(atof(temp));
				
				//Bandwidth
				temp = strtok(NULL, ":");
				nodes_[i]->bandwidth(atof(temp));
				
				// Energy level
				temp = strtok(NULL, ":");
				double energy = atof(temp); // Process is completed after identifying nodeType below.
				
				// NodeType = CH,ACH,NCH
				temp = strtok(NULL, ":");
				if(strcmp(temp, "CH") == 0) {
					nodes_[i]->nodeType(CH);
					nclusters_++;
				}
				else if(strcmp(temp, "ACH") == 0)
					nodes_[i]->nodeType(ACH);
				else if(strcmp(temp, "NCH") == 0)
					nodes_[i]->nodeType(NCH);
				else if(strcmp(temp, "BS") == 0) {
					nodes_[i]->nodeType(BS);
					bs_ = nodes_[i];
					bs_->queueLimit(BS_QUEUE_SIZE);
				}
				else {
					fprintf(stderr, "[ERROR]: Unknown node type '%s'\n", temp);
					exit(1);
				}
				
				// (Contd.) Energy level
				/* FIXME...
				if((nodes_[i]->nodeType() != CH) && (energy > Node::maxEnergy())) {
					fprintf(stderr, "[ERROR]: Energy of node(%d) = %lf is more than MaxEnergy = %lf.\n",	\
								nodes_[i]->id(), energy, Node::maxEnergy());
					exit(1);
					
				}
				*/
				nodes_[i]->energy(energy);
				//printf("energy = %lf\n", nodes_[i]->energy());
				
				// Cluster number
				nodes_[i]->cluster(atoi(strtok(NULL, ":")));
				
				/* XXX...To be removed
				// Neighbour nodes
				temp = strtok(NULL, ":");
				char *neighbors = new char[strlen(temp)+1];
				strcpy(neighbors, temp);
				// Rest of the operations are done below...
				*/
				
				
				//TODO...Anymore attribute then add here(or sometimes somewhere above)
				
				//Extract coordinates
				int x = atoi(strtok(coord, ","));
				int y = atoi(strtok(NULL, ","));
				nodes_[i]->location(x, y, 0);
				
				//XXX Uncomment this if you want to input z-axis too. But this has dependencies over other modules and input file.
				//nodes_[i]->location().z = atoi(strtok(NULL, ","));
				
			}//for
			// Create the graph of the given topology
			createTopology();
			// Generate Cluster neighbour Tables
			for(int i=0; i<nnodes_; i++) {
				if(nodes_[i]->nodeType() != NCH) {
					nodes_[i]->generateCLTable();
					nodes_[i]->generateACLTable();
				}
			}
			printf("[INFO]: Cluster neighbour Tables created\n");
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
				else if(strcmp(temp, "CBR_EVENT") == 0) {
					CBREvent *e = new CBREvent();
					e->handler_ = new CBREventHandler();
					e->atNode_ = atoi(strtok(NULL, ":") + 1);
					e->startTime_ = atof(strtok(NULL, ":"));
					e->stopTime_ = atof(strtok(NULL, ":"));
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
		else if(strcmp(temp, "EnergyTracefile") == 0) { // Energy trace file
			char energy_trace_file[128]="";
			strcpy(energy_trace_file, strtok(NULL, "="));
			Trace::instance().energyTraceFile(energy_trace_file);
		}
		else if(strcmp(temp, "EnergyGapThreshold") == 0) {	// energyGapThreshold
			energyGapThreshold_ = (atof(strtok(NULL, "=")) / 100.0) * Node::maxEnergy();
			printf("energyGapThreshold = %lf\n", energyGapThreshold_);
		}
		else if(strcmp(temp, "PacketQueueSize") == 0) {	// Default packet queue size
			unsigned int queueSize = atoi(strtok(NULL, "="));
			for(int i=0; i<nnodes_; i++) {
					nodes_[i]->queueLimit(queueSize);
			}
		}
		else if(strcmp(temp, "EnergyDivisions") == 0) {	// Energy divisions
			int ed = atoi(strtok(NULL, "="));
			for(int i=0; i<nnodes_; i++) {
				nodes_[i]->energyDivisions_ = ed;
				nodes_[i]->partitionEnergy();
			}
		}
		else if(strcmp(temp, "RelaxTime") == 0) { // Relax time
			relaxPeriodTime_ = atof(strtok(NULL, ":"));
		}
		else {
			// Unknown configuration
			fprintf(stderr, "[WARNING]: Unknown configuration attribute : '%s'\n", temp);
		}
	}
}

//Connect all the nodes according to their distances
void Simulator :: createTopology() {
	int i, j;
	for(i=0; i<nnodes_; i++) {
		for(j=0; j<nnodes_; j++) {
			nodes_[i]->addNeighbour(j);
		}
	}
	printf("[INFO]: Network Topology created\n");
}

// Insert/Enqueue event to the event list. Insert to tail, i.e, right ended.
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
	eventCount_++;
	totalEvents_++;
}

// Remove/Dequeue event from the event list. Remove from head, i.e., left ended.
Event* Simulator :: deque() {
	Event *ptr = eventList_;
	if(!ptr)
		return NULL;
	eventList_ = eventList_->next_;
	if(eventList_)
		eventList_->prev_ = NULL;
	eventCount_--;
	return ptr;
}

// Execute an event
void Simulator :: dispatch(Event *e) {
	//clock_ = e->time_;
	// TODO... run a new thread here
	printf("%80lf\n", (pseudoCurrentTime()));
	//printf("Simulator time : %lf\n", clock());
	fflush(stdout);
	e->handler_->handle(e);
	delete e;
}

// Remove node from the network
void Simulator :: killNode(Node *n) {
	if(n->state() == DEAD_MODE) {
		fprintf(stderr, "[WARNING]: Node(%d) is already dead.\n", n->id());
		return;
	}
	double time = pseudoCurrentTime();
	n->state(DEAD_MODE);
	char str[100];
	sprintf(str, "X %lf %d", time, n->id());
	Trace::instance().traceDump(str);
	ndead_nodes++;
	if(ndead_nodes == nnodes_) {
		printf("[INFO]: All nodes are dead at time %lf\n", time);
		exit(0);
	}
}

//
//
//	Run simulator
//
//
void Simulator :: run() {
	Event *e;
	
	int threadCount = eventCount_;
	pthread_t *tid_list = new pthread_t[threadCount+1];
	
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
	
	pseudoStartTime_ = time.tv_sec;
	pseudoStartTime_ += (time.tv_nsec / 1e+9);
	printf("Pseudo start time = %lf\n", pseudoStartTime_);
	
	unsigned int i=0;
	while((e=deque())) {
		while((clock_ = (pseudoCurrentTime())) < e->time_);
		printf("(%d)", i);
		pthread_create(&tid_list[i], NULL, &threadify, (void*)e);	// Run each event parallely.
		i++;
		/* This code is used if you want to run the events sequentially.
		 * dispatch(e);
		 */
	}
	
	for(i=0; i<threadCount; i++)
		pthread_join(tid_list[i], (void**)0);
	
	printf("Events left = %d, Threads executed = %d\n", eventCount_, threadCount);
	fflush(stdout);
	delete[] tid_list;
}

void* threadify(void *p) {
	Event *e = (Event*)p;
	Simulator::instance().dispatch(e);
	//printf("Exiting thread %lu\n", pthread_self());
	pthread_exit((void*)0);
	return (void*)0;
}
