/* nanosleep() test */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main() {
	double sleepTime = 0.0004455, integral, fractional;
	struct timespec time1, time2;
	time1.tv_sec = (long)sleepTime;
	fractional = modf(sleepTime, &integral);
	time1.tv_nsec = (long)(fractional * 10e9);
	printf("tv_sec=%ld, tv_nsec=%ld\n", time1.tv_sec, time1.tv_nsec);
	if(nanosleep(&time1, &time2) < 0) {
		perror("nanosleep");
		exit(1);
	}
	
	return 0;
}
