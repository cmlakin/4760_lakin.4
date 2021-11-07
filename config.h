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


#define PT_IO_BOUND  0x0001
#define PT_CPU_BOUND 0x0002
#define PT_USE_TIME  0x0004
#define PT_BLOCK     0x0008
#define PT_TERMINATE 0x0010

#define PROB_IO			30		// 30% of the time IO bound
#define PROB_CPU		70		// 70% of the time CPU bound
#define PROB_TERMINATE 	5		// 5% of the time terminate
#define PROB_IO_BLOCK	80		// 75% of the time block if IO bound (80 - 5 = 75)
#define PROB_CPU_BLOCK	30		// 25% of the time block if CPU bound

enum queue_priority {
    QT_HIGH_PRIORITY,
    QT_LOW_PRIORITY
};


//extern int runningProc; // 0 is no process running, 1 if process running

static int totalProcesses = 0;
static struct shared_data * shm_data = NULL;

typedef struct proc_ctrl_blck {

	int id; // pid of uproc
	int loc_id;
	int ptype; // 0 - CPU, 1 - I/O
	int operation; // used time, blocked, terminated
	//int startsec;
	//int startnano;
	int runsec;
	int runnano;
	int totalsec;
	int totalnano;
	int pqueue; // hold value of priority queue assigned
	//
	// used only for testing
	//
	int testsec;
	int testnano;
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
	//int op;

	// os simulated clock
	int osSec;	// initial value for clock seconds
	int osNano; // initial value for clock nanoseconds
	int osRunSec;	// allowed runtime sec given to uproc
	int osRunNano; // allowed runtime nano given to uproc
	int launchSec;
	int launchNano;

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
typedef struct ipcmsg {
	long mtype;
	char mtext[MAX_TEXT];

	int ossid;
	int pRunSec;
	int pRunNano;
	int pOperation;

} ipcmsg;
