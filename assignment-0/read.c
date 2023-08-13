#include <stdlib.h>
#include <stdio.h>

int main () {

	FILE *file;
	file = fopen("/home/alex/Desktop/HPC/assignment-0/matrix.txt","r");
	int size= sizeof(*file);

	if(file == NULL) {
		printf("error opening file \n");
		return 1;	
	}
	


    //Parse contents of file:
	
	int number;
	int row = 0;
	int col = 0;
	
	while (fscanf(file, "%d", &number) == 1){
		if (number == col*row){
			printf("true\n");
		}
		col++;
		if (col % 10 == 0) {
			row++;
			col = 0;
		}	
	}
	fclose(file);
	
	return 0;
}
