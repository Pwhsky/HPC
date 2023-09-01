#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char** argv) {
	if (argc != 4) {
		printf("Error, not enough arguments.\n");
		return 1;
	}
	
	char* arg1   = argv[1];
	char* arg2   = argv[2];
	char   arg3   = argv[3];
	char   arg1Char = arg1[1];
	char   arg2Char = arg2[1];

	int threads,lines;
	
	//Parse args ./newton -t5 -l1000 7 in any arbitrary order
	if (arg1Char == 't' && arg2Char == 'l')  {
		threads = arg1[2]-'0';
		lines = atoi(arg2+(strlen(arg2)- (strlen(arg2)-2)    )  );

	}else if (arg1Char == 'l' && arg2Char == 't'){
		threads = arg2[2]-'0';
		lines = atoi(arg1+(strlen(arg1)- (strlen(arg1)-2)    )  );
	}else{
		printf("Error retrieving arguments \n");
		return 1;
	}
	int degree = (**(argv+3))-'0';
	
	//Parse the args: degree will be less than 10
	
	
	
	printf("%d\n", lines );
	return 0;
}
