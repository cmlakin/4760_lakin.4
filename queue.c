#include "config.h"
#include "queue.h"

typedef struct queueItem {
    struct queueItem * next;
    PCB * pcb;
} queueItem;

typedef struct queue {
    struct queueItem * head;
    struct queueItem * tail;
} queue;

queue highPriority = {(queueItem*)0, (queueItem*)0};
queue lowPriority = {(queueItem*)0, (queueItem*)0};

queue * getQueue(int which) {
    queue * q;
    if(which == LowPriority) {
        q = &lowPriority;
    } else {
        q = &highPriority;
    }
    // printf("got head %x tail %lx\n", (int)q->head, (long)q->tail);
    return q;
}

queueItem * newItem(PCB * pcb) {
    queueItem * new = (queueItem *)malloc(sizeof(queueItem));

    new->next = NULL;
    new->pcb = pcb;
    return new;
}

void queueCreate() {
    // highPriority.head = NULL;
    // highPriority.tail = NULL;
    // lowPriority.head = lowPriority.tail = NULL;
    //
    // printf("create head %x tail %x\n", (int)lowPriority.head, (int)lowPriority.tail);
    //
}

PCB * queueShift(int which) {
    queue * q = getQueue(which);
    queueItem * item = q->head;

    if(item == NULL) {
        q->tail = NULL;
        return NULL;
    }
    q->head = q->head->next;

    return item->pcb;
}

void queuePush(int which, PCB * pcb) {
    queue * q = getQueue(which);
    queueItem  * new = newItem(pcb);
    // printf("created head %x tail %lx\n", (int)lowPriority.head, (long)lowPriority.tail);

    // new->pcb = pcb;
    // printf("push head %x tail %lx\n", (int)q->head, (long)q->tail);

    if(q->tail == NULL) {
        printf("in here\n");
        q->tail = new;
        q->head = q->tail;
        printf("pushed 1 head %x tail %x\n", (int)q->head, (int)q->tail);
    } else {
        printf("in here 2\n");
        q->tail->next = new;
        q->tail = new;
        printf("pushed 2 head %x tail %x\n", (int)q->head, (int)q->tail);
    }
    return;
}

void queueDump(int which) {
    queue * q = getQueue(which);

    printf("queue dump head %x tail %x\n", (int)q->head, (int)q->tail);
    queueItem *h = q->head;


    while(h != NULL) {
        printf("queue %d\n", (int)h->pcb->id);
        h = h->next;
    }

}
