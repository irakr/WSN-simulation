/*
 * --------------------------------------- MAIN ENTRY POINT --------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "simulator.h"

pthread_mutex_t trace_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t energy_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t nodeData_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t simulatorData_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threshold_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t eventData_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pktEnqueue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pktDequeue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t forwardData_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t relax_mutex = PTHREAD_MUTEX_INITIALIZER;

void cleanUp();

static Simulator *simulator_ptr;

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
	simulator_ptr = new Simulator();
	Simulator& simulator = Simulator :: instance();
	simulator.init(argv[1]);
	// Start simulation
	simulator.run();
	
	cleanUp();
	
	return 0;
}

void cleanUp() {
	// Cleanup all remaining object like simulator, trace, mutex_locks, etc...
	
	delete simulator_ptr;
	pthread_mutex_destroy(&trace_mutex);
	pthread_mutex_destroy(&energy_mutex);
	pthread_mutex_destroy(&simulatorData_mutex);
	pthread_mutex_destroy(&threshold_mutex);
	pthread_mutex_destroy(&recv_mutex);
	pthread_mutex_destroy(&send_mutex);
	pthread_mutex_destroy(&eventData_mutex);
	pthread_mutex_destroy(&pktEnqueue_mutex);
	pthread_mutex_destroy(&pktDequeue_mutex);
	pthread_mutex_destroy(&forwardData_mutex);
	pthread_mutex_destroy(&relax_mutex);
}
