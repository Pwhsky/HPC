#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <getopt.h>

// ----------------------------------------------------
// TMA881 Assignment 2: OpenMP
// Submission by Stanislaw Jan Malec
// ----------------------------------------------------

/*
Main idea: 
	Use two blocks. First block computes distances within itself (parallelized).
	Then slide the 2nd block from the beginning up until before the current block, 
	computing distances between current and 2nd block. 
*/

typedef long long ll;

// loads a block of cells
// ------------ args ------------
// i     : block index
// width : how many cells
// block : pointer to where to store 
ll load_block(FILE* file, ll i, ll width, float *block ) {
	ll counter = 0;
	fseek(file, i*width*3L*8L, SEEK_SET); // beginning of file
	while(fscanf(file, "%f", &block[counter]) != EOF) {
		// printf("read %f \n", block[counter]);
		counter++;
		if (counter >= width*3)
			break;
	}
	return counter/3;
}
// increment frequency of a calculated distance 
static inline void add_count(long i, long j, int* distance_map, float* cell_floats) {
	float startx = cell_floats[i*3];
	float starty = cell_floats[i*3 + 1];
	float startz = cell_floats[i*3 + 2];

	float x = (startx - cell_floats[j*3]);
	float y = (starty - cell_floats[j*3 + 1]);
	float z = (startz - cell_floats[j*3 + 2]);
	float dist = sqrtf(x*x + y*y + z*z);
	int idx = (int)(dist*100.0f + 0.5f);
	// #pragma omp atomic
	distance_map[idx]++;
}
// increment frequency of a calculated distance across two blocks
static inline void add_count_from_2_blocks(long i, long j, int* distance_map, float* current_block, float*prev_block) {
	float startx = current_block[i*3];
	float starty = current_block[i*3 + 1];
	float startz = current_block[i*3 + 2];

	float x = (startx - prev_block[j*3]);
	float y = (starty - prev_block[j*3 + 1]);
	float z = (startz - prev_block[j*3 + 2]);
	float dist = sqrtf(x*x + y*y + z*z);
	int idx = (int)(dist*100.0f + 0.5f);
	// int idx = (int)roundf(dist*100.0f);
	// #pragma omp atomic
	distance_map[idx]++;
}

int main(int argc, char*argv[]) {
	// --- get thread count parameter
	clock_t begin = clock();
	int opt;
	int numthreads = 1;
	while((opt = getopt(argc, argv, "t:")) != -1) {
		switch (opt) {
		case 't':
			int temp = 1;
			temp = atoi(optarg);
			if (temp > 0)
				numthreads = temp;
			else
				printf("ERROR: invalid number of threads=%d provided. Must be positive. Reverting to 1.\n", numthreads);
			break;
		}
	}

	omp_set_num_threads(numthreads);

	// --- 1. read cell coordintes ---
	FILE *file = fopen("cells", "r");

	if(file==NULL) {
	 	printf("ERROR: could not find file cells...\n");
		return -1;
	}
	// find size in bytes of file
	fseek(file, 0L, SEEK_END); // end of file
	ll file_total_bytes = ftell(file);
	fseek(file, 0L, SEEK_SET); // beginning of file
	
	ll num_cells = file_total_bytes/(3L*8L); // total amount of cells
	ll arr_size = file_total_bytes/(8L);     // total amount of numbers

	// block setup
	ll width = 10000L; // load at most 1e4 cells in blocks at a time
	ll total_blocks = num_cells/width + ((num_cells%width) ? 1 : 0); // account for remaining items to fit in a block

	int dist_max_len = 3465; // 3 * (20^2) => 3465 different distance locations [0,...,3464], len=3465
	int* distance_map = (int*) malloc(dist_max_len* sizeof(int));

	// --- 2. compute distances ---
	float * CURRENT_BLOCK = (float*) malloc(width*3 * sizeof(float));
	float * PREVIOUS_BLOCK = (float*) malloc(width*3 * sizeof(float));

	for (long block_i = 0; block_i < total_blocks; block_i++) {
		int cells_read = 0;
		cells_read= load_block(file, block_i, width, CURRENT_BLOCK);

		// compute distances within block first
		#pragma omp parallel for reduction(+:distance_map[:dist_max_len])
		for (long i = 0; i < cells_read; i++) {
			for(long j = i+1; j < cells_read; j++) {
				add_count(i,j,distance_map, CURRENT_BLOCK);
			}
		}
		
		if (block_i == 0) // there are no blocks before the first one
			continue;
		
		// compute distances of current block against all previous blocks
		for(int block_prev = 0; block_prev < block_i; block_prev++){
			
			int prev_cells_read = 0;
			prev_cells_read = load_block(file, block_prev, width, PREVIOUS_BLOCK);

			#pragma omp parallel for reduction(+:distance_map[:dist_max_len])
			for (int current_block_i = 0; current_block_i < cells_read; current_block_i++) {
				for(int prev_block_i = 0; prev_block_i < prev_cells_read; prev_block_i++) {
					add_count_from_2_blocks(current_block_i,prev_block_i, distance_map, CURRENT_BLOCK, PREVIOUS_BLOCK);
				}
			}
		}
	}
	
	// --- 3. print distances
	for(int i = 0; i < dist_max_len; i++) {
		if(distance_map[i])
			printf("%d%d.%d%d %d\n", i/1000, (i%1000)/100,(i%100)/10,(i%10), distance_map[i]);
	}

	// --- 4. cleanup
	free(CURRENT_BLOCK);
	free(PREVIOUS_BLOCK);
	free(distance_map);
	fclose(file);
		clock_t end = clock();
   	double elapsed_time = (double)(end - begin) / (CLOCKS_PER_SEC* numthreads);
	
	printf("Elapsed time: %lf \n",elapsed_time);


	return 0;
}
