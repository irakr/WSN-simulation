/*
 * net.h
 *
 */
 
#ifndef NET_H_
#define NET_H_

#include <string.h>
#include "node.h"

#define DEFAULT_PKTSIZE	sizeof(Packet)

#define BROADCAST_ADDRESS	-2
#define UNKNOWN_ADDRESS		-1

// A generic packet format
struct Packet {
	// Header
	int sourceId_, destId_, forwarderId_;
	
	//Payload
	char payload_[128];
	
	Packet() {
		sourceId_ = destId_ = 0;
		strcpy(payload_, "");
	}
	Packet(int s, int d, const char *payload) {
		sourceId_ = s;
		destId_ = d;
		strcpy(payload_, payload);
	}
};

// Relaxation packet format
typedef enum{RELAXATION, ACTIVATION} Notify_t;
struct RelaxPacket : public Packet {
	RelaxPacket() : Packet() {}
	Notify_t type_;
	double currentEnergy_;
};

class NetworkLayer {
public:
	NetworkLayer() {}
};

#endif
