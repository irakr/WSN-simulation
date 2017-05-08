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
