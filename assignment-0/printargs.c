#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	//args will be aA and bB
	
	char* arg1 = argv[1];
	char* arg2 = argv[2];
	
	char char1 = arg1[1];
	char char2 = arg2[1];
	
	printf("%c",arg1);
	printf("%c",arg2);
	if (char1 == 'a' && char2 == 'b')  {
		printf("A is %c and B is %c \n",arg1[2],arg2[2]);
		
	} else if (char1 == 'b' && char2 == 'a') {
	
		printf("A is %c and B is %c \n",arg2[2],arg1[2]);
	}else {
		printf("error retrieving arguments.\n");
	}
	
	return 0;
}

//note A[i] = *(A+i)
