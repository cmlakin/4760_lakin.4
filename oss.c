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

#define MAX_TEXT 50

char perror_buf[50]; // buffer for perror
const char * perror_arg0 = "oss"; // pointer to return error value

static int shm_id = -1; // shared memory identifier
//static char *shm_keyname;  // shared memory key path
static struct shared_data * shm_data = NULL; // shared memory pointer

static int msg_id = -1;

void processCommandLine(int, char **);
void initialize();
void initializeSharedMemory();
void initializeMessageQueue();
int deinit_shared_data();
void createProcess();
void testSync();

int main(int argc, char ** argv){

	
	processCommandLine(argc, argv);
	initialize();
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
		execl("uprocess", "uprocess", NULL); 
	}
	else{
		
		printf("Program finished");
		//while(1); // jsut using to stall for right now. 
	}

	//deinit_shared_data();
	//shmdt(shdata);
	//shmctl(shmid, IPC_RMID, NULL);

	printf("oss done\n");

	exit(-1);;
}




void processCommandLine(int argc, char ** argv) {

	int option; // parse command line arguements
	//char * filename; // filename pointer

	while((option = getopt(argc, argv, "hs:l:")) != -1){
		
		switch(option){
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
				
	}

/*	if(optind < argc){
		
		while(optind < argc){
			filename = argv[optind];
			optind++;
		}
	}
	else{
		filename = "oss.log";
	}*/

}

void initialize() {
	initializeSharedMemory();
	initializeMessageQueue();
}


void initializeSharedMemory(){

	int flags = 0;
	
	// set flags for shared memory creation
	flags = (0700 | IPC_CREAT);

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

	shm_data->ossec = 0;
	shm_data->osnano = 0;
}

void initializeMessageQueue() {
	
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


int deinit_shared_data() {

	//detach shared pointer
	if (shmdt(shm_data) == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmdt: ", perror_arg0);
		perror(perror_buf);
		return -1;
	}

	// remove the region from system
	shmctl(shm_id, IPC_RMID, NULL);

	return 0;
}






void createProcess(){

}

void testSync() {


}












