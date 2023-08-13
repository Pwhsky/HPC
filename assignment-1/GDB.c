#include <stdlib.h>
#include <stdio.h>

int main (){
	int* as = NULL;
	int sum = 0;
	
	for (int ix = 0; ix<10; ix++) {
		as[ix] = ix;
	}	
	for (int ix = 0; ix<10; ix++){
		sum += as[ix];
	}
	free(as);
	return 0;
}
