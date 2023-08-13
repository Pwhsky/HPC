#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include <math.h>

double getDistance(double x, double y, double z) {
	double output = roundf(sqrtf(x*x + y*y + z*z));
	return output;
}

int main (int argc, char** argv) {
	//parse arguments
	int threads = 2;
	if (argc<2){
	printf("No arguments given, default threads = 2\n");
	}else {
		threads = *(*(argv + 1)+2);
	}

	//open file:
		char cwd[PATH_MAX];
		char filename[] = "/cells";
		getcwd(cwd,sizeof(cwd));
		strcat(cwd,filename);
		FILE *file = fopen(cwd,"r");
		if (file == NULL){
			printf("Error: failed to open file \n");
			return 1;
		}
	//Parse contents of file:
	double *x = (double*)malloc(sizeof(double)*10);
	double *y = (double*)malloc(sizeof(double)*10);
	double *z = (double*)malloc(sizeof(double)*10);
	
	double i,j,k;
	int index = 0;
	while (fscanf(file, "%lf %lf %lf", &i,&j,&k) == 3){
		x[index] = i;
		y[index] = j;
		z[index] = k;
		index++;
	}
	printf("%.3lf\n",x[6]);

	//an entry is 7 chars

	//Close file
	free(x);
	free(y);
	free(z);
	fclose(file);
	return 0;
}
