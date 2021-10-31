#include "config.h"


char perror_buf[50];
const char * perror_arg0 = "uprocess";
//static char * FTOK_BASE[PATH_MAX];


static int msg_id;
static struct msgbuf sndmsg;

struct uproc_msgbuf{

	long int mtype;
	char mtext[BUFSIZ];
};

void uprocInitialize();
void uprocFinished();
void pickType();
void operationChoice();

int main (int argc, char ** argv){

	printf("In uprocess\n");


  uprocInitialize();
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

// TODO determine % of how many can be I/O bound and CPU processes
// TODO pick random number to determine if I/O bound (1) or CPU (0)
}

void operationChoice() {

// TODO randomly pick operation choice:
// 		0 - use all time allowed (higher chance for CPU)
// 		1 - terminate (chance of this option should be low)
// 		2 - use part of time (higher chance for I/O bound)

}









