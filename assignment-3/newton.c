#include <stdbool.h>
#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

int lines,degree;
int nCalThreads;
//Computational parameters
	const size_t maxSteps = 128;
	const float tolerance =1e-3 ;
	const float toleranceSquared = tolerance*tolerance;    
	const float maxValue   = 10000000000;
        int*   attractor;
	int*   convergence;
        int**  attractors;
	int**  convergences;

float complex * rootEntries;

float complex ** roots;
static inline void step(float complex *z);

//parsing, and initialization of color lookup tables for a computed attractor
	void parseArgs(char** argv);
	static inline void initRoots();
	void initGrayscale();
	void initColorScale();
	float offsetParam;			    //pre-computed and assigned a value in parseArgs().
//Writing parameters

	int header_size;			    //will vary depending on how many chars in lines argument
	char* convergenceFilename;
	char* attractorFilename;
	const size_t nmbgrays = maxSteps +1;	    //total no. of gray colors, 129 in our case
	char grays[129][12];			
	char colors[10][12]; 			    //10 roots, 10 colors


//Declare mutexes and an array to signal when a row is completed:
	mtx_t completedRowMtx;
	mtx_t mutex;
	mtx_t writingRowMtx;
	cnd_t rowCompleteCond;
	int* completedRows;

static inline int checkAttractor(complex float *z){
         //loop over the list of attractors for the given degree
          int attractorType = -1; 
          for (size_t i = 0; i < degree; i++){
  
                 const float complex  difference =  *z - roots[degree-1][i];
   
                  if( cimag(difference)*cimag(difference)+ creal(difference)*creal(difference) < toleranceSquared){
                          attractorType = i;
                          break;
                  }
          }
          return attractorType;
}

void writeRowToFile(int index,FILE* fpConv,FILE* fpAttr){
	char rowToWriteAttr[lines*12];
	char rowToWriteConv[lines*12];
		
        int* attractor = attractors[index];
        int* convergence = convergences[index];

	for (int i = 0; i < lines; i++){
		memcpy(&rowToWriteAttr[i*12], colors[attractor[i]], 12);
		memcpy(&rowToWriteConv[i*12], grays[convergence[i]], 12);
	}

	fseek(fpConv,header_size + index*lines*12,SEEK_SET);
	fseek(fpAttr,header_size + index*lines*12,SEEK_SET);
	
	fwrite(rowToWriteAttr,1,lines*12,fpAttr);
	fwrite(rowToWriteConv,1, lines*12, fpConv);

	free(attractor);
	free(convergence);
}

//This will be the main function for threads
static inline int newtonRhapson(void* arg){
	size_t threadNum = *((size_t*)arg);
	
	for(size_t ix = threadNum; ix < lines; ix += nCalThreads){ 
		const float complex imPart = (-2.0 + ix*offsetParam)*I;

                int *attractor = malloc(sizeof(int)*lines);
                int *convergence = malloc(sizeof(int)*lines);
                attractors[ix] = attractor;
                convergences[ix] = convergence;

		for(size_t jx = 0; jx < lines; jx+=1){
			float complex z = imPart + (jx*offsetParam-2.0);
			int attractorType = -1;
			size_t iter  	  =  0;
			if( cimag(z)*cimag(z) + creal(z)*creal(z) > tolerance ){
				for (iter = 0; iter < maxSteps && attractorType == -1; iter++){    
					step(&z);
					attractorType = checkAttractor(&z);
				}
			}else{
				attractorType = -1;
				iter = 0;
			}
			mtx_lock(&mutex);
				attractor[jx]   = attractorType+1;
				convergence[jx] = iter;
			mtx_unlock(&mutex);
		} 

		mtx_lock(&completedRowMtx);
		completedRows[ix] = 1;
		mtx_unlock(&completedRowMtx);

		cnd_signal(&rowCompleteCond);
	}


	return thrd_success;
}

int  writingThread(void* arg){
	//Write attractors and convergences to .ppm file 
	//Create files 
	char* convergenceFilename = (char*)malloc(27);
	char* attractorFilename   = (char*)malloc(25);	
	sprintf(convergenceFilename,"newton_convergence_x%d.ppm",degree);
	sprintf(attractorFilename,  "newton_attractors_x%d.ppm",degree);

	FILE *fpConv;
	FILE *fpAttr;	
	fpConv = fopen(convergenceFilename,"w");
	fpAttr = fopen(attractorFilename,"w");

	//Write headers
	char convHeader[32], attrHeader[32];
	if (lines >= 100000)
		thrd_exit(1);
	sprintf(convHeader,"P3\n%d %d\n65 \n", lines,lines);
	sprintf(attrHeader,"P3\n%d %d\n255\n", lines,lines);
	fprintf(fpConv, convHeader);
	fprintf(fpAttr, attrHeader);

	header_size = strlen(convHeader);
	if ( header_size != strlen(attrHeader) )
		thrd_exit(1);
	//Prepare a row for writing:	
	for(int row = 0; row < lines;) {
		mtx_lock(&completedRowMtx);	
		while(!completedRows[row]){
			nanosleep((const struct timespec[]){{0, 1000L}}, NULL);
			cnd_wait(&rowCompleteCond,&completedRowMtx);
		} 

		int nextRow;
		for ( nextRow = row + 1; completedRows[nextRow]; nextRow++ );
		mtx_unlock(&completedRowMtx);
		// check further entries to get a "block" of lines to write
		for ( int futureRows = row; futureRows < nextRow; ++futureRows )
			writeRowToFile(futureRows,fpConv,fpAttr);
		row = nextRow;
	}
	fclose(fpConv);
	fclose(fpAttr);
	free(convergenceFilename);
	free(attractorFilename);	
	return thrd_success;
}
void multithread_newtonRhapson(){

	thrd_t threads[nCalThreads+1];
	size_t threadNums[nCalThreads+1];
	mtx_init(&mutex,mtx_plain);
	mtx_init(&completedRowMtx,mtx_plain);
	mtx_init(&writingRowMtx,mtx_plain);
	cnd_init(&rowCompleteCond);
	//Create and prepare threads
	//Thread 0 will always be writing thread.

	for(size_t i = 0 ; i<nCalThreads; i++){
		threadNums[i] = i;
		thrd_create(&threads[i],newtonRhapson, &threadNums[i]);
	}
	thrd_create(&threads[nCalThreads],writingThread,&threadNums[nCalThreads]);

	//initiate threads
	for(size_t i = 0; i < nCalThreads+1; i++){
		thrd_join(threads[i],NULL);
	}
	mtx_destroy(&mutex);
	mtx_destroy(&completedRowMtx);
	cnd_destroy(&rowCompleteCond);
}



int main(int argc, char** argv) {
        if(argc != 4){
                printf("Error: arguments != 4, exiting.\n Correct example: ./main -t5 -l100 5 \n \n \n");
                return 1;
        }
        initColorScale();
        initGrayscale();
        initRoots();
        parseArgs(argv);

        attractors    = (int**)  malloc(sizeof(int*)*lines);
        convergences  = (int**)  malloc(sizeof(int*)*lines);
	completedRows = (int*)calloc(lines,sizeof(int));
	multithread_newtonRhapson();	

	free(convergence);
	free(attractor);
	free(attractors);
	free(convergences);
	free(completedRows);
        return 0;
}
static inline void step(float complex *z){
//Adjust algebraically the expressions:
//       compute the type of rhapson step based on degree
        switch (degree){
                case 1:
                        *z = -1;
                        break;
                case 2:
                        (*z) -= ( (*z)*(*z)-1)/(2*(*z));
                        break;
                case 3:
                         (*z)-= ( (*z)* (*z) * (*z)-1)/(3* (*z)* (*z));
                        break;
                case 4:
                        *z -= ( (*z)* (*z)* (*z)* (*z)-1/(4* (*z)* (*z)* (*z)));
                        break;
                case 5:
                        *z -= ( (*z)* (*z)* (*z)* (*z)* (*z)-1)/(5* (*z)* (*z)* (*z)*(*z));
                        break;
                case 6:
                        *z -= ( (*z)* (*z)* (*z)* (*z)* (*z)* (*z)-1)/(6* (*z)* (*z)* (*z)* (*z)* (*z));
                        break;
                case 7:
                        *z -= ( (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)-1)/(7* (*z)* (*z)* (*z)* (*z)* (*z)* (*z));
                        break;
                case 8:
                        *z -= ( (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)-1)/(8* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z));
                        break;
                case 9:
                        *z-= ( (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)-1)/(9* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z));
                        break;
                default:
                        printf("Please select degree up to 9\n");
                        exit(1);
        }
}

static inline void initRoots(){
	rootEntries =(float complex*)malloc(sizeof(double complex)*9*9);
        roots = (float complex**)malloc(sizeof(float complex*)*9);
        for (size_t i = 0,j=0; i <9 ; i++,j+= 9){
                roots[i] = rootEntries + j;
        }

        roots[0][0] =  1.0 + 0.0* I;

        roots[1][0] =  1.0 + 0.0* I;
        roots[1][1] = -1.0 + 0.0* I;

        roots[2][0] =  1.0 + 0.0* I;
        roots[2][1] = -0.5 + 0.86603 * I;
        roots[2][2] = -0.5 - 0.86603 * I;

        roots[3][0] =  1.0  ;
        roots[3][1] =  1.0*I;
        roots[3][2] = -1.0 ;
        roots[3][3] = -1.0*I;

        roots[4][0] =  1.0;
        roots[4][1] =  0.309017 + 0.951057 * I;
        roots[4][2] = -0.809017 + 0.587785 * I;
        roots[4][3] = -0.809017 - 0.587785 * I;
        roots[4][4] =  0.309017 - 0.951057 * I;

        roots[5][0] =   1.0;
        roots[5][1] =   0.5 + 0.866025 * I;
        roots[5][2] =  -0.5 + 0.866025 * I;
        roots[5][3] =  -1.0;
        roots[5][4] =  -0.5 - 0.866025 * I;
        roots[5][5] =   0.5 - 0.866025 * I;

        roots[6][0] =   1.0 + 0.0*I;
        roots[6][1] =   0.62349  + 0.781831 * I;
        roots[6][2] =  -0.222521 + 0.974928 * I;
        roots[6][3] =  -0.900969 + 0.433884 * I;
        roots[6][4] =  -0.900969 - 0.433884 * I;
        roots[6][5] =  -0.222521 - 0.974928 * I;
        roots[6][6] =   0.62349  - 0.781831 * I;

         roots[7][0] =  1        +       0 * I;
         roots[7][1] =  0.707107 + 0.707107 * I;
         roots[7][2] =  0        + 1 * I;
         roots[7][3] = -0.707107 + 0.707107 * I;
         roots[7][4] = -1        + 0 * I;
         roots[7][5] = -0.707107 - 0.707107 * I;
         roots[7][6] =  0        - 1 * I;
         roots[7][7] =  0.707107 - 0.707107 * I;

         roots[8][0] =  1.0      + 0 * I;
         roots[8][1] =  0.766044 + 0.642788 * I;
         roots[8][2] =  0.173648 + 0.984808 * I;
         roots[8][3] = -0.5      + 0.866025 * I;
         roots[8][4] = -0.939693 + 0.34202  * I;
         roots[8][5] = -0.939693 - 0.34202  * I;
         roots[8][6] = -0.5      - 0.866025 * I;
         roots[8][7] =  0.173648 - 0.984808 * I;
         roots[8][8] =  0.766044 - 0.642788 * I;

}
void parseArgs(char** argv){
        char* arg1   = argv[1];
        char* arg2   = argv[2];
        char  arg3   = argv[3][0]; //Not including [0] gives a warning
        char  char1  = arg1[1];
        char  char2  = arg2[1];

        degree = (**(argv+3))-'0';

        //Parse args ./newton -t5 -l1000 7 in any arbitrary order
	if (char1 == 't' && char2 == 'l')  {
		nCalThreads = atoi(arg1+2);
		lines = atof(arg2+2);

	} else if (char1 == 'l' && char2 == 't'){
		nCalThreads = atoi(arg2+2);
		lines = atof(arg1+2);
	}
	offsetParam = 4.0/lines;
}

void initColorScale(){
	
	memcpy(colors[0], "255 000 000\n",12);
	memcpy(colors[1], "000 255 000\n",12);
	memcpy(colors[2], "000 000 255\n",12);
	memcpy(colors[3], "255 128 000\n",12);
	memcpy(colors[4], "255 255 000\n",12);
	memcpy(colors[5], "000 255 255\n",12);
	memcpy(colors[6], "255 000 255\n",12);
	memcpy(colors[7], "255 000 128\n",12);
	memcpy(colors[8], "153 000 153\n",12);
	memcpy(colors[9], "000 000 100\n",12);
}


void initGrayscale(){
	char this_gray[13];
	for ( int ix = 0; ix < nmbgrays; ++ix ) {
		sprintf(this_gray, "%03d %03d %03d\n", ix, ix, ix);
		memcpy(grays[ix], this_gray, 12);
	}
}


/* Old firewriting function
void writeToFile(){
	//Write attractors and convergences to .ppm file 
        //Create files 
	char* convergenceFilename = (char*)malloc(27);
	char* attractorFilename   = (char*)malloc(25);	
        sprintf(convergenceFilename,"newton_convergences_x%d.ppm",degree);
	sprintf(attractorFilename,  "newton_attractors_x%d.ppm",degree);
	
	FILE *fpConv;
	FILE *fpAttr;	
 	fpConv = fopen(convergenceFilename,"wb");
	fpAttr = fopen(attractorFilename,"wb");
	//Write headers
	fprintf(fpConv,"P3\n%d %d\n65\n",lines,lines);
	fprintf(fpAttr,"P3\n%d %d\n255\n",lines,lines);
		
	//Use 1 thread to write first convergence then attractors to file

	
//Prepare a row for writing:	
	char* rowToWriteAttr = (char*)malloc(sizeof(char)*lines*12);
	char* rowToWriteConv = (char*)malloc(sizeof(char)*lines*12);
	
	for(size_t i = 0; i< lines; i++){
		for (size_t j = 0; j< lines; j++){
			const int colorindex = attractors[i][j];
			const char* gravy = grays[(convergences[i][j])];
			
			memcpy(&rowToWriteAttr[j*12],colors[colorindex],12);
			memcpy(&rowToWriteConv[j*12],gravy,12);
		}
		fwrite(rowToWriteAttr,1,lines*12,fpAttr);
		fwrite(rowToWriteConv,1, lines*12, fpConv);
	}	


	free(rowToWriteAttr);
	free(rowToWriteConv);
	free(convergenceFilename);
	free(attractorFilename);

}*/
