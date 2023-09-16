#include <stdio.h>
#include <stdlib.h>
#include <time.h>

 void mul_cpx(
    double *a_re,
    double *a_im,
    double *b_re,
    double *b_im,
    double *c_re,
    double *c_im
    );
    
    
int main (){
	double average_time = 0;
	int iLimit = 300000;
	for(int t = 0; t< iLimit; t++){
	 
		clock_t begin = clock();
		int size = 30000;
		double* as_re = (double*)malloc(sizeof(double)*30000);
		double* as_im = (double*)malloc(sizeof(double)*30000);
		double* bs_im = (double*)malloc(sizeof(double)*30000);
		double* bs_re = (double*)malloc(sizeof(double)*30000);
		double* cs_im = (double*)malloc(sizeof(double)*30000);
		double* cs_re = (double*)malloc(sizeof(double)*30000);
	
		int sum = 0;
	
		for (double ix = 0; ix<size; ix++) {
			bs_re[(int)ix] = ix;
			cs_re[(int)ix] = ix;
			bs_im[(int)ix] = ix;
			cs_im[(int)ix] = ix;
		}
	
		for (int ix = 0; ix<size; ix++) {
			mul_cpx(&as_re[ix], &as_im[ix], &bs_re[ix], &cs_re[ix],&bs_im[ix],&cs_im[ix]); 
		}	

   		
		   printf("%f\n",as_im[rand()%1000]);
   		free(as_re);
   		free(as_im);
   		free(bs_re);
   		free(bs_im);
   		free(cs_re);
   		free(cs_im);
   		clock_t end = clock();
		average_time += (double)(end - begin) / CLOCKS_PER_SEC;
   	}
  

   	
   	average_time = average_time/iLimit;
   	printf("Average time per run: %f s\n", average_time);
	return 0;
}
//Compiled with $gcc different_file.c different_file_mul.c -o sim -O2
