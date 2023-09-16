#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void row_sums(double * sums, const double ** matrix, size_t nrs, size_t ncs)
{

  for ( size_t ix = 0; ix < nrs; ++ix ) {
    double sum = 0.;
    for ( size_t jx = 0; jx < ncs; ++jx ){
      sum += matrix[ix][jx];
    }
    sums[ix] = sum;
  } 
}

void col_sums(double * sums, const double ** matrix, size_t nrs, size_t ncs)
{
  for ( size_t ix = 0; ix < nrs; ++ix ) {
    double sum = 0.;
    for ( size_t jx = 0; jx < ncs; jx ++ )
    {
      sum += matrix[jx][ix]
    }
    sums[ix] = sum;
  }
}

int main()
{
  #define SIZE 1000
  double sums[SIZE];
  size_t iter = 5000;

  double *matrixEntries = (double*)malloc(sizeof(double)*SIZE*SIZE); //Avoid memory fragmentation
  double **matrix = (double**)malloc(sizeof(double*)*SIZE);
  for (size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx += SIZE)
    matrix[ix] = matrixEntries + jx;

  for (size_t i = 0; i < SIZE; ++i)
  {
    for (size_t j = 0; j < SIZE; ++j)
    {
      matrix[i][j] = 1;
    }
  }
  
  clock_t begin = clock();
  for (int i = 0; i < iter; ++i){
  	row_sums(sums, (const double**)matrix, SIZE,SIZE);
  }
  clock_t end = clock();

  double time_spent = (double)(end - begin)/ CLOCKS_PER_SEC;
  printf("Row sum:\n");
  printf("Elapsed time of %d iterations: %f s. Average time for one loop: %f\n", iter, time_spent, time_spent/iter);
  printf("Random element: %f\n\n", sums[0]);



  clock_t begin_c = clock();
  for (int i = 0; i < iter; ++i)
  {
    col_sums(sums, (const double**)matrix, SIZE,SIZE);
  }
  clock_t end_c = clock();

  double time_spent_c = (double)(end_c - begin_c)/ CLOCKS_PER_SEC;
  printf("Col sum: \n");
  printf("Elapsed time of %d iterations: %f s. Average time for one loop: %f\n", iter, time_spent_c, time_spent_c/iter);
  printf("Random element of sums: %f\n", sums[0]);

  free(matrix);
  free(matrixEntries);
}
