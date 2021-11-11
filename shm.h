#pragma once

#include "osclock.h"


typedef struct proc_ctrl_blck {
	int pid;       // real pid of uproc
	int local_pid; 		//
	int ptype; //0 - CPU, 1 - I/O
	int operation; 	// used time, blocked, terminated
	int startsec; //clock time when started
	int startnano; // clock time when started
	int endSec;
	int endNano;
	int runnano;
	int totalsec; // total time in system
	int totalnano; // total time in system
	int pqueue; 		// hold value of priority queue assigned
	//
	// used only for testing
	//
	int testsec;
	int testnano;
} PCB;

typedef struct proc_table {
	struct proc_ctrl_blck pcb[17];
} proc_table;

// shared memory
typedef struct shared_data {

	int local_pid; 	// assigned PID
	int ioCount;		// io count
	int cpuCount;		// cpu count
	int ptype;				// cpu/io
	int operation;

	int osRunNano; // allowed runtime nano given to uproc
	int launchSec;
	int launchNano;
	// simulated clock
   osclock_t osclock;

	// process table
	struct proc_table ptab;

	

	// report stats
	int avgWait;
	int avgTimeInSys;
	int avgIOblock;
	int avgCPUblock;
	int avgCPUnotReady;

} shared_data;

int shmGet();
shared_data * shmAttach();
void shmDetach();

// 	((struct shared_data *) shm_data)->ptab.pcb[pcbIndex].id = pcbIndex * 1000
//			+ shm_data->local_pid++;
// shared_data * shmGetData();
// proc_table * shmGetProcTable();
