/*
 * simulator.cc
 *
 * Implementation of the simulator engine.
 */
 
#include "simulator.h"
#include <string.h>

Simulator :: Simulator(char *ifile) {
	configFile_ = fopen(ifile, "r");
	if(!configFile_) {
		fprintf(stderr, "[ERROR]: Could not open simulation file %s", ifile);
		exit(1);
	}
	init();
}

/* Extracts simulation and network configuration data from the input file and initializes
 * the environment.
 */
void Simulator :: init() {
	char line[512] = "", *temp;
	while(!feof(configFile_)) {
		fgets(line, 512, configFile_);
		line[strlen(line)-1] = '\0';
		if((*line == '\0') || (*line == '#'))	//Ignore comment and blank lines
			continue;
		if(strcmp(line, "END") == 0)
			break;
		//Read the configuration attribute type and values
		temp = strtok(line, "=");
		if(strcmp(temp, "Protocol") == 0) {	// Protocol Name
			//Get the value
			temp = strtok(NULL, "=");
			strcpy(proto, temp);
		}
		else if(strcmp(temp, "Nodes") == 0) {	// Node count
			//Get the value
			temp = strtok(NULL, "=");
			nnodes_ = atoi(temp);
			nodes_ = new Node[nnodes_];
		}
		else if(strcmp(temp, "Attributes") == 0) {	// Node attributes(Location coordinates, energy, etc...)
			for(int i = 0; i < nnodes_; i++) {	// There must be nnodes_ lines that defines the node attributes from this point on.
				fgets(line, 512, configFile_);
				line[strlen(line)-1] = '\0';
				//Coordinates...Process still pending
				char coord[64]="";
				temp = strtok(line, ":");
				strcpy(coord, temp);
				
				//Transmission Range value
				temp = strtok(NULL, ":");
				nodes_[i].transmissionRange(atof(temp));
				
				// Energy level
				temp = strtok(NULL, ":");
				nodes_[i].energy(atof(temp));
				
				//Packet size
				temp = strtok(NULL, ":");
				pktSize_ = atoi(temp);
				
				//TODO...Anymore attribute then add here
				
				//Extract coordinates
				nodes_[i].location().x = atoi(strtok(coord, ","));
				nodes_[i].location().y = atoi(strtok(NULL, ","));
				nodes_[i].location().z = atoi(strtok(NULL, ","));
			}
		}
		else {
			// Unknown configuration
			fprintf(stderr, "[WARNING]: Unknown configuration attribute : '%s'\n", temp);
		}
	}
}

void Simulator :: run() {
	
}

//
//
//
//
//
//
//
//
//
//
//
//
/*
 * --------------------------------------- MAIN ENTRY POINT --------------------------------------
 */

int main(int argc, char *argv[]) {
	if(argc == 1) {
		fprintf(stderr, "[ERROR]: Please provide a configuration file.\n");
		exit(1);
	}
	if(argc > 2) {
		fprintf(stderr, "[ERROR]: Cannot take more than one config file.\n");
		exit(1);
	}
	Simulator *simulator = new Simulator(argv[1]);
	
	return 0;
}
