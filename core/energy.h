/*
 * energy.h
 *
 * Implementation of energy model of wireless node.
 */

#ifndef ENERGY_H_
#define ENERGY_H_

#include "node.h"

typedef enum{TX, RX, SENSOR, COMPUTE} EnergyConsumption_t;
class Energy {
public:
	static void spend(Node*, Packet*, EnergyConsumption_t);
private:
	// XXX... Currently these values are per operation rather than per bit.
	static const double energyPerBitTx;
	static const double energyPerBitTxAmp;
	static const double energyPerBitRx;
	static const double energyPerBitSensor;
	static const double energyIdle;
};

#endif
