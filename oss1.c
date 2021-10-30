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

static int shmid = -1; // shared memory identifier
static char *shm_keyname;  // shared memory key path
static struct shared_data * shdata = NULL; // shared memory pointer

static char *msg1_keyname = "oss";


void processCommandLine(int, char **);
void init_shared_data();
void createProcess();
void testSync();

int main(int argc, char ** argv){

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
		execl("uprocess", "uprocess", NULL); 
	}
	else{
		
		while(1); // jsut using to stall for right now. 
	}

	return 0;
}




void processCommandLine(int argc, char ** argv) {

	int option; // parse command line arguements
	char * filename; // filename pointer

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




void init_shared_data(){

	int flags = 0;
	// create shared memory keyname
	//snprintf(shm_keyname, PATH_MAX, "/tmp/oss.%u", getuid());
	shm_keyname = "./oss.c";
	
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

	key_t sndkey = ftok(msg1_keyname, 5);

	if (sndkey == -1) {
		
		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	key_t rcvkey = ftok(shm_keyname, 2);
		
	if (rcvkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
		//return -1;
	}

	int running = 1;
	int msgid;
	struct msgbuf sndmsg;
	long int msg_rec = 0;
	
	msgid = msgget(sndkey, 0666 | IPC_CREAT);
	if (msgid == -1) {
		printf("Error creating queue\n");
	}

	while(running) {
		
		sndmsg.mtype = 1;
		
		strcpy(sndmsg.mtext, "foo\n");
		if (msgsnd(msgid, (void *)&sndmsg, MAX_TEXT, 0) == -1) {

			printf("Message not sent\n");
		}
	
		running = 0;
	}

	msgrcv(msgid, (void *)&sndmsg, BUFSIZ, msg_rec, 0);
	printf("msg received: %s\n", sndmsg.mtext);
}

void createProcess(){

}

void testSync() {


}












