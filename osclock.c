#include <stdlib.h>
#include <stdio.h>
#include "osclock.h"

void set(int seconds, int nanoseconds) {
    // printf("s %d n %d\n", osclock.time.seconds, osclock.time.nanoseconds);
    //
    // printf("set %d %d\n", seconds, nanoseconds);
    osclock.seconds = seconds;
    osclock.nanoseconds = nanoseconds;
    // printf("s %d n %d\n", osclock.time.seconds, osclock.time.nanoseconds);
}


void add(int seconds, int nanoseconds) {
    osclock.nanoseconds += nanoseconds;

    if(osclock.nanoseconds > 999999999) {
        osclock.seconds += osclock.nanoseconds / 1000000000;
        osclock.nanoseconds = osclock.nanoseconds % 1000000000;
    }
    osclock.seconds += seconds;
}

osclock_t osclock = {
    .set = set,
    .add = add,
    .seconds = 0,
    .nanoseconds = 0
};
