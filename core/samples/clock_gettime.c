#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	struct timespec start, end;
	
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(unsigned long i=0;i<999990000; i++);
	clock_gettime(CLOCK_MONOTONIC, &end);
	
	double elapsed = (end.tv_sec - start.tv_sec);
	elapsed += (end.tv_nsec - start.tv_nsec) / 1e+9;
	printf("Elapsed time = %lf\n", elapsed);
	return 0;
}
