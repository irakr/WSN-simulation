/*
 * trace.h
 *
 * Recording event traces
 */

#ifndef TRACE_H_
#define TRACE_H_

#include <stdio.h>
#include <stdlib.h>


// Trace data format
struct TraceFormat {
	char eventType_;
	double time_;
	int fromNode_;	//node ids
	int toNode_;
	char pktType[16];
	int pktSize_;
	
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
	
	void trace(TraceFormat*); // Write the trace data to the tracefile
private:
	FILE *traceFile_;
	static Trace* instance_;
};

#endif
