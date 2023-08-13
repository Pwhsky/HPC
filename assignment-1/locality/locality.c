#include <stdlib.h>
#include <stdio.h>
#include <time.h>
int main () {
	clock_t begin = clock();
	
	
   	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
   	printf("Elapsed time: %f s\n",time_spent);
   	
	return 0;
}
