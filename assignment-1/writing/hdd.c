#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t size = 1048576;
    size_t *as = (size_t*)malloc(sizeof(size_t)*size);
    FILE *f;

    for (size_t i = 0; i < size; ++i){
        as[i] = i;
    }

    f = fopen("textfile.txt","w");
    fwrite(as, sizeof(size_t), size, f); //Write the file
    fflush(f);
    fclose(f);

    f = fopen("textfile.txt","r");
    fread(as, sizeof(size_t), size, f); //Read the file
    fclose(f);

    free(as);

    return 0;
}
