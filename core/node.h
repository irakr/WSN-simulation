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

#define DEFAULT_QUEUE_LIMIT 100
#define DEFAULT_ENERGY_DIVISION	5

// Node type
typedef enum{CH, ACH, NCH, BS} NodeType_t;

// Node state
typedef enum{ACTIVE_MODE, SLEEP_MODE, DEAD_MODE} NodeState_t;

class Node;

// Argument to be passed to thread function sleepPeriod()
struct sleepArg {
	Node* node_;
	double duration_;
};

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
		//pktSize_ = PKTHDR_SIZE;
		thresholdLevel_ = 1; //Initial threshold partition number
		energyDivisions_ = DEFAULT_ENERGY_DIVISION;
		thresholdEnergy_ = NULL;
		partitionEnergy();
		state_ = ACTIVE_MODE;
	}
	Node(int nneighbor);
	
	// Accessors
	int id() { return id_; }
	static double maxEnergy() { return maxEnergy_; }
	double energy() { return energy_; }
	void energy(double val) { energy_ = (val<=0) ? 0 : val; }
	double* thresholdEnergy() { return thresholdEnergy_; }
	double transmissionRange() { return transmissionRange_; }
	void transmissionRange(double val) { transmissionRange_ = val; }
	void bandwidth(double bw) { bandwidth_ = bw; }
	double bandwidth() { return bandwidth_; }
	void queueLimit(unsigned int val) { queueLimit_ = val; }
	unsigned int queueLimit() { return queueLimit_; }
	Coordinate location() { return location_; }
	void location(int x, int y, int z) { location_.x = x; location_.y = y; location_.z = z; }
	void eventData(const char* data);
	char* eventData() { return eventData_; }
	Node* nextHop() { return nextHop_; }
	void nodeType(NodeType_t t) { nodeType_ = t; }
	NodeType_t nodeType() { return nodeType_; }
	int cluster() { return clusterId_; }
	void cluster(int cl) { clusterId_ = cl; }
	NodeState_t state() { return state_; }
	void state(NodeState_t st) { state_ = st; }
	CTable* chTable() { return chTable_; }
	
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
	int selectNextHop();
	int forwardData(); // This calls selectNextHop() and forwards packet to it.
	
	int enqueuePkt(Packet*);
	int dequeuePkt();
	int send(Node*);	// Send the first packet from the queue to node
	int send(Node*, Packet*);	// Send the packet p to the node n(for special purposes)
	int sendHighPriority(Node*, Packet*); // Send the packet p to the node n without queueing and constant bandwidth delay
	int recv(Packet*);	// Recieve and enqueu packet p
	int recvHighPriority(Packet*);
	int broadcast(Packet*);	// Broadcast packet p
	int notifyRelax();	// Broadcast a relaxation packet
	int notifyActive();	// Broadcast a activation packet
	static void* sleepPeriod(void*); //A thread function to let a node be in SLEEP_MODE and then back to ACTIVE_MODE
	
	int energyDivisions_; //No of energy partitions to be made
	
private:
	static int ids_;	//ID generated and ID count
	static const double maxEnergy_;
	
	// MAC layer properties
	Coordinate location_;
	double energy_, *thresholdEnergy_;
	double transmissionRange_;
	std::vector<Packet> pktQueue_;
	unsigned int queueLimit_;	//Max pktqueue size after which the packets will be dropped.
	// Keeps track of currently in which partition the energy level of the node lies(Lower bound of a partition)
	// If this value goes beyond (ENERGY_DIVISION+1), the battery is dead.
	int thresholdLevel_;
	double bandwidth_;	// Bandwidth provided by the channel
	
	// Network layer properties
	int id_;	//ID or address
	NodeType_t nodeType_; //CH,ACH,NCH
	int clusterId_; // Cluster number (NOTE: ClusterID == ID of CH node)
	Node *nextHop_;	//Next hop node selected by routing protocol TODO
	Node **neighbours_; //List of neighbour nodes
	int nneighbours_;	// No of neighbours
	CTable *chTable_, *achTable_; //The tabl {M} as represented in the protocol
	//int pktSize_;
	Node *assistantCH_;	// ACH node of a CH node. This remains NULL for NCH and ACH type nodes
	NodeState_t state_;
	double sleepDuration_; // Duration of RELAX period
	
	// Application layer properties
	char eventData_[128];	// Some data collected during detection of an event
};

#include "clTable.h"

#endif
