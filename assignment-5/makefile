CC = gcc
BINS = diffusion
LIBRARIES = -L/usr/lib64/openmpi/lib -lm -lmpi -Wl,-rpath,/usr/lib64/openmpi/lib -Wl,--enable-new-dtags

CFLAGS = -O3 -ffast-math -march=native -lpthread -I. -I/usr/include/openmpi-x86_64 $(LIBRARIES)
.PHONY : all

all : $(BINS)

diffusion : diffusion.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BINS)

