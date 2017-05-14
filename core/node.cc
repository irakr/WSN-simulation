/*
 * node.cc
 *
 * Implementation of a network node.
 */

#include "node.h"
#include "simulator.h"
#include <math.h>

int Node :: ids_ = 0;	//ID generated and ID count
const double Node :: maxEnergy_ = 1000;

Node :: Node(int nneighbor) {
	id_ = ids_++;
	nodeType_ = NCH;
	location_.x = location_.y = location_.z = 0;
	nextHop_ = NULL;
	queueLimit_ = DEFAULT_QUEUE_LIMIT;
	pktSize_ = DEFAULT_PKTSIZE;
	neighbours_ = new Node*[nneighbor];
	//for(int i=0; i<nneighbor; i++)
		//neighbours_[i] = NULL;
	nneighbours_ = nneighbor;
	chTable_ = new CTable(nneighbours_);
	achTable_ = new CTable(nneighbours_);
	partitionEnergy();
}
	
// Add a neighbour to the neighbour list
int Node :: addNeighbour(int id) {
	Simulator& simulator = Simulator::instance();
	Node *n = simulator.node(id);
	if(!n) {
		fprintf(stderr, "[ERROR]: Unidentified Node. id=%d\n", id);
		return -1;
	}
	printf("[Neighbour adding]: node(%d)<->node(%d) D=%lf", this->id_, n->id_, distance(this, n));
	if(distance(this, n) > this->transmissionRange_)
	{
		//fprintf(stderr, "[WARNING]: addNeighbour() failed for node(%d). node(%d) is out of range\n", this->id_, n->id_);
		putchar('\n');
		return -1;
	}
	
	neighbours_[id] = n;
	n->neighbours_[this->id_] = this;
	printf(" [DONE]\n");
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
	if(pktQueue_.size() == queueLimit_) {
		fprintf(stderr, "[ERROR]: Packet queue full. Dropping packet.\n");
		return -1;
	}
	pktQueue_.push_back(*p);
	return 0;
}

int Node :: dequeuePkt() {
	if(pktQueue_.size() == 0) {
		fprintf(stderr, "[ERROR]: Packet queue is empty. Cannot deque.\n");
		return -1;
	} 
	pktQueue_.erase(pktQueue_.begin());
	return 0;
}

//Deque packet and send to node 'n'
int Node :: send(Node *n) {
	
	if(pktQueue_.size() == 0) {
		fprintf(stderr, "[ERROR]: Packet queue is empty. Cannot send.\n");
		return -1;
	}
	if(!n) {
		fprintf(stderr, "[ERROR]: Recieving node is not defined. Cannot send.\n");
		return -1;
	}
	Packet *p1 = new Packet(pktQueue_.front()); //Backup before dequeueing it
	dequeuePkt();
	p1->destId_ = n->id_;
	n->recv(p1);
	return 0;
}

// Recieve and enqueue packet
int Node :: recv(Packet *p) {
	if(enqueuePkt(p) == -1) {
		// Drop packet
		delete p;
		return -1;
	}
	Packet& p1 = pktQueue_.front();
	printf("[PACKET_RECIEVED]: s=%d d=%d payload='%s'\n", p1.sourceId_, p1.destId_, p1.payload_); 
	return 0;
}

// Generate application data by a Non-CH sensor node
void Node :: eventData(const char* data) {
	// Store data and enqueue into packet queue
	strcpy(eventData_, data);
	Packet p(this->id_, clusterHead()->id_, data);
	if(enqueuePkt(&p) == -1) {
		//XXX... Possibly wait for queue to be available
		fprintf(stderr, "[ERROR]: Packet queue is full. Application data will be dropped or waiting.\n");
	}
	
}

// Generate CLTable from the neighbour_ table. This is the table M that we defined in our protocol.
// It contains those cluster neighbours which are closer to the BS(sink) than this node.
void Node :: generateCLTable() {
	printf("[INFO]: Generating CLT for node(%d)\n", this->id_);
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
		if(!neighbours_[i] || (neighbours_[i]->nodeType() != CH) || (this == neighbours_[i]))
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
			//FIXME... See the output for CLT generation of node(1). Something is still missing.
			if(nodeType_ == CH) {
				// Find its ACH and associate it
				entry->assistantNode_ = neighbours_[i]->findAssistantCH();
			}
			chTable_->addEntry(entry);
			delete entry;
		}
	}
}

//Compute and return the ACH of the CH node
Node* Node :: findAssistantCH() {
	
	/* This is checked by the caller...
	if(nodeType_ != CH)
		return NULL;
	*/
	if(assistantCH_) {
		printf("[INFO]: Assistant node of %d is %d\n", id_, assistantCH_->id());
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
	printf("[INFO]: Assistant node of %d is %d\n", id_, assistantCH_->id());
	return assistantCH_;
}

// Generate threshold energy partition
void Node :: partitionEnergy() {
	double perPercent = 100.0 / ENERGY_DIVISION;
	double perDivisionEnergy = maxEnergy_ * (perPercent / 100.0);
	thresholdEnergy_[0] = maxEnergy_;
	for(int i=1; i<ENERGY_DIVISION; i++) {
		thresholdEnergy_[i] = thresholdEnergy_[i-1] - perDivisionEnergy;
	}
}

// Perform routing logic
void Node :: routeLogic() {
	
}
