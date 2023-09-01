nclude <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int lines;
int threads;
int degree;
int maxSteps = 128;
double tolerance = 0.001;

/* Pseudo-code for newtonRhapson
complex double newtonRhapson(complex double x0 ){
	complex double x = x0;
	complex double newX;
	complex double root; 
	for (int i = 0; i<maxSteps; i++){
		newX = x*( (d+1)/d * (cpow(x,d)-1));

		if(abs(newX) - abs(root) <= tolerance){
			Then the point converges to the root. 
			break;
		}
	}
	return which root point converges to.
}
*/
int main(int argc, char** argv) {
	//Ugly as sin, sorry.
	char* arg1   = argv[1];
	char* arg2   = argv[2];
	char  arg3   = argv[3];
	char  char1  = arg1[1];
	char  char2  = arg2[1];
	
	degree = (**(argv+3))-'0';

	//Parse args ./newton -t5 -l1000 7 in any arbitrary order
	if (char1 == 't' && char2 == 'l')  {
		threads = arg1[2]-'0';
		lines = atoi(arg2+(strlen(arg2)- (strlen(arg2)-2)    )  );

	}else if (char1 == 'l' && char2 == 't'){
		threads = arg2[2]-'0';
		lines = atoi(arg1+(strlen(arg1)- (strlen(arg1)-2)    )  );
	}
	complex double grid[lines][lines];
		
	//Parse the args: degree will be less than 10
	
		
	return 0;
}
