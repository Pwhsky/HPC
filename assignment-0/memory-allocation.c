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

//Contiguous
int size = 10;

int * asentries = (int*) malloc(sizeof(int) * size*size);
int ** as = (int**) malloc(sizeof(int*) * size);
for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size )
  as[ix] = asentries + jx;

for ( size_t ix = 0; ix < size; ++ix )
  for ( size_t jx = 0; jx < size; ++jx )
    as[ix][jx] = 0;

printf("%d\n", as[0][0]);

free(as);
free(asentries);


Non-contiguous
int size = 10;

int ** as = (int**) malloc(sizeof(int*) * size);
for ( size_t ix = 0; ix < size; ++ix )
  as[ix] = (int*) malloc(sizeof(int) * size);

for ( size_t ix = 0; ix < size; ++ix )
  for ( size_t jx = 0; jx < size; ++jx )
    as[ix][jx] = 0;

printf("%d\n", as[0][0]);

for ( size_t ix = 0; ix < size; ++ix )
    free(as[ix]);
free(as);


*/
