#include "mpi.h"
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int width, height;
int nSteps = 0;
double c = 0.0;

static inline const double getAbs(double number){
	return (number < 0.0 ? -number : number);
}
void parseArgs(int argc, char* argv[]);

int main(int argc, char* argv[]){

	MPI_Init(NULL, NULL);
	int nThreads, mpi_rank;
	MPI_Comm mpiworld = MPI_COMM_WORLD;

	MPI_Comm_size(mpiworld, &nThreads);
	MPI_Comm_rank(mpiworld, &mpi_rank);
	
	parseArgs(argc,argv);	

	char line[80];
	FILE* input;
	if (mpi_rank == 0) {
		input = fopen("init", "r");
		if (input == NULL) {
			perror("Error opening file");
			exit(2);
		}
		fgets(line, sizeof(line), input);
		sscanf(line, "%d %d", &height, &width);
	}
	MPI_Bcast(&width, 1, MPI_INT, 0, mpiworld);
	MPI_Bcast(&height, 1, MPI_INT, 0, mpiworld);

	//Pad width and height to avoid boundary problems
	const int width2 = width + 2;
	const int height2 = height + 2;
	const int nElements = (width2) * (height2);

	const int ix_sub_m = (width - 1) / nThreads + 1;
	int lens[nThreads];
	int poss[nThreads];
	if (mpi_rank == 0) {
		for (int jx = 0, pos = 0; jx < nThreads; ++jx, pos += ix_sub_m) {
			lens[jx] = ix_sub_m < width - pos ? ix_sub_m : width - pos;
			poss[jx] = pos + 1;
		}
	}
	//Bcast(broadcast) declares variables to all processes.
	MPI_Bcast(lens, nThreads, MPI_INT, 0, mpiworld);
	MPI_Bcast(poss, nThreads, MPI_INT, 0, mpiworld);
	const int loclen = lens[mpi_rank];
	const int lastNodeRank = nThreads - 1;
	MPI_Status recvsta;

	double* currentValuesFull;
	double* currentValues;
	double* nextEntries;


	if (mpi_rank == 0) {
		currentValuesFull = (double*)calloc(nElements, sizeof(double));
	}
	currentValues = (double*)aligned_alloc(64, sizeof(double) * (loclen + 2) * height2);
	nextEntries   = (double*)aligned_alloc(64, sizeof(double) * (loclen + 2) * height2);

	// read the first line
	if (mpi_rank == 0) {
		// store initial values (note: row major order)
		int i, j;
		double t;
		while (fgets(line, sizeof(line), input) != NULL) {
			sscanf(line, "%d %d %lf", &j, &i, &t);
			currentValuesFull[(i + 1) * (height + 2) + j + 1] = t;
		}
		fclose(input);

		memcpy(currentValues, currentValuesFull, sizeof(double) * (lens[0] + 2) * height2);

		if (nThreads > 1) {
			for (int ix = 1; ix < nThreads; ix++) {
				MPI_Send(currentValuesFull + (poss[ix] - 1) * height2,
						(lens[ix] + 2) * height2,MPI_DOUBLE,ix,0,mpiworld);
			}
		}
	}
	if (mpi_rank > 0) {
		MPI_Recv(currentValues, (loclen + 2) * height2, MPI_DOUBLE, 0, 0, mpiworld, &recvsta);
	}

	int batchSize = 3000, ib, jb, ie, je;
	double sum = 0.0;
	// compute the temperatures in next time step
	for (int n = 0; n < nSteps; n++) {

		for (ib = 1; ib < loclen + 1; ib += batchSize) {
			ie = ib + batchSize < loclen + 1 ? ib + batchSize : loclen + 1;
			for (jb = 1; jb < height + 1; jb += batchSize) {
				je = jb + batchSize < height + 1 ? jb + batchSize : height + 1;

				for (int ix = ib; ix < ie; ix++) {
					for (int jx = jb; jx < je; jx++) {

						const double left   = currentValues[ix * (height2) + jx - 1];
						const double center = currentValues[ix * (height2) + jx];
						const double right  = currentValues[ix * (height2) + jx + 1];
						const double top    = currentValues[(ix - 1) * (height2) + jx];
						const double bottom = currentValues[(ix + 1) * (height2) + jx];

						nextEntries[ix * (height2) + jx] =
							center + c* ( 0.25 * (left + right + bottom + top) -center);
						if (n + 1 == nSteps) {
							sum += nextEntries[ix * (height2) + jx];
						}
					}
				}

			}
		}

		double* swp = nextEntries;
		nextEntries = currentValues;
		currentValues = swp;

		//Send messages to update
		if (nThreads > 1) {
			// messaging from rank 0 to rank n
			if (mpi_rank == 0) {
				// send the curren rank last line to the right rank
					double* sendind = currentValues + (loclen) * (height2);
					MPI_Send(sendind, height2, MPI_DOUBLE, 1, 0, mpiworld);
				// receive right rank top line to the current rank bottom - 1
					double* recvind = currentValues + (loclen + 1) * (height2);
					MPI_Recv(recvind, height2, MPI_DOUBLE, 1, 0, mpiworld, &recvsta);
			} else if (mpi_rank == lastNodeRank) {
				// receive left rank last line to the current rank top - 1
					double* recvind = currentValues;
					MPI_Recv(recvind, height2, MPI_DOUBLE, mpi_rank - 1, 0, mpiworld, &recvsta);
				// send the curren rank top line to the left rank
					double* sendind = currentValues + (height2);
					MPI_Send(sendind, height2, MPI_DOUBLE, mpi_rank - 1, 0, mpiworld);
			} else {
				// receive left rank last line to the current rank top - 1
					double* recvindl2r = currentValues;
					MPI_Recv(recvindl2r, height2, MPI_DOUBLE, mpi_rank - 1, 0, mpiworld, &recvsta);
				// send the curren rank last line to the right rank
					double* sendindl2r = currentValues + loclen * (height2);
					MPI_Send(sendindl2r, height2, MPI_DOUBLE, mpi_rank + 1, 0, mpiworld);
				// send the curren rank top line to the right rank
					double* sendindr2l = currentValues + (height2);
					MPI_Send(sendindr2l, height2, MPI_DOUBLE, mpi_rank - 1, 0, mpiworld);
				// receive right rank top line to the current rank bottom - 1
					double* recvindr2l = currentValues + (loclen + 1) * (height2);
					MPI_Recv(recvindr2l, height2, MPI_DOUBLE, mpi_rank + 1, 0, mpiworld, &recvsta);
			}
		}

	}//nSteps for-loop

	double reducedSum = 0.0;
	MPI_Allreduce(&sum, &reducedSum, 1, MPI_DOUBLE, MPI_SUM, mpiworld);
	double avg = reducedSum / (double)(width * height);

	double absoluteSum = 0.0;
	for (ib = 1; ib < loclen + 1; ib += batchSize) {
		ie = ib + batchSize < loclen + 1 ? ib + batchSize : loclen + 1;
		for (jb = 1; jb < height + 1; jb += batchSize) {
			je = jb + batchSize < height + 1 ? jb + batchSize : height + 1;
			for (int ix = ib; ix < ie; ++ix) {
				for (int jx = jb; jx < je; ++jx) {
					const double contribution = getAbs(currentValues[ix * (height2) + jx] - avg);
					absoluteSum += contribution;
				}
			}
		}
	}

	double reducedAbsoluteSum = 0.0;
	MPI_Reduce(&absoluteSum, &reducedAbsoluteSum, 1, MPI_DOUBLE, MPI_SUM, 0, mpiworld);
	double absavg = reducedAbsoluteSum / (double)(width * height);

	if (mpi_rank == 0) {
		free(currentValuesFull);
	}
	free(nextEntries);
	free(currentValues);

	if (mpi_rank == 0) {
		printf("average: %e\n", avg);
		printf("average absolute difference: %e\n", absavg);
	}

	MPI_Finalize();
	return 0;
}


void parseArgs(int argc, char* argv[]){

	char* arg1 = NULL;
	char* arg2 = NULL;
	if (argc == 3) {
		for (size_t ix = 1; ix < argc; ++ix) {
			arg1 = strchr(argv[ix], 'n');
			arg2 = strchr(argv[ix], 'd');
			if (arg1) {
				nSteps = strtol(arg1+1, NULL, 10);
			} else if (arg2) {
				c = atof(arg2+1);
			}
		}
	} else {
		printf("Invalid number of arguments. Correct syntax is: heat_diffusion "
				"-n#numberOfTimeSteps4 -d#diffusionConstant\n");
		exit(1);
	}
}


