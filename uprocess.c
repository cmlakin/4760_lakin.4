#include "config.h"


char perror_buf[50];
const char * perror_arg0 = "uprocess";
//static char * FTOK_BASE[PATH_MAX];

static int shm_id;
static int msg_id;
static struct msgbuf sndmsg;



struct uproc_msgbuf{

	long int mtype;
	char mtext[BUFSIZ];
};

void uprocInitialize();
void uprocFinished();
void pickType();
void operationChoice(int);

int main (int argc, char ** argv){

	srand(time(NULL));

	printf("In uprocess\n");


  uprocInitialize();
	pickType();
	uprocFinished();
}


void uprocInitialize(){

// TODO wait for message from oss
	printf("Initializing user process: %d\n", getpid());

	key_t sndkey = ftok(FTOK_BASE, FTOK_MSG);

	if (sndkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
	}

	msg_id=msgget(sndkey, 0666 | IPC_CREAT);

	msgrcv(msg_id, (void *)&sndmsg, BUFSIZ, MSG_RECV_UPROC, 0);
	printf("proc msg received: %s\n", sndmsg.mtext);

}

void uprocFinished() {

// TODO send msg back to oss:
// 		- what type of process it is
// 		- operation number it chose
//		- how much time it will run for
//			- if terminate: send msg to oss then terminate itself

	sndmsg.mtype = MSG_SEND_OSS;
	strcpy(sndmsg.mtext, "bar\n");
	if (msgsnd(msg_id, (void *)&sndmsg, MAX_TEXT, 0) == -1) {
		printf("Message not sent\n");
	}

	printf("uproc message sent\n");
	printf("uproc done\n");
	exit(0);
}

void pickType() {
	int flags = 0;
	key_t fkey = ftok(FTOK_BASE, FTOK_SHM);

	shm_id = shmget(fkey, sizeof(struct shared_data), flags);

	// if shmget failed
	if(shm_id == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: shmget: ", perror_arg0);
		perror(perror_buf);
	}

	// pick type CPU: 0, I/O: 1, terminate: 2
	int typeChoice = rand() % 100;

	shm_data = (struct shared_data*)shmat(shm_id, NULL, 0);

	if(typeChoice <= 41 ){
		((struct shared_data *) shm_data)->cpuCount += 1;
		shm_data->type = 0;
		printf("cpuCount: %d\n", shm_data->cpuCount);
	} else if(typeChoice <= 47) {
		shm_data->type = 2;
	} else {
		shm_data->ioCount += 1;
		shm_data->type = 1;
		printf("ioCount: %d\n", shm_data->ioCount);
	}

	printf("uproc Choice: %d\n", typeChoice);
}
