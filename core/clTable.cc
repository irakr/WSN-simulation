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

void CTable :: addEntry(CTableEntry *entry) {
	entry_[entryCounter_++] = new CTableEntry(*entry);
}
