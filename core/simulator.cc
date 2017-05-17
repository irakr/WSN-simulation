/*
 * simulator.cc
 *
 * Implementation of the simulator engine.
 */
 
#include "simulator.h"
#include "wrappers.h"
#include "senseEvent.h"
#include <pthread.h>
#include <string.h>

Simulator* Simulator :: instance_;
int Event :: uids_ = 0;

Simulator :: Simulator() {
	configFile_ = NULL;
	nodes_ = NULL;
	nnodes_ = 0;
	strcpy(proto, "");
	eventList_ = NULL;
	nclusters_ = 0;
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
				
				//Coordinates...Process still pending
				char coord[64]="";
				temp = strtok(NULL, ":");
				strcpy(coord, temp);
				
				//Transmission Range value
				temp = strtok(NULL, ":");
				nodes_[i]->transmissionRange(atof(temp));
				
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
				
				
				//TODO...Anymore attribute then add here
				
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
				else {
					fprintf(stderr, "[WARNING]: Unknown Event Type : '%s'\n", temp);
				}
			}
		}
		else if(strcmp(temp, "Tracefile") == 0) {	// Trace file
			(new Trace(strtok(NULL, "=")));
			
		}
		else if(strcmp(temp, "EnergyGapThreshold") == 0) {	// energyGapThreshold
			energyGapThreshold_ = (atof(strtok(NULL, "=")) / 100.0) * Node::maxEnergy();
			printf("energyGapThreshold = %lf\n", energyGapThreshold_);
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

// Remove node from the network
void Simulator :: killNode(Node *n) {
	printf("Node(%d) removed from the network.\n", n->id());
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
}

// Remove/Dequeue event from the event list. Remove from head, i.e., left ended.
Event* Simulator :: deque() {
	Event *ptr = eventList_;
	if(!ptr)
		return NULL;
	eventList_ = eventList_->next_;
	if(eventList_)
		eventList_->prev_ = NULL;
	return ptr;
}

// Execute an event
void Simulator :: dispatch(Event *e) {
	//clock_ = e->time_;
	// TODO... run a new thread here
	printf("Pseudo system time : %lf\n", (PSEUDO_CURRENT_TIME));
	printf("Simulator time : %lf\n", clock());
	fflush(stdout);
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
	pthread_t tid;
	pseudoStartTime_ = (double)::clock()/CLOCKS_PER_SEC;
	printf("Pseudo start time = %lf\n", pseudoStartTime_);
	while((e=deque())) {
		while((clock_ = (PSEUDO_CURRENT_TIME)) < e->time_);
		pthread_create(&tid, NULL, &threadify, (void*)e);
		pthread_join(tid, (void**)0);
		
		//dispatch(e);
	}
	
}

void* threadify(void *p) {
	Event *e = (Event*)p;
	Simulator::instance().dispatch(e);
	return (void*)0;
}
