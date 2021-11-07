#Makefile/lakin/4760/Project4

GCC= gcc
CFLAGS= -g -Wall

all: oss uprocess test

clean:
	rm *.o oss uprocess test

oss: oss.o scheduler.o queue.o osclock.o
	$(GCC) $(CFLAGS) oss.o scheduler.o queue.o osclock.o -o oss

oss.o: oss.c oss.h config.h osclock.h 
	$(GCC) $(CFLAGS) -c oss.c

scheduler.o: scheduler.c

	$(GCC) $(CFLAGS) -c scheduler.c

uprocess: uprocess.o
	$(GCC)  $(CFLAGS) uprocess.o -o uprocess

uprocess.o: uprocess.c
	$(GCC) $(CFLAGS) -c uprocess.c

queue.o: queue.c queue.h
	$(GCC)  $(CFLAGS) -c queue.c

blocking.o: blocking.c blocking.h
	$(GCC)  $(CFLAGS) -c blocking.c

osclock.o: osclock.c osclock.h
	$(GCC)  $(CFLAGS) -c osclock.c

test.o: test.c config.h
	$(GCC) $(CFLAGS) -c test.c

test: test.o scheduler.o queue.o osclock.o
	$(GCC) $(CFLAGS) test.o scheduler.o queue.o osclock.o -o test
