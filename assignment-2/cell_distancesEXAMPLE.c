#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <math.h>

static short colSize = 3;
static size_t count[3465]; //Maximal number of distances

static inline void distancesWithinBlock(size_t currentBlockLoad, float** block)
{   
    extern size_t count[];
    float currentCoordinates[colSize], innerProduct;
    int totalDistance;
    size_t i,j;
    #pragma omp parallel for private(i,j,innerProduct, totalDistance,currentCoordinates) shared(block,currentBlockLoad) reduction(+:count)
    for(i = 0; i < currentBlockLoad - 1; ++i) //currentBlockLoad = number of rows in the block
    {
        currentCoordinates[0] = block[i][0];
        currentCoordinates[1] = block[i][1];
        currentCoordinates[2] = block[i][2];
        for(j = i + 1; j < currentBlockLoad; ++j)
        {
            innerProduct = (currentCoordinates[0] - block[j][0])*(currentCoordinates[0] - block[j][0]) +
            (currentCoordinates[1] - block[j][1]) * (currentCoordinates[1] - block[j][1]) +
            (currentCoordinates[2] - block[j][2]) * (currentCoordinates[2] - block[j][2]);
            totalDistance = (int)(100*sqrtf(innerProduct));
            
            count[totalDistance] += 1;
            
        }
    }
}

static inline void distancesBetweenBlocks(size_t currentBlockLoad, float** block,float previousBlockValues[colSize])
{
    extern size_t count[];
    float innerProduct;
    int totalDistance;
    size_t i;
    #pragma omp parallel for private(i,innerProduct,totalDistance) shared(block,currentBlockLoad, previousBlockValues) reduction(+:count)
    for(i = 0; i < currentBlockLoad; ++i)
    {
        innerProduct = (previousBlockValues[0] - block[i][0]) * (previousBlockValues[0] - block[i][0]) +
        (previousBlockValues[1] - block[i][1]) * (previousBlockValues[1] - block[i][1]) +
        (previousBlockValues[2] - block[i][2]) * (previousBlockValues[2] - block[i][2]);

        totalDistance = (int)(sqrtf(innerProduct)*100);
        count[totalDistance] += 1;
    }
}

//Maximal theoretical rows = (5*1024*1024)/(bytes of a certain datatype)
int main(int argc, char *argv[])
{
    char int_string[strlen(argv[1]) - 2];
    for (int i = 2; i < strlen(argv[1]); ++i){
      int_string[i-2] = argv[1][i];
    }
    int numThreads = atoi(int_string);
    
    omp_set_num_threads(numThreads);
    
    FILE *file;
    file = fopen("cells","r");
    fseek(file, 0L, SEEK_END);
    long int res = ftell(file); 
    size_t rowCount = res/24; 
    size_t maximumRowsLoad = 100000; 
    size_t numOfBlocksNeeded; 

    if(rowCount % maximumRowsLoad) //Number of blocks needed. Tells whether the last contains fewer rows than the rest, and if it is it adds an additional block
    {
        numOfBlocksNeeded = rowCount/maximumRowsLoad + 1;
    }
    else
    {
        numOfBlocksNeeded = rowCount/maximumRowsLoad;
    }
    
    size_t maximumRowsInBlock = rowCount;
    size_t numOfRowsInBlock[numOfBlocksNeeded], firstBlock[numOfBlocksNeeded]; //Determines number of rows each block receives
    for(size_t i = 0; i < numOfBlocksNeeded; ++i)
    {
        if(maximumRowsInBlock >= maximumRowsLoad)
        {
            numOfRowsInBlock[i] = maximumRowsLoad;
            maximumRowsInBlock -= maximumRowsLoad;
        }
        else
        {
            numOfRowsInBlock[i] = maximumRowsInBlock;
        }
        firstBlock[i] = i * maximumRowsLoad;
    }
    
    for(size_t i = 0; i < numOfBlocksNeeded; ++i) //Start of calculating all blocks
    {
        size_t currentBlockLoad = numOfRowsInBlock[i]; 
        float *blockEntries = (float*)malloc(sizeof(float) * currentBlockLoad * colSize);
        float **block = (float**)malloc(sizeof(float*) * currentBlockLoad);

        
        for(size_t j = 0; j < currentBlockLoad; j++)
        {
            block[j] = blockEntries + j * colSize;
        }

        fseek(file, firstBlock[i] * 24L, SEEK_SET);
        size_t ix;
        char stringLine[25]; 
        #pragma omp parallel for private(ix, stringLine) shared(block,currentBlockLoad)
        for(ix = 0; ix < currentBlockLoad; ++ix)
        {
            fgets(stringLine, 25, file);
            block[ix][0] = atof(stringLine);
            block[ix][1] = atof(stringLine + 8);
            block[ix][2] = atof(stringLine + 16);
            
        }
        
        //Calculate distances within a block
        distancesWithinBlock(currentBlockLoad, block);

        if(i > 0)
        {
            size_t previousBlock = i;
            for(size_t ix = 0; ix < previousBlock; ++ix)
            {
                size_t previousBlockStart = firstBlock[ix];
                size_t previousBlockRow = numOfRowsInBlock[ix];
                fseek(file, previousBlockStart * 24L, SEEK_SET);
                float previousBlockValues[colSize];
                char stringLine[25];
                for(size_t jx = 0; jx < previousBlockRow; ++jx)
                {
                    fgets(stringLine, 25, file);
                    previousBlockValues[0] = atof(stringLine);
                    previousBlockValues[1] = atof(stringLine + 8);
                    previousBlockValues[2] = atof(stringLine + 16);
                    
                    //Here we calculate the distance between block    
                    distancesBetweenBlocks(currentBlockLoad, block, previousBlockValues);
                }
            }
        }

        free(blockEntries);
        free(block);
    } //End of calculating all blocks
    fclose(file);

    size_t i;
    for(i = 0; i < 3465; ++i)
    {   
        printf("%05.2f %ld\n", (double)i/100, count[i]);
    }
    return 0;

}
