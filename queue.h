#pragma once
#include "shm.h"

//
// priority queues items
//
typedef struct priorityItem {
    struct priorityItem * next;
    PCB * pcb;
} priorityItem;
//
// priority queue
//
typedef struct priorityQueue {
    void (*enqueue)(PCB *);
    PCB *(*dequeue)(void);
    struct priorityItem * head;
    struct priorityItem * tail;
} priorityQueue;

//
// dequeue time, used by clocking queue
//
typedef struct dequeueTime {
    int sec;
    int nano;
} dequeueTime;
//
// blocking queue item
//
typedef struct blockingItem {
    struct blockingItem * next;
    dequeueTime dqtime;
    PCB * pcb;
} blockingItem;

typedef struct blockingQueue {
    blockingItem *head;
    void (*enqueue)(PCB *, int, int);
    PCB *(*dequeue)(int, int);
} blockingQueue;

typedef struct all_queues {
    priorityQueue lowPriority;
    priorityQueue highPriority;
    blockingQueue blocking;
} all_queues;


extern all_queues queues;


void createQueues(void);
void queueDump(int which, char * indent);
