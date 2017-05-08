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

// Represents a wireless sensor node
class Node {
public:
	Node() {
		id_ = ids_++;
	}
	
	// Accessors
	int id() { return id_; }
	double& energy() { return energy_; }
	void energy(double val) { energy_ = val; }
	double& transmissionRange() { return transmissionRange_; }
	void transmissionRange(double val) { transmissionRange_ = val; }
	Coordinate& location() { return location_; }
	void location(int x, int y, int z) { location_.x = x; location_.y = y; location_.z = z; }

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
	
	
	//Protocol Stack functionalities
	NetworkLayer networkLayer_;
	MACLayer macLayer_;
};

#endif
