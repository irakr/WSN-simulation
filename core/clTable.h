/*
 * clTable.h
 *
 * Representation of Cluster Neighbour Table.
 */

#ifndef CLTABLE_H_
#define CLTABLE_H_

// Data types for a Cluster Neighbour Table
typedef enum{RELAX, ACTIVE, DEAD} State_t;
struct CTableEntry {
	Node *node_;	// Address of node
	double distance_to_me_;
	double distance_to_BS_;
	// Energy can be extracted from 'node'
	State_t currentState_;
	Node *assistantNode_;
};

struct CTable {
	CTable() { entryCounter_ = maxEntries_ = 0; }
	CTable(int entries);
	
	int entryCounter_, maxEntries_;
	CTableEntry **entry_;
	void addEntry(CTableEntry*);
	void sortByBSDistance();
	CTableEntry* maxEnergyEntry();
};

#endif
