/*
 * node.h
 *
 * Implementation of a network node.
 */

#ifndef NODE_H_
#define NODE_H_

#include "net.h"
#include "mac.h"
#include <iostream>
#include <vector>

#define DEFAULT_QUEUE_LIMIT 512

// Node type
typedef enum{CH, ACH, NCH, BS} NodeType_t;

// Represents a wireless sensor node
class Node {
public:
	Node() {
		id_ = ids_++;
		nodeType_ = NCH;
		location_.x = location_.y = location_.z = 0;
		nextHop_ = NULL;
		queueLimit_ = DEFAULT_QUEUE_LIMIT;
		pktSize_ = DEFAULT_PKTSIZE;
	}
	Node(int nneighbor) {
		id_ = ids_++;
		nodeType_ = NCH;
		location_.x = location_.y = location_.z = 0;
		nextHop_ = NULL;
		queueLimit_ = DEFAULT_QUEUE_LIMIT;
		pktSize_ = DEFAULT_PKTSIZE;
		neighbours_ = new Node*[nneighbor];
	}
	
	// Accessors
	int id() { return id_; }
	double& energy() { return energy_; }
	void energy(double val) { energy_ = val; }
	double& transmissionRange() { return transmissionRange_; }
	void transmissionRange(double val) { transmissionRange_ = val; }
	Coordinate& location() { return location_; }
	void location(int x, int y, int z) { location_.x = x; location_.y = y; location_.z = z; }
	void eventData(const char* data);
	char* eventData() { return eventData_; }
	Node *nextHop() { return nextHop_; }
	void nodeType(NodeType_t t) { nodeType_ = t; }
	
	// Functions ...
	
	void nextHop(Node *n) { nextHop_ = n;}
	int addNeighbour(int id);	// Add a neighbour to the neighbour list
	double distance(Node*, Node*); // Distance between two nodes
	
	int enqueuePkt(Packet*);
	int dequeuePkt();
	int send(Node*);
	int recv(Packet*);
	
private:
	static int ids_;	//ID generated and ID count
	
	// Network layer properties
	int id_;	//ID or address
	NodeType_t nodeType_; //CH,ACH,NCH
	Node *nextHop_;	//Next hop node selected by routing protocol
	Node **neighbours_; //List of neighbour nodes
	int pktSize_;
	
	// MAC layer properties
	Coordinate location_;
	double energy_;
	double transmissionRange_;
	std::vector<Packet> pktQueue_;
	int queueLimit_;	//Max pktqueue size after which the packets will be dropped. //TODO... Have to take as input
	
	// Application layer properties
	char eventData_[128];	// Some data collected during detection of an event
	 
	//Protocol Stack functionalities ///XXX...Not required
	NetworkLayer networkLayer_;
	MACLayer macLayer_;
};

#endif
