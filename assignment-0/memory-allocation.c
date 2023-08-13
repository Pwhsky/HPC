#include <stdio.h>
#include <stdlib.h>


//Heap memory allocation:
int main() {
	int size = 1000000000;
	int * as = (int*) malloc(sizeof(int) * size);
	
	for ( size_t ix = 0; ix < size; ++ix ){
  		as[ix] = 0;
	}
	printf("%d\n", as[0]);

}

//stack memory allocation: (segmentation fault)
/*
int main() {
	int size = 1000000000;
	int as[size]; 
	
	for ( size_t ix = 0; ix < size; ++ix ){
  		as[ix] = 0;
	}
	printf("%d\n", as[0]);

}
*/
