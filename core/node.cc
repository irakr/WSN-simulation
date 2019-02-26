/*
 * node.cc
 *
 * Implementation of a network node.
 */

#include "node.h"
#include "simulator.h"
#include "energy.h"
#include <math.h>
#include <pthread.h>


extern pthread_mutex_t pktEnqueue_mutex, pktDequeue_mutex, 			\
		threshold_mutex, recv_mutex, send_mutex, eventData_mutex,	\
		forwardData_mutex, relax_mutex;

int Node :: ids_ = 0;	//ID generated and ID count
const double Node :: maxEnergy_ = 1000;

Node :: Node(int nneighbor) {
	id_ = ids_++;
	nodeType_ = NCH;
	location_.x = location_.y = location_.z = 0;
	nextHop_ = NULL;
	queueLimit_ = DEFAULT_QUEUE_LIMIT;
	//pktSize_ = PKTHDR_SIZE;
	neighbours_ = new Node*[nneighbor];
	//for(int i=0; i<nneighbor; i++)
		//neighbours_[i] = NULL;
	nneighbours_ = nneighbor;
	chTable_ = new CTable(nneighbours_);
	achTable_ = new CTable(nneighbours_);
	thresholdLevel_ = 1; //Initial threshold partition number
	energyDivisions_ = DEFAULT_ENERGY_DIVISION;
	thresholdEnergy_ = NULL;
	partitionEnergy();
	state_ = ACTIVE_MODE;
}
	
// Add a neighbour to the neighbour list(Bi-directional, self-directional)
int Node :: addNeighbour(int id) {
	Simulator& simulator = Simulator::instance();
	Node *n = simulator.node(id);
	if(!n) {
		fprintf(stderr, "[ERROR]: Unidentified Node. id=%d\n", id);
		return -1;
	}
	//printf("[Neighbour adding]: node(%d)<->node(%d) D=%lf", this->id_, n->id_, distance(this, n));
	// Check distance to the other side
	if(distance(this, n) > this->transmissionRange_)
	{
		//fprintf(stderr, "[WARNING]: addNeighbour() failed for node(%d). node(%d) is out of range\n", this->id_, n->id_);
		//putchar('\n');
		return -1;
	}
	
	neighbours_[id] = n;
	
	// Check distance from the other side
	if(distance(this, n) > n->transmissionRange_)
	{
		//fprintf(stderr, "[WARNING]: addNeighbour() failed for node(%d). node(%d) is out of range\n", n->id_, this->id_);
		//putchar('\n');
		return -1;
	}
	n->neighbours_[this->id_] = this;
	//printf(" [DONE]\n");
	return 0;
}

// Distance between two nodes
double Node :: distance(Node *n1, Node *n2) {
	double x1 = (double)(n1->location_.x);
	double y1 = (double)(n1->location_.y);
	double x2 = (double)(n2->location_.x);
	double y2 = (double)(n2->location_.y);
	//return (sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) ));
	return (sqrt( pow((x2-x1), 2) + pow((y2-y1), 2) ));
}

int Node :: enqueuePkt(Packet *p) {
	pthread_mutex_lock(&pktEnqueue_mutex);
	
	if(pktQueue_.size() == queueLimit_) {
		fprintf(stderr, "[ERROR]: Packet queue full. Dropping packet.\n");
		pthread_mutex_unlock(&pktEnqueue_mutex);
		return -1;
	}
	pktQueue_.push_back(*p);
	
	pthread_mutex_unlock(&pktEnqueue_mutex);
	
	// Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('+', s.pseudoCurrentTime(), p);
	
	return 0;
}

int Node :: dequeuePkt() {
	pthread_mutex_lock(&pktDequeue_mutex);
	
	if(pktQueue_.size() == 0) {
		fprintf(stderr, "[ERROR]: Packet queue is empty. Cannot deque.\n");
		pthread_mutex_unlock(&pktDequeue_mutex);
		return -1;
	}
	Packet *p = new Packet(pktQueue_.front());
	pktQueue_.erase(pktQueue_.begin());
	
	pthread_mutex_unlock(&pktDequeue_mutex);
	
	
	delete p;	//Temporary one deleted
	return 0;
}

//Deque packet and send to node 'n'
int Node :: send(Node *n) {
	//pthread_mutex_lock(&send_mutex);
	
	if(pktQueue_.empty()) {
		fprintf(stderr, "[ERROR]: Packet queue is empty. Cannot send.\n");
		//pthread_mutex_unlock(&send_mutex);
		return -1;
	}
	if(!n) {
		fprintf(stderr, "[ERROR]: Recieving node is not defined. Cannot send.\n");
		//pthread_mutex_unlock(&send_mutex);
		return -1;
	}
	Packet *p = new Packet(pktQueue_.front()); //Backup before dequeueing it
	dequeuePkt();	
	p->destId_ = n->id_;
	p->forwarderId_ = id_;
	
	// Tracing for dequeuing
	Simulator& s = Simulator::instance();
	Trace::instance().format('-', s.pseudoCurrentTime(), p);
	
	// TODO... Add the bandwidth and delay logic here
	double pktTransmissionTime = (p->size() * 8) / (bandwidth_ * 1000);
	double pktPropagationDelay = (distance(this, s.node(p->destId_)) / MAC::propagationSpeed_) * (p->size() * 8);
	double totalLatency = pktTransmissionTime + pktPropagationDelay;
	
	//printf("Latency = %lf, PTT = %lf, PPD = %lf\n", totalLatency, pktTransmissionTime, pktPropagationDelay);
	//fflush(stdout);
	
	s.delay(totalLatency);
	
	//@@@ Alternative solution provided by the system is nanosleep()
	
	n->recv(p); // The receiving node forward this packet atleast even if it has reached its threshold energy.
	
	Energy::spend(this, p, TX);	//energy consumption by transmitter
	
	//pthread_mutex_unlock(&send_mutex);
	
	delete p;	//Temporary one deleted
	return 0;
}

// Send the packet p to the node n without queueing
int Node :: send(Node *n, Packet *p) {
	//pthread_mutex_lock(&send_mutex);
	
	p->forwarderId_ = id_;
	
	// TODO... Add the bandwidth and delay logic here
	Simulator& s = Simulator::instance();
	double pktTransmissionTime = (p->size() * 8) / (bandwidth_ * 1000);
	double pktPropagationDelay = (distance(this, s.node(p->destId_)) / MAC::propagationSpeed_) * (p->size() * 8);
	double totalLatency = pktTransmissionTime + pktPropagationDelay;
	
	s.delay(totalLatency);
	
	n->recv(p);
	Energy::spend(this, p, TX); //energy consumption by transmitter
	
	/*XXX Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('s', s.pseudoCurrentTime(), p);
	*/
	//pthread_mutex_unlock(&send_mutex);
	
	return 0;
}

// Send the packet p to the node n without queueing and no bandwidth delay. (Used for broadcasting only)
int Node :: sendHighPriority(Node *n, Packet *p) {
	//pthread_mutex_lock(&send_mutex);
	
	p->forwarderId_ = id_;
	
	n->recvHighPriority(p);
	
	//Energy::spend(this, p, TX); //energy consumption by transmitter
	
	/* Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('s', s.pseudoCurrentTime(), p);
	*/
	//pthread_mutex_unlock(&send_mutex);
	
	return 0;
}

// Broadcast packet p (Currently only mend for broadcasting relax packet)
int Node :: broadcast(Packet *p) {
	
	// TODO... Add the constant bandwidth and delay logic here
	Simulator& s = Simulator::instance();
	double pktTransmissionTime = (p->size() * 8) / (bandwidth_ * 1000);
	double pktPropagationDelay = (transmissionRange_ / MAC::propagationSpeed_) * (p->size() * 8); //XXX
	double totalLatency = pktTransmissionTime + pktPropagationDelay;
	
	s.delay(totalLatency);
	
	for(int i=0; i<nneighbours_; i++)
		if(neighbours_[i] && (neighbours_[i] != this))
			sendHighPriority(neighbours_[i], p);
	
	Energy::spend(this, p, TX); //energy consumption by transmitter
	
	return 0;
}

// Recieve and enqueue packet
int Node :: recv(Packet *p) {
	//pthread_mutex_lock(&recv_mutex);
	
	Energy::spend(this, p, RX); //energy consumption by receiver
	
	// Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('r', s.pseudoCurrentTime(), p);
	
	if(enqueuePkt(p) == -1) {	//Queue full
		// Drop packet
		
		// Tracing
		Simulator& s = Simulator::instance();
		Trace::instance().format('d', s.pseudoCurrentTime(), p);
	
		//XXX...delete p;	'double free() error'
		
		//pthread_mutex_unlock(&recv_mutex);
		return -1;
	}
	
	//Packet& p1 = pktQueue_.back();
	printf("[PACKET_RECIEVED]: s=%d d=%d f=%d payload='%s'\n", p->sourceId_, p->destId_, p->forwarderId_, p->payload_); 
	
	//pthread_mutex_unlock(&recv_mutex);
	
	return 0;
}

// Recieve and without enqueuing the packet, execute the command send by the packet
int Node :: recvHighPriority(Packet *p) {
	//pthread_mutex_lock(&recv_mutex);
	
	Energy::spend(this, p, RX); //energy consumption by receiver
	
	// Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('r', s.pseudoCurrentTime(), p);
	
	printf("[NOTIFICATION]: s=%d d=%d f=%d payload='%s'\n", p->sourceId_, p->destId_, p->forwarderId_, p->payload_); 
	
	//pthread_mutex_unlock(&recv_mutex);
	
	return 0;
}

// This calls selectNextHop() and forwards packet to it. Not called by 'NCH' nodes.
int Node :: forwardData() {
	//@pthread_mutex_lock(&forwardData_mutex);
	
	// TODO... A node will always select a next hop freshly and forward data. This might add a
	// delay packet delivery time but at the same time it provides to the quality of a path.
	
	if(selectNextHop() == -1) {
		fprintf(stderr, "[INFO]: Next hop could not be decided for node(%d)\n", id_);
		//@pthread_mutex_unlock(&forwardData_mutex);
		return -1; // No route
	}
	send(nextHop_); //Packet may or may not drop. It is not dealt here.
	//@pthread_mutex_unlock(&forwardData_mutex);
	return 0;
}

// Generate application data by a Non-CH sensor node
void Node :: eventData(const char* data) {
	// Store data and enqueue into packet queue
	char str[256]="";
	
	pthread_mutex_lock(&eventData_mutex);
	
	strcpy(eventData_, data);
	
	pthread_mutex_unlock(&eventData_mutex);
	
	sprintf(str, "%s [Node=%d, Cluster=%d]\n", data, id_, clusterId_);
	Packet *p = new Packet(this->id_, clusterHead()->id_, str, "sensor");
	p->forwarderId_ = this->id_;
	
	// Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('e', s.pseudoCurrentTime(), p);
	
	if(enqueuePkt(p) == -1) {
		//XXX... Possibly wait for queue to be available
		fprintf(stderr, "[ERROR]: Packet queue is full. Application data will be dropped or waiting.\n");
	}
	
	Energy::spend(this, p, SENSOR);	//energy consumption by sensor
	
	delete p;
}

// TODO... Make similar function for 'achTable_'
// Generate CLTable from the neighbour_ table. This is the table M that we defined in our protocol.
// It contains those cluster neighbours which are closer to the BS(sink) than this node.
// (Excludes itself in the Table)
void Node :: generateCLTable() {
	//printf("[INFO]: Generating CLT for node(%d)\n", this->id_);
	if(this->nodeType_ == NCH) {
		fprintf(stderr, "[WARNING]: CLT generation failed. node(%d) is of type NCH.\n", id_);
		return;
	}
	Simulator& sim = Simulator::instance();
	CTableEntry *entry = NULL;
	int i;
	double me_to_bs = distance(this, sim.baseStation());
	double dist_to_bs;
	double dist_to_me;
	for(i=0; i<nneighbours_; i++) {
		if(!neighbours_[i] || ((neighbours_[i]->nodeType() != CH) && (neighbours_[i]->nodeType() != BS)) || (this == neighbours_[i]))
			continue;
		dist_to_bs = distance(neighbours_[i], sim.baseStation());
		dist_to_me = distance(neighbours_[i], this);
		if(dist_to_bs < me_to_bs) {
			entry = new CTableEntry;
			entry->node_ = neighbours_[i];
			entry->distance_to_me_ = dist_to_me;
			entry->distance_to_BS_ = dist_to_bs;
			entry->currentState_ = ACTIVE;	// XXX... May depend.
			//Entry for Assistant CH node
			if(neighbours_[i]->nodeType_ == CH) {
				// Find its ACH and associate it
				entry->assistantNode_ = neighbours_[i]->findAssistantCH();
			}
			chTable_->addEntry(entry);
			delete entry;
		}
	}
	// Sort the table contents in increasing order of 'distance_to_BS_' parameter.
	chTable_->sortByBSDistance();
}

// Generate achTable_ from the neighbour_ table for assistant CH nodes. (Excludes self's ACH)
void Node :: generateACLTable() {
	//printf("[INFO]: Generating CLT for node(%d)\n", this->id_);
	if(this->nodeType_ == NCH) {
		fprintf(stderr, "[WARNING]: CLT generation failed. node(%d) is of type NCH.\n", id_);
		return;
	}
	Simulator& sim = Simulator::instance();
	CTableEntry *entry = NULL;
	int i;
	double me_to_bs = distance(this, sim.baseStation());
	double dist_to_bs;
	double dist_to_me;
	for(i=0; i<nneighbours_; i++) {
		if(!neighbours_[i] || (neighbours_[i]->nodeType() != ACH) ||	\
			(neighbours_[i]->cluster() == this->cluster()) ||(this == neighbours_[i]))
			continue;
		dist_to_bs = distance(neighbours_[i], sim.baseStation());
		dist_to_me = distance(neighbours_[i], this);
		if(dist_to_bs < me_to_bs) {
			entry = new CTableEntry;
			entry->node_ = neighbours_[i];
			entry->distance_to_me_ = dist_to_me;
			entry->distance_to_BS_ = dist_to_bs;
			entry->currentState_ = ACTIVE;	// XXX... May depend.
			//Entry for Assistant CH node = NULL, since it is an ACH itself.
			entry->assistantNode_ = NULL;
			achTable_->addEntry(entry);
			delete entry;
		}
	}
	// Sort the table contents in increasing order of 'distance_to_BS_' parameter.
	achTable_->sortByBSDistance();
}

//Compute and return the ACH of the CH node
Node* Node :: findAssistantCH() {
	
	/* This is checked by the caller...
	if(nodeType_ != CH)
		return NULL;
	*/
	if(assistantCH_) {
		//printf("[INFO]: Assistant node of %d is %d\n", id_, assistantCH_->id());
		return assistantCH_;
	}
	Simulator& sim = Simulator::instance();
	// For all ACH type nodes...
	Node **ptr = sim.nodePtr(sim.nClusters()) + 1; // +1 to go one step of BS node
	for(int i=0; (i < sim.nClusters()) && (ptr[i]->nodeType() == ACH); i++) {
		if(ptr[i]->cluster() == this->cluster()) {
			this->assistantCH_ = ptr[i];
			break;
		}
	}
	//printf("[INFO]: Assistant node of %d is %d\n", id_, assistantCH_->id());
	return assistantCH_;
}

// Generate threshold energy partition
void Node :: partitionEnergy() {
	if(thresholdEnergy_)
		delete[] thresholdEnergy_; //Delete the original and recreate
	thresholdEnergy_ = new double[energyDivisions_+1];
	
	double perPercent = 100.0 / energyDivisions_;
	double perDivisionEnergy = maxEnergy_ * (perPercent / 100.0);
	thresholdEnergy_[0] = maxEnergy_;
	for(int i=1; i<energyDivisions_; i++) {
		thresholdEnergy_[i] = thresholdEnergy_[i-1] - perDivisionEnergy;
	}
	/*
	printf("Energy Partitions = ");
	for(int i=0; i<DEFAULT_ENERGY_DIVISION+1; i++)
		printf("%.2lf\t", thresholdEnergy_[i]);
	printf("\n");
	*/
}

// Check if energy level has reached a threshold
int Node :: reachedThreshold() {
	//pthread_mutex_lock(&threshold_mutex);
	
	// XXX... Not tested yet
	if(thresholdLevel_ > energyDivisions_+1) {
		printf("[INFO]: Node(%d) is dead\n", id_);
		//pthread_mutex_unlock(&threshold_mutex);
		return -2;	//Node dead
	}
	if(energy_ <= thresholdEnergy_[thresholdLevel_]) {
		thresholdLevel_++;
		//pthread_mutex_unlock(&threshold_mutex);
		return 0;	//Reached a threshold bound
	}
	
	//pthread_mutex_unlock(&threshold_mutex);
	
	return -1;	//Not yet
		
}

/* XXX
 * - This is a protocol specific procedure. It may need changes often.
 * - Mind that this code might be crashable if nodes die and are deleted causing their pointers to be NULL.
 */
// Select a next-hop node from chTable_ or achTable_
int Node :: selectNextHop() {
	CTableEntry** ptr = chTable_->entry_;	// Entries are already sorted in increasing distance_to_BS_ order
	CTableEntry* maxEnergy = chTable_->maxEnergyEntry();
	Simulator& sim = Simulator::instance();
	while(*ptr != (CTableEntry*)0) {
		// difference between 'highest_energy' and 'energy_of_currently_selected_node' is compared against 'energyGapThreshold'
		if(ptr[0][0].node_->state() == ACTIVE_MODE) {
			if(abs(maxEnergy->node_->energy() - ptr[0][0].node_->energy()) < sim.energyGapThreshold()) {
				nextHop_ = ptr[0][0].node_;
				return 0;
			}
		}
		//@else {
			//@fprintf(stdout, "[INFO]: Node(%d) is in either SLEEP_MODE or DEAD_MODE. Skipping!\n", ptr[0]->node_->id());
			//@fflush(stdout);
		//@}
		ptr++;
	}
	if(*ptr == (CTableEntry*)0) {
		fprintf(stderr, "[INFO]: Next hop could not be decided for node(%d) in CH Table. Trying in ACH table.\n", id_);
		fflush(stderr);
	}
	
	// TODO... Search in ACH table if not found in CH table
	ptr = achTable_->entry_;	// Entries are already sorted in increasing distance_to_BS_ order
	maxEnergy = achTable_->maxEnergyEntry();
	while(*ptr != (CTableEntry*)0) {
		// difference between 'highest_energy' and 'energy_of_currently_selected_node' is compared against 'energyGapThreshold'
		if(ptr[0][0].node_->state() == ACTIVE_MODE) {
			if(abs(maxEnergy->node_->energy() - ptr[0][0].node_->energy()) < sim.energyGapThreshold()) {
				nextHop_ = ptr[0][0].node_;
				return 0;
			}
		}
		//@else {
			//@fprintf(stdout, "[INFO]: Node(%d) is in either SLEEP_MODE or DEAD_MODE. Skipping!\n", ptr[0]->node_->id());
			//@fflush(stdout);
		//@}
		ptr++;
	}
	if(*ptr == (CTableEntry*)0) {
		return -1;
	}
}

// Broadcast a relaxation packet. This packet is transmitted without being queued.
int Node :: notifyRelax() {
	if(state_ == SLEEP_MODE)
		return -1;
	
	RelaxPacket p(this->id_, BROADCAST_ADDRESS, "[Notify]: News for relaxation.", "relax");
	//p.sourceId_ = id_;
	p.destId_ = BROADCAST_ADDRESS;
	p.forwarderId_ = id_;
	p.notifyType_ = RELAXATION;
	p.currentEnergy_ = energy_;
	
	// Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('n', s.pseudoCurrentTime(), &p);
	
	broadcast(&p);
	//pthread_mutex_lock(&relax_mutex);
	// Change state to SLEEP
	state_ = SLEEP_MODE;
	
	//pthread_mutex_unlock(&relax_mutex);
	
	// TODO...Create a new thread and let it sleep. After the duration of sleep invoke 'notifyActive()' and exit.
	sleepArg *arg = new sleepArg;
	arg->duration_ = s.relaxPeriodTime_;
	arg->node_ = this;
	pthread_t tid;
	pthread_create(&tid, NULL, &sleepPeriod, (void*)arg);
	
	return 0;
}

// Broadcast a activation packet. This packet is transmitted without being queued.
int Node :: notifyActive() {
	if(state_ == ACTIVE_MODE)
		return -1;
	
	RelaxPacket p(this->id_, BROADCAST_ADDRESS, "[Notify]: News for activation.", "active");
	//p.sourceId_ = id_;
	p.destId_ = BROADCAST_ADDRESS;
	p.forwarderId_ = id_;
	p.notifyType_ = ACTIVATION;
	p.currentEnergy_ = energy_;
	
	// Tracing
	Simulator& s = Simulator::instance();
	Trace::instance().format('n', s.pseudoCurrentTime(), &p);
	
	broadcast(&p);
	
	state_ = ACTIVE_MODE;
	
	return 0;
}

// Sleep for 'duration' and wake up as ACTIVE
void* Node :: sleepPeriod(void* arg) {
	sleepArg *ptr = (sleepArg*)arg;
	
	Simulator::instance().delay(ptr->duration_);
	
	ptr->node_->notifyActive();
	
	return (void*)0;
}
