enum queue_priority {
    HighPriority,
    LowPriority
};

void queueCreate(void);
PCB * queueShift(int which);
void queuePush(int which, PCB * pcb);
void queueDump(int which);
