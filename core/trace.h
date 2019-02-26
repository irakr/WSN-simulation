/*
 * trace.h
 *
 * Recording event traces
 */

#ifndef TRACE_H_
#define TRACE_H_

#include <stdio.h>
#include <stdlib.h>
#include "net.h"

// Trace data format
struct TraceFormat {
	char eventType_;
	double time_;
	int fromNode_;	// Node ids
	int toNode_;
	double energyS_; // sender's energy
	double energyR_; // reveiver's energy
	char pktType_[16];
	size_t pktSize_;
	unsigned int pktId_; // Unique id of pkt
};

class Trace {
public:
	Trace() {}
	Trace(char *trace_file) {
		traceFile_ = fopen(trace_file, "w");
		if(!traceFile_) {
			fprintf(stderr, "[ERROR]: Could not open trace file '%s'\n", trace_file);
			exit(1);
		}
		energyTraceFile_ = NULL;
		instance_ = this;
	}
	Trace(char *trace_file, char *energyTraceFile) {
		traceFile_ = fopen(trace_file, "w");
		if(!traceFile_) {
			fprintf(stderr, "[ERROR]: Could not open trace file '%s'\n", trace_file);
			exit(1);
		}
		energyTraceFile_ = fopen(energyTraceFile, "w");
		if(!energyTraceFile_) {
			fprintf(stderr, "[ERROR]: Could not open energy trace file '%s'\n", energyTraceFile);
			exit(1);
		}
		instance_ = this;
	}
	~Trace() {
		if(traceFile_)
			fclose(traceFile_);
		if(energyTraceFile_)
			fclose(energyTraceFile_);
	}
	static Trace& instance() { return *instance_; }
	FILE* traceFile() { return traceFile_; }
	FILE* energyTraceFile() { return energyTraceFile_; }
	void energyTraceFile(char *fname) {
		energyTraceFile_ = fopen(fname, "w");
		if(!energyTraceFile_) {
			fprintf(stderr, "[ERROR]: Could not open energy trace file '%s'\n", fname);
			exit(1);
		}
	}
	
	void traceDump(TraceFormat*); // Write the trace data from the traceformat argument to the tracefile
	void traceDump(const char*); // Write the trace data in the argument to the tracefile directly
	void format(const char event, const double time, const Packet *pkt);
private:
	FILE *traceFile_, *energyTraceFile_;
	static Trace* instance_;
};

#endif
