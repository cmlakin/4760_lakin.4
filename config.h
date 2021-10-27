#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


#define PROCESSES 18
#define LOG_FILENAME "oss.log"

// shared memory 
struct shmbuf {
	
	// os simulated clock
	int ossec = 0;	// initial value for clock seconds
	int osnano = 0; // initial value for clock nanoseconds

	// process table
	struct proc_table ptab[18];
}

struct proc_table {

	struct proc_ctrl_blck pcb[18];
}

struct proc_ctrl_blck {

	int id = NULL; // pid of uproc
	int ptype; // 0 - CPU, 1 - I/O
	int operation = 0;
	int startsec = 0;
	int startnano = 0;
	int runsec = 0;
	int runnano = 0;
	int donesec = 0;
	int donenano = 0;
	int pqueue; // hold value of priority queue assigned
}


// time given to uproc from os for run time alloted
extern int assignedsec;
extern int assignednano;


/* TODO not sure what to put in here yet. 
 * Do I need a separte queue for each priority/blocked? */

struct queue {
	

}

// TODO need to check if queue is full/empty
// TODO have some time that will allow blocked queue to be
// 		reprioritized. 

// TODO msgbuffer oss to uprocess
// 			- id
// 			- assigned sec
// 			- assigned nano


// TODO msgbuffer  uprocess to oss
// 			- id
// 			- type (CPU/IO)
// 			- runsec
// 			- runnano




