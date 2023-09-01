#include <stdlib.h>
#include <math.h>
#include <complex.h>
//In here we want to store the roots to a given polynomial, given degrees 1-9
//d = 1-9


double complex* getRoot(int degree){
	double complex* roots[degree];

	switch (degree){
		case 1:	
			roots[0] = 1.0;	
		case 2:
			roots[0] = 1.0;
			roots[1] =-1.0;		
		case 3:
			roots[0] = 1.0;
			roots[1] = -0.5 + sqrt(3)/2*I;
			roots[2] = -0.5 - sqrt(3)/2*I;

	}
}
