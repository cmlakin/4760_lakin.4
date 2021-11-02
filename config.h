#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ALARM_TIME 100

#define FTOK_SHM 1
#define FTOK_MSG 2
#define FTOK_BASE "oss.c"

#define MSG_SEND_UPROC 1
#define MSG_RECV_UPROC MSG_SEND_UPROC
#define MSG_SEND_OSS 2
#define MSG_RECV_OSS MSG_SEND_OSS

#define PROCESSES 18
#define LOG_FILENAME "oss.log"


extern int running; // 0 is no process running, 1 if process running

static struct shared_data * shm_data = NULL;

typedef struct proc_ctrl_blck {

	int id; // pid of uproc
	int loc_id;
	int ptype; // 0 - CPU, 1 - I/O
	int operation;
	int startsec;
	int startnano;
	int runsec;
	int runnano;
	int donesec;
	int donenano;
	int pqueue; // hold value of priority queue assigned
} PCB;

struct proc_table {

	struct proc_ctrl_blck pcb[17];
};

// shared memory
struct shared_data {

	int local_pid;
	int ioCount;
	int cpuCount;
	int type;
	int op;

	// os simulated clock
	int ossec;	// initial value for clock seconds
	int osnano; // initial value for clock nanoseconds
	int osRunSec;	// initial allowed runtime sec given to uproc
	int osRunNano; // initial allowed runtime nano given to uproc

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
