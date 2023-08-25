#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {
double avgTime=0;
for (int t = 0; t<1000;t++){
    clock_t begin = clock();
	int a = 2;
	size_t size = 100000;
	//Linear initialization
	int* p = (int*)malloc(sizeof(int)*size);
	int* y = (int*)malloc(sizeof(int)*size);
	int* x = (int*)malloc(sizeof(int)*size);
	
	for(size_t ix = 0; ix < size; ix++){
		p[ix] = ix;
	}
	
	for(size_t kx = 0; kx<size; kx++){
		size_t jx = p[kx];
		y[jx] += a*x[jx];
	}
	    clock_t end = clock();
  	  avgTime += (double)(end - begin)/ CLOCKS_PER_SEC;

printf("Random element: %zu\n", y[rand()%size]);
  	

  	free(y);
    	free(x);
    	free(p);
}
avgTime /= 1000;


    	printf("average time: %f s\n", avgTime);
	return 0;
}
