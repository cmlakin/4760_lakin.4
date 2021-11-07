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

#include "config.h"
#include "queue.h"
#include "osclock.h"
#include "oss.h"

int main(int argc, char ** argv){

	srand(getpid());

	processCommandLine(argc, argv);

	initialize();
	launchNewProc();

	scheduler();

	deinitSharedMemory();
	printf("oss done\n");
	exit(-1);
}

void scheduler() {
	PCB * foo;

	foo = createProcess();

	//while(1) {
		if (totalProcesses < 50) {
			printf("lanuch  %0d.%09d\n", shm_data->launchSec, shm_data->launchNano);
			printf("current %0d.%09d\n", osclock.seconds, osclock.nanoseconds);

			if ((osclock.seconds >= (osclock.seconds + shm_data->launchSec)) &&
				((osclock.nanoseconds >= (osclock.nanoseconds + shm_data->launchNano)))) {
				launchNewProc();
				// try to create new process
				foo = createProcess();
				totalProcesses++;
			}
		}
		// check blocked queue and move to new queue as procs are ready
		while ((foo = queues.blocking.dequeue(osclock.seconds, osclock.nanoseconds)) != NULL) {
			queues.highPriority.enqueue(foo);
			foo->pqueue = 0;
		}
		// check high queue and dispactch first process, until all are finished
		if ((foo = queues.highPriority.dequeue()) != NULL) {
			dispatch(foo);
		}

		// check low queue and dispatch until cycled through all
		if ((foo = queues.lowPriority.dequeue()) != NULL) {
			dispatch(foo);
		}
		osclock.add(0, rand() % 1000000000);
		sleep(1);
	}
//}

void dispatch(PCB *pcb) {
	printf("oss: dispatch\n");

	// create time slice for process
	timeSlice();

	// create msg to send to uproc
	struct ipcmsg send;
	struct ipcmsg recv;

	send.mtype = pcb->loc_id;
	send.pRunSec = shm_data->osRunSec;
	send.pRunNano = shm_data->osRunNano;
	send.ossid = 55;
	strcpy(send.mtext, "foo");

	printf("oss: dispatch local_pid %i from queue %i at time %i.%09i\n", pcb->loc_id, pcb->pqueue,
		osclock.seconds, osclock.nanoseconds);

	snprintf(logbuf, sizeof(logbuf), "OSS: Dispatching process with PID %i from queue %i at time %i:%i\n",
		pcb->loc_id, pcb->pqueue, osclock.seconds, osclock.nanoseconds);
	logStatistics(logbuf);
	//
	// TODO this can fail with errno 22 if receiver not ready!
	//
	sleep(1);
	if (msgsnd(msg_id, (void *)&send, sizeof(send), 0) == -1) {
		printf("oss: msg not sent %d\n", errno);
	} else {
		printf("oss: msg sent\n");
	}

	printf("oss: waiting for msg\n");

	while(msgrcv(msg_id, (void *)&recv, sizeof(recv), 55, 0) == -1) {
		printf("oss: waiting for msg error %d\n", errno);
	}
	printf("oss msg received: %s\n", recv.mtext);

	// update clock with time run
	shm_data->osRunSec = recv.pRunSec;
	shm_data->osRunNano = recv.pRunNano;

	// update pcb values (total time in system)
	pcb->totalsec += shm_data->osRunSec;
	pcb->totalnano += shm_data->osRunNano;

	snprintf(logbuf, sizeof(logbuf),"OSS: Receiving that process with PID %i ran for %i nanoseconds\n",
		shm_data->local_pid, shm_data->osNano);
	logStatistics(logbuf);
	//send info back to sheduler
}

PCB * createProcess() {
	PCB *pcb;

	// find available pcb and initialize first values
	int pcbIndex = findAvailablePcb();
	if(pcbIndex == -1) {
		printf("oss: createProcess: no free pcbs\n");
		return NULL;
	}
	printf("oss: createProcess: available pcb %d\n", pcbIndex);
	setBit(pcbIndex);

	pcb = &shm_data->ptab.pcb[pcbIndex];

	pcb->id = pcbIndex << (8 + shm_data->local_pid++);

	pid = fork();

	if (pid == -1) {
		perror("Failed to create new process\n");
		return NULL;
	} else if (pid == 0) {
		char strbuf[16];

		shm_data->local_pid += 1;
		pcb->loc_id = shm_data->local_pid;
		pcb->id = pid;
		printf("oss: local_pid %d\n", shm_data->local_pid);
		snprintf(strbuf, sizeof(strbuf), "%d", shm_data->local_pid);
		execl("uprocess", "uprocess", strbuf, NULL);
		exit(-1);
	} else {
	}

	pcb->ptype = rand() % 100 < PROB_IO ? PT_IO_BOUND : PT_CPU_BOUND;
	addToQueue(pcbIndex);

	snprintf(logbuf, sizeof(logbuf),
		"OSS: Generating process with PID %i and putting it in queue %i at time %i:%i\n",
		shm_data->local_pid, shm_data->ptab.pcb[pcbIndex].pqueue, osclock.seconds, osclock.nanoseconds);
	logStatistics(logbuf);
	return pcb;
}

void initialize() {
	createQueues();
	initializeSharedMemory();
	initializeMessageQueue();
}

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
	} else {
		memset((void *)shm_data, 0, sizeof(struct shared_data));
	}
	shmctl(shm_id, IPC_RMID, NULL);

	ossClock();
}

void initializeMessageQueue() {
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
	//
	// shortcut since bit vector will often be all 1s
	//
	if((g_bitVector & 0x3ffff) == 0x3ffff) {
		return -1;
	}
	for(i = 0; i < PROCESSES; i++) {
		if(!bitIsSet(i)) {
			return i;
		}
	}
	return -1;
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

void launchNewProc() {
	//set new launch values;
	shm_data->launchSec = rand() % maxTimeBetweenNewProcsSecs + 1;
	shm_data->launchNano = rand() % maxTimeBetweenNewProcsNS + 1;

	// add to ossClock to get new time to run
	shm_data->launchSec += osclock.seconds;
	shm_data->launchNano += osclock.nanoseconds;
}

void timeSlice() {
	int MAXINT = 1000000000;
	//srand(time(NULL));
	shm_data->osRunNano = rand() % 3;
	shm_data->osRunSec = rand() % MAXINT;

}

void ossClock() {
	// set up initial clock values operated by oss
	osclock.set(0, 0);
	printf("oss: clockInit %i:%i\n", osclock.seconds, osclock.nanoseconds);
}

void addToQueue(int pcbIndex) {
	if(shm_data->ptab.pcb[pcbIndex].ptype & PT_BLOCK) {
		queues.blocking.enqueue(&(shm_data->ptab.pcb[pcbIndex]), osclock.seconds, osclock.nanoseconds);
		shm_data->ptab.pcb[pcbIndex].pqueue = 2;
	} else if (shm_data->ptab.pcb[pcbIndex].ptype & PT_IO_BOUND) {
		queues.lowPriority.enqueue(&shm_data->ptab.pcb[pcbIndex]);
		shm_data->ptab.pcb[pcbIndex].pqueue = 1;
	} else {
		queues.highPriority.enqueue(&shm_data->ptab.pcb[pcbIndex]);
		shm_data->ptab.pcb[pcbIndex].pqueue = 0;
	}
}


void dispatchUpdateClock() {
	updateClock(0, rand() % 500000000);
}

void updateClock(int sec, int nano) {

	if (nano > 1000000000) {
		osclock.add(sec + nano / 1000000000, nano % 1000000000);
	} else {
		osclock.add(sec, nano);
	}
}

void logStatistics(const char * string_buf) {
	int fid;
	fid = open(LOG_FILENAME, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

	if (fid == -1) {
		snprintf(perror_buf, sizeof(perror_buf), "%s: open: ", perror_arg0);
		perror(perror_buf);
	} else {
		//printf("%s", string_buf);
		write(fid, (void *) string_buf, strlen(string_buf));
		close(fid);
	}
}

void deinitSharedMemory() {
	if (shmdt(shm_data) == -1) {
		snprintf(perror_buf, sizeof(perror_buf), "%s: shmdt: ", perror_arg0);
		perror(perror_buf);
	}
}

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

void doSigHandler(int sig) {
	if (sig == SIGTERM) {
		// kill child process - reconfig to work with current code
		kill(getpid(), SIGKILL); // resend to child
	}
}

void bail() {
	kill(0, SIGTERM);
	deinitSharedMemory();
	exit(0);
}


void sigHandler(const int sig) {
	sigset_t mask, oldmask;
	sigfillset(&mask);

	// block all signals
	sigprocmask(SIG_SETMASK, &mask, &oldmask);

	if (sig == SIGINT) {
									printf("oss[%d]: Ctrl-C received\n", getpid());
									bail();
	}
	else if (sig == SIGALRM) {
									printf("oss[%d]: Alarm raised\n", getpid());
									bail();
	}
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

int initializeSig() {
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask); // ignore next signals

	if(sigaction(SIGTERM, &sa, NULL) == -1) {
									perror("sigaction");
									return -1;
	}

	// alarm and Ctrl-C(SIGINT) have to be handled
	sa.sa_handler = sigHandler;
	if ((sigaction(SIGALRM, &sa, NULL) == -1) ||
					(sigaction(SIGINT, &sa, NULL) == -1)) {
									perror("sigaction");
									return -1;
	}
	alarm(ALARM_TIME);
	return 0;
}
