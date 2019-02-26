/*
 * energy.cc
 *
 * Implementation of energy model of wireless node.
 */

#include "energy.h"
#include "simulator.h"

const double Energy :: energyPerBitTx = 50.0e-6; //	Joules per bit
const double Energy :: energyPerBitTxAmp = 100.0e-7;	//	J/bit/m2
const double Energy :: energyPerBitRx = 50.0e-6;
const double Energy :: energyPerBitSensor = 45.0e-6;
const double Energy :: energyIdle = 40.0e-6;

extern pthread_mutex_t energy_mutex;

// Reduce energy level according operation type. Then check whether it is required to broadcast a relaxation packet.
void Energy :: spend(Node *n, Packet *p, EnergyConsumption_t type) {
	pthread_mutex_lock(&energy_mutex);
	
	Simulator& sim = Simulator::instance();
	
	double energy = n->energy();
	int k = p->size() * 8;	// Packet size in bits
	
	if(type == TX) {
		double d;	// Distance to receiver node
		if(p->destId_ < 0)
			d = n->chTable()->entry_[0]->distance_to_me_; //The furthest neighbour to 'n'
		else
			d = n->distance(n, sim.node(p->destId_));
		n->energy(energy - ((energyPerBitTx * (double)k) + (energyPerBitTxAmp * (double)k * d*d) ) );
	}
	else if(type == RX)
		n->energy(energy - (energyPerBitRx * k));
	else if(type == SENSOR)
		n->energy(energy - (energyPerBitSensor * k));
	
	pthread_mutex_unlock(&energy_mutex);
	
	// Check for threshold
	if(n->nodeType() != BS) {
		int ret = n->reachedThreshold();
		if(ret == 0)
			n->notifyRelax();
		else if(ret == -2)	//Energy over
			Simulator::instance().killNode(n);
	}
}
