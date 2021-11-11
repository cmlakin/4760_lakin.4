#include "config.h"
#include "queue.h"
#include "osclock.h"

void testQueues();
void testPriorityQueues();
void testBlockingQueue();
void testClock();

int main(int argc, char **argv) {
    srand(time(NULL));
    createQueues();
    printf("\n=== starting tests ===\n\n");
    testClock();
    testPriorityQueues();
    testBlockingQueue();
    printf("\n=== done ===\n");


	//setBit(b);
	////printf("word is %04x is set %d\n", g_bitVector, b);
	//bitIsSet(b);
}

void testClock() {
    printf("=== test clock ===\n");

    osclock.set(1, 2);
    printf("s %d n %d\n", osclock.seconds(), osclock.nanoseconds());

    osclock.add(1, 1000000001);
    printf("s %d n %d\n", osclock.seconds(), osclock.nanoseconds());

    printf("done\n");
}

void testPriorityQueues() {
    printf("=== test priority queue ===\n");

    PCB one;
    PCB two;
    PCB *foo;

    one.pid = 1;
    two.pid = 2;

    queues.highPriority.enqueue(&one);
    // queues.highPriority.enqueue(&two);

    printf("queue has\n");
    queueDump(QT_HIGH_PRIORITY, "  ");

    while((foo = queues.highPriority.dequeue()) != NULL) {
        printf("dequeued %d\n", foo->pid);
    }

    printf("\n");
}

void incrementClock(int * seconds, int * nanoseconds, int amount) {
    if((*nanoseconds += amount) > 999999999) {
        *seconds += *nanoseconds / 1000000000;
        *nanoseconds %= 1000000000;
    }
}
void testBlockingQueue() {
    printf("=== test blocking queue ===\n");

    PCB one;
    PCB *foo = NULL;

    int seconds = rand() % 3;
    int nanoseconds = rand() % 1000000000;

    one.pid = 100;
    //
    // nota bene
    //   enqueue adds some random amounts to the seconds and nanoseconds
    //   specificaly, s.n seconds where s and n are random numbers with range
    //   s <= {0...5} and n <= {0...1000}
    //
    //   dequeue succeeds when the process at the head of the queue
    //   has s.n lower than or equal to values passed to dequeue
    //
    //   only one is dequeued at a time so repeated calls may make sense
    //   in actual use-case
    //
    printf("start time %d.%d\n",seconds, nanoseconds);

    queues.blocking.enqueue(&one, seconds, nanoseconds);

    while((foo = queues.blocking.dequeue(seconds, nanoseconds)) == NULL) {
        incrementClock(&seconds, &nanoseconds, 1000);
    }

    printf("  end time %d.%d\n",seconds, nanoseconds);

    printf("blocking dequeued %d\n", foo->pid);
}
