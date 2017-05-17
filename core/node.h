/*
 * node.h
 *
 * Implementation of a network node.
 */

#ifndef NODE_H_
#define NODE_H_

#include <iostream>
#include <vector>
#include "net.h"
#include "mac.h"

#define DEFAULT_QUEUE_LIMIT 512

// Node type
typedef enum{CH, ACH, NCH, BS} NodeType_t;

struct CTable; //Forward declaration of 'CTable' from 'clTable.h' include below

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
	Node(int nneighbor);
	
	// Accessors
	int id() { return id_; }
	static double maxEnergy() { return maxEnergy_; }
	double energy() { return energy_; }
	void energy(double val) { energy_ = val; }
	double transmissionRange() { return transmissionRange_; }
	void transmissionRange(double val) { transmissionRange_ = val; }
	Coordinate location() { return location_; }
	void location(int x, int y, int z) { location_.x = x; location_.y = y; location_.z = z; }
	void eventData(const char* data);
	char* eventData() { return eventData_; }
	Node* nextHop() { return nextHop_; }
	void nodeType(NodeType_t t) { nodeType_ = t; }
	NodeType_t nodeType() { return nodeType_; }
	int cluster() { return clusterId_; }
	void cluster(int cl) { clusterId_ = cl; }
	
	// Functions ...
	
	void nextHop(Node *n) { nextHop_ = n;}
	int addNeighbour(int id);	// Add a neighbour to the neighbour list
	double distance(Node*, Node*); // Distance between two nodes
	Node* clusterHead() { return neighbours_[clusterId_]; }
	void generateCLTable();
	void generateACLTable();
	Node *findAssistantCH(); //Store and return ACH node
	//Just return the associated ACH node
	Node *assistantCH() {return assistantCH_; }
	
	
	void partitionEnergy(); // Generate threshold energy partition
	int reachedThreshold(); // Check if energy level has reached a threshold
	
	// Perform routing logic
	void routeLogic();
	void selectNextHop();
	void forwardData(); // This calls selectNextHop() and forwards packet to it.
	
	int enqueuePkt(Packet*);
	int dequeuePkt();
	int send(Node*);	// Send the first packet from the queue to node
	int send(Node*, Packet*);	// Send the packet p to the node n(for special purposes)
	int recv(Packet*);	// Recieve and enqueu packet p
	int broadcast(Packet*);	// Broadcast packet p
	int notifyRelax();	// Broadcast a relaxation packet
	
private:
	static int ids_;	//ID generated and ID count
	static const double maxEnergy_;
	
	// MAC layer properties
	Coordinate location_;
#define ENERGY_DIVISION	5
	double energy_, thresholdEnergy_[ENERGY_DIVISION+1];
	double transmissionRange_;
	std::vector<Packet> pktQueue_;
	unsigned int queueLimit_;	//Max pktqueue size after which the packets will be dropped.
	
	// Network layer properties
	int id_;	//ID or address
	NodeType_t nodeType_; //CH,ACH,NCH
	int clusterId_; // Cluster number (NOTE: ClusterID == ID of CH node)
	Node *nextHop_;	//Next hop node selected by routing protocol TODO
	Node **neighbours_; //List of neighbour nodes
	int nneighbours_;	// No of neighbours
	CTable *chTable_, *achTable_; //The tabl {M} as represented in the protocol
	int pktSize_;
	Node *assistantCH_;	// ACH node of a CH node. This remains NULL for NCH and ACH type nodes
	
	// Application layer properties
	char eventData_[128];	// Some data collected during detection of an event
};

#include "clTable.h"

#endif
