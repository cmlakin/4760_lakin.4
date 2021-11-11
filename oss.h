#pragma once
#include "shm.h"

const unsigned long maxTimeBetweenNewProcsNS = 100;
const int maxTimeBetweenNewProcsSecs = 1;

char perror_buf[50]; // buffer for perror
const char * perror_arg0 = "oss"; // pointer to return error value

static int shm_id = -1; // shared memory identifier

//static int totProcsCreated = 0;
static int allocatedProcs = 0;
static int activeProcs = 0;
static int msg_id = -1;
int g_bitVector = 0;
char logbuf[200];
pid_t pid;

void processCommandLine(int, char **);
void initialize();
void initializeSharedMemory();
void initializeMessageQueue();
struct proc_ctrl_blck  * createProcess();
void dispatch();
void createMessageQueue();
void launchNewProc();
void ossClock();
void addToQueue(struct proc_ctrl_blck  *);
void dispatchUpdateClock();
void updateClock(int, int);
void logStatistics(const char *);
void deinitSharedMemory();
void setBit(int);
bool bitIsSet(int);
void clearBit(int);
void timeSlice();
void bail();
void sigHandler(const int);
int initializeSig();
void scheduler();
int findAvailablePcb(void);
