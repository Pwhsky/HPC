CC = gcc
SRC = cell_distances.c
OUTPUT = main
OPT = -O2
LIBS = -lm


cell_distances: $(OBJ)
	$(CC) $(SRC) -o $(OUTPUT) -fopenmp $(OPT) $(LIBS) 
	
clean:
	rm -f $(OBJ) && rm -f $(OUTPUT)

