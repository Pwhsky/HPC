#include <stdlib.h>
#include <stdio.h>
#include <time.h>



void row_sums(double * sums, double ** matrix, size_t nrs, size_t ncs){
	for(size_t ix = 0; ix < nrs; ix++){
		double sum = 0.;
		for (size_t jx = 0 ; jx < ncs; jx++) {
			sum += matrix[ix][jx];
		}
		sums[ix] = sum;
	}
}
void col_sums(double * sums, double **matrix, size_t nrs, size_t ncs){
	for(size_t ix = 0; ix <ncs; ix++) {
		double sum = 0.;
		for(size_t jx = 0; jx < nrs;jx++){
			sum+=matrix[ix][jx];
		}
		sums[ix] = sum;
	}
}
const int size = 1000;

int main () {

	srand((unsigned) time(NULL));
	clock_t begin = clock();
	//Generate matrix
	double ** matrix = (double**)malloc(sizeof(double*)*size);
	for(int i = 0; i<size;i++){
		matrix[i] = (double*)malloc(sizeof(double)*size);
	}
	
	
	for (int i = 0; i< size;i++ ){
		for (int j = 0; j<size; j++){
		matrix[i][j] = 1;
		
		}
	}
	//Compute sums of matrix cols and rows
	
	double sums[size];
	row_sums(sums,matrix,size,size);
	col_sums(sums,matrix,size,size);
	
	int randindex = rand()%size;
	printf("%lf\n",sums[randindex]);
   	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	
	for (int i = 0; i<size; i++) {
		free(matrix[i]);
	}
	free(matrix);
	
   	printf("Elapsed time: %f s\n",time_spent);
   	
	return 0;
}
