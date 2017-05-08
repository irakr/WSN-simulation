/*
 * simulator.h
 *
 * Implementation of the simulator engine.
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "scheduler.h"

class Simulator {
public:
	Simulator(char*);
	~Simulator() {
		fclose(configFile_);
		delete[] nodes_;
	}
	
	void init();
	void run();
private:
	FILE *configFile_; //Input config file
	int nnodes_, pktSize_;
	char proto[128]; //Protocol name
	Node *nodes_; //Array of node objects
};

#endif
