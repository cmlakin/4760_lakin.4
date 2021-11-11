#pragma once

typedef struct ostime {
    int seconds;
    int nanoseconds;
} ostime;

typedef struct osclock_t {
    ostime  time;
    void (*set)(int seconds, int nanoseconds);
    void (*add)(int seconds, int nanoseconds);
    void (*get)(ostime *);
    int  (*seconds)(void);
    int  (*nanoseconds)(void);

} osclock_t;

extern osclock_t osclock;
