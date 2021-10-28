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

// TODO random time between launching a new process
// int maxTimeBetweenNewProcsNS;
// int maxTimeBetweenNewProcsSecs;

char perror_buf[50]; // buffer for perror
const char * perror_arg0 = "oss"; // pointer to return error value

static int shmid = -1; // shared memory identifier
static char shm_keyname[PATH_MAX];  // shared memory key path
static struct shared_data * shdata = NULL; // shared memory pointer

void processCommandLine(int, char *);
void init_shared_data();
void createProcess();
void testSync();

int main(int argc, char * argv){

	processCommandLine(argc, argv);
	init_shared_data();
	createProcess();
	testSync();


	pid_t pid; 
	

	pid = fork();

	if (pid == -1){

		perror("Failed to create new process\n");
		//break;
	}
	else if (pid == 0){

		printf("UPROC Created: %d\n", getpid());
	}

}

void processCommandLine(int argc, char * argv) {

	int option; // parse command line arguements
	char * filename; // filename pointer

	while((option = getopt(argc, argv, "hs:l:")) != -1){

		switch(option){
			case 'h':
					printf(stderr, "usage: %s -h\n", argv[0]);
					exit(-1);
			case 's':
					printf(stderr, "usage: %s -s\n", argv[0]);
					break;
			case 'l':
					printf(stderr, "usage: %s -l\n", argv[0]);
					break;
			default:
					fprintf(stderr, "errno: %i\n", errno);
		}

		if(optind < argc){
			while(optind < argc){
				filename = argv[optind];
				optind++;
			}
		}
		else{
			filename = "oss.log";
		}
	}
}

void init_shared_data(){

	int flags = 0;

	// create shared memory keyname
	snprintf(shm_keyname, PATH_MAX, "/tmp/oss.%u", getuid());

	
	// set flags for shared memory creation
	flags = IPC_CREAT | IPC_EXCL | S_IRWXU;

	// make a key for our shared memory
	key_t fkey = ftok(shm_keyname, 9256);
	if (fkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	// get a shared memory region
	shmid = shmget(fkey, sizeof(struct shared_data), flags);

	// if shmget failed
	if (shmid == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmget: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}
	
	
	// attach the region to process memory
	shdata = (struct shared_data*) shmat(shmid, NULL, 0);

	// if attach failed
	if (shdata == (void*)-1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmat: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	// TODO clear all of shared data
	
}

void createProcess(){

}

void testSync() {


}










