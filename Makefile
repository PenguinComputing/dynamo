# Without MPI
#CC = gcc
#CFLAGS = -O3 -fopenmp

# With MPI
CC = mpicc
CFLAGS = -O3 -fopenmp -DMPI

LDFLAGS = -lm -fopenmp

dynamo: main.o global.o mysecond.o task.o worker.o
	$(CC) -o $@ $^ $(LDFLAGS)
