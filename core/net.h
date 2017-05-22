/*
 * net.h
 *
 */
 
#ifndef NET_H_
#define NET_H_

#include <string.h>
#include <stdlib.h>
//#include "node.h"


#define BROADCAST_ADDRESS	-2
#define UNKNOWN_ADDRESS		-1

//Packet Priority values
#define MIN_PRIORITY		1
#define NORMAL_PRIORITY		5
#define MAX_PRIORITY		10

// A generic packet format
struct Packet {
	// Header
	int sourceId_, destId_, forwarderId_;
	
	//Payload
	char *payload_;
	
	// Unique Packet ID
	static int ids;
	int id_;
	unsigned short priority_;
	
	char type_[16]; // Packet type
	
	Packet() {
		id_ = ids++;
		sourceId_ = destId_ = 0;
		priority_ = NORMAL_PRIORITY;
		strcpy(payload_, "");
	}
	Packet(int s, int d, const char *payload, const char *type) {
		id_ = ids++;
		sourceId_ = s;
		destId_ = d;
		if(strcmp(type, "sensor") == 0)
			priority_ = NORMAL_PRIORITY;
		else if((strcmp(type, "relax") == 0) || (strcmp(type, "active") == 0))
			priority_ = MAX_PRIORITY;
		payload_ = new char[strlen(payload)+1];
		strcpy(payload_, payload);
		strcpy(type_, type);
	}
	
	~Packet() { 
		// NOTE: The data pointed by pointer payload_ will get deleted by the STL vector functions that
		// I have used in the code elsewhere. So doing a delete again generates a 'double free' error at runtime.
		//XXX... delete payload_;
	}
	
	// Size of packet including payload size
	size_t size() const { return (sizeof(Packet) + strlen(payload_)); }
};

// Relaxation packet format
typedef enum{RELAXATION, ACTIVATION} Notify_t;
struct RelaxPacket : public Packet {
	RelaxPacket() : Packet() {}
	RelaxPacket(int s, int d, const char *payload, const char *type) 
	: Packet(s, d, payload, type) {	}
	
	Notify_t notifyType_;
	double currentEnergy_;
};

#endif
