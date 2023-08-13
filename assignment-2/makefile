CC = gcc
SRC = cell_distances.c
OUTPUT = main
OPT = -Ofast
LIBS = -lm
OBJ = cell_distances.o

cell_distances: $(OBJ)
	$(CC) -o $(OUTPUT) $(OBJ) $(OPT) $(LIBS)
	
clean:
	rm -f $(OBJ)

