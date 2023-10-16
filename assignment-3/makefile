CC = gcc
OPT = -O3 -lpthread -march=native
LIBS = -lm

OUTPUT = newton 


$(OUTPUT): newton.c
	$(CC) $< -o $@ $(OPT) $(LIBS)
	

.PHONY: clean
clean:
	rm -f $(OUTPUT)

