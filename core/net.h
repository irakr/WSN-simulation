/*
 * net.h
 *
 */
 
#ifndef NET_H_
#define NET_H_

#include <string.h>
//#include "node.h"

#define DEFAULT_PKTSIZE	sizeof(Packet)

#define BROADCAST_ADDRESS	-2
#define UNKNOWN_ADDRESS		-1

// A generic packet format
struct Packet {
	// Header
	int sourceId_, destId_, forwarderId_;
	
	//Payload
	char payload_[128];
	
	// Unique Packet ID
	static int ids;
	int id_;
	
	char type_[16]; // Packet type
	
	Packet() {
		id_ = ids++;
		sourceId_ = destId_ = 0;
		strcpy(payload_, "");
	}
	Packet(int s, int d, const char *payload, const char *type) {
		id_ = ids++;
		sourceId_ = s;
		destId_ = d;
		strcpy(payload_, payload);
		strcpy(type_, type);
	}
};

// Relaxation packet format
typedef enum{RELAXATION, ACTIVATION} Notify_t;
struct RelaxPacket : public Packet {
	RelaxPacket() : Packet() {}
	RelaxPacket(int s, int d, const char *payload, const char *type) {
		Packet(s, d, payload, type);
	}
	Notify_t notifyType_;
	double currentEnergy_;
};

class NetworkLayer {
public:
	NetworkLayer() {}
};

#endif
