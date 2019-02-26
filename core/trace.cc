/*
 * trace.cc
 *
 * Recording event traces
 */

#include "trace.h"
#include "simulator.h"
#include <string.h>
#include <pthread.h>

extern pthread_mutex_t trace_mutex;

Trace* Trace :: instance_;

// Write the trace data to the tracefile
void Trace :: traceDump(TraceFormat *format) {
	pthread_mutex_lock(&trace_mutex);
	
	fprintf(traceFile_, "%c %lf %d %d %lf %lf %s %lu %u\n",					\
			format->eventType_, format->time_, format->fromNode_,	\
			format->toNode_, format->energyS_, format->energyR_,	\
			format->pktType_, format->pktSize_, format->pktId_);
	fflush(traceFile_);
	
	if(energyTraceFile_) {
		Simulator& sim = Simulator::instance();
		fprintf(energyTraceFile_, "%lf", format->time_);
		for(int i=0; i<sim.nnodes(); i++) {
			if(sim.node(i)->nodeType() != BS)
				fprintf(energyTraceFile_, " %lf", sim.node(i)->energy());
		}
		fprintf(energyTraceFile_, "\n");
		fflush(energyTraceFile_);
	}
	
	pthread_mutex_unlock(&trace_mutex);
}

// Write the trace data in the argument to the tracefile directly
void Trace :: traceDump(const char *format) {
	char *str = new char[strlen(format)+2];
	strcpy(str, format);
	pthread_mutex_lock(&trace_mutex);
	
	fprintf(traceFile_, "%s\n", str);
	fflush(traceFile_);
	
	pthread_mutex_unlock(&trace_mutex);
	delete str;
}

// Generate trace format	XXX... Not so much checking
void Trace :: format(const char event, const double time, const Packet *pkt) {
	if(!pkt) {
		fprintf(stderr, "[ERROR]: Empty argument to Trace::format()\n");
		exit(1);
	}
	Simulator& sim = Simulator::instance();
	TraceFormat format;
	
	format.eventType_ = event;
	format.time_ = time;
	format.fromNode_ = pkt->forwarderId_;
	format.toNode_ = pkt->destId_;
	format.energyS_ = sim.node(pkt->forwarderId_)->energy();
	format.energyR_ = -1;
	if(pkt->destId_ >= 0)
		format.energyR_ = sim.node(pkt->destId_)->energy();
	strcpy(format.pktType_, pkt->type_);
	format.pktSize_ = pkt->size();
	format.pktId_ = pkt->id_;
	
	traceDump(&format);
}
