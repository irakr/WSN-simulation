#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main() {
 clock_t start, end;
 double cpu_time_used;
 char str[128]="";
 start = clock();
 strcpy(str, "a");
 //printf("a\n");
 //sleep(1);
 end = clock();
 cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 //cpu_time_used = ((double) (end - start));
 printf("Execution Duration = %lf\n", cpu_time_used);
 return 0;
}

