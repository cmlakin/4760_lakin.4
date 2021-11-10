#include <stdlib.h>
#include <stdio.h>
#include "osclock.h"

void set(int seconds, int nanoseconds) {
    osclock.time.seconds = seconds;
    osclock.time.nanoseconds = nanoseconds;
}


void add(int seconds, int nanoseconds) {
    osclock.time.nanoseconds += nanoseconds;

    if(osclock.time.nanoseconds > 999999999) {
        osclock.time.seconds += osclock.time.nanoseconds / 1000000000;
        osclock.time.nanoseconds = osclock.time.nanoseconds % 1000000000;
    }
    osclock.time.seconds += seconds;
}

void get(ostime *time) {
    time->seconds = osclock.time.seconds;
    time->nanoseconds = osclock.time.nanoseconds;
}

int seconds(void) {
    return osclock.time.seconds;
}
int nanoseconds(void) {
    return osclock.time.nanoseconds;
}

void init() {
    osclock.set = set;
    osclock.add = add;
    osclock.get = get;
    osclock.seconds = seconds;
    osclock.nanoseconds = nanoseconds;
}

void foo() {
    printf("foo\n");
}

int bar() {
    printf("bar\n");
    return 0;
}

void initSet(int seconds, int nanoseconds) {
    init();
    return set(seconds, nanoseconds);
}


void initAdd(int seconds, int nanoseconds) {
    init();
    return add(seconds, nanoseconds);
}

void initGet(ostime * time) {
    init();
    get(time);
}

int initSeconds(void) {
    init();
    return seconds();
}

int initNanoseconds(void) {
    init();
    return nanoseconds();
}


osclock_t osclock = {
    .time = {0, 0},
    .set = initSet,
    .add = initAdd,
    .get = initGet,
    .seconds = initSeconds,
    .nanoseconds = initNanoseconds
};
