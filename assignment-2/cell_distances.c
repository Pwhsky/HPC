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
const size_t maxChunkSize = 425;


static inline void getLocalDistances(size_t blockSize, float**chunk ){
    extern  size_t count[];
    float  distance,x,y,z;
    size_t i,j;
	#pragma omp parallel for shared(chunk,pointIndex) private(i,j,distance,x,y,z) reduction(+:count)
	for(i = 0; i<blockSize-1;i++){
		x = chunk[i][0];
		y = chunk[i][1];
		z = chunk[i][2];
		for(j = i + 1; j<blockSize; j++){
		
		distance = sqrtf((x-chunk[j][0])*(x-chunk[j][0])+
				 (y-chunk[j][1])*(y-chunk[j][1])+
				 (z-chunk[j][2])*(z-chunk[j][2]));
				 
		//truncate by casting to int.
	        count[(int)(100*distance)] += 1;
		}
	    	
	}
}






int main (int argc, char** argv) {
clock_t begin = clock();
extern  size_t count[];

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
	omp_set_num_threads(threads);

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
    	long int nChars  = ftell(file); 
    	rewind(file);
    	size_t nRows   = nChars/24; 
    	const size_t maxRows = 100000;
  	
	size_t nChunks; //number of total chunks to be processed
	size_t maxRowsPerChunk = nRows;	
	if(nRows % 100000){
		nChunks = nRows/maxRows +1;
	}else{
		nChunks = nRows/maxRows;
	}

	
	size_t nRowsPerChunk[nChunks]; //CONTINUE FROM HERE, WIP
	
	size_t firstChunk[nChunkS]; 
	
	for(size_t i = 0; i < nChunks; ++i){
       		if(maxRowsPerChunk >= maxRows){
            		nRowsPerChunk[i] = maxRows;
            		maxRowsPerChunk -= maxRows;
      		}else{
           		nRowsPerChunk[i] = maxRowsPerChunk;
       		}
       		firstBlock[i] = i * maximumRowsLoad;
   	}
	
	for(size_t i = 0; i< nChunks; i++){
		//Allocate entries
		size_t chunkIndex = nRowsPerChunk[i];
		
		float** chunk     =  (float**)malloc(sizeof(float*) * chunkIndex)
		
		
	
	
	
	}
	
	
	
		
	float ** pointsToCompare = (float**) malloc(sizeof(float*)*maxChunkSize);
	#pragma omp parallel for private(ix) shared(pointsToCompare)
	for(size_t ix = 0; ix < maxChunkSize; ix++) {
		pointsToCompare[ix] = (float*)malloc(sizeof(float)*3); //3 float coordinates per entry
	}
		
	float ** comparisonPoints = (float**) malloc(sizeof(float*)*maxChunkSize);
	#pragma omp parallel for private(ix) shared(comparisonPoints)
	for(size_t ix = 0; ix < maxChunkSize; ix++) {
		comparisonPoints[ix] = (float*)malloc(sizeof(float)*3); //3 float coordinates per entry
	}
		
		
		//Load a chunk of points to process:

	int distance;
      	float product;
      	float xPrim,yPrim,zPrim;
      	size_t point;
      	char stringLine[25]; 
      	#pragma omp parallel for private(point) reduction(+:count)
	for (point = 0; point < nChunks; point++) {
	
		
		fseek(file, point*maxChunkSize*24L, SEEK_SET);
    		
      		
        	#pragma omp parallel for private(ix, stringLine) shared(pointsToCompare)
       		for(ix = 0; ix < maxChunkSize; ++ix){
           		fgets(stringLine, 25, file);
           		pointsToCompare[ix][0] = atof(stringLine);
          		pointsToCompare[ix][1] = atof(stringLine + 8);
          		pointsToCompare[ix][2] = atof(stringLine + 16);
      		}
      		
      		
      		//for point = 0...n
      		
      		//Process the chunk by first populating a comparisonPoints :
      		fseek(file, (point)*maxChunkSize*24L, SEEK_SET);
      		
      		  #pragma omp parallel for private(ix, stringLine) shared(comparisonPoints)
      		for (ix = 0 ; ix < maxChunkSize; ix++) {
           		fgets(stringLine, 25, file);
           		comparisonPoints[ix][0] = atof(stringLine);
          		comparisonPoints[ix][1] = atof(stringLine + 8);
          		comparisonPoints[ix][2] = atof(stringLine + 16);
			
      		}
      	
      	
      		for (ix = 0; ix < maxChunkSize; ix++){
      			//Get point:
      			float x = pointsToCompare[ix][0];
      			float y = pointsToCompare[ix][1];
      			float z = pointsToCompare[ix][2];
      			
      			//iterate through the chunk with the given point and compute euclidean distance
      			
      			for (size_t i = 0; i<maxChunkSize;i++){
      				xPrim = comparisonPoints[i][0];
      				yPrim = comparisonPoints[i][1];
      				zPrim = comparisonPoints[i][2];
      			
      				product = sqrtf((x-xPrim)*(x-xPrim) +
	            				(y-yPrim)*(y-yPrim) +
	            				(z-zPrim)*(z-zPrim));
	            				 
	  			 distance = (int)(100*product);
	  			
	          		 count[distance] += 1;
      			  
      			}
      		
      		}
      	}
      		
      	

       		
      		 
      		
      		
        
			
			
		
		
	
	

		/*for (long int point = 0; point < nRows; point++) {
			//Get point in list to compute distances from
			
			fseek(file,  (24)*point ,SEEK_SET );	
    			fscanf(file, "%f %f %f", &xVal, &yVal, &zVal); // this is slow because of file reading
    			
    			
    			#pragma omp parallel for shared(iChunk,point)
      			for (iChunk = 0; iChunk < nChunks; iChunk++){
      				long int offset = (point+1)*24*iChunk*maxChunkSize;
				//Iterate through chunks using xVal yVal zVal as point of comparison,
				//No more than 425 floats per array!
				//place pointer at correct position in the list based on the chunk
				fseek(file,offset,SEEK_SET );
				for (int i = 0; i < maxChunkSize; i++) {
            				fscanf(file, "%f %f %f", &x[i], &y[i], &z[i]);
           			}
           			
       				getDistances(xVal, yVal, zVal, x, y, z);
       				
       			}      			
       			
			//Move filepointer back (fscanf moves it forward)
			fseek(file,  (24 + 1)*point ,SEEK_SET );
		}*/

		
		
		
			//Compare xVal yVal zVal to all other points in the dataset by loading in chunks of 425 floats untill the end of the file.
			
			//dataset needs to be loaded in blocks starting at the second row, then 3th etc 


		////////////////////////////////////////
		
		
	//Close file
	fclose(file);

    //Print results:
    for(size_t i = 0; i<nDistances; i++){
    
      printf("%05.2f %ld\n", (double)i/100,count[i]);
    }

	clock_t end = clock();
   	double elapsed_time = (double)(end - begin) / (CLOCKS_PER_SEC*threads);
	
	printf("Elapsed time: %lf \n",elapsed_time);
	return 0;
}




