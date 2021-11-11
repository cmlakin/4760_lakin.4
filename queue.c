#include <stdbool.h>

#include "config.h"
#include "queue.h"


all_queues queues;

PCB * queueShift(int which);
void queuePush(int which, PCB * pcb);


// priorityQueue highPriority = {(priorityItem*)0, (priorityItem*)0};
// priorityQueue lowPriority = {(priorityItem*)0, (priorityItem*)0};

priorityQueue * getQueue(int which) {
    priorityQueue * q;
    if(which == QT_LOW_PRIORITY) {
        q = &queues.lowPriority;
    } else {
        q = &queues.highPriority;
    }
    // printf("got head %x tail %lx\n", (int)q->head, (long)q->tail);
    return q;
}

priorityItem * newItem(PCB * pcb) {
    priorityItem * new = (priorityItem *)malloc(sizeof(priorityItem));

    new->next = NULL;
    new->pcb = pcb;
    return new;
}

void enqueueLow(PCB * pcb) {
    queuePush(QT_LOW_PRIORITY, pcb);
}

PCB *dequeueLow() {
    return queueShift(QT_LOW_PRIORITY);
}

void enqueueHigh(PCB * pcb) {
    queuePush(QT_HIGH_PRIORITY, pcb);
}

PCB *dequeueHigh() {
    return queueShift(QT_HIGH_PRIORITY);
}


void enqueueBlocking(PCB * pcb, int ossec, int ossnano);
PCB * dequeueBlocking(int ossec, int ossnano);

void createQueues() {
    queues.lowPriority.enqueue = enqueueLow;
    queues.lowPriority.dequeue = dequeueLow;
    queues.highPriority.enqueue = enqueueHigh;
    queues.highPriority.dequeue = dequeueHigh;
    queues.blocking.head = NULL;
    queues.blocking.enqueue = enqueueBlocking;
    queues.blocking.dequeue = dequeueBlocking;
}

PCB * queueShift(int which) {
    priorityQueue * q = getQueue(which);
    priorityItem * item = q->head;

    if(item == NULL) {
        q->tail = NULL;
        return NULL;
    }
    q->head = q->head->next;
    if(q->head == NULL) {
        q->tail = NULL;
    }
    return item->pcb;
}

void queuePush(int which, PCB * pcb) {
    priorityQueue * q = getQueue(which);
    priorityItem  * new = newItem(pcb);

    new->pcb = pcb;
    // printf("push head %x tail %lx\n", (int)q->head, (long)q->tail);

    if(q->tail == NULL) {
        q->tail = new;
        q->head = q->tail;
    } else {
        q->tail->next = new;
        q->tail = new;
    }
    return;
}

void queueDump(int which, char * indent) {
    priorityQueue * q = getQueue(which);

    priorityItem *h = q->head;


    while(h != NULL) {
        h = h->next;
    }

}


void enqueueBlocking(PCB * pcb, int ossec, int ossnano) {
    // generate r.s seconds where r and s are random numbers with range
    // r <= {0...5} and s <= {0...1000}
    blockingItem * current = queues.blocking.head ;
    blockingItem * previous = queues.blocking.head ;
    blockingItem * new = (blockingItem *)malloc(sizeof(blockingItem));

    new->next = NULL;
    new->dqtime.sec = ossec + (rand() % 6);
    new->dqtime.nano = ossnano + (rand() % 1001 * 1000 * 1000);
    new->pcb = pcb;
    //
    // save these in pcb for testing
    //
    pcb->testsec = new->dqtime.sec;
    pcb->testnano = new->dqtime.nano;

    if(queues.blocking.head  == NULL) {
        queues.blocking.head  = new;
        return;
    }

    while(current != NULL && new->dqtime.sec >= current->dqtime.sec && new->dqtime.sec >= current->dqtime.nano) {
        previous = current;
        current = current->next;
    }

    if(current == NULL) {
        previous->next = new;
    } else if(current == queues.blocking.head ){
        new->next = current;
        queues.blocking.head  = new;
    } else {
        new->next = current;
        previous->next = new;
    }
}

PCB * dequeueBlocking(int ossec, int ossnano) {
    if(queues.blocking.head == NULL) {
        return NULL;
    }
    PCB * dequeued = NULL;
    int qsec = queues.blocking.head->dqtime.sec;
    int qnano = queues.blocking.head->dqtime.nano;


    if(qsec > ossec) {
        return NULL;
    }

    if(qsec == ossec && qnano > ossnano) {
        return NULL;
    }
    //
    // here
    //  if qsec < ossec we don't care about nanoseconds so we're going to dequeue
    //  otherwise qsec == ossec and qnano <= osssnano so we're going to dequeue
    //
    dequeued = queues.blocking.head->pcb;
    queues.blocking.head = queues.blocking.head->next;
    //
    // TODO free malloced memory
    //

    return dequeued;
}
