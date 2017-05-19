/*
 * energy.cc
 *
 * Implementation of energy model of wireless node.
 */

#include "energy.h"
#include "simulator.h"

const double Energy :: energyPerBitTx = 1.0;
const double Energy :: energyPerBitRx = 0.5;
const double Energy :: energyPerBitSensor = 0.2;

extern pthread_mutex_t energy_mutex;

// Reduce energy level according operation type. Then check whether it is required to broadcast a relaxation packet.
void Energy :: spend(Node *n, EnergyConsumption_t type) {
	pthread_mutex_lock(&energy_mutex);
	
	double energy = n->energy();
	if(type == TX)
		n->energy(energy - energyPerBitTx);
	else if(type == RX)
		n->energy(energy - energyPerBitRx);
	else if(type == SENSOR)
		n->energy(energy - energyPerBitSensor);
	
	// Check for threshold
	if(n->reachedThreshold() == 0)
		n->notifyRelax();
	else if(n->reachedThreshold() == -2)
		Simulator::instance().killNode(n);
		
	pthread_mutex_unlock(&energy_mutex);
}
