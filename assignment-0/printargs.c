#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	//args will be aA and bB
	
	char* arg1 = argv[1];
	char* arg2 = argv[2];
	
	if (*arg1 == 'a' && *arg2 == 'b')  {
		printf("A is %c and B is %c \n",*(arg1+1),*(arg2+1));
	} else if (*arg1 == 'b' && *arg2 == 'a') {
		printf("A is %c and B is %c \n",*(arg2+1),*(arg1+1));
	}else {
		printf("error retrieving arguments.\n");
	}
	
	return 0;
}

//note A[i] = *(A+i)
