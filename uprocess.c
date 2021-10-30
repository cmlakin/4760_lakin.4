#include "config.h"


char perror_buf[50];
const char * perror_arg0 = "uprocess";
//static char *msg1_keyname = "oss";

// TODO wait for message from oss



// TODO determine % of how many can be I/O bound and CPU processes



// TODO pick random number to determine if I/O bound (1) or CPU (0)



// TODO randomly pick operation choice:	
// 		0 - use all time allowed (higher chance for CPU)
// 		1 - terminate (chance of this option should be low)
// 		2 - use part of time (higher chance for I/O bound)
// 		



// TODO send msg back to oss:
// 		- what type of process it is
// 		- operation number it chose
// 		- how much time it ran for (add this to the otime clock)
//			- if terminate: send msg to oss then terminate itself
//
struct uproc_msgbuf{

	long int mtype;
	char mtext[BUFSIZ];
};

int main (int argc, char ** argv){

	printf("In uprocess\n");

	//int running = 1;
	int msgid;
	struct msgbuf sndmsg;
	long int msg_rec = 0;

	key_t sndkey = ftok(FTOK_BASE, FTOK_MSG);

	if (sndkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
	}

	msgid=msgget(sndkey, 0666 | IPC_CREAT);

	msgrcv(msgid, (void *)&sndmsg, BUFSIZ, msg_rec, 0);
	printf("msg received: %s\n", sndmsg.mtext);
	

	sndmsg.mtype = MSG_SEND_OSS;
	strcpy(sndmsg.mtext, "bar\n");
	if (msgsnd(msgid, (void *)&sndmsg, MAX_TEXT, 0) == -1) {

		printf("Message not sent\n");
	}

	printf("Uproc message sent\n");
	printf("Uproc done\n");

	while (1);
	//msgctl(msgid, IPC_RMID, 0);
	exit(0);;
}
