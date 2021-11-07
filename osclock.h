typedef struct osclock_t {
    void (*set)(int seconds, int nanoseconds);
    void (*add)(int seconds, int nanoseconds);
    int seconds;
    int nanoseconds;
} osclock_t;

extern osclock_t osclock;
