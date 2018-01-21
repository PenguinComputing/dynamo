# Without MPI
#CC = gcc
#CFLAGS = -O3 -fopenmp

# With MPI
CC = mpicc
CFLAGS = -O3 -fopenmp -DMPI

LDFLAGS = -lm -fopenmp

dynamo: main.o global.o mysecond.o streamtask.o worker.o control.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	/bin/rm dynamo main.o global.o mysecond.o streamtask.o worker.o control.o
