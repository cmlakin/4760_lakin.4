#include "config.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <sys/fcntl.h>
#include <stdbool.h>

// shared memory
// 		/* own simulated time maintained by os - otime: only oss changes clock */
// 		clock: int seconds, int nano
//
// 		/* priority queues */
// 		queue1 - high priority
// 		queue2 - low priority
// 		queue3 - blocked
//
// 		/* process table - ptable: only 18 entries */
//
// 		/* process control block - pcb: max of 18 */
//		keep track of statistics:
//				- pid
//				- type of process (will determine priority)
//				- time started
//				- how long it ran for
//				- time finished
//				- what queue it was put into

// TODO randomly launch new process

// TODO send msg to uprocess and let it know how much time it has to run

#define WORDSIZE 32
#define BITS_WS 5
#define MASK 0x1f

const unsigned long maxTimeBetweenNewProcsNS = 1000000000;
const int maxTimeBetweenNewProcsSecs = 2;

char perror_buf[50]; // buffer for perror
const char * perror_arg0 = "oss"; // pointer to return error value

static int shm_id = -1; // shared memory identifier
//static struct shared_data * shm_data = NULL; // shared memory pointer
//static char FTOK_BASE[PATH_MAX];

static int msg_id = -1;

int g_bitVector = 0;

void processCommandLine(int, char **);
void initialize();
void initializeSharedMemory();
void initializeMessageQueue();
pid_t createProcess();
void testSync();
void createMessageQueue();
unsigned long launchTime();
void ossClock();
void addToQueue();
void evaluateQueue();
void scheduleProc();
void updateClock(int, int);
void initProcRunTime();
void initializeProcTable();
void logStatistics(const char *);
void deinitSharedMemory();
//int initBit(int, int);
void setBit(int);
bool bitIsSet(int);
void clearBit(int);
void runTime();

int main(int argc, char ** argv){

	int b = 17;

	setBit(b);
	printf("word is %04x is set %d\n", g_bitVector, b);
	bitIsSet(b);


	processCommandLine(argc, argv);
	initialize();
	initProcRunTime();

	//shm_data->local_pid = 0;
	int i = 0;
	while (i++ < 5) {

		unsigned long temp = 0;
		unsigned long l_sec = 0;
		unsigned long l_nano = 0;

		temp = launchTime();
		//printf("temp: %ld\n", temp);
		if (temp >= 1000000000) {
			l_sec = temp / 1000000000;
			l_nano = temp - (l_sec * 1000000000);
		}
		else {
			l_nano = temp;
		}
		//printf("***sec: %ld, nano: %ld\n\n", l_sec, l_nano);

		if (l_sec == 0) {
			sleep(2);
		}
		else {
			sleep(l_sec);
			sleep(2);
			//printf("***sleeping for %ld seconds and %ld nanoseconds\n\n", l_sec, l_nano);
		}
		createProcess();
		sleep(1);
		testSync();

		clearBit(b);
		printf("word is %04x is set %d\n", g_bitVector, bitIsSet(b));
	}

	deinitSharedMemory();
	printf("oss done\n");
	exit(-1);

}

void initialize() {
	initializeSharedMemory();
	initializeMessageQueue();
}

//
// 0 0000 0000
// 1 1011 0110
void initializeSharedMemory() {
	int flags = 0;

	// set flags for shared memory creation
	flags = (0666 | IPC_CREAT);

	//snprintf(FTOK_BASE, PATH_MAX, "/tmp/oss.%u", getuid());

	// make a key for our shared memory
	key_t fkey = ftok(FTOK_BASE, FTOK_SHM);

	if (fkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	// get a shared memory region
	shm_id = shmget(fkey, sizeof(struct shared_data), flags);

	// if shmget failed
	if (shm_id == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmget: ", perror_arg0);
		perror(perror_buf);
		exit(0);
		//return -1;
	}

	// attach the region to process memory
	shm_data = (struct shared_data*)shmat(shm_id, NULL, 0);

	// if attach failed
	if (shm_data == (void*)-1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmat: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	ossClock();
}

void initializeMessageQueue() {
	// TODO clear all of shared data

	// messages
	key_t msgkey = ftok(FTOK_BASE, FTOK_MSG);

	if (msgkey == -1) {
		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	msg_id = msgget(msgkey, 0666 | IPC_CREAT);
	if (msg_id == -1) {
		printf("Error creating queue\n");
	}

}

int findAvailablePcb(void) {
	int i;
	for(i = 0; i < PROCESSES; i++) {

		if(!bitIsSet(i)) {
			return i;
		}
	}
	return -1;
}

pid_t createProcess(){

	// find available pcb and initialize first values
	int pcbIndex = findAvailablePcb();
	if(pcbIndex == -1) {
		printf("createProcess: no free pcbs\n");
		return -1;
	}
	printf("createProcess: available pcb %d\n", pcbIndex);
	setBit(pcbIndex);
	//return -1;

	((struct shared_data *) shm_data)->ptab.pcb[pcbIndex].id = pcbIndex * 1000
			+ shm_data->local_pid++;

	shm_data->ptab.pcb[pcbIndex].ptype = shm_data->type;
	shm_data->ptab.pcb[pcbIndex].operation = shm_data->op;

	runTime();

	pid_t pid;
	pid = fork();

	if (pid == -1) {
		perror("Failed to create new process\n");
		//break;
	}
	else if (pid == 0) {
		//printf("UPROC Created: %d\n", getpid());
		shm_data->local_pid += 1;
		//printf("local_pid %d\n", shm_data->local_pid);
		execl("uprocess", "uprocess", NULL);
		exit(-1);
	}
	else{
	}

	shm_data->ptab.pcb[pcbIndex].runsec = shm_data->osRunSec;
	shm_data->ptab.pcb[pcbIndex].runnano = shm_data->osRunNano;

	char logbuf[200];
	//printf("pid: %i\n", getpid());
	snprintf(logbuf, sizeof(logbuf), "Generating process with PID %i\n", pid);
	logStatistics(logbuf);
}

void testSync() {
	struct msgbuf sndmsg;

	sndmsg.mtype = MSG_SEND_UPROC;
	strcpy(sndmsg.mtext, "foo\n");
	if (msgsnd(msg_id, (void *)&sndmsg, MAX_TEXT, 0) == -1) {
	}

	//printf("oss message sent\n");
	sleep(1);
	msgrcv(msg_id, (void *)&sndmsg, MAX_TEXT, MSG_RECV_OSS, 0);
	//printf("oss msg received: %s\n", sndmsg.mtext);
}

void processCommandLine(int argc, char ** argv) {
	int option; // parse command line arguements
	char * filename; // filename pointer

	while((option = getopt(argc, argv, "hs:l:")) != -1) {
		switch(option) {
		case 'h':
			fprintf(stderr, "usage: %s -h\n", argv[0]);
			exit(-1);
		case 's':
			fprintf(stderr, "usage: %s -s\n", argv[0]);
			break;
		case 'l':
			fprintf(stderr, "usage: %s -l\n", argv[0]);
			break;
		default:
			fprintf(stderr, "errno: %i\n", errno);
		}

		if(optind < argc) {
			while(optind < argc) {
				filename = argv[optind];
				optind++;
			}
		} else {
			filename = "oss.log";
		}
	}
}

unsigned long launchTime() {

	srand(time(NULL));
	unsigned long launch_nano = rand() % maxTimeBetweenNewProcsNS + 1;
	unsigned long launch_sec = rand() % maxTimeBetweenNewProcsSecs + 1;

	unsigned long launchTime = launch_nano + (launch_sec * 1000000000);

	return launchTime;
}

void runTime() {

	srand(time(NULL));
	shm_data->osRunNano = rand() % maxTimeBetweenNewProcsNS + 1;
	shm_data->osRunSec = rand() % maxTimeBetweenNewProcsSecs + 1;

}

void ossClock() {

	// set up initial clock values operated by oss
	shm_data->ossec = 0;
	shm_data->osnano = 0;

}

void addToQueue() {

	// determine which queue to add process to
}

void evaluateQueue() {

	// determine which to schedule next
	//		- how long in system
	//		- how long in blocked queue
	//		- which are CPU and I/O
}

void scheduleProc() {

	// schedule a process to run

}

void updateClock(int sec, int nano) {

	// update oss clock to reflect new time
	shm_data->ossec += sec;

	if (nano > 1000000000) {

		int temp = launchTime();
		shm_data->ossec += temp / 1000000000;
		shm_data->osnano = temp - (sec * 1000000000);
	}
	else {

		shm_data->osnano += nano;
	}
}


void initProcRunTime() {

	srand(time(NULL));

	shm_data->osRunSec = rand() % 5 + 1;
	shm_data->osRunNano = rand() % 1000000000 + 1;

}

void initializeProcTable() {

}


void logStatistics(const char * string_buf) {

	int fid;

	fid = open(LOG_FILENAME, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

	if (fid == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: open: ", perror_arg0);
		perror(perror_buf);
	}
	else {
		//printf("%s", string_buf);
		write(fid, (void *) string_buf, strlen(string_buf));
		close(fid);
	}


}

void deinitSharedMemory() {
	shmctl(shm_id, IPC_RMID, NULL);

	if (shmdt(shm_data) == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmdt: ", perror_arg0);
		perror(perror_buf);
	}

}


/*int initBit(int g_bitVector, int val) {

	g_bitVector = calloc(val/WORDSIZE +1, sizeof(int));

	return g_bitVector != 0; // will return 1

}
*/

void setBit(int b) {
	g_bitVector |= (1 << b);
}

// test if int i is set in bv
bool bitIsSet(int b) {
	return (g_bitVector & (1 << b)) != 0;
}

void clearBit(int b) {

	g_bitVector &= ~(1 << b);
}

/*
void pcbInit(int loc_p, struct proc_ctrl_blck * init, int pid) {

	init->id = pid;
	init->loc_id = loc_p;
	init->ptype = 0;
	init->operation = 0;
	init->startsec = 0;
	init->startnano = 0;
	init->runsec = 0;
	init->runnano = 0;
	init->totalsec = 0;
	init->totalnano = 0;
	init->pqueue = 0;

} */
