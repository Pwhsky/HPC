#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char** argv) {
	//Ugly as sin, sorry.
	char* arg1   = argv[1];
	char* arg2   = argv[2];
	char   arg3   = argv[3];
	char   char1 = arg1[1];
	char   char2 = arg2[1];
	
	int degree = (**(argv+3))-'0';
	int threads;
	int lines;
	
	//Parse args ./newton -t5 -l1000 7 in any arbitrary order
	if (char1 == 't' && char2 == 'l')  {
		threads = arg1[2]-'0';
		lines = atoi(arg2+(strlen(arg2)- (strlen(arg2)-2)    )  );

	}else if (char1 == 'l' && char2 == 't'){
		threads = arg2[2]-'0';
		lines = atoi(arg1+(strlen(arg1)- (strlen(arg1)-2)    )  );
	}
	
	//Parse the args: degree will be less than 10
	
	
	
	printf("%d\n", lines );
	return 0;
}
