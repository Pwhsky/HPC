#include <stdlib.h>
#include <stdio.h>

int main() {
	FILE *file;
	file = fopen("/home/alex/Desktop/HPC/assignment-0/matrix","wb");
	int size = 10;
	
	int*   matrixEntries = (int*)malloc(sizeof(int)*size*size);
	int** matrix	     = (int**)malloc(sizeof(int*)*size);
	for (size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size){
		matrix[ix] = matrixEntries + jx;
	}
	
	for (int ix = 0; ix<size; ix++){
		for (int jx = 0; jx<size;jx++){
			matrix[ix][jx] = ix*jx;
		}	
	}
	
	printf("made matrix \n");
 	fwrite(matrixEntries, sizeof(int), 100, file);
	

	fclose(file);
	return 0;
}
