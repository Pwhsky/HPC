#include <math.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
double lines;
int threads;
int degree;
const int maxSteps = 128;
const double tolerance = 0.001;
const double toleranceSquared = tolerance*tolerance;    
const double maxValue   = 10000000000;
        int*   attractor;
        int**  attractors;
        int*   convergence;
        int**  convergences; 
complex double ** roots; //Make the roots const complex double for optimization.
//These functions are after main(), as they are ugly.
static inline void step(complex *z);

char* convertToGrayscale(int iter);
    

static inline void initRoots();
void parseArgs(char** argv); 
static inline void checkAttractor(complex double *z){
        //loop over the list of attractors for the given degree
    
        for (int i = 0; i < degree-1; i++){
    
                complex double difference =  *z - roots[degree-1][i];   
    
                if( cimag(difference)*cimag(difference)+ creal(difference)*creal(difference) < toleranceSquared){
                        *attractor = i;
                        break;
                }else {
                continue;
                }
        }    
}

static inline void newtonRhapson(){
        for(int ix = 0, offsetRe = 0; ix < lines; ix+=1,offsetRe += 4){ 
                double realPart = -2.0 + ((double)offsetRe)/(lines); //Optimize to remove division in the future        
                for(int jx = 0, offsetIm = 0; jx < lines; jx+=1, offsetIm +=4){
                        complex double z0 = realPart + ((double)offsetIm/(lines) -2.0)*I;

                        *attractor = -1; 
                        int iter   =  0;  
                        complex double *z = &z0;
                        for (iter = 0; iter<maxSteps && *attractor == -1; iter++){    
                                step(z);
                                checkAttractor(z);
                        }
                        attractors[ix][jx]    = *attractor;
                        convergences[ix][jx]  = iter;
                }    
        }
}

int main(int argc, char** argv) {
        if(argc != 4){
                printf("Error: arguments != 4, exiting.\n Correct example: ./main -t5 -l100 5 \n \n \n");
                return 1;
        }
        parseArgs(argv);
        attractor     = (int*)   malloc(sizeof(int)  *lines*lines);
        attractors    = (int**)  malloc(sizeof(int*) *lines);
        convergence   = (int*)   malloc(sizeof(int)  *lines*lines);
        convergences  = (int**)  malloc(sizeof(int*) *lines);

        initRoots();
        for (size_t ix = 0,jx = 0; ix < lines; ix++, jx+=lines){
                attractors[ix] = attractor +jx;
                convergences[ix] = convergence +jx;
        }
        newtonRhapson();
        //Write attractors and convergences to .ppm file 
        //Creat files  
        //loop through all entries of attractor and convergences.
        //cap convergences values to 50, hard cap.
        convertToGrayscale(128);
        //assign a rgb color to each root of attractors.
        return 0;
}

static inline void step(complex *z){

//       compute the type of rhapson step based on degree
        switch (degree){
                case 1:
                        *z -= (*z)-1;
                        break;
                case 2:
                        (*z) -= ( (*z)*(*z)-1)/(2*(*z));
                        break;
                case 3:
                         (*z) -= ( (*z)* (*z) * (*z)-1)/(3* (*z)* (*z));
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
                        *z -= ( (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)-1)/(9* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z)* (*z));
                        break;
                default:
                        printf("Please select degree up to 9\n");
                        exit(1);
        }
}

static inline void initRoots(){
        roots = (double complex**)malloc(sizeof(double complex*)*9);
        for (int i = 0; i <9 ; i++){
                roots[i] = (double complex*)malloc(sizeof(double complex)*(i+1));
        }
        roots[0][0] =  1.0 + 0.0* I;

        roots[1][0] =  1.0 + 0.0* I;
        roots[1][1] = -1.0 + 0.0* I;

        roots[2][0] =  1.0 + 0.0* I;
        roots[2][1] = -0.5 + 0.86603 * I;
        roots[2][2] = -0.5 - 0.86603 * I;

        roots[3][0] =  1.0 + 0.0* I;
        roots[3][1] =  1.0*I;
        roots[3][2] = -1.0 + 0.0 *I;
        roots[3][3] = -1.0*I;

        roots[4][0] =  1.0 + 0.0*I;
        roots[4][1] =  0.309017 + 0.951057 * I;
        roots[4][2] = -0.809017 + 0.587785 * I;
        roots[4][3] = -0.809017 - 0.587785 * I;
        roots[4][4] =  0.309017 - 0.951057 * I;

        roots[5][0] =   1.0 + 0.0*I;
        roots[5][1] =   0.5 + 0.866025 * I;
        roots[5][2] =  -0.5 + 0.866025 * I;
        roots[5][3] =  -1.0 + 0.0*I;
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
        char  arg3   = argv[3];
        char  char1  = arg1[1];
        char  char2  = arg2[1];

        degree = (**(argv+3))-'0';

        //Parse args ./newton -t5 -l1000 7 in any arbitrary order
        if (char1 == 't' && char2 == 'l')  {
                threads = arg1[2]-'0';
                lines = (double)atoi(arg2+2);

        }else if (char1 == 'l' && char2 == 't'){
                threads = arg2[2]-'0';
                lines =(double) atoi(arg1+2);
        }
}
char* convertToGrayscale(int iter){

        char* grayscale =(char*)malloc(12); //Sizeof(char) = 1

        int rescaled = iter*2; //255/128 roughly equals 2
        if(rescaled == 256){
                rescaled = 255;
        }

        sprintf(grayscale,"%03d %03d %03d",rescaled, rescaled,rescaled);

        return grayscale;
}




