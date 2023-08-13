#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>




int main (int argc, char** argv) {
	//parse arguments
	int threads = *(*(argv + 1)+2);
	//open file:
	
	char cwd[PATH_MAX];
	char filename[] = "/cell";
	getcwd(cwd,sizeof(cwd));
	strcat(cwd,filename);

	FILE *file = fopen(cwd,"r");
	if (file == NULL){
		printf("Error: failed to open file \n");
		return 1;
	}
	//Parse contents of file:
	
	
	
	//Close file
	fclose(file);
	return 0;
}
