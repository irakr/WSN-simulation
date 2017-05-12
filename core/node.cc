/*
 * node.cc
 *
 * Implementation of a network node.
 */

#include "node.h"
#include "simulator.h"
#include <math.h>

int Node :: ids_ = 0;	//ID generated and ID count

// Add a neighbour to the neighbour list
int Node :: addNeighbour(int id) {
	Simulator& simulator = Simulator::instance();
	Node *n = simulator.node(id);
	if(!n) {
		fprintf(stderr, "[ERROR]: Unidentified Node. id=%d\n", id);
		return -1;
	}
	printf("[Neighbour adding]: node(%d)<->node(%d) D=%lf\n", this->id_, n->id_, distance(this, n));
	if(distance(this, n) > this->transmissionRange_)
	{
		fprintf(stderr, "[WARNING]: addNeighbour() failed for node(%d). node(%d) is out of range\n", this->id_, n->id_);
		return -1;
	}
	
	neighbours_[id] = n;
	return 0;
}

// Distance between two nodes
double Node :: distance(Node *n1, Node *n2) {
	int x1 = n1->location_.x;
	int y1 = n1->location_.y;
	int x2 = n2->location_.x;
	int y2 = n2->location_.y;
	return (sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) ));
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

//Deque packet and send
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

// Generate application data
void Node :: eventData(const char* data) {
	// Store data and enqueue into packet queue
	strcpy(eventData_, data);
	Packet p(this->id_, -1, data);	// NOTE: Destination ID is not defined now == -1
	if(enqueuePkt(&p) == -1) {
		//XXX... Possibly wait for queue to be available
		fprintf(stderr, "[ERROR]: Packet queue is full. Application data will be dropped or waiting.\n");
	}
	
}
