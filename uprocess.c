#include "config.h"


char perror_buf[50];
const char * perror_arg0 = "uprocess";
//static char * FTOK_BASE[PATH_MAX];

static int shm_id;
static int msg_id;
static struct ipcmsg sndmsg;



struct uproc_msgbuf{

	long int mtype;
	char mtext[BUFSIZ];
};

void uprocInitialize();
void uprocFinished();
int pickType();
void updateSharedCounters(int);
void doit();

int main (int argc, char ** argv){
	printf("uproc: %s\n",argv[1]);
	int ptype;
	int id = atoi(argv[1]);

	srand(getpid());

	printf("uproc: id=%d\n", id);


 	uprocInitialize();
	doit(id);
	ptype = pickType();
	updateSharedCounters(ptype);
	uprocFinished();
}

void doit(id) {
	while(1) {
		ipcmsg msg;
		printf("uproc: waiting for message\n");
		msgrcv(msg_id, (void *)&msg, sizeof(msg), id, 0);

		msg.mtype = msg.ossid;
		printf("uproc:  msg received\n");
		printf("uproc: ossid %d\n", msg.ossid);
		printf("uproc: mtext %s\n", msg.mtext);
		printf("uproc: sending to %d\n", msg.ossid);
		strcpy(msg.mtext, "bar");
		if (msgsnd(msg_id, (void *)&msg, sizeof(msg), 0) == -1) {
			printf("oss msg not sent");
		} else {
			printf("uproc: message sent\n");
		}
		sleep(1);
	}
}

void uprocInitialize(){
	printf("uproc: init %d\n", getpid());

	key_t sndkey = ftok(FTOK_BASE, FTOK_MSG);

	if (sndkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
	}

	msg_id=msgget(sndkey, 0666 );
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

	//printf("uproc message sent\n");
	printf("uproc done\n");
	exit(0);
}

int pickType() {
	int type = 0;
	int r = rand() % 100;
	//
	// get run type
	//
	r = rand() % 100;

	if(r < PROB_TERMINATE) {
		type |= PT_TERMINATE;
	} else if(type & PT_IO_BOUND && r < PROB_IO_BLOCK) {
		type |= PT_BLOCK;
	} else if(type & PT_CPU_BOUND && r < PROB_CPU_BLOCK) {
		type |= PT_BLOCK;
	} else {
		type |= PT_USE_TIME;
	}

	printf("uproc: type %x\n", type);
	return type;
}

void updateSharedCounters(int ptype) {
	key_t fkey = ftok(FTOK_BASE, FTOK_SHM);

	shm_id = shmget(fkey, sizeof(struct shared_data), 0);

	if(shm_id == -1) {
		snprintf(perror_buf, sizeof(perror_buf), "%s: shmget: ", perror_arg0);
		perror(perror_buf);
	}

	shm_data = (struct shared_data*)shmat(shm_id, NULL, 0);

	if(ptype & PT_CPU_BOUND) {
		((struct shared_data *) shm_data)->cpuCount += 1;
		printf("cpuCount: %d\n", shm_data->cpuCount);
	} else {
		shm_data->ioCount += 1;
		printf("ioCount: %d\n", shm_data->ioCount);
	}
	shm_data->type = ptype;
}
