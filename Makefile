CC = gcc
CFLAGS = -O3 -fopenmp
LDFLAGS = -lm -fopenmp

dynamo: main.o global.o mysecond.o task.o worker.o
	gcc -o $@ $^ $(LDFLAGS)
