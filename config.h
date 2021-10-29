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

struct proc_ctrl_blck {

	int id; // pid of uproc
	int ptype; // 0 - CPU, 1 - I/O
	int operation;
	int startsec;
	int startnano;
	int runsec;
	int runnano;
	int donesec;
	int donenano;
	int pqueue; // hold value of priority queue assigned
};

struct proc_table {

	struct proc_ctrl_blck pcb[18];
};

// shared memory 
struct shared_data {
	
	// os simulated clock
	int ossec;	// initial value for clock seconds
	int osnano; // initial value for clock nanoseconds

	// process table
	struct proc_table ptab;
};

// time given to uproc from os for run time alloted
extern int assignedsec;
extern int assignednano;

//extern const int QUEUE_ID = 1;
//extern const int SHM_ID = 2;

#define MAX_TEXT 50

// message buffer
struct msgbuf {
	long mtype;
	char mtext[MAX_TEXT];
};


/* TODO not sure what to put in here yet. 
 * Do I need a separte queue for each priority/blocked? */

struct queue {
	

};

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




