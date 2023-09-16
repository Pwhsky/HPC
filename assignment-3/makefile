CC = gcc
SRC = newton.c
OUTPUT = main
OPT = -O2
LIBS = -lm

.PHONY: all

cell_distances: $(OBJ)
	$(CC) $(SRC) -o $(OUTPUT) -fopenmp $(OPT) $(LIBS) -march=native
	

clean:
	rm -f $(OBJ) && rm -f $(OUTPUT)

