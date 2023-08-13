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
	

         int value;
    int counter = 0;

    while (fscanf(file, "%d,", &value) != EOF) {
        if (value >= 0 && value < 10) {
            printf("%d ", value);
        } else {
            printf("%d ", value);
        }

        counter++;
        if (counter == 10) {
            printf("\n");
            counter = 0;
        }
    }
    
    
    
    


	
	fclose(file);
	
	return 0;
}
