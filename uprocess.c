#include "config.h"
#include "shm.h"


char perror_buf[50];
const char * perror_arg0 = "uprocess";
//static char * FTOK_BASE[PATH_MAX];

static int shm_id;
static int msg_id;
static int foo;
static int canUseNano;
static int ptype;
static int operation;

void uprocInitialize();
int opType();
void updateSharedCounters(int);
void doit();
int getProcessType(int id);
void attachSharedMemory();

char strbuf[20];


int main (int argc, char ** argv){
	int ptype;
	int id = atoi(argv[1]);

	foo = id;

	srand(getpid());


	uprocInitialize();
	attachSharedMemory();
	ptype = getProcessType(id);
	// printf( "uproc: type=%i\n", ptype);
	updateSharedCounters(ptype);
	// printf("uproc id %d ptype %d\n", id, ptype);
	doit((long)id);
	//uprocFinished(ptype);
}

void doit(long id) {
	printf("in doit\n");
	while(1) {
		ipcmsg msg;
		printf("msg_id %i uproc\n", msg_id);
		if(msgrcv(msg_id, (void *)&msg, sizeof(ipcmsg), id + 1, 0) == -1) {
			printf("error receving message\n");
			exit(-1);
		}
		operation = opType(ptype);

		canUseNano = msg.pRunNano;
		msg.pRunNano = canUseNano;
		msg.mtype = msg.ossid;

		msg.pOperation = operation;
		// strcpy(msg.mtext, strbuf);
		// snprintf(&msg.mtext[0],sizeof(msg.mtext), "from %ld",  id);
		if (msgsnd(msg_id, (void *)&msg, sizeof(msg), 0) == -1) {
			printf("oss msg not sent");
		}
		id = foo;

		if(operation == PT_TERMINATE) {
			printf("uproc terminated\n");
			kill(getpid(), SIGKILL);
		}
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

int opType(int ptype) {
	int optype;
	//
	// get run type
	//
	int r = rand() % 100;

	if (r < 10){

		optype = PT_TERMINATE;
		canUseNano /= 5;
	} else if(r < PROB_CB_IU) {
		if (ptype == PT_CPU_BOUND) {
			optype = PT_BLOCK;
		} else {
			optype = PT_USE_TIME;
		}
		canUseNano /= 2;
	} else {
		if (ptype == PT_IO_BOUND) {
			optype = PT_BLOCK;
		} else {
			optype = PT_USE_TIME;
		}
	}

	shm_data->operation = optype;

	updateSharedCounters(ptype);
	return optype;
}

void attachSharedMemory() {
	key_t fkey = ftok(FTOK_BASE, FTOK_SHM);

	shm_id = shmget(fkey, sizeof(struct shared_data), 0666 | IPC_CREAT);

	if(shm_id == -1) {
		snprintf(perror_buf, sizeof(perror_buf), "%s: shmget: ", perror_arg0);
		perror(perror_buf);
		return;
	}

	shm_data = (struct shared_data*)shmat(shm_id, NULL, 0);
}
void updateSharedCounters(int ptype) {
	if(ptype & PT_CPU_BOUND) {
		((struct shared_data *) shm_data)->cpuCount += 1;
	} else {
		shm_data->ioCount += 1;
	}
	shm_data->ptype = ptype;
}

int getProcessType(int id) {
	return shm_data->ptab.pcb[id].ptype;
}
