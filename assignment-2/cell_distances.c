#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include <sys/resource.h>



//Program cannot use more than 5Mb of memory, i.e 5*1024*1024
//Maximal theoretical entries = (5*1024*1024)/(bytes of a certain datatype)


const int nDistances = 3465;
static size_t count[3465]; //The number of unique distances


static inline void processBlock(float *x, float* y, float* z, size_t blockSize){
    extern  size_t count[];

         float innerProduct;
	int   distance;

	for(int i = 0; i<(blockSize-1);i++){
	    	for(int j = i+1; j< blockSize; j++){
	            innerProduct = sqrtf((x[i]-x[j])*(x[i]-x[j]) + (y[i]-y[j])*(y[i]-y[j]) + (z[i]-z[j])*(z[i]-z[j]));
	            distance = (int)(100*sqrtf(innerProduct));
	            count[distance] += 1;
        //add innerproduct to list of distances
	    	}
	}

}



int main (int argc, char** argv) {
	//parse arguments
	int threads = 2;
	if (argc<2){
	printf("No arguments given, default threads = 2\n");
	}else {
		threads = *(*(argv + 1)+2);
	}
	//omp_set_num_threads(threads);
	

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

		//24 characters per row (including \n), compute how many rows.
		 fseek(file, 0L, SEEK_END);
    	          long int  nChars = ftell(file); 
  		 size_t    nRows   = nChars/24; 
		
	
		int maxChunkSize = 425/threads+1;      
		int nChunks 	 = nRows/maxChunkSize +1;
		int threadLoad	 = nRows/threads +1;  //divide the workload evenly
		
		fseek(file, 0L, SEEK_END);
		
		float i,j,k; 

		long int index;
		float *x = (float*)malloc(sizeof(float)*maxChunkSize);
		float *y = (float*)malloc(sizeof(float)*maxChunkSize);
		float *z = (float*)malloc(sizeof(float)*maxChunkSize);
		
		
		for (int iChunk = 0; iChunk < nChunks; iChunk++) {
		
		
		
		
		//allocate a sinle block of data from the threadLoad, not exceeding maxChunkSize
			index = iChunk*maxChunkSize;
			
			while (fscanf(file, "%f %f %f", &i,&j,&k) == 3 && index < (iChunk+1)*maxChunkSize){
				x[index] = i;
				y[index] = j;
				z[index] = k;
           			index++;
			}

   		
			processBlock(x,y,z,maxChunkSize);
	
		
		}
	free(x);
	free(y);
	free(z);

	//Close file
	
	fclose(file);

    //Print results:
    for(size_t i = 0; i<nDistances; i++){
    	printf("%05.2f %ld\n", (double)i/100,count[i]);
    }


	return 0;
}




