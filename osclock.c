#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "osclock.h"
#include "shm.h"

static shared_data * data;

void set(int seconds, int nanoseconds) {
    data->osclock.time.seconds = seconds;
    data->osclock.time.nanoseconds = nanoseconds;
}


void add(int seconds, int nanoseconds) {
    data->osclock.time.nanoseconds += nanoseconds;

    if(data->osclock.time.nanoseconds > 999999999) {
        data->osclock.time.seconds += data->osclock.time.nanoseconds / 1000000000;
        data->osclock.time.nanoseconds = data->osclock.time.nanoseconds % 1000000000;
    }
    data->osclock.time.seconds += seconds;
}

void get(ostime *time) {
    time->seconds = data->osclock.time.seconds;
    time->nanoseconds = data->osclock.time.nanoseconds;
}

int seconds(void) {
    return data->osclock.time.seconds;
}
int nanoseconds(void) {
    return data->osclock.time.nanoseconds;
}

void init() {
    data = shmAttach();

    if(data == NULL) {
        printf("could not get shared data\n");
    } else {
        printf("clock got shared data\n");
    }
    osclock.set = set;
    osclock.add = add;
    osclock.get = get;
    osclock.seconds = seconds;
    osclock.nanoseconds = nanoseconds;
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
