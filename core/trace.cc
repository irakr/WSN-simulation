/*
 * trace.cc
 *
 * Recording event traces
 */

#include "trace.h"
#include <string.h>

Trace* Trace :: instance_;

// Write the trace data to the tracefile
void Trace :: traceDump(TraceFormat *format) {
	fprintf(traceFile_, "%c %lf %d %d %s %u %u\n", 					\
			format->eventType_, format->time_, format->fromNode_,	\
			format->toNode_, format->pktType_, format->pktSize_,	\
			format->pktId_);
	fflush(traceFile_);
}

// Write the trace data in the argument to the tracefile directly
void Trace :: traceDump(const char *format) {
	char *str = new char[strlen(format)+2];
	strcpy(str, format);
	fprintf(traceFile_, "%s\n", str);
	fflush(traceFile_);
}

// Generate trace format
void Trace :: format(const char event, const double time, const Packet *pkt) {
	if(!pkt) {
		fprintf(stderr, "[ERROR]: Empty argument to Trace::format()\n");
		exit(1);
	}
	TraceFormat format;
	format.eventType_ = event;
	format.time_ = time;
	format.fromNode_ = pkt->forwarderId_;
	format.toNode_ = pkt->destId_;
	strcpy(format.pktType_, pkt->type_);
	format.pktSize_ = sizeof(*pkt);
	format.pktId_ = pkt->id_;
	
	traceDump(&format);
}
