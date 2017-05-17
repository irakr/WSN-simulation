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
	static void spend(Node*, EnergyConsumption_t);
private:
	static const double energyPerBitTx;
	static const double energyPerBitRx;
	static const double energyPerBitSensor;
};

#endif
