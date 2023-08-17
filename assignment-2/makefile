CC = gcc
SRC = cell_distances.c
OUTPUT = main
OPT = -O2
LIBS = -lm
OBJ = cell_distances.o

cell_distances: $(OBJ)
	$(CC) -o $(OUTPUT) $(OBJ) $(OPT) $(LIBS) -Wall -march=native -fopenmp
	
clean:
	rm -f $(OBJ) && rm -f $(OUTPUT)

