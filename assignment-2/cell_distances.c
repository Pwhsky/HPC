#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include <math.h>




//Program cannot use more than 5Mb of memory, i.e 5*1024*1024
//Maximal theoretical entries = (5*1024*1024)/(bytes of a certain datatype)


const int nDistances = 3465;
static size_t count[3465]; //The number of unique distances
const int blockSize = 425;

static inline void getDistances(float xVal,float yVal, float zVal, float *x, float* y, float* z){
    extern  size_t count[];
    float innerProduct;
    int   distance;
    
	for(int  i = 0; i<(blockSize);i++){
	            innerProduct = sqrtf((xVal-x[i])*(xVal-x[i]) +
	            			 (yVal-y[i])*(yVal-y[i]) +
	            			 (zVal-z[i])*(zVal-z[i]));
	        
	            distance = (int)(100*sqrtf(innerProduct));
	            count[distance] += 1;
        //add innerproduct to list of distances
	    	
	}

}



int main (int argc, char** argv) {
clock_t begin = clock();


//parse arguments
	size_t threads = 2;
	char arg[10];
	for (size_t i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
    	  		memset(arg, 0, sizeof(arg));
     		 	memcpy(arg, &argv[i][2], strlen(argv[i]) - 2);
     		 	
   	  		if (argv[i][1] == 't') {
   	     			threads = (size_t) strtol(arg, NULL, 10);
   	   		}

   		 }	
  	}

	

//open file:
		//char cwd[PATH_MAX];
		//char filename[] = "/cells";
		//getcwd(cwd,sizeof(cwd));
		//strcat(cwd,filename);
		
		FILE *file = fopen("cells","r");
		if (file == NULL){
			printf("Error: failed to open file \n");
			return 1;
		}
		
//24 characters per row (including \n), compute how many rows.

		 fseek(file, 0L, SEEK_END);
    	         long int  nChars = ftell(file); 
  		 int    nRows   = nChars/24; 
		 rewind(file);
	
		int maxChunkSize = 425;      
		int nChunks 	 = nRows/(maxChunkSize)+1; //number of chunks per thread
		
		
		//THREAD INSTRUCTION:////////////////////
		float i,j,k; 
		float xVal,yVal,zVal;
		
		int index;
		float *x = (float*)malloc(sizeof(float)*maxChunkSize);
		float *y = (float*)malloc(sizeof(float)*maxChunkSize);
		float *z = (float*)malloc(sizeof(float)*maxChunkSize);

		for (int point = 0; point < nRows; point++) {
			//Get point in list to compute distances from
			
    			 if (fscanf(file, "%f %f %f", &xVal, &yVal, &zVal) != 3) {
  				return 1;
			  }
			omp_set_num_threads(threads);
  		#pragma omp parallel for 
  		
      			for (int iChunk = 0; iChunk < nChunks; iChunk++){
				//Iterate through chunks using xVal yVal zVal as point of comparison,
				//No more than 425 floats per array!
				
				//place pointer at correct position in the list based on the chunk
				
				fseek(file, ( point*nChunks + iChunk)*3*sizeof(float)*maxChunkSize,SEEK_SET );
			
				for (int i = 0; i < maxChunkSize; i++) {
            				fscanf(file, "%f %f %f", &x[i], &y[i], &z[i]);
           			}
				
       				getDistances(xVal, yVal, zVal, x, y, z);
       			}
       		
       			
       			 
			//Move filepointer to the next point to compare (fscanf moves it forward)
			fseek(file,  3 * (sizeof(float) + 1)*point ,SEEK_SET );
			
		}


		
		
		
			//Compare xVal yVal zVal to all other points in the dataset by loading in chunks of 425 floats untill the end of the file.
			
			//dataset needs to be loaded in blocks starting at the second row, then 3th etc 

		free(x);
		free(y);
		free(z);
		////////////////////////////////////////
		
		
	//Close file
	fclose(file);

    //Print results:
    for(size_t i = 0; i<nDistances; i++){
    
    	printf("%05.2f %ld\n", (double)i/100,count[i]);
    }

	clock_t end = clock();
   	double elapsed_time = (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf("Elapsed time: %lf \n",elapsed_time);
	return 0;
}




