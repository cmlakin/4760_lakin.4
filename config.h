#pragma once

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "osclock.h"

#define ALARM_TIME 100

#define FTOK_SHM 1
#define FTOK_MSG 2
#define FTOK_BASE "oss.c"

#define MSG_SEND_UPROC 1
#define MSG_RECV_UPROC MSG_SEND_UPROC
#define MSG_SEND_OSS 2
#define MSG_RECV_OSS MSG_SEND_OSS

#define PROCESSES 18
#define LOG_FILENAME "oss.log"


#define PT_IO_BOUND  0x0001
#define PT_CPU_BOUND 0x0002
#define PT_BLOCK     0x0004
#define PT_USE_TIME  0x0008
#define PT_TERMINATE 0x0010

#define PROB_IO			30		// 30% of the time IO bound
#define PROB_CPU		100		// 70% of the time CPU bound
#define PROB_TERMINATE 	10		// 5% of the time terminate
#define PROB_CB_IU	40		// CPU block or IO use all 35%
#define PROB_IB_CA	100		// IO block or CPU use all 60%


#define NEW_PROC_MAX_SECS 2
#define NEW_PROC_MAX_NANO 1000000000

#define MAX_TEXT 50

enum queue_priority {
    QT_HIGH_PRIORITY = 100,
    QT_LOW_PRIORITY = 101
};


//extern int runningProc; // 0 is no process running, 1 if process running

static int totalProcesses = 0;
static struct shared_data * shm_data = NULL;

// message buffer
typedef struct ipcmsg {
	long mtype;
	char mtext[MAX_TEXT];

	int ossid;
	int pRunSec;
	int pRunNano;
	int pOperation;

} ipcmsg;
