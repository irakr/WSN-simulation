/*
 * clTable.cc
 */

#include "node.h"
#include "clTable.h"

CTable :: CTable(int entries) {
	entryCounter_ = 0;
	maxEntries_ = entries;
	entry_ = new CTableEntry*[entries];
}

// Add a new entry
void CTable :: addEntry(CTableEntry *entry) {
	entry_[entryCounter_++] = new CTableEntry(*entry);
}

// Sort according to 'distance_to_BS_' in increasing order
void CTable :: sortByBSDistance() {
	CTableEntry temp;
	int i, j, minIndex;
	for(i=0; i < entryCounter_; i++) {	//i is the location where the smallest item from i+1 to n-1 will be swapped by its current item 
		minIndex = i;
		for(j=i+1; j < entryCounter_; j++) {
			//Find the smallest item
			if(entry_[j]->distance_to_BS_ < entry_[minIndex]->distance_to_BS_)
				minIndex = j;
		}
		//swap it with item[i]
		if(minIndex != i) {
			temp = *(entry_[minIndex]);
			*(entry_[minIndex]) = *(entry_[i]);
			*(entry_[i]) = temp;
		}
	}
}

// Return the entry with the highest energy node
CTableEntry* CTable :: maxEnergyEntry() {
	CTableEntry** ptr = entry_;
	CTableEntry* maxEnergyNodeEntry = ptr[0];
	for(int i=1; i < entryCounter_; i++) {
		if(ptr[i]->node_->state() == ACTIVE_MODE) {
			if(ptr[i]->node_->energy() > maxEnergyNodeEntry->node_->energy())
				maxEnergyNodeEntry = ptr[i];
		}
	}
	return maxEnergyNodeEntry;
}
