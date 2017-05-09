/*
 * --------------------------------------- MAIN ENTRY POINT --------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"

int main(int argc, char *argv[]) {
	if(argc == 1) {
		fprintf(stderr, "[ERROR]: Please provide a configuration file.\n");
		exit(1);
	}
	if(argc > 2) {
		fprintf(stderr, "[ERROR]: Cannot take more than one config file.\n");
		exit(1);
	}
	
	// Initialize simulation environment
	(new Simulator());
	Simulator& simulator = Simulator :: instance();
	simulator.init(argv[1]);
	// Start simulation
	simulator.run();
	return 0;
}
