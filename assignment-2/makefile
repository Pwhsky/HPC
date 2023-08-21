CC = gcc
SRC = cell_distances.c
OUTPUT = main
OPT = -O2
LIBS = -lm

.PHONY: all

cell_distances: $(OBJ)
	$(CC) $(SRC) -o $(OUTPUT) -fopenmp $(OPT) $(LIBS) -march=native
	
test : cell_distances.c makefile
	tar -czvf cell_distances.tar.gz cell_distances.c makefile
	echo "built archive...\n\n"
	./check_submission.py cell_distances.tar.gz
	
clean:
	rm -f $(OBJ) && rm -f $(OUTPUT)

