/*
 * energy.cc
 *
 * Implementation of energy model of wireless node.
 */

#include "energy.h"
#include "simulator.h"

const double Energy :: energyPerBitTx = 1.04e-03; // Joules per bit
const double Energy :: energyPerBitTxAmp = 1.0e-9; // XXX...Not yet used
const double Energy :: energyPerBitRx = 0.4e-03;
const double Energy :: energyPerBitSensor = 0.3e-03;
const double Energy :: energyIdle = 0.2e-03;

extern pthread_mutex_t energy_mutex;

// Reduce energy level according operation type. Then check whether it is required to broadcast a relaxation packet.
void Energy :: spend(Node *n, Packet *p, EnergyConsumption_t type) {
	pthread_mutex_lock(&energy_mutex);
	
	double energy = n->energy();
	if(type == TX)
		n->energy(energy - (energyPerBitTx * (double)p->size()));
	else if(type == RX)
		n->energy(energy - (energyPerBitRx* (double)p->size()));
	else if(type == SENSOR)
		n->energy(energy - (energyPerBitSensor* (double)p->size()));
	
	pthread_mutex_unlock(&energy_mutex);
	
	// Check for threshold
	if(n->nodeType() != BS) {
		int ret = n->reachedThreshold();
		if((ret == 0) && (n->state() == ACTIVE_MODE))
			n->notifyRelax();
		else if(ret == -2)	//Energy over
			Simulator::instance().killNode(n);
	}
}
