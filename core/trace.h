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
	char pktType_[16];
	unsigned short pktSize_;
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
		instance_ = this;
	}
	~Trace() {
		if(traceFile_)
			fclose(traceFile_);
	}
	static Trace& instance() { return *instance_; }
	FILE* traceFile() { return traceFile_; }
	
	void traceDump(TraceFormat*); // Write the trace data from the traceformat argument to the tracefile
	void traceDump(const char*); // Write the trace data in the argument to the tracefile directly
	void format(const char event, const double time, const Packet *pkt);
private:
	FILE *traceFile_;
	static Trace* instance_;
};

#endif
