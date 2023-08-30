#include <stdlib.h>
#include <stdio.h>

int main () {

	FILE *file;
	file = fopen("/home/alex/Desktop/HPC/assignment-0/matrix","rb");
	int size= sizeof(*file);
	int* readData = (int*)malloc(sizeof(int)*100);
	
	
	if(file == NULL) {
		printf("error opening file \n");
		return 1;	
	}
	
	fread(readData, sizeof(int), 100, file);

    //Parse contents of file:
	

	
	for(int i = 0; i < 100; i++){
		printf("%d\n",readData[i]);
	}
	fclose(file);
	
	return 0;
}
