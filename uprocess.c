#include "config.h"


char perror_buf[50];
const char * perror_arg0 = "uprocess";
//static char * FTOK_BASE[PATH_MAX];

static int shm_id;
static int msg_id;
static struct ipcmsg sndmsg;
static int foo;
static int canUseNano;
static int ptype;

void uprocInitialize();
void uprocFinished(int);
int pickType();
void updateSharedCounters(int);
void doit();

char strbuf[20];


int main (int argc, char ** argv){
	int id = atoi(argv[1]);

	foo = id;
	sprintf(strbuf, "back from id=%i\n", id);

	srand(getpid());

	// printf("uproc: id=%d\n", id);


 	uprocInitialize();
	doit((long)id);
	//ptype = pickType();
	updateSharedCounters(ptype);
	uprocFinished(ptype);
}

void doit(long id) {
	while(1) {
		ipcmsg msg;
		msgrcv(msg_id, (void *)&msg, sizeof(ipcmsg), id, 0);
		canUseNano = msg.pRunNano;

		pickType();
		msg.pRunNano = canUseNano;
		//printf("doit: canUseNano: %i\n", canUseNano);
		msg.mtype = msg.ossid;
		//printf("uproc:  msg received\n");
		//printf("uproc: ossid %d\n", msg.ossid);
		//printf("uproc: mtext %s\n", msg.mtext);
		//printf("uproc: sending to %d\n", msg.ossid);
		strcpy(msg.mtext, strbuf);
		// snprintf(&msg.mtext[0],sizeof(msg.mtext), "from %ld",  id);
		if (msgsnd(msg_id, (void *)&msg, sizeof(msg), 0) == -1) {
			printf("oss msg not sent");
		} else {
			//printf("uproc: message sent\n");
		}
		id = foo;
	}
}

void uprocInitialize(){
	key_t sndkey = ftok(FTOK_BASE, FTOK_MSG);

	if (sndkey == -1) {

		snprintf(perror_buf, sizeof(perror_buf), "%s: ftok: ", perror_arg0);
		perror(perror_buf);
	}

	msg_id=msgget(sndkey, 0666 );
}

void uprocFinished(int pType) {

	sndmsg.pRunNano = canUseNano;
	sndmsg.mtype = MSG_SEND_OSS;
	strcpy(sndmsg.mtext, "bar\n");
	if (msgsnd(msg_id, (void *)&sndmsg, MAX_TEXT, 0) == -1) {
		printf("Message not sent\n");
	}

	//if(pType == PT_TERMINATE) {
	//	kill(getpid(), SIGKILL); // resend to child
	//	printf("uproc chose to terminate");
	//}

	//printf("uproc message sent\n");
	printf("uproc done\n");
	exit(0);
}

int pickType() {
	int type;
	int t = rand() % 100;
	//
	// get run type
	//
	int r = rand() % 100;

	if (t < 30) {
		type = PT_IO_BOUND;
	} else {
		type = PT_CPU_BOUND;
	}
	
	printf("1ptype: canUseNano: %i\n", canUseNano);
	if (r < 10){
		
		type = PT_TERMINATE;
		canUseNano /= 5;
		printf("in terminate\n");
	} else if(r < PROB_CB_IU) {
		if (type == PT_CPU_BOUND) {
			type = PT_BLOCK;
		} else {
			type = PT_USE_TIME;
		}
		canUseNano /= 2;
		printf("in cpu and block\n");
	} else {
		if (type == PT_IO_BOUND) {
			type = PT_BLOCK;
		} else {
			type = PT_USE_TIME;
		}
		printf("in use time\n");
	}
	
	//shm_data->type = type;
	printf("2pickType: canUseNano: %i\n", canUseNano);
	printf("uproc: type %x\n", type);

	updateSharedCounters(type);
	return type;
}

void updateSharedCounters(int ptype) {
	key_t fkey = ftok(FTOK_BASE, FTOK_SHM);

	shm_id = shmget(fkey, sizeof(struct shared_data), 0);

	if(shm_id == -1) {
		snprintf(perror_buf, sizeof(perror_buf), "%s: shmget: ", perror_arg0);
		perror(perror_buf);
		return;
	}

	shm_data = (struct shared_data*)shmat(shm_id, NULL, 0);

	if(ptype & PT_CPU_BOUND) {
		((struct shared_data *) shm_data)->cpuCount += 1;
		//printf("cpuCount: %d\n", shm_data->cpuCount);
	} else {
		shm_data->ioCount += 1;
		//printf("ioCount: %d\n", shm_data->ioCount);
	}
	shm_data->type = ptype;
}
