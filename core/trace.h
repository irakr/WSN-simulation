/*
 * trace.h
 *
 * Recording event traces
 */

#ifndef TRACE_H_
#define TRACE_H_

#include <stdio.h>
#include <stdlib.h>

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
private:
	FILE *traceFile_;
	static Trace* instance_;
};

#endif
