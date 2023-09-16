CC = gcc
SRC = cell_distances.c
OUTPUT = cell_distances
OPT = -O2
LIBS = -lm

.PHONY: all

cell_distances: $(OBJ)
	$(CC) $(SRC) -o $@ -fopenmp $(OPT) $(LIBS) -march=native
	
clean:
	rm -f $(OBJ) && rm -f $(OUTPUT)

