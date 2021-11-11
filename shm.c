#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>

#include "shm.h"
#include "config.h"

static char xperror_buf[50]; // buffer for perror
static const char * xperror_arg0 = "oss"; // pointer to return error value

static int id = -1;


shared_data *  shmAttach();
void shmDetach();

// 	((struct shared_data *) shm_data)->ptab.pcb[pcbIndex].id = pcbIndex * 1000
//			+ shm_data->local_pid++;
// shared_data * shmGetData();
// proc_table * shmGetProcTable();

int shmGet() {
    int flags = 0;

    // set flags for shared memory creation
    flags = (0666 | IPC_CREAT);

    //snprintf(FTOK_BASE, PATH_MAX, "/tmp/oss.%u", getuid());

    // make a key for our shared memory
    key_t fkey = ftok(FTOK_BASE, FTOK_SHM);

    if (fkey == -1) {

        snprintf(xperror_buf, sizeof(xperror_buf), "%s: ftok: ", xperror_arg0);
        perror(xperror_buf);
        return -1;
    }

    // get a shared memory region
    id = shmget(fkey, sizeof(struct shared_data), flags);

    // if shmget failed
    if (id == -1) {

        snprintf(xperror_buf, sizeof(xperror_buf), "%s: shmget: ", xperror_arg0);
        perror(xperror_buf);
        exit(0);
        return -1;
    }

    return id;
}

shared_data * shmAttach() {
    shared_data * data;

    if(id == -1 && shmGet() == -1) {
        return NULL;
    }
    // attach the region to process memory
    data = (struct shared_data*)shmat(id, NULL, 0);

    // if attach failed
    if (data == (void*)-1) {

        snprintf(xperror_buf, sizeof(xperror_buf), "%s: shmat: ", xperror_arg0);
        perror(xperror_buf);
        return NULL;
    }
    return data;
}

void shmDetach() {

}

// 	((struct shared_data *) shm_data)->ptab.pcb[pcbIndex].id = pcbIndex * 1000
//			+ shm_data->local_pid++;
// shared_data * getSharedData() {
//
// }
// proc_table * getProcTable();
// PCB * getPCB(id);
//
// static int shm_id = -1;
