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

// Represents a wireless sensor node
class Node {
public:
	Node() {
		id_ = ids_++;
		location_.x = location_.y = location_.z = 0;
		nextHop_ = NULL;
		queueLimit_ = DEFAULT_QUEUE_LIMIT;
	}
	
	// Accessors
	int id() { return id_; }
	double& energy() { return energy_; }
	void energy(double val) { energy_ = val; }
	double& transmissionRange() { return transmissionRange_; }
	void transmissionRange(double val) { transmissionRange_ = val; }
	Coordinate& location() { return location_; }
	void location(int x, int y, int z) { location_.x = x; location_.y = y; location_.z = z; }
	
	// Functions ...
	
	void nextHop(Node *n) { nextHop_ = n;}
	void addNeighbour(Node*);	// Add a neighbour to the neighbour list
	
	int enqueuePkt(Packet*);
	int dequeuePkt();
	int send(Node*);
	int recv(Packet*);
	
private:
	static int ids_;	//ID generated and ID count
	
	// Network layer properties
	int id_;	//ID or address
	Node *nextHop_;	//Next hop node selected by routing protocol
	std::vector<Node> neighbours_; //List of neighbour nodes
	
	// MAC layer properties
	Coordinate location_;
	double energy_;
	double transmissionRange_;
	std::vector<Packet> pktQueue;
	int queueLimit_;	//Max pktqueue size after which the packets will be dropped. //TODO... Have to take as input
	
	//Protocol Stack functionalities ///XXX...Not required
	NetworkLayer networkLayer_;
	MACLayer macLayer_;
};

#endif
