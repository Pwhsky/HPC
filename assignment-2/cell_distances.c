#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>
#include <math.h>




//Program cannot use more than 5Mb of memory, i.e 5*1024*1024
//Maximal theoretical entries = (5*1024*1024)/(bytes of a certain datatype)


const int nDistances = 3465;
static size_t count[3465]; //The number of unique distances
const size_t maxChunkSize = 425;




static inline void getLocalDistances(size_t chunkSize,float**chunk ){
    extern  size_t count[];
    float  distance,x,y,z;
    size_t i,j;
	#pragma omp parallel for shared(chunk,chunkSize)  private(i,j,distance,x,y,z) reduction(+:count) 
	for(i = 0; i<chunkSize-1;i++){
		x = chunk[i][0];
		y = chunk[i][1];
		z = chunk[i][2];
		for(j = i + 1; j<chunkSize; j++){
		
		distance = sqrt((x-chunk[j][0])*(x-chunk[j][0])+
				 (y-chunk[j][1])*(y-chunk[j][1])+
				 (z-chunk[j][2])*(z-chunk[j][2]));
				 
		//truncate by casting to int.
	        count[(int)(100*distance)] += 1;
		}
	    	
	}
}

static inline void getGlobalDistances(const size_t chunkSize, float** chunk, const float prevChunkEntries[3]){
	extern size_t count[];
	float distance;
	size_t i;
	#pragma omp parallel for private(i,distance) shared(chunk,chunkSize, prevChunkEntries) reduction(+:count)
	for(i=0; i<chunkSize; i++){
		 distance =  sqrtf((prevChunkEntries[0] - chunk[i][0])* (prevChunkEntries[0] - chunk[i][0]) +
		 			  (prevChunkEntries[1] - chunk[i][1])* (prevChunkEntries[1] - chunk[i][1]) + 
		 			  (prevChunkEntries[2] - chunk[i][2])* (prevChunkEntries[2] - chunk[i][2]));
		count[(int)(distance)*100] +=1;
	}
}

static inline float** makeNewChunk(const size_t chunkSize ){

	float** chunk   =  (float**)malloc(sizeof(float*) * chunkSize);
	float*  chunkEntries  =  (float*)malloc(sizeof(float*)  * chunkSize*3);
	size_t j;

		for ( j = 0; j < chunkSize; j++){
			chunk[j]  = chunkEntries + j*3;
		}

	return chunk;
}



int main (int argc, char** argv) {

extern  size_t count[];

//parse arguments
	size_t threads = 2;
	char arg[10];
	for (size_t i = 1; i < argc; i++) {
		if (argv[i] == '-') {
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
		
	FILE *file = fopen("cells5","r");
	if (file == NULL){
		printf("Error: failed to open file \n");
		return 1;
	}
		
	 //24 characters per row (including \n), compute how many rows.

	fseek(file, 0L, SEEK_END);
    	const long int nChars  = ftell(file); 
    	rewind(file);
    	const size_t nRows   = nChars/24; 
    	const size_t maxRows = 100000;
  	
	size_t nChunks; //number of total chunks to be processed
	 size_t maxRowsPerChunk = nRows;	
	if(nRows % 100000){
		nChunks = nRows/maxRows +1;
	}else{
		nChunks = nRows/maxRows;
	}

	
	size_t nRowsPerChunk[nChunks]; 
	
	size_t firstChunk[nChunks]; 
	
	for(size_t i = 0; i < nChunks; ++i){
       		if(maxRowsPerChunk >= maxRows){
            		nRowsPerChunk[i] = maxRows;
            		maxRowsPerChunk -= maxRows;
      		}else{
           		nRowsPerChunk[i] = maxRowsPerChunk;
       		}
       		firstChunk[i] = i * maxRows;
   	}
	
	for (size_t i = 0; i< nChunks; i++){
		//Allocate entries
		const size_t chunkSize= nRowsPerChunk[i];
		
		float **chunk = makeNewChunk(chunkSize);
		
		
		fseek(file, firstChunk[i]*24L,SEEK_SET);
		char stringLine[25];
		
		size_t ix;
		#pragma omp parallel for private(ix,stringLine) shared(chunk, chunkSize)
		for( ix = 0; ix < chunkSize; ix++) {
		
			fgets(stringLine, 25, file);
			chunk[ix][0] = atof(stringLine+0);
			chunk[ix][1] = atof(stringLine+8);
			chunk[ix][2] = atof(stringLine+16);
		}
		getLocalDistances(chunkSize,chunk);
		
		if(i > 0){
			const size_t prevIndex = i; //prevIndex = previous index of chunk
			for(size_t ix = 0; ix < prevIndex; ix++){
			
				const size_t prevIndexStart = firstChunk[ix];
				const size_t prevIndexRow    = nRowsPerChunk[ix];
				fseek(file,prevIndexStart*24L,SEEK_SET);
				float prevChunkEntries[3];
				char stringLine[25];
				
				for(size_t jx = 0; jx < prevIndexRow; jx++){
				
					fgets(stringLine,25,file);
					prevChunkEntries[0] = atof(stringLine+0);
					prevChunkEntries[1] = atof(stringLine+8);
					prevChunkEntries[2] = atof(stringLine+16);
					
				
					getGlobalDistances(chunkSize,chunk,prevChunkEntries);
				
				}
			}
		}

		free(chunk);
	}
	
	
	//Close file
	fclose(file);

    //Print results:
    for(size_t i = 0; i<nDistances; i++){
    
      printf("%05.2f %ld\n", (double)i/100,count[i]);
    }

	
	return 0;
}




