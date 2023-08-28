#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char** argv) {
	//Parse args ./newton -t5 -l1000 7 in any arbitrary order
	if (argc != 4) {
	printf("Error, argument number not met! \n Example: ./main <-t5> <-l1000> <7>\n");
	
	return 1;
	}
	//Parse the args: degree will be less than 10
	int degree = (**(argv+3))-'0';
	
	
	printf("%d\n", degree );
	return 0;
}
