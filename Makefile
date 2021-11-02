#Makefile/lakin/4760/Project4

GCC= gcc
CFLAGS= -g -Wall

all: oss uprocess

clean:
	rm *.o oss uprocess

oss: oss.o scheduler.o queue.o
	$(GCC) $(CFLAGS) oss.o scheduler.o queue.o -o oss

oss.o: oss.c config.h
	$(GCC) $(CFLAGS) -c oss.c

scheduler.o: scheduler.c

	$(GCC) $(CFLAGS) -c scheduler.c

uprocess: uprocess.o
	$(GCC)  $(CFLAGS) uprocess.o -o uprocess

uprocess.o: uprocess.c
	$(GCC) $(CFLAGS) -c uprocess.c

queue.o: queue.c queue.h
	$(GCC)  $(CFLAGS) -c queue.c
