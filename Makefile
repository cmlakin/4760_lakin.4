#Makefile/lakin/4760/Project4

GCC= gcc
CFLAGS= -g -Wall

all: oss uprocess

oss: oss.o uprocess.o
	$(GCC) $(CFLAGS) oss.o scheduler.o -o oss

oss.o: oss.c
	$(GCC) $(CFLAGS) -c oss.c

scheduler.o: scheduler.c
	$(GCC) $(CFLAGS) -c scheduler.c

uprocess: uprocess.o
	$(GCC)  $(CFLAGS) uprocess.o scheduler.o -o uprocess

testsims.o: uprocess.c
	$(GCC) $(CFLAGS) -c uprocess.c

clean:
	rm *.o oss uprocess

