CC = gcc
SRC = newton.c
OUTPUT = main
OPT = -O2
LIBS = -lm

.PHONY: all

cell_distances: $(OBJ)
	$(CC) $(SRC) -o $(OUTPUT) -fopenmp $(OPT) $(LIBS) -march=native
	
test : newton.c makefile
	tar -czvf newton.tar.gz newton.c makefile
	echo "built archive...\n\n"
	./check_submission.py newton.tar.gz
	
clean:
	rm -f $(OBJ) && rm -f $(OUTPUT)

