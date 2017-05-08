/*
 * net.h
 *
 */
 
#ifndef NET_H_
#define NET_H_

#include <string.h>

// A generic packet format
struct Packet {
	// Header
	int sourceId_, destId_;
	
	//Payload
	char payload[128];
	
	Packet() {
		sourceId_ = destId_ = 0;
		strcpy(payload, "");
	}
};

class NetworkLayer {
public:
	NetworkLayer() {}
};

#endif
