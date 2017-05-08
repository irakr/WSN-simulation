/*
 * node.cc
 *
 * Implementation of a network node.
 */

#include "node.h"

int Node :: ids_ = 0;	//ID generated and ID count

// Add a neighbour to the neighbour list
void Node :: addNeighbour(Node *n) {
	neighbours_.push_back(*n);
}

int Node :: enqueuePkt(Packet *p) {
	if(pktQueue.size() == queueLimit_) {
		fprintf(stderr, "[ERROR]: Packet queue full. Dropping packet.\n");
		return -1;
	}
	pktQueue.push_back(*p);
	return 0;
}

int Node :: dequeuePkt() {
	if(pktQueue.size() == 0) {
		fprintf(stderr, "[ERROR]: Packet queue is empty. Cannot deque.\n");
		return -1;
	} 
	pktQueue.erase(pktQueue.begin());
	return 0;
}

//Deque packet and send
int Node :: send(Node *n) {
	
	if(pktQueue.size() == 0) {
		fprintf(stderr, "[ERROR]: Packet queue is empty. Cannot send.\n");
		return -1;
	}
	
	Packet *p1 = new Packet(pktQueue.front()); //Backup before dequeueing it
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
	return 0;
}
