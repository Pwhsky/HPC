#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
Implement a program that

in a loop repeats,
the naive computation of the sum of the first billion integers,
*writes the result to stdout,
*takes the time before and after the loop,
*computes the runtime of a single loop iteration from the        difference of start and stop time.

Compile and run the program with all possible optimization levels (add the flags -O0, -O1, -O2, -O3, -Os, and -Og to the compiler).

Discuss the timing results and speculate about the possible cause.

*/


int main(){
   int size = 100000000;
   long long sum = 0;

   clock_t start_loop = clock();
   
   for (size_t i = 0; i < size+1; i++) {
         	 sum += i;	
   }
   
   clock_t end_loop = clock();
   
   double total_time = (double)(end_loop-start_loop)/CLOCKS_PER_SEC;
   double time_per_loop = total_time/(double)size;
   
   printf("Total sum = %lld\n",sum);
   printf("Total time = %.13f seconds\n",total_time);
   printf("Time per iteration = %.13f seconds \n", time_per_loop);

   return 0;
}

//Note: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html for a list of all optimization features.
// -Os optimizes for size, -Og optimizes debugging experience
