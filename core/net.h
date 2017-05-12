/*
 * net.h
 *
 */
 
#ifndef NET_H_
#define NET_H_

#include <string.h>

#define DEFAULT_PKTSIZE	sizeof(Packet)

// A generic packet format
struct Packet {
	// Header
	int sourceId_, destId_;
	
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

class NetworkLayer {
public:
	NetworkLayer() {}
};

#endif
