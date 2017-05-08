/*
 * proto.hh
 *
 * Implementation of the protocol logic.
 */

#ifndef PROTO_H_
#define PROTO_H_

#include "node.h"
#include "net.h"
#include "mac.h"

// Relaxation packet format
typedef enum{RELAX, ACTIVE} Notify_t;
struct RelaxPacket : public Packet {
	RelaxPacket() : Packet() {}
	Notify_t type_;
	double currentEnergy_;
};

#endif
