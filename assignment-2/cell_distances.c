#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include <math.h>
//Program cannot use more than 5Mb of memory, i.e 5*1024*1024
//Maximal theoretical rows = (5*1024*1024)/(bytes of a certain datatype)


const int nDistances = 3465;

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
	omp_set_num_threads(threads);
	
	
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
	//Parse contents of file: 10 will be the size of a block
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
		float innerProduct;
		int printval;
		for(int i = 0; i<(10-1);i++){
			for(int j = i+1; j< 10; j++){
			
			
			innerProduct = sqrtf((x[i]-x[j])*(x[i]-x[j]) + (y[i]-y[j])*(y[i]-y[j]) + (z[i]-z[j])*(z[i]-z[j]));
			printf("%f\n",innerProduct);
			
			}	
			
			
		}

	//an entry is 7 chars

	//Close file
	free(x);
	free(y);
	free(z);
	fclose(file);
	return 0;
}
