#include <stdlib.h>
#include <stdio.h>

int main() {
	FILE *file;
	file = fopen("/home/alex/Desktop/HPC/assignment-0/matrix.txt","w");
	int size = 10;
	
	for (int ix = 0; ix<size; ix++){
		for (int jx = 0; jx<size;jx++){
			int entry = ix*jx;
			fprintf(file, "%d " ,entry);
		
		}	
		fprintf(file, "\n");
	}
	
	fclose(file);
	return 0;
}
